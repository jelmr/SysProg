#include "my_strstr.h"

const char *my_strstr(const char *s1, const char *s2)
{
    int i;
    int j;

    if (s2 == '\0')
        return s1;

    i = 0;

    while (s1[i] != '\0')
    {
        j = 0;

        while (s1[i + j] == s2[j] && s2[j] != '\0')
            j++;

        if (s2[j] == '\0')
            return (s1 + i);

        i++;
    }

    return 0;
}
