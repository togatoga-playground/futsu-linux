#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "httpdata.h"
#include "httpsignal.h"
#include "httpnet.h"
#define USAGE "Usage: %s [--port=n] [--chroot --user=u --group=g] <docroot>\n"
static int debug_mode = 0;

static void service(FILE *in, FILE *out, char *docroot);
static void server_main(int server_fd, char *docroot);
static struct option longopts[] = {
    {"debug", no_argument, &debug_mode, 1},
    {"chroot", no_argument, NULL, 'c'},
    {"user", required_argument, NULL, 'u'},
    {"group", required_argument, NULL, 'g'},
    {"port", required_argument, NULL, 'p'},
    {"help", required_argument, NULL, 'h'},
    {0, 0, 0, 0}};

int main(int argc, char *argv[])
{
    int server_fd;
    char *port = NULL;
    char *docroot;
    int do_chroot = 0;
    char *user = NULL;
    char *group = NULL;
    int opt;
    while ((opt = getopt_long(argc, argv, "", longopts, NULL)) != -1)
    {
        switch (opt)
        {
        case 0:
            break;
        case 'c':
            do_chroot = 1;
            break;
        case 'u':
            user = optarg;
            break;
        case 'g':
            group = optarg;
            break;
        case 'p':
            port = optarg;
            break;
        case 'h':
            fprintf(stdout, USAGE, argv[0]);
            exit(0);
        case '?':
            fprintf(stderr, USAGE, argv[0]);
            exit(1);
        }
    }

    if (optind != argc - 1)
    {
        fprintf(stderr, USAGE, argv[0]);
        exit(1);
    }
    docroot = argv[optind];
    if (do_chroot)
    {
        //setup_environment(docroot, user, group);
        docroot = "";
    }
    install_signal_handlers();
    server_fd = listen_socket(port);
    if (!debug_mode)
    {
        //openlog(SERVER_NAME, LOG_PID | LOG_NDELAY, LOG_DAEMON);
        //become_daemon();
    }
    //server_main(server_fd, docroot);
    exit(0);
}

static void service(FILE *in, FILE *out, char *docroot)
{
    struct HTTPRequest *req;
    req = read_request(in);
    respond_to(req, out, docroot);
    free_requst(req);
}

static void server_main(int server_fd, char *docroot)
{
    for (;;)
    {
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        int sock;
        int pid;
        sock = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
        if (sock < 0)
        {
            log_exit("accept(2) failed: %s", strerror(errno));
        }
        pid = fork();
        if (pid < 0)
        {
            exit(3);
        }
        if (pid == 0)
        {
            /*child*/
            FILE *inf = fdopen(sock, "r");
            FILE *outf = fdopen(sock, "w");
            service(inf, outf, docroot);
            exit(0);
        }
        close(sock);
    }
}