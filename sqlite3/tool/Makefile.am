EXTRA_DIST = \
  diffdb.c \
  lemon.c \
  lempar.c \
  memleak2.awk \
  memleak3.tcl \
  memleak.awk \
  mkkeywordhash.c \
  mkopts.tcl \
  mksqlite3c.tcl \
  mksqlite3internalh.tcl \
  omittest.tcl \
  opcodeDoc.awk \
  showdb.c \
  showjournal.c \
  soak1.tcl \
  spaceanal.tcl \
  space_used.tcl \
  speedtest2.tcl \
  speedtest.tcl \
  genfkey.README \
  speedtest16.c \
  speedtest8inst1.c \
  mkspeedsql.tcl \
  genfkey.test \
  speedtest8.c \
  vdbe-compress.tcl





noinst_PROGRAMS = lemon

lemon_SOURCES = lemon.c

BUILT_SOURCES = $(srcdir)/../src/lempar.c 

$(srcdir)/../src/lempar.c: lempar.c
	cp $(srcdir)/lempar.c $(srcdir)/../src/



CLEANFILES=*~ core

