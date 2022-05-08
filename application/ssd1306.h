#ifndef _SSD1306_H
#define _SSD1306_H

#define SSD1306_DRIVER_NAME     "ssd1306"
#define I2C_ADAPTER             "/dev/i2c-2"

enum SSD1306_CONSTS{
    ERROR_SSD1306_INIT,
    SUCCESS_SSD1306_INIT
};

int check_init_ssd1306(void);

#endif  // _SSD1306_H

