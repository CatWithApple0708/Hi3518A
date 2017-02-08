#ifndef MY_DEBUG_H
#define MY_DEBUG_H


// #define MY_DEBUG

#ifdef MY_DEBUG
#define debug_printk(fmt,...) printk(fmt,##__VA_ARGS__)
#else
#define debug_printk(fmt,...) do{}while(0)
#endif

#define debug_func_start(void) \
	debug_printk("%s{\n",__func__)


#define debug_func_end(value) \
	debug_printk("}\n")



#define MASK_0 (0x01<<0)
#define MASK_1 (0x01<<1)
#define MASK_2 (0x01<<2)
#define MASK_3 (0x01<<3)
#define MASK_4 (0x01<<4)
#define MASK_5 (0x01<<5)
#define MASK_6 (0x01<<6)
#define MASK_7 (0x01<<7)
#define MASK_8 (0x01<<8)
#define MASK_9 (0x01<<9)
#define MASK_10 (0x01<<10)
#define MASK_11 (0x01<<11)
#define MASK_12 (0x01<<12)
#define MASK_13 (0x01<<13)
#define MASK_14 (0x01<<14)
#define MASK_15 (0x01<<15)
#define MASK_16 (0x01<<16)
#define MASK_17 (0x01<<17)
#define MASK_18 (0x01<<18)
#define MASK_19 (0x01<<19)
#define MASK_20 (0x01<<20)
#define MASK_21 (0x01<<21)
#define MASK_22 (0x01<<22)
#define MASK_23 (0x01<<23)
#define MASK_24 (0x01<<24)
#define MASK_25 (0x01<<25)
#define MASK_26 (0x01<<26)
#define MASK_27 (0x01<<27)
#define MASK_28 (0x01<<28)
#define MASK_29 (0x01<<29)
#define MASK_30 (0x01<<30)
#define MASK_31 (0x01<<31)








#endif
