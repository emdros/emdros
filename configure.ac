dnl Process this file with autoconf to produce a configure script.
AC_INIT([emdros],[3.6.0])
AC_CONFIG_SRCDIR(src/mql.cpp)
AM_INIT_AUTOMAKE
AC_PREREQ(2.63)
AC_CONFIG_HEADERS([include/emdros-config.h])
AC_CANONICAL_HOST
AC_PREFIX_DEFAULT(/usr)
AC_CONFIG_MACRO_DIR([m4])

EMDROS_BUILDENV

dnl Invoke libtool
LT_INIT([])
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL



dnl Library versioning
LIB_EMDF_CURRENT=5
LIB_EMDF_REVISION=0
LIB_EMDF_AGE=0
LIB_MQL_CURRENT=5
LIB_MQL_REVISION=0
LIB_MQL_AGE=0
LIB_PCRE_CURRENT=1
LIB_PCRE_REVISION=0
LIB_PCRE_AGE=0
LIB_SWIG_CURRENT=5
LIB_SWIG_REVISION=0
LIB_SWIG_AGE=0
AC_SUBST(LIB_EMDF_CURRENT)
AC_SUBST(LIB_EMDF_REVISION)
AC_SUBST(LIB_EMDF_AGE)
AC_SUBST(LIB_MQL_CURRENT)
AC_SUBST(LIB_MQL_REVISION)
AC_SUBST(LIB_MQL_AGE)
AC_SUBST(LIB_PCRE_CURRENT)
AC_SUBST(LIB_PCRE_REVISION)
AC_SUBST(LIB_PCRE_AGE)
AC_SUBST(LIB_SWIG_CURRENT)
AC_SUBST(LIB_SWIG_REVISION)
AC_SUBST(LIB_SWIG_AGE)

dnl Set CFLAGS to empty unless the user has set it
dnl if test "x$CFLAGS" = "x"; then
  dnl we need to set it to something non-empty because AC_PROG_CC 
  dnl sets it to '-g -O2' by default.
  dnl CFLAGS=" "
  dnl export CFLAGS
dnl fi

dnl Set CXXFLAGS to empty unless the user has set it
dnl if test "x$CXXFLAGS" = "x"; then
  dnl we need to set it to something non-empty because AC_PROG_CXX 
  dnl sets it to '-g -O2' by default.
  dnl CXXFLAGS=" "
  dnl export CXXFLAGS
dnl fi





AM_CONDITIONAL(DO_SHARED_LIBS, test x$enable_shared = xyes) 


dnl
dnl Use amalgamation instead of libpcre_emdros, libsqlite3_emdros,
dnl libemdf, libmql, libharvest ?
dnl


dnl --enable-amalgamation
AC_ARG_ENABLE(amalgamation,
[  --enable-amalgamation         turns amalgamation library on 
  --disable-amalgamation        turns amalgamation library off (default)],
[case "$enableval" in
       no) USE_AMALGAMATION=no ;;
       yes) USE_AMALGAMATION=yes ;;
       *) USE_AMALGAMATION=no ;;
 esac],
  USE_AMALGAMATION=no)

AM_CONDITIONAL(USE_AMALGAMATION, test "x$USE_AMALGAMATION" = "xyes")

# Use automatically created ac variable
ENABLE_AMALGAMATION=--enable-amalgamation=$enable_amalgamation
AC_SUBST(ENABLE_AMALGAMATION)





AC_ARG_WITH(postgresql,
[  --with-postgresql       Use PostgreSQL as a backend (default: maybe)],
[case "${withval}" in
       yes) DO_POSTGRESQL=yes ;;
       no)  DO_POSTGRESQL=no ;;
       maybe) DO_POSTGRESQL=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-postgresql) ;;
     esac],
[DO_POSTGRESQL=maybe])
ORIGINAL_DO_POSTGRESQL=$DO_POSTGRESQL

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



dnl No fsync for SQLite 3
AC_ARG_WITH(sqlite3-no-fsync,
[  --with-sqlite3-no-fsync     Don't fsync on SQLite 3. This speeds things up, 
                             but is unsafe w.r.t. power outages.],
[case "${withval}" in
       yes) sqlite3_no_fsync=yes ;;
       no)  sqlite3_no_fsync=no ;;
       *) sqlite3_no_fsync=yes ;;       
     esac],
[sqlite3_no_fsync=yes])
AM_CONDITIONAL(SQLITE3_EMDFDB_NO_FSYNC, test x$sqlite3_no_fsync = xyes) 



AC_ARG_WITH(sqlite3,
[  --with-sqlite3           Use Sqlite3 as a backend (default: maybe)
                          Valid values: yes, no, local, system, maybe],
[case "${withval}" in
       yes) DO_SQLITE3=yes ;;
       no)  DO_SQLITE3=no ;;
       maybe) DO_SQLITE3=maybe ;;
       local)  DO_SQLITE3=local ;;
       system)  DO_SQLITE3=system ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-sqlite3) ;;
     esac],
[DO_SQLITE3=maybe])
ORIGINAL_DO_SQLITE3=$DO_SQLITE3


SQLITE3_VERSION=`cat sqlite3/VERSION`
AC_SUBST(SQLITE3_VERSION)



dnl
dnl The BPT backend is proprietary, and not available in the Open
dnl Source version of Emdros.
dnl
dnl The BPT backend is a lot faster than SQLite 3 and has much smaller
dnl databases than SQLite 3.
dnl
dnl For more information, please contact the author of Emdros,
dnl Ulrik Sandborg-Petersen, at ulrikp -at- emergence -dot- dk
dnl
AC_ARG_WITH(bpt,
[  --with-bpt               Use BPT as a backend (default: no)],
[case "${withval}" in
       yes) DO_BPT=yes ;;
       maybe) DO_BPT=maybe ;;
       no)  DO_BPT=no ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-bpt) ;;
     esac],
[DO_BPT=maybe])
ORIGINAL_DO_BPT=$DO_BPT

dnl
dnl If DO_BPT is maybe, test whether we should, in fact, build it.
dnl
if test x$DO_BPT = xmaybe; then
  BPTNOTREAL=`grep "This is not the real thing" EMdF/bptemdfdb.cpp`;
  if test "x$BPTNOTREAL" == "x"; then
    DO_BPT=yes;
  else
    DO_BPT=no;
  fi
fi



dnl
dnl The BPT2 backend (BPT version 2) is proprietary, and not available 
dnl in the Open Source version of Emdros.
dnl
dnl The BPT2 backend is a lot faster than BPT version 1, and has much
dnl smaller databases than BPT version 1.
dnl
dnl For more information, please contact the author of Emdros,
dnl Ulrik Sandborg-Petersen, at ulrikp -at- emergence -dot- dk
dnl
AC_ARG_WITH(bpt2,
[  --with-bpt2              Use BPT2 as a backend (default: no)],
[case "${withval}" in
       yes) DO_BPT2=yes ;;
       maybe) DO_BPT2=maybe ;;
       no)  DO_BPT2=no ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-bpt2) ;;
     esac],
[DO_BPT2=maybe])
ORIGINAL_DO_BPT2=$DO_BPT2

dnl
dnl If DO_BPT2 is maybe, test whether we should, in fact, build it.
dnl
if test x$DO_BPT2 = xmaybe; then
  BPT2NOTREAL=`grep "This is not the real thing" EMdF/bpt2emdfdb.cpp`;
  if test "x$BPT2NOTREAL" == "x"; then
    DO_BPT2=yes;
  else
    DO_BPT2=no;
  fi
fi



AC_ARG_WITH(32bit,
[  --with-32bit            Use 32bit architecture on AMD64],
[case "${withval}" in
       yes) USE_32BIT=yes ;;
       no)  USE_32BIT=no ;;
       *)   USE_32BIT=no ;;
     esac])




dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CXXCPP
AC_PROG_INSTALL


dnl Support for wxWidgets, in m4/wxemdros.m4
EMDROS_WX_SUPPORT

dnl More build env stuff
EMDROS_BUILDENV2

dnl SWIG support (from m4/emdrosswig.m4)
EMDROS_SWIG_SUPPORT





dnl
dnl Check for flex version 2.5.4
dnl
dnl AC_CHECK_PROG(FLEX, flex, flex, no)
dnl AC_MSG_CHECKING([that version of flex is >= 2.5.4])
dnl if test "x$FLEX" = "xno"; then 
dnl   AC_MSG_ERROR([flex 2.5.4a or higher must be present]); 
dnl fi
dnl FLEXVERSIONOK=`flex --version | awk 'NR==1 { if ($3 >= "2.5.4") print "yes"; else print "no";}'` ;\
dnl if test "x$FLEXVERSIONOK" != "xyes"; then 
dnl   AC_MSG_ERROR(flex version 2.5.4a or higher must be present); 
dnl else 
dnl   AC_MSG_RESULT(yes); 
dnl fi

dnl
dnl Check for bison version 1.28
dnl
dnl AC_CHECK_PROG(BISON, bison, bison, no)
dnl AC_MSG_CHECKING([that version of bison is >= 1.28])
dnl if test "x$BISON" = "xno"; then 
dnl   AC_MSG_ERROR([bison 1.28 or higher must be present]); 
dnl fi
dnl BISONVERSIONOK=`bison --version | awk 'NR==1 { if ($4 >= "1.28") print "yes"; else print "no";}'` ;\
dnl if test "x$BISONVERSIONOK" != "xyes"; then 
dnl   AC_MSG_ERROR(bison version 1.28 or higher must be present); 
dnl else 
dnl   AC_MSG_RESULT(yes); 
dnl fi


dnl
dnl Checks PostgreSQL
dnl

dnl
dnl Checks for libraries.
dnl

if test "x$DO_POSTGRESQL" != "xno"; then 
  dnl 
  dnl Check for libpq.a/libpq.so/libpq.dylib
  dnl Set LDFLAGS or report error.
  AC_MSG_CHECKING([for postgresql libpq.a/libpq.so/libpq.dylib])
  libpqdir_found="false";
  libpqdir_try_dirs="$POSTGRESQL_LIB_DIR /usr/lib /usr/local/lib /usr/lib64 /usr/lib/x86_64-linux-gnu /usr/pkg/lib /usr/local/pgsql/lib /opt/PostgreSQL/lib /sw/lib /usr/postgres/8.3/lib"
  for d in $libpqdir_try_dirs; do
    if test -e $d/libpq.a -o -e $d/libpq.so -o -e $d/libpq.dylib; then
      libpqdir_dir=$d;
      libpqdir_found="true";
      break;
    fi;
  done
  if test x$libpqdir_found = xtrue; then
    AC_MSG_RESULT($libpqdir_dir);
  else
    if test "x$DO_POSTGRESQL" = "xmaybe"; then
      AC_MSG_RESULT([not found... Not doing PostgreSQL.]);
      DO_POSTGRESQL="no"
    else
      AC_MSG_ERROR([
Error: Could not find 
the libpq.a or libpq.so or libpq.dylib library.
Please set the POSTGRESQL_LIB_DIR environment variable and run ./configure
again.  For example:

export POSTGRESQL_LIB_DIR=/opt/MyPostgreSQLInstallation/lib

])
    fi
  fi
fi

PG_LDADD=""
PG_LIBDIRS=""
PG_LIBS=""
pg_libfound="no"
if test "x$DO_POSTGRESQL" != "xno"; then
   dnl
   dnl It was a bad idea to include libpq.a, for several reasons, 
   dnl one of which was, that when compiling against the standard 
   dnl MySQL libraries (MySQL 5.0.28) on Mac OS X, linker errors ensued, 
   dnl because MySQL at some point in the 5.0.X series decided to include 
   dnl some of the -lcrypto objects in the libmysqlclient.a, but not all.  
   dnl Since libpq.a depends on -lcrypto, this was a problem.
   PG_LDADD="-L$libpqdir_dir -lpq";
   PG_LIBDIRS="-L$libpqdir_dir"
   PG_LIBS="-lpq"
   
   dnl if test -f $libpqdir_dir/libpq.a; then
   dnl    if test "x$HOSTISDARWIN" = "xyes"; then
   dnl       dnl On Darwin, we know how to do it...
   dnl       PG_LDADD="$libpqdir_dir/libpq.a -lssl -lcrypto /System/Library/Frameworks/Kerberos.framework/Versions/A/Kerberos";
   dnl        pg_libfound="yes"
   dnl    else
   dnl       dnl On all others, we don't
   dnl       PG_LDADD="-L$libpqdir_dir -lpq";
   dnl   fi
   dnl else
   dnl    dnl If we could not find libpq.a, then we use good old -lpq.
   dnl    PG_LDADD="-L$libpqdir_dir -lpq"
   dnl fi
fi

dnl If we are still to do PostgreSQL, AND we haven't found it already,
dnl proceed with checking -lpq.
if test "x$DO_POSTGRESQL" != "xno" -a "x$pg_libfound" != "xyes"; then

  POSTGRESQL_LDPATH=$libpqdir_dir
  save_LDFLAGS="$LDFLAGS"
  LDFLAGS="$LDFLAGS -L$POSTGRESQL_LDPATH"
  export LDFLAGS

  AC_LANG([C])
  AC_CHECK_LIB(pq, PQconnectdb, LIBS="-lpq $LIBS" ; pqlib=yes, pqlib=tryagain)
  if test "x$pqlib" = "xtryagain"; then 
    AC_CHECK_LIB(pq, 
                 PQsetdbLogin, 
                 LIBS="-lpq -lsocket -lnsl $LIBS", 
                 [
                 if test "x$DO_POSTGRESQL" = "xmaybe"; then
                   DO_POSTGRESQL="no"
                 else
                   AC_MSG_ERROR([
    PostgreSQL library libpq.a must be installed.  If it is
    please set the LDFLAGS environment variable.  E.g.:
      $ LDFLAGS=-L/usr/local/pgsql/lib
      $ export LDFLAGS
  ])
                 fi
                 ]
  , [-lsocket -lnsl $LIBS]) 
  fi
  LDFLAGS="$save_LDFLAGS"
  export LDFLAGS
fi

dnl Set a sensible default, so as not to be empty
PG_INCLUDE_DIR="/usr/include"
if test "x$DO_POSTGRESQL" != "xno"; then
  dnl Check for PostgreSQL header files
  AC_LANG([C++])


  dnl Postgresql include-dir

  dnl Set CPPFLAGS or report error.
  AC_MSG_CHECKING([for postgresql include directory])
  postgresqldir_found="false";
  postgresqldir_try_dirs="$POSTGRESQL_INCLUDE_DIR /usr/include /usr/local/include /usr/pkg/include /usr/local/pgsql/include /usr/include/pgsql /usr/include/postgresql /opt/PostgreSQL/include /sw/include/postgresql /usr/postgres/8.3/include"
  for d in $postgresqldir_try_dirs; do
    if test -f $d/libpq-fe.h; then
      postgresqldir_dir=$d;
      postgresqldir_found="true";
      break;
    fi;
  done
  if test x$postgresqldir_found = xtrue; then
    AC_MSG_RESULT($postgresqldir_dir);
  else
    if test "x$DO_POSTGRESQL" = "xmaybe"; then
      AC_MSG_RESULT([Not found... Not doing PostgreSQL])
      DO_POSTGRESQL="no"
    else
      AC_MSG_ERROR([
  Error: Could not find 
  the libpq-fe.h include-file.
  Please set the POSTGRESQL_INCLUDE_DIR environment variable and run ./configure
  again.
      ])
    fi
  fi
  export CPPFLAGS="$CPPFLAGS -I$postgresqldir_dir"
  PG_INCLUDE_DIR="$postgresqldir_dir"

  AC_LANG([C])

  dnl
  dnl Take "-lpq" out of LIBS.  They are taken care of
  dnl by EMDFLDADD.
  LIBS=`echo $LIBS | sed 's/-lpq//g'`
fi
AC_SUBST(PG_INCLUDE_DIR)


dnl End checking for PostgreSQL

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



dnl
dnl Sqlite3
dnl
USE_LOCAL_SQLITE3=no
SQLITE3_DEP=""
SQLITE3_DEP_SWIG=""
SQLITE3_LDADD=""
SQLITE3_LDADD_SWIG=""
if test "x$DO_SQLITE3" != "xno"; then
  AC_CHECK_HEADER([sqlite3.h], [HAVE_SQLITE3_H=1], [HAVE_SQLITE3_H=0])
  AC_CHECK_LIB(sqlite3, sqlite3_prepare, sqlite32lib=system, sqlite32lib=local)

  USE_LOCAL_SQLITE3=yes
  if test "x$HAVE_SQLITE3_H" = "x0"; then
    USE_LOCAL_SQLITE3=yes
  else
    if test "x$sqlite32lib" = "xsystem" -a "x$DO_SQLITE3" != "xlocal"; then
      USE_LOCAL_SQLITE3=no;
    else
      USE_LOCAL_SQLITE3=yes;
    fi
  fi
fi

if test "x$DO_SQLITE3" != "xno"; then
  if test "x$USE_LOCAL_SQLITE3" = "xyes"; then
    SQLITE3_DEP="../sqlite3/src/libsqlite3_emdros.la"
    SQLITE3_DEP_SWIG="../../sqlite3/src/libsqlite3_emdros.la"
    SQLITE3_LDADD="../sqlite3/src/libsqlite3_emdros.la"
    SQLITE3_LDADD_SWIG="../../sqlite3/src/libsqlite3_emdros.la"
  else
    AC_MSG_CHECKING([for Sqlite3 libsqlite3.a])
    libsqlite3dir_found="false";
    libsqlite3dir_try_dirs="$SQLITE3_LIB_DIR /usr/lib /usr/local/lib /usr/lib64 /usr/pkg/lib /sw/lib"
    for d in $libsqlite3dir_try_dirs; do
      if test -e $d/libsqlite3.a -o -e $d/libsqlite3.dylib -o -e $d/libsqlite3.so; then
        libsqlite3dir_dir=$d;
        libsqlite3dir_found="true";
        break;
      fi;
    done
    if test x$libsqlite3dir_found = xtrue; then
      AC_MSG_RESULT($libsqlite3dir_dir);
    else
      AC_MSG_RESULT([not found. But I know it's there, so I won't panic...]);
    fi

    SQLITE3_DEP=""
    SQLITE3_DEP_SWIG=""
    SQLITE3_LDADD="-lsqlite3"
    SQLITE3_LDADD_SWIG="-lsqlite3"
  fi
fi

AM_CONDITIONAL(USE_LOCAL_SQLITE3, test "x$USE_LOCAL_SQLITE3" = "xyes")

if test "x$USE_LOCAL_SQLITE3" = "xyes"; then
   USE_SYSTEM_SQLITE3=0
else
   USE_SYSTEM_SQLITE3=1
fi
AC_SUBST(USE_SYSTEM_SQLITE3)

AC_SUBST(SQLITE3_DEP)
AC_SUBST(SQLITE3_DEP_SWIG)
AC_SUBST(SQLITE3_LDADD)
AC_SUBST(SQLITE3_LDADD_SWIG)

if test "x$DO_SQLITE3" != "xno"; then
  if test "x$USE_LOCAL_SQLITE3" = "xyes"; then
    DO_SQLITE3="local"
  else
    DO_SQLITE3="system"
  fi
fi


##########
# Are we using UTF-8 or iso8859 encodings for SQLite?
#
AC_ARG_ENABLE(utf8, 
[  --enable-utf8           Use UTF-8 encodings],,enable_utf8=no)
AC_MSG_CHECKING([character encoding])
if test "$enable_utf8" = "no"; then
  ENCODING=ISO8859
  AC_MSG_RESULT([iso8859])
else
  ENCODING=UTF8
  AC_MSG_RESULT([UTF-8])
fi
AC_SUBST(ENCODING)





OS_UNIX=1
OS_WIN=0
tclsubdir=unix
TARGET_CFLAGS="$TARGET_CFLAGS -DOS_UNIX=$OS_UNIX -DOS_WIN=$OS_WIN"

AC_SUBST(OS_UNIX)
AC_SUBST(OS_WIN)


##########
# Figure out what C libraries are required to compile Tcl programs.
#
if test "$config_TARGET_TCL_LIBS" != ""; then
  TARGET_TCL_LIBS="$config_TARGET_TCL_LIBS"
else
  if test "$with_tcl" != ""; then
    extra=`echo $with_tcl/$tclsubdir/libtcl8*.a`
  fi
  dnl AC_CHECK_FUNC(sin, LIBS="", LIBS="-lm")
  AC_CHECK_FUNC(sin, LIBS="",LIBS="-lm")
  AC_CHECK_LIB(dl, dlopen)
  otherlibs="$LIBS -lpthread"
  AC_SEARCH_LIBS(Tcl_Init, dnl
        tcl86 tcl8.6 tcl85 tcl8.5 tcl84 tcl8.4 tcl8.3 tcl83 tcl82 tcl8.2 tcl8.1 tcl8.0 tcl80 tcl,,,$otherlibs)
  TARGET_TCL_LIBS="$LIBS $otherlibs"
fi
echo "TARGET_TCL_LIBS=$TARGET_TCL_LIBS... otherlibs=$otherlibs"
AC_SUBST(TARGET_TCL_LIBS)


##########
# Figure out where to get the TCL header files.
#
AC_MSG_CHECKING([TCL header files])
found=no
if test "$config_TARGET_TCL_INC" != ""; then
  TARGET_TCL_INC=$config_TARGET_TCL_INC
  found=yes
else
  if test "$with_tcl" != ""; then
    TARGET_TCL_INC="-I$with_tcl/generic -I$with_tcl/$tclsubdir"
    found=yes
  else
    TARGET_TCL_INC=""
    found=no
  fi
fi
if test "$found" = "yes"; then
  AC_MSG_RESULT($TARGET_TCL_INC)
else
  AC_MSG_RESULT(not specified: still searching...)
  AC_CHECK_HEADERS(tcl.h, [found=yes],,
  [[#include <stdlib.h>
  ]])
fi
if test "$found" = "no"; then
  for dir in /usr/local /usr/X11 /usr/X11R6 /usr/pkg /usr/contrib /usr; do	
    AC_CHECK_FILE($dir/include/tcl.h, found=yes)
    # Break out of outer loop
    if test "$found" = "yes"; then
       break
    fi
    for dir2 in . tcl86 tcl8.6 tcl85 tcl8.5 tcl84 tcl8.4 tcl83 tcl8.3 tcl; do
      AC_CHECK_FILE($dir/include/$dir2/tcl.h, found=yes)
      if test "$found" = "yes"; then
        TARGET_TCL_INC="-I$dir/include/$dir2"
        # Break out of inner loop
        break
      fi
    done
    # Break out of outer loop
    if test "$found" = "yes"; then
       break
    fi
  done
fi
if test "$found" = "no"; then
  TARGET_TCL_INC="-DNO_TCL=1"
  TCL_H_PRESENT=0
else
  TCL_H_PRESENT=1
fi
AM_CONDITIONAL(USE_TCL, test x$TCL_H_PRESENT = x1)
AC_SUBST(TARGET_TCL_INC)


##########
# Figure out what C libraries are required to compile programs
# that use "readline()" library.
#
if test "$config_TARGET_READLINE_LIBS" != ""; then
  TARGET_READLINE_LIBS="$config_TARGET_READLINE_LIBS"
else
  readline_save_LIBS="$LIBS"
  LIBS=""
  AC_SEARCH_LIBS(tgetent, 
                 [readline curses ncurses termcap],
		 [TARGET_READLINE_LIBS="$LIBS"], dnl Action if found
		 [TARGET_READLINE_LIBS=""]) dnl Action if not found
  LIBS=""
  AC_SEARCH_LIBS(readline, 
                 [readline],
		 [TARGET_READLINE_LIBS="$LIBS $TARGET_READLINE_LIBS"], dnl Action if found
		 [TARGET_READLINE_LIBS=""]) dnl Action if not found
  LIBS="$readline_save_LIBS"
fi
AC_SUBST(TARGET_READLINE_LIBS)


##########
# Figure out where to get the READLINE header files.
#
AC_MSG_CHECKING([readline header files])
found=no
if test "$config_TARGET_READLINE_INC" != ""; then
  TARGET_READLINE_INC=$config_TARGET_READLINE_INC
  found=yes
fi
if test "$found" = "yes"; then
  AC_MSG_RESULT($TARGET_READLINE_INC)
else
  AC_MSG_RESULT(not specified: still searching...)
  AC_CHECK_HEADER(readline.h, [found=yes])
  if test "$found" = "yes"; then
    AC_CHECK_HEADER(history.h, [found=yes], [found=no])
  fi
fi
if test "$found" = "no"; then
  for dir in /usr /usr/local /usr/local/readline /usr/contrib; do
    AC_CHECK_FILE($dir/include/readline.h, found=yes)
    if test "$found" = "yes"; then
      AC_CHECK_FILE($dir/include/history.h, found=yes, found=no)
      if test "$found" = "yes"; then
        TARGET_READLINE_INC="$dir/include"
        break
      fi
    fi
    AC_CHECK_FILE($dir/include/readline/readline.h, found=yes)
    if test "$found" = "yes"; then
      AC_CHECK_FILE($dir/include/readline/history.h, found=yes, found=no)
      if test "$found" = "yes"; then
        TARGET_READLINE_INC="$dir/include/readline"
        break
      fi
    fi
  done
fi
if test "$found" = "yes"; then
  if test "$TARGET_READLINE_LIBS" = ""; then
    TARGET_HAVE_READLINE=0
  else
    TARGET_HAVE_READLINE=1
  fi
else
  TARGET_HAVE_READLINE=0
fi
AC_SUBST(TARGET_READLINE_INC)
AC_SUBST(TARGET_HAVE_READLINE)


#########
# Figure out whether or not we have a "usleep()" function.
#
AC_CHECK_FUNC(usleep, [TARGET_CFLAGS="$TARGET_CFLAGS -DHAVE_USLEEP=1"])



USE_POSTGRESQL=0
USE_MYSQL=0
USE_ODBC=0
USE_SQLITE3=0
USE_BPT=0
USE_BPT2=0

if test "x$DO_POSTGRESQL" != "xno"; then
  USE_POSTGRESQL=1
else
  USE_POSTGRESQL=0
fi

if test "x$DO_MYSQL" != "xno"; then
  USE_MYSQL=1
else
  USE_MYSQL=0
fi

if test "x$DO_SQLITE3" != "xno"; then
  USE_SQLITE3=1
else
  USE_SQLITE3=0
fi

if test "x$DO_BPT" != "xno"; then
  USE_BPT=1
else
  USE_BPT=0
fi

if test "x$DO_BPT2" != "xno"; then
  USE_BPT2=1
else
  USE_BPT2=0
fi



AM_CONDITIONAL(POSTGRESQL, test x$DO_POSTGRESQL != xno) 
AM_CONDITIONAL(MYSQL, test x$DO_MYSQL != xno) 
AM_CONDITIONAL(SQLITE3, test x$DO_SQLITE3 != xno)
AM_CONDITIONAL(BPT, test x$DO_BPT != xno)
AM_CONDITIONAL(BPT2, test x$DO_BPT2 != xno)


AC_ARG_WITH(default-backend,
[  --with-default-backend         Default backend to use (postgresql,mysql,sqlite,sqlite3)],
   [case "${withval}" in
       postgresql) DEFAULT_BACKEND=postgresql ;;
       mysql)  DEFAULT_BACKEND=mysql ;;
       sqlite)  DEFAULT_BACKEND=sqlite ;;
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

      ulrikp<write-the-sign>users.sourceforge.net

   telling him the exact parameters you gave to configure.

   Thank you.
])
esac

if test "x$DEFAULT_BACKEND_ENUM" == "xnone"; then
   USE_CONFIGURE_DEFAULT_BACKEND_ENUM=0
else	
   USE_CONFIGURE_DEFAULT_BACKEND_ENUM=1
fi

AC_SUBST(DEFAULT_BACKEND_ENUM)
AC_SUBST(USE_CONFIGURE_DEFAULT_BACKEND_ENUM)



dnl Check for FlexLexer.h
dnl AC_MSG_CHECKING([for FlexLexer.h])
dnl if test "x$FLEXLEXER" != "x"; then 
dnl   if test -f $FLEXLEXER; then
dnl     AC_MSG_RESULT(yes);
dnl   else
dnl     AC_MSG_ERROR([
dnl   Environment variable FLEXLEXER does not point to FlexLexer.h.
dnl   Please unset FLEXLEXER, or set it to the correct value.]);
dnl   fi;
dnl else
dnl   if test -f /usr/include/FlexLexer.h; then
dnl     FLEXLEXER=/usr/include/FlexLexer.h;
dnl     AC_MSG_RESULT(yes);
dnl   else
dnl     if test -f /usr/local/include/FlexLexer.h; then
dnl       FLEXLEXER=/usr/local/include/FlexLexer.h;
dnl       AC_MSG_RESULT(yes);
dnl     else
dnl       AC_MSG_ERROR([
dnl   Could not find FlexLexer.h.  Please set the environment variable
dnl   FLEXLEXER to the fully qualified pathname of FlexLexer.h.  E.g.
dnl   with bash:
dnl     $ FLEXLEXER=/usr/local/include/flex/FlexLexer.h
dnl     $ export FLEXLEXER]);
dnl     fi;
dnl   fi;
dnl fi

AC_LANG([C])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE


dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS(memmove memset regcomp strchr strcspn strdup strerror strtol)

dnl Check for types



dnl
dnl PCRE library: Local or System?
dnl


dnl --enable-local-pcre
AC_ARG_ENABLE(local-pcre,
[  --enable-local-pcre           turns local PCRE library on (default)
  --disable-local-pcre          turns local PCRE library off (must use system)],
[case "$enableval" in
       no) LOCAL_PCRE=no ;;
       yes) LOCAL_PCRE=yes ;;
       *) LOCAL_PCRE=yes ;;
 esac],
  LOCAL_PCRE=yes)


AC_CHECK_HEADER([pcre.h], [HAVE_PCRE_H=1], [HAVE_PCRE_H=0])
AC_CHECK_LIB(pcre, pcre_compile, pcrelib=system, pcrelib=local)

USE_LOCAL_PCRE=yes
if test "x$HAVE_PCRE_H" = "x0"; then
  USE_LOCAL_PCRE=yes;
else
  if test "x$pcrelib" = "xsystem"; then
    if test "x$LOCAL_PCRE" = "xyes"; then
       USE_LOCAL_PCRE=yes
    else
       if test "x$LOCAL_PCRE" = "xno"; then
         USE_LOCAL_PCRE=no;
       else
         USE_LOCAL_PCRE=yes
       fi
    fi
  else
    USE_LOCAL_PCRE=yes;
  fi
fi

if test "x$USE_LOCAL_PCRE" = "xyes"; then
  PCRE_DEP=../pcre/libpcre_emdros.la
  PCRE_LIBADD=../pcre/libpcre_emdros.la
  PCRE_DEP_SWIG=../../pcre/libpcre_emdros.la
  PCRE_LIBADD_SWIG=../../pcre/libpcre_emdros.la
else
  PCRE_DEP=
  PCRE_LIBADD=-lpcre
  PCRE_DEP_SWIG=
  PCRE_LIBADD_SWIG=-lpcre
fi

AM_CONDITIONAL(USE_LOCAL_PCRE, test "x$USE_LOCAL_PCRE" = "xyes")
AC_SUBST(PCRE_LIBADD)
AC_SUBST(PCRE_DEP)
AC_SUBST(PCRE_LIBADD_SWIG)
AC_SUBST(PCRE_DEP_SWIG)


dnl
dnl ZLIB library: Local or System?
dnl


dnl --enable-local-zlib
AC_ARG_ENABLE(local-zlib,
[  --enable-local-zlib           turns local ZLIB library on 
  --disable-local-zlib          turns local ZLIB library off (default; must use system)],
[case "$enableval" in
       no) LOCAL_ZLIB=no ;;
       yes) LOCAL_ZLIB=yes ;;
       *) LOCAL_ZLIB=no ;;
 esac],
  LOCAL_ZLIB=no)


AC_CHECK_HEADER([zlib.h], [HAVE_ZLIB_H=1], [HAVE_ZLIB_H=0])
AC_CHECK_LIB(z, deflateEnd, zliblib=system, zliblib=local)

USE_LOCAL_ZLIB=no
if test "x$HAVE_ZLIB_H" = "x0"; then
  USE_LOCAL_ZLIB=yes;
else
  if test "x$zliblib" = "xsystem"; then
    if test "x$LOCAL_ZLIB" = "xyes"; then
       USE_LOCAL_ZLIB=yes
    else
       if test "x$LOCAL_ZLIB" = "xno"; then
         USE_LOCAL_ZLIB=no;
       else
         USE_LOCAL_ZLIB=yes
       fi
    fi
  else
    USE_LOCAL_ZLIB=yes;
  fi
fi

if test "x$USE_LOCAL_ZLIB" = "xyes"; then
  ZLIB_DEP=../zlib/libz_emdros.la
  ZLIB_LIBADD=../zlib/libz_emdros.la
  ZLIB_DEP_SWIG=../../zlib/libz_emdros.la
  ZLIB_LIBADD_SWIG=../../zlib/libz_emdros.la
else
  ZLIB_DEP=
  ZLIB_LIBADD=-lz
  ZLIB_DEP_SWIG=
  ZLIB_LIBADD_SWIG=-lz
fi

AM_CONDITIONAL(USE_LOCAL_ZLIB, test "x$USE_LOCAL_ZLIB" = "xyes")

if test "x$USE_LOCAL_ZLIB" = "xyes"; then
   USE_SYSTEM_ZLIB=0
else
   USE_SYSTEM_ZLIB=1
fi
AC_SUBST(USE_SYSTEM_ZLIB)


AC_SUBST(ZLIB_LIBADD)
AC_SUBST(ZLIB_DEP)
AC_SUBST(ZLIB_LIBADD_SWIG)
AC_SUBST(ZLIB_DEP_SWIG)




dnl
dnl Set BACKENDLDADD and friends
dnl 
BACKENDLDADD=""
BACKENDLDADDSWIG=""
BACKEND_LIBDIRS_AMALGAMATION="/usr/lib"
dnl This must not be empty, or node.js bindings will fail to build, at least
dnl on Mac OS X.
BACKEND_LIBS_AMALGAMATION="-lm"

if test "x$DO_POSTGRESQL" != "xno"; then
  BACKENDLDADD="$BACKENDLDADD $PG_LDADD";
  BACKENDLDADDSWIG="$BACKENDLDADDSWIG $PG_LDADD";
  BACKEND_LIBDIRS_AMALGAMATION="$BACKEND_LIBDIRS_AMALGAMATION $PG_LIBDIRS"
  BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION $PG_LIBS"
fi

if test "x$DO_MYSQL" != "xno"; then
  BACKENDLDADD="$BACKENDLDADD $MY_LDADD";
  BACKENDLDADDSWIG="$BACKENDLDADDSWIG $MY_LDADD";
  BACKEND_LIBDIRS_AMALGAMATION="$BACKEND_LIBDIRS_AMALGAMATION $MY_LIBDIRS"
  BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION $MY_LIBS"
fi

if test x$addlibz = xyes; then
  BACKENDLDADD="$BACKENDLDADD $ZLIB_LIBADD";
  BACKENDLDADDSWIG="$BACKENDLDADDSWIG $ZLIB_LIBADD_SWIG";
  BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION -lz"
fi

if test "x$DO_SQLITE3" != "xno" && test "x${USE_AMALGAMATION}" != "xyes"; then
  BACKENDLDADD="$BACKENDLDADD $SQLITE3_LDADD";
  BACKENDLDADDSWIG="$BACKENDLDADDSWIG $SQLITE3_LDADD_SWIG";
fi

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
dnl Set EMDFLDADD and EMDFDEPS, among others
dnl 
EMDFLDADD="../EMdF/libemdf.la $BACKENDLDADD";
EMDFLDADDSWIG="../../EMdF/libemdf.la $BACKENDLDADDSWIG";
EMDFLDADDSWIGAMALGAMATION="$BACKENDLDADDAMALGAMATION"
EMDFDEPS="../EMdF/libemdf.la";
EMDFDEPSSWIG="../../EMdF/libemdf.la";

if test "x$DO_SQLITE3" != "xno"; then
   EMDFDEPS="$EMDFDEPS $SQLITE3_DEP"
   EMDFDEPSSWIG="$EMDFDEPSSWIG $SQLITE3_DEP_SWIG"
fi

EMDFLDADD="$EMDFLDADD $PROGRAMLDADD"

if test "x${USE_AMALGAMATION}" == "xyes"; then
   EMDROS_LDADD="../amalgamation/libemdros_amalgamation.la"
   EMDROS_LDADD_SWIG="../../amalgamation/libemdros_amalgamation.la"
   EMDROS_DEPS="../amalgamation/libemdros_amalgamation.la"
   EMDROS_DEPS_SWIG="../../amalgamation/libemdros_amalgamation.la"
   if test "x$DO_SQLITE3" != "xno" && test "x$USE_LOCAL_SQLITE3" != "xyes"; then
      EMDROS_LDADD="${EMDROS_LDADD} ${SQLITE3_LDADD}"
      EMDROS_LDADD_SWIG="${EMDROS_LDADD_SWIG} ${SQLITE3_LDADD_SWIG}"
   fi
   EMDROS_LDADD="${EMDROS_LDADD} $PG_LDADD $MY_LDADD -lpthread -ldl"
   EMDROS_LDADD_SWIG="${EMDROS_LDADD_SWIG} $PG_LDADD $MY_LDADD -lpthread -ldl"
else
   EMDROS_LDADD="../harvest/libharvest.la ../MQL/libmql.la ../EMdF/libemdf.la ${SQLITE3_LDADD} ${PCRE_LIBADD} -lpthread -ldl"
   EMDROS_LDADD_SWIG="../../harvest/libharvest.la ../../MQL/libmql.la ../../EMdF/libemdf.la ${SQLITE3_LDADD_SWIG} ${PCRE_LIBADD_SWIG} $PG_LDADD $MY_LDADD -lpthread -ldl"

   EMDROS_DEPS="../harvest/libharvest.la ../MQL/libmql.la ../EMdF/libemdf.la ${SQLITE3_DEP} ${PCRE_DEP}"
   EMDROS_DEPS_SWIG="../../harvest/libharvest.la ../../MQL/libmql.la ../../EMdF/libemdf.la ${SQLITE3_DEP_SWIG} ${PCRE_DEP_SWIG}"

fi

dnl
dnl Set SUN_LDFLAGS
dnl
if test x$ISHOSTSUNCC = xyes; then

  dnl Set SUNWSPRO_PATH
  SUNWSPRO_PATH=`type $CXX | awk '{print $NF}' | sed -e 's:/bin/CC::'`

  dnl Set SUN_LDFLAGS
  SUN_LDFLAGS="-L$SUNWSPRO_PATH/lib $SUN_LDFLAGS"
  SUN_LDFLAGS="-R${my_prefix}/lib/emdros $SUN_LDFLAGS"
  if test "x$DO_POSTGRESQL" != "xno"; then
     SUN_LDFLAGS="-R$libpqdir_dir $SUN_LDFLAGS"
  fi

  if test "x$DO_MYSQL" != "xno"; then
     SUN_LDFLAGS="-R$libmysqlclientdir_dir $SUN_LDFLAGS"
  fi
else
  SUN_LDFLAGS=""
fi
AC_SUBST(SUN_LDFLAGS)

dnl
dnl Set SUN_CXXFLAGS
dnl
if test x$ISHOSTSUNCC = xyes; then
  SUN_CXXFLAGS=""
else
  SUN_CXXFLAGS=""
fi
AC_SUBST(SUN_CXXFLAGS)

dnl Set Darwin flags
if test x$HOSTISDARWIN = xyes; then
  DARWIN_LDFLAGS="";
  DARWIN_CXXFLAGS="";
else
  DARWIN_LDFLAGS="";
  DARWIN_CXXFLAGS="";
fi
AC_SUBST(DARWIN_LDFLAGS)
AC_SUBST(DARWIN_CXXFLAGS)



dnl Set ADDITIONAL_CXXFLAGS and ADDITIONAL_CFLAGS
if test "x$COMPILER_IS_GPLUSPLUS" = "xyes"; then
   ADDITIONAL_CXXFLAGS="-Wall -Wextra"
   ADDITIONAL_CFLAGS="-Wall -Wextra"
   if test x$USE_32BIT = xyes; then
     ADDITIONAL_CXXFLAGS="$ADDITIONAL_CXXFLAGS -m32"
     ADDITIONAL_CFLAGS="$ADDITIONAL_CFLAGS -m32"
   fi
else
   ADDITIONAL_CXXFLAGS=""
   ADDITIONAL_CFLAGS=""
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


if test "x$ISHOSTSUNCC" = "xyes"; then
  for d in EMdF MQL src tests
  do
     mkdir $d/SunWS_cache $d/.libs
     ln -s ../SunWS_cache $d/.libs
  done
  if test "x$DO_SQLITE3" != "xno"; then
    for d in sqlite3/tool sqlite3/src 
    do
       mkdir $d/SunWS_cache $d/.libs
       ln -s ../SunWS_cache $d/.libs
    done
  fi
fi


# 
# --with-XXX=yyy for emdros.spec.in and debian/rules.in
#

WITH_SQLITE3=--with-sqlite3=$ORIGINAL_DO_SQLITE3
WITH_POSTGRESQL=--with-postgresql=$ORIGINAL_DO_POSTGRESQL
WITH_MYSQL=--with-mysql=$ORIGINAL_DO_MYSQL
WITH_BPT=--with-bpt=$ORIGINAL_DO_BPT
WITH_BPT2=--with-bpt=$ORIGINAL_DO_BPT2

WITH_WXWIDGETS=--with-wx=$ORIGINAL_DO_WXWIDGETS

if test xDEBUG = xyes; then
  ENABLE_DEBUG=--enable-debug
else
  ENABLE_DEBUG=--disable-debug
fi

# Use automatically created ac variable
ENABLE_SHARED=--enable-shared=$enable_shared


#
# Compute the "characteristic backend string"
#
# This is the concatentation of the presence or absence of the strings
# "_sq3", "_pg", and "_my" (in that order), where the presence of each
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

CHARACTERISTIC_BACKEND_STRING={CBS_SQLITE3}${CBS_POSTGRESQL}${CBS_MYSQL}
AC_SUBST(CHARACTERISTIC_BACKEND_STRING)


AM_CONDITIONAL(SWIG_WITH_CSHARP_WRAPPERS, test x$DO_SWIG_CSHARP = xyes)
AC_SUBST(SWIG_WITH_CSHARP_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PERL_WRAPPERS, test x$DO_SWIG_PERL = xyes)
AC_SUBST(SWIG_WITH_PERL_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_JAVA_WRAPPERS, test x$DO_SWIG_JAVA = xyes)
AC_SUBST(SWIG_WITH_JAVA_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PHP_WRAPPERS, test x$DO_SWIG_PHP = xyes)
AC_SUBST(SWIG_WITH_PHP_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PHP7_WRAPPERS, test x$DO_SWIG_PHP7 = xyes)
AC_SUBST(SWIG_WITH_PHP7_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PYTHON_WRAPPERS, test x$DO_SWIG_PYTHON = xyes)
AC_SUBST(SWIG_WITH_PYTHON_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PYTHON3_WRAPPERS, test x$DO_SWIG_PYTHON3 = xyes)
AC_SUBST(SWIG_WITH_PYTHON3_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_RUBY_WRAPPERS, test x$DO_SWIG_RUBY = xyes)
AC_SUBST(SWIG_WITH_RUBY_WRAPPERS)



dnl set make macro MAKE
AC_PROG_MAKE_SET


dnl Miscellaneous AC_SUBST
AC_SUBST(LIBS)
AC_SUBST(BISON)
AC_SUBST(FLEX)
AC_SUBST(FLEXLEXER)
AC_SUBST(HAVE_ISTREAM)
AC_SUBST(HAVE_ISTREAM_H)
AC_SUBST(HAVE_OSTREAM)
AC_SUBST(HAVE_OSTREAM_H)
AC_SUBST(USE_POSTGRESQL)
AC_SUBST(USE_SQLITE3)
AC_SUBST(USE_MYSQL)
AC_SUBST(USE_BPT)
AC_SUBST(USE_BPT2)
AC_SUBST(SUN)
AC_SUBST(LINUX)
AC_SUBST(CC)
AC_SUBST(CXX)
AC_SUBST(LDFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(PROGRAMLDADD)
AC_SUBST(BACKENDLDADD)
AC_SUBST(BACKEND_LIBDIRS_AMALGAMATION)
AC_SUBST(BACKEND_LIBS_AMALGAMATION)
AC_SUBST(EMDFLDADD)
AC_SUBST(EMDFLDADDSWIG)
AC_SUBST(EMDFDEPS)
AC_SUBST(EMDFDEPSSWIG)
AC_SUBST(EMDFDB_BACKEND)
AC_SUBST(DEBUGFLAG)
AC_SUBST(SWIG_EXTRA_LIBS)
AC_SUBST(BIG_ENDIAN)
AC_SUBST(WITH_SQLITE3)
AC_SUBST(WITH_POSTGRESQL)
AC_SUBST(WITH_MYSQL)
AC_SUBST(WITH_BPT)
AC_SUBST(WITH_BPT2)
AC_SUBST(WITH_WXWIDGETS)
AC_SUBST(ENABLE_SHARED)
AC_SUBST(ENABLE_DEBUG)
AC_SUBST(EMDROS_LDADD)
AC_SUBST(EMDROS_LDADD_SWIG)
AC_SUBST(EMDROS_DEPS)
AC_SUBST(EMDROS_DEPS_SWIG)


dnl Output
AC_CONFIG_FILES([Makefile 
          include/emdros-lconfig.h 
          include/Makefile 
          debian/rules
          djgpp/Makefile
          pcre/Makefile 
          zlib/Makefile 
          util/Makefile
          wx/Makefile
          src/Makefile 
          src/importtool/Makefile 
          src/importtool/doc/Makefile 
          src/qrytool/Makefile 
          src/qrytool/doc/Makefile 
          src/qrytool/TECkit/Makefile 
          EMdF/Makefile 
          MQL/Makefile
	  amalgamation/Makefile
          tests/Makefile 
          tests/asv/Makefile 
          tests/test_json/Makefile 
          examples/Makefile 
          examples/HAL/Makefile 
          examples/HAL/doc/Makefile 
          examples/ChunkingTool/Makefile
          examples/ChunkingTool/doc/Makefile
          examples/KJVParsed/Makefile
          examples/sfm/Makefile
          harvest/Makefile
          SWIG/Makefile 
          SWIG/csharp/Makefile
          SWIG/java/Makefile
          SWIG/Android/Makefile
          SWIG/perl/Makefile 
          SWIG/php/Makefile
          SWIG/php7/Makefile
          SWIG/python/Makefile 
          SWIG/python3/Makefile 
          SWIG/ruby/Makefile 
          doc/Makefile 
          doc/progref/Makefile 
          win32/Makefile
          sqlite3/Makefile
          sqlite3/tool/Makefile
          sqlite3/src/Makefile])
AC_OUTPUT

chmod 755 debian/rules

dnl Message
if test x$DO_SWIG_PERL = xyes; then
  echo "-----------------------------------------------------------------";
  echo "WARNING: Perl bindings are still experimental.  They may not work."
  echo "Ulrik Sandborg-Petersen"
  echo "-----------------------------------------------------------------";
fi


echo "********************************************"
echo "Using C compiler:     $CC"
echo "Using C++ compiler:   $CXX"
echo "Doing shared libs:    $enable_shared"
echo ""
echo "Compiling with the following backend(s) and libraries:"
if test "x$DO_POSTGRESQL" = "xno"; then
  echo "PostgreSQL:           no"
else
  echo "PostgreSQL:           yes"
fi
if test "x$DO_MYSQL" = "xno"; then
  echo "MySQL:                no"
else
  echo "MySQL:                yes"
fi

echo "SQLite 3.X.X:         $DO_SQLITE3"
echo "BPT:                  $DO_BPT"
echo "BPT2:                 $DO_BPT2"

echo ""

if test "x$USE_CONFIGURE_DEFAULT_BACKEND_ENUM" != "x0"; then
echo "Default backend:      $DEFAULT_BACKEND"
else
echo "Default backend:      Chosen by include/emdros-lconfig.h"
fi

echo ""

if test "x$DO_WXWIDGETS" = "xno"; then
  echo "wxWidgets:            no"
else
  echo "wxWidgets:            yes: version = $WX_VERSION"
  echo "  - Do non-Unicode:   $WX_DO_NONUNICODE"
  echo "  - Do Unicode:       $WX_DO_UNICODE"
fi
echo ""
if test "x$USE_AMALGAMATION" = "xyes"; then
  echo "Use amalgamation lib: yes"
else
  echo "Use amalgamation lib: no"
fi
if test "x$USE_LOCAL_PCRE" = "xyes"; then
  echo "PCRE library:         local"
else
  echo "PCRE library:         system"
fi
if test "x$USE_LOCAL_ZLIB" = "xyes"; then
  echo "ZLIB library:         local"
else
  echo "ZLIB library:         system"
fi
if test "x$TCL_H_PRESENT" = "x1"; then
  echo "Tcl (for SQLite):     yes"
else
  echo "Tcl (for SQLite):     no"
fi

echo ""


if test "x$DO_SWIG_JAVA" = "xyes"; then
  echo "SWIG Backend:         java"
fi
if test "x$DO_SWIG_PYTHON" = "xyes"; then
  echo "SWIG Backend:         python2"
fi
if test "x$DO_SWIG_PYTHON3" = "xyes"; then
  echo "SWIG Backend:         python3"
fi
if test "x$DO_SWIG_CSHARP" = "xyes"; then
  echo "SWIG Backend:         csharp"
fi
if test "x$DO_SWIG_PHP" = "xyes"; then
  echo "SWIG Backend:         php5"
fi
if test "x$DO_SWIG_PHP7" = "xyes"; then
  echo "SWIG Backend:         php7"
fi
if test "x$DO_SWIG_PERL" = "xyes"; then
  echo "SWIG Backend:         perl"
fi
if test "x$DO_SWIG_RUBY" = "xyes"; then
  echo "SWIG Backend:         ruby"
fi

