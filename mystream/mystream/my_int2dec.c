#include "my_conv.h"

int my_int2dec(char *dst, int v, unsigned n)
{
    int i;

    if (n == 0)
        return 0;

    if (v < 0)
    {
        i = my_uint2dec(dst + 1, (unsigned)(-v), n - 1);
        if (i != 0)
        {
            *dst = '-';
            return i + 1;
        }
        return i;
    }
    else
        return my_uint2dec(dst, (unsigned) v, n);
}
