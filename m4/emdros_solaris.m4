AC_DEFUN([EMDROS_SOLARIS], [

dnl 
dnl Set host information
dnl
SUN=0
BIG_ENDIAN=0
if test "`uname -s`" = SunOS; then
  HOSTISSUN=yes;
  SUN=1;
else
  HOSTISSUN=no;
  SUN=0;
fi
AC_SUBST(SUN)

HOSTHASSPARC=`uname -p | grep "sparc"`
if test x$HOSTHASSPARC != x; then
  HOSTISSPARC=yes;
  BIG_ENDIAN=1;
else
  HOSTISSPARC=no;
  BIG_ENDIAN=0;
fi


dnl
dnl FIXME: Is the code commented out below necessary on HOSTSUNCC?
dnl
dnl if test x$ISHOSTSUNCC = xyes; then
dnl  cat ./libtool | sed -e 's/CXX=\"g++\"/CXX=\"CC\"/g;' > ./tmplibtool;
dnl  mv ./tmplibtool ./libtool;
dnl  chmod 755 ./libtool;
dnl fi

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

if test "x$ISHOSTSUNCC" = "xyes"; then
  for d in lib/base lib/EMdF lib/MQL lib/harvest lib/importers lib/util src/gui src/cli
  do
     mkdir $d/SunWS_cache $d/.libs
     ln -s ../../SunWS_cache $d/.libs
  done
  if test "x$DO_SQLITE3" != "xno"; then
    for d in contrib/sqlite3/tool contrib/sqlite3/src 
    do
       mkdir $d/SunWS_cache $d/.libs
       ln -s ../SunWS_cache $d/.libs
    done
  fi
fi


dnl
dnl Set SUN_LDFLAGS
dnl
if test x$ISHOSTSUNCC = xyes; then

  dnl Set SUNWSPRO_PATH
  SUNWSPRO_PATH=`type $CXX | awk '{print $NF}' | sed -e 's:/bin/CC::'`

  dnl Set SUN_LDFLAGS
  SUN_LDFLAGS="-L$SUNWSPRO_PATH/lib $SUN_LDFLAGS"
  SUN_LDFLAGS="-R${my_prefix}/lib/emdros $SUN_LDFLAGS"
  if test "x$DO_POSTGRESQL" != "xno"; then
     SUN_LDFLAGS="-R$libpqdir_dir $SUN_LDFLAGS"
  fi

  if test "x$DO_MYSQL" != "xno"; then
     SUN_LDFLAGS="-R$libmysqlclientdir_dir $SUN_LDFLAGS"
  fi
else
  SUN_LDFLAGS=""
fi
AC_SUBST(SUN_LDFLAGS)

dnl
dnl Set SUN_CXXFLAGS
dnl
if test x$ISHOSTSUNCC = xyes; then
  SUN_CXXFLAGS=""
else
  SUN_CXXFLAGS=""
fi
AC_SUBST(SUN_CXXFLAGS)



])
