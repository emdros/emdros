ZIP = C:\UTILS\PAK\ZIP.EXE
DISTZIP = ..\emdrosdj.zip
DISTDIR = emdros

TARGETS = ..\include\elconf.h ..\include\econf.h

CLEANFILES = $(TARGETS)

all: $(TARGETS)
	cd ../pcre ; make -f djgpp.mak 
	cd ../sqlite ; make -f djgpp.mak 
	cd ../include ; make -f djgpp.mak
	cd ../emdf ; make -f djgpp.mak 
	cd ../mql ; make -f djgpp.mak 
	cd ../util ; make -f djgpp.mak 
	cd ../src ; make -f djgpp.mak 
	cd ../src/qrytool/teckit ; make -f djgpp.mak 
	cd ../../../src/qrytool ; make -f djgpp.mak 
	cd ../../examples/HAL ; make -f djgpp.mak 
	cd ../../tests ; make -f djgpp.mak 


..\include\elconf.h: lconfig.h
	copy lconfig.h ..\include\elconf.h

..\include\econf.h: config.h
	copy config.h ..\include\econf.h

clean:
	for %f in ($(CLEANFILES)) DO del %f
	cd ../pcre ; make -f djgpp.mak clean
	cd ../sqlite ; make -f djgpp.mak clean
	cd ../include ; make -f djgpp.mak clean
	cd ../emdf ; make -f djgpp.mak clean
	cd ../mql ; make -f djgpp.mak clean
	cd ../util ; make -f djgpp.mak clean
	cd ../src ; make -f djgpp.mak clean
	cd ../src/qrytool/teckit ; make -f djgpp.mak clean
	cd ../../../src/qrytool ; make -f djgpp.mak clean
	cd ../../examples/HAL ; make -f djgpp.mak clean
	cd ../../tests ; make -f djgpp.mak clean


DIST_BIN_FILES1 = ..\sqlite\src\sqlshell.exe ..\src\mql.exe 
DIST_BIN_FILES2 = ..\src\mqldump.exe  ..\src\upgrdb.exe \
                  ..\src\qrytool\eqtc.exe
DIST_BIN_FILES3 = ..\src\mangindx.exe ..\tests\emdftry.exe \
                  ..\tests\mqltry.exe  ..\tests\mqllgtry.exe
DIT_BIN_FILES4  = ..\examples\hal\halblddb.exe ..\examples\hal\mqlhal.exe

DIST_LIB_FILES = ..\sqlite\src\libsqlit.a \
                 ..\pcre\libpcre.a \
                 ..\emdf\libemdf.a \
                 ..\mql\libmql.a 
DIST_LIB_FILES2 = ..\util\libutil.a \
                  ..\src\qrytool\teckit\libteckt.a

DIST_DOC_FILES = readmedj.txt \
  ..\doc\MQL-Programmers-Guide.pdf ..\doc\MQL-Query-Guide.pdf 

DIST_DOC_FILES2 = ..\doc\Monad-Sets.pdf \
  ..\doc\New-EMdF.pdf ..\doc\Rel-EMdF.pdf ..\doc\Standard-MdF.pdf

DIST_DOC_FILES3 = ..\doc\emdros.html \
   ..\doc\mql.html ..\doc\mqldump.html \
   ..\doc\manage_indices.html 

DIST_DOC_FILES4 = ..\doc\upgrade_db.html \
   ..\doc\emdftry.html ..\doc\mqltry.html \

DIST_DOC_FILES5 = mqllingtry.html \
   ..\doc\halblddb.html ..\doc\mqlhal.html ..\doc\eqtc.html

DIST_ETC_FILES = ..\src\qrytool\*.cfg ..\src\qrytool\Amsterdam.map \
   ..\examples\hal\*.cfg 

dist: all
	cd ..\DJGPP
	-deltree /Y $(DISTDIR)
	-del /S /F $(DISTDIR)
	del $(DISTZIP)
	md $(DISTDIR)
	md $(DISTDIR)\bin
	md $(DISTDIR)\lib
	md $(DISTDIR)\etc
	md $(DISTDIR)\include
	md $(DISTDIR)\doc
	for %f in ($(DIST_BIN_FILES1)) DO copy %f $(DISTDIR)\bin
	for %f in ($(DIST_BIN_FILES2)) DO copy %f $(DISTDIR)\bin
	for %f in ($(DIST_BIN_FILES3)) DO copy %f $(DISTDIR)\bin
	for %f in ($(DIST_BIN_FILES4)) DO copy %f $(DISTDIR)\bin
	for %f in ($(DIST_LIB_FILES)) DO copy %f $(DISTDIR)\lib
	for %f in ($(DIST_LIB_FILES2)) DO copy %f $(DISTDIR)\lib
	for %f in ($(DIST_ETC_FILES)) DO copy %f $(DISTDIR)\etc
	for %f in ($(DIST_DOC_FILES)) DO copy %f $(DISTDIR)\doc
	for %f in ($(DIST_DOC_FILES2)) DO copy %f $(DISTDIR)\doc
	for %f in ($(DIST_DOC_FILES3)) DO copy %f $(DISTDIR)\doc
	for %f in ($(DIST_DOC_FILES4)) DO copy %f $(DISTDIR)\doc
	for %f in ($(DIST_DOC_FILES5)) DO copy %f $(DISTDIR)\doc
	for %f in (..\include\*.h) DO copy %f $(DISTDIR)\include
	$(ZIP) -R $(DISTZIP) $(DISTDIR)\*




