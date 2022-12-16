MQL = ../../src/mql
RENDEROBJECTS = ../../src/renderobjects
FTS_INDEXER = ../../src/fts_indexer
FTS_HARVESTER = ../../src/fts_harvester


MYOUTPUT_SQLITE3 = asv.sqlite3.renderobjects.context.xml \
                   asv.sqlite3.renderobjects.parallel.xml \
                   asv.sqlite3.renderxml.context.xml \
                   asv.sqlite3.renderxml.parallel.xml \
                   asv.sqlite3.fts_harvester.fe1.1.txt \
                   asv.sqlite3.fts_harvester.fe1.1.xml \
                   asv.sqlite3.fts_harvester.fe2.1.txt \
                   asv.sqlite3.fts_harvester.fe2.1.xml \
                   asv.sqlite3.fts_harvester.fe1.2.txt \
                   asv.sqlite3.fts_harvester.fe1.2.xml \
                   asv.sqlite3.fts_harvester.fe2.2.txt \
                   asv.sqlite3.fts_harvester.fe2.2.xml 


if SQLITE3
myoutput_sqlite3 = $(MYOUTPUT_SQLITE3)
else
myoutput_sqlite3 = 
endif

MYOUTPUT_STATIC = $(MYOUTPUT_SQLITE3) 

MYOUTPUT = $(myoutput_sqlite3)

KNOWN_GOOD_OUTPUT = $(subst,asv,known_good_output/asv,$(MYOUTPUT_STATIC))

CLEANFILES = *~ *.pyc asv.mql asv.sqlite3 \
             $(MYOUTPUT) diff.txt

EXTRA_DIST = asv.Gen1-3.osis.xml osis2mql.py osis_schema.mql asv.mql \
             osistohtml.json \
             known_good_output/*.xml \
             known_good_output/*.txt

.PHONY: test
test:
	$(MAKE) clean
	$(MAKE) diff.txt

asv.mql: asv.Gen1-3.osis.xml osis2mql.py
	$(PYTHON_PROGRAM) osis2mql.py --OT asv.Gen1-3.osis.xml $@



###########################################################
##
## SQLite3 
##
###########################################################
asv.sqlite3: asv.mql osis_schema.mql $(MQL) 
	-echo "DROP DATABASE '$@' GO" | $(MQL) -b 3
	echo "CREATE DATABASE '$@' GO" | $(MQL) -b 3
	$(MQL) -b 3 -d $@ osis_schema.mql
	echo "DROP INDEXES ON OBJECT TYPES [ALL] GO" | $(MQL) -b 3 -d $@
	$(MQL) -b 3 -d $@ asv.mql
	$(FTS_INDEXER) -b 3 --fts-engine 1 -d $@ -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface --no-vacuum 
	$(FTS_INDEXER) -b 3 --fts-engine 2 -d $@ -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface --no-vacuum 
	echo "CREATE INDEXES ON OBJECT TYPES [ALL] GO" | $(MQL) -b 3 -d $@
	echo "VACUUM DATABASE ANALYZE GO" | $(MQL) -b 3 -d $@

# fts_harvester sqlite3
asv.sqlite3.fts_harvester.fe1.1.txt: asv.sqlite3 osistohtml.json
	$(FTS_HARVESTER) -b 3 -d asv.sqlite3  --fts-engine 1 -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface -o $@ "God"

asv.sqlite3.fts_harvester.fe1.1.xml: asv.sqlite3 osistohtml.json
	$(FTS_HARVESTER) -b 3 -d asv.sqlite3  --fts-engine 1 -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface --xhtml --bookcase-title-otn verse --bookcase-title-feature osisID -o $@ "God"

asv.sqlite3.fts_harvester.fe2.1.txt: asv.sqlite3 osistohtml.json
	$(FTS_HARVESTER) -b 3 -d asv.sqlite3  --fts-engine 2 -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface -o $@ "God"

asv.sqlite3.fts_harvester.fe2.1.xml: asv.sqlite3 osistohtml.json
	$(FTS_HARVESTER) -b 3 -d asv.sqlite3  --fts-engine 2 -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface --xhtml --bookcase-title-otn verse --bookcase-title-feature osisID -o $@ "God" 

asv.sqlite3.fts_harvester.fe1.2.txt: asv.sqlite3 osistohtml.json
	$(FTS_HARVESTER) -b 3 -d asv.sqlite3  --fts-engine 1 -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface -o $@ "God" "said" 

asv.sqlite3.fts_harvester.fe1.2.xml: asv.sqlite3 osistohtml.json
	$(FTS_HARVESTER) -b 3 -d asv.sqlite3  --fts-engine 1 -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface --xhtml --bookcase-title-otn verse --bookcase-title-feature osisID -o $@ "God" "said" 

asv.sqlite3.fts_harvester.fe2.2.txt: asv.sqlite3 osistohtml.json
	$(FTS_HARVESTER) -b 3 -d asv.sqlite3  --fts-engine 2 -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface -o $@ "God" "said" 

asv.sqlite3.fts_harvester.fe2.2.xml: asv.sqlite3 osistohtml.json
	$(FTS_HARVESTER) -b 3 -d asv.sqlite3  --fts-engine 2 -sf osistohtml.json -s searchfilterlist --bookcase-otn verse --indexed-otn token --indexed-feature wholesurface --xhtml --bookcase-title-otn verse --bookcase-title-feature osisID -o $@ "God" "said" 


# Renderobjects SQLite 3

asv.sqlite3.renderobjects.context.xml: asv.sqlite3 osistohtml.json
	$(RENDEROBJECTS) -b 3 -s context -o $@ osistohtml.json asv.sqlite3

asv.sqlite3.renderobjects.parallel.xml: asv.sqlite3 osistohtml.json
	$(RENDEROBJECTS) -b 3 -s parallel -o $@ osistohtml.json asv.sqlite3

asv.sqlite3.renderxml.context.xml: asv.sqlite3 osistohtml.json
	$(RENDEROBJECTS) -b 3 --renderxml -s context -o $@ osistohtml.json asv.sqlite3

asv.sqlite3.renderxml.parallel.xml: asv.sqlite3 osistohtml.json
	$(RENDEROBJECTS) -b 3 --renderxml -s parallel -o $@ osistohtml.json asv.sqlite3



diff.txt: $(MYOUTPUT)
	rm -f $@
	for f in $(MYOUTPUT); do diff $$f known_good_output/$$f >> $@; done
	test -z "`cat $@`"
	@echo ""
	@echo "All tests passed."
	@echo ""

