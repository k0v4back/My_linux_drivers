#include <stdio.h>

#include "common.h"
#include "ssd1306.h"
#include "dht11.h"
#include "led.h"
#include "aht10.h"

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
    
    char str_dht11_temperature[10];
    char str_dht11_humidity[10];
    char str_aht10_temperature[10];
    char str_aht10_humidity[10];

    while(1){
        if(flag == 1){
            sprintf(str_dht11_temperature, "%dC", dht11_data.temperature);
            sprintf(str_dht11_humidity, "%d%%", dht11_data.humidity);
            sprintf(str_aht10_temperature, "%dC", aht10_data.temperature);
            sprintf(str_aht10_humidity, "%d%%", aht10_data.humidity);

            send_cursor_pos(5);
            send_line_num(0);
            send_message("DHT11", sizeof("DHT11"));
            send_cursor_pos(70);
            send_line_num(0);
            send_message("AHT10", sizeof("AHT10"));

            send_cursor_pos(5);
            send_line_num(2);
            send_message(str_dht11_temperature, sizeof(str_dht11_temperature));
            send_line_num(4);
            send_cursor_pos(5);
            send_message(str_dht11_humidity, sizeof(str_dht11_humidity));
            read_cursor_pos();
            read_line_num();

            send_cursor_pos(70);
            send_line_num(2);
            send_message(str_aht10_temperature, sizeof(str_aht10_temperature));
            send_line_num(4);
            send_cursor_pos(70);
            send_message(str_aht10_humidity, sizeof(str_aht10_humidity));
            read_cursor_pos();
            read_line_num();

            read_dht11_update();
            read_dht11_temperature();
            read_dht11_humidity();
            
            read_aht10_temperature();
            read_aht10_humidity();

//            send_led_value(LED_1, LED_DISABLED);
           send_led_value(LED_2, LED_ENABLED);
//            led_toggle(LED_1);


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
