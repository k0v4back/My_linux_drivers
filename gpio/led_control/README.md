### Sysfs interface for Led control driver

#### Get LED1 value
```console
cat /sys/devices/platform/led_control_devs/gpio_led_control/P9_PIN25_gpio3.21/value
``` 
#### Set LED1 value
```console
echo 1 > /sys/devices/platform/led_control_devs/gpio_led_control/P9_PIN25_gpio3.21/value
``` 

#### Get LED2 value
```console
cat /sys/devices/platform/led_control_devs/gpio_led_control/P9_PIN25_gpio3.19/value
``` 
#### Set LED2 value
```console
echo 1 > /sys/devices/platform/led_control_devs/gpio_led_control/P9_PIN25_gpio3.19/value
``` 

<br>
 
  The connection between BBB and two leds:
  | BeagleBone Black | LEDs                  |
  |:----------------:|:---------------------:|
  | GPIO3_21         | Led 1                 |
  | GPIO3_19         | Led 2                 |
