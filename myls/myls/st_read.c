#include "mysys.h"
#include "mystream.h"
#include "my_memcpy.h"

unsigned st_read(st_t *st, void *buf, unsigned sz)
{
    unsigned bytes_remaining;
    unsigned bytes_written;

    bytes_written = 0;

    if ((st->mode == O_RDONLY) || (st->mode & O_RDWR))
    {
        while (sz)
        {
            bytes_remaining = (unsigned)(st->index_end - st->index_begin);
            bytes_remaining = (sz > bytes_remaining) ? bytes_remaining : sz;

            sz -= bytes_remaining;
            my_memcpy((char *)buf + bytes_written, st->buf, bytes_remaining);
            st->index_end = my_read(st->fd, st->buf, ST_BUFFER_SIZE);
            bytes_written += bytes_remaining;
            st->index_begin = 0;
            bytes_remaining = (unsigned)st->index_end;

            if (st->index_end == 0)
                break;
        }
    }
    return bytes_written;
}
