#
# Emdros Win32 Makefile for include directory
#
# Makefile for MinGW


WIN32DIR = ..\win32

include $(WIN32DIR)\config.mak

PROGRAM1 = .\get_version.exe
PROGRAM1_OBJS = get_version.o



EMDROS_LCONFIG_WIN32_H = win32-lconfig.h

TARGETS = pcre_emdros_h pcre_config_h lconfig
CLEANFILES= pcre_emdros.h pcre_config.h $(PROGRAM1) $(PROGRAM1_OBJS) $(EMDROS_LCONFIG_WIN32_H)


include $(WIN32DIR)\mingw-body.mak

pcre_emdros_h:
	copy /Y /B ..\pcre\pcre_emdros.h.win32 +..\pcre\pcre.h .\pcre_emdros.h

pcre_config_h:
	copy /Y /B ..\pcre\config.h.win32 .\pcre_config.h

lconfig:
	-erase -q emdros-lconfig.h
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
ifeq ($(PGSQL),1)
	echo #define USE_POSTGRESQL 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_POSTGRESQL 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
	echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
	echo #ifndef USE_MYSQL >> $(EMDROS_LCONFIG_WIN32_H)
ifeq ($(MYSQL),1)
	echo #define USE_MYSQL 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_MYSQL 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
	echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
	echo #ifndef USE_SQLITE3 >> $(EMDROS_LCONFIG_WIN32_H)
ifeq ($(SQLITE3),1)
	echo #define USE_SQLITE3 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_SQLITE3 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
	echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
	echo #ifndef USE_BPT >> $(EMDROS_LCONFIG_WIN32_H)
ifeq ($(BPT),1)
	echo #define USE_BPT 1 >> $(EMDROS_LCONFIG_WIN32_H)
else
	echo #define USE_BPT 0 >> $(EMDROS_LCONFIG_WIN32_H)
endif
	echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
	echo #ifndef DEFAULT_BACKEND_ENUM >> $(EMDROS_LCONFIG_WIN32_H)
	echo #define DEFAULT_BACKEND_ENUM $(DEFAULT_BACKEND_ENUM) >> $(EMDROS_LCONFIG_WIN32_H)
	echo #endif >> $(EMDROS_LCONFIG_WIN32_H)
	$(PROGRAM1) ..\configure.ac >> $(EMDROS_LCONFIG_WIN32_H)
	echo #endif /* WIN32LCONFIG__H__ */ >> $(EMDROS_LCONFIG_WIN32_H)
	copy /Y /B $(EMDROS_LCONFIG_WIN32_H) + .\emdros-lconfig.h.in .\emdros-lconfig.h
ifeq ($(SQLITE3),1)
	copy /Y /B ..\sqlite3\src\sqlite3.h .\sqlite3_emdros.h
endif

