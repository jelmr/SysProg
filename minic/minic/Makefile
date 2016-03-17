CC = cc
LD = $(CC)
lib_NAME = libminic.a
TARGETS = test
test_SOURCES = test.c $(lib_NAME)
lib_SOURCES = my_strlen.c my_strcmp.c

all: $(TARGETS)

rebuild: clean all

test: $(test_SOURCES:.c=.o)
	$(LD) $(MYLDFLAGS) $(LDFLAGS) -L. -lminic -o $@ $<

$(lib_NAME): $(lib_SOURCES:.c=.o)
	ar -cvq $@ $^

%.o: %.c
	$(CC) $(MYCFLAGS) $(CFLAGS) -c -o $@ $^

clean:
	rm -f *.o
	rm -f $(TARGETS)
	rm -f $(lib_NAME)
