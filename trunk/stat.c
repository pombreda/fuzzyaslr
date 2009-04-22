#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "stat.h"
#include "debug.h"

// Fetch stat information for pid from open fd statfd.
bool s_getstat(pid_t pid, int statfd, stat_t *stat)
{
    const unsigned kMinMatchingStatFields = 30;
    const unsigned kStatBufferMaxSize = 2048;
    char statline[kStatBufferMaxSize];
    int count;

    assert(stat);
    assert(pid > 0);
    assert(statfd >= 0);

    if (pread(statfd, statline, sizeof(statline), 0) <= 0) {
        if (errno != ESPIPE) {
            dprintf(WARN, "unable to pread data from statfile, %m");
            return false;
        }

        // Try to lseek() instead, for some reason Robert's kernel does not
        // allow you to pread() on /proc/pid/stat, but lseek() is fine?!?
        if (lseek(statfd, 0, SEEK_SET) != 0
            || read(statfd, statline, sizeof(statline)) <= 0) {
            dprintf(WARN, "unable to lseek/read data from statfile, %m");
            return false;
        }
    }

    count = sscanf(statline, "%d %s %c %d %d %d %d %d %llu %llu "
                "%llu %llu %llu %llu %llu %llu %llu %lld "
                "%lld %lld %lld %llu %llu %lld %llu "
                "%llu %llu %llu %llu %llu %llu %llu "
                "%llu %llu %llu %llu %llu %d %d %llu "
                "%llu",
                &stat->pid, stat->comm,
                &stat->state, &stat->ppid,
                &stat->pgrp, &stat->session,
                &stat->tty_nr, &stat->tpgid,
                &stat->flags, &stat->minflt,
                &stat->cminflt, &stat->majflt,
                &stat->cmajflt, &stat->utime,
                &stat->stime, &stat->cutime,
                &stat->cstime, &stat->priority,
                &stat->nice, &stat->zero,
                &stat->itrealvalue, &stat->stattime,
                &stat->vsize, &stat->rss,
                &stat->rlim, &stat->startcode,
                &stat->endcode, &stat->startstack,
                &stat->kstkesp, &stat->kstkeip,
                &stat->signal, &stat->blocked,
                &stat->sigignore, &stat->sigcatch,
                &stat->wchan, &stat->nswap,
                &stat->cnswap, &stat->exit_signal,
                &stat->processor, &stat->rt_priority,
                &stat->policy);

    // Sanity check
    assert(stat->pid == pid);

    // Allso some variation based on kernel
    return count >= kMinMatchingStatFields;
}
