#include "mysys.h"
#include "mystream.h"
#include "my_memcpy.h"

unsigned st_write(st_t *st, const void *str, unsigned sz)
{
    unsigned bytes_remaining;
    unsigned bytes_written;

    bytes_written = 0;

    if ((st->mode & O_WRONLY) || (st->mode & O_RDWR))
    {
        bytes_remaining = (unsigned)(ST_BUFFER_SIZE - st->index_begin);

        while (sz > bytes_remaining)
        {
            sz -= bytes_remaining;
            my_memcpy(st->buf + st->index_begin, (char *)str + bytes_written,
                    bytes_remaining);
            my_write(st->fd, st->buf, ST_BUFFER_SIZE);
            bytes_written += bytes_remaining;
            st->index_begin = 0;
            bytes_remaining = ST_BUFFER_SIZE;
        }
        my_memcpy(st->buf + st->index_begin, (char *)str + bytes_written, sz);
        st->index_begin += sz;
        bytes_written += sz;
    }

    return bytes_written;
}
