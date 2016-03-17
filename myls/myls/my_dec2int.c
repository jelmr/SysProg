#include "my_conv.h"

int my_dec2int(const char *s)
{
    long      result_long;
    char      *ptr;
    const int max_int = (int)((unsigned)(-1) / 2);
    const int min_int = -max_int - 1;

    result_long = my_strtol(s, &ptr, 10);

    if (result_long > max_int)
        return max_int;
    else if (result_long < min_int)
        return min_int;
    else
        return (int)result_long;
}
