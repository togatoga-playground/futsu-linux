#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

extern void log_exit(char *fmt, ...);

extern void log_exit(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
    exit(1);
}
#endif // !HTTP_UTIL_H