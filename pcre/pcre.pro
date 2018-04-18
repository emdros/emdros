TEMPLATE = lib

TARGET = pcre_emdros

CONFIG = make_sources copyheader staticlib 

VERSION = 3.2.1

INCLUDEPATH = . ../include


SOURCES = \
  pcre_byte_order.c \
  pcre_chartables.c \
  pcre_compile.c \
  pcre_config.c \
  pcre_dfa_exec.c \
  pcre_exec.c \
  pcre_fullinfo.c \
  pcre_get.c \
  pcre_globals.c \
  pcre_jit_compile.c \
  pcre_maketables.c \
  pcre_newline.c \
  pcre_ord2utf8.c \
  pcre_refcount.c \
  pcre_string_utils.c \
  pcre_study.c \
  pcre_tables.c \
  pcre_ucd.c \
  pcre_valid_utf8.c \
  pcre_version.c \
  pcre_xclass.c 

make_sources {
  win32 {
      system(copy /Y config.h.win32 /B ..\\include\\pcre_config.h /B)
  }
  unix {
      system(cp -pf config.h ../include/pcre_config.h)
  }
  macx {
      system(cp -pf config.h ../include/pcre_config.h)
  }
}

copyheader {
  win32 {
      system(copy /Y pcre.h ..\\include\\pcre_emdros.h)
  }
  unix {
      system(cp -f pcre.h ../include/pcre_emdros.h)
  }
  macx {
      system(cp -f pcre.h ../include/pcre_emdros.h)
  }
}


	

#macx:QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
macx:CONFIG += x86_64 



