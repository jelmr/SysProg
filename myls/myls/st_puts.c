#include "mysys.h"
#include "mystream.h"

int st_puts(st_t *st, char *str)
{
    int bytes_written;

    bytes_written = 0;

    if ((st->mode & O_WRONLY) || (st->mode & O_RDWR))
    {
        while (*str != '\0')
        {
            st_putchar(st, *str++);
            bytes_written++;
        }
    }

    return bytes_written;
}
