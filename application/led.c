#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "led.h"
#include "common.h"

void send_led_value(enum LEDS led, enum LED_VALUE led_val)
{
    int fd;
    ssize_t write_num;

    if(led == LED_1)
        fd = open(LED1_VALUE, O_WRONLY | O_SYNC); 
    else if(led == LED_2)
        fd = open(LED2_VALUE, O_WRONLY | O_SYNC); 
    open_errors_check(fd);

    if(led_val == LED_DISABLED)
        write_num = write(fd, "0", 2);
    else if(led_val == LED_ENABLED)
        write_num = write(fd, "1", 2);
    write_errors_check(write_num);

    close(fd);
}

void led_toggle(enum LEDS led)
{
    int fd;
    ssize_t write_num;

    if(led == LED_1){
        fd = open(LED1_VALUE, O_WRONLY | O_SYNC); 
        if(leds_data.led1_value == LED_DISABLED){
            write_num = write(fd, "1", 2);
            leds_data.led1_value = 1;
        }
        else if(leds_data.led1_value == LED_ENABLED){
            write_num = write(fd, "0", 2);
            leds_data.led1_value = 0;
        }
        write_errors_check(write_num);
        close(fd);
    }else if(led == LED_2){
        fd = open(LED2_VALUE, O_WRONLY | O_SYNC); 
        if(leds_data.led2_value == LED_DISABLED){
            write_num = write(fd, "1", 2);
            leds_data.led2_value = 1;
        }
        else if(leds_data.led2_value == LED_ENABLED){
            write_num = write(fd, "0", 2);
            leds_data.led2_value = 0;
        }
        write_errors_check(write_num);
        close(fd);
    }

}
