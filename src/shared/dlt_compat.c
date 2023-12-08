#include <errno.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "dlt_compat.h"

#if defined(__VXWORKS__)

int alphasort(const struct dirent **a, const struct dirent **b)
{
    return strcoll((**a).d_name, (**b).d_name);
}

static int __scandir(DIR *dp, struct dirent ***namelist,
                     int (*select)(const struct dirent *),
                     int (*cmp)(const struct dirent **, const struct dirent **))
{
    if (dp == NULL) {
        return -1;
    }

    int save = errno;
    errno = 0;

    int result;
    struct dirent **v = NULL;
    size_t cnt = 0;
    size_t vsize = 0;
    struct dirent *d;

    while ((d = readdir(dp)) != NULL) {
        if (select != NULL) {
            int selected = (*select)(d);
            errno = 0; // reset in case modified externally

            if (!selected) {
                continue;
            }
        }

        if (cnt == vsize) {
            if (vsize == 0) {
                vsize = 10;
            }
            else {
                vsize *= 2;
            }

            struct dirent **new = realloc(v, vsize * sizeof(struct dirent *));

            if (new == NULL) {
                break;
            }

            v = new;
        }

        size_t dsize = &d->d_name[strlen(d->d_name)] - (char *)d;
        struct dirent *vnew = malloc(dsize);
        if (vnew == NULL) {
            break;
        }
        v[cnt++] = (struct dirent *)memcpy(vnew, d, dsize);
        errno = 0; // reset in case modified externally
    }

    if (errno == 0) {
        closedir(dp);

        /* Sort the list if we have a comparison function to sort with.  */
        if (cmp != NULL) {
            qsort(v, cnt, sizeof(struct dirent *),
                  (int (*)(const void *, const void *))cmp);
        }

        *namelist = v;
        result = cnt;
    }
    else {
        /* This frees everything and calls closedir.  */
        for (size_t i = 0; i < cnt; ++i) {
            free(v[i]);
        }
        free(v);
        closedir(dp);
        result = -1;
    }

    if (result >= 0)
        errno = save;
    return result;
}

int scandir(const char *dir, struct dirent ***namelist,
            int (*select)(const struct dirent *),
            int (*cmp)(const struct dirent **, const struct dirent **))
{
    return __scandir(opendir(dir), namelist, select, cmp);
}

#endif
