#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "dht11.h"

void read_dht11_temperature(void)
{
    int fd;
    ssize_t read_num;
    int dht11_temperature;
    char read_temperature[SIZE_READ_BUFFER];

    fd = open(DHT11_TEMPERATURE, O_RDONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    read_num = read(fd, read_temperature, SIZE_READ_BUFFER);
    if(read_num < 0){
        if(errno == EINVAL){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("read");
        exit(EXIT_FAILURE);
    }
    close(fd);

    dht11_temperature = atoi(read_temperature);
    dht11_data.temperature = dht11_temperature;
}

void read_dht11_humidity(void)
{
    int fd;
    ssize_t read_num;
    int dht11_humidity;
    char read_humidity[SIZE_READ_BUFFER];

    fd = open(DHT11_HUMIDITY, O_RDONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    read_num = read(fd, read_humidity, SIZE_READ_BUFFER);
    if(read_num < 0){
        if(errno == EINVAL){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("read");
        exit(EXIT_FAILURE);
    }
    close(fd);

    dht11_humidity = atoi(read_humidity);
    dht11_data.humidity = dht11_humidity;
}
