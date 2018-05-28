dnl
dnl ZLIB library: Local or System?
dnl

AC_DEFUN([EMDROS_ZLIB], [



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
  ZLIB_DEPS=../contrib/zlib/libz_emdros.la
  ZLIB_DEPS_BACKBACK=../../contrib/zlib/libz_emdros.la
  ZLIB_DEPS_BACKBACKBACK=../../../contrib/zlib/libz_emdros.la
  ZLIB_LDADD=../contrib/zlib/libz_emdros.la
  ZLIB_LDADD_BACKBACK=../../contrib/zlib/libz_emdros.la
  ZLIB_LDADD_BACKBACKBACK=../../../contrib/zlib/libz_emdros.la
else
  ZLIB_DEPS=
  ZLIB_DEPS_BACKBACK=
  ZLIB_DEPS_BACKBACKBACK=
  ZLIB_LDADD=-lz
  ZLIB_LDADD_BACKBACK=-lz
  ZLIB_LDADD_BACKBACKBACK=-lz
fi

AM_CONDITIONAL(USE_LOCAL_ZLIB, test "x$USE_LOCAL_ZLIB" = "xyes")

if test "x$USE_LOCAL_ZLIB" = "xyes"; then
   USE_SYSTEM_ZLIB=0
else
   USE_SYSTEM_ZLIB=1
fi
AC_SUBST(USE_SYSTEM_ZLIB)


AC_SUBST(ZLIB_LDADD)
AC_SUBST(ZLIB_LDADD_BACKBACK)
AC_SUBST(ZLIB_LDADD_BACKBACKBACK)
AC_SUBST(ZLIB_DEPS)
AC_SUBST(ZLIB_DEPS_BACKBACK)
AC_SUBST(ZLIB_DEPS_BACKBACKBACK)



])
