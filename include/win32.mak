#
# Emdros Win32 Makefile for include directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)


WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak


EMDROS_LCONFIG_WIN32_H = win32-lconfig.h

TARGETS = pcre_emdros_h lconfig
CLEANFILES= pcre_emdros.h


!INCLUDE $(WIN32DIR)\body.mak

pcre_emdros_h:
	copy /Y /B ..\pcre\pcre_emdros.h.win32 +..\pcre\pcre.h .\pcre_emdros.h

lconfig:
    -erase -q $(EMDROS_LCONFIG_WIN32_H)
    copy $(WIN32DIR)\lconfig.h.win32 .\$(EMDROS_LCONFIG_WIN32_H)
    echo #undef USE_POSTGRESQL >> $(EMDROS_LCONFIG_WIN32_H)
!IF "$(PGSQL)"=="1"
    echo #define USE_POSTGRESQL 1 >> $(EMDROS_LCONFIG_WIN32_H)
!ELSE
    echo #define USE_POSTGRESQL 0 >> $(EMDROS_LCONFIG_WIN32_H)
!ENDIF
    echo #undef USE_MYSQL >> $(EMDROS_LCONFIG_WIN32_H)
!IF "$(MYSQL)"=="1"
    echo #define USE_MYSQL 1 >> $(EMDROS_LCONFIG_WIN32_H)
!ELSE
    echo #define USE_MYSQL 0 >> $(EMDROS_LCONFIG_WIN32_H)
!ENDIF
!IF "$(SQLITE3)"=="1"
    echo #define USE_SQLITE3 1 >> $(EMDROS_LCONFIG_WIN32_H)
!ELSE
    echo #define USE_SQLITE3 0 >> $(EMDROS_LCONFIG_WIN32_H)
!ENDIF
!IF "$(BPT)"=="1"
    echo #define USE_BPT 1 >> $(EMDROS_LCONFIG_WIN32_H)
!ELSE
    echo #define USE_BPT 0 >> $(EMDROS_LCONFIG_WIN32_H)
!ENDIF
    echo #undef DEFAULT_BACKEND_ENUM >> $(EMDROS_LCONFIG_WIN32_H)
    echo #define DEFAULT_BACKEND_ENUM $(DEFAULT_BACKEND_ENUM) >> $(EMDROS_LCONFIG_WIN32_H)



