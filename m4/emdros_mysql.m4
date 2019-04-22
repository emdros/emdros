AC_DEFUN([EMDROS_MYSQL], [

AC_ARG_WITH(mysql,
[  --with-mysql            Use MySQL as a backend (default: maybe)],
[case "${withval}" in
       yes) DO_MYSQL=yes ;;
       no)  DO_MYSQL=no ;;
       maybe) DO_MYSQL=maybe ;;
       *)   AC_MSG_ERROR([Bad value ${withval} for --with-mysql]) ;;
     esac],
[DO_MYSQL=maybe])
ORIGINAL_DO_MYSQL=$DO_MYSQL

dnl
dnl Check for MySQL
dnl
dnl 
dnl Check for libmysqlclient.a
AC_LANG([C])
addlibz=no

if test "x$DO_MYSQL" != "xno"; then
  dnl mysql_config program
  AC_CHECK_PROG(MYSQL_CONFIG_PROG, mysql_config, yes, no)
  AC_MSG_CHECKING([whether we got mysql_config])
  if test x$MYSQL_CONFIG_PROG = xno; then
    dnl We don't have mysql_config
    AC_MSG_RESULT([no, but we'll keep looking for MySQL])
  else
    dnl We have mysql_config.
    AC_MSG_RESULT([yes.])
  fi
fi

dnl Attempt to find MySQL ourselves, even if we got
dnl the mysql_config program.
my_libfound="no"
MYSQL_LDPATH=""
if test "x$DO_MYSQL" != "xno"; then
  dnl Check for libmysqlclient.a/libmysqlclient.so/libmysqlclient.dylib
  dnl Set LDFLAGS or report error.
  libmysqlclientdir_found="false";
  libmysqlclientdir_try_dirs="$MYSQL_LIB_DIR /usr/lib /usr/local/lib /usr/lib64 /usr/lib/x86_64-linux-gnu /usr/pkg/lib /usr/lib/mysql /usr/local/lib/mysql /usr/lib64/mysql /usr/pkg/lib/mysql /usr/local/mysql/lib/mysql /usr/local/mysql/lib /usr/sfw/lib /sw/lib /sw/lib/mysql"
  for d in $libmysqlclientdir_try_dirs; do
    if test -e $d/libmysqlclient.a -o -e $d/libmysqlclient.so -o -e $d/libmysqlclient.dylib; then
      libmysqlclientdir_dir=$d;
      libmysqlclientdir_found="true";
      break;
    fi;
  done

  AC_MSG_CHECKING([for mysql libmysqlclient.a/libmysqlclient.so/libmysqlclient.dylib])
  if test x$libmysqlclientdir_found = xtrue; then
    AC_MSG_RESULT([$libmysqlclientdir_dir]);
  else
    AC_MSG_RESULT([Not found... trying harder...])
  fi
fi

MY_LDADD=""
if test "x$DO_MYSQL" != "xno"; then
   AC_MSG_CHECKING([for MySQL libraries])
   if test x$MYSQL_CONFIG_PROG != xno; then
      MY_LDADD=`mysql_config --libs`
      AC_MSG_RESULT([$MY_LDADD])
      my_libfound="yes";
   elif test "x$libmysqlclientdir_found" = xtrue; then
      MY_LDADD="-L$libmysqlclientdir_dir -lmysqlclient"
      AC_MSG_RESULT([$MY_LDADD])
      my_libfound="yes";
   else
      AC_MSG_ERROR([Not found.

Please set

   export MYSQL_LIB_DIR="/path/to/libmysqlclient.{so,dylib}"

then run configure again.
])
   fi
fi

dnl If we are still to do MySQL, AND we haven't found it already,
dnl proceed with checking -lmysqlclient.
if test "x$DO_MYSQL" != "xno" -a "x$my_libfound" != "xyes"; then
  LDFLAGS="$LDFLAGS $MY_LDADD"
  export LDFLAGS

  AC_CHECK_LIB(mysqlclient, mysql_info, mysqllib=yes ; LIBS="$LIBS", mysqllib=tryagain)
  if test "x$mysqllib" = "xtryagain"; then
    AC_CHECK_LIB(mysqlclient, mysql_init, mysqllib=yes ; addlibz=yes; LIBS="$LIBS", mysqllib=no ; addlibz=no, -lz)
  fi

  if test "x$mysqllib" = "xno"; then
    if test "x$DO_MYSQL" = "xmaybe"; then
      DO_MYSQL="no";
    else
      AC_MSG_ERROR([
MySQL library libmysqlclient.a must be installed.  If it is
please set the LDFLAGS environment variable.  E.g.:
  $ LDFLAGS=-L/usr/lib/mysql
  $ export LDFLAGS
      ])
    fi
  fi
fi

dnl
dnl Check for <mysql.h> header
dnl
dnl Set CPPFLAGS or report error.
if test "x$DO_MYSQL" != "xno"; then
  AC_MSG_CHECKING([for mysql include directory])
  mysqldir_found="false";
  mysqldir_try_dirs="$MYSQL_INCLUDE_DIR /usr/include /usr/local/include /usr/pkg/include /usr/include/mysql /usr/local/mysql/include /usr/sfw/include/mysql /sw/include /sw/include/mysql"
  for d in $mysqldir_try_dirs; do
    if test -f $d/mysql.h; then
      mysqldir_dir=$d;
      mysqldir_found="true";
      break;
    else
      if test -f $d/mysql/mysql.h; then
        mysqldir_dir=$d/mysql;
        mysqldir_found="true";
        break;
      fi
    fi;
  done

  if test "x$mysqldir_found" = "xtrue"; then
    AC_MSG_RESULT([$mysqldir_dir])
  else
    AC_MSG_RESULT([Not found... Let's try again.])
  fi


  AC_MSG_CHECKING([for <mysql.h>])
  if test x$MYSQL_CONFIG_PROG != xno; then
      MY_CFLAGS=`mysql_config --include`
      AC_MSG_RESULT([$MY_CFLAGS])
  elif test "x$libmysqldir_found" = xtrue; then
      MY_CFLAGS="-I$mysqldir_dir"
      AC_MSG_RESULT([$MY_CFLAGS])
  else
      AC_MSG_ERROR([
Error: Could not find mysql.h from MySQL.

Please set

   export MYSQL_INCLUDE_DIR="/path/to/mysql.h"

then run configure again.
])
  fi

  MYSQL_CFLAGS="$MY_CFLAGS"

  CXXFLAGS="$CXXFLAGS $MY_CFLAGS"
  CPPFLAGS="$CPPFLAGS $MY_CFLAGS"
  export CXXFLAGS
  export CPPFLAGS

  AC_CHECK_HEADER([mysql.h],
                  [have_mysql_h=true],
                  [have_mysql_h=false])

  if test "xhave_mysql_h" = "x1"; then
     AC_DEFINE([HAVE_MYSQL_H], [1],
               [Define to 1 if you have <libpq-fe.h> from PostgreSQL])
  else
     AC_DEFINE([HAVE_MYSQL_H], [0],
               [Define to 1 if you have <libpq-fe.h> from PostgreSQL])
  fi

  AC_MSG_CHECKING([whether to use <mysql.h>])
  if test "x$have_mysql_h" = "xtrue"; then
    AC_MSG_RESULT([yes]);
  else
    if test "x$DO_MYSQL" = "xmaybe"; then
      AC_MSG_RESULT([Not found... Not doing MySQL])
      DO_MYSQL="no"
    else
      AC_MSG_ERROR([
  Error: Could not find the <mysql.h> include-file.
  Please set the MYSQL_INCLUDE_DIR environment variable and run ./configure
  again.
      ])
    fi
  fi



fi
AC_SUBST(MYSQL_CFLAGS)

dnl End checking for MySQL

USE_MYSQL=0

if test "x$DO_MYSQL" != "xno"; then
  USE_MYSQL=1
else
  USE_MYSQL=0
fi

AM_CONDITIONAL(MYSQL, test x$DO_MYSQL != xno) 

if test "x$DO_MYSQL" != "xno"; then
  BACKEND_LDADD="$BACKEND_LDADD $MY_LDADD";
  BACKEND_LDADD_BACKBACK="$BACKEND_LDADD_BACKBACK $MY_LDADD";
  BACKEND_LDADD_BACKBACKBACK="$BACKEND_LDADD_BACKBACKBACK $MY_LDADD";
  BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION $MY_LDADD"
  BACKEND_LDADD_AMALGAMATION="$BACKEND_LDADD_AMALGAMATION $MY_LDADD"
  PKGCONFIG_LIBS_AMALGAMATION="$PKGCONFIG_LIBS_AMALGAMATION $MY_LDADD"
fi

WITH_MYSQL=--with-mysql=$ORIGINAL_DO_MYSQL

if test "x$DO_MYSQL" != "xno"; then
  CBS_MYSQL="_my"
else
  CBS_MYSQL=""
fi

AC_SUBST(USE_MYSQL)
AC_SUBST(WITH_MYSQL)

])
