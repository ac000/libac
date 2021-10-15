SOVER	= 1
VERSION	= $(SOVER).99.0

CC	= gcc
CFLAGS  += -Wall -Wextra -Wdeclaration-after-statement -Wvla -std=gnu99 \
	   -g -O2 -fexceptions -fno-common -fvisibility=hidden \
	   -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -fPIC
LDFLAGS	+= -shared -Wl,-z,now,-z,defs,-z,relro,--as-needed
LIBS    += -lm -lcrypt

ifeq ($(CC),gcc)
        GCC_MAJOR  := $(shell gcc -dumpfullversion -dumpversion | cut -d . -f 1)
        GCC_MINOR  := $(shell gcc -dumpfullversion -dumpversion | cut -d . -f 2)
        GCC_SUB	   := $(shell gcc -dumpfullversion -dumpversion | cut -d . -f 3)
        GCC_VER_OK := $(shell test $(GCC_MAJOR) -ge 5 || test $(GCC_MAJOR) -ge 4 \
                        -a $(GCC_MINOR) -ge 9 && echo 1)

        ifeq ($(GCC_VER_OK),1)
                # Need GCC >= 4.9 for -fstack-protector-strong
                CFLAGS += -fstack-protector-strong
        endif
endif

ifeq ($(ASAN),1)
        override ASAN = -fsanitize=address
endif

UNAME_S := $(shell uname -s | tr A-Z a-z)
ifeq ($(UNAME_S),freebsd)
        CFLAGS 	+= -I/usr/local/include
        LIBS	:= -L/usr/local/lib $(LIBS) -luuid
endif

sources     =	$(wildcard platform/common/*.c platform/$(UNAME_S)/*.c *.c)
objects_all =	$(sources:.c=.o)
objects     =	$(filter-out test.o,$(objects_all))

v = @
ifeq ($V,1)
	v =
endif

libac: $(objects)
	@echo -e "  LNK\t$@"
	$(v)$(CC) $(LDFLAGS),-soname,libac.so.$(SOVER) -o libac.so.${VERSION} $(objects) $(LIBS)
	$(v)ln -sf libac.so.${VERSION} libac.so.$(SOVER)
	$(v)ln -sf libac.so.${VERSION} libac.so

$(objects): %.o: %.c
	@echo -e "  CC\t$@"
	$(v)$(CC) $(CFLAGS) -c -o $@ $<

test: test.c $(objects)
	@echo -e "  CCLNK\t$@"
	$(v)$(CC) $(CFLAGS) $(ASAN) -o $@ $< $(objects) $(LIBS)

clean:
	rm -f libac.so* *.o platform/*/*.o test
