#include "my_conv.h"
#include "mystream.h"
#include "my_strlen.h"
#include "mysys.h"
#include <stdlib.h>
#include <stdarg.h>

static int process_directive(st_t *st, char **fmt, va_list ap);
static char *read_int(va_list ap);
static char *read_uint(va_list ap);
static char *read_ulong(va_list ap);
static char *read_long(va_list ap);
static char *read_pointer(va_list ap);
static char *read_hex_long(va_list ap);
static char *read_hex(va_list ap);
static char *read_char(va_list ap);
static padding_mode_t get_padding_mode(char **fmt, int *padding_count,
        va_list ap);
static int pad_and_print(st_t *st, char *s, padding_mode_t padding, int width);
static int pad_and_print_nr(st_t *st, char *s, padding_mode_t padding,
        int width);

int st_printf(st_t *st, const char *fmt, ...)
{
    va_list ap;
    int     bytes_written;

    bytes_written = 0;

    if ((st->mode & O_WRONLY) || (st->mode & O_RDWR))
    {
        va_start(ap, fmt);

        while (*fmt)
        {
            if (*fmt == '%' && ++fmt)
                bytes_written += process_directive(st, (char **)&fmt, ap);
            else
                bytes_written += st_putchar(st, *fmt++);
        }
        va_end(ap);
    }

    return bytes_written;
}

/**
* This function processes the directive, writes the result to st, and
* returns the amount of characters written to st.
*
* The limitation of 25 rules per method severely reduced the readability
* of this method.
*/
static int process_directive(st_t *st, char **fmt, va_list ap)
{
    int padding_count;

    padding_mode_t padding = get_padding_mode(fmt, &padding_count, ap);
    if (**fmt == 's' && (*fmt)++)
        return pad_and_print(st, va_arg(ap, char*), padding, padding_count);
    else if (**fmt == 'd' && (*fmt)++)
        return pad_and_print_nr(st, read_int(ap), padding, padding_count);
    else if (**fmt == 'u' && (*fmt)++)
        return pad_and_print(st, read_uint(ap), padding, padding_count);
    else if (**fmt == 'l' && (*fmt)[1] == 'u' && ((*fmt) += 2))
        return pad_and_print(st, read_ulong(ap), padding, padding_count);
    else if (**fmt == 'l' && (*fmt)[1] == 'd' && ((*fmt) += 2))
        return pad_and_print_nr(st, read_long(ap), padding, padding_count);
    else if (**fmt == 'p' && (*fmt)++)
        return pad_and_print(st, read_pointer(ap), padding, padding_count);
    else if (**fmt == 'l' && (*fmt)[1] == 'x' && ((*fmt) += 2))
        return pad_and_print(st, read_hex_long(ap), padding, padding_count);
    else if (**fmt == 'x' && (*fmt)++)
        return pad_and_print(st, read_hex(ap), padding, padding_count);
    else if (**fmt == 'c' && (*fmt)++)
        return pad_and_print(st, read_char(ap), padding, padding_count);
    else if (**fmt == '%' && (*fmt)++)
        return st_putchar(st, '%');
    return 0;
}

/**
* This function applies padding to string 'x' based on 'padding' and
* 'width'. It writes the resulting string to the stream identified by
* 'st' and returns the amount of characters written.
*/
static int pad_and_print(st_t *st, char *s, padding_mode_t padding, int width)
{
    int length;
    int i;

    length = (int)my_strlen(s);

    if (padding == PAD_ZERO_FILL)
        for (i = 0; i < width - length; ++i)
            st_putchar(st, '0');
    else if (padding == PAD_REGULAR)
        for (i = 0; i < width - length; ++i)
            st_putchar(st, ' ');
    st_puts(st, s);
    if (padding == PAD_LEFT_ALIGN)
        for (i = 0; i < width - length; ++i)
            st_putchar(st, ' ');

    return (padding != PAD_NONE && width > length) ? width : length;
}

/**
* This function has roughly the same functionality as 'pad_and_print', but
* is used for numbers that can be negative.
*
* This is a rather ugly fix to avoid printing the filling zeroes before the
* minus sign when printing negative numbers. A prettier solution would
* require rewriting a significant portion of the program.
*
* Note that a second padAndPrint method was needed, because otherwise strings
* starting with a '-' would be printed incorrectly.
*/
static int pad_and_print_nr(st_t *st, char *s, padding_mode_t padding,
        int width)
{
    int length;
    int i;

    length = (int)my_strlen(s);
    if (padding == PAD_ZERO_FILL)
    {
        if (*s == '-')
        {
            st_putchar(st, '-');
            for (i = 0; i < width - length; ++i)
                st_putchar(st, '0');
            s++;
        }
        else
            for (i = 0; i < width - length; ++i)
                st_putchar(st, '0');
    }
    else if (padding == PAD_REGULAR)
        for (i = 0; i < width - length; ++i)
            st_putchar(st, ' ');
    st_puts(st, s);
    if (padding == PAD_LEFT_ALIGN)
        for (i = 0; i < width - length; ++i)
            st_putchar(st, ' ');
    return (padding != PAD_NONE && width > length) ? width : length;
}

static padding_mode_t get_padding_mode(char **fmt, int *padding_count,
        va_list ap)
{
    if (**fmt == '0' && (*fmt)++)
    {
        *padding_count = (int)my_strtoul(*fmt, fmt, 10);
        return PAD_ZERO_FILL;
    }
    else if (**fmt == '-' && (*fmt)[1] == '*' && ((*fmt) += 2))
    {
        *padding_count = va_arg(ap, int);
        return PAD_LEFT_ALIGN;
    }
    else if (**fmt == '-' && (*fmt)++)
    {
        *padding_count = (int)my_strtoul(*fmt, fmt, 10);
        return PAD_LEFT_ALIGN;
    }
    else if (('0' < **fmt && **fmt <= '9'))
    {
        *padding_count = (int)my_strtoul(*fmt, fmt, 10);
        return PAD_REGULAR;
    }
    else if (**fmt == '*' && (*fmt)++)
    {
        *padding_count = va_arg(ap, int);
        return PAD_REGULAR;
    }
    else
    {
        *padding_count = 0;
        return PAD_NONE;
    }
}

static char *read_long(va_list ap)
{
    long     i;
    unsigned digits;
    char     *s;

    i = va_arg(ap, long);
    digits = (unsigned)f_count_digits_l(i, 10) + 1;
    s = malloc(sizeof (char) * (unsigned long)digits);
    my_long2dec(s, i, digits);
    return s;

}

static char *read_char(va_list ap)
{
    char *s;

    s = malloc(sizeof (char) * 2lu);
    s[0] = (char)va_arg(ap, int);
    s[1] = '\0';
    return s;
}

static char *read_hex_long(va_list ap)
{
    unsigned long i;
    unsigned      digits;
    char          *s;

    i = va_arg(ap, unsigned long);
    digits = (unsigned)f_count_digits_ul(i, 16) + 1;
    s = malloc(sizeof (char) * (unsigned long)digits);
    my_ulong2hex(s, i, digits);
    return s;
}

static char *read_hex(va_list ap)
{
    unsigned i;
    unsigned digits;
    char     *s;

    i = va_arg(ap, unsigned);
    digits = (unsigned)f_count_digits_u(i, 16) + 1;
    s = malloc(sizeof (char) * digits);
    my_ulong2hex(s, (unsigned long)i, digits);
    return s;
}



static char *read_pointer(va_list ap)
{
    unsigned long i;
    unsigned      digits;
    char          *s;

    i = (unsigned long)va_arg(ap, void *);

    digits = (unsigned)f_count_digits_ul(i, 16) + 2;
    s = malloc(sizeof (char) * (unsigned long)digits);
    s[0] = '0';
    s[1] = 'x';
    my_ulong2hex(s + 2, i, digits);
    s[digits] = '\0';
    return s;
}

static char *read_ulong(va_list ap)
{
    unsigned long i;
    unsigned      digits;
    char          *s;

    i = va_arg(ap, unsigned long);
    digits = (unsigned)f_count_digits_ul(i, 10) + 1;
    s = malloc(sizeof (char) * (unsigned long)digits);
    my_ulong2dec(s, i, digits);
    return s;
}

static char *read_int(va_list ap)
{
    int      i;
    unsigned digits;
    char     *s;

    i = va_arg(ap, int);
    digits = (unsigned)f_count_digits_i(i, 10) + 1;
    s = malloc(sizeof (char) * (unsigned long)digits);
    my_long2dec(s, (long)i, digits);
    return s;
}


static char *read_uint(va_list ap)
{
    unsigned i;
    unsigned digits;
    char     *s;

    i = va_arg(ap, unsigned);
    digits = (unsigned)f_count_digits_u(i, 10) + 1;
    s = malloc(sizeof (char) * (unsigned long)digits);
    my_ulong2dec(s, (unsigned long)i, digits);
    return s;
}

