#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2C_ADAPTER     "/dev/i2c-2"
 
int read_buffer(int fd)
{
        struct i2c_rdwr_ioctl_data data;
        struct i2c_msg messages[2];
        //unsigned char write_buf[1] = {66}, read_buf[1] = {0x00};
        unsigned char write_buf[1024] = "k0v4";

        /* 
         * .addr - Адрес устройства (датчика)
         * .flags - операция чтения или записи (0 - w, 1 - r)
         * .len - кол-во передаваемых/принимаемых сообщений
         * .buf - буфер на чтение или запись
         */
        messages[0].addr = 0x38;
        messages[0].flags = 0;
        messages[0].len = 5;
        messages[0].buf = write_buf;

	/* read msg (flag = 1) */
	/*
        messages[1].addr = 0x38;
        messages[1].flags = 1;
        messages[1].len = 1;
        messages[1].buf = read_buf;
	*/
 
        data.msgs = messages;
        //data.nmsgs = 2;
        data.nmsgs = 1;
 
        if (ioctl(fd, I2C_RDWR, &data) < 0)
                printf("Cant send data!\n");
	/*
        else
                printf("ID = 0x%x\n", read_buf[0]);
		*/

}

int main(int argc, char **argv)
{
        int fd;
 
         /*
          * Open I2C file descriptor.
          */
         fd = open(I2C_ADAPTER, O_RDWR);
 
         if (fd < 0) {
                 printf("Unable to open i2c file\n");
                 return 0;
         }
 
         read_buffer(fd);
 
         return 0;
 }
