lib_LTLIBRARIES = 

if HOSTISDARWIN
MACLDFLAGS = -static
endif

if USE_LOCAL_PCRE
  lib_LTLIBRARIES += libpcre_emdros.la
  DO_MACDIST = cp -f .libs/libpcre_emdros.a $(MACDISTDIR)/development/lib/
endif

libpcre_emdros_la_SOURCES =  \
  pcre_byte_order.c \
  pcre_chartables.c \
  pcre_compile.c \
  pcre_config.c \
  pcre_dfa_exec.c \
  pcre_exec.c \
  pcre_fullinfo.c \
  pcre_get.c \
  pcre_globals.c \
  pcre_jit_compile.c \
  pcre_maketables.c \
  pcre_newline.c \
  pcre_ord2utf8.c \
  pcre_refcount.c \
  pcre_string_utils.c \
  pcre_study.c \
  pcre_tables.c \
  pcre_ucd.c \
  pcre_valid_utf8.c \
  pcre_version.c \
  pcre_xclass.c 




libpcre_emdros_la_LDFLAGS = $(MACLDFLAGS) -version-info \
                     @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                     @SUN_LDFLAGS@
BUILT_SOURCES = $(srcdir)/../include/pcre_config.h pcre_chartables.c 

noinst_PROGRAMS = dftables
dftables_SOURCES = dftables.c
dftables_DEPENDENCIES = $(srcdir)/../include/pcre_config.h

noinst_HEADERS = config.h  pcre.h  pcre_internal.h  ucp.h  


CLEANFILES = $(srcdir)/../include/pcre.h $(srcdir)/../include/pcre_config.h pcre_chartables.c *~ core

EXTRA_DIST = win32.mak mingw.mak LICENCE NON-UNIX-USE README AUTHORS \
             COPYING NEWS \
             config.h.win32 pcre_emdros.h.win32

AM_CPPFLAGS = 
AM_CFLAGS = @DEBUGFLAG@ @ADDITIONAL_CFLAGS@




pcre_compile.c: pcre_chartables.c

pcre_chartables.c: $(srcdir)/../include/pcre_config.h  dftables
	./dftables pcre_chartables.c

#dftables$(EXEEXT):
#	$(COMPILE) $(CFLAGS) -c dftables.c
#	$(LINK) -o dftables dftables.o

$(srcdir)/../include/pcre_config.h: config.h
	cp -pf config.h $(srcdir)/../include/pcre_config.h


macdist:	
	$(DO_MACDIST)
