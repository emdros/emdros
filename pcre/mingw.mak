#
# Emdros Win32 Makefile for pcre directory
#
# Makefile for MinGW

# Will build a Win32 library libpcre_emdros.a

WIN32DIR = ..\win32


LOCAL_CPPFLAGS = -I. -DPCRE_STATIC -DHAVE_CONFIG_H

ifeq ("$(BUILD_FOR_DEVEL)","1")
LIBTARGET = libpcre_emdros.a
TARGETS = pcre_config_h "$(OUTDIR)\$(LIBTARGET)"
!ELSE
LIBTARGET = 
TARGETS = pcre_config_h 
!ENDIF

CLEANFILES = chartables.c dftables.exe pcre_config.h

LIBTARGET_OBJS= \
  pcre_byte_order.o \
  pcre_chartables.o \
  pcre_compile.o \
  pcre_config.o \
  pcre_dfa_exec.o \
  pcre_exec.o \
  pcre_fullinfo.o \
  pcre_get.o \
  pcre_globals.o \
  pcre_jit_compile.o \
  pcre_maketables.o \
  pcre_newline.o \
  pcre_ord2utf8.o \
  pcre_refcount.o \
  pcre_string_utils.o \
  pcre_study.o \
  pcre_tables.o \
  pcre_ucd.o \
  pcre_valid_utf8.o \
  pcre_version.o \
  pcre_xclass.o 



CPROGRAM1 = .\dftables.exe
CPROGRAM1_OBJS = dftables.o


include $(WIN32DIR)\config.mak
include $(WIN32DIR)\mingw-body.mak


pcre_maketables.c: pcre_chartables.c

pcre_chartables.c: .\dftables.exe
	.\dftables.exe pcre_chartables.c

pcre_config_h:
        copy /Y /B config.h.win32 .\config.h
