CC = cc
LD = $(CC)
TARGETS = cryptserv
cryptserv_SOURCES = cryptserv.c

all: $(TARGETS)

rebuild: clean all

cryptserv: $(cryptserv_SOURCES:.c=.o)
	$(LD) $(MYLDFLAGS) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $(MYCFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f $(TARGETS)

