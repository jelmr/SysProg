#include "mysys.h"
#include "mystream.h"

int st_gets(st_t *st, char *str, unsigned sz)
{
    int  c;
    char *start;
    int  char_read;

    char_read = 0;
    start = str;

    if ((st->mode == O_RDONLY) || (st->mode & O_RDWR))
    {
        while (sz && --sz)
        {
            c = st_getchar(st);
            char_read = 1;
            *str++ = (char)c;
            if (c == '\n')
                break;
        }
        if (char_read)
            *str = '\0';

        return (int)(str - start);
    }
    return 0;
}
