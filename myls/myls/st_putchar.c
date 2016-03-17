#include "mysys.h"
#include "mystream.h"

int st_putchar(st_t *st, int c)
{
    int result;

    result = 0;

    if ((st->mode & O_WRONLY) || (st->mode & O_RDWR))
    {
        if (ST_BUFFER_SIZE == st->index_begin)
            st_flush(st);

        st->buf[st->index_begin++] = (char)c;
        result = 1;
    }

    return result;
}
