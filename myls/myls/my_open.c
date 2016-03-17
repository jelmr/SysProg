#include "mysys.h"

/*
 *  NOTE: This piece of code contains inline assembly using the asm keyword.
 *  Since asm is not part of the actual C syntax, the compiler might give some
 *  warnings. Do not worry about any of the following errors:
 *
 *   - "implicit declaration of function 'asm' is invalid in C99"
 *   - "expected ')'"
 *   - "unused parameter"
 */
int my_open(const char *path, int oflag, int mode)
{
    long result;
    int  carry;

    carry = 0;

    __asm__(SYSCALL
        "jnc 1f ;"
        "movl $1, %[xc];"
        "1: ;"
            : "=a"(result), [xc] "=m" (carry)
            : REG_A(SYS_open), REG_B(path), REG_C(oflag), REG_D(mode));

    if (carry || result < 0)
        return 0;

    return (int)result;
}
