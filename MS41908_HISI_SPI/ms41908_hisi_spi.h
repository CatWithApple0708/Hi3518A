#ifndef MS41908_HISI_SPI_H
#define MS41908_HISI_SPI_H

#define ms41908_R 1
#define ms41908_W 0



#define MS41908_RST_CMD 5
#define MS41908_WR_REG_CMD 6
#define MS41908_RD_REG_CMD 7
#define MS41908_RD_ZOOM 8
#define MS41908_RD_FOCUS 9
#define MS41908_SET_VD_FZ 10
#define MS41908_CLEAR_VD_FZ 11

struct ms41908_msg
{
	unsigned char addr;//6bit
	char rw;//only  1 or 0
	unsigned int data; //16bit
};

struct ms41908_msgs
{
	struct ms41908_msg *msg;
	unsigned size;
};


#endif