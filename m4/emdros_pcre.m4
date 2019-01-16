AC_DEFUN([EMDROS_PCRE], [

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
  LOCAL_PCRE=no)


AC_CHECK_HEADER([pcre.h], [HAVE_PCRE_H=1], [HAVE_PCRE_H=0], [
#define PCRE_CODE_UNIT_WIDTH 8
])
AC_CHECK_LIB(pcre-8, pcre_compile_8, pcrelib=system, pcrelib=local)

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
  if test "x$DEVEL" = "xyes"; then
    PCRE_DEPS="../pcre/libpcre_emdros.la"
    PCRE_LDADD="../pcre/libpcre_emdros.la"
    PCRE_DEPS_BACKBACK="../../pcre/libpcre_emdros.la"
    PCRE_LDADD_BACKBACK="../../pcre/libpcre_emdros.la"
  else
    PCRE_DEPS="../amalgamation/libemdros_amalgamation.la"
    PCRE_LDADD="../amalgamation/libemdros_amalgamation.la"
    PCRE_DEPS_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
    PCRE_LDADD_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
    PCRE_DEPS_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
    PCRE_LDADD_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
  fi
else
  PCRE_DEPS=
  PCRE_LDADD=-lpcre-8
  PCRE_DEPS_BACKBACK=
  PCRE_DEPS_BACKBACKBACK=
  PCRE_LDADD_BACKBACK=-lpcre-8
  PCRE_LDADD_BACKBACKBACK=-lpcre-8
fi

if test "x$USE_LOCAL_PCRE" = "xyes"; then
   USE_SYSTEM_PCRE=0
else
   USE_SYSTEM_PCRE=1
fi
AC_SUBST(USE_SYSTEM_PCRE)



AM_CONDITIONAL(USE_LOCAL_PCRE, test "x$USE_LOCAL_PCRE" = "xyes")
AC_SUBST(PCRE_LDADD)
AC_SUBST(PCRE_DEPS)
AC_SUBST(PCRE_LDADD_BACKBACK)
AC_SUBST(PCRE_DEPS_BACKBACK)
AC_SUBST(PCRE_LDADD_BACKBACKBACK)
AC_SUBST(PCRE_DEPS_BACKBACKBACK)



])
