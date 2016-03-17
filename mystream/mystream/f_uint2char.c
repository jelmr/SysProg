#include "my_conv.h"

char f_uint2char(unsigned n)
{
    if (n <= 9)
        return (char)(n + '0');
    else if (10 <= n && n <= 36)
        return (char)(n - 10 + 'A');
    return '\0';
}
