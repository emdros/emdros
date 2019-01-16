SUBDIRS = progref

man_MANS = emdros.1 \
           mql.1 mqldump.1 manage_indices.1 upgrade_db.1 \
           emdftry.1 mqltry.1 mqllingtry.1 jsontry.1 renderobjects.1 \
           hal_build_db.1 mqlhal.1 eqtc.1 eqt.1 eqtu.1 \
           ubimport.1 pennimport.1 negraimport.1 tigerxmlimport.1 \
           plaintextimport.1 slashedtextimport.1 sfmimport.1 bptdump.1 \
           fts_indexer.1 fts_harvester.1 fts_filters.5

man_html1 = $(man_MANS:.1=.html)
man_html = $(man_html1:.5=.html)

LYXFILES = Monad-Sets.lyx \
           MQL-Programmers-Guide.lyx MQL-Query-Guide.lyx \
           New-EMdF.lyx Rel-EMdF.lyx Standard-MdF.lyx

PDFFILES = $(LYXFILES:.lyx=.pdf)

TXTFILES = bootstrapping.txt HOW-TO-USE.txt MQL-README.txt \
           README-doc.txt 

WIN32FILES = README.MySQL.Win32.txt 

EXTRA_DIST = index.html $(TXTFILES) $(WIN32FILES) \
             $(man_MANS) $(man_html) mqlhal.1.in eqtc.1.in \
             eqt.1.in eqtu.1.in $(LYXFILES) $(PDFFILES) \
             TheDoor.fig TheDoor.eps

CLEANFILES = *~ core 

DISTCLEANFILES = $(PDFFILES) TheDoor.eps \
             $(man_html) mqlhal.1 eqtc.1 \
             eqt.1 eqtu.1  eqtu.1.in 

BUILT_SOURCES = mqlhal.1 eqtc.1 eqt.1 eqtu.1 eqtu.1 TheDoor.eps \
                $(PDFFILES)

dist_doc_DATA = $(PDFFILES)


TheDoor.eps: TheDoor.fig
	fig2dev -L eps TheDoor.fig TheDoor.eps

Monad-Sets.pdf : Monad-Sets.lyx
	lyx -e pdf2 Monad-Sets.lyx

MQL-Query-subset.pdf: MQL-Query-subset.lyx TheDoor.eps
	lyx -e pdf2 MQL-Query-subset.lyx

MQL-Programmers-Guide.pdf: MQL-Programmers-Guide.lyx  TheDoor.eps
	lyx -e pdf2 MQL-Programmers-Guide.lyx

MQL-Query-Guide.pdf: MQL-Query-Guide.lyx  TheDoor.eps
	lyx -e pdf2 MQL-Query-Guide.lyx

New-EMdF.pdf: New-EMdF.lyx
	lyx -e pdf2 New-EMdF.lyx

Rel-EMdF.pdf: Rel-EMdF.lyx
	lyx -e pdf2 Rel-EMdF.lyx

Standard-MdF.pdf: Standard-MdF.lyx  TheDoor.eps
	lyx -e pdf2 Standard-MdF.lyx


mqlhal.1: mqlhal.1.in
	$(top_builddir)/config.status --file=$(srcdir)/mqlhal.1

eqtc.1: eqtc.1.in
	$(top_builddir)/config.status --file=$(srcdir)/eqtc.1

eqt.1: eqt.1.in
	$(top_builddir)/config.status --file=$(srcdir)/eqt.1

eqtu.1.in: eqt.1.in
	cp eqt.1.in eqtu.1.in

eqtu.1: eqtu.1.in
	$(top_builddir)/config.status --file=$(srcdir)/eqtu.1

%.html: %.1
	groff -mandoc $< -T html > $@

%.html: %.5
	groff -mandoc $< -T html > $@

html: $(man_html)

MACDOCDISTDIR = $(MACDISTDIR)/doc

macdist: all html
	cp $(PDFFILES) $(MACDOCDISTDIR)/PDF
	cp $(TXTFILES) $(MACDOCDISTDIR)/txt
	cp $(man_html) $(MACDOCDISTDIR)/html
	cp $(man_MANS) $(MACDISTDIR)/development/man/1
	cd progref ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..

