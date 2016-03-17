#include "my_memcpy.h"

void *my_memcpy(void *dst, const void *src, unsigned n)
{
    unsigned i;
    char     *dst_byte;
    char     *src_byte;

    src_byte = (char*)src;
    dst_byte = (char*)dst;


    for (i = 0; i < n; i++)
        dst_byte[i] = src_byte[i];

    return dst;
}
