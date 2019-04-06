#
# Emdros Win32 Makefile for include directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)


WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak


EMDROS_LCONFIG_WIN32_H = win32-lconfig.h


PROGRAM1 = .\get_version.exe
PROGRAM1_OBJS = "$(INTDIR)\get_version.obj"


TARGETS = $(PROGRAM1) pcre_emdros_h pcre_config_h lconfig
CLEANFILES= pcre_emdros.h pcre_config.h get_version.exe win32-lconfig.h



!INCLUDE $(WIN32DIR)\body.mak

pcre_emdros_h:
	copy /Y /B ..\pcre\pcre_emdros.h.win32 +..\pcre\pcre.h .\pcre_emdros.h

pcre_config_h:
	copy /Y /B ..\pcre\config.h.win32 .\pcre_config.h

lconfig:
    -erase -q .\emdros-lconfig.h
    -erase -q $(EMDROS_LCONFIG_WIN32_H)
    echo #ifndef WIN32LCONFIG__H__ >> $(EMDROS_LCONFIG_WIN32_H)
    echo #define WIN32LCONFIG__H__ >> $(EMDROS_LCONFIG_WIN32_H)

    echo #define LINUX 0 >> $(EMDROS_LCONFIG_WIN32_H)
    echo #define SUN 0 >> $(EMDROS_LCONFIG_WIN32_H)

    echo /* Don't use "system-wide" SQLite3. There is none on Win32. */ >> $(EMDROS_LCONFIG_WIN32_H)
    echo #define USE_SYSTEM_SQLITE3 (0) >> $(EMDROS_LCONFIG_WIN32_H)

    echo /* Don't use "system-wide" zlib. There is none on Win32. */ >> $(EMDROS_LCONFIG_WIN32_H)
    echo #define USE_SYSTEM_ZLIB (0) >> $(EMDROS_LCONFIG_WIN32_H)
    echo #ifndef USE_POSTGRESQL >> $(EMDROS_LCONFIG_WIN32_H)
!IF "$(PGSQL)"=="1"
    echo #define USE_POSTGRESQL 1 >> $(EMDROS_LCONFIG_WIN32_H)
!ELSE
    echo #define USE_POSTGRESQL 0 >> $(EMDROS_LCONFIG_WIN32_H)
!ENDIF
    echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
    echo #ifndef USE_MYSQL >> $(EMDROS_LCONFIG_WIN32_H)
!IF "$(MYSQL)"=="1"
    echo #define USE_MYSQL 1 >> $(EMDROS_LCONFIG_WIN32_H)
!ELSE
    echo #define USE_MYSQL 0 >> $(EMDROS_LCONFIG_WIN32_H)
!ENDIF
    echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
    echo #ifndef USE_SQLITE3 >> $(EMDROS_LCONFIG_WIN32_H)
!IF "$(SQLITE3)"=="1"
    echo #define USE_SQLITE3 1 >> $(EMDROS_LCONFIG_WIN32_H)
!ELSE
    echo #define USE_SQLITE3 0 >> $(EMDROS_LCONFIG_WIN32_H)
!ENDIF
    echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
    echo #ifndef USE_BPT >> $(EMDROS_LCONFIG_WIN32_H)
!IF "$(BPT)"=="1"
    echo #define USE_BPT 1 >> $(EMDROS_LCONFIG_WIN32_H)
!ELSE
    echo #define USE_BPT 0 >> $(EMDROS_LCONFIG_WIN32_H)
!ENDIF
    echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
    echo #ifndef DEFAULT_BACKEND_ENUM >> $(EMDROS_LCONFIG_WIN32_H)
    echo #define DEFAULT_BACKEND_ENUM $(DEFAULT_BACKEND_ENUM) >> $(EMDROS_LCONFIG_WIN32_H)
    echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
    $(PROGRAM1) ..\configure.ac >> $(EMDROS_LCONFIG_WIN32_H)
    echo #endif /* WIN32LCONFIG__H__ */ >> $(EMDROS_LCONFIG_WIN32_H)
    copy /Y /B $(EMDROS_LCONFIG_WIN32_H) + .\emdros-lconfig.h.in .\emdros-lconfig.h 
    
!IF "$(SQLITE3)"=="1"
    copy ..\sqlite3\src\sqlite3.h .\sqlite3_emdros.h
!ENDIF

