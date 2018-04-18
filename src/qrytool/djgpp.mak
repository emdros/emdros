LIBRARY = libqrytl.a
PROGRAM = eqtc.exe
TARGETS = $(LIBRARY) $(PROGRAM)

libqrytl_SOURCES = solution.cpp mytable.cpp \
                   harvest.cpp output_style.cpp emdros_wrapper.cpp \
                   exec.cpp consout.cpp qtoutput.cpp
libqrytl_OBJECTS = $(libqrytl_SOURCES:.cpp=.o)

CLEANFILES = *.O $(TARGETS)

CXXFLAGS=-I..\..\include -I.  -O3

LDFLAGS = -L../../emdf -L../../pcre \
          -L../../mql -L../../util -L. -L./teckit

EMDF_LDLIBS = -lemdf -lsqlit 
MQL_LDLIBS = -lmql -lpcre $(EMDF_LDLIBS) 
QRYTOOL_LDLIBS = -lqrytl -lteckt -lutil $(MQL_LDLIBS)


all: $(TARGETS) 

libqrytl.a: $(libqrytl_OBJECTS)
	ar r libqrytl.a $(libqrytl_OBJECTS)

$(PROGRAM): $(LIBRARY) mqlqt.o
	gxx -o $(PROGRAM) mqlqt.o $(LDFLAGS) $(QRYTOOL_LDLIBS)

mqlqt.o: mqlqt.cpp

clean:
	for %f in ($(CLEANFILES)) DO del %f


