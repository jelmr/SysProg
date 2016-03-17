#include "mysys.h"
#include "mystream.h"
#include <stdlib.h>

st_t *st_open(const char *path, const char *mode)
{
    st_t *st;

    st = malloc(sizeof (st_t));
    st->buf = malloc(ST_BUFFER_SIZE * sizeof (char));
    st->index_end = st->index_begin = 0;

    if (mode[0] == 'r' && mode[1] == '+')
        st->mode = O_RDWR | O_CREAT;
    else if (mode[0] == 'r')
        st->mode = O_RDONLY;
    else if (mode[0] == 'w')
        st->mode = O_WRONLY | O_CREAT;
    else if (mode[0] == 'a' && mode[1] == '+')
        st->mode = O_APPEND | O_RDWR | O_CREAT;
    else if (mode[0] == 'a')
        st->mode = O_APPEND | O_WRONLY | O_CREAT;
    else
        return 0;

    st->fd = my_open(path, st->mode, 0666);

    return st;
}
