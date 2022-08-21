#ifndef _CHECK_DRIVERS_H
#define _CHECK_DRIVERS_H

typedef enum {
    false = 0,
    true = 1
} bool;

enum drivers_list {
    AHT10,
    DHT11,
    LED_CONTROL,
    SSD1306,
    AT24C64,
    GPIO_IRQ
};

struct loaded_drivers {
    bool aht10;
    bool dht11;
    bool led_control;
    bool ssd1306;
    bool at24c64;
    bool gpio_irq;
};

struct loaded_drivers list_loadede_drivers = {
    .aht10          = true,
    .dht11          = true,
    .led_control    = true,
    .ssd1306        = true,
    .at24c64        = true,
    .gpio_irq       = true
};

void check_for_drivers(void);
void open_driver(char *path, enum drivers_list driver);

#endif
