#include "my_strpbrk.h"

const char *my_strpbrk(const char *s1, const char *s2)
{
    unsigned i;
    unsigned j;

    i = 0;

    while (s1[i] != '\0')
    {
        j = 0;

        while (s2[j] != '\0')
        {
            if (s1[i] == s2[j])
                return (s1 + i);
            j++;
        }

        i++;
    }

    return 0;
}
