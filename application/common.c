#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

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
        exit(1);
    }

    interval_val.it_value.tv_sec = TIMER_INTERVAL_MS/1000;
    interval_val.it_value.tv_usec = (TIMER_INTERVAL_MS*1000) % 1000000;
    interval_val.it_interval = interval_val.it_value;

    if(setitimer(ITIMER_REAL, &interval_val, NULL) == -1){
        perror("error calling setitimer()");
        exit(1);
    }
}
