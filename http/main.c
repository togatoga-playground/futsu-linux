#include <signal.h>
#include <stdarg.h>
#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef void (*sighandler_t)(int);
static const int LINE_BUF_SIZE = 256;

static void log_exit(char *fmt, ...);
static void *xmalloc(size_t sz);
static void install_signal_handlers(void);
static void trap_signal(int sig, sighandler_t handler);
static void signal_exit(int sig);
static void service(FILE *in, FILE *out, char *docroot);
struct HTTPHeaderField;
struct HTTPRequest;
static void free_requst(struct HTTPRequest *req);
static struct HTTPRequest *read_request(FILE *in);
static void read_request_line(struct HTTPRequest *req, FILE *in);
static struct HTTPHeaderField *read_header_field(FILE *in);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <docroot>\n", argv[0]);
        exit(1);
    }
    install_signal_handlers();
    exit(0);
}

static void service(FILE *in, FILE *out, char *docroot)
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

static void trap_signal(int sig, sighandler_t handler)
{
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    if (sigaction(sig, &act, NULL) < 0)
    {
        log_exit("sigaction() failed: %s", strerror(errno));
    }
}

static void signal_exit(int sig)
{
    log_exit("exit by signal %d", sig);
}

struct HTTPHeaderField
{
    char *name;
    char *value;
    struct HTTPHeaderField *next;
};

struct HTTPRequest
{
    int protocol_minor_version;
    char *method;
    char *path;
    struct HTTPHeaderField *header;
    char *body;
    long length;
};

static void free_requst(struct HTTPRequest *req)
{
    struct HTTPHeaderField *h, *head;
    head = req->header;
    while (head)
    {
        h = head;
        head = head->next;
        free(h->name);
        free(h->value);
        free(h);
    }
    free(req->method);
    free(req->path);
    free(req->body);
    free(req);
}

static struct HTTPRequest *read_request(FILE *in)
{
    struct HTTPRequest *req;
    struct HTTPHeaderField *h;
    req = xmalloc(sizeof(struct HTTPRequest));
    //read_request_line(req, in);
    req->header = NULL;
}

static void read_request_line(struct HTTPRequest *req, FILE *in)
{

    char buf[LINE_BUF_SIZE];
    char *path, *p;
    if (!fgets(buf, LINE_BUF_SIZE, in))
    {
        log_exit("no request line");
    }
    p = strchr(buf, ' ');
    if (!p)
    {
        log_exit("parse error on request line (1): %s", buf);
    }
    *p++ = '\0';
    req->method = xmalloc(p - buf);
    strcpy(req->method, buf);
    //upcase(req->method);
    path = p;
    p = strchr(path, ' ');
    if (!p)
    {
        log_exit("parse error on request line (2): %s", buf);
    }
    *p++ = '\0';

    req->path = xmalloc(p - path);
    strcpy(req->path, path);

    if (strncasecmp(p, "HTTP/1.", strlen("HTTP/1.")) != 0)
    {
        log_exit("parse error no request line (3): %s", buf);
    }
    p += strlen("HTTP/1.");
    req->protocol_minor_version = atoi(p);
}

static struct HTTPHeaderField *read_header_field(FILE *in)
{
    struct HTTPHeaderField *h;
    char buf[LINE_BUF_SIZE];
    char *p;
    if (!fgets(buf, LINE_BUF_SIZE, in))
    {
        log_exit("failed to read request header field: %s", strerror(errno));
    }

    if (buf[0] == '\n' || strcmp(buf, "\r\n") == 0)
    {
        return NULL;
    }

    p = strchr(buf, ':');
    if (!p)
    {
        log_exit("parse error on request header field: %s", buf);
    }
    *p++ = '\0';
    h = xmalloc(sizeof(struct HTTPHeaderField));
    h->name = xmalloc(p - buf);
    strcpy(h->name, buf);

    p += strspn(p, " \t");
    h->value = xmalloc(strlen(p) + 1);
    strcpy(h->value, p);

    return h;
}