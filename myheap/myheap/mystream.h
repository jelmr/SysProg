#ifndef MYSTREAM_H
#define MYSTREAM_H
#ifndef ST_BUFFER_SIZE
#define ST_BUFFER_SIZE 1
#endif
typedef struct st
{
    int  fd;
    char *buf;
    long index_begin;
    long index_end;
    int  mode;
} st_t;

typedef enum
{
    PAD_ZERO_FILL,
    PAD_REGULAR,
    PAD_LEFT_ALIGN,
    PAD_NONE
} padding_mode_t;

st_t *st_open(const char *, const char *);
void st_close(st_t *);
unsigned st_read(st_t *, void *, unsigned);
unsigned st_write(st_t *, const void *, unsigned);
void st_flush(st_t *);
int st_gets(st_t *, char *, unsigned);
int st_puts(st_t *, char *);
int st_putchar(st_t *, int);
int st_getchar(st_t *);
char *st_getline(st_t *);
int st_printf(st_t *, const char *fmt, ...);
#endif
