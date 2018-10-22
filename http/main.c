#include <stdarg.h>
#include <stdio.h>
#include <signal.h>
#include <memory.h>

static void log_exit(char *fmt, ...);
static void *xmalloc(size_t sz);
static void install_signal_handlers(void);
static void trap_signal(int sig, sighandler_t handler);
static void signal_exit(int sig);

int main()
{
}

static void log_exit(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
    exit(1);
}

static void *xmalloc(size_t sz)
{
    void *p = malloc(sz);
    if (!p)
    {
        log_exit("failed to allocate memory");
    }
    return p;
}

static void install_signal_handlers(void)
{
    trap_signal(SIGPIPE, signal_exit);
}

static void signal_exit(int sig)
{
    log_exit("exit by signal %d", sig);
}
