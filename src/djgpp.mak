TARGETS = mql.exe mqldump.exe upgrdb.exe mangindx.exe

CLEANFILES = *.O $(TARGETS)

CXXFLAGS = -I../include -O3
#CXXFLAGS = -I../include -g

LDFLAGS = -L../emdf -L../pcre -L../mql -L../util

EMDF_LDLIBS = -lemdf -lsqlit 
MQL_LDLIBS = -lmql -lpcre $(EMDF_LDLIBS) 


all: $(TARGETS)

# mql
mql.exe: mql.o
	gxx -o mql.exe mql.o $(LDFLAGS) $(MQL_LDLIBS)

mql.o: mql.cpp

# mqldump
mqldump.exe: mqldump.o
	gxx -o mqldump.exe mqldump.o $(LDFLAGS) -lutil $(MQL_LDLIBS)

mqldump.o: mqldump.cpp

# upgrdb
upgrdb.exe: upgrad~1.o
	gxx -o upgrdb.exe upgrad~1.o $(LDFLAGS) $(MQL_LDLIBS)

upgrad~1.o: upgrad~1.cpp

# mangindx
mangindx.exe: manage~1.o
	gxx -o mangindx.exe manage~1.o $(LDFLAGS) $(EMDF_LDLIBS)

manage~1.o: manage~1.cpp

# agexport
agexport.exe: agexport.o
	gxx -o agexport.exe agexport.o $(LDFLAGS) -lutil $(MQL_LDLIBS)

agexport.o: agexport.cpp



clean:
	for %f in ($(CLEANFILES)) DO del %f


