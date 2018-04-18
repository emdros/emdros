MYTARGETS_SQLITE3 = KJV.sqlite3

if SQLITE3
mytargets_sqlite3 = $(MYTARGETS_SQLITE3)
else
mytargets_sqlite3 =
endif

MYTARGETS = KJV_schema.mql KJV_data.mql $(mytargets_sqlite3)

CLEANFILES = *~ $(MYTARGETS)

EXTRA_DIST = parsed/*.txt \
             queries/*.mql \
             EmdrosQueryTool-KJVParsed.cfg \
             EmdrosChunkingTool-KJVParsed.cfg \
             README-KJVParsed.txt \
             queries/*.mql

KJV_schema.mql: ../../src/pennimport
	../../src/pennimport -d KJV --distinct-nt-otns -s > $@

KJV_data.mql: ../../src/pennimport
	../../src/pennimport -d KJV --distinct-nt-otns parsed/*.txt > $@

KJV.sqlite3: KJV_schema.mql KJV_data.mql
	-echo "DROP DATABASE 'KJV.sqlite3' GO" | ../../src/mql -b 3
	-echo "DROP DATABASE 'KJV' GO" | ../../src/mql -b 3
	../../src/mql -b 3 KJV_schema.mql
	../../src/mql -b 3 -d KJV KJV_data.mql
	echo "CREATE INDEXES ON OBJECT TYPES [All]" | ../../src/mql -b 3 -d KJV
	echo "VACUUM DATABASE ANALYZE GO" | ../../src/mql -b 3 -d KJV
	mv KJV $@

examples: $(MYTARGETS)
	mkdir -p $(EXAMPLESDIR)/KJVParsed
	mkdir -p $(EXAMPLESDIR)/KJVParsed/MQL
	mkdir -p $(EXAMPLESDIR)/KJVParsed/parsed
	mkdir -p $(EXAMPLESDIR)/KJVParsed/queries
	cp -p KJV_schema.mql KJV_data.mql $(EXAMPLESDIR)/KJVParsed/MQL/
	cp -p parsed/*.txt $(EXAMPLESDIR)/KJVParsed/parsed/
	cp -p queries/*.mql $(EXAMPLESDIR)/KJVParsed/queries/
	cp -p KJV.sqlite3  $(EXAMPLESDIR)/KJVParsed/
	cp -p EmdrosQueryTool-KJVParsed.cfg $(EXAMPLESDIR)/KJVParsed/
	cp -p EmdrosChunkingTool-KJVParsed.cfg $(EXAMPLESDIR)/KJVParsed/
	cp -p README-KJVParsed.txt $(EXAMPLESDIR)/KJVParsed/
