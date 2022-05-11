#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "led.h"
#include "common.h"

void send_led1_value(enum LED_VALUE led_val)
{
    int fd;
    ssize_t write_num;
    
    fd = open(LED1_VALUE, O_WRONLY | O_SYNC); 
    open_errors_check(fd);

    if(led_val == LED_DISABLED)
        write_num = write(fd, "0", 2);
    else if(led_val == LED_ENABLED)
        write_num = write(fd, "1", 2);
    write_errors_check(write_num);

    close(fd);
}
