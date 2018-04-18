TARGET = libpcre.a

libpcre_SOURCES = \
  pcre_chartables.c \
  pcre_compile.c \
  pcre_config.c \
  pcre_dfa_exec.c \
  pcre_exec.c \
  pcre_fullinfo.c \
  pcre_get.c \
  pcre_globals.c \
  pcre_info.c \
  pcre_maketables.c \
  pcre_ord2utf8.c \
  pcre_printint.c \
  pcre_refcount.c \
  pcre_study.c \
  pcre_tables.c \
  pcre_try_flipped.c \
  pcre_ucp_findchar.c \
  pcre_valid_utf8.c \
  pcre_version.c \
  pcre_xclass.c 

libpcre_OBJECTS = $(libpcre_SOURCES:.c=.o)

CLEANFILES = ..\include\pcre_emdros.h chartabl.c *.O $(TARGET)

CFLAGS=-I..\include -I.  -O2  


all: chartabl.c $(TARGET) ..\include\pcre_emdros.h

libpcre.a: $(libpcre_OBJECTS)
	ar r libpcre.a $(libpcre_OBJECTS)

..\include\pcre_emdros.h: pcre.h
	copy pcre.h ..\include\pcre_emdros.h

pcre.o: pcre.c

pcre.c: chartabl.c

chartabl.c: dftables.exe
	.\dftables.exe chartabl.c

dftables.exe: dftables.o
	gcc -o dftables.exe dftables.o


clean:
	for %f in ($(CLEANFILES)) DO del %f


