#
# Emdros Win32 Makefile for SWIG directory
#
# Makefile for MinGW


include ..\win32\config.mak

all:
ifeq ($(WITH_SWIG_PYTHON),yes)
	$(MAKE) -C python -f mingw.mak all
endif
ifeq ($(WITH_SWIG_JAVA),yes)
	$(MAKE) -C java -f mingw.mak all
endif
ifeq ($(WITH_SWIG_CSHARP),yes)
	$(MAKE) -C csharp -f mingw.mak all
endif


clean:
	$(MAKE) -C python -f mingw.mak clean
	$(MAKE) -C java -f mingw.mak clean
	$(MAKE) -C csharp -f mingw.mak clean

