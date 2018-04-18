#
# Emdros Win32 Makefile for include directory
#
# Makefile for MinGW


WIN32DIR = ..\win32

include $(WIN32DIR)\config.mak


EMDROS_LCONFIG_WIN32_H = win32-lconfig.h

TARGETS = pcre_emdros_h lconfig
CLEANFILES= pcre_emdros.h


include $(WIN32DIR)\mingw-body.mak

pcre_emdros_h:
	copy /Y /B ..\pcre\pcre_emdros.h.win32 +..\pcre\pcre.h .\pcre_emdros.h

lconfig:
	-erase -q $(EMDROS_LCONFIG_WIN32_H)
	copy $(WIN32DIR)\lconfig.h.win32 .\$(EMDROS_LCONFIG_WIN32_H)
	echo #undef USE_POSTGRESQL >> $(EMDROS_LCONFIG_WIN32_H)
ifeq ($(PGSQL),1)
	echo #define USE_POSTGRESQL 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_POSTGRESQL 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
	echo #undef USE_MYSQL >> $(EMDROS_LCONFIG_WIN32_H)
ifeq ($(MYSQL),1)
	echo #define USE_MYSQL 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_MYSQL 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
ifeq ($(SQLITE3),1)
	echo #define USE_SQLITE3 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_SQLITE3 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
ifeq ($(BPT),1)
	echo #define USE_BPT 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_BPT 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
ifeq ($(BPT2),1)
	echo #define USE_BPT2 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_BPT2 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
	echo #undef DEFAULT_BACKEND_ENUM >> $(EMDROS_LCONFIG_WIN32_H)
	echo #define DEFAULT_BACKEND_ENUM $(DEFAULT_BACKEND_ENUM) >> $(EMDROS_LCONFIG_WIN32_H)


ifeq ($(SQLITE3),1)
	copy /Y /B ..\sqlite3\src\sqlite3.h .\sqlite3_emdros.h
endif

