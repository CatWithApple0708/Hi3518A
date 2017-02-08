/***********************************************************************************************
*  Filename      : ms41908_hisi_app.h
*  Time          :
*  Revision      :
*  NAME          : ZhaoHe/Cat_With_Apple
*  ID            :
*  School        : GuiZhou_university
*  Email         : 1013909206@qq.com
*  Writer:Cat_Like_Apples
*
*  Tab_to_space  : NO
*  Description   :
*
************************************************************************************************/

#ifndef MS41908_HISI_APP_H
#define MS41908_HISI_APP_H

#include <sys/ioctl.h>
#include "ms41908_hisi_spi.h"

static inline void ms41908_rst(int fd){
	ioctl(fd,MS41908_RST_CMD,NULL);
}


static inline int ms41908_rd_zoom(int fd){
	return ioctl(fd,MS41908_RD_ZOOM);
}

static inline int ms41908_rd_focus(int fd){
	return ioctl(fd,MS41908_RD_FOCUS);
}

static inline void ms41908_set_vd_fz(int fd){
	ioctl(fd,MS41908_SET_VD_FZ ,NULL);
}
static inline void ms41908_clear_vd_fz(int fd){
	ioctl(fd,MS41908_CLEAR_VD_FZ ,NULL);
}

static inline void ms41908_write_reg(int fd,int addr,int data)
{
	struct ms41908_msg msg[1]={0};
	struct ms41908_msgs msgs={0};

	msg[0].addr = addr;
	msg[0].rw = ms41908_W;
	msg[0].data = data;

	msgs.msg = &msg[0];
	msgs.size = 1;

	ioctl(fd,MS41908_WR_REG_CMD,&msgs);

	return;

}
static inline void ms41908_read_reg(int fd,int addr, int *data)
{
	struct ms41908_msg msg[1]={0};
	struct ms41908_msgs msgs={0};
	int d;
	msg[0].addr = addr;
	msg[0].rw = ms41908_R;
	msg[0].data = 0x00;

	msgs.msg = &msg[0];
	msgs.size = 1;
	ioctl(fd,MS41908_RD_REG_CMD,&msgs);
	*data = msgs.msg[0].data;

	return;
}

#endif