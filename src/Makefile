VERSION	= 0.20.0

CC	= gcc
CFLAGS  = -Wall -Wextra -std=c99 -g -O2 -fvisibility=hidden -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -fPIC
LDFLAGS	= -shared -Wl,-z,now
LIBS    = -lm -lcrypt

GLIBC_MAJOR	:= $(shell ldd --version | grep -Eo '[0-9]+\.[0-9]+' | \
                           cut -d . -f 1)
GLIBC_MINOR	:= $(shell ldd --version | grep -Eo '[0-9]+\.[0-9]+' | \
                           cut -d . -f 2)
GLIBC_VER_OK	:= $(shell test $(GLIBC_MAJOR) -ge 2 && \
                           test $(GLIBC_MINOR) -ge 17 && \
                           echo 1)

GCC_MAJOR	:= $(shell gcc -dumpversion | cut -d . -f 1)
GCC_MINOR	:= $(shell gcc -dumpversion | cut -d . -f 2)
GCC_SUB		:= $(shell gcc -dumpversion | cut -d . -f 3)
GCC_VER_OK	:= $(shell test $(GCC_MAJOR) -ge 5 || test $(GCC_MAJOR) -ge 4 \
                           -a $(GCC_MINOR) -ge 9 && echo 1)

ifneq "$(GLIBC_VER_OK)" "1"
        # clock_* functions need linking against -lrt in glibc < 2.17
        LIBS += -lrt
endif

ifeq "$(GCC_VER_OK)" "1"
        # Need GCC >= 4.9 for -fstack-protector-strong
        CFLAGS += -fstack-protector-strong
endif

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
	@$(CC) $(LDFLAGS),-soname,libac.so.0 -o libac.so.${VERSION} $(objects) $(LIBS)

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
