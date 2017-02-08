#ifndef HISI3518A_ZHAOHE_H
#define HISI3518A_ZHAOHE_H

#include <asm/sched_clock.h>

#include <asm/system.h>
#include <asm/irq.h>
#include <linux/leds.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/vic.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
#include <asm/mach/map.h>

#include <mach/time.h>
#include <mach/hardware.h>
#include <mach/early-debug.h>
#include <mach/irqs.h>
#include <mach/io.h>


struct HISI_GPIO
{
	volatile int DATA;
	volatile int GPIO_DIR;
	volatile int GPIO_IS;
	volatile int GPIO_IBE;
	volatile int GPIO_IEV;
	volatile int GPIO_IE;
	volatile int GPIO_RIS;
	volatile int GPIO_MIS;
	volatile int GPIO_IC;
};

#define  HI3518_GPIO11_PHYS (0x201F0000 + 0x3FC)
#define  HI3518_GPIO10_PHYS (0x201E0000 + 0x3FC)
#define  HI3518_GPIO9_PHYS   (0x201D0000 + 0x3FC)
#define  HI3518_GPIO8_PHYS   (0x201C0000 + 0x3FC)
#define  HI3518_GPIO7_PHYS   (0x201B0000 + 0x3FC)
#define  HI3518_GPIO6_PHYS   (0x201A0000 + 0x3FC)
#define  HI3518_GPIO5_PHYS   (0x20190000 + 0x3FC)
#define  HI3518_GPIO4_PHYS   (0x20180000 + 0x3FC)
#define  HI3518_GPIO3_PHYS   (0x20170000 + 0x3FC)
#define  HI3518_GPIO2_PHYS   (0x20160000 + 0x3FC)
#define  HI3518_GPIO1_PHYS   (0x20150000 + 0x3FC)
#define  HI3518_GPIO0_PHYS   (0x20140000 + 0x3FC)

#define  HI3518_GPIO11_VIRT	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO11_PHYS))
#define  HI3518_GPIO10_VIRT	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO10_PHYS))
#define  HI3518_GPIO9_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO9_PHYS))
#define  HI3518_GPIO8_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO8_PHYS))
#define  HI3518_GPIO7_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO7_PHYS))
#define  HI3518_GPIO6_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO6_PHYS))
#define  HI3518_GPIO5_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO5_PHYS))
#define  HI3518_GPIO4_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO4_PHYS))
#define  HI3518_GPIO3_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO3_PHYS))
#define  HI3518_GPIO2_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO2_PHYS))
#define  HI3518_GPIO1_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO1_PHYS))
#define  HI3518_GPIO0_VIRT 	( (struct HISI_GPIO *)IO_ADDRESS(HI3518_GPIO0_PHYS))


#define HIGPIO11 HI3518_GPIO11_VIRT
#define HIGPIO10 HI3518_GPIO10_VIRT
#define HIGPIO9 HI3518_GPIO9_VIRT
#define HIGPIO8 HI3518_GPIO8_VIRT
#define HIGPIO7 HI3518_GPIO7_VIRT
#define HIGPIO6 HI3518_GPIO6_VIRT
#define HIGPIO5 HI3518_GPIO5_VIRT
#define HIGPIO4 HI3518_GPIO4_VIRT
#define HIGPIO3 HI3518_GPIO3_VIRT
#define HIGPIO2 HI3518_GPIO2_VIRT
#define HIGPIO1 HI3518_GPIO1_VIRT
#define HIGPIO0 HI3518_GPIO0_VIRT

#define MASK_0 (0x01<<0)
#define MASK_1 (0x01<<1)
#define MASK_2 (0x01<<2)
#define MASK_3 (0x01<<3)
#define MASK_4 (0x01<<4)
#define MASK_5 (0x01<<5)
#define MASK_6 (0x01<<6)
#define MASK_7 (0x01<<7)




#define GPIO2_IRQ (HI3518_IRQ_START+29)
#define GPIO0_IRQ (HI3518_IRQ_START+29)
#define GPIO5_IRQ (HI3518_IRQ_START+30)




static inline int GetHi3518GpioData(struct HISI_GPIO * gpio)
{
	return gpio->DATA;
}

static inline int GetHi3518GpioDataM(struct HISI_GPIO * gpio,int mask)
{
	return !!(GetHi3518GpioData(gpio) & mask) ;
}
static inline int GetHi3518GpioIrqMask(struct HISI_GPIO * gpio,int mask)
{
	// return 1已发生中断 0没有发生中断
	return (!!(gpio->GPIO_RIS & mask));
}
static inline void  Hi3518ClearIrq(struct HISI_GPIO * gpio,int mask)
{
	//GPIO 中断清除寄存器,bit[7:0]分别对应 GPIO_DATA[7:0],各比特可独立控制。写1清除标志位
	gpio->GPIO_IC |= mask;

}

/*init irp and set it to input  */
static inline void InitGpioHi3518(struct HISI_GPIO * gpio,int mask)
{
	// 0:输入; 1:输出。
	gpio->GPIO_DIR &= ~mask;
	//0边沿触发1电平触发
	gpio->GPIO_IS &= ~mask;
	//0:下降沿或低电平触发中断; 1:上升沿或高电平触发中断
	gpio->GPIO_IEV |= mask;
	//0单边沿触发1双边沿触发
	gpio->GPIO_IBE |= mask ;
	//GPIO 中断清除寄存器,bit[7:0]分别对应 GPIO_DATA[7:0],各比特可独立控制。写1清除标志位
	gpio->GPIO_IC |= 0xff;
	//GPIO 中断屏蔽寄存器,bit[7:0]分别对应 GPIO_DATA[7:0],各比特独立控制。
	gpio->GPIO_IE |= mask;
}
#endif



