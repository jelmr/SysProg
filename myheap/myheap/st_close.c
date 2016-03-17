#include "mysys.h"
#include "mystream.h"
#include "stdlib.h"

void st_close(st_t *st)
{
    st_flush(st);
    my_close(st->fd);
    free(st->buf);
    free(st);
}
