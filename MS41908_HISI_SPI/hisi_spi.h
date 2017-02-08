/***********************************************************************************************
*  Filename      : hisi_spi.h
*  Time          :
*  Revision      :
*  NAME          : ZhaoHe/Cat_With_Apple
*  ID            :
*  School        : GuiZhou_university
*  Email         : 1013909206@qq.com
*  Writer:Cat_Like_Apples
*
*  Tab_to_space  : NO
*  Description   : FOR HISI SPI_DRIVER
*
************************************************************************************************/

#ifndef HISI_SPI_H
#define HISI_SPI_H

#include "my_debug.h"
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


struct HISI_SPI_REG
{
	volatile int SPICR0;  //控制寄存器0
	volatile int SPICR1;  //控制寄存器1
	volatile int SPIDR;  //数据寄存器
	volatile int SPISR;  //状态寄存器
	volatile int SPICPSR;  //时钟分频寄存器
	volatile int SPIIMSC;  //中断屏蔽寄存器a
	volatile int SPIRIS;  //原始中断状态寄存器
	volatile int SPIMIS;  //屏蔽后中断状态寄存器
	volatile int SPIICR;  //中断清除寄存器
	volatile int SPIDMACR;  //DMA控制寄存器
	volatile int SPITXFIFOCR;  //发送FIFO控制寄存器
	volatile int SPIRXFIFOCR;  //接收FIFO控制寄存器

};


#define HISI_SPI0_REG_VIRT ((struct HISI_SPI_REG *)IO_ADDRESS(0x200C0000))
#define HISI_SPI1_REG_VIRT ((struct HISI_SPI_REG *)IO_ADDRESS(0x200E0000))



/**
 *    这个函数由你根据不同的需求进行相应的配置 这里的配置是对应的查询模式 并将这个函数放在主程序中
 *    @param hisi_spi_reg [description]
 */
#if 0
static inline void hisi_spi_int(struct HISI_SPI_REG *hisi_spi_reg)
{
	//Step1:disable SPI
	hisi_spi_reg->SPICR1 &=  ~MASK_1;

	//Step2:配置位宽等工作方式
	hisi_spi_reg->SPICR0 = (0x00<<8)|(0x01<<7)|(0x01<<6)|(0x00<<4)|(0x07<<0);

	//Step3:配置时钟分频 SPI_CLK = 200MHZ/2/SPICPSR
	hisi_spi_reg->SPICPSR = 50;

	//Step4:屏蔽所有中断
	hisi_spi_reg->SPIIMSC = 0x00;
}
#endif



static inline int is_spi_free(struct HISI_SPI_REG * hisi_spi_reg){
 	return !(hisi_spi_reg->SPISR & MASK_4);
}
static inline int is_spi_tx_fifo_empty(struct HISI_SPI_REG * hisi_spi_reg){
	return !!(hisi_spi_reg->SPISR & MASK_0);
}
static inline int is_spi_rx_fifo_empty(struct HISI_SPI_REG * hisi_spi_reg){
	return !(hisi_spi_reg->SPISR & MASK_2);
}

/**
 *    spi连续传输函数/查询模式
 *    @param  hisi_spi_reg
 *    @param  data
 *    @param  num
 *    @return error return -1 sucess return num
 */
static int hisi_spi_transfer(struct HISI_SPI_REG* hisi_spi_reg,unsigned int *data,int num)
{
	int i = 0;
	int trash;
// debug_func_start();
	if (num>256||data==NULL||hisi_spi_reg==NULL)
		return -1;

	/*
	*步骤0. 清空接受FIFO
	*步骤1. 向寄存器SPICR1[sse]写“1”,使能SPI。
	*步骤2. 将需发送的数据连续写到寄存器SPIDR。
	*步骤3. 轮询寄存器SPISR,直到[BSY]=0(表示总线不忙)、[TFE]=1(表示发送FIFO已 空)、[RNE]=1(表示接收 FIFO 非空),进入步骤 5。
	*步骤4. 读出数据,需保证读空接收FIFO(可通过查询SPISR[RNE]得到)。
	*步骤5. 向寄存器SPICR1[sse]写“0”,禁止SPI。
	*/

	//step 1
	while(!is_spi_rx_fifo_empty(hisi_spi_reg))
		trash = hisi_spi_reg->SPIDR;

	//step2
	hisi_spi_reg->SPICR1 |= MASK_1;

	//step3
	for (i = 0; i < num; ++i)
		hisi_spi_reg->SPIDR = data[i];

	//step4
	// debug_printk("start translate");
	while(!(is_spi_free(hisi_spi_reg) && is_spi_tx_fifo_empty(hisi_spi_reg)))
		;
	// debug_printk("\n");

	//step5
	for (i = 0; i < num; ++i)
		data[i] = hisi_spi_reg->SPIDR;

	hisi_spi_reg->SPICR1 &= ~MASK_1;

// debug_func_end(num);
	return num ;
}







/**
 *    For debug
 */

#if 0

#define DEBUG_PRINK_SPI1_REG \
debug_printk("DEBUG_PRINK_SPI1_REG:\n\
	SPICR0:%x\n \
	SPICR1:%x\n \
	SPIDR:%x\n \
	SPISR:%x\n \
	SPICPSR:%x\n \
	SPIIMSC:%x\n \
	SPIRIS:%x\n \
	SPIMIS:%x\n \
	SPIICR:%x\n \
	SPIDMACR:%x\n \
	SPITXFIFOCR:%x\n \
	SPIRXFIFOCR:%x\n \
******************************************\n", \
	HISI_SPI1_REG_VIRT->SPICR0,HISI_SPI1_REG_VIRT->SPICR1,HISI_SPI1_REG_VIRT->SPIDR,\
	HISI_SPI1_REG_VIRT->SPISR,HISI_SPI1_REG_VIRT->SPICPSR,HISI_SPI1_REG_VIRT->SPIIMSC,\
	HISI_SPI1_REG_VIRT->SPIRIS,HISI_SPI1_REG_VIRT->SPIMIS,HISI_SPI1_REG_VIRT->SPIICR,\
	HISI_SPI1_REG_VIRT->SPIDMACR,HISI_SPI1_REG_VIRT->SPITXFIFOCR,HISI_SPI1_REG_VIRT->SPIRXFIFOCR)


#define DEBUG_PRINK_SPI2_REG \
debug_printk(" \
	SPICR0:%x\n \
	SPICR1:%x\n \
	SPIDR:%x\n \
	SPISR:%x\n \
	SPICPSR:%x\n \
	SPIIMSC:%x\n \
	SPIRIS:%x\n \
	SPIMIS:%x\n \
	SPIICR:%x\n \
	SPIDMACR:%x\n \
	SPITXFIFOCR:%x\n \
	SPIRXFIFOCR:%x\n \
******************************************\n", \
	HISI_SPI0_REG_VIRT->SPICR0,HISI_SPI0_REG_VIRT->SPICR1,HISI_SPI0_REG_VIRT->SPIDR,\
	HISI_SPI0_REG_VIRT->SPISR,HISI_SPI0_REG_VIRT->SPICPSR,HISI_SPI0_REG_VIRT->SPIIMSC,\
	HISI_SPI0_REG_VIRT->SPIRIS,HISI_SPI0_REG_VIRT->SPIMIS,HISI_SPI0_REG_VIRT->SPIICR,\
	HISI_SPI0_REG_VIRT->SPIDMACR,HISI_SPI0_REG_VIRT->SPITXFIFOCR,HISI_SPI0_REG_VIRT->SPIRXFIFOCR)
#endif



#endif