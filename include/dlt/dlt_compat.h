#ifndef DLT_COMPAT_H
#define DLT_COMPAT_H

#include <dirent.h>
#include <sys/stat.h>

#if defined(__VXWORKS__)

#define S_ISSOCK(mode) ((mode & S_IFMT) == S_IFSOCK)
#define S_ISVTX S_ISTXT // sticky bit
#define SOCK_CLOEXEC 0  // no-op flag

int alphasort(const struct dirent **a, const struct dirent **b);

int scandir(const char *dir, struct dirent ***namelist,
            int (*select)(const struct dirent *),
            int (*cmp)(const struct dirent **, const struct dirent **));

#endif

#endif
