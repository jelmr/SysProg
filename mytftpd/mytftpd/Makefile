CC = cc
LD = $(CC)
TARGETS = mytftpd
mytftpd_SOURCES = mytftpd.c

all: $(TARGETS)

rebuild: clean all

mytftpd: $(mytftpd_SOURCES:.c=.o)
	$(LD) $(MYLDFLAGS) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $(MYCFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f $(TARGETS)
