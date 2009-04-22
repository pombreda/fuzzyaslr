#ifndef __STAT_H
#define __STAT_H

typedef struct {
    int pid;
    char comm[1024];
    char state;
    int ppid;
    int pgrp;
    int session;
    int tty_nr;
    int tpgid;
    unsigned long long flags;
    unsigned long long minflt;
    unsigned long long cminflt;
    unsigned long long majflt;
    unsigned long long cmajflt;
    unsigned long long utime;
    unsigned long long stime;
    long long cutime;
    long long cstime;
    long long priority;
    long long nice;
    long long zero;
    long long itrealvalue;
    unsigned long long stattime;
    unsigned long long vsize;
    long long rss;
    unsigned long long rlim;
    unsigned long long startcode;
    unsigned long long endcode;
    unsigned long long startstack;
    unsigned long long kstkesp;
    unsigned long long kstkeip;
    unsigned long long signal;
    unsigned long long blocked;
    unsigned long long sigignore;
    unsigned long long sigcatch;
    unsigned long long wchan;
    unsigned long long nswap;
    unsigned long long cnswap;
    int exit_signal;
    int processor;
    unsigned long long rt_priority;
    unsigned long long policy;
} stat_t;

bool s_getstat(pid_t pid, int statfd, stat_t *stat);

#endif
