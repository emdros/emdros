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


AC_CHECK_HEADER([pcre2.h], [HAVE_PCRE2_H=1], [HAVE_PCRE2_H=0], [
#define PCRE2_CODE_UNIT_WIDTH 8
])
AC_CHECK_LIB(pcre2-8, pcre2_compile_8, pcrelib=system, pcrelib=local)

USE_LOCAL_PCRE2=yes
if test "x$HAVE_PCRE2_H" = "x0"; then
  USE_LOCAL_PCRE2=yes;
else
  if test "x$pcrelib" = "xsystem"; then
    if test "x$LOCAL_PCRE" = "xyes"; then
       USE_LOCAL_PCRE2=yes
    else
       if test "x$LOCAL_PCRE" = "xno"; then
         USE_LOCAL_PCRE2=no;
       else
         USE_LOCAL_PCRE2=yes
       fi
    fi
  else
    USE_LOCAL_PCRE2=yes;
  fi
fi

if test "x$USE_LOCAL_PCRE2" = "xyes"; then
  if test "x$DEVEL" = "xyes"; then
    PCRE2_DEPS="../pcre2/libpcre2_emdros.la"
    PCRE2_LDADD="../pcre2/libpcre2_emdros.la"
    PCRE2_DEPS_BACKBACK="../../pcre2/libpcre2_emdros.la"
    PCRE2_LDADD_BACKBACK="../../pcre2/libpcre2_emdros.la"
  else
    PCRE2_DEPS="../amalgamation/libemdros_amalgamation.la"
    PCRE2_LDADD="../amalgamation/libemdros_amalgamation.la"
    PCRE2_DEPS_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
    PCRE2_LDADD_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
    PCRE2_DEPS_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
    PCRE2_LDADD_BACKBACK="../../amalgamation/libemdros_amalgamation.la"
  fi
else
  PCRE2_DEPS=
  PCRE2_LDADD=-lpcre2-8
  PCRE2_DEPS_BACKBACK=
  PCRE2_DEPS_BACKBACKBACK=
  PCRE2_LDADD_BACKBACK=-lpcre2-8
  PCRE2_LDADD_BACKBACKBACK=-lpcre2-8
fi

if test "x$USE_LOCAL_PCRE2" = "xyes"; then
   USE_SYSTEM_PCRE2=0
else
   USE_SYSTEM_PCRE2=1
fi
AC_SUBST(USE_SYSTEM_PCRE2)



AM_CONDITIONAL(USE_LOCAL_PCRE2, test "x$USE_LOCAL_PCRE2" = "xyes")
AC_SUBST(PCRE2_LDADD)
AC_SUBST(PCRE2_DEPS)
AC_SUBST(PCRE2_LDADD_BACKBACK)
AC_SUBST(PCRE2_DEPS_BACKBACK)
AC_SUBST(PCRE2_LDADD_BACKBACKBACK)
AC_SUBST(PCRE2_DEPS_BACKBACKBACK)



])
