# Emdros makefile for win32 directory

# Makefile for MinGW

include .\config.mak

include ..\include\Makefile.inc

all: config.nsh
	copy config.h.win32 ..\include\emdros-config.h
ifeq ($(MYSQL),1)
	-mkdir ..\win32\mysql
	copy "$(MYSQLTOP)\include\*.*" ..\win32\mysql
endif
	$(MAKE) -C ..\include -f mingw.mak $(MAKEMACRO) 
	$(MAKE) -C ..\pcre -f mingw.mak $(MAKEMACRO) 
ifeq ($(BPT),1)
	$(MAKE) -C ..\zlib -f mingw.mak $(MAKEMACRO) 
endif
ifeq ($(SQLITE2),1)
	$(MAKE) -C ..\sqlite -f mingw.mak $(MAKEMACRO) 
endif
ifeq ($(SQLITE3),1)
	$(MAKE) -C ..\sqlite3 -f mingw.mak $(MAKEMACRO) 
endif
	$(MAKE) -C ..\EMdF -f mingw.mak $(MAKEMACRO) 
	$(MAKE) -C ..\MQL -f mingw.mak $(MAKEMACRO) 
	$(MAKE) -C ..\util -f mingw.mak $(MAKEMACRO) 
	$(MAKE) -C ..\harvest -f mingw.mak $(MAKEMACRO) 
	$(MAKE) -C ..\amalgamation -f mingw.mak $(MAKEMACRO) 
ifeq ($(EMDROS_ONLY),0)
ifeq ($(WITH_WXWIN),yes)
	$(MAKE) -C ..\wx -f mingw.mak $(MAKEMACRO) 
endif
	$(MAKE) -C ..\src\qrytool\TECkit -f mingw.mak $(MAKEMACRO) 
	$(MAKE) -C ..\src\qrytool -f mingw.mak $(MAKEMACRO) 
	$(MAKE) -C ..\src\importtool -f mingw.mak $(MAKEMACRO) 
endif
	$(MAKE) -C ..\src -f mingw.mak $(MAKEMACRO) 
	$(MAKE) -C ..\tests -f mingw.mak $(MAKEMACRO) 
ifeq ($(EMDROS_ONLY),0)
	$(MAKE) -C ..\SWIG -f mingw.mak $(MAKEMACRO) 
endif
	echo All Win32 parts have been built!


clean:
	-del config.nsh
	$(MAKE) -C ../include -f mingw.mak clean
ifeq ($(SQLITE2),1)
	$(MAKE) -C ../sqlite -f mingw.mak clean
endif
ifeq ($(SQLITE3),1)
	$(MAKE) -C ../sqlite3 -f mingw.mak clean
endif
	$(MAKE) -C ../EMdF -f mingw.mak clean
	$(MAKE) -C ../pcre -f mingw.mak clean
	$(MAKE) -C ../MQL -f mingw.mak clean
	$(MAKE) -C ../util -f mingw.mak clean
	$(MAKE) -C ../harvest -f mingw.mak clean
	$(MAKE) -C ../wx -f mingw.mak clean
	$(MAKE) -C ../src/qrytool/TECkit -f mingw.mak clean
	$(MAKE) -C ../src/qrytool -f mingw.mak clean
	$(MAKE) -C ../src -f mingw.mak clean
	$(MAKE) -C ../tests -f mingw.mak clean
	$(MAKE) -C ../SWIG -f mingw.mak clean
	-@erase *~
	-@rmdir Release\ /s /q
	-@rmdir ReleaseUnicode\ /s /q
	-@rmdir Debug\ /s /q
	-@rmdir DebugUnicode\ /s /q
	echo All Win32 parts have been cleaned!

distclean: clean
	-del ..\config.h



DIST_DOC_DOC_FILES=bootstrapping.txt HOW-TO-USE.txt MQL-README.txt \
	                README-doc.txt ChangeLog-doc.txt NEWS-doc.txt \
	                mql.html mqldump.html manage_indices.html upgrade_db.html \
	                emdftry.html jsontry.html mqltry.html mqllingtry.html \
	                fts_indexer.html fts_harvester.html fts_filters.html \
	                jsontry.html renderobjects.html \
	                eqtc.html eqtu.html \
	                ubimport.html pennimport.html negraimport.html \
	                plaintextimport.html slashedtextimport.html \
	                sfmimport.html tigerxmlimport.html bptdump.html \
	                Monad-Sets.lyx \
	                MQL-Programmers-Guide.lyx MQL-Query-Guide.lyx \
	                New-EMdF.lyx Rel-EMdF.lyx Standard-MdF.lyx \
	                Monad-Sets.pdf \
	                MQL-Programmers-Guide.pdf MQL-Query-Guide.pdf \
	                New-EMdF.pdf Rel-EMdF.pdf Standard-MdF.pdf \
	                progref\EmdrosProgRefGuide.pdf \
	                progref\emdros-progref-guide-$(VERSION).zip



DIST_DOC_ROOT_FILES=INSTALL.Win32.txt

DIST_DOC_ROOT_FILES_ADD_TXT = INSTALL AUTHORS COPYING README NEWS ChangeLog 

DIST_DOC_SWIG_PYTHON_FILES= SWIG\python\README-Python \
	                         SWIG\python\test.py \
	                         SWIG\python\dir.py

DIST_DOC_SWIG_JAVA_FILES  = SWIG\java\README-Java \
	                         SWIG\java\TestEmdros.java \
	                         SWIG\java\drop_db.mql

DIST_DOC_SWIG_CSHARP_FILES= SWIG\csharp\README-csharp \
	                         SWIG\csharp\TestEmdros.cs \
	                         SWIG\csharp\CSharpSources.zip

ifeq ($(WITH_WXWIN),yes)
DIST_WX_BIN = win32\ReleaseUnicode\eqtu.exe \
	           win32\ReleaseUnicode\EmdrosImportToolUnicode.exe \
	           win32\ReleaseUnicode\eqtu.exe.manifest \
	           win32\ReleaseUnicode\EmdrosImportToolUnicode.exe.manifest \
	           src\qrytool\doc\EQTUsersGuide.htb \
DIST_WX_LIB = win32\Release\libemdrosgui.lib \
	           win32\ReleaseUnicode\libemdrosguiu.lib 
else
DIST_WX_BIN =
DIST_WX_LIB =
endif


DIST_ROOT_FILES = .\win32\emdros.nsi \
	               .\win32\config.nsh \
	               .\win32\emdros-logo-installer.bmp


DIST_BIN_FILES=  win32\Release\mql.exe \
	              win32\Release\mqldump.exe \
	              win32\Release\manage_indices.exe \
	              win32\Release\upgrade_db.exe \
	              win32\Release\ubimport.exe \
	              win32\Release\pennimport.exe \
	              win32\Release\negraimport.exe \
	              win32\Release\tigerxmlimport.exe \
	              win32\Release\plaintextimport.exe \
	              win32\Release\slashedtextimport.exe \
	              win32\Release\sfmimport.exe \
	              win32\Release\bptdump.exe \
	              win32\Release\eqtc.exe \
	              win32\Release\emdftry.exe \
	              win32\Release\jsontry.exe \
	              win32\Release\renderobjects.exe \
	              win32\Release\mqltry.exe \
	              win32\Release\bpttry.exe \
	              win32\Release\mqllingtry.exe \
	              win32\Release\fts_indexer.exe \
	              win32\Release\fts_harvester.exe \
	              $(DIST_WX_BIN)


DIST_LIB_FILES=  win32\Release\libpcre_emdros.lib \
	              win32\Release\libemdf.lib \
	              win32\Release\libmql.lib \
	              win32\Release\libutil_emdros.lib \
	              win32\Release\libharvest.lib \
	              win32\Release\libemdrosgui.lib \
	              win32\ReleaseUnicode\libemdrosguiu.lib \
	              win32\Release\libteckit.lib \
	              $(DIST_WX_LIB)

DIST_ETC_QRYTOOL_FILES = src\qrytool\default.cfg \
	              src\qrytool\av.cfg \
	              src\qrytool\penn.cfg \
	              src\qrytool\wh.cfg \
	              src\qrytool\whutf8.cfg \
	              src\qrytool\whwordsonly.cfg \
	              src\qrytool\whwordsonlyutf8.cfg \
	              src\qrytool\wihebrew.cfg \
	              src\qrytool\wihebrew_new.cfg \
	              src\qrytool\wihebrew_manylines.cfg \
	              src\qrytool\Amsterdam.map 

DIST_PYTHON_FILES =  \
	              win32\Release\_EmdrosPy.pyd \
	              win32\Release\EmdrosPy.py 


DIST_JAVA_FILES = win32\Release\jemdros.dll \
	               win32\Release\jemdros.jar 


DIST_CSHARP_FILES =  win32\Release\csemdros.dll \
	                  win32\Release\EmdrosDotNet.dll




DIST_PG_BIN_FILES = "$(PGTOP)\bin\libpq.dll" \
	                 "$(PGTOP)\bin\comerr32.dll" \
	                 "$(PGTOP)\bin\ssleay32.dll" \
	                 "$(PGTOP)\bin\libeay32.dll" \
	                 "$(PGTOP)\bin\krb5_32.dll" \
	                 "$(PGTOP)\bin\libiconv2.dll" \
	                 "$(PGTOP)\bin\libintl3.dll" \
	                 "$(PGTOP)\bin\k5sprt32.dll" \
	                 "$(PGTOP)\bin\gssapi32.dll" 


DIST_PG_LIB_FILES = $(DIST_PG_BIN_FILES)



DIST_MY_BIN_FILES = "$(MYSQLTOP)\lib\libmysql.dll"

DIST_MY_LIB_FILES = $(DIST_MY_BIN_FILES) \
	                 "$(MYSQLTOP)\lib\libmysql.lib"

DIST_MY_DOC_FILES = doc\README.MySQL.Win32.txt




DIST_SQLITE2_LIB_FILES = win32\Release\libsqlite_emdros.lib

DIST_SQLITE2_BIN_FILES = win32\Release\sqlite_emdros.exe



DIST_SQLITE3_LIB_FILES = win32\Release\libsqlite3_emdros.lib

DIST_SQLITE3_BIN_FILES = win32\Release\sqlite3_emdros.exe



TECKIT_INCLUDE_HEADER_FILES = \
	                  Compiler.h \
	                  Engine.h \
	                  TECkit_Common.h \
	                  TECkit_Compiler.h \
	                  TECkit_Engine.h \
	                  TECkit_Format.h \
	                  ulong_chartraits.h \
	                  Prefix_Mac.h \
	                  Prefix_Win32.h


dist: all
	$(MAKE) -f mingw.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_DIST_DIRS
	$(MAKE) -f mingw.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_DIST_FILES
ifeq ($(PGSQL),1)
	$(MAKE) -f mingw.mak  DIST_PGSQL
endif
ifeq ($(MYSQL),1)
	$(MAKE) -f mingw.mak  DIST_MYSQL
endif
ifeq ($(SQLITE2),1)
	$(MAKE) -f mingw.mak  DIST_SQLITE2
endif
ifeq ($(SQLITE3),1)
	$(MAKE) -f mingw.mak  DIST_SQLITE3
endif
	cd ..
	cd win32


DIST_MYSQL: 
	$(MAKE) -f mingw.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_MY_DIST_FILES

DIST_PGSQL: 
	$(MAKE) -f mingw.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_PG_DIST_FILES

DIST_SQLITE2: 
	$(MAKE) -f mingw.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_SQLITE2_DIST_FILES
DIST_SQLITE3: 
	$(MAKE) -f mingw.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_SQLITE3_DIST_FILES


MAKE_PG_DIST_FILES:
	cd ..
	for %%F in ($(DIST_PG_BIN_FILES)) do copy %F "$(DIST_DIR)\bin"
	for %%F in ($(DIST_PG_LIB_FILES)) do copy %F "$(DIST_DIR)\lib"
	cd win32

MAKE_MY_DIST_FILES:
	cd ..
	for %%F in ($(DIST_MY_BIN_FILES)) do copy %F "$(DIST_DIR)\bin"
	for %%F in ($(DIST_MY_LIB_FILES)) do copy %F "$(DIST_DIR)\lib"
	for %%F in ($(DIST_MY_DOC_FILES)) do copy %F "$(DIST_DIR)\doc"
	cd win32

MAKE_SQLITE2_DIST_FILES:
	cd ..
	for %%F in ($(DIST_SQLITE2_LIB_FILES)) do copy %F "$(DIST_DIR)\lib"
	for %%F in ($(DIST_SQLITE2_BIN_FILES)) do copy %F "$(DIST_DIR)\bin"
	cd win32

MAKE_SQLITE3_DIST_FILES:
	cd ..
	for %%F in ($(DIST_SQLITE3_LIB_FILES)) do copy %F "$(DIST_DIR)\lib"
	for %%F in ($(DIST_SQLITE3_BIN_FILES)) do copy %F "$(DIST_DIR)\bin"
	cd win32

MAKE_DIST_SWIG: MAKE_DIST_SWIG_PYTHON MAKE_DIST_SWIG_JAVA MAKE_DIST_SWIG_CSHARP

MAKE_DIST_SWIG_PYTHON:
ifeq ($(WITH_SWIG_PYTHON),yes)
	cd ..
	-mkdir "$(DIST_DIR)\doc\python"
	for %%F in ($(DIST_DOC_SWIG_PYTHON_FILES)) do copy .\%F "$(DIST_DIR)\doc\python"
	for %%F in ($(DIST_PYTHON_FILES)) do copy .\%F "$(DIST_DIR)\lib"
	cd win32
endif

MAKE_DIST_SWIG_JAVA:
ifeq ($(WITH_SWIG_JAVA),yes)
	     cd ..
	-mkdir "$(DIST_DIR)\doc\java"
	for %%F in ($(DIST_DOC_SWIG_JAVA_FILES)) do copy .\%F "$(DIST_DIR)\doc\java"
	for %%F in ($(DIST_JAVA_FILES)) do copy .\%F "$(DIST_DIR)\lib"
	     cd win32
endif

MAKE_DIST_SWIG_CSHARP:
ifeq ($(WITH_SWIG_CSHARP),yes)
	cd ..
	-mkdir "$(DIST_DIR)\doc\csharp"
	for %%F in ($(DIST_DOC_SWIG_CSHARP_FILES)) do copy .\%F "$(DIST_DIR)\doc\csharp"
	for %%F in ($(DIST_CSHARP_FILES)) do copy .\%F "$(DIST_DIR)\lib"
	     cd win32
endif

MAKE_DIST_DIRS:
	cd ..
	-rmdir /s /q "$(DIST_DIR)"
	-mkdir "$(DIST_DIR)\doc"
	-mkdir "$(DIST_DIR)\bin"
	-mkdir "$(DIST_DIR)\lib"
	-mkdir "$(DIST_DIR)\etc"
ifeq ($(WITH_WXWIN),yes)
	-mkdir "$(DIST_DIR)\etc\qrytool"
endif
	-mkdir "$(DIST_DIR)\include"
	cd win32


MAKE_DIST_FILES: MAKE_DIST_SWIG
	cd ..
	for %%F in ($(PACKAGE_INCLUDE_HEADER_FILES) pcre_emdros.h pcre_config.h) do copy include\%F "$(DIST_DIR)\include"
	for %%F in ($(TECKIT_INCLUDE_HEADER_FILES)) do copy src\qrytool\TECkit\%F "$(DIST_DIR)\include"
	for %%F in ($(DIST_DOC_DOC_FILES)) do copy doc\%F "$(DIST_DIR)\doc"
	copy src\qrytool\doc\EQTUsersGuide.pdf "$(DIST_DIR)\doc\EmdrosQueryTool_UsersGuide.pdf"
	for %%F in ($(DIST_DOC_ROOT_FILES)) do copy .\%F "$(DIST_DIR)\doc"
	for %%F in ($(DIST_DOC_ROOT_FILES_ADD_TXT)) do copy .\%F "$(DIST_DIR)\doc\%F.txt"
	for %%F in ($(DIST_ROOT_FILES)) do copy .\%F "$(DIST_DIR)"
	for %%F in ($(DIST_BIN_FILES)) do copy .\%F "$(DIST_DIR)\bin"
	for %%F in ($(DIST_LIB_FILES)) do copy .\%F "$(DIST_DIR)\lib"
	for %%F in ($(DIST_ETC_QRYTOOL_FILES)) do copy .\%F "$(DIST_DIR)\etc\qrytool"
	copy win32\config.mak "$(DIST_DIR)\doc"
	cd win32


inst: dist
	cd ..\emdros-$(VERSION)-windows
	"$(NSISEXE)" emdros.nsi
	cd ..\win32

config.nsh:
	-del config.nsh
ifeq ($(SQLITE2),1)
	echo !define SQLITE2 >> config.nsh
endif
ifeq ($(SQLITE3),1)
	echo !define SQLITE3 >> config.nsh
endif
ifeq ($(MYSQL),1)
	echo !define MYSQL >> config.nsh
endif
ifeq ($(PGSQL),1)
	echo !define POSTGRESQL >> config.nsh
endif
ifeq ($(TARGET_CPU),X86)
	echo "Target is X86; not putting DO_X64 in config.nsh"
else
	echo !define DO_X64  >> config.nsh
endif

