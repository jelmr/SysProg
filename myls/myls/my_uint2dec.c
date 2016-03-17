#include "my_conv.h"

int my_uint2dec(char *dst, unsigned v, unsigned n)
{
    return my_ulong2dec(dst, (unsigned long)v, n);
}
