#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "common.h"

volatile _Bool flag = 0;

void sig_handler(int signal)
{
    flag = 1;
}

void reg_timer(void)
{
    struct itimerval interval_val;

    if(signal(SIGALRM, sig_handler) == SIG_ERR){
        perror("Unable to catch SIGALRM");
        log_file_write("signal() -> Unable to catch SIGALRM");
        exit(1);
    }

    interval_val.it_value.tv_sec = TIMER_INTERVAL_MS/1000;
    interval_val.it_value.tv_usec = (TIMER_INTERVAL_MS*1000) % 1000000;
    interval_val.it_interval = interval_val.it_value;

    if(setitimer(ITIMER_REAL, &interval_val, NULL) == -1){
        perror("Error calling setitimer()");
        log_file_write("setitimer() -> Error calling setitimer()");
        exit(1);
    }
}

void open_errors_check(int fd)
{
    if(fd < 0){
        if(errno == ENOENT){
            /* Write to log file about problem */
            log_file_write("open() -> ENOENT error");
            exit(EXIT_FAILURE);
        }
        perror("open");
        log_file_write("open() -> Other error");
        exit(EXIT_FAILURE);
    }
}

void write_errors_check(int write_num)
{
    if(write_num < 0){
        if(errno == EBADF){
            /* Write to log file about problem */
            log_file_write("write() -> EBADF error");
            exit(EXIT_FAILURE);
        }else if(errno == EFAULT){
            /* Write to log file about problem */
            log_file_write("write() -> EFAULT error");
            exit(EXIT_FAILURE);
        }
        perror("write");
        log_file_write("write() -> Other error");
        exit(EXIT_FAILURE);
    }
}

void read_errors_check(int read_num)
{
    if(read_num < 0){
        if(errno == EINVAL){
            /* Write to log file about problem */
            log_file_write("read() -> EINVAL error");
            exit(EXIT_FAILURE);
        }
        perror("read");
        log_file_write("read() -> Other error");
        exit(EXIT_FAILURE);
    }
}

void log_file_write(char *msg)
{
    int fd;
    ssize_t write_num;
    struct tm *local;
    time_t t = time(NULL);

    /*
    fd = open(LOG_FILE_PATH, O_WRONLY | O_CREAT | O_APPEND,
           S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); 
    open_errors_check(fd);

    write_num = write(fd, "\n----------\n", sizeof("\n----------\n"));
    write_num = write(fd, msg, size);
    write_num = write(fd, "\n----------\n", sizeof("\n----------\n"));
    write_errors_check(write_num);

    close(fd);
    */

    local = localtime(&t);
    FILE *fp = fopen(LOG_FILE_PATH, "a");
    fprintf(fp, "%s", asctime(local));
    fprintf(fp, "%s\n", msg);
    fprintf(fp, "%s\n\n", "--------------------");
    fclose(fp);
}


