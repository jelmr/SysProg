#include "my_conv.h"

static void remove_whitespace(const char **s);

long  my_strtol(const char *str, char **endptr, int base)
{
    int           sign;
    unsigned long result_long;
    const long    max_long = (long)(((unsigned long)(-1)) / 2);
    const long    min_long = -max_long - 1;

    remove_whitespace(&str);
    sign = 1;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }

    result_long = my_strtoul(str, endptr, base);

    if (sign == 1 && result_long > max_long)
        return max_long;
    else if (sign == -1 && result_long > (max_long - 1))
        return min_long;
    else
        return (sign * (long)result_long);
}

static void remove_whitespace(const char **s)
{
    while (**s == '\t' || **s == '\n' || **s == '\v' || **s == '\f'
            || **s == '\r' || **s == ' ')
        (*s)++;
}

