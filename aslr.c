#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <elf.h>
#include <libelf.h>
#include <gelf.h>
#include <unistd.h>
#include <search.h>
#include <sched.h>
#include <asm/page.h>
#include <errno.h>

#include "tree.h"
#include "sample.h"
#include "bounds.h"
#include "stat.h"
#include "needed.h"
#include "debug.h"

int main(int argc, char **argv)
{
    char statfilename[32];
    stat_t state;
    pid_t pid;
    int statfd;
    boundary_t *bounds, *bp;
    tree_t *samples;
    
    if (argc != 3) {
        fprintf(stderr, "usage: %s <pid> </path/to/binary>\n", argv[0]);
        return 1;
    }

    pid = strtoul(argv[1], NULL, 0);

    snprintf(statfilename, sizeof(statfilename), "/proc/%d/stat", pid);

    statfd = open(statfilename, O_RDONLY);

    b_new_boundary_record(&bounds);

    void disassemble(const char *filename)
    {
        b_add_boundary_record(bounds, filename);
    }

    n_walk_needed(argv[2], disassemble);

    tree_create(&samples, s_compare, s_free);

    while (s_getstat(pid, statfd, &state)) {
        if (s_record(samples, state.kstkeip)) {
            for (bp = bounds; bp; bp = bp->next) {
                b_fuzzy_match(bp, samples);
            }
        }
        sched_yield();
    }

    return 0;
}

