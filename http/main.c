#include <stdio.h>
#include "httpdata.h"
#include "httpsignal.h"

static void service(FILE *in, FILE *out, char *docroot);

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
    struct HTTPRequest *req;
    req = read_request(in);
    //respond_to(req, out, docroot);
    free_requst(req);
}
