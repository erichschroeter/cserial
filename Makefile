# default target is...
all::

PREFIX:=/usr/local

CROSS_COMPILE=
CC=gcc
AR=ar
CFLAGS=-Wall -I. -L.

MAJOR=1
MINOR=2
PATCH=0
VERSION=$(MAJOR).$(MINOR).$(PATCH)
LIBNAME=libcserial
ifeq ($(OS),Windows_NT)
LIBDYNAMIC=$(LIBNAME).dll
else
LIBDYNAMIC=$(LIBNAME).so
endif

# Compiles the specified .c files to .o files.
%.o: %.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c -o $@ $<

# Compiles the specified .c files to .so files. These are used for
# compiling a shared library.
%.so: %.c
ifeq ($(OS),Windows_NT)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c -o $@ $<
else
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -fPIC -c -o $@ $<
endif

libstatic: serial_port.o
	$(AR) rcs $(LIBNAME).a $?

libdynamic: serial_port.so
ifeq ($(OS),Windows_NT)
	cat resource.rc.in | sed "s/@VERSION@/$(VERSION).0/g" | sed "s/@VERSION_COMMA@/$(MAJOR),$(MINOR),$(PATCH),0/g" > resource.rc
	windres -i resource.rc -o resource.o
	$(CROSS_COMPILE)$(CC) -o $(LIBDYNAMIC) $? resource.o -shared -Wl,--out-implib,$(LIBNAME).a -lpthread
else
	$(CROSS_COMPILE)$(CC) -shared -Wl,-soname,$(LIBNAME).so -o $(LIBDYNAMIC) $? -lpthread
endif

.PHONY: clean version

all:: libstatic libdynamic

install: all
	install -m 0755 $(LIBDYNAMIC) $(PREFIX)/lib
	ln -sf $(LIBDYNAMIC) $(PREFIX)/lib/$(LIBNAME).so

clean:
	rm -rf *.o *.so *.a *.dll *.exe resource.rc

version:
	@echo $(VERSION)
