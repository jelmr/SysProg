#include "my_strncmp.h"

int my_strncmp(const char *s1, const char *s2, unsigned n)
{
    unsigned i;

    if (n == 0)
        return 0;

    i = 0;

    while (i < (n - 1) && s1[i] != 0 && s2[i] != 0 && s1[i] == s2[i])
        i++;

    if (s1[i] < s2[i])
        return -1;
    else if (s1[i] > s2[i])
        return 1;
    else
        return 0;
}
