#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "dht11.h"
#include "common.h"

void read_dht11_temperature(void)
{
    int fd;
    ssize_t read_num;
    int dht11_temperature;
    char read_temperature[SIZE_READ_BUFFER];

    fd = open(DHT11_TEMPERATURE, O_RDONLY | O_SYNC); 
    open_errors_check(fd, __func__);

    read_num = read(fd, read_temperature, SIZE_READ_BUFFER);
    read_errors_check(read_num, __func__);

    dht11_temperature = atoi(read_temperature);
    dht11_data.temperature = dht11_temperature;

    close(fd);
}

void read_dht11_humidity(void)
{
    int fd;
    ssize_t read_num;
    int dht11_humidity;
    char read_humidity[SIZE_READ_BUFFER];

    fd = open(DHT11_HUMIDITY, O_RDONLY | O_SYNC); 
    open_errors_check(fd, __func__);

    read_num = read(fd, read_humidity, SIZE_READ_BUFFER);
    read_errors_check(read_num, __func__);

    dht11_humidity = atoi(read_humidity);
    dht11_data.humidity = dht11_humidity;

    close(fd);
}

void read_dht11_update(void)
{
    int fd;
    ssize_t read_num;
    int dht11_humidity;
    char read_humidity[SIZE_READ_BUFFER];

    fd = open(DHT11_UPDATE, O_RDONLY | O_SYNC); 
    open_errors_check(fd, __func__);

    read_num = read(fd, read_humidity, SIZE_READ_BUFFER);
    read_errors_check(read_num, __func__);

    close(fd);
}

