EXTRA_DIST = windows.mak mingw.mak body.mak mingw-body.mak \
   config.h.win32 lconfig.h.win32 \
   config.mak README.txt \
   emdros.nsi.in emdros-logo-installer.bmp emdros.nsi

CLEANFILES = *~ core emdros.nsi

BUILT_SOURCES = emdros.nsi


emdros.nsi: emdros.nsi.in
	$(top_builddir)/config.status --file=$(srcdir)/emdros.nsi
