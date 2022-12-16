DOCUMENT_NAME = EQTUsersGuide

include Makefile.inc


BOOKFILES = $(DOCUMENT_NAME).hhc $(DOCUMENT_NAME).hhk $(DOCUMENT_NAME).hhp 
EXTRA_DIST = Content.txt files.txt \
             $(PNGFILES_ALL) \
             $(BOOKFILES) \
             $(PNGFILES_ALL) \
             $(DOCUMENT_NAME).htb 1001.htm \
             buildhtb.py config.py config.py.in \
             $(DOCUMENT_NAME).pdf $(DOCUMENT_NAME).tex

BUILT_SOURCES = 1001.htm \
                $(DOCUMENT_NAME).hhc $(DOCUMENT_NAME).hhk $(DOCUMENT_NAME).htb \
                $(PNGFILES_FROM_XPM) $(DOCUMENT_NAME).pdf

$(DOCUMENT_NAME).htb: 1001.htm
	cd $(srcdir); zip $(DOCUMENT_NAME).htb *.htm *.png $(DOCUMENT_NAME).hhc $(DOCUMENT_NAME).hhk $(DOCUMENT_NAME).hhp 


1001.htm: $(DOCUMENT_NAME).hhp \
          $(srcdir)/config.py $(srcdir)/buildhtb.py \
          Content.txt files.txt \
          $(PNGFILES_ALL)
	  cd $(srcdir); $(PYTHON_PROGRAM) $(srcdir)/buildhtb.py --htb

$(DOCUMENT_NAME).hhc: 1001.htm

$(DOCUMENT_NAME).hhk: 1001.htm

$(DOCUMENT_NAME).tex: \
          $(srcdir)/config.py $(srcdir)/buildhtb.py \
          Content.txt files.txt \
          $(PNGFILES_ALL)
	cd $(srcdir) ; $(PYTHON_PROGRAM) $(srcdir)/buildhtb.py --latex


$(DOCUMENT_NAME).pdf: $(DOCUMENT_NAME).tex
	pdflatex $(DOCUMENT_NAME).tex > pdflatex-log.txt 2>&1


DISTCLEANFILES = *.htm $(srcdir)/../$(DOCUMENT_NAME).htb \
                  $(DOCUMENT_NAME).htb \
                  $(DOCUMENT_NAME).hhk \
                  $(DOCUMENT_NAME).hhc \
                  $(DOCUMENT_NAME).pdf \
                  $(DOCUMENT_NAME).toc \
                  $(DOCUMENT_NAME).tex \
                  $(DOCUMENT_NAME).aux \
                  $(DOCUMENT_NAME).out \
                  $(DOCUMENT_NAME).log \
                  $(PNGFILES_FROM_XPM) \
                  buildhtb.py \
                  config.py config.pyc config.log

CLEANFILES = *~ core \
             pdflatex-log.txt



qrytooldir = $(prefix)/share/emdros/qrytool
dist_qrytool_DATA = $(DOCUMENT_NAME).htb


%.png : ../../../wx/%.xpm
	convert $< $@
#	ppmmake 1,1,1 48 48 > /tmp/EQTEmdrosWhite.ppm
#	ALPHA=`echo $< | sed -e 's_xpm_pbmalpha_g'` ; xpmtoppm --alphaout=$$ALPHA $< > /tmp/EQTEmdrostmp.ppm
#	ALPHA=`echo $< | sed -e 's_xpm_pbmalpha_g'` ; pnmcomp -alpha=$$ALPHA /tmp/EQTEmdrostmp.ppm /tmp/EQTEmdrosWhite.ppm | pnmtopng > $@ ; rm -f $$ALPHA
#	rm -f /tmp/EQTEmdrostmp.ppm /tmp/EQTEmdrosWhite.ppm


$(srcdir)/config.py: $(srcdir)/config.py.in
	$(top_builddir)/config.status --file=$(srcdir)/config.py

$(srcdir)/buildhtb.py: $(top_srcdir)/doc/progref/buildhtb.py
	cp $(top_srcdir)/doc/progref/buildhtb.py $(srcdir)/buildhtb.py


