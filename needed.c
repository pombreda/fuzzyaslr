#include <gelf.h>
#include <stdbool.h>
#include <string.h>
#include <libelf.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <assert.h>
#include <errno.h>

#include "debug.h"
#include "needed.h"

// Stupid lazy routine so that I dont have to handle ld.so.conf logic.
static const char * n_find_lib(const char *filename)
{
    static char name[8192];

    snprintf(name, sizeof name, "%s", filename);

    if (access(filename, F_OK) == 0)
        return filename;

    snprintf(name, sizeof name, "/lib/%s", filename);

    if (access(name, F_OK) == 0)
        return name;

    snprintf(name, sizeof name, "/usr/lib/%s", filename);

    if (access(name, F_OK) == 0)
        return name;

    return filename;
}

// Given elf file at filename, call function() on every DT_NEEDED.
bool n_walk_needed(const char *filename, void (*function)(const char *))
{
    Elf_Scn * sect = NULL;
    GElf_Shdr shdr;
    GElf_Ehdr ehdr;
    Elf * elf;
    int fd;

    void __attribute__((constructor)) init()
    {
        elf_version(EV_CURRENT);
    }

    fd = open(n_find_lib(filename), O_RDONLY);

    if (fd == -1 && errno == ENOENT) {
        dprintf(INFO, "cannot find %s", filename);
        abort();
    }
    
    function(n_find_lib(filename));

    elf = elf_begin(fd, ELF_C_READ, NULL);
    
    gelf_getehdr(elf, &ehdr);

    // Search for the .dynamic section
    while ((sect = elf_nextscn(elf, sect))) {
        char *name;
        Elf_Data * data;
        GElf_Dyn sym;
        unsigned entries, i;

        gelf_getshdr(sect, &shdr);
        name = elf_strptr (elf, ehdr.e_shstrndx, shdr.sh_name);

        if (strcmp(name, ".dynamic") == 0) {
            data = elf_getdata(sect, NULL);
            entries = shdr.sh_size / shdr.sh_entsize;

            // Search for NEEDED tags
            for (i = 0; i < entries; i++) {
                gelf_getdyn(data, i, &sym);
                if (sym.d_tag == DT_NEEDED) {
                    // Call myself recursively to handle NEEDED's NEEDED.
                    n_walk_needed(elf_strptr(elf, shdr.sh_link, sym.d_un.d_val), function);
                }
            }

            // All dependencies handled
            return true;
        }
    }

    // No dependencies
    return false;
}
