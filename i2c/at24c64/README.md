### Write data to eeprom AT24C64 and read

#### Write data
```console
echo "SOME DATA" > /dev/at24c64_eeprom
``` 

<br>
 
  The connection between BBB and AT24C64:
  | BeagleBone Black | AT24C64               |
  |:----------------:|:---------------------:|
  | P9_19 (I2C2_SCL) | SCL                   |
  | P9_20 (I2C2_SDA) | SDA                   |
