#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([emdros], [3.6.0], [ulrikp@scripturesys.com])
AC_CONFIG_MACRO_DIRS([m4])
LT_INIT
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([MQL/mql_R.cpp])
AC_CONFIG_HEADERS([include/emdros-config.h])


# Unix/Linux .so library versioning
LIB_EMDROS_CURRENT=5
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
EMDROS_BPT2
EMDROS_MONGODB

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


EMDROS_BUILDENV_END

AC_CONFIG_FILES([Makefile
                 build/Makefile
                 build/rpm/Makefile
                 doc/Makefile
                 doc/cms/Makefile
                 contrib/Makefile
                 contrib/zlib/Makefile
                 contrib/pcre2/Makefile
                 contrib/sqlite3/Makefile
                 contrib/sqlite3/src/Makefile
		 lib/Makefile
                 lib/base/emdros-lconfig.h
                 lib/base/version-emdros.h
                 lib/base/Makefile
                 lib/EMdF/Makefile
                 lib/MQL/Makefile
                 lib/harvest/Makefile
                 lib/util/Makefile
                 lib/importers/Makefile
                 amalgamation/Makefile
                 src/Makefile
                 src/cli/Makefile
                 src/gui/Makefile
                 src/gui/art/Makefile
                 src/gui/wx/Makefile 
                 src/gui/importtool/Makefile
		 src/gui/importtool/doc/Makefile
                 src/gui/querytool/Makefile
                 src/gui/querytool/doc/Makefile
                 bindings/Makefile
                 bindings/SWIG/Makefile
                 bindings/SWIG/csharp/Makefile
                 bindings/SWIG/java/Makefile
                 bindings/SWIG/php7/Makefile
                 bindings/SWIG/python2/Makefile
                 bindings/SWIG/python3/Makefile
		 tests/Makefile
		 tests/asv/Makefile
		 tests/test_json/Makefile
                 build/rpm/generate_extrafiles.py 
                 build/rpm/emdros.spec
                 emdros.pc
		 debian/rules
               ])
AC_OUTPUT

chmod 755 debian/rules



EMDROS_BANNER
