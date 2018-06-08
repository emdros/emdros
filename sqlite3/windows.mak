# Emdros makefile for sqlite3 directory

# Makefile for Microsoft Visual C++ 5.0 (or compat)

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!MESSAGE Building sqlite on Win32
!MESSAGE

!INCLUDE ..\build\windows\config.mak

ALL: 
   cd src
   nmake /f windows.mak $(MAKEMACRO) 
   cd ..

CLEANFILES = *~

CLEAN:
   cd src
   nmake /f windows.mak CLEAN
   cd ..
   -@erase $(CLEANFILES)


DISTCLEAN: CLEAN


