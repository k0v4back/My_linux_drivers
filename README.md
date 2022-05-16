## Demonstration of the project (V 1.0)
<p align="center">
  <img src="https://github.com/k0v4back/My_linux_drivers/blob/main/application/etc/my_linux_drivers.gif" />

###  The project uses drivers for sensors:
- [aht10](../../tree/main/i2c/aht10): get temperature and humidity from the AHT10 sensor.
- [dht11](../../tree/main/gpio/dht11): get temperature and humidity from the DHT11 sensor.
- [ssd1306](../../tree/main/i2c/ssd1306): printing text to the SSD1306 display.
- [led_control](../../tree/main/gpio/led_control): control of two LEDs.
  
  <br>
  
###  Linux user space application:
- [application](../../tree/main/application): All drivers work via sysfs, use system calls to control sensors and display information on the display
  
</p>

