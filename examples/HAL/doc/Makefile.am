DOCUMENT_NAME = HALGuide

PROGREF_DIR = ../../../doc/progref

EXTRA_DIST = Content.txt files.txt config.py.in \
             $(DOCUMENT_NAME).pdf $(DOCUMENT_NAME).tex \
             1001.html config.py buildhtb.py cms.css dot.gif

BUILT_SOURCES = buildhtb.py cms.css dot.gif \
                1001.html $(DOCUMENT_NAME).pdf config.py 


STATIC_CLEANFILES = \
             $(DOCUMENT_NAME).aux \
             $(DOCUMENT_NAME).out \
             $(DOCUMENT_NAME).tex \
             $(DOCUMENT_NAME).toc \
             $(DOCUMENT_NAME).log \
             $(DOCUMENT_NAME).htb \
             $(DOCUMENT_NAME).htb \
             $(DOCUMENT_NAME).hhk \
             $(DOCUMENT_NAME).hhc 


CLEANFILES = *~ core 

DISTCLEANFILES = \
             *.html *.htm texput.log \
             $(DOCUMENT_NAME).pdf \
             $(DOCUMENT_NAME).tex \
             $(DOCUMENT_NAME).htm \
             config.py config.pyc config.log \
             $(srcdir)/buildhtb.py dot.gif cms.css \
             $(STATIC_CLEANFILES)



1001.html: $(srcdir)/config.py $(srcdir)/buildhtb.py Content.txt files.txt
	cd $(srcdir) ; python $(srcdir)/buildhtb.py --web

$(DOCUMENT_NAME).tex: $(srcdir)/config.py $(srcdir)/buildhtb.py Content.txt files.txt
	cd $(srcdir) ; python $(srcdir)/buildhtb.py --latex

$(DOCUMENT_NAME).pdf: $(DOCUMENT_NAME).tex
	pdflatex $(DOCUMENT_NAME).tex

$(srcdir)/config.py: $(srcdir)/config.py.in
	$(top_builddir)/config.status --file=$(srcdir)/config.py

$(srcdir)/buildhtb.py: $(srcdir)/$(PROGREF_DIR)/buildhtb.py
	cp $(srcdir)/$(PROGREF_DIR)/buildhtb.py $(srcdir)/buildhtb.py 

$(srcdir)/cms.css: $(srcdir)/$(PROGREF_DIR)/cms.css
	cp $(srcdir)/$(PROGREF_DIR)/cms.css $(srcdir)/cms.css 

$(srcdir)/dot.gif: $(srcdir)/$(PROGREF_DIR)/dot.gif
	cp $(srcdir)/$(PROGREF_DIR)/dot.gif $(srcdir)/dot.gif 

