#ifndef __BOUNDS_H
# define __BOUNDS_H

typedef struct {
    unsigned count;
    unsigned npages;
    void *next;
    size_t filesize;
    char *filename;
    unsigned *offsets;
    uintptr_t candidate;
    unsigned matches;
} boundary_t;

bool b_new_boundary_record(boundary_t **bndry);
bool b_add_boundary_record(boundary_t *bndry, const char *filename);
bool b_fuzzy_match(boundary_t *bndry, tree_t *samples);

#endif
