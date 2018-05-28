AC_DEFUN([EMDROS_MACOS], [

dnl test for Darwin
HOSTHASDARWIN=`echo $host | grep "darwin"`
DARWIN=0
if test x$HOSTHASDARWIN != x; then
  HOSTISDARWIN=yes;
  DARWIN=1;
else
  HOSTISDARWIN=no;
  DARWIN=0;
fi
echo "HOSTISDARWIN is: $HOSTISDARWIN"
AM_CONDITIONAL(HOSTISDARWIN, test x$HOSTISDARWIN = xyes) 


dnl test Darwin major version
if test x$HOSTISDARWIN = xyes; then
  DARWINMAJORVERSION=`uname -r | awk -F '\\.' '{a=1; print $a;}'`
else
  DARWINMAJORVERSION="0"
fi
echo "DARWINMAJORVERSION is: $DARWINMAJORVERSION"


HOSTHASPOWERPC=`echo $host | grep "powerpc"`
if test x$HOSTHASPOWERPC != x; then
  HOSTISPOWERPC=yes;
else
  HOSTISPOWERPC=no;
fi




dnl --enable-universal_binary
AC_ARG_ENABLE(universal_binary, 
[  --enable-universal_binary     turns Mac OS X Universal Binaries on
  --disable-universal_binary    turns Mac OS X Universal Binaries off],
[case "$enableval" in
       no) UNIVERSAL_BINARY=no ;;
       *)  UNIVERSAL_BINARY=yes ;;
 esac],
  UNIVERSAL_BINARY=no)





dnl MACCPU and BIG_ENDIAN
MACCPU=""
if test x$HOSTISDARWIN = xyes; then
  dnl BIG_ENDIAN is set to __BIG_ENDIAN__ on Darwin, 
  dnl so as to facilitate universal compilation.
  BIG_ENDIAN=__BIG_ENDIAN__
  if test x$UNIVERSAL_BINARY = xyes; then
    MACCPU=Universal
  else
    if test x$HOSTISPOWERPC = xyes; then
      MACCPU=PPC
    else
      MACCPU=Intel
    fi
  fi
fi
AC_SUBST(MACCPU)

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


])
