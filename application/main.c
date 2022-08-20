#include <stdio.h>

#include "common.h"
#include "ssd1306.h"
#include "dht11.h"
#include "led.h"
#include "aht10.h"
#include "at24c64.h"

extern volatile _Bool flag;
extern volatile unsigned int button_push;

static struct at24c64 eeprom_data_at24c64 = {
    .fpos = 100
};

static struct at24c64 *ptr_eeprom_data_at24c64 = &eeprom_data_at24c64;

int main(int argc, char **argv)
{
    reg_timer();
    button_signal();

    send_command("0x00", sizeof("0x00"));
    
    char str_dht11_temperature[10];
    char str_dht11_humidity[10];
    char str_aht10_temperature[10];
    char str_aht10_humidity[10];
    char str_button_push[10];

    button_signal_reg();

    while (1) {
        if (flag == 1) {
            sprintf(str_dht11_temperature, "%dC", dht11_data.temperature);
            sprintf(str_dht11_humidity, "%d%%", dht11_data.humidity);
            sprintf(str_aht10_temperature, "%dC", aht10_data.temperature);
            sprintf(str_aht10_humidity, "%d%%", aht10_data.humidity);
            sprintf(str_button_push, "%d", button_push);

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

            send_cursor_pos(70);
            send_line_num(2);
            send_message(str_aht10_temperature, sizeof(str_aht10_temperature));
            send_line_num(4);
            send_cursor_pos(70);
            send_message(str_aht10_humidity, sizeof(str_aht10_humidity));

            send_line_num(6);
            send_cursor_pos(40);
            send_message(str_button_push, sizeof(str_button_push));

            read_dht11_update();
            read_dht11_temperature();
            read_dht11_humidity();
            
            read_aht10_temperature();
            read_aht10_humidity();

//          send_led_value(LED_1, LED_DISABLED);
//          send_led_value(LED_2, LED_ENABLED);
            led_toggle(LED_1);
            led_toggle(LED_2);

            /* Save temperature and humidity to eeprom */
            if (write_at24c64(str_dht11_temperature, 10, 
                        (ptr_eeprom_data_at24c64->fpos + 0)) == WRITE_FAIL) { 
                log_file_write("Failed dht11 temperature write to eeprom\n");
            }

            if (write_at24c64(str_dht11_humidity, 10, 
                        (ptr_eeprom_data_at24c64->fpos + 10)) == WRITE_FAIL) { 
                log_file_write("Failed dht11 humidity write to eeprom\n");
            }

            if (write_at24c64(str_dht11_temperature, 10, 
                        (ptr_eeprom_data_at24c64->fpos + 20)) == WRITE_FAIL) { 
                log_file_write("Failed aht10 temperature write to eeprom\n");
            }

            if (write_at24c64(str_dht11_humidity, 10, 
                        (ptr_eeprom_data_at24c64->fpos + 30)) == WRITE_FAIL) { 
                log_file_write("Failed aht10 humidity write to eeprom\n");
            }
            

            flag = 0;
        }
    }
    
    return 0;
}
