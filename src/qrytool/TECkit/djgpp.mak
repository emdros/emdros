CXXFLAGS=-I../include -O3 -DTARGET_RT_BIG_ENDIAN=0 -DNO_ZLIB


#
# Sources
#

teckitlib_src = Compiler.cpp \
                Engine.cpp \
                UnicodeNames.cpp 


#
# Objects
#
objects = $(teckitlib_src:.cpp=.o)

CLEANFILES = *.O $(TARGETS)

TARGETS = libteckt.a

all: $(TARGETS)

libteckt.a: $(objects)
	ar r $@ $(objects)

.CPP.O:
	gcc $(CXXFLAGS) -c $<

        

clean:
	for %f in ($(CLEANFILES)) DO del %f

