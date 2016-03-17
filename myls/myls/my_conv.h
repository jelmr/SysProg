#ifndef MY_CONV_H
#define MY_CONV_H
char f_uint2char(unsigned n);
int f_count_digits_i(int v, unsigned base);
int f_count_digits_u(unsigned v, unsigned base);
int f_count_digits_ul(unsigned long v, unsigned base);
int f_count_digits_l(long v, unsigned base);
int my_dec2int(const char *s);
unsigned int my_dec2uint(const char *s);
unsigned int my_hex2uint(const char *s);
int my_int2dec(char *dst, int v, unsigned n);
int my_long2dec(char *dst, long v, unsigned n);
int my_uint2dec(char *dst, unsigned v, unsigned n);
int my_ulong2dec(char *dst, unsigned long v, unsigned n);
int my_ulong2hex(char *dst, unsigned long v, unsigned n);
int my_uint2hex(char *dst, unsigned v);
long my_strtol(const char *str, char **endptr, int base);
unsigned long my_strtoul(const char *str, char **endptr, int base);
#endif
