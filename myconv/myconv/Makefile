CC = cc
LD = $(CC)
lib_NAME = libminic.a
TARGETS = $(lib_NAME)
lib_SOURCES = my_strchr.c my_strlcpy.c my_strlcat.c my_strlen.c my_strncmp.c
lib_SOURCES += my_strcmp.c my_memcpy.c my_memmove.c my_memset.c my_strrchr.c
lib_SOURCES += my_strpbrk.c my_strstr.c my_dec2int.c my_dec2uint.c
lib_SOURCES += my_hex2uint.c my_int2dec.c my_uint2dec.c my_strtoul.c
lib_SOURCES += f_uint2char.c my_uint2hex.c my_strtol.c f_count_digits.c

all: $(TARGETS)

rebuild: clean all

$(lib_NAME): $(lib_SOURCES:.c=.o)
	ar -cru $@ $^
	ranlib $@

%.o: %.c
	$(CC) $(MYCFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f $(TARGETS)
