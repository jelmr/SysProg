#include "my_conv.h"

unsigned my_hex2uint(const char *s)
{
    unsigned long  result_long;
    char           *ptr;
    const unsigned max_uint = (unsigned)(-1);

    result_long = my_strtoul(s, &ptr, 16);

    if (result_long > max_uint)
        return (unsigned) max_uint;
    else
        return (unsigned) result_long;
}
