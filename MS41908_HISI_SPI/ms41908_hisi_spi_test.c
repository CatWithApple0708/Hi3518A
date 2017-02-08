#include<stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ms41908_hisi_app.h"
int main(int argc, char const *argv[])
{
	int fd;
	int command = 0;
	int data = 0xAA;
	int retdata = 0;
	char buff[128];
	printf("Timer:\n",__TIME__);
	fd = open("/dev/MS41908",O_RDWR);
	if(fd<0){
		perror("open wrong!");return -1;
	}
	while(1)
	{
		printf("input numb\n");
		command=getc(stdin)-'0';
		getc(stdin);//clear '\n'

		switch(command){
		case 0:
			printf("ms41908_rst\n");
			ms41908_rst(fd);
			break;
		case 1:
			printf("ms41908_write_reg\n");
			ms41908_write_reg(fd,0x00,data);
			break;
		case 2:
			printf("ms41908_read_reg\n");
			ms41908_read_reg(fd,0x00,&retdata);
			printf("retdata:%x\n",retdata);
			break;
		case 3:
			printf("read zoom\n");
			printf("zoom %d\n", ms41908_rd_zoom(fd));
			break;
		case 4:
			printf("read focus\n");
			printf("focus %d\n", ms41908_rd_focus(fd));
			break;

		case 5:
			printf("ms41908_set_vd_fz\n");
			ms41908_set_vd_fz(fd);
			break;
		case 6:
			printf("ms41908_clear_vd_fz\n");
			ms41908_clear_vd_fz(fd);
			break;
		default: printf("input wrong command is %d\n",command);
		}
	}

	close(fd);
	return 0;
}