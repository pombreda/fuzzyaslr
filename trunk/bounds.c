#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <libelf.h>
#include <stdlib.h>
#include <gelf.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <asm/page.h>
#include <string.h>
#include <search.h>

#include "xed-interface.h"
#include "tree.h"
#include "bounds.h"
#include "debug.h"
#include "sample.h"

bool b_new_boundary_record(boundary_t **bndry)
{
    return !! (*bndry = calloc(1, sizeof(boundary_t)));
}

bool b_valid_if_mapped_at_page(boundary_t *bndry, uintptr_t offset, uintptr_t map)
{
    int compar(const void *a, const void *b)
    {
        const uintptr_t *x = a, *y = b;
        return *x - *y;
    }

    return !! bsearch(&offset, bndry->offsets, bndry->count, sizeof(bndry->offsets[0]), compar);
}


bool b_fuzzy_match(boundary_t *bndry, tree_t *samples)
{
    unsigned const kMatchesConfidenceLow = 2;
    unsigned const kMatchesConfidenceMed = 4;
    unsigned const kMatchesConfidenceHigh = 8;
    unsigned page, i, j;
    uintptr_t *addrs, best;
    unsigned bestcnt = 0;
    size_t naddrs;

    s_dump(samples, &addrs, &naddrs);
    
    // For every instruction address I know about
    for (i = 0; i < naddrs; i++) {
        uintptr_t base = addrs[i] & PAGE_MASK;

        // For every possible map address that would contain this instruction
        for (page = 0; page < bndry->npages; page++) {
            uintptr_t map = base - page * PAGE_SIZE;
            bool candidate = true;
            unsigned nmatches = 0;

            // Find all the instructions that would be contained in this map
            for (j = 0; j < naddrs; j++) {
                if (addrs[j] >= map && addrs[j] <= map + PAGE_SIZE * bndry->npages) {

                    // This instruction would be inside the map, see if it's at a valid boundary
                    if ((candidate &= b_valid_if_mapped_at_page(bndry, addrs[j] - map, map)) == false)
                        break;

                    nmatches++;
                }
            }

            // Check if this could be the map location
            if (candidate) {
                if (nmatches > bestcnt) {
                    best = map;
                    bestcnt = nmatches;
                }
            }
        }
    }

    if (bestcnt) {
        if (best != bndry->candidate || bndry->matches < bestcnt) {
            if (bestcnt >= kMatchesConfidenceHigh) {
                if (bndry->matches < kMatchesConfidenceHigh) {
                    dprintf(INFO, "%s could be mapped at %#x, %u matches, HIGH confidence (%u samples)", bndry->filename, best, bestcnt, naddrs);
                    fprintf(stderr, "GUESS: %08x-%08lx r-xp fd:00 %u %s\n",
                        best,
                        best + bndry->npages * PAGE_SIZE,
                        bndry->filesize,
                        bndry->filename);
                }
            } else if (bestcnt >= kMatchesConfidenceMed) {
                if (bndry->matches < kMatchesConfidenceMed)
                    dprintf(INFO, "%s could be mapped at %#x, %u matches, MEDIUM confidence (%u samples)", bndry->filename, best, bestcnt, naddrs);
            } else if (bestcnt >= kMatchesConfidenceLow) {
                if (bndry->matches < kMatchesConfidenceLow)
                    dprintf(INFO, "%s could be mapped at %#x, %u matches, LOW confidence (%u samples)", bndry->filename, best, bestcnt, naddrs);
            }
            bndry->candidate = best;
            bndry->matches = bestcnt;
        }
    } else {
        // dprintf(INFO, "no fuzzy candidates for %s yet", bndry->filename);
    }

    free(addrs);
    return true;
}

bool b_add_boundary_record(boundary_t *bndry, const char *filename)
{
    Elf_Scn *sect;
    GElf_Shdr shdr;
    GElf_Ehdr ehdr;
    GElf_Phdr phdr;
    xed_state_t dstate;
    Elf *elf;
    int fd;
    unsigned i;

    void __attribute__((constructor)) init()
    {
        elf_version(EV_CURRENT);
        xed_tables_init();
    }

    fd = open(filename, O_RDONLY);
    elf = elf_begin(fd, ELF_C_READ, NULL);

    // Initialise Xed
    xed_state_zero(&dstate);
    xed_state_init(&dstate, XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b, XED_ADDRESS_WIDTH_32b);

    // Read the ELF header
    gelf_getehdr(elf, &ehdr);

    // Seek to end of bndry list
    while (bndry->next) {
        if (bndry->filename) {
            if (strcmp(bndry->filename, filename) == 0) {
                close(fd);
                elf_end(elf);
                return true;
            }
        }
        bndry = bndry->next;
    }

    assert(bndry->filename == NULL);
    assert(bndry->count == 0);
    assert(bndry->offsets == NULL);

    // Add a new record
    bndry->next = calloc(1, sizeof(boundary_t));
    bndry->filename = strdup(filename);

    // Search through the program headers for PT_LOAD segments with execute permission
    for (i = 0; i < ehdr.e_phnum; i++) {
        // Read the next program header
        gelf_getphdr(elf, i, &phdr);

        // Check the type and flags match an segment containing code
        if (phdr.p_type == PT_LOAD && (phdr.p_flags & PF_X)) {

            bndry->npages = (phdr.p_memsz + PAGE_SIZE - 1) / PAGE_SIZE;
            
            // XXX: Cannot currently handle > 1 executable PT_LOAD. Remove this when it works.
            assert(bndry->count == 0);

            // Now search for sections that fit into this segment
            for (sect = elf_nextscn(elf, NULL); sect; sect = elf_nextscn(elf, sect)) {

                gelf_getshdr(sect, &shdr);

                // Check if it's executable and within this segment
                if  (shdr.sh_offset >= phdr.p_offset 
                        && shdr.sh_offset + shdr.sh_size < phdr.p_offset + phdr.p_filesz
                        && shdr.sh_type == SHT_PROGBITS
                        && shdr.sh_flags & SHF_EXECINSTR
                        && shdr.sh_flags & SHF_ALLOC) {
                    Elf_Data *data = elf_getdata(sect, NULL);
                    xed_decoded_inst_t xedd;
                    size_t offset;
                    
                    assert(data->d_type == ELF_T_BYTE);
                    // Disassemble this section
                    for (offset = 0; offset < data->d_size;) {
                        xed_decoded_inst_zero_set_mode(&xedd, &dstate);

                        if (xed_decode(&xedd, data->d_buf + offset, 16) == XED_ERROR_NONE) {
                            bndry->offsets = realloc(bndry->offsets, sizeof(bndry->offsets[0]) * (bndry->count + 1));
                            bndry->offsets[bndry->count++] = offset + (shdr.sh_offset - phdr.p_offset);

                            // Move to next instruction
                            offset += xed_decoded_inst_get_length(&xedd);
                        } else {
                            // XED says this isnt valid :-(
                            dprintf(WARN, "oops, desync@%#llx in %s, skipping...", offset + shdr.sh_addr, filename);
                            break;
                        }
                    }
                }
            }
            dprintf(INFO, "Found %u instruction boundaries from PT_LOAD@%#llx in %s", bndry->count, phdr.p_vaddr, filename);
        }
    }

    elf_end(elf);
    close(fd);
    return true;
}
