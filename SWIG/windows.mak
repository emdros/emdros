#
# Emdros Win32 Makefile for SWIG directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 static library (non-debug) libmql.lib


!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!INCLUDE ..\win32\config.mak

ALL:
!IF "$(WITH_SWIG_PYTHON)" == "yes"
	cd python 
	nmake /f windows.mak
	cd ..
!ENDIF
!IF "$(WITH_SWIG_JAVA)" == "yes"
	cd java
	nmake /f windows.mak
	cd ..
!ENDIF
!IF "$(WITH_SWIG_CSHARP)" == "yes"
	cd csharp
	nmake /f windows.mak
	cd ..
!ENDIF


CLEAN:
	cd python 
	nmake /f windows.mak clean
	cd ..
	cd java 
	nmake /f windows.mak clean
	cd ..
	cd csharp
	nmake /f windows.mak clean
	cd ..

