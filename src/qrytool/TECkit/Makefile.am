AM_CXXFLAGS = @CXXFLAGS@ @DEBUGFLAG@ \
              @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ \
              @ADDITIONAL_CXXFLAGS@ -DTARGET_RT_BIG_ENDIAN=@BIG_ENDIAN@ \
              -DNO_ZLIB

ARFLAGS = @ARFLAGS@

if HOSTISDARWIN
MACLDFLAGS = -static
endif


bin_PROGRAMS = TxtConv TECkit_Compile

TxtConv_SOURCES = TxtConv.cpp 
TxtConv_LDFLAGS = libteckit.la
TxtConv_DEPENDENCIES = libteckit.la

TECkit_Compile_SOURCES = TECkit_Compile.cpp
TECkit_Compile_LDFLAGS = libteckit.la
TECkit_Compile_DEPENDENCIES = libteckit.la


AM_CPPFLAGS = -I$(top_srcdir)/include -I.
CLEANFILES = *~ core config.log

EXTRA_DIST = win32.mak mingw.mak djgpp.mak README.txt \
             License_CPLv05.txt License_LGPLv21.txt LICENSING.txt \
             NormalizationData.c

lib_LTLIBRARIES = libteckit.la

libteckit_la_SOURCES = Compiler.cpp \
                      Engine.cpp \
                      UnicodeNames.cpp 
libteckit_la_LDFLAGS = $(MACLDFLAGS) \
                       @SUN_LDFLAGS@ \
                       @DARWIN_LDFLAGS@
if HOSTISSUNCC
libteckit_la_AR = $(CXX) -xar -o
else
libteckit_la_AR = $(AR) $(ARFLAGS)
endif



pkginclude_HEADERS = Compiler.h \
                 Engine.h \
                 TECkit_Common.h \
                 TECkit_Compiler.h \
                 TECkit_Engine.h \
                 TECkit_Format.h \
                 ulong_chartraits.h \
                 Prefix_Mac.h \
                 Prefix_Win32.h
