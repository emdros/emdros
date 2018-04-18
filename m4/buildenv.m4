AC_DEFUN([EMDROS_BUILDENV], [

EMDROS_VERSION_MAJOR=`echo $VERSION | cut -f1 -d.`
EMDROS_VERSION_MINOR=`echo $VERSION | cut -f2 -d.`
EMDROS_VERSION_RELEASE=`echo $VERSION | cut -f3 -d.`
 
AC_SUBST(EMDROS_VERSION_MAJOR)
AC_SUBST(EMDROS_VERSION_MINOR)
AC_SUBST(EMDROS_VERSION_RELEASE)

dnl --enable-debug
AC_ARG_ENABLE(debug, 
[  --enable-debug          turns debugging on
  --disable-debug         turns debugging off],
[case "$enableval" in
       no) DEBUG=no ;;
       *)  DEBUG=yes ;;
 esac],
  DEBUG=yes)

AX_CXX_COMPILE_STDCXX(11, noext, optional)


dnl --enable-universal_binary
AC_ARG_ENABLE(universal_binary, 
[  --enable-universal_binary     turns Mac OS X Universal Binaries on
  --disable-universal_binary    turns Mac OS X Universal Binaries off],
[case "$enableval" in
       no) UNIVERSAL_BINARY=no ;;
       *)  UNIVERSAL_BINARY=yes ;;
 esac],
  UNIVERSAL_BINARY=no)



dnl set my_prefix
if test x$prefix = xNONE; then
  my_prefix=$ac_default_prefix;
else
  my_prefix=$prefix;
fi

dnl 
dnl Set host information
echo "host is $host"
SUN=0
LINUX=0
DARWIN=0
BIG_ENDIAN=0
if test "`uname -s`" = SunOS; then
  HOSTISSUN=yes;
  SUN=1;
else
  HOSTISSUN=no;
  SUN=0;
fi

HOSTHASSPARC=`uname -p | grep "sparc"`
if test x$HOSTHASSPARC != x; then
  HOSTISSPARC=yes;
  BIG_ENDIAN=1;
else
  HOSTISSPARC=no;
  BIG_ENDIAN=0;
fi

HOSTHASPOWERPC=`echo $host | grep "powerpc"`
if test x$HOSTHASPOWERPC != x; then
  HOSTISPOWERPC=yes;
else
  HOSTISPOWERPC=no;
fi

HOSTHASLINUX=`echo $host | grep "linux"`
if test x$HOSTHASLINUX != x; then
  HOSTISLINUX=yes;
  LINUX=1;
else
  HOSTISLINUX=no;
  LINUX=0;
fi

dnl test for Darwin
HOSTHASDARWIN=`echo $host | grep "darwin"`
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

HOSTHASNETBSD=`echo $host | grep "netbsd"`
if test x$HOSTHASNETBSD != x; then
  HOSTISNETBSD=1
else
  HOSTISNETBSD=0
fi

if test x$HOSTISNETBSD = x1; then
  LDFLAGS="$LDFLAGS -L/usr/pkg/lib "
fi




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


dnl on Darwin, the default rule in aclocal.m4 doesn't
dnl produce the correct result, since we must not link with gcc (CC) but
dnl with g++ (CXX).
dnl So we hand-modify libtool.
dnl A hack, I know, I know.
dnl if test x$HOSTISDARWIN = xyes; then
dnl   cat ./libtool | sed -e '/^archive_cmds/ s/CC/CXX/g;' -e '/^CC=/ a\
dnl CXX="g++"' > ./tmplibtool;
dnl   mv ./tmplibtool ./libtool;
dnl   chmod 755 ./libtool;
dnl fi

if test x$ISHOSTSUNCC = xyes; then
  cat ./libtool | sed -e 's/CXX=\"g++\"/CXX=\"CC\"/g;' > ./tmplibtool;
  mv ./tmplibtool ./libtool;
  chmod 755 ./libtool;
fi

])
