#include "my_conv.h"

int my_uint2dec(char *dst, unsigned v, unsigned n)
{
    int digits;
    int i;

    digits = f_count_digits(v, 10);

    if (n < (unsigned)digits)
        return 0;

    for (i = 0; i < digits; i++)
    {
        dst[digits - i - 1] = f_uint2char(v % 10);
        v /= 10;
    }

    return digits;
}
