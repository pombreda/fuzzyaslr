#ifndef __DEBUG_H
#define __DEBUG_H

#ifndef NDEBUG
# define dprintf(x, y, z...) fprintf(stderr, "%s:%03d,%s():%s: " y "\n", __FILE__, __LINE__, __func__, x, ## z)
# define DEBUG(x) x
#else
# define dprintf(x, y, z...)
# define DEBUG(x) 
#endif
#define WARN "\awarn"
#define INFO "info"
#define ERROR "\aerror"

#else
#warning debug.h included twice
#endif
