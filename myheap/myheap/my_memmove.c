#include "my_memmove.h"

void *my_memmove(void *dst, const void *src, unsigned n)
{
    unsigned i;
    char     *dst_byte;
    char     *src_byte;

    src_byte = (char*)src;
    dst_byte = (char*)dst;

    if (src_byte < dst_byte)
    {
        for (i = n; i > 0; i--)
            dst_byte[i - 1] = src_byte[i - 1];
    }
    else if (src_byte > dst_byte)
    {
        for (i = 0; i < n; i++)
            dst_byte[i] = src_byte[i];
    }

    return dst;
}
