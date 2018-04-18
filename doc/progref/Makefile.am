XPMFILES = 

PNGFILES_FROM_XPM = $(XPMFILES:.xpm=.png)

EMDROS_PROGREF_GUIDE_HTML_DIR = emdros-progref-guide-@VERSION@

include Makefile.inc

ALL_HTML_FILES = $(HTML_FILES) 1001.html

BOOKFILES = EmdrosProgRefGuide.hhc \
            EmdrosProgRefGuide.hhk \
            EmdrosProgRefGuide.hhp 

EXTRA_DIST = Content.txt files.txt \
             $(BOOKFILES) \
             $(PNGFILES_FROM_XPM) \
             Makefile.inc \
             EmdrosProgRefGuide.htb \
             1001.htm $(HTML_FILES) 1001.html \
             buildhtb.py \
             EmdrosProgRefGuide.pdf \
             EmdrosProgRefGuide.tex \
             search.php cms.css dot.gif \
             config.py.in config.py \
             $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip

BUILT_SOURCES = EmdrosProgRefGuide.hhc \
                EmdrosProgRefGuide.hhk \
                $(PNGFILES_FROM_XPM) \
                EmdrosProgRefGuide.htb \
                1001.htm 1001.html $(HTML_FILES) \
                EmdrosProgRefGuide.pdf \
                EmdrosProgRefGuide.tex \
                config.py \
                $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip

progrefdir = ${docdir}/emdros-progref-guide.html

dist_progref_DATA = $(ALL_HTML_FILES) $(PNGFILES_FROM_XPM) dot.gif cms.css 

Makefile.inc: files.txt
	rm -f Makefile.inc
	echo "HTML_FILES = \\" > $@
	$(AWK) '{print "    " $$1 ".html \\";}' files.txt | $(GREP) -v "1001.html" >> $@
	echo "    index.html" >> $@

EmdrosProgRefGuide.htb: 1001.htm
	cd $(srcdir); zip EmdrosProgRefGuide.htb *.htm *.png EmdrosProgRefGuide.hhc EmdrosProgRefGuide.hhk EmdrosProgRefGuide.hhp 


1001.htm: config.py EmdrosProgRefGuide.hhp buildhtb.py \
          Content.txt files.txt \
          $(PNGFILES_FROM_XPM)
	  cd $(srcdir); python $(srcdir)/buildhtb.py --htb

EmdrosProgRefGuide.hhc: 1001.htm

EmdrosProgRefGuide.hhk: 1001.htm

EmdrosProgRefGuide.tex: config.py buildhtb.py \
                        Content.txt files.txt \
                        $(PNGFILES_FROM_XPM)
	cd $(srcdir); python $(srcdir)/buildhtb.py --latex

EmdrosProgRefGuide.pdf: EmdrosProgRefGuide.tex
	pdflatex EmdrosProgRefGuide.tex


DISTCLEANFILES = \
             config.py config.pyc config.log \
             EmdrosProgRefGuide.aux \
             EmdrosProgRefGuide.out \
             EmdrosProgRefGuide.tex \
             EmdrosProgRefGuide.toc \
             EmdrosProgRefGuide.log \
             EmdrosProgRefGuide.htb \
             EmdrosProgRefGuide.hhk \
             EmdrosProgRefGuide.hhc \
             EmdrosProgRefGuide.pdf \
             EmdrosProgRefGuide.tex \
             $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip \
             $(PNGFILES_FROM_XPM) \
             *.htm $(ALL_HTML_FILES)

CLEANFILES = *~ core 


%.png : ../../../wx/%.xpm
	ppmmake white 48 48 > /tmp/EmdrosProgRefGuideWhite.ppm
	ALPHA=`echo $< | sed -e 's_xpm_pbmalpha_g'` ; xpmtoppm --alphaout=$$ALPHA $< > /tmp/EmdrosProgRefGuidetmp.ppm
	ALPHA=`echo $< | sed -e 's_xpm_pbmalpha_g'` ; pnmcomp -alpha=$$ALPHA /tmp/EmdrosProgRefGuidetmp.ppm /tmp/EmdrosProgRefGuideWhite.ppm | pnmtopng > $@ ; rm -f $$ALPHA
	rm -f /tmp/EmdrosProgRefGuidetmp.ppm /tmp/EmdrosProgRefGuideWhite.ppm


config.py: config.py.in
	$(top_srcdir)/config.status --file=$(srcdir)/config.py 

1001.html: config.py buildhtb.py \
          Content.txt files.txt \
          $(PNGFILES_FROM_XPM)
	cd $(srcdir); python $(srcdir)/buildhtb.py --web
	cp 1001.html index.html

index.html: 1001.html

$(HTML_FILES): 1001.html

$(EMDROS_PROGREF_GUIDE_HTML_DIR).zip: $(ALL_HTML_FILES)
	-rm -rf $(EMDROS_PROGREF_GUIDE_HTML_DIR)
	mkdir $(EMDROS_PROGREF_GUIDE_HTML_DIR)
	cp $(ALL_HTML_FILES) $(EMDROS_PROGREF_GUIDE_HTML_DIR)/
	cp Content.txt files.txt search.php cms.css dot.gif $(EMDROS_PROGREF_GUIDE_HTML_DIR)/
	zip -r $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip $(EMDROS_PROGREF_GUIDE_HTML_DIR)/
	rm -rf $(EMDROS_PROGREF_GUIDE_HTML_DIR)

macdist: $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip EmdrosProgRefGuide.pdf
	cp EmdrosProgRefGuide.pdf $(MACDISTDIR)/doc/PDF
	cp $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip $(MACDISTDIR)/doc/html

