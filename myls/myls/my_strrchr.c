#include "my_strrchr.h"

const char *my_strrchr(const char *s, int c)
{
    int  i;
    char *result;

    i = 0;
    result = 0;

    while (s[i] != '\0')
    {
        if (s[i] == c)
            result = (char *)(s + i);

        i++;
    }

    return (const char *)result;
}
