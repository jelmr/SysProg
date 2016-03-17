#include "my_strlcpy.h"

unsigned my_strlcpy(char *dst, const char *src, unsigned n)
{
    unsigned i;

    i = 0;

    if (n > 0)
    {
        while (i < (n - 1) && src[i] != 0)
        {
            dst[i] = src[i];
            i++;
        }

        dst[i] = 0;
    }

    while (src[i] != 0)
        i++;

    return i;
}
