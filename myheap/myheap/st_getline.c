#include "mysys.h"
#include "mystream.h"
#include <stdlib.h>

char *st_getline(st_t *st)
{
    char          *result;
    int           c;
    unsigned long size;

    result = (char*)(size = 0);

    if ((st->mode == O_RDONLY) || (st->mode & O_RDWR))
    {
        while ((c = st_getchar(st)) != '\n')
            *((result = realloc(result, ++size)) + size - 1) = (char)c;

        if (size)
            *((result = realloc(result, size)) + size) = '\0';
    }

    return result;
}
