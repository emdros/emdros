AC_DEFUN([EMDROS_SOLARIS_END], [
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
  if test "x$SUN_CXXFLAGS" = "x"; then
    dnl We can se it here explicitly if need be
    SUN_CXXFLAGS="";
  else
    dnl It was set by the user. Don't touch it.
    ;
  fi
else
  dnl We are not doing Sun CC
  SUN_CXXFLAGS=""
fi
AC_SUBST(SUN_CXXFLAGS)



])
