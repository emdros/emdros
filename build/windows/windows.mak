# Emdros makefile for build\windows directory

# Makefile for Microsoft Visual C++ 10.0 (or compat)

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!MESSAGE Building Emdros on Windows
!MESSAGE

!INCLUDE .\config.mak

!INCLUDE ..\..\include\Makefile.inc

ALL: config.nsh 
   copy config.h.win32 ..\..\include\emdros-config.h
!IF "$(MYSQL)" == "1"
   -mkdir ..\..\build\windows\mysql_include
   -mkdir ..\..\build\windows\mysql_include\mysql
   -mkdir ..\..\build\windows\mysql_include\mysql\psi
   copy "$(MYSQLTOP)\include\*.*" "..\..\build\windows\mysql_include\"
   copy "$(MYSQLTOP)\include\mysql\*.*" "..\..\build\windows\mysql_include\mysql\"
   copy "$(MYSQLTOP)\include\mysql\psi\*.*" "..\..\build\windows\mysql_include\mysql\psi\"
!ENDIF
   cd ..\..\include
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..\pcre2
   nmake /f windows.mak $(MAKEMACRO) 
!IF "$(BPT)" == "1"
   cd ..\zlib
   nmake /f windows.mak $(MAKEMACRO) 
!ENDIF
!IF "$(BPT2)" == "1"
   cd ..\zlib
   nmake /f windows.mak $(MAKEMACRO) 
!ENDIF
!IF "$(SQLITE3)" == "1"
   cd ..\sqlite3
   nmake /f windows.mak $(MAKEMACRO) 
!ENDIF
   cd ..\EMdF
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..\MQL
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..\util
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..\harvest
   nmake /f windows.mak $(MAKEMACRO) 
!IF "$(EMDROS_ONLY)"=="0"
   cd ..\importers
   nmake /f windows.mak $(MAKEMACRO)
!IF "$(WITH_WXWIN)"=="yes"
   cd ..\wx
   nmake /f windows.mak $(MAKEMACRO) 
!ENDIF
   cd ..\src\qrytool\TECkit
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..\..\..\src\qrytool
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..\..\src\importtool
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..
!ENDIF
   cd ..\src
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..\tests
   nmake /f windows.mak $(MAKEMACRO) 
!IF "$(EMDROS_ONLY)"=="0"
   cd ..\SWIG
   nmake /f windows.mak $(MAKEMACRO) 
!ENDIF 
   cd ..\..\build\windows
   echo All Windows parts have been built!


CLEAN:
   -del config.nsh
   cd ..\..\include
   nmake /f windows.mak CLEAN
!IF "$(SQLITE3)" == "1"
   cd ..\sqlite
   nmake /f windows.mak CLEAN
!ENDIF
   cd ..\EMdF
   nmake /f windows.mak CLEAN
   cd ..\pcre2
   nmake /f windows.mak CLEAN
   cd ..\MQL
   nmake /f windows.mak CLEAN
   cd ..\util
   nmake /f windows.mak CLEAN
   cd ..\harvest
   nmake /f windows.mak CLEAN
   cd ..\importers
   nmake /f windows.mak CLEAN
   cd ..\wx
   nmake /f windows.mak CLEAN
   cd ..\src\qrytool\TECkit
   nmake /f windows.mak CLEAN
   cd ..\..\..\src\qrytool
   nmake /f windows.mak CLEAN
   cd ..
   nmake /f windows.mak CLEAN
   cd ..\tests
   nmake /f windows.mak CLEAN
   cd ..\SWIG
   nmake /f windows.mak CLEAN
   cd ..\..\build\windows
   -@erase *~
   -@rmdir Release\ /s /q
   -@rmdir ReleaseUnicode\ /s /q
   -@rmdir Debug\ /s /q
   -@rmdir DebugUnicode\ /s /q
   echo All Windows parts have been cleaned!

DISTCLEAN: CLEAN
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
                   sfmimport.html tigerxmlimport.html bptdump.html 
                   


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
                 win32\Release\libharvest.lib \
                 win32\Release\libemdrosgui.lib \
                 win32\ReleaseUnicode\libemdrosguiu.lib \
                 win32\Release\libteckit.lib \
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


DIST: ALL
   nmake /f windows.mak DIST_DIR="emdros-windows" MAKE_DIST_DIRS
   nmake /f windows.mak DIST_DIR="emdros-windows" MAKE_DIST_FILES
!IF "$(PGSQL)" == "1"
   nmake /f windows.mak  DIST_PGSQL
!ENDIF
!IF "$(MYSQL)" == "1"
   nmake /f windows.mak  DIST_MYSQL
!ENDIF
!IF "$(SQLITE3)" == "1"
   nmake /f windows.mak  DIST_SQLITE3
!ENDIF
   cd ..
   cd win32


DIST_MYSQL: 
   nmake /f windows.mak DIST_DIR="emdros-windows" MAKE_MY_DIST_FILES

DIST_PGSQL: 
   nmake /f windows.mak DIST_DIR="emdros-windows" MAKE_PG_DIST_FILES

DIST_SQLITE3: 
   nmake /f windows.mak DIST_DIR="emdros-windows" MAKE_SQLITE3_DIST_FILES


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
!IF "$(WITH_WXWIN)"=="yes"
	-mkdir "$(DIST_DIR)\etc\qrytool"
!ENDIF
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
	copy build\windows\config.mak "$(DIST_DIR)\doc"
	cd build\windows


INST: DIST
	cd ..\emdros-windows
        "$(NSISEXE)" emdros.nsi
        cd ..\..\build\windows

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
