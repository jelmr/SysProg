#include "my_conv.h"

int f_count_digits(unsigned v, unsigned base)
{
    int digits;

    digits = 1;

    while ((v /= base) != 0)
        digits++;

    return digits;
}
