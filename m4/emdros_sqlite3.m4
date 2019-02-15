AC_DEFUN([EMDROS_SQLITE3], [

AC_ARG_WITH(sqlite3,
[  --with-sqlite3           Use Sqlite3 as a backend (default: maybe)
                          Valid values: yes, no],
[case "${withval}" in
       yes) DO_SQLITE3=yes ;;
       no)  DO_SQLITE3=no ;;
       maybe) DO_SQLITE3=yes ;;
       local)  DO_SQLITE3=yes ;;
       system)  DO_SQLITE3=yes ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-sqlite3) ;;
     esac],
[DO_SQLITE3=yes])
ORIGINAL_DO_SQLITE3=$DO_SQLITE3


dnl
dnl Sqlite3
dnl

dnl always use local SQLite3
USE_LOCAL_SQLITE3=yes
SQLITE3_DEPS=""
SQLITE3_DEPS_BACKBACK=""
SQLITE3_DEPS_BACKBACKBACK=""
SQLITE3_LDADD=""
SQLITE3_LDADD_BACKBACK=""
SQLITE3_LDADD_BACKBACKBACK=""

if test "x$DO_SQLITE3" != "xno"; then
  if test "x$DEVEL" = "xyes"; then
    SQLITE3_DEPS="../sqlite3/src/libsqlite3_emdros.la"
    SQLITE3_DEPS_BACKBACK="../../sqlite3/src/libsqlite3_emdros.la"
    SQLITE3_DEPS_BACKBACKBACK="../../../sqlite3/src/libsqlite3_emdros.la"
    SQLITE3_LDADD="../sqlite3/src/libsqlite3_emdros.la"
    SQLITE3_LDADD_BACKBACK="../../sqlite3/src/libsqlite3_emdros.la"
    SQLITE3_LDADD_BACKBACKBACK="../../../sqlite3/src/libsqlite3_emdros.la"
  else
    SQLITE3_DEPS="../amalgamation/libemdros_amalgamation.la"
    SQLITE3_DEPS_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
    SQLITE3_DEPS_BACKBACKBACK="../../../amalgamation/libemdros_amalgamation.la"
    SQLITE3_LDADD="../amalgamation/libemdros_amalgamation.la" 
    SQLITE3_LDADD_BACKBACK="../../amalgamation/libemdros_amalgamation.la" 
    SQLITE3_LDADD_BACKBACKBACK="../../../amalgamation/libemdros_amalgamation.la"
  fi
fi

AM_CONDITIONAL(USE_LOCAL_SQLITE3, test "x$USE_LOCAL_SQLITE3" = "xyes")

if test "x$USE_LOCAL_SQLITE3" = "xyes"; then
   USE_SYSTEM_SQLITE3=0
else
   USE_SYSTEM_SQLITE3=1
fi
AC_SUBST(USE_SYSTEM_SQLITE3)

AC_SUBST(SQLITE3_DEPS)
AC_SUBST(SQLITE3_DEPS_BACKBACK)
AC_SUBST(SQLITE3_DEPS_BACKBACKBACK)
AC_SUBST(SQLITE3_LDADD)
AC_SUBST(SQLITE3_LDADD_BACKBACK)
AC_SUBST(SQLITE3_LDADD_BACKBACKBACK)

if test "x$DO_SQLITE3" != "xno"; then
  if test "x$USE_LOCAL_SQLITE3" = "xyes"; then
    DO_SQLITE3="local"
  else
    AC_MSG_ERROR(["Error in configure.ac logic for m4/emdros_sqlite3.m4. Please contact Ulrik Sandborg-Petersen <ulrikp-at-scripturesys-dot-com>"])
  fi
fi


USE_SQLITE3=0

if test "x$DO_SQLITE3" != "xno"; then
  USE_SQLITE3=1
else
  USE_SQLITE3=0
fi

AM_CONDITIONAL(SQLITE3, test x$DO_SQLITE3 != xno)


WITH_SQLITE3=--with-sqlite3=$ORIGINAL_DO_SQLITE3

if test "x$DO_SQLITE3" != "xno"; then
  CBS_SQLITE3="_sq3"
else
  CBS_SQLITE3=""
fi

AC_SUBST(USE_SQLITE3)

AC_SUBST(WITH_SQLITE3)


])
