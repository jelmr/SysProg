#include "my_strlen.h"

unsigned long my_strlen(const char *s)
{
    unsigned long length;

    length = 0;

    while (s[length] != 0)
        length++;

    return length;
}
