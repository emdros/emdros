# Emdros makefile for sqlite3 directory

# Makefile for MinGW

ifeq ($(OS), Windows_NT)
NULL=
else 
NULL=nul
endif

$(info Building sqlite3 on Win32)
$(info)

include ..\win32\mingw-config.mak

all: 
	$(MAKE) -C src -f mingw.mak 

CLEANFILES = *~

clean:
	$(MAKE) -C src -f mingw.mak clean
	-@erase $(CLEANFILES)


distclean: clean


