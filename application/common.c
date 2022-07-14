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
#include <sys/ioctl.h>

#include "common.h"

volatile _Bool flag = 0;
volatile unsigned int button_push = 0;

void sig_handler(int signal)
{
    flag = 1;
}

void reg_timer(void)
{
    struct itimerval interval_val;

    if (signal(SIGALRM, sig_handler) == SIG_ERR) {
        perror("Unable to catch SIGALRM");
        log_file_write("signal() -> Unable to catch SIGALRM");
        exit(1);
    }

    interval_val.it_value.tv_sec = TIMER_INTERVAL_MS/1000;
    interval_val.it_value.tv_usec = (TIMER_INTERVAL_MS*1000) % 1000000;
    interval_val.it_interval = interval_val.it_value;

    if (setitimer(ITIMER_REAL, &interval_val, NULL) == -1) {
        perror("Error calling setitimer()");
        log_file_write("setitimer() -> Error calling setitimer()");
        exit(1);
    }
}

void button_signal_handler(int sig)
{
    button_push++;
    //printf("%d\n", button_push);
}

void button_signal(void)
{
    int fd;

    signal(SIG_PUSH_BUTTON, button_signal_handler);
}

void button_signal_reg(void)
{
    int fd;

    fd = open("/dev/gpio_irq_signal", O_RDONLY);
    open_errors_check(fd, __func__);

    if (ioctl(fd, REGISTER_UAPP, NULL)) {
        perror("Error registering app");
        close(fd);
    }
}

void open_errors_check(int fd, const char *func_err)
{
    char err_msg[LOG_ERROR_MSG];

    if (fd < 0) {
        if (errno == ENOENT) {
            /* Write to log file about problem */
            sprintf(err_msg, "open() -> ENOENT error (Error sourse -> %s)", func_err);
            log_file_write(err_msg);
            exit(EXIT_FAILURE);
        }
        sprintf(err_msg, "open() -> Other error (Error sourse -> %s)", func_err);
        log_file_write(err_msg);
        exit(EXIT_FAILURE);
    }
}

void write_errors_check(int write_num, const char *func_err)
{
    char err_msg[LOG_ERROR_MSG];

    if (write_num < 0) {
        if (errno == EBADF) {
            /* Write to log file about problem */
            sprintf(err_msg, "write() -> ENOENT error (Error sourse -> %s)", func_err);
            log_file_write(err_msg);
            exit(EXIT_FAILURE);
        } else if (errno == EFAULT) {
            /* Write to log file about problem */
            sprintf(err_msg, "write() -> EFAULT error (Error sourse -> %s)", func_err);
            log_file_write(err_msg);
            exit(EXIT_FAILURE);
        }
        sprintf(err_msg, "write() -> Other error (Error sourse -> %s)", func_err);
        log_file_write(err_msg);
        exit(EXIT_FAILURE);
    }
}

void read_errors_check(int read_num, const char *func_err)
{
    char err_msg[LOG_ERROR_MSG];

    if (read_num < 0) {
        if (errno == EINVAL) {
            /* Write to log file about problem */
            sprintf(err_msg, "read() -> EINVAL error (Error sourse -> %s)", func_err);
            log_file_write(err_msg);
            exit(EXIT_FAILURE);
        }
        sprintf(err_msg, "read() -> Other error (Error sourse -> %s)", func_err);
        log_file_write(err_msg);
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


