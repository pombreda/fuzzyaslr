#ifndef __SAMPLE_H
#define __SAMPLE_H

// Utility routines for making a tree_t, do not call directly
void *s_compare;
void *s_free;

typedef struct {
    uintptr_t address;
    unsigned long long count;
} sample_t;

// Record an address from sampling (dupes are handled)
bool s_record(tree_t *samples, uintptr_t address);
bool s_dump(tree_t *samples, uintptr_t **values, size_t *count);

#endif
