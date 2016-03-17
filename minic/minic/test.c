#include "my_strlen.h"
#include "my_strcmp.h"

int main(int c, char *a[])
{
    if (c == 2)
        return (int) my_strlen(a[1]);
    else if (c == 3)
        return my_strcmp(a[1], a[2]);
}
