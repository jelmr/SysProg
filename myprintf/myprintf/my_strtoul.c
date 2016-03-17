#include "my_conv.h"

static int is_digit(char c);
static int is_lowercase(char c);
static int is_uppercase(char c);
static void remove_whitespace(const char **s);
static int determine_base(const char **s, int base);
static unsigned char2uint(const char c);

unsigned long my_strtoul(const char *str, char **endptr, int base)
{
    unsigned long       result;
    unsigned long       limit;
    unsigned long       rest_limit;
    unsigned            next_digit;
    const unsigned long max_ulong = (unsigned long)(-1);

    result = 0;
    remove_whitespace(&str);
    base = determine_base(&str, base);

    while (is_digit(*str) || is_lowercase(*str) || is_uppercase(*str))
    {
        next_digit = char2uint(*str);
        if (next_digit >= (unsigned)base)
            break;
        str++;
        limit = max_ulong / (unsigned long)base;
        rest_limit = max_ulong % (unsigned long)base;

        if (result > limit || (result == limit && next_digit > rest_limit))
            result = max_ulong;
        else
            result = result * (unsigned long)base + next_digit;
    }
    *endptr = (result == 0) ? (char *)(str - 1) : (char *)str;
    return result;
}

static int determine_base(const char **s, int base)
{
    if (**s == '+')
        (*s)++;

    if ((base == 0 || base == 16) && **s == '0' && *(*s + 1) == 'x')
    {
        *s += 2;
        return 16;
    }
    else if (base == 0 && **s == '0')
    {
        s++;
        return 8;
    }
    else if (base == 0)
    {
        return 10;
    }
    else
    {
        return base;
    }
}

static void remove_whitespace(const char **s)
{
    while (**s == '\t' || **s == '\n' || **s == '\v' || **s == '\f'
            || **s == '\r' || **s == ' ')
        (*s)++;
}

static int is_digit(char c)
{
    return ('0' <= c && c <= '9');
}

static int is_lowercase(char c)
{
    return ('a' <= c && c <= 'z');
}

static int is_uppercase(char c)
{
    return ('A' <= c && c <= 'Z');
}

static unsigned char2uint(const char c)
{
    if (is_digit(c))
        return (unsigned)(c - '0');
    else if (is_lowercase(c))
        return (unsigned)(c - 'a' + 10);
    else if (is_uppercase(c))
        return (unsigned)(c - 'A' + 10);
    return 0;
}
