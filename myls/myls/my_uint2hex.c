#include "my_conv.h"

int my_uint2hex(char *dst, unsigned v)
{
    int digits;
    int i;

    digits = f_count_digits_u(v, 16);

    for (i = 0; i < digits; i++)
    {
        dst[digits - i - 1] = f_uint2char(v % 16);
        v /= 16;
    }

    return digits;
}
