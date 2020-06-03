#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([emdros], [3.7.3], [ulrikp@emdros.org])
AC_CONFIG_MACRO_DIRS([m4])
LT_INIT
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([MQL/mql_R.cpp])
AC_CONFIG_HEADERS([include/emdros-config.h])
AC_PREFIX_DEFAULT(/usr/local)

# Unix/Linux .so library versioning
LIB_EMDROS_CURRENT=6
LIB_EMDROS_REVISION=0
LIB_EMDROS_AGE=0


AC_SUBST(LIB_EMDROS_CURRENT)
AC_SUBST(LIB_EMDROS_REVISION)
AC_SUBST(LIB_EMDROS_AGE)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AX_CXX_COMPILE_STDCXX(11, noext, optional)

EMDROS_BUILDENV
EMDROS_SOLARIS
EMDROS_MACOS




EMDROS_SQLITE3
EMDROS_POSTGRESQL
EMDROS_MYSQL
EMDROS_BPT
dnl EMDROS_MONGODB

EMDROS_PCRE
EMDROS_ZLIB



# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdint.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_C_RESTRICT
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Do wxWidgets, if necessary
EMDROS_WX_SUPPORT

# Do SWIG support, if necessary
EMDROS_SWIG_SUPPORT

# Find program dependencies for documentation
EMDROS_DOCS

# If on Sun Solaris, do various things
EMDROS_SOLARIS_END

EMDROS_BUILDENV_END

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 doc/cms/Makefile
                 doc/progref/Makefile
		 zlib/Makefile
		 pcre/Makefile
		 sqlite3/Makefile
		 sqlite3/tool/Makefile
		 sqlite3/src/Makefile	
		 include/Makefile	 
                 include/emdros-lconfig.h
                 EMdF/Makefile
                 MQL/Makefile
                 harvest/Makefile
                 util/Makefile
                 importers/Makefile
                 amalgamation/Makefile
                 src/Makefile
                 src/importtool/Makefile
                 src/importtool/doc/Makefile
                 src/qrytool/Makefile
                 src/qrytool/doc/Makefile
                 SWIG/Makefile
                 SWIG/csharp/Makefile
                 SWIG/java/Makefile
                 SWIG/php7/Makefile
                 SWIG/python/Makefile
                 SWIG/python3/Makefile
		 wx/Makefile
		 tests/Makefile
		 tests/asv/Makefile
		 tests/test_json/Makefile
		 win32/Makefile
                 generate_extrafiles.py 
                 emdros.pc
		 debian/rules
               ])
AC_OUTPUT

chmod 755 debian/rules



EMDROS_BANNER
