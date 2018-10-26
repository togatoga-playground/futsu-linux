#ifndef HTTP_FILE_H
#define HTTP_FILE_H
#include <sys/stat.h>
#include "httpcore.h"
struct FileInfo;

struct FileInfo
{
    char *path;
    long size;
    int ok;
};

extern struct FileInfo *get_fileinfo(char *docroot, char *urlpath);
extern void free_fileinfo(struct FileInfo *info);
static char *build_fspath(char *docroot, char *urlpath);

extern struct FileInfo *get_fileinfo(char *docroot, char *urlpath)
{
    struct FileInfo *info;
    struct stat st;
    info = (FileInfo *)xmalloc(sizeof(struct FileInfo));
    info->path = build_fspath(docroot, urlpath);
    info->ok = 0;

    if (lstat(info->path, &st) < 0)
    {
        return info;
    }
    if (!S_ISREG(st.st_mode))
    {
        return info;
    }
    info->ok = 1;
    info->size = st.st_size;
    return info;
}

extern void free_fileinfo(struct FileInfo *info)
{
    free((void *)info->path);
    free((void *)info);
}

static char *build_fspath(char *docroot, char *urlpath)
{
    char *path;
    path = (char *)xmalloc(strlen(docroot) + 1 + strlen(urlpath) + 1);
    sprintf(path, "%s%s", docroot, urlpath);
    return path;
}

#endif // !HTTP_FILE_H