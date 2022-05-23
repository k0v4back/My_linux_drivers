# Drivers and linux user space application training project
## Demonstration of the project (V 1.0)
<p align="center">
  <img src="https://github.com/k0v4back/My_linux_drivers/blob/main/application/etc/my_linux_drivers.gif" />
  
  The drivers are written for BeagleBone Black, device tree (DT) files may not be suitable for your system.
  I use linux system calls to access the drivers and interact with them.
  I get the temperature and humidity from the DHT11 and AHT10 sensors. Flashing LEDs indicate the operability of the system.
  And display all the information on the display.
  
###  Drivers that are included in the project:
- [aht10](../../tree/main/i2c/aht10): get temperature and humidity from the AHT10 sensor.
- [dht11](../../tree/main/gpio/dht11): get temperature and humidity from the DHT11 sensor.
- [ssd1306](../../tree/main/i2c/ssd1306): printing text to the SSD1306 display.
- [led_control](../../tree/main/gpio/led_control): control of two LEDs.
  
  <br>
  
###  Linux user space application:
- [application](../../tree/main/application): All drivers work via sysfs, use system calls to control sensors and display information on the display
  
</p>

