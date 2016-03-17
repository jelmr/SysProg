#include "mysys.h"
#include "mystream.h"

void st_flush(st_t *st)
{
    my_write(st->fd, st->buf, (unsigned)st->index_begin);
    st->index_begin = 0;
}
