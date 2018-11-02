EXTRA_DIST = win32.mak mingw.mak body.mak mingw-body.mak \
   config.h.win32 lconfig.h.win32 \
   config.mak.in config.mak README.txt \
   lconfig.h.win32 lconfig.h.win32.in \
   emdros.nsi.in emdros-logo-installer.bmp emdros.nsi 

CLEANFILES = *~ core config.mak emdros.nsi lconfig.h.win32

BUILT_SOURCES = config.mak emdros.nsi lconfig.h.win32


config.mak: config.mak.in Makefile
	$(top_builddir)/config.status --file=$(srcdir)/config.mak


emdros.nsi: emdros.nsi.in
	$(top_builddir)/config.status --file=$(srcdir)/emdros.nsi

lconfig.h.win32: lconfig.h.win32.in
	$(top_builddir)/config.status --file=$(srcdir)/lconfig.h.win32
