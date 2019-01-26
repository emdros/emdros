EMDROS_PROGREF_GUIDE_HTML_DIR = emdros-progref-guide-@VERSION@

include Makefile.inc

if DO_PROG_PDFLATEX
MAYBE_PDF = EmdrosProgRefGuide.pdf
else
MAYBE_PDF =
endif

if DO_PROG_ZIP
MAYBE_ZIP = EmdrosProgRefGuide.htb  $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip
else
MAYBE_ZIP = 
endif

ALL_HTML_FILES = $(HTML_FILES) 1001.html

BOOKFILES = EmdrosProgRefGuide.hhc \
            EmdrosProgRefGuide.hhk \
            EmdrosProgRefGuide.hhp 

EXTRA_DIST = Content.txt files.txt \
             $(BOOKFILES) \
             Makefile.inc \
             $(MAYBE_ZIP) \
             1001.htm $(HTML_FILES) 1001.html \
             buildhtb.py \
             $(MAYBE_PDF) \
             EmdrosProgRefGuide.tex \
             search.php cms.css dot.gif \
             config.py.in config.py 

BUILT_SOURCES = EmdrosProgRefGuide.hhc \
                EmdrosProgRefGuide.hhk \
                EmdrosProgRefGuide.tex \
                1001.htm 1001.html $(HTML_FILES) \
                config.py \
                $(MAYBE_ZIP) \
                $(MAYBE_PDF)

progrefdir = ${docdir}/emdros-progref-guide.html

dist_progref_DATA = $(ALL_HTML_FILES) dot.gif cms.css 

Makefile.inc: files.txt
	rm -f Makefile.inc
	echo "HTML_FILES = \\" > $@
	$(AWK) '{print "    " $$1 ".html \\";}' files.txt | $(GREP) -v "1001.html" >> $@
	echo "    index.html" >> $@

EmdrosProgRefGuide.htb: 1001.htm
	cd $(srcdir); $(PROG_ZIP) -q EmdrosProgRefGuide.htb *.htm *.png EmdrosProgRefGuide.hhc EmdrosProgRefGuide.hhk EmdrosProgRefGuide.hhp 


1001.htm: config.py EmdrosProgRefGuide.hhp buildhtb.py \
          Content.txt files.txt
	cd $(srcdir); python $(srcdir)/buildhtb.py --htb

EmdrosProgRefGuide.hhc: 1001.htm

EmdrosProgRefGuide.hhk: 1001.htm

EmdrosProgRefGuide.tex: config.py buildhtb.py \
                        Content.txt files.txt 
	cd $(srcdir); python $(srcdir)/buildhtb.py --latex

EmdrosProgRefGuide.pdf: EmdrosProgRefGuide.tex
	$(PROG_PDFLATEX) EmdrosProgRefGuide.tex > pdflatex-log.txt 2>&1


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
             *.htm $(ALL_HTML_FILES)

CLEANFILES = *~ core \
             pdflatex-log.txt



#%.png : ../../../wx/%.xpm
#	ppmmake white 48 48 > /tmp/EmdrosProgRefGuideWhite.ppm
#	ALPHA=`echo $< | sed -e 's_xpm_pbmalpha_g'` ; xpmtoppm --alphaout=$$ALPHA $< > /tmp/EmdrosProgRefGuidetmp.ppm
#	ALPHA=`echo $< | sed -e 's_xpm_pbmalpha_g'` ; pnmcomp -alpha=$$ALPHA /tmp/EmdrosProgRefGuidetmp.ppm /tmp/EmdrosProgRefGuideWhite.ppm | pnmtopng > $@ ; rm -f $$ALPHA
#	rm -f /tmp/EmdrosProgRefGuidetmp.ppm /tmp/EmdrosProgRefGuideWhite.ppm


config.py: config.py.in
	$(top_srcdir)/config.status --file=$(srcdir)/config.py 

1001.html: config.py buildhtb.py \
          Content.txt files.txt
	cd $(srcdir); python $(srcdir)/buildhtb.py --web
	cp 1001.html index.html

index.html: 1001.html

$(HTML_FILES): 1001.html

$(EMDROS_PROGREF_GUIDE_HTML_DIR).zip: $(ALL_HTML_FILES)
	-rm -rf $(EMDROS_PROGREF_GUIDE_HTML_DIR)
	mkdir $(EMDROS_PROGREF_GUIDE_HTML_DIR)
	cp $(ALL_HTML_FILES) $(EMDROS_PROGREF_GUIDE_HTML_DIR)/
	cp Content.txt files.txt search.php cms.css dot.gif $(EMDROS_PROGREF_GUIDE_HTML_DIR)/
	$(PROG_ZIP) -q -r $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip $(EMDROS_PROGREF_GUIDE_HTML_DIR)/
	rm -rf $(EMDROS_PROGREF_GUIDE_HTML_DIR)

macdist: $(MAYBE_PDF) $(MAYBE_ZIP)
	if test -f $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip; then cp $(EMDROS_PROGREF_GUIDE_HTML_DIR).zip $(MACDISTDIR)/doc/html; fi
	if test -f EmdrosProgRefGuide.pdf; then cp EmdrosProgRefGuide.pdf $(MACDISTDIR)/doc/PDF; fi
