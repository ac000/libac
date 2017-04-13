CC	= gcc
CFLAGS	= -Wall -std=c99 -g -O2 -fstack-protector-strong -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -fPIC
LDFLAGS	= -shared -Wl,-z,now

objects = 	ac_btree.o \
		ac_str.o \
		ac_time.o

libac: $(objects)
	@echo -e "  LNK\t$@"
	@$(CC) $(LDFLAGS),-soname,libac.so.0.2 -o libac.so.0.2.0 $(objects)

ac_btree.o: ac_btree.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_str.o: ac_str.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_time.o: ac_time.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

test: test.c $(objects)
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -o $@ $< $(objects)

clean:
	rm -f libac.so.* *.o test
