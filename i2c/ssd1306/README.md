### Sysfs interface for SSD1306

#### Send command
```console
echo (command) > /sys/devices/ssd1306/command
``` 

#### Set cursor position
```console
echo (cursor_pos) > /sys/devices/ssd1306/cursor_pos
``` 
#### Set line number
```console
echo (line_num) > /sys/devices/ssd1306/line_num
``` 

#### Send message
```console
echo "Some text" > /sys/devices/ssd1306/message
``` 

<br>
 
  The connection between BBB and SSD1306:
  | BeagleBone Black | SSD1306               |
  |:----------------:|:---------------------:|
  | P9_19 (I2C2_SCL) | SCL                   |
  | P9_20 (I2C2_SDA) | SDA                   |
