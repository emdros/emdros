AC_DEFUN([EMDROS_BUILDENV], [

dnl
dnl Use automatically created ac variable
dnl
AM_CONDITIONAL(DO_SHARED_LIBS, test x$enable_shared = xyes) 
ENABLE_SHARED=--enable-shared=$enable_shared

dnl --enable-devel
AC_ARG_ENABLE(devel, 
[  --enable-devel          turns development on
  --disable-devel         turns development off (default)],
[case "$enableval" in
       no) DEVEL=no ;;
       *)  DEVEL=yes ;;
 esac],
  DEVEL=no)

if test xDEVEL = xyes; then
  ENABLE_DEVEL =--enable-devel
  USE_AMALGAMATION="0"
else
  ENABLE_DEVEL=--disable-devel
  USE_AMALGAMATION="1"
fi

AC_SUBST(USE_AMALGAMATION)

AM_CONDITIONAL(DO_DEVEL, test x${DEVEL} = xyes) 


dnl --enable-debug
AC_ARG_ENABLE(debug, 
[  --enable-debug          turns debugging on
  --disable-debug         turns debugging off],
[case "$enableval" in
       no) DEBUG=no ;;
       *)  DEBUG=yes ;;
 esac],
  DEBUG=yes)

if test xDEBUG = xyes; then
  ENABLE_DEBUG=--enable-debug
else
  ENABLE_DEBUG=--disable-debug
fi




dnl set my_prefix
if test x$prefix = xNONE; then
  my_prefix=$ac_default_prefix;
else
  my_prefix=$prefix;
fi

dnl 
dnl Set host information
LINUX=0

HOSTHASLINUX=`echo $host | grep "linux"`
if test x$HOSTHASLINUX != x; then
  HOSTISLINUX=yes;
  LINUX=1;
else
  HOSTISLINUX=no;
  LINUX=0;
fi

AC_SUBST(LINUX)

HOSTHASNETBSD=`echo $host | grep "netbsd"`
if test x$HOSTHASNETBSD != x; then
  HOSTISNETBSD=1
else
  HOSTISNETBSD=0
fi

if test x$HOSTISNETBSD = x1; then
  LDFLAGS="$LDFLAGS -L/usr/pkg/lib "
fi





dnl Checks for header files.
AC_LANG([C])

AC_CHECK_HEADERS(limits.h malloc.h stddef.h stdlib.h string.h unistd.h)


EMDROS_VERSION_MAJOR=`echo $VERSION | cut -f1 -d.`
EMDROS_VERSION_MINOR=`echo $VERSION | cut -f2 -d.`
EMDROS_VERSION_RELEASE=`echo $VERSION | cut -f3 -d.`

AC_SUBST(EMDROS_VERSION_MAJOR)
AC_SUBST(EMDROS_VERSION_MINOR)
AC_SUBST(EMDROS_VERSION_RELEASE)

AC_SUBST(ENABLE_SHARED)
AC_SUBST(DEBUG)
AC_SUBST(ENABLE_DEBUG)
AC_SUBST(DEVEL)
AC_SUBST(ENABLE_DEVEL)

dnl set make macro MAKE
AC_PROG_MAKE_SET


dnl
dnl Check for availability of a Python interpreter.
dnl
dnl SWIG 3.0 is required for PHP7 support.
dnl SWIG 4.1 is required for PHP8 support
dnl
AC_CHECK_PROGS(PYTHON_PROGRAM, [python3 python2 python], [no])
if test "x$PYTHON_PROGRAM" == "xno"; then 
   PYTHON_INTERPRETER_AVAILABLE="no"
else
   PYTHON_INTERPRETER_AVAILABLE="yes"
fi
echo "PYTHON_INTERPRETER_AVAILABLE = ${PYTHON_INTERPRETER_AVAILABLE}"
AC_SUBST(PYTHON_INTERPRETER_AVAILABLE)

AC_MSG_CHECKING([if python interpreter available...])
if test "x½PYTHON_INTERPRETER" = "xno"; then
   AC_MSG_RESULT([not found.]);
else
   AC_MSG_RESEULT([${PYTHON_PROGRAM}])
fi



])
