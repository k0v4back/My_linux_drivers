#ifndef _SSD1306_H
#define _SSD1306_H

#define I2C_ADAPTER                 "/dev/i2c-2"

#define SSD1306_COMMAND             "/sys/devices/ssd1306/command"
#define SSD1306_CURSOR_POS          "/sys/devices/ssd1306/cursor_pos"
#define SSD1306_LINE_NUM            "/sys/devices/ssd1306/line_num"
#define SSD1306_MESSAGE             "/sys/devices/ssd1306/message"
#define SSD1306_NAME                "/sys/devices/ssd1306/name"
#define SSD1306_DIR                 "/sys/devices/ssd1306"

#define CHAR_BUF_SIZE 5

struct ssd1306_device_data {
    int cursor_pos;
    int line_num;
} ssd1306_data;

void send_command(char *command, int lenght);
void send_message(char *message, int lenght);
void send_cursor_pos(int cursor_pos);
void send_line_num(int line_num);

#endif  // _SSD1306_H

