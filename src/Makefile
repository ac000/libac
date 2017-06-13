CC	= gcc
CFLAGS	= -Wall -Wextra -std=c99 -g -O2 -fvisibility=hidden -fstack-protector-strong -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -fPIC
LDFLAGS	= -shared -Wl,-z,now
LIBS    = -lm -lcrypt

objects = 	ac_btree.o \
		ac_cqueue.o \
		ac_fs.o \
		ac_geo.o \
		ac_misc.o \
		ac_net.o \
		ac_quark.o \
		ac_slist.o \
		ac_str.o \
		ac_time.o

libac: $(objects)
	@echo -e "  LNK\t$@"
	@$(CC) $(LDFLAGS),-soname,libac.so.0 -o libac.so.0.15.0 $(objects) $(LIBS)

ac_btree.o: ac_btree.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_cqueue.o: ac_cqueue.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_fs.o: ac_fs.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_geo.o: ac_geo.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_misc.o: ac_misc.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_net.o: ac_net.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_quark.o: ac_quark.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

ac_slist.o: ac_slist.c
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
	@$(CC) $(CFLAGS) -o $@ $< $(objects) $(LIBS)

clean:
	rm -f libac.so.* *.o test
