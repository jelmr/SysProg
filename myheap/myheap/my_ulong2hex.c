#include "my_conv.h"

int my_ulong2hex(char *dst, unsigned long v, unsigned n)
{
    int digits;
    int i;

    digits = f_count_digits_ul(v, 16);

    if (n < (unsigned)digits)
        return 0;

    for (i = 0; i < digits; i++)
    {
        dst[digits - i - 1] = f_uint2char(v % 16);
        v /= 16;
    }

    return digits;
}
