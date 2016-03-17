#include "mysys.h"
#include "mystream.h"

int st_getchar(st_t *st)
{
    int result;

    if ((st->mode == O_RDONLY) || (st->mode & O_RDWR))
    {
        if (st->index_end == st->index_begin)
        {
            st->index_end = my_read(st->fd, st->buf, ST_BUFFER_SIZE);
            st->index_begin = 0;
        }

        if (st->index_end == st->index_begin)
            return -1;

        result = st->buf[st->index_begin++];

        if (result < 0 || result > 255)
            result = -1;
    }
    else
        result = -1;

    return result;
}
