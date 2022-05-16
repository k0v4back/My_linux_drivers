### Sysfs interface for AHT10

#### Get temperature
```console
cat /sys/devices/aht10/temp
``` 

#### Get humidity
```console
cat /sys/devices/aht10/humid
``` 

<br>
 
  The connection between BBB and AHT10:
  | BeagleBone Black | AHT10                 |
  |:----------------:|:---------------------:|
  | P9_19 (I2C2_SCL) | SCL                   |
  | P9_20 (I2C2_SDA) | SDA                   |
