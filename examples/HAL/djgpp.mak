LIBRARY = libhal.a
PROGRAMS = halblddb.exe mqlhal.exe
TARGETS = $(LIBRARY) $(PROGRAMS)

libhal_SOURCES = hallexer.cpp \
                 hal_execution_environment.cpp \
                 hal_schema.cpp \
                 hal_build_database.cpp \
                 hal_string_func.cpp \
                 HALSpace.cpp

libhal_OBJECTS = $(libhal_SOURCES:.cpp=.o)

CLEANFILES = *.O $(TARGETS)

CXXFLAGS=-I..\..\include -I.  -O3

LDFLAGS = -L../../emdf -L../../sqlite/src -L../../pcre -L../../mql

EMDF_LDLIBS = -lemdf -lsqlit 
MQL_LDLIBS = -lmql -lpcre $(EMDF_LDLIBS) 


all: $(TARGETS) 

libhal.a: $(libhal_OBJECTS)
	ar r libhal.a $(libhal_OBJECTS)

halblddb.exe: $(LIBRARY) halblddb.o
	gxx -o $@ halblddb.o -L. -lhal $(LDFLAGS) $(MQL_LDLIBS)

mqlhal.exe: $(LIBRARY) hal.o
	gxx -o $@ hal.o -L../../util -lutil -L. -lhal $(LDFLAGS) $(MQL_LDLIBS)

halblddb.o: halblddb.cpp

hal.o: hal.cpp

clean:
	for %f in ($(CLEANFILES)) DO del %f


