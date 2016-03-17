#include "my_strlcat.h"
#include "my_strlen.h"

unsigned my_strlcat(char *dst, const char *src, unsigned n)
{
    unsigned dst_length;
    unsigned src_length;
    unsigned length;

    length = 0;
    dst_length = (unsigned) my_strlen(dst);
    while (dst[length] && n - length)
        length++;
    if (dst[length])
        return length + (unsigned)my_strlen(src);
    src_length = 0;
    while ((dst_length + src_length) < n && src[src_length] != 0)
    {
        dst[dst_length + src_length] = src[src_length];
        src_length++;
    }
    if ((dst_length + src_length) >= n)
        dst[dst_length + src_length - 1] = '\0';
    else
        dst[dst_length + src_length] = '\0';

    while (src[src_length] != 0)
        src_length++;
    return (dst_length + src_length);
}
