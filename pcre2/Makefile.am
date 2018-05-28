BUILT_SOURCES = pcre2.h config.h


pcre2.h: pcre2.h.generic
	cp pcre2.h.generic pcre2.h

config.h: config.h.generic
	cp config.h.generic config.h

if DO_DEVEL
MAYBE_LIBS_PCRE2 = libpcre2_emdros.la
else
MAYBE_LIBS_PCRE2 = 
endif

lib_LTLIBRARIES = $(MAYBE_LIBS_PCRE2)

AM_CFLAGS = -DPCRE2_CODE_UNIT_WIDTH=8  -DLINK_SIZE=2  -DSUPPORT_UNICODE=1

pcre2_emdros_src = \
                   pcre2_auto_possess.c \
                   pcre2_chartables.c \
                   pcre2_compile.c \
                   pcre2_config.c \
                   pcre2_context.c \
                   pcre2_dfa_match.c \
                   pcre2_error.c \
                   pcre2_find_bracket.c \
                   pcre2_fuzzsupport.c \
                   pcre2_maketables.c \
                   pcre2_match.c \
                   pcre2_match_data.c \
                   pcre2_newline.c \
                   pcre2_ord2utf.c \
                   pcre2_pattern_info.c \
                   pcre2_serialize.c \
                   pcre2_string_utils.c \
                   pcre2_study.c \
                   pcre2_substitute.c \
                   pcre2_substring.c \
                   pcre2_tables.c \
                   pcre2_ucd.c \
                   pcre2_valid_utf.c \
                   pcre2_xclass.c \
                   pcre2_jit_compile.c 

pcre2_emdros_hdr = \
                   pcre2_internal.h \
                   pcre2_intmodedep.h \
                   pcre2_ucp.h \
                   pcre2_jit_match.c \
                   pcre2_jit_misc.c



noinst_HEADERS = $(pcre2_emdros_hdr)

#
# libpcre2_emdros.la
#
libpcre2_emdros_la_SOURCES = $(pcre2_emdros_src)
libpcre2_emdros_la_LIBADD = 
libpcre2_emdros_la_LDFLAGS = $(MACLDFLAGS) -version-info \
                     @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                     @SUN_LDFLAGS@ \
                     @DARWIN_LDFLAGS@

EXTRA_DIST = COPYING LICENCE \
             pcre2.h.generic pcre2.h.in \
             config.h.generic config.h.in
