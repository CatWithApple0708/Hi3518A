/***********************************************************************************************
*  Filename      : ms41908_hisi_spi.c
*  Time          :
*  Revision      :
*  NAME          : ZhaoHe/Cat_With_Apple
*  ID            :
*  School        : GuiZhou_university
*  Email         : 1013909206@qq.com
*  Writer:Cat_Like_Apples
*
*  Tab_to_space  : NO
*  Description   : For ms41908 Connet With hisi3518 spi1
*
************************************************************************************************/

#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <mach/irqs.h>
#include <mach/io.h>


#include <asm/sched_clock.h>

#include <asm/system.h>
#include <asm/irq.h>
#include <linux/leds.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/vic.h>
#include <asm/mach-types.h>


#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>

#include <mach/time.h>
#include <mach/hardware.h>
#include <mach/early-debug.h>
#include <mach/irqs.h>
#include <mach/io.h>


#include "my_debug.h"
#include "hisigpio.h"
#include "hisi_spi.h"
#include "hi3518_muxctrl.h"
#include "ms41908_hisi_spi.h"



/**
 *    使用的所有资源 这里没有做资源保护，如果其他模块使用了这里的IO
 *    无法保证其寄存器不被修改
 *    MS41908_MUTEX_SPI1_SCLK   HI3518_SPI1_SCLK
 *    MS41908_MUTEX_SPI1_SDO    HI3518_SPI1_SDO
 *    MS41908_MUTEX_SPI1_SDI      HI3518_SPI1_SDI
 *    MS41908_MUTEX_SPI1_CSN     HI3518_SPI1_CSN
 *    MS41908_MUTEX_VD_FZ	  HI3518_UART1_RXD
 *    MS41908_MUTEX_VD_IS	  HI3518_UART1_TXD
 *    MS41908_MUTEX_RST_N	   HI3518_IR_IN
 *    MS41908_MUTEX_ZOOM	  HI3518_UART2_RXD //Gpio7_6
 *    MS41908_MUTEX_FOCUS	  HI3518_UART2_TXD //Gpio7_7
 */


/**
 *    复用管教配置
 */


#define MS41908_MUTEX_SPI1_SCLK (unsigned int *)HI3518_SPI1_SCLK
#define MS41908_MUTEX_SPI1_SDO  (unsigned int *)HI3518_SPI1_SDO
#define MS41908_MUTEX_SPI1_SDI    (unsigned int *)HI3518_SPI1_SDI
#define MS41908_MUTEX_SPI1_CSN   (unsigned int *)HI3518_SPI1_CSN
#define MS41908_MUTEX_VD_FZ	(unsigned int *)HI3518_UART1_RXD
#define MS41908_MUTEX_VD_IS	(unsigned int *)HI3518_UART1_TXD
#define MS41908_MUTEX_RST_N	 (unsigned int *)HI3518_IR_IN

#define MS41908_MUTEX_ZOOM	(unsigned int *)HI3518_JTAG_TRSTN //Gpio0_0
#define MS41908_MUTEX_FOCUS	(unsigned int *)HI3518_JTAG_TCK //Gpio0_1


#define MS41908_CS()  (HIGPIO5->DATA |= MASK_7)
#define MS41908_DIS_CS() ((HIGPIO5->DATA &= ~MASK_7))

#define MS41908_RST()\
do{\
	HIGPIO7->DATA |=MASK_5;\
	ndelay(100);\
	HIGPIO7->DATA &= ~MASK_5;\
	ndelay(100);\
	HIGPIO7->DATA |=MASK_5;\
}while(0)



static inline void ms41908_mutex_init(void){
	/**
	 *    Init Mutex
	 */
	iowrite32(1,MS41908_MUTEX_SPI1_SCLK);
	iowrite32(1,MS41908_MUTEX_SPI1_SDO);
	iowrite32(1,MS41908_MUTEX_SPI1_SDI);
	iowrite32(0,MS41908_MUTEX_SPI1_CSN); //select by ourself GPIO5_7 高电平有效
	iowrite32(0,MS41908_MUTEX_VD_FZ);//GPIO2_3
	iowrite32(0,MS41908_MUTEX_VD_IS);//Gpio2_5
	iowrite32(1,MS41908_MUTEX_RST_N);//Gpio7_5 1:gpio7_5 0: r_in

	iowrite32(0,MS41908_MUTEX_ZOOM);//Gpio0_0
	iowrite32(0,MS41908_MUTEX_FOCUS);//Gpio0_1

	/**
	 *    init Gpio
	 */
	HIGPIO5->GPIO_DIR |= MASK_7;
	HIGPIO2->GPIO_DIR |= MASK_3;
	HIGPIO2->GPIO_DIR |= MASK_5;
	HIGPIO7->GPIO_DIR |= MASK_5;

	HIGPIO0->GPIO_DIR &= ~MASK_0;
	HIGPIO0->GPIO_DIR &= ~MASK_1;

	HIGPIO5->DATA &= ~MASK_7;
	HIGPIO2->DATA &= ~MASK_3;
	HIGPIO2->DATA &= ~MASK_5;
	HIGPIO7->DATA |= MASK_5;
	/**
	 *    init Ms41908
	 */
	MS41908_RST();

	/**
	 *    init spi reg
	 *    Step1:disable SPI
	*     Step2:配置位宽等工作方式
	*     Step3:配置时钟分频 SPI_CLK = 200MHZ/2/SPICPSR =2MHZ
	*     Step4:屏蔽所有中断
	 */
	HISI_SPI1_REG_VIRT->SPICR1 &=  ~MASK_1;
	HISI_SPI1_REG_VIRT->SPICR1 |=  MASK_4;//Set Big end
	HISI_SPI1_REG_VIRT->SPICR0 = (0x00<<8)|(0x01<<7)|(0x01<<6)|(0x00<<4)|(0x07<<0);
	HISI_SPI1_REG_VIRT->SPICPSR = 50;
	HISI_SPI1_REG_VIRT->SPIIMSC = 0x00;

}

#define SET_MS41908_CMD(command,addr,rw) (command = (addr&0x3f)|((rw?0x1:0x0)<<6)|(0<<7))

static  int ms41908_transfer(struct ms41908_msg * ms41908_msg)
{

	unsigned int data[3]={0};
	debug_func_start();
	if (ms41908_msg==NULL){
		printk("ms41908_transfer para wrong\n");return -1;
	}
	/**
	 *    select ms41908
	 */
	HIGPIO5->DATA |= MASK_7;
	/**
	 *    	Analysis Msg Set CMD
	 */
	SET_MS41908_CMD(data[0],ms41908_msg->addr,ms41908_msg->rw);
	data[1]=ms41908_msg->data&0xff;
	data[2]=(ms41908_msg->data>>8)&0xff;
	debug_printk("Send:command:%x,data[1]:%x,data[2]:%x\n",data[0],data[1],data[2]);

	/**
	 *     Spi Transfer
	 */

	hisi_spi_transfer(HISI_SPI1_REG_VIRT,data,ARRAY_SIZE(data));

	/**
	 *    Set Msg
	 */
	debug_printk("Receive:data[1]:%x,data[2]:%x\n",data[1],data[2]);
	ms41908_msg->data=(data[1])|(data[2]<<8);
	/**
	 *    disable ms41908
	 */
	HIGPIO5->DATA &= ~MASK_7;
	debug_func_end(0);
	return 0;
}


static void ms41908_transfers(struct ms41908_msgs * ms41908_msgs)
{
	int i =0;
	for (i = 0; i < ms41908_msgs->size; ++i)
	{
		ms41908_transfer(&(ms41908_msgs->msg[i]));
	}

}



/**
 *    Kernel Need
 */

static int ms41908_open(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t ms41908_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
/**
 *    Test
 */
#if 0
	struct ms41908_msg msg={0x00,ms41908_R,0x00};
	ms41908_transfer(&msg);
	debug_printk("Reg:%x/Data:%x/",msg.addr,msg.data);
#endif
	return size;
}

static ssize_t ms41908_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
/**
 *    Test
 */
#if 0
	struct ms41908_msg msg={0x00,ms41908_W,0xAA};
	ms41908_transfer(&msg);
	debug_printk("Reg:%x/Data:%x/",msg.addr,msg.data);
#endif

	return size;
}

static long ms41908_ioctl(struct file *file, unsigned int cmd , unsigned long arg)
{
	debug_printk("cmd is %d",cmd);
	switch(cmd){
	case MS41908_RST_CMD:
		debug_printk("MS41908_RST_COMMAND\n");
		MS41908_RST();
		break;
	case MS41908_WR_REG_CMD:
		debug_printk("MS41908_WR_REG_COMMAND\n");
		ms41908_transfers((struct ms41908_msgs *)arg);
		break;
	case MS41908_RD_REG_CMD:
		debug_printk("MS41908_RD_REG_COMMAND\n");
		ms41908_transfers((struct ms41908_msgs *)arg);
		break;
	case MS41908_RD_ZOOM:
		debug_printk("MS41908_RD_ZOOM\n");
		return !!(HIGPIO0->DATA & MASK_0);
		break;
	case MS41908_RD_FOCUS:
		debug_printk("MS41908_RD_FOCUS\n");
		return !!(HIGPIO0->DATA & MASK_1);
		break;
	case MS41908_SET_VD_FZ:
		debug_printk("MS41908_SET_VD_FZ\n");
		HIGPIO2->DATA |=MASK_3;
		break;
	case MS41908_CLEAR_VD_FZ:
		debug_printk("MS41908_SET_VD_FZ\n");
		HIGPIO2->DATA &=~MASK_3;
	break;
	default:debug_printk("IOCTL CMD WRONG");
	}
	return 0;
}



static int ms41908_close(struct inode *inode, struct file *file)
{
    pr_err("%s\n", __func__);
    return 0;
}


static struct file_operations ms41908_fops = {
	.owner    =  THIS_MODULE,
	.open    =  ms41908_open,
	.read    =  ms41908_read,
	.write    =  ms41908_write,
	.unlocked_ioctl = ms41908_ioctl,
	.release    = ms41908_close,
};

static struct miscdevice  ms41908_dev = {
	.minor   = MISC_DYNAMIC_MINOR,
	.name    = "MS41908" ,
	.fops    = &ms41908_fops,
};



static int __init simplie_misc_init(void)
{
    int ret;
    ms41908_mutex_init();
    ret = misc_register(&ms41908_dev);
    if(ret<0)
        pr_err("misc_register wrong!");
    return 0;
}

static void  __exit simplie_misc_exit(void)
{
    misc_deregister(&ms41908_dev);
}
module_init(simplie_misc_init);
module_exit(simplie_misc_exit);

MODULE_AUTHOR("Zhao He <1013909206@qq.com>");
MODULE_DESCRIPTION("Hello Module");
MODULE_LICENSE("GPL");


