#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#include "aht10.h"
#include "dht11.h"
#include "led.h"
#include "ssd1306.h"
#include "at24c64.h"
#include "check_drivers.h"

void check_for_drivers(void)
{
    /* Try to open AHT10 driver */
    open_driver(AHT10_DIR, AHT10);

    /* Try to open DHT11 driver */
    open_driver(DHT11_DIR, DHT11);

    /* Try to open LED driver */
    open_driver(LED_DIR, LED_CONTROL);

    /* Try to open SSD1306 driver */
    open_driver(SSD1306_DIR, SSD1306);

    /* Try to open AT24C64 driver */
    open_driver(AT24C64_DIR, AT24C64);
}

void open_driver(char *path, enum drivers_list driver)
{
    DIR *dir;

    dir = opendir(path);
    if (dir == NULL) {
        if (errno == ENOENT) {
            switch (driver) {
                case AHT10:
                    list_loadede_drivers.aht10 = false;
                    break;
                case DHT11:
                    list_loadede_drivers.dht11 = false;
                    break;
                case LED_CONTROL:
                    list_loadede_drivers.led_control = false;
                    break;
                case SSD1306:
                    list_loadede_drivers.ssd1306 = false;
                    break;
                case AT24C64:
                    list_loadede_drivers.at24c64 = false;
                    break;
                case GPIO_IRQ:
                    list_loadede_drivers.gpio_irq = false;
                    break;
                default:
                    break;
            }
        }
    }
}
