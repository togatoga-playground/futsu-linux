#ifndef HTTP_DATA_H
#define HTTP_DATA_H
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include "httpconst.h"
#include "httpsignal.h"
#include "httpcore.h"
#include "httpfile.h"

struct HTTPHeaderField;
struct HTTPRequest;

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

extern void free_requst(struct HTTPRequest *req);
extern struct HTTPRequest *read_request(FILE *in);
extern void read_request_line(struct HTTPRequest *req, FILE *in);
extern struct HTTPHeaderField *read_header_field(FILE *in);
extern long content_length(struct HTTPRequest *req);
extern char *lookup_header_field_value(struct HTTPRequest *req, char *name);
extern void respond_to(struct HTTPRequest *req, FILE *out, char *docroot);
extern void do_file_response(struct HTTPRequest *req, FILE *out, char *docroot);
extern void output_common_header_fields(struct HTTPRequest *req, FILE *out, char *status);

extern void free_requst(struct HTTPRequest *req)
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

extern struct HTTPRequest *read_request(FILE *in)
{
    struct HTTPRequest *req;
    struct HTTPHeaderField *h;

    req = xmalloc(sizeof(struct HTTPRequest));
    read_request_line(req, in);
    req->header = NULL;

    while (h = read_header_field(in))
    {
        h->next = req->header;
        req->header = h;
    }
    req->length = content_length(req);

    if (req->length != 0)
    {
        if (req->length > MAX_REQUEST_BODY_LENGTH)
        {
            log_exit("request body too long");
        }
        req->body = xmalloc(req->length);
        if (fread(req->body, req->length, 1, in) < 1)
        {
            log_exit("failed to read request body");
        }
    }
    else
    {
        req->body = NULL;
    }

    return req;
}

extern void read_request_line(struct HTTPRequest *req, FILE *in)
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

extern struct HTTPHeaderField *read_header_field(FILE *in)
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

extern long content_length(struct HTTPRequest *req)
{
    char *val;
    long len;
    val = lookup_header_field_value(req, "Content-Length");
    if (!val)
    {
        return 0;
    }
    len = atol(val);
    if (len < 0)
    {
        log_exit("negative Content-length value");
    }
    return len;
}

extern char *lookup_header_field_value(struct HTTPRequest *req, char *name)
{
    struct HTTPHeaderField *h;
    for (h = req->header; h; h = h->next)
    {
        if (strcasecmp(h->name, name) == 0)
        {
            return h->value;
        }
    }
    return NULL;
}

extern void respond_to(struct HTTPRequest *req, FILE *out, char *docroot)
{
    if (strcmp(req->method, "GET") == 0)
    {
    }
    else if (strcmp(req->method, "HEAD") == 0)
    {
    }
    else if (strcmp(req->method, "POST") == 0)
    {
    }
    else
    {
    }
}

extern void do_file_response(struct HTTPRequest *req, FILE *out, char *docroot)
{
    struct FileInfo *info;
    info = get_fileinfo(docroot, req->path);
    if (!info->ok)
    {
        free_fileinfo(info);
        //not_found
        return;
    }
    output_common_header_fields(req, out, "200 OK");
    fprintf(out, "Content-Length: %ld\r\n", info->size);
}

extern void output_common_header_fields(struct HTTPRequest *req, FILE *out, char *status)
{
    time_t t;
    struct tm *tm;
    char buf[TIME_BUF_SIZE];
    t = time(NULL);
    tm = gmtime(&t);
    if (!tm)
    {
        log_exit("gmtime() failed: %s", strerror(errno));
    }
    strftime(buf, TIME_BUF_SIZE, "%a %d %b %Y %H:%M:%S GMT", tm);
    fprintf(out, "HTTP/1. %d %s\r\n", HTTP_MINOR_VERSION, status);
    fprintf(out, "Date: %s\r\n", buf);
    fprintf(out, "Server: %s/%s\r\n", SERVER_NAME, SERVER_VERSION);
    fprintf(out, "Connection: close\r\n");
}

#endif // HTTP_DATA_H