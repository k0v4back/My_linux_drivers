#ifndef _LED_H
#define _LED_H

#define LED1_VALUE      "/sys/devices/platform/led_control_devs/gpio_led_control/P9_PIN25_gpio3.21/value" 
#define LED2_VALUE      "/sys/devices/platform/led_control_devs/gpio_led_control/P9_PIN27_gpio3.19/value" 
#define LED1_DIRECTION  "/sys/devices/platform/led_control_devs/gpio_led_control/P9_PIN25_gpio3.21/direction" 
#define LED2_DIRECTION  "/sys/devices/platform/led_control_devs/gpio_led_control/P9_PIN27_gpio3.19/direction" 
#define LED_DIR         "/sys/devices/platform/led_control_devs/gpio_led_control" 

#define SIZE_READ_BUFFER        10 

struct leds_device_data {
    _Bool led1_value;
    _Bool led2_value;
    char led1_direction[SIZE_READ_BUFFER];
    char led2_direction[SIZE_READ_BUFFER];
} leds_data;

enum LED_VALUE {
    LED_DISABLED,
    LED_ENABLED,
};

enum LEDS {
    LED_1,
    LED_2,
};

void send_led_value(enum LEDS led, enum LED_VALUE led_val);
void led_toggle(enum LEDS led);

#endif  // _LED_H
