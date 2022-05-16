### Sysfs interface for DHT11

#### Get temperature
```console
cat /sys/class/dht11/DHT11/temperature
``` 

#### Get humidity
```console
cat /sys/class/dht11/DHT11/humidity
``` 

#### Update temperature and humidity
```console
cat /sys/class/dht11/DHT11/update
``` 

<br>
 
  The connection between BBB and DHT11:
  | BeagleBone Black | DHT11                 |
  |:----------------:|:---------------------:|
  | GPIO2_7          | Data pin connection   |
