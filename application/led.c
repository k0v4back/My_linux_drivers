#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "led.h"

void send_led1_value(int led_val)
{
    int fd;
    ssize_t write_num;
    
    fd = open(LED1_VALUE, O_WRONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    if(led_val == LED_DISABLED)
        write_num = write(fd, "0", 2);
    else if(led_val == LED_ENABLED)
        write_num = write(fd, "1", 2);
    if(write_num < 0){
        if(errno == EBADF){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }else if(errno == EFAULT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd);
}
