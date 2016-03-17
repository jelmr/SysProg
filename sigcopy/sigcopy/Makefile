CC = cc
LD = $(CC)
TARGETS = sigsend sigrecv
sigrecv_SOURCES = sigrecv.c $(lib_NAME)
sigsend_SOURCES = sigsend.c $(lib_NAME)

all: $(TARGETS)

rebuild: clean all

sigsend: $(sigsend_SOURCES:.c=.o)
	$(LD) $(MYLDFLAGS) $(LDFLAGS) $< -o $@

sigrecv: $(sigrecv_SOURCES:.c=.o)
	$(LD) $(MYLDFLAGS) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $(MYCFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f $(TARGETS)
