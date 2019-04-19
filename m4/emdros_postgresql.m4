AC_DEFUN([EMDROS_POSTGRESQL], [

#
# Add --with-postgresql
#
#
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
Error: Could not find the libpq.a or libpq.so or libpq.dylib library.
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

dnl
dnl Test for PostgreSQL header files
dnl
if test "x$DO_POSTGRESQL" != "xno"; then
  dnl Check for PostgreSQL header files
  AC_LANG([C])

  dnl First try some well-known directories.
  dnl We want to do this even though we check for <libpg-fe.h> below
  dnl with the AC_CHECK_HEADER function, because we have some special
  dnl directories to check for Solaris support.
  AC_MSG_CHECKING([for postgresql include directory])
  postgresqldir_dir="";
  postgresqldir_found="false";
  postgresqldir_try_dirs="$POSTGRESQL_INCLUDE_DIR /usr/local/include /usr/pkg/include /usr/local/pgsql/include /usr/include/pgsql /usr/include/postgresql /opt/PostgreSQL/include /sw/include/postgresql /usr/postgres/8.3/include"
  for d in $postgresqldir_try_dirs; do
    if test -f $d/libpq-fe.h; then
      postgresqldir_dir=$d;
      postgresqldir_found="true";
      break;
    fi;
  done
  if test x$postgresqldir_found = xtrue; then
    AC_MSG_RESULT([$postgresqldir_dir])

    if test "x$postgresqldir_dir" != "x"; then
      CXXFLAGS="$CXXFLAGS -I$postgresqldir_dir";
      CPPLAGS="$CPPLAGS -I$postgresqldir_dir";
      CPPFLAGS="$CPPFLAGS -I$postgresqldir_dir"

      export CXXFLAGS;
      export CPPFLAGS;
      export CPPFLAGS;
    fi
  else
    AC_MSG_RESULT([Not found... Let's try again.])
  fi

  dnl Postgresql include-dir
  AC_CHECK_HEADER([libpq-fe.h],
                  [have_libpq_fe_h=true],
                  [have_libpq_fe_h=false])

  if test "xhave_libpq_fe_h" = "x1"; then
     AC_DEFINE([HAVE_LIBPQ_FE_H], [1],
               [Define to 1 if you have <libpq-fe.h> from PostgreSQL])
  else
     AC_DEFINE([HAVE_LIBPQ_FE_H], [0],
               [Define to 1 if you have <libpq-fe.h> from PostgreSQL])
  fi

  AC_MSG_CHECKING([whether to use <libpq-fe.h>])
  if test "x$have_libpq_fe_h" = "xtrue"; then
    AC_MSG_RESULT([yes]);
  else
    if test "x$DO_POSTGRESQL" = "xmaybe"; then
      AC_MSG_RESULT([Not found... Not doing PostgreSQL])
      DO_POSTGRESQL="no"
    else
      AC_MSG_ERROR([
  Error: Could not find the <libpq-fe.h> include-file.
  Please set the POSTGRESQL_INCLUDE_DIR environment variable and run ./configure
  again.
      ])
    fi
  fi


  dnl
  dnl Take "-lpq" out of LIBS.  They are taken care of
  dnl by EMDFLDADD.
  AC_LANG([C])
  LIBS=`echo $LIBS | sed 's/-lpq//g'`
fi


dnl End checking for PostgreSQL
USE_POSTGRESQL=0

if test "x$DO_POSTGRESQL" != "xno"; then
  USE_POSTGRESQL=1
else
  USE_POSTGRESQL=0
fi

AM_CONDITIONAL(POSTGRESQL, test x$DO_POSTGRESQL != xno) 

if test "x$DO_POSTGRESQL" != "xno"; then
  BACKEND_LDADD="$BACKEND_LDADD $PG_LDADD";
  BACKEND_LDADD_BACKBACK="$BACKEND_LDADD_BACKBACK $PG_LDADD";
  BACKEND_LDADD_BACKBACKBACK="$BACKEND_LDADD_BACKBACKBACK $PG_LDADD";
  BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION $PG_LIBS"
  BACKEND_LDADD_AMALGAMATION="$BACKEND_LDADD_AMALGAMATION $PG_LIBS"
dnl As of 2017-11-10 (Fedora 26),
dnl the libpq.pc pkg-config file on Fedora does not list the correct
dnl libraries, so we don't include libpq among PKGCONFIG_REQUIRED_AMALGAMATION.
dnl PKGCONFIG_REQUIRED_AMALGAMATION="$PKGCONFIG_REQUIRED_AMALGAMATION libpq"
  PKGCONFIG_LIBS_AMALGAMATION="$PKGCONFIG_LIBS_AMALGAMATION $PG_LDADD"
  if test "x$PG_INCLUDE_DIR" != "x"; then
    POSTGRESQL_DASH_INCLUDE="-I${PG_INCLUDE_DIR}"
  else
    POSTGRESQL_DASH_INCLUDE=""
  fi
fi


WITH_POSTGRESQL=--with-postgresql=$ORIGINAL_DO_POSTGRESQL

if test "x$DO_POSTGRESQL" != "xno"; then
  CBS_POSTGRESQL="_pg"
else
  CBS_POSTGRESQL=""
fi

AC_SUBST(USE_POSTGRESQL)
AC_SUBST(WITH_POSTGRESQL)

AC_SUBST(POSTGRESQL_DASH_INCLUDE)


])
