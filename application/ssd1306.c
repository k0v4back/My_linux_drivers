#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "ssd1306.h"

int check_exist_ssd1306_driver(void)
{
    int ret = SUCCESS_SSD1306_INIT;
    int fd = 0;
    //lsmod | grep dht11 | wc -l
    
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg messages[2];
    unsigned char write_buf[1024] = "test";

    messages[0].addr = 0x38;
    messages[0].flags = 0; /* 0-w, 1-r */
    messages[0].len = 5;
    messages[0].buf = write_buf;

    data.msgs = messages;
    data.nmsgs = 1;

    fd = open(I2C_ADAPTER, O_RDWR);
    if(fd < 0){
        ret = ERROR_SSD1306_INIT;
        printf("Unable to open i2c file\n");
        return ret;
    }

    if(ioctl(fd, I2C_RDWR, &data) < 0)
        printf("Cant send data!\n");

    return ret; 
}
