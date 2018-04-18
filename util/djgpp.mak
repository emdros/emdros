CXXFLAGS=-I../include -O3
#CXXFLAGS=-I../include -g


#
# Sources
#

utillib_src = \
    conf.cpp \
    opt.cpp \
    schema.cpp \
    agxml.cpp \
    mql_exporter.cpp \
    qdxml.cpp 


#
# Objects
#
objects = $(utillib_src:.cpp=.o)

CLEANFILES = *.O

TARGETS = libutil.a

all: $(TARGETS)

libutil.a: $(objects)
	ar r $@ $(objects)

.CPP.O:
	gcc $(CXXFLAGS) -c $<

        

clean:
	for %f in ($(CLEANFILES)) DO del %f
