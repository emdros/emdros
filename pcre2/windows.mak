#
# Emdros Win32 Makefile for pcre2 directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 static library (non-debug) libpcre2_emdros.lib
# and sqlite3_emdros.exe

TARGETNAME=libpcre2_emdros

BUILT_SOURCES = pcre2.h config.h

CPP_PROJ_EXTRA=/Gz 





WIN32DIR = ..\build\windows

!INCLUDE $(WIN32DIR)\config.mak

LIBTARGET = libpcre2_emdros.lib

TARGETS = $(BUILT_SOURCES) $(OUTDIR)\$(LIBTARGET)

LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES)


LOCAL_CPPFLAGS = /D PCRE2_CODE_UNIT_WIDTH=8 /D HAVE_CONFIG_H=1 \
   /D "NDEBUG=1" /I.

LIBTARGET_OBJS= \
     "$(INTDIR)\pcre2_auto_possess.obj" \
     "$(INTDIR)\pcre2_chartables.obj" \
     "$(INTDIR)\pcre2_compile.obj" \
     "$(INTDIR)\pcre2_config.obj" \
     "$(INTDIR)\pcre2_context.obj" \
     "$(INTDIR)\pcre2_dfa_match.obj" \
     "$(INTDIR)\pcre2_error.obj" \
     "$(INTDIR)\pcre2_find_bracket.obj" \
     "$(INTDIR)\pcre2_fuzzsupport.obj" \
     "$(INTDIR)\pcre2_maketables.obj" \
     "$(INTDIR)\pcre2_match.obj" \
     "$(INTDIR)\pcre2_match_data.obj" \
     "$(INTDIR)\pcre2_newline.obj" \
     "$(INTDIR)\pcre2_ord2utf.obj" \
     "$(INTDIR)\pcre2_pattern_info.obj" \
     "$(INTDIR)\pcre2_serialize.obj" \
     "$(INTDIR)\pcre2_string_utils.obj" \
     "$(INTDIR)\pcre2_study.obj" \
     "$(INTDIR)\pcre2_substitute.obj" \
     "$(INTDIR)\pcre2_substring.obj" \
     "$(INTDIR)\pcre2_tables.obj" \
     "$(INTDIR)\pcre2_ucd.obj" \
     "$(INTDIR)\pcre2_valid_utf.obj" \
     "$(INTDIR)\pcre2_xclass.obj" \
     "$(INTDIR)\pcre2_jit_compile.obj" 

!INCLUDE $(WIN32DIR)\body.mak


pcre2.h: pcre2.h.generic
	copy /B pcre2.h.generic pcre2.h

config.h: config.h.generic
	copy /B config.h.generic config.h

