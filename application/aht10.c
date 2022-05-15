#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "aht10.h"
#include "common.h"

void read_aht10_temperature(void)
{
    int fd;
    ssize_t read_num;
    int aht10_temperature;
    char read_temperature[SIZE_READ_BUFFER];

    fd = open(AHT10_TEMPERATURE, O_RDONLY | O_SYNC); 
    open_errors_check(fd);

    read_num = read(fd, read_temperature, SIZE_READ_BUFFER);
    read_errors_check(read_num);

    aht10_temperature = atoi(read_temperature);
    aht10_data.temperature = aht10_temperature;

    close(fd);
}

void read_aht10_humidity(void)
{
    int fd;
    ssize_t read_num;
    int aht10_humidity;
    char read_humidity[SIZE_READ_BUFFER];

    fd = open(AHT10_HUMIDITY, O_RDONLY | O_SYNC); 
    open_errors_check(fd);

    read_num = read(fd, read_humidity, SIZE_READ_BUFFER);
    read_errors_check(read_num);

    aht10_humidity = atoi(read_humidity);
    aht10_data.humidity = aht10_humidity; 

    close(fd);
}
