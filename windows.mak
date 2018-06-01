# Emdros makefile for source root directory

# Makefile for Microsoft Visual C++ 5.0 (or compat)

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!MESSAGE Building Emdros on Windows
!MESSAGE

ALL:
   cd build\windows
   nmake /f windows.mak $(MAKEMACRO)
   cd ..\..

CLEAN:
   cd build\windows
   nmake /f windows.mak CLEAN
   cd ..\..

DISTCLEAN: CLEAN
   cd build\windows
   nmake /f windows.mak DISTCLEAN
   cd ..\..

