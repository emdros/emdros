AC_DEFUN([EMDROS_MYSQL], [

AC_ARG_WITH(mysql,
[  --with-mysql            Use MySQL as a backend (default: maybe)],
[case "${withval}" in
       yes) DO_MYSQL=yes ;;
       no)  DO_MYSQL=no ;;
       maybe) DO_MYSQL=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-mysql) ;;
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
  dnl Check for libmysqlclient.a/libmysqlclient.so
  dnl Set LDFLAGS or report error.
  AC_MSG_CHECKING([for mysql libmysqlclient.a/libmysqlclient.so/libmysqlclient.dylib])
  libmysqlclientdir_found="false";
  libmysqlclientdir_try_dirs="$MYSQL_LIB_DIR /usr/lib /usr/local/lib /usr/lib64 /usr/lib/x86_64-linux-gnu /usr/pkg/lib /usr/lib/mysql /usr/local/lib/mysql /usr/lib64/mysql /usr/pkg/lib/mysql /usr/local/mysql/lib/mysql /usr/local/mysql/lib /usr/sfw/lib /sw/lib /sw/lib/mysql"
  for d in $libmysqlclientdir_try_dirs; do
    if test -e $d/libmysqlclient.a -o -e $d/libmysqlclient.so -o -e $d/libmysqlclient.dylib; then
      libmysqlclientdir_dir=$d;
      libmysqlclientdir_found="true";
      break;
    fi;
  done
  if test x$libmysqlclientdir_found = xtrue; then
    AC_MSG_RESULT($libmysqlclientdir_dir);
  else
    if test "x$DO_MYSQL" = "xmaybe"; then
      AC_MSG_RESULT([Not found... not doing MySQL])
      DO_MYSQL="no"
    else
      AC_MSG_ERROR([
Error: Could not find 
the libmysqlclient.a or libmysqlclient.so library.
Please set the MYSQL_LIB_DIR environment variable and run ./configure
again.  For example:

export MYSQL_LIB_DIR=/opt/LocalMySQLInstallation/lib

])
    fi
  fi
  MYSQL_LDPATH=$libmysqlclientdir_dir
fi

MY_LDADD=""
MY_LIBDIRS=""
MY_LIBS=""
my_libfound="no"
if test "x$DO_MYSQL" != "xno"; then
   dnl
   dnl Linking against .a libraries... just don't do it...!
   dnl
   MY_LDADD="-L$libmysqlclientdir_dir -lmysqlclient"
   MY_LIBDIRS="-L$libmysqlclientdir_dir"
   MY_LIBS="-lmysqlclient"

   dnl if test -f $libmysqlclientdir_dir/libmysqlclient.a; then
   dnl   if test "x$HOSTISDARWIN" = "xyes"; then
   dnl      dnl On Darwin, we know how to do it...
   dnl      MY_LDADD="$libmysqlclientdir_dir/libmysqlclient.a -lz";
   dnl      my_libfound="yes"
   dnl   else
   dnl      dnl On all others, we don't
   dnl      MY_LDADD="-L$libmysqlclientdir_dir -lmysqlclient";
   dnl   fi
   dnl else
   dnl   dnl If we could not find libmysqlclient.a, 
   dnl   dnl then we use good old -lmysqlclient.
   dnl   MY_LDADD="-L$libmysqlclientdir_dir -lmysqlclient"
   dnl fi
fi

dnl If we are still to do MySQL, AND we haven't found it already,
dnl proceed with checking -lmysqlclient.
if test "x$DO_MYSQL" != "xno" -a "x$my_libfound" != "xyes"; then
  save_LDFLAGS="$LDFLAGS"
  LDFLAGS="$LDFLAGS -L$MYSQL_LDPATH"
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
  LDFLAGS="$save_LDFLAGS"
  export LDFLAGS
fi

dnl Set a sensible default, so as not to be empty
MY_INCLUDE_DIR="/usr/include"
dnl
dnl Check for header
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
  if test x$mysqldir_found = xtrue; then
    AC_MSG_RESULT($mysqldir_dir);
  else
    if test "x$DO_MYSQL" = "xmaybe"; then
      AC_MSG_RESULT("not found... not doing MySQL")
      DO_MYSQL="no"
    else
      AC_MSG_ERROR([
Error: Could not find 
the mysql.h include-file.
Please set the MYSQL_INCLUDE_DIR environment variable and run ./configure
again.

For example:
   export MYSQL_INCLUDE_DIR=/opt/LocalMySQLInstallation/include/mysql
      ])
    fi
  fi
  export CPPFLAGS="$CPPFLAGS -I$mysqldir_dir"
  MY_INCLUDE_DIR="$mysqldir_dir"
  AC_CHECK_HEADER(mysql.h, , 
    AC_MSG_ERROR([
    Error: Could not find mysql.h from MySQL.
    Please set the CPPFLAGS environment variable so that it
    includeS the -I path to MySQL. E.g. with bash: 
      CPPFLAGS=-I/usr/include/mysql
      export CPPFLAGS
  ]))
fi
AC_SUBST(MY_INCLUDE_DIR)

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
  BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION $MY_LIBS"
  BACKEND_LDADD_AMALGAMATION="$BACKEND_LDADD_AMALGAMATION $MY_LDADD"
  MYSQL_DASH_INCLUDE="-I$MY_INCLUDE_DIR"
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

AC_SUBST(MYSQL_DASH_INCLUDE)



])
