CC = cc
LD = $(CC)
lib_NAME = libminic.a
TARGETS = $(lib_NAME)
lib_SOURCES = $(wildcard my_*.c) $(wildcard f_*.c) $(wildcard st_*.c)


all: $(TARGETS)

rebuild: clean all

$(lib_NAME): $(lib_SOURCES:.c=.o)
	ar -cru $@ $^
	ranlib $@

%.o: %.c
	$(CC) $(MYCFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f $(lib_NAME)
