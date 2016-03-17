#include "my_conv.h"

int my_long2dec(char *dst, long v, unsigned n)
{
    int i;

    if (n == 0)
        return 0;

    if (v < 0)
    {
        i = my_ulong2dec(dst + 1, (unsigned long)(-v), n - 1);
        if (i != 0)
        {
            *dst = '-';
            return i + 1;
        }
        return i;
    }
    else
        return my_ulong2dec(dst, (unsigned long) v, n);
}
