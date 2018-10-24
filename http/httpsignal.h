#ifndef HTTP_SIGNAL_H
#define HTTP_SIGNAL_H
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "httputil.h"

typedef void (*sighandler_t)(int);

extern void install_signal_handlers(void);
extern void trap_signal(int sig, sighandler_t handler);
extern void signal_exit(int sig);

extern void install_signal_handlers(void)
{
    trap_signal(SIGPIPE, signal_exit);
}

extern void trap_signal(int sig, sighandler_t handler)
{
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    if (sigaction(sig, &act, NULL) < 0)
    {
        log_exit("sigaction() failed: %s", strerror(errno));
    }
}

extern void signal_exit(int sig)
{
    log_exit("exit by signal %d", sig);
}

#endif // !HTTP_SIGNAL_H