#include <stdio.h>

#include "common.h"
#include "ssd1306.h"
#include "dht11.h"
#include "led.h"

extern volatile _Bool flag;

int main(int argc, char **argv)
{
    reg_timer();

    send_command("0x00", sizeof("0x00"));
    /*
    send_cursor_pos(5);
    send_line_num(5);
    send_message("HELLO", sizeof("HELLO"));
    */
    
    char str_dht11_temperature[5];
    char str_dht11_humidity[5];

    while(1){
        if(flag == 1){
            sprintf(str_dht11_temperature, "%d", dht11_data.temperature);
            sprintf(str_dht11_humidity, "%d", dht11_data.humidity);

            send_cursor_pos(5);
            send_line_num(0);
            send_message("Temp", sizeof("Temp"));
            send_cursor_pos(40);
            send_line_num(0);
            send_message("Humid", sizeof("Humid"));

            send_cursor_pos(5);
            send_line_num(3);
            send_message(str_dht11_temperature, sizeof(str_dht11_temperature));
            send_line_num(3);
            send_cursor_pos(40);
            send_message(str_dht11_humidity, sizeof(str_dht11_humidity));
            read_cursor_pos();
            read_line_num();

            read_dht11_update();
            read_dht11_temperature();
            read_dht11_humidity();

            send_led1_value(LED_ENABLED);

            /* Debug info */
            /*
            printf("cursor_pos = %d; line_num = %d\n",
                ssd1306_data.cursor_pos, ssd1306_data.line_num);
            printf("read_cursor_pos = %d; read_line_num = %d\n",
                ssd1306_data.read_cursor_pos, ssd1306_data.read_line_num);
                */

            flag = 0;
        }
    }
    
    return 0;
}
