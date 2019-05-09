EXTRA_DIST = win32.mak mingw.mak body.mak mingw-body.mak \
   config.h.win32 \
   config.mak.in config.mak \
   mingw-config.sed mingw-config.mak \
   README.txt \
   emdros.nsi.in emdros-logo-installer.bmp emdros.nsi 

CLEANFILES = *~ core config.mak emdros.nsi 

BUILT_SOURCES = config.mak emdros.nsi mingw-config.mak


config.mak: config.mak.in Makefile
	$(top_builddir)/config.status --file=$(srcdir)/config.mak


mingw-config.mak: config.mak
	sed -f mingw-config.sed $< > $@

emdros.nsi: emdros.nsi.in
	$(top_builddir)/config.status --file=$(srcdir)/emdros.nsi

