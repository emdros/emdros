SWIG_PROGRAM = @SWIG_PROGRAM@

JEMDROS_CPPDIRECTORY = Cpp
JEMDROS_WRAPPER_CXX = $(JEMDROS_CPPDIRECTORY)/jemdros_wrapper.cpp
JAVA_OUTDIR = Java

ZIP_JAVA_OUTDIR = app/src/main/java/com/scripturesys/jemdros
ZIP_JEMDROS_CPPDIRECTORY = app/src/main/cpp

COPIED_FILES = \
        $(ZIP_JEMDROS_CPPDIRECTORY)/emdros_c_amalgamation_1.c \
        $(ZIP_JEMDROS_CPPDIRECTORY)/emdros_c_amalgamation_2.c \
        $(ZIP_JEMDROS_CPPDIRECTORY)/emdros_amalgamation_1.cpp \
        $(ZIP_JEMDROS_CPPDIRECTORY)/emdros_amalgamation_2.cpp \
        $(ZIP_JEMDROS_CPPDIRECTORY)/emdros.h \
        $(ZIP_JEMDROS_CPPDIRECTORY)/emdros-config.h \
        $(ZIP_JEMDROS_CPPDIRECTORY)/emdros-lconfig.h 


$(ZIP_JEMDROS_CPPDIRECTORY)/%.cpp: 	../../amalgamation/%.cpp
	cp $< $@

$(ZIP_JEMDROS_CPPDIRECTORY)/%.c: 	../../amalgamation/%.c
	cp $< $@

$(ZIP_JEMDROS_CPPDIRECTORY)/%.h: 	../../amalgamation/%.h
	cp $< $@

EXTRA_DIST = jemdros.i \
             $(BUILT_SOURCES) 
DISTCLEANFILES = $(BUILT_SOURCES) $(JAVA_OUTDIR)/*.java





#
#
$(JEMDROS_WRAPPER_CXX) $(JAVA_FILES): jemdros.i
	mkdir -p $(JAVA_OUTDIR)
	mkdir -p $(JEMDROS_CPPDIRECTORY)
	$(SWIG_PROGRAM) -package com.scripturesys.jemdros -c++ -java -outdir $(JAVA_OUTDIR)/ -o $(JEMDROS_WRAPPER_CXX) jemdros.i


clean_tildefiles:
	rm -f *~
	rm -f app/src/main/cpp/*~
	rm -f app/src/main/java/*~


CLEANFILES = *~ core $(COPIED_FILES) $(JEMDROS_WRAPPER_CXX) 


if SWIG_WITH_JAVA_WRAPPERS

ZIPFILENAME = jemdros-Android-@VERSION@.zip
ZIPFILES = app/

zip: $(ZIPFILENAME)


$(ZIPFILENAME): mkdirs_zip $(COPIED_FILES) $(JEMDROS_WRAPPER_CXX) clean_tildefiles
	cp -p $(JEMDROS_WRAPPER_CXX) $(ZIP_JEMDROS_CPPDIRECTORY)/
	zip -r $(ZIPFILENAME) $(ZIPFILES)

mkdirs_zip:
	mkdir -p $(ZIP_JAVA_OUTDIR)
	mkdir -p $(ZIP_JEMDROS_CPPDIRECTORY)


macdist: 
	cp -fL blah blah do something $(MACDISTDIR)/development/SWIG/Android

BUILT_SOURCES = $(JEMDROS_WRAPPER_CXX) $(JAVA_FILES)

JAVA_FILES = $(JAVA_OUTDIR)/*.java


else

# Not SWIG_WITH_JAVA_WRAPPERS

# We must have these dummy install-hooks because they are defined
# in the other section

zip:

macdist:

BUILT_SOURCES =

JAVA_FILES = 

endif # end if SWIG_WITH_JAVA_WRAPPERS


