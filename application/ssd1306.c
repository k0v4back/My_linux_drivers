#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "ssd1306.h"
#include "common.h"

void send_command(char *command, int lenght)
{
    int fd;
    ssize_t write_num;
    
    fd = open(SSD1306_COMMAND, O_WRONLY | O_SYNC); 
    open_errors_check(fd);

    write_num = write(fd, command, lenght);
    write_errors_check(write_num);

    close(fd);
}

void send_message(char *message, int lenght)
{
    int fd;
    ssize_t write_num;
    
    fd = open(SSD1306_MESSAGE, O_WRONLY | O_SYNC); 
    open_errors_check(fd);

    write_num = write(fd, message, lenght);
    write_errors_check(write_num);

    close(fd);
}

void send_cursor_pos(int cursor_pos)
{
    int fd;
    ssize_t write_num;
    char str_cursor_pos[CHAR_BUF_SIZE];

    ssd1306_data.cursor_pos = cursor_pos;

    sprintf(str_cursor_pos, "%d", cursor_pos);
    
    fd = open(SSD1306_CURSOR_POS, O_WRONLY | O_SYNC); 
    open_errors_check(fd);

    write_num = write(fd, str_cursor_pos, CHAR_BUF_SIZE);
    write_errors_check(write_num);

    close(fd);
}

void read_cursor_pos(void)
{
    int fd;
    ssize_t read_num;
    char str_cursor_pos[CHAR_BUF_SIZE];
    int cursor_pos;

    fd = open(SSD1306_CURSOR_POS, O_RDONLY | O_SYNC); 
    open_errors_check(fd);

    read_num = read(fd, str_cursor_pos, CHAR_BUF_SIZE);
    read_errors_check(read_num);

    cursor_pos = atoi(str_cursor_pos);
    ssd1306_data.read_cursor_pos = cursor_pos;

    close(fd);
}

void send_line_num(int line_num)
{
    int fd;
    ssize_t write_num;
    char str_line_num[CHAR_BUF_SIZE];

    ssd1306_data.line_num = line_num;

    sprintf(str_line_num, "%d", line_num);
    
    fd = open(SSD1306_LINE_NUM, O_WRONLY | O_SYNC); 
    open_errors_check(fd);

    write_num = write(fd, str_line_num, CHAR_BUF_SIZE);
    write_errors_check(write_num);
    
    close(fd);
}

void read_line_num(void)
{
    int fd;
    ssize_t read_num;
    char str_line_num[CHAR_BUF_SIZE];
    int line_num;

    fd = open(SSD1306_LINE_NUM, O_RDONLY | O_SYNC); 
    open_errors_check(fd);

    read_num = read(fd, str_line_num, CHAR_BUF_SIZE);
    read_errors_check(read_num);

    close(fd);

    line_num = atoi(str_line_num);
    ssd1306_data.read_line_num = line_num;
}

