dnl
dnl Should come at the very end.
dnl
AC_DEFUN([EMDROS_BUILDENV_END], [

AC_ARG_WITH(default-backend,
[  --with-default-backend         Default backend to use (postgresql,mysql,sqlite3)],
   [case "${withval}" in
       postgresql) DEFAULT_BACKEND=postgresql ;;
       mysql)  DEFAULT_BACKEND=mysql ;;
       sqlite3)  DEFAULT_BACKEND=sqlite3 ;;
       *)   DEFAULT_BACKEND=none
     esac],
   [DEFAULT_BACKEND=none])


case "$DEFAULT_BACKEND" in
   postgresql ) DEFAULT_BACKEND_ENUM="kPostgreSQL" ;;
   mysql) DEFAULT_BACKEND_ENUM="kMySQL" ;;
   sqlite3) DEFAULT_BACKEND_ENUM="kSQLite3" ;;
   none) DEFAULT_BACKEND_ENUM="none" ;;
   *) AC_MSG_ERROR([
   Error in configure script: This should not occur. Please contact
   the maintainer of Emdros via the email address 

      ulrikp<write-the-sign>scripturesys.com

   telling him the exact parameters you gave to configure.

   Thank you.
])
esac

if test "x$DEFAULT_BACKEND_ENUM" = "xnone"; then
   USE_CONFIGURE_DEFAULT_BACKEND_ENUM=0
else	
   USE_CONFIGURE_DEFAULT_BACKEND_ENUM=1
fi

AC_SUBST(DEFAULT_BACKEND_ENUM)
AC_SUBST(USE_CONFIGURE_DEFAULT_BACKEND_ENUM)

dnl
dnl Set BACKEND_LDADD and friends
dnl 
BACKEND_LDADD=""
BACKEND_LDADD_BACKBACK=""
dnl This must not be empty, or node.js bindings will fail to build, at least
dnl on Mac OS X.
BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION -lm"
BACKEND_LDADD_AMALGAMATION="$BACKEND_LDADD_AMALGAMATION -lm"

if test "x$DO_POSTGRESQL" != "xno"; then
  BACKEND_LDADD="$BACKEND_LDADD $PG_LDADD";
  BACKEND_LDADD_BACKBACK="$BACKEND_LDADD_BACKBACK $PG_LDADD";
fi

if test "x$DO_MYSQL" != "xno"; then
  BACKEND_LDADD="$BACKEND_LDADD $MY_LDADD";
  BACKEND_LDADD_BACKBACK="$BACKEND_LDADD_BACKBACK $MY_LDADD";
fi

if test "x$DO_MONOGDB" != "xno"; then
  BACKEND_LDADD="$BACKEND_LDADD $LIBMONGOC_LIBS $LIBBSON_LIBS";
  BACKEND_LDADD_BACKBACK="$BACKEND_LDADD_BACKBACK $LIBMONGOC_LIBS $LIBBSON_LIBS";
fi

if test "x$USE_BPT" = "x1"; then
   addlibz="yes"
fi




if test x$addlibz = xyes; then
  BACKEND_LDADD="$BACKEND_LDADD $ZLIB_LDADD";
  BACKEND_LDADD_BACKBACK="$BACKEND_LDADD_BACKBACK $ZLIB_LDADD_BACKBACK";
  BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION -lz"
  BACKEND_LDADD_AMALGAMATION="$BACKEND_LDADD_AMALGAMATION -lz"
fi

if test "x$DO_SQLITE3" != "xno" && test "x$USE_AMALGAMATION" != "x1"; then
  BACKEND_LDADD="$BACKEND_LDADD $SQLITE3_LDADD";
  BACKEND_LDADD_BACKBACK="$BACKEND_LDADD_BACKBACK $SQLITE3_LDADD_BACKBACK";
fi

PKGCONFIG_LIBS_AMALGAMATION="-lemdros_amalgamation $BACKEND_LIBS_AMALGAMATION"

AC_SUBST(BACKEND_LDADD)
AC_SUBST(BACKEND_LDADD_BACKBACK)
AC_SUBST(BACKEND_LIBS_AMALGAMATION)
AC_SUBST(BACKEND_LDADD_AMALGAMATION)
AC_SUBST(PKGCONFIG_REQUIRED_AMALGAMATION)
AC_SUBST(PKGCONFIG_LIBS_AMALGAMATION)



dnl
dnl Set program-specific LDADD
dnl
if test "x$COMPILER_IS_GPLUSPLUS" = "xyes"; then
  PROGRAMLDADD=""
  SWIG_EXTRA_LIBS=""
else
  PROGRAMLDADD=""
  SWIG_EXTRA_LIBS=""
fi


dnl
dnl Set EMDF_LDADD and EMDF_DEPS, among others
dnl
if test x$DEVEL = xyes; then
   dnl DEVEL=yes


   EMDF_LDADD="../EMdF/libemdf.la $BACKEND_LDADD";
   EMDF_LDADD_BACKBACK="../../EMdF/libemdf.la $BACKEND_LDADD_BACKBACK";
   EMDF_LDADD_BACKBACKBACK="../../../EMdF/libemdf.la $BACKEND_LDADD_BACKBACKBACK";
   EMDF_DEPS="../EMdF/libemdf.la";
   EMDF_DEPS_BACKBACK="../../EMdF/libemdf.la";
   EMDF_DEPS_BACKBACKBACK="../../../EMdF/libemdf.la";

   EMDF_LDADD="$EMDF_LDADD $PROGRAMLDADD"

   MQL_LDADD="../MQL/libmql.la"
   MQL_LDADD_BACKBACK="../../MQL/libmql.la"
   MQL_LDADD_BACKBACKBACK="../../../MQL/libmql.la"
   MQL_DEPS="../MQL/libmql.la"
   MQL_DEPS_BACKBACK="../../MQL/libmql.la"
   MQL_DEPS_BACKBACKBACK="../../../MQL/libmql.la"

   HARVEST_LDADD="../harvest/libharvest.la"
   HARVEST_DEPS="../harvest/libharvest.la"
   HARVEST_LDADD_BACKBACK="../../harvest/libharvest.la"
   HARVEST_DEPS_BACKBACK="../../harvest/libharvest.la"
   HARVEST_LDADD_BACKBACKBACK="../../../harvest/libharvest.la"
   HARVEST_DEPS_BACKBACKBACK="../../../harvest/libharvest.la"

   UTIL_LDADD="../util/libutil_emdros.la"
   UTIL_DEPS="../util/libutil_emdros.la"
   UTIL_LDADD_BACKBACK="../../util/libutil_emdros.la"
   UTIL_DEPS_BACKBACK="../../util/libutil_emdros.la"
   UTIL_LDADD_BACKBACKBACK="../../../util/libutil_emdros.la"
   UTIL_DEPS_BACKBACKBACK="../../../util/libutil_emdros.la"

   IMPORTERS_LDADD="../importers/libemdros_importers.la"
   IMPORTERS_DEPS="../importers/libemdros_importers.la"
   IMPORTERS_LDADD_BACKBACK="../../importers/libemdros_importers.la"
   IMPORTERS_DEPS_BACKBACK="../../importers/libemdros_importers.la"
   IMPORTERS_LDADD_BACKBACKBACK="../../../importers/libemdros_importers.la"
   IMPORTERS_DEPS_BACKBACKBACK="../../../importers/libemdros_importers.la"

   EMDROS_LDADD="${IMPORTERS_LDADD} ${UTIL_LDADD} ${HARVEST_LDADD} ${MQL_LDADD} ${EMDF_LDADD} ${SQLITE3_LDADD} ${PCRE2_LDADD} -lpthread -ldl"
   EMDROS_LDADD_BACKBACK="${IMPORTERS_LDADD_BACKBACK} ${UTIL_LDADD_BACKBACK} ${HARVEST_LDADD_BACKBACK} ${MQL_LDADD_BACKBACK} ${EMDF_LDADD_BACKBACK} ${SQLITE3_LDADD_BACKBACK} ${PCRE2_LDADD_BACKBACK} -lpthread -ldl"
   EMDROS_LDADD_BACKBACKBACK="${IMPORTERS_LDADD_BACKBACKBACK} ${UTIL_LDADD_BACKBACKBACK} ${HARVEST_LDADD_BACKBACKBACK} ${MQL_LDADD_BACKBACKBACK} ${EMDF_LDADD_BACKBACKBACK} ${SQLITE3_LDADD_BACKBACKBACK} ${PCRE2_LDADD_BACKBACKBACK} -lpthread -ldl"

   EMDROS_DEPS="${IMPORTERS_DEPS} ${UTIL_DEPS} ${HARVEST_DEPS} ${MQL_DEPS} ${EMDF_DEPS} ${SQLITE3_DEPS} ${PCRE2_DEPS}"
   EMDROS_DEPS_BACKBACK="${IMPORTERS_DEPS_BACKBACK} ${UTIL_DEPS_BACKBACK} ${HARVEST_DEPS_BACKBACK} ${MQL_DEPS_BACKBACK} ${EMDF_DEPS_BACKBACK} ${SQLITE3_DEPS_BACKBACK} ${PCRE2_DEPS_BACKBACK}"
   EMDROS_DEPS_BACKBACKBACK="${IMPORTERS_DEPS_BACKBACKBACK} ${UTIL_DEPS_BACKBACKBACK} ${HARVEST_DEPS_BACKBACKBACK} ${MQL_DEPS_BACKBACKBAC} ${EMDF_DEPS_BACKBACKBACK} ${SQLITE3_DEPS_BACKBACKBACK} ${PCRE2_DEPS_BACKBACKBACK}"

   EMDROS_LDADD="${EMDROS_LDADD} $PG_LDADD $MY_LDADD -lpthread -ldl"
   EMDROS_LDADD_BACKBACK="${EMDROS_LDADD_BACKBACK} $PG_LDADD $MY_LDADD -lpthread -ldl"
   EMDROS_LDADD_BACKBACKBACK="${EMDROS_LDADD_BACKBACKBACK} $PG_LDADD $MY_LDADD -lpthread -ldl"

else
   dnl DEVEL=no
   EMDROS_LDADD="../amalgamation/libemdros_amalgamation.la ${BACKEND_LDADD}"
   EMDROS_LDADD_BACKBACK="../../amalgamation/libemdros_amalgamation.la ${BACKEND_LDADD_BACKBACK}"
   EMDROS_LDADD_BACKBACKBACK="../../../amalgamation/libemdros_amalgamation.la ${BACKEND_LDADD_BACKBACKBACK}"
   EMDROS_DEPS="../amalgamation/libemdros_amalgamation.la"
   EMDROS_DEPS_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
   EMDROS_DEPS_BACKBACKBACK="../../../amalgamation/libemdros_amalgamation.la"
   EMDROS_LDADD="${EMDROS_LDADD} $PG_LDADD $MY_LDADD -lpthread -ldl"
   EMDROS_LDADD_BACKBACK="${EMDROS_LDADD_BACKBACK} $PG_LDADD $MY_LDADD -lpthread -ldl"
   EMDROS_LDADD_BACKBACKBACK="${EMDROS_LDADD_BACKBACKBACK} $PG_LDADD $MY_LDADD -lpthread -ldl"
fi

AC_SUBST(BASE_LDADD)
AC_SUBST(BASE_LDADD_BACKBACK)
AC_SUBST(BASE_LDADD_BACKBACKBACK)
AC_SUBST(BASE_DEPS)
AC_SUBST(BASE_DEPS_BACKBACK)
AC_SUBST(BASE_DEPS_BACKBACKBACK)


AC_SUBST(EMDF_LDADD)
AC_SUBST(EMDF_LDADD_BACKBACK)
AC_SUBST(EMDF_LDADD_BACKBACKBACK)
AC_SUBST(EMDF_DEPS)
AC_SUBST(EMDF_DEPS_BACKBACK)
AC_SUBST(EMDF_DEPS_BACKBACKBACK)

AC_SUBST(MQL_LDADD)
AC_SUBST(MQL_LDADD_BACKBACK)
AC_SUBST(MQL_LDADD_BACKBACKBACK)
AC_SUBST(MQL_DEPS)
AC_SUBST(MQL_DEPS_BACKBACK)
AC_SUBST(MQL_DEPS_BACKBACKBACK)

AC_SUBST(HARVEST_LDADD)
AC_SUBST(HARVEST_LDADD_BACKBACK)
AC_SUBST(HARVEST_LDADD_BACKBACKBACK)
AC_SUBST(HARVEST_DEPS)
AC_SUBST(HARVEST_DEPS_BACKBACK)
AC_SUBST(HARVEST_DEPS_BACKBACKBACK)

AC_SUBST(UTIL_LDADD)
AC_SUBST(UTIL_LDADD_BACKBACK)
AC_SUBST(UTIL_LDADD_BACKBACKBACK)
AC_SUBST(UTIL_DEPS)
AC_SUBST(UTIL_DEPS_BACKBACK)
AC_SUBST(UTIL_DEPS_BACKBACKBACK)

AC_SUBST(IMPORTERS_LDADD)
AC_SUBST(IMPORTERS_LDADD_BACKBACK)
AC_SUBST(IMPORTERS_LDADD_BACKBACKBACK)
AC_SUBST(IMPORTERS_DEPS)
AC_SUBST(IMPORTERS_DEPS_BACKBACK)
AC_SUBST(IMPORTERS_DEPS_BACKBACKBACK)

AC_SUBST(EMDROS_DEPS)
AC_SUBST(EMDROS_DEPS_BACKBACK)
AC_SUBST(EMDROS_DEPS_BACKBACKBACK)
AC_SUBST(EMDROS_LDADD)
AC_SUBST(EMDROS_LDADD_BACKBACK)
AC_SUBST(EMDROS_LDADD_BACKBACKBACK)




dnl Set ADDITIONAL_CXXFLAGS and ADDITIONAL_CFLAGS
if test "x$ISHOSTSUNCC" = "xyes"; then
  dnl We are on Solaris + Sun's CC compiler.
  dnl Don't touch the variables.
  : 
else
  if test -z "$ADDITIONAL_CXXFLAGS"; then
     dnl It was not set by the user.
     if test "x$COMPILER_IS_GPLUSPLUS" = "xyes"; then
        dnl We are on GNU g++     
        ADDITIONAL_CXXFLAGS="-Wall -Wextra"
     fi
  fi
  if test -z "$ADDITIONAL_CFLAGS"; then
     dnl It was not set by the user
     if test "x$COMPILER_IS_GPLUSPLUS" = "xyes"; then
        dnl We are on GNU g++     
        ADDITIONAL_CFLAGS="-Wall -Wextra"
     fi
  fi
fi

if test "xDO_MONGODB" != "xno"; then
  ADDITIONAL_CXXFLAGS="$ADDITIONAL_CXXFLAGS $LIBMONGOC_CFLAGS $LIBBSON_CFLAGS"
fi
AC_SUBST(ADDITIONAL_CXXFLAGS)
AC_SUBST(ADDITIONAL_CFLAGS)


dnl Set DEBUGFLAG
if test "x$DEBUG" = "xyes"; then
  if test "x$COMPILER_IS_GPLUSPLUS" = "xyes"; then
     dnl Remove -g from CXXFLAGS and CFLAGS
     dnl CXXFLAGS=`echo $CXXFLAGS | sed 's/-g //g' | sed 's/-g$//g'`
     dnl CFLAGS=`echo $CFLAGS | sed 's/-g //g' | sed 's/-g$//g'`
     DEBUGFLAG="-g -Wall";
  else
     if test "x$COMPILERISCC" = "xyes"; then
        DEBUGFLAG="-g";
     else
        DEBUGFLAG="-g";
     fi
  fi
else
  DEBUGFLAG="-DNDEBUG=1";
fi

AC_SUBST(DEBUGFLAG)


#
# Compute the "characteristic backend string"
#
# This is the concatentation of the presence or absence of the strings
# "_sq", "_pg", and "_my" (in that order), where the presence of each
# of these strings means that we are doing that backend.
#

if test "x$DO_SQLITE3" != "xno"; then
  CBS_SQLITE3="_sq3"
else
  CBS_SQLITE3=""
fi

if test "x$DO_POSTGRESQL" != "xno"; then
  CBS_POSTGRESQL="_pg"
else
  CBS_POSTGRESQL=""
fi

if test "x$DO_MYSQL" != "xno"; then
  CBS_MYSQL="_my"
else
  CBS_MYSQL=""
fi

CHARACTERISTIC_BACKEND_STRING=${CBS_SQLITE3}${CBS_POSTGRESQL}${CBS_MYSQL}
AC_SUBST(CHARACTERISTIC_BACKEND_STRING)





])
