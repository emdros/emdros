#
# Emdros Win32 Makefile for pcre directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libpcre_emdros.lib

WIN32DIR = ..\win32


LOCAL_CPPFLAGS = -I. -DPCRE_STATIC -DHAVE_CONFIG_H


!IF "$(BUILD_FOR_DEVEL)" == "1"
LIBTARGET = libpcre_emdros.lib
TARGETS = pcre_config_h pcre_emdros_h "$(OUTDIR)\$(LIBTARGET)"
!ELSE
LIBTARGET = 
TARGETS = 
!ENDIF

CLEANFILES = chartables.c dftables.exe 

LIBTARGET_OBJS= \
  	"$(INTDIR)\pcre_byte_order.obj" \
  	"$(INTDIR)\pcre_chartables.obj" \
  	"$(INTDIR)\pcre_compile.obj" \
  	"$(INTDIR)\pcre_config.obj" \
  	"$(INTDIR)\pcre_dfa_exec.obj" \
  	"$(INTDIR)\pcre_exec.obj" \
  	"$(INTDIR)\pcre_fullinfo.obj" \
  	"$(INTDIR)\pcre_get.obj" \
  	"$(INTDIR)\pcre_globals.obj" \
  	"$(INTDIR)\pcre_jit_compile.obj" \
  	"$(INTDIR)\pcre_maketables.obj" \
  	"$(INTDIR)\pcre_newline.obj" \
  	"$(INTDIR)\pcre_ord2utf8.obj" \
  	"$(INTDIR)\pcre_refcount.obj" \
  	"$(INTDIR)\pcre_string_utils.obj" \
  	"$(INTDIR)\pcre_study.obj" \
  	"$(INTDIR)\pcre_tables.obj" \
  	"$(INTDIR)\pcre_ucd.obj" \
  	"$(INTDIR)\pcre_valid_utf8.obj" \
  	"$(INTDIR)\pcre_version.obj" \
  	"$(INTDIR)\pcre_xclass.obj" 







#PROGRAM1 = .\dftables.exe
#PROGRAM1_OBJS = "$(INTDIR)\dftables.obj"


!INCLUDE $(WIN32DIR)\config.mak
!INCLUDE $(WIN32DIR)\body.mak


..\include\pcre_emdros.h:
    copy /Y /B pcre_emdros.h.win32 +pcre.h ..\include\pcre_emdros.h

#pcre_maketables.c: pcre_chartables.c

#pcre_chartables.c: .\dftables.exe
#	.\dftables.exe pcre_chartables.c

pcre_emdros_h:
	copy /Y /B pcre_emdros.h.win32 +pcre.h ..\include\pcre_emdros.h

pcre_config_h:
	copy /Y /B config.h.win32 ..\include\pcre_config.h
        copy /Y /B config.h.win32 .\config.h
