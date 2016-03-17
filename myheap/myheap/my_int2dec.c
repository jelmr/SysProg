#include "my_conv.h"

int my_int2dec(char *dst, int v, unsigned n)
{
    return my_long2dec(dst, (long)v, n);
}
