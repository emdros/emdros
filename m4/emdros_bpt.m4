AC_DEFUN([EMDROS_BPT], [

dnl
dnl The BPT backend is proprietary, and not available in the Open
dnl Source version of Emdros.
dnl
dnl The BPT backend is a lot faster than SQLite, and has much
dnl smaller databases than even SQLite 3.
dnl
dnl For more information, please contact the author of Emdros,
dnl Ulrik Sandborg-Petersen, at ulrikp -at- scripturesys -dot- com
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

USE_BPT=0

if test "x$DO_BPT" != "xno"; then
  USE_BPT=1
else
  USE_BPT=0
fi

AM_CONDITIONAL(BPT, test x$DO_BPT != xno)

WITH_BPT=--with-bpt=$ORIGINAL_DO_BPT

AC_SUBST(USE_BPT)

AC_SUBST(WITH_BPT)


])
