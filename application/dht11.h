#ifndef _DHT11_H
#define _DHT11_H

#define DHT11_TEMPERATURE       "/sys/class/dht11/DHT11/temperature"
#define DHT11_HUMIDITY          "/sys/class/dht11/DHT11/humidity"
#define DHT11_UPDATE            "/sys/class/dht11/DHT11/update"

#define SIZE_READ_BUFFER        10 

struct dht11_device_data {
    int temperature;
    int humidity;
} dht11_data;

void read_dht11_temperature(void);
void read_dht11_humidity(void);
void read_dht11_update(void);

#endif  // _DHT11_H

