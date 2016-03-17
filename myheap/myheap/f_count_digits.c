#include "my_conv.h"

int f_count_digits_i(int v, unsigned base)
{
    return f_count_digits_l((long)v, base);
}

int f_count_digits_u(unsigned v, unsigned base)
{
    return f_count_digits_ul((unsigned long)v, base);
}

int f_count_digits_ul(unsigned long v, unsigned base)
{
    int digits;

    digits = 1;

    while ((v /= base) != 0)
        digits++;

    return digits;
}

int f_count_digits_l(long v, unsigned base)
{
    int digits;

    digits = 1;

    while ((v /= base) != 0)
        digits++;

    return digits;
}
