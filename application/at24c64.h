#ifndef _AT24C64_H
#define _AT24C64_H

#define AT24C64_PATH        "/dev/at24c64_eeprom"

struct at24c64 {
    int fpos;
};

enum at24c64_read {
    READ_SUCCESS,
    READ_FAIL
};

enum at24c64_write {
    WRITE_SUCCESS,
    WRITE_FAIL
};

enum at24c64_lseek {
    LSEEK_SUCCESS,
    LSEEK_FAIL
};

enum at24c64_write write_at24c64(char *data, 
        int size, int fpos);
enum at24c64_read read_at24c64(char *data,
        int size, int fpos);
enum at24c64_lseek lseek_at24c64(int fd, int fpos);

#endif // _AT24C64_H
