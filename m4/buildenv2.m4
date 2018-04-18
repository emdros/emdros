AC_DEFUN([EMDROS_BUILDENV2], [

echo "HOSTISSUN is $HOSTISSUN"
COMPILER_IS_GPLUSPLUS=no
CXX_filename=`echo "$CXX" | awk '{print $1;}'`
CXX_basename=`basename $CXX_filename`
if test "x$CXX_basename" = "xCC"; then
  COMPILERISCC=yes;
  COMPILER_IS_GPLUSPLUS=no;
else
  COMPILERISCC=no;
  if test "x$CXX_basename" = "xg++"; then
    COMPILER_IS_GPLUSPLUS=yes;
  fi
fi
echo "COMPILERISCC is $COMPILERISCC"
echo "COMPILER_IS_GPLUSPLUS is $COMPILER_IS_GPLUSPLUS"
AM_CONDITIONAL(HOSTISSUNCC, test x$HOSTISSUN = xyes -a x$COMPILERISCC = xyes)
AM_CONDITIONAL(COMPILER_IS_GNU_GPLUSPLUS, test x$COMPILER_IS_GPLUSPLUS = xyes)


if test x$HOSTISSUN = xyes -a x$COMPILERISCC = xyes; then
  ISHOSTSUNCC=yes;
else
  ISHOSTSUNCC=no;
fi
echo "ISHOSTSUNCC is $ISHOSTSUNCC"


dnl If this is Sun CC, set AR, AR_FLAGS, and LD
if test x$ISHOSTSUNCC = xyes; then
  AR=CC;
  AR_FLAGS="-xar -o";
  ARFLAGS="-xar -o";
  LD=CC;
  CXX=CC;
  CC=cc;
  export AR AR_FLAGS ARFLAGS LD CC CXX;
else
  ARFLAGS="cru"
fi

AC_SUBST(ARFLAGS)

dnl Checks for header files.
AC_LANG([C])
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h malloc.h stddef.h stdlib.h string.h unistd.h)

dnl Check for istream
AC_LANG([C++])
AC_CHECK_HEADER([istream], [HAVE_ISTREAM=1], [HAVE_ISTREAM=0])
if test "x$HAVE_ISTREAM" = "x0"; then
  AC_CHECK_HEADER([istream.h], [HAVE_ISTREAM_H=1], [HAVE_ISTREAM_H=0])
  if test "x$HAVE_ISTREAM_H" = "x0"; then
    AC_MSG_ERROR([
Error: Could not find istream header file, and istream.h is not
present either. Do you have a working C++ environment?
])
  fi
fi

dnl Check for ostream
AC_CHECK_HEADER([ostream], [HAVE_OSTREAM=1], [HAVE_OSTREAM=0])
if test "x$HAVE_OSTREAM" = "x0"; then
  AC_CHECK_HEADER([ostream.h], [HAVE_OSTREAM_H=1], [HAVE_OSTREAM_H=0])
  if test "x$HAVE_OSTREAM_H" = "x0"; then
    AC_MSG_ERROR([
Error: Could not find ostream header file, and ostream.h is not
present either. Do you have a working C++ environment?
])
  fi
fi





])
