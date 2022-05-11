#ifndef _COMMON_H
#define _COMMON_H

#define TIMER_INTERVAL_MS 500

void sig_handler(int signal);
void reg_timer(void);

/* Helper functions */
void open_errors_check(int fd);
void write_errors_check(int write_num);
void read_errors_check(int read_num);

#endif  // _COMMON_H
