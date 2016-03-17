#include "my_memset.h"

void *my_memset(void *dst, int c, unsigned n)
{
    unsigned      i;
    unsigned char *dst_byte;
    unsigned char c_byte;

    dst_byte = (unsigned char*)dst;
    c_byte = (unsigned char)c;


    for (i = 0; i < n; i++)
        dst_byte[i] = c_byte;

    return dst;
}
