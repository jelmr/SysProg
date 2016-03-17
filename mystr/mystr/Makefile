CC = cc
LD = $(CC)
lib_NAME = libminic.a
TARGETS = $(lib_NAME)
lib_SOURCES = my_strchr.c my_strlcpy.c my_strlcat.c my_strlen.c my_strncmp.c
lib_SOURCES += my_strcmp.c my_memcpy.c my_memmove.c my_memset.c my_strrchr.c
lib_SOURCES += my_strpbrk.c my_strstr.c

all: $(TARGETS)

rebuild: clean all

$(lib_NAME): $(lib_SOURCES:.c=.o)
	ar -cvq $@ $^

%.o: %.c
	$(CC) $(MYCFLAGS) $(CFLAGS) -c -o $@ $^

clean:
	rm -f *.o
	rm -f $(TARGETS)
	rm -f $(lib_NAME)
