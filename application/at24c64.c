#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "at24c64.h"
#include "common.h"

enum at24c64_write write_at24c64(char *data, int size, int fpos)
{
    int fd;
    ssize_t write_num;

    /* Open driver file */
    fd = open(AT24C64_PATH, O_WRONLY | O_SYNC);
    open_errors_check(fd, __func__);

    /* Move to the needed position */
    if (lseek_at24c64(fd, fpos) == LSEEK_FAIL) {
        log_file_write("Cant move to needed position (read_at24c64)");
        exit(EXIT_FAILURE);
    }

    /* Write data to eeprom */
    write_num = write(fd, data, size);
    write_errors_check(write_num, __func__);

    close(fd);

    return WRITE_SUCCESS;
}

enum at24c64_read read_at24c64(char *data, int size, int fpos)
{
    int fd;
    ssize_t read_num;
    
    /* Open driver file */
    fd = open(AT24C64_PATH, O_RDONLY | O_SYNC);
    open_errors_check(fd, __func__);

    /* Move to the needed position */
    if (lseek_at24c64(fd, fpos) == LSEEK_FAIL) {
        log_file_write("Cant move to needed position (read_at24c64)");
        exit(EXIT_FAILURE);
    }

    data = (char *)malloc(sizeof(int) * size);
    if (data == NULL) {
        log_file_write("Unable to allocate memory (read_at24c64)");
        exit(EXIT_FAILURE);
    }

    read_num = read(fd, data, size);
    read_errors_check(read_num, __func__);
    
    close(fd);

    return READ_SUCCESS; 
}

enum at24c64_lseek lseek_at24c64(int fd, int fpos)
{
    int ret;

    ret = lseek(fd, fpos, SEEK_SET);
    if (ret == -1) {
        log_file_write("lseek fail (lseek_at24c64)");
        return LSEEK_FAIL;
    }

    return LSEEK_SUCCESS;
}
