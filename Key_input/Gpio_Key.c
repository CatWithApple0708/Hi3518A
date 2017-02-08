//====================================================================================
//  Filename      : Gpio_Key.c/input
//  Time          :
//  Revision      :
//  NAME          : ZhaoHe/Cat_With_Apple
//  ID            :
//  School        : GuiZhou_university
//  Email         : 1013909206@qq.com
//
//  Tab_to_space  : NO
//  Description   : For Hi3518_Gpio_Key_Driver input_2nd
//
//                       		  _______________________
//                       		/ I am a cat,                      |
//                          		| I like eating apples!         |
// | \_____^^^^_____/ |   O |_______________________/
// |                              |  O
// |        __       __        | o
// |                              |                       .:'
// |     @      o       @     |               __    :'__
// |                              |          .'`      `-     ' ``.
//  \__  Ho^^^^oH  __/          :                    .-'
//     T                      T            :                    :
//     |         YY          |             :                    `-;
//     |        .....          |              `. __. -- . .__ ./	 		   Writer:Cat_Like_Apples
//====================================================================================


#include <linux/module.h>
#include <linux/fs.h>
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




#include "hisi3518a_zhaohe.h"

// #define MY_DEBUG

#ifdef MY_DEBUG
#define debug_printk(fmt,...) printk(fmt,##__VA_ARGS__)
#else
#define debug_printk(fmt,...) do{}while(0)
#endif



#define HI3518_MUXCTRL_BASE_PHYS 0x200F0000
#define HI3518_MUXCTRL_REG26_VIRT IO_ADDRESS(HI3518_MUXCTRL_BASE_PHYS+0x068)
#define HI3518_MUXCTRL_REG47_VIRT IO_ADDRESS(HI3518_MUXCTRL_BASE_PHYS+0x0BC)
#define HI3518_MUXCTRL_REG78_VIRT IO_ADDRESS(HI3518_MUXCTRL_BASE_PHYS+0x138)

#define MY_KEY_DOWN 1
#define MY_KEY_UP 0

struct Hi3518GpioKey
{
	char *name;
	unsigned int irq;
	struct HISI_GPIO *gpio;
	int mask;
	int key;
	int last_value;
};


static struct input_dev *buttons_dev;
static struct Hi3518GpioKey pins_desc[] = {
	{"gpio26",GPIO2_IRQ,HIGPIO2,MASK_6,KEY_A, MY_KEY_UP},
	{"gpio52",GPIO5_IRQ,HIGPIO5,MASK_2,KEY_S,  MY_KEY_UP},
	{"gpio06",GPIO0_IRQ,HIGPIO0,MASK_6,KEY_D, MY_KEY_UP},
};





 static inline void hi3518_gpio26_init(void)
 {
 	//set_to_gpio2_6
 	iowrite32(0,HI3518_MUXCTRL_REG26_VIRT);
 	InitGpioHi3518(HIGPIO2,MASK_6);
 }
 static inline void hi3518_gpio52_init(void)
 {
 	//set_to_gpio5_2
 	iowrite32(0,HI3518_MUXCTRL_REG47_VIRT);
 	InitGpioHi3518(HIGPIO5,MASK_2);
 }
 static inline void hi3518_gpio06_init(void)
 {
 	//set_to_gpio0_6
 	iowrite32(0,HI3518_MUXCTRL_REG78_VIRT);
 	InitGpioHi3518(HIGPIO0,MASK_6);
 }



/*****************************************button_timer**********************************/

static void buttons_timer_function(unsigned long data);
static struct timer_list buttons_timer =
	TIMER_INITIALIZER(buttons_timer_function, 0, 0);



static void set_buttons_timer(unsigned long data)
{
	debug_printk("%s\n",__func__);
	buttons_timer.data = data;
	buttons_timer.expires = jiffies + HZ/100;
	del_timer(&buttons_timer);
	add_timer(&buttons_timer);
	return;
}

static void buttons_timer_function(unsigned long data)
{
	struct Hi3518GpioKey *irq_pd=(struct Hi3518GpioKey *)data;
	debug_printk("%skey_name:%s\n",__func__,irq_pd->name);
	if (irq_pd->last_value != !GetHi3518GpioDataM(irq_pd->gpio,irq_pd->mask))
	{
		if (!GetHi3518GpioDataM(irq_pd->gpio,irq_pd->mask)){
			input_event(buttons_dev, EV_KEY, irq_pd->key, 1);
			input_sync(buttons_dev);
			irq_pd->last_value = MY_KEY_DOWN;
		}
		else{
			input_event(buttons_dev, EV_KEY, irq_pd->key, 0);
			input_sync(buttons_dev);
			irq_pd->last_value = MY_KEY_UP;
		}

	}
}




/*****************************************button_timer**********************************/




static irqreturn_t buttons_irq(int irq, void *dev_id)
{

	struct Hi3518GpioKey *irq_pd = (struct Hi3518GpioKey *)dev_id;
	debug_printk("key_name:%s,key value is %d IntMask:%d\n",\
		 irq_pd->name,\
		GetHi3518GpioDataM(irq_pd->gpio,irq_pd->mask),\
		GetHi3518GpioIrqMask(irq_pd->gpio,irq_pd->mask) );

	if(GetHi3518GpioIrqMask(irq_pd->gpio,irq_pd->mask)){
		set_buttons_timer((unsigned long )irq_pd);
	}
	Hi3518ClearIrq(irq_pd->gpio,irq_pd->mask);
	return IRQ_RETVAL(IRQ_HANDLED);
}



/*******************************input*****************************************/
static inline void input_init(void)
{
	/* 1. 分配一个input_dev结构体 */
	buttons_dev = input_allocate_device();;
	buttons_dev->name = "keys";
	/* 2. 设置 */
	/* 2.1 能产生哪类事件 */
	set_bit(EV_KEY, buttons_dev->evbit);
	// set_bit(EV_REP, buttons_dev->evbit);

	/* 2.2 能产生这类操作里的哪些事件: L,S,ENTER,LEFTSHIT */
	set_bit(KEY_A, buttons_dev->keybit);
	set_bit(KEY_S, buttons_dev->keybit);
	set_bit(KEY_D, buttons_dev->keybit);

	/* 3. 注册 */
	input_register_device(buttons_dev);
}


/*******************************input*****************************************/




static int __init hisi_adc_init(void)
{

	int i=0;
	debug_printk("Time:%s,\n",__TIME__);
	hi3518_gpio26_init();
	hi3518_gpio52_init();
	hi3518_gpio06_init();

	input_init();


	for (i = 0;i<ARRAY_SIZE(pins_desc); i++)
	{
		request_irq(pins_desc[i].irq, buttons_irq, IRQF_SHARED,NULL, &pins_desc[i]);
	}

	return 0;
}

static void  __exit hisi_adc_exit(void)
{
	int i=0;
	for (i = 0;i<ARRAY_SIZE(pins_desc); i++)
	{
		free_irq(pins_desc[i].irq, &pins_desc[i]);
	}

	input_unregister_device(buttons_dev);
	input_free_device(buttons_dev);

}
module_init(hisi_adc_init);
module_exit(hisi_adc_exit);

MODULE_AUTHOR("Zhao He <1013909206@qq.com>");
MODULE_DESCRIPTION("Hello Module");
MODULE_LICENSE("GPL");








