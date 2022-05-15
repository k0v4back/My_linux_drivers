#ifndef _AHT10_H
#define _AHT10_H

#define AHT10_TEMPERATURE       "/sys/devices/aht10/temp"
#define AHT10_HUMIDITY          "/sys/devices/aht10/humid"

#define SIZE_READ_BUFFER        10 

struct aht10_device_data {
    int temperature;
    int humidity;
} aht10_data;

void read_aht10_temperature(void);
void read_aht10_humidity(void);

#endif  // _AHT10_H
