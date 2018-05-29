man_MANS = emdros.1 \
           mql.1 mqldump.1 manage_indices.1 upgrade_db.1 \
           emdftry.1 mqltry.1 mqllingtry.1 jsontry.1 renderobjects.1 \
           hal_build_db.1 mqlhal.1 eqtc.1 eqt.1 eqtu.1 \
           ubimport.1 pennimport.1 negraimport.1 tigerxmlimport.1 \
           plaintextimport.1 slashedtextimport.1 sfmimport.1 bptdump.1 \
           fts_indexer.1 fts_harvester.1 fts_filters.5

man_html1 = $(man_MANS:.1=.html)
man_html = $(man_html1:.5=.html)

TXTFILES = bootstrapping.txt HOW-TO-USE.txt MQL-README.txt \
           README-doc.txt ChangeLog-doc.txt NEWS-doc.txt 

WIN32FILES = README.MySQL.Win32.txt

CMSFILES = \
           cms/buildhtb.py \
           cms/cms.css \
           cms/dot.gif 



EXTRA_DIST = index.html $(TXTFILES) $(WIN32FILES) $(CMSFILES) \
             $(man_MANS) $(man_html) mqlhal.1.in eqtc.1.in \
             eqt.1.in eqtu.1.in 

CLEANFILES = *~ core 

DISTCLEANFILES = \
             $(man_html) mqlhal.1 eqtc.1 \
             eqt.1 eqtu.1  eqtu.1.in 

BUILT_SOURCES = mqlhal.1 eqtc.1 eqt.1 eqtu.1 eqtu.1 

dist_doc_DATA = 




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
	cp $(TXTFILES) $(MACDOCDISTDIR)/txt
	cp $(man_html) $(MACDOCDISTDIR)/html
	cp $(man_MANS) $(MACDISTDIR)/development/man/1

