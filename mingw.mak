# Emdros makefile for source root directory

# Makefile for MinGW

all:
	$(MAKE) -C build/windows -f mingw.mak $(MAKEMACRO)


clean:
	$(MAKE) -C build/windows -f mingw.mak clean


distclean: clean
	$(MAKE) -C build/windows -f mingw.mak distclean
