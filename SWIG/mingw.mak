#
# Emdros Win32 Makefile for SWIG directory
#
# Makefile for MinGW


include ..\win32\mingw-config.mak

# We currently can't do SWIG bindings with mingw.
all:
#ifeq ($(WITH_SWIG_PYTHON),yes)
#	$(MAKE) -C python -f mingw.mak all
#endif
#ifeq ($(WITH_SWIG_JAVA),yes)
#	$(MAKE) -C java -f mingw.mak all
#endif
#ifeq ($(WITH_SWIG_CSHARP),yes)
#	$(MAKE) -C csharp -f mingw.mak all
#endif


# We currently can't do SWIG bindings with mingw.
clean:
#	$(MAKE) -C python -f mingw.mak clean
#	$(MAKE) -C java -f mingw.mak clean
#	$(MAKE) -C csharp -f mingw.mak clean

