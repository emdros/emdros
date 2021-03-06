# Emdros makefile for win32 directory

# Makefile for Microsoft Visual C++ 5.0 (or compat)

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!MESSAGE Building Emdros on Win32
!MESSAGE

!INCLUDE .\config.mak

!INCLUDE ..\include\Makefile.inc

ALL: config.nsh 
   copy config.h.win32 ..\include\emdros-config.h
!IF "$(MYSQL)" == "1"
   -mkdir ..\win32\mysql_include
   -mkdir ..\win32\mysql_include\mysql
   -mkdir ..\win32\mysql_include\mysql\psi
   copy "$(MYSQLTOP)\include\*.*" "..\win32\mysql_include\"
   copy "$(MYSQLTOP)\include\mysql\*.*" "..\win32\mysql_include\mysql\"
   copy "$(MYSQLTOP)\include\mysql\psi\*.*" "..\win32\mysql_include\mysql\psi\"
!ENDIF
   cd ..\include
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\pcre
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\zlib
   $(MAKE) /f win32.mak $(MAKEARGS) 
!IF "$(SQLITE3)" == "1"
   cd ..\sqlite3
   $(MAKE) /f win32.mak $(MAKEARGS) 
!ENDIF
   cd ..\EMdF
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\MQL
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\util
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\importers
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\harvest
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\amalgamation
   $(MAKE) /f win32.mak $(MAKEARGS) 
!IF "$(EMDROS_ONLY)"=="0"
!IF "$(WITH_WXWIN)"=="yes"
   cd ..\wx
   $(MAKE) /f win32.mak $(MAKEARGS) 
!ENDIF
   cd ..\src\qrytool
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\..\src\importtool
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..
!ENDIF
   cd ..\src
   $(MAKE) /f win32.mak $(MAKEARGS) 
   cd ..\tests
   $(MAKE) /f win32.mak $(MAKEARGS) 
!IF "$(EMDROS_ONLY)"=="0"
   cd ..\SWIG
   $(MAKE) /f win32.mak $(MAKEARGS) 
!ENDIF 
   cd ..\win32
   echo All Win32 parts have been built!


CLEAN:
   -del config.nsh
   cd ..\include
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
!IF "$(SQLITE3)" == "1"
   cd ..\sqlite3
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
!ENDIF
   cd ..\EMdF
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\pcre
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\zlib
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\MQL
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\util
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\importers
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\harvest
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\wx
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\src\qrytool
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\tests
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\SWIG
   $(MAKE) /f win32.mak $(MAKEARGS) CLEAN
   cd ..\win32
   -@erase *~
   -@rmdir Release\ /s /q
   -@rmdir ReleaseUnicode\ /s /q
   -@rmdir ReleaseUnicode2\ /s /q
   -@rmdir ReleaseUnicode3\ /s /q
   -@rmdir ReleaseUnicode4\ /s /q
   -@rmdir ReleaseUnicode5\ /s /q
   -@rmdir ReleaseUnicode6\ /s /q
   -@rmdir ReleaseUnicode7\ /s /q
   -@rmdir Debug\ /s /q
   -@rmdir DebugUnicode\ /s /q
   -@rmdir DebugUnicode2\ /s /q
   -@rmdir DebugUnicode3\ /s /q
   -@rmdir DebugUnicode4\ /s /q
   -@rmdir DebugUnicode5\ /s /q
   -@rmdir DebugUnicode6\ /s /q
   -@rmdir DebugUnicode7\ /s /q
   echo All Win32 parts have been cleaned!

DISTCLEAN: CLEAN
   -del ..\config.h


TEST: ALL
   cd ..\tests
   $(MAKE) /f win32.mak $(MAKEARGS) test
   cd ..\win32
   echo SUCCESS: All tests have passed


DIST_DOC_DOC_FILES = \
                   progref\EmdrosProgRefGuide.pdf \
                   progref\emdros-progref-guide-$(VERSION).zip

DIST_DOC_MAN1_FILES = \
                   mql.html mqldump.html manage_indices.html upgrade_db.html \
                   emdftry.html jsontry.html mqltry.html mqllingtry.html \
                   fts_indexer.html fts_harvester.html \
                   jsontry.html renderobjects.html \
                   eqtc.html eqtu.html \
                   ubimport.html pennimport.html negraimport.html \
                   plaintextimport.html slashedtextimport.html \
                   sfmimport.html tigerxmlimport.html bptdump.html 

DIST_DOC_MAN5_FILES = \
                   fts_filters.html emdros.html
                   


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

!IF "$(WITH_WXWIN)"=="yes"
DIST_WX_BIN = win32\ReleaseUnicode\eqtu.exe \
              win32\ReleaseUnicode\EmdrosImportToolUnicode.exe \
              win32\ReleaseUnicode\eqtu.exe.manifest \
              win32\ReleaseUnicode\EmdrosImportToolUnicode.exe.manifest \
              src\qrytool\doc\EQTUsersGuide.htb 
DIST_WX_LIB = win32\Release\libemdrosgui.lib \
              win32\ReleaseUnicode\libemdrosguiu.lib 
!ELSE
DIST_WX_BIN =
DIST_WX_LIB =
!ENDIF


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
                 win32\Release\bptdetach.exe \
                 win32\Release\eqtc.exe \
                 win32\Release\emdros_qdtry.exe \
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
                 win32\Release\libemdros_importers.lib \
                 win32\Release\libharvest.lib \
                 win32\Release\libemdrosgui.lib \
                 win32\ReleaseUnicode\libemdrosguiu.lib \
                 win32\Release\libemdros_amalgamation.lib \
                 $(DIST_WX_LIB)

DIST_ETC_QRYTOOL_FILES = src\qrytool\default.cfg \
                 src\qrytool\av.cfg \
                 src\qrytool\penn.cfg \
                 src\qrytool\wh.cfg \
                 src\qrytool\bhs4_trees.cfg \
                 src\qrytool\bhs4_bracketed.cfg \
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




DIST_SQLITE3_LIB_FILES = win32\Release\libsqlite3_emdros.lib

DIST_SQLITE3_BIN_FILES = win32\Release\sqlite3_emdros.exe



DIST: ALL
   $(MAKE) /f win32.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_DIST_DIRS
   $(MAKE) /f win32.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_DIST_FILES
!IF "$(PGSQL)" == "1"
   $(MAKE) /f win32.mak  DIST_PGSQL
!ENDIF
!IF "$(MYSQL)" == "1"
   $(MAKE) /f win32.mak  DIST_MYSQL
!ENDIF
!IF "$(SQLITE3)" == "1"
   $(MAKE) /f win32.mak  DIST_SQLITE3
!ENDIF
   cd ..
   cd win32


DIST_MYSQL: 
   $(MAKE) /f win32.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_MY_DIST_FILES

DIST_PGSQL: 
   $(MAKE) /f win32.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_PG_DIST_FILES

DIST_SQLITE3: 
   $(MAKE) /f win32.mak DIST_DIR="emdros-$(VERSION)-windows" MAKE_SQLITE3_DIST_FILES

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

MAKE_SQLITE3_DIST_FILES:
   cd ..
   for %%F in ($(DIST_SQLITE3_LIB_FILES)) do copy %F "$(DIST_DIR)\lib"
   for %%F in ($(DIST_SQLITE3_BIN_FILES)) do copy %F "$(DIST_DIR)\bin"
   cd win32

MAKE_DIST_SWIG: MAKE_DIST_SWIG_PYTHON MAKE_DIST_SWIG_JAVA MAKE_DIST_SWIG_CSHARP

MAKE_DIST_SWIG_PYTHON:
!IF "$(WITH_SWIG_PYTHON)" == "yes"
	cd ..
	-mkdir "$(DIST_DIR)\doc\python"
	for %%F in ($(DIST_DOC_SWIG_PYTHON_FILES)) do copy .\%F "$(DIST_DIR)\doc\python"
	for %%F in ($(DIST_PYTHON_FILES)) do copy .\%F "$(DIST_DIR)\lib"
        cd win32
!ENDIF

MAKE_DIST_SWIG_JAVA:
!IF "$(WITH_SWIG_JAVA)" == "yes"
        cd ..
	-mkdir "$(DIST_DIR)\doc\java"
	for %%F in ($(DIST_DOC_SWIG_JAVA_FILES)) do copy .\%F "$(DIST_DIR)\doc\java"
	for %%F in ($(DIST_JAVA_FILES)) do copy .\%F "$(DIST_DIR)\lib"
        cd win32
!ENDIF

MAKE_DIST_SWIG_CSHARP:
!IF "$(WITH_SWIG_CSHARP)" == "yes"
	cd ..
	-mkdir "$(DIST_DIR)\doc\csharp"
	for %%F in ($(DIST_DOC_SWIG_CSHARP_FILES)) do copy .\%F "$(DIST_DIR)\doc\csharp"
	for %%F in ($(DIST_CSHARP_FILES)) do copy .\%F "$(DIST_DIR)\lib"
        cd win32
!ENDIF

MAKE_DIST_DIRS:
	cd ..
	-rmdir /s /q "$(DIST_DIR)"
	-mkdir "$(DIST_DIR)\doc"
	-mkdir "$(DIST_DIR)\bin"
	-mkdir "$(DIST_DIR)\lib"
	-mkdir "$(DIST_DIR)\etc"
	-mkdir "$(DIST_DIR)\etc\qrytool"
	-mkdir "$(DIST_DIR)\include"
	cd win32


MAKE_DIST_FILES: MAKE_DIST_SWIG
	cd ..
	for %%F in ($(PACKAGE_INCLUDE_HEADER_FILES) pcre_emdros.h pcre_config.h) do copy include\%F "$(DIST_DIR)\include"
	for %%F in ($(DIST_DOC_DOC_FILES)) do copy doc\%F "$(DIST_DIR)\doc"
	for %%F in ($(DIST_DOC_MAN1_FILES)) do copy doc\man1\%F "$(DIST_DIR)\doc"
	for %%F in ($(DIST_DOC_MAN5_FILES)) do copy doc\man5\%F "$(DIST_DIR)\doc"
	copy src\qrytool\doc\EQTUsersGuide.pdf "$(DIST_DIR)\doc\EmdrosQueryTool_UsersGuide.pdf"
	for %%F in ($(DIST_DOC_ROOT_FILES)) do copy .\%F "$(DIST_DIR)\doc"
	for %%F in ($(DIST_DOC_ROOT_FILES_ADD_TXT)) do copy .\%F "$(DIST_DIR)\doc\%F.txt"
	for %%F in ($(DIST_ROOT_FILES)) do copy .\%F "$(DIST_DIR)"
	for %%F in ($(DIST_BIN_FILES)) do copy .\%F "$(DIST_DIR)\bin"
	for %%F in ($(DIST_LIB_FILES)) do copy .\%F "$(DIST_DIR)\lib"
	for %%F in ($(DIST_ETC_QRYTOOL_FILES)) do copy .\%F "$(DIST_DIR)\etc\qrytool"
	copy win32\config.mak "$(DIST_DIR)\doc"
	cd win32


INST: TEST DIST
	cd ..\emdros-$(VERSION)-windows
        "$(NSISEXE)" emdros.nsi
        cd ..\win32

config.nsh:
   -del config.nsh
!IF "$(SQLITE3)" == "1"
   echo !define SQLITE3 >> config.nsh
!ENDIF
!IF "$(MYSQL)" == "1"
   echo !define MYSQL >> config.nsh
!ENDIF
!IF "$(PGSQL)" == "1"
   echo !define POSTGRESQL >> config.nsh
!ENDIF
!IF "$(TARGET_CPU)" == "X86"

!ELSE
   echo !define DO_X64  >> config.nsh
!ENDIF
