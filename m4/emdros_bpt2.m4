AC_DEFUN([EMDROS_BPT2], [

dnl
dnl The BPT2 backend (BPT version 2) is proprietary, and not available 
dnl in the Open Source version of Emdros.
dnl
dnl The BPT2 backend is a lot faster than BPT version 1, and has much
dnl smaller databases than BPT version 1.
dnl
dnl For more information, please contact the author of Emdros,
dnl Ulrik Sandborg-Petersen, at ulrikp -at- scripturesys -dot- com
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

USE_BPT2=0

if test "x$DO_BPT2" != "xno"; then
  USE_BPT2=1
else
  USE_BPT2=0
fi


AM_CONDITIONAL(BPT2, test x$DO_BPT2 != xno)

WITH_BPT2=--with-bpt=$ORIGINAL_DO_BPT2

AC_SUBST(USE_BPT2)

AC_SUBST(WITH_BPT2)

])
