#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "debug.h"
#include "tree.h"
#include "sample.h"

// tsearch/qsort comparison routine for sample_t
static int compar(const void *a, const void *b)
{
    const sample_t *x = a, *y = b;
    return x->address - y->address;
}

// You can use these values externally, but dont call them
void *s_compare = compar;
void *s_free = free;

// Record a new execution address
bool s_record(tree_t *samples, uintptr_t address)
{
    sample_t *r, i = { address, 0 };
    void **data;

    tree_find(samples, &i, &data);

    if (data != NULL) {
        // Already seen this address
        r = *data;
        assert(r->address == address);
        assert(r->count);
        r->count++;
        return false;
    } else {
        // Record this new isntruction
        dprintf(INFO, "new unique sample %#x", address);
        if ((r = malloc(sizeof(sample_t)))) {
            memcpy(r, &i, sizeof(i));
            tree_add(samples, r, &data);
            r->count++;
        }
        return true;
    }

    abort();
    return false;
}

// Return an array of seen values, caller must free().
bool s_dump(tree_t *samples, uintptr_t **values, size_t *count)
{
    void save(const void *data)
    {
        const sample_t *s = data;

        *values = realloc(*values, sizeof(uintptr_t) * (*count + 1));
        (*values)[(*count)++] = s->address;
    }
    
    *values = NULL;
    *count = 0;
    tree_walk(samples, save);
    return true;
}
