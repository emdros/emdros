SUBDIRS = cms progref

CLEANFILES = *~

man_MANS = man1/mql.1 \
           man1/mqldump.1 \
           man1/renderobjects.1 \
           man1/eqtc.1 \
           man1/eqtu.1 \
           man1/bptdump.1 \
           man1/fts_indexer.1 \
           man1/fts_harvester.1 \
           man1/emdftry.1 \
           man1/upgrade_db.1 \
           man1/manage_indices.1 \
           man1/mqltry.1 \
           man1/mqllingtry.1 \
           man1/jsontry.1 \
           man5/emdros.5 \
           man5/fts_filters.5

man_html1 = $(man_MANS:.1=.html)
man_html = $(man_html1:.5=.html)

EXTRA_DIST = $(man_MANS) $(man_html) man1/eqtc.1.in \
             man1/eqtu.1.in 

DISTCLEANFILES = $(man_html) man1/eqtc.1 man1/eqtu.1

BUILT_SOURCES = man1/eqtc.1 man1/eqtu.1

man1/eqtc.1: man1/eqtc.1.in
	$(top_builddir)/config.status --file=$(srcdir)/man1/eqtc.1

man1/eqtu.1: man1/eqtu.1.in
	$(top_builddir)/config.status --file=$(srcdir)/man1/eqtu.1

.1.html: 
	groff -mandoc $< -T html > $@

.5.html:
	groff -mandoc $< -T html > $@

SUFFIXES = .html .1 .5

html: $(man_html)

MACDOCDISTDIR = $(MACDISTDIR)/doc

macdist: all html
	cp $(man_html) $(MACDOCDISTDIR)/html
	cp $(man_MANS) $(MACDISTDIR)/development/man/1
	cd progref ; make macdist MACDISTDIR=../$(MACDISTDIR) ; cd ..
