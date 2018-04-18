TARGETS = emdftry.exe mqltry.exe mqllgtry.exe

CLEANFILES = *.O $(TARGETS)

CXXFLAGS = -I../include -O3

LDFLAGS = -L../emdf -L../sqlite/src -L../pcre -L../mql

EMDF_LDLIBS = -lemdf -lsqlit 
MQL_LDLIBS = -lmql -lpcre $(EMDF_LDLIBS) 
MQL_LDLIBS = -lmql -lpcre $(EMDF_LDLIBS) 



all: $(TARGETS)

# mqltry
mqltry.exe: mqltry.o
	gxx -o mqltry.exe mqltry.o $(LDFLAGS) $(MQL_LDLIBS)

mqltry.o: mqltry.cpp

# mqllingtry
mqllgtry.exe: mqllin~1.o
	gxx -o mqllgtry.exe mqllin~1.o $(LDFLAGS) $(MQL_LDLIBS)

mqllin~1.o: mqllin~1.cpp

# emdftry
emdftry.exe: emdftry.o
	gxx -o emdftry.exe emdftry.o $(LDFLAGS) $(EMDF_LDLIBS)

emdftry.o: emdftry.cpp


clean:
	for %f in ($(CLEANFILES)) DO del %f


