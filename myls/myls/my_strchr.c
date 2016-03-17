#include "my_strchr.h"

const char *my_strchr(const char *s, int c)
{
    int i;

    i = 0;

    while (s[i] != c && s[i])
        i++;

    if (s[i] == c)
        return (s + i);
    else
        return 0;
}
