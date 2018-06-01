EXTRA_DIST = generate_extrafiles.py.in \
             emdros.spec.in emdros.spec


CLEANFILES = *~ generate_extrafiles.py extrafiles-emdros.txt extrafiles-emdros-devel.txt

DISTCLEANFILES = emdros.spec \
                 extrafiles-emdros.txt extrafiles-emdros-devel.txt 

BUILT_SOURCES = generate_extrafiles.py extrafiles-emdros.txt emdros.spec

extrafiles-emdros.txt: generate_extrafiles.py
	python generate_extrafiles.py

emdros.spec: emdros.spec.in  $(top_builddir)/config.status
	$(top_builddir)/config.status --file=$@
