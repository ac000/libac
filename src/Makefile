VERSION	= 0.28.6

CC	= gcc
CFLAGS  = -Wall -Wextra -Wdeclaration-after-statement -Wvla -std=c99 -g -O2 -fexceptions -fvisibility=hidden -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -fPIC
LDFLAGS	= -fpic -shared -Wl,-z,now,-z,defs,-z,relro,--as-needed
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

sources     =	$(wildcard *.c)
objects_all =	$(sources:.c=.o)
objects     =	$(filter-out test.o,$(objects_all))

libac: $(objects)
	@echo -e "  LNK\t$@"
	@$(CC) $(LDFLAGS),-soname,libac.so.0 -o libac.so.${VERSION} $(objects) $(LIBS)

$(objects): %.o: %.c
	@echo -e "  CC\t$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

test: test.c $(objects)
	@echo -e "  CCLNK\t$@"
	@$(CC) $(CFLAGS) -o $@ $< $(objects) $(LIBS)

clean:
	rm -f libac.so.* *.o test
