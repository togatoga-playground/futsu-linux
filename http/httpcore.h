#ifndef HTTP_CORE_H
#define HTTP_CORE_H
#include <memory.h>
#include "httputil.h"

extern void *xmalloc(size_t sz);

extern void *xmalloc(size_t sz)
{
    void *p = malloc(sz);
    if (!p)
    {
        log_exit("failed to allocate memory");
    }
    return p;
}
#endif // !HTTP_CORE_H