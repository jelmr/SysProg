#include "my_strlcat.h"
#include "my_strlen.h"

unsigned my_strlcat(char *dst, const char *src, unsigned n)
{
    unsigned dst_length;
    unsigned src_length;

    dst_length = (unsigned) my_strlen(dst);

    if (dst_length >= n)
        return (dst_length + n);

    src_length = 0;

    while ((dst_length + src_length) < n && src[src_length] != 0)
    {
        dst[dst_length + src_length] = src[src_length];
        src_length++;
    }

    if ((dst_length + src_length) >= n)
        dst[dst_length + src_length - 1] = '\0';

    while (src[src_length] != 0)
        src_length++;

    return (dst_length + src_length);
}
