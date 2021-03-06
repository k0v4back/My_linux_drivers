#ifndef _COMMON_H
#define _COMMON_H

#define LOG_FILE_PATH       "log_file.txt"

#define TIMER_INTERVAL_MS 500

#define LOG_ERROR_MSG 80
#define SIG_PUSH_BUTTON 44

#define REGISTER_UAPP _IO('R', 'g')

void sig_handler(int signal);
void reg_timer(void);

void button_signal_handler(int sig);
void button_signal(void);
void button_signal_reg(void);

/* Helper functions */
void open_errors_check(int fd, const char *func_err);
void write_errors_check(int write_num, const char *func_err);
void read_errors_check(int read_num, const char *func_err);
void log_file_write(char *msg);

#endif  // _COMMON_H
