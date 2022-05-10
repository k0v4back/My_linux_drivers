#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "ssd1306.h"

void send_command(char *command, int lenght)
{
    int fd;
    ssize_t write_num;
    
    fd = open(SSD1306_COMMAND, O_WRONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    write_num = write(fd, command, lenght);
    if(write_num < 0){
        if(errno == EBADF){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }else if(errno == EFAULT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void send_message(char *message, int lenght)
{
    int fd;
    ssize_t write_num;
    
    fd = open(SSD1306_MESSAGE, O_WRONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    write_num = write(fd, message, lenght);
    if(write_num < 0){
        if(errno == EBADF){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }else if(errno == EFAULT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void send_cursor_pos(int cursor_pos)
{
    int fd;
    ssize_t write_num;
    char str_cursor_pos[15];

    ssd1306_data.cursor_pos = cursor_pos;

    sprintf(str_cursor_pos, "%d", cursor_pos);
    
    fd = open(SSD1306_CURSOR_POS, O_WRONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    write_num = write(fd, str_cursor_pos, 15);
    if(write_num < 0){
        if(errno == EBADF){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }else if(errno == EFAULT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void read_cursor_pos(void)
{
    int fd;
    ssize_t read_num;
    char str_cursor_pos[15];
    int cursor_pos;

    fd = open(SSD1306_CURSOR_POS, O_RDONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    read_num = read(fd, str_cursor_pos, 15);
    if(read_num < 0){
        if(errno == EINVAL){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("read");
        exit(EXIT_FAILURE);
    }
    close(fd);

    cursor_pos = atoi(str_cursor_pos);
    ssd1306_data.read_cursor_pos = cursor_pos;
}

void send_line_num(int line_num)
{
    int fd;
    ssize_t write_num;
    char str_line_num[15];

    ssd1306_data.line_num = line_num;

    sprintf(str_line_num, "%d", line_num);
    
    fd = open(SSD1306_LINE_NUM, O_WRONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    write_num = write(fd, str_line_num, 15);
    if(write_num < 0){
        if(errno == EBADF){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }else if(errno == EFAULT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void read_line_num(void)
{
    int fd;
    ssize_t read_num;
    char str_line_num[15];
    int line_num;

    fd = open(SSD1306_LINE_NUM, O_RDONLY | O_SYNC); 
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("open");
        exit(EXIT_FAILURE);
    }

    read_num = read(fd, str_line_num, 15);
    if(read_num < 0){
        if(errno == EINVAL){
            /* Write to log file about problem */
            exit(EXIT_FAILURE);
        }
        perror("read");
        exit(EXIT_FAILURE);
    }
    close(fd);

    line_num = atoi(str_line_num);
    ssd1306_data.read_line_num = line_num;
}

