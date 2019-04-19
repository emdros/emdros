AC_DEFUN([EMDROS_WX_SUPPORT], [
AC_ARG_WITH(wx,
[  --with-wx                 Compile the programs for wxwidgets (default: maybe)
                            Valid values: yes, no, maybe],
[case "${withval}" in
       yes) DO_WXWIDGETS=yes ;;
       no)  DO_WXWIDGETS=no ;;
       maybe) DO_WXWIDGETS=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-wx) ;;
     esac],
[DO_WXWIDGETS=maybe])
ORIGINAL_DO_WXWIDGETS=$DO_WXWIDGETS

WITH_WXWIDGETS="--with-wx=$ORIGINAL_DO_WXWIDGETS"
AC_SUBST(WITH_WXWIDGETS)


WX_UNICODE_CXXFLAGS=
WX_UNICODE_LDFLAGS=
WX_UNICODE_LIBS=
WX_DO_UNICODE=no
WX_REZFLAGS=""
WX_VERSION="unknown"

#
# Make '--with-wx=maybe' into '=yes' or '=no'.
# Also find wx-config program.
#
if test x$DO_WXWIDGETS != xno; then
  dnl wx-config program
  AC_CHECK_PROGS([WX_CONFIG_PROG], [wx-config wx-config-3.0], [no])
  if test x$WX_CONFIG_PROG = xno; then
    if test x$DO_WXWIDGETS = xyes; then
      AC_MSG_ERROR([
Error: Could not find wx-config program in path.  This is necessary 
if you wish to compile the programs made with wxWidgets.
])
    else
      # --with-wx=maybe
      DO_WXWIDGETS=no
    fi    
  fi
  if test x$WX_CONFIG_PROG = xno; then
    DO_WXWIDGETS=no
  else
    DO_WXWIDGETS=yes
  fi
fi

AC_MSG_CHECKING([for placement of wx-config])
AC_MSG_RESULT($WX_CONFIG_PROG)
AC_MSG_CHECKING([Whether to do wxWidgets])
AC_MSG_RESULT($DO_WXWIDGETS)


# No we've converted --with-wx=maybe to --with-wx=[no|yes].
# That is, we know whether it's on the system.
WX_DO_UNICODE=no
WX_RELEASE=none
if test x$DO_WXWIDGETS = xyes; then
  WX_RELEASE=`$WX_CONFIG_PROG --release`
  WX_VERSION=`$WX_CONFIG_PROG --version`
  if test "x$WX_RELEASE" = "x2.9" -o "x$WX_RELEASE" = "x3.0" -o "x$WX_RELEASE" = "x3.1"; then
     WXLIBS_ALL="all";
  else
     WXLIBS_ALL="";
  fi
  if test "x$WX_RELEASE" = "x2.8" -o "x$WX_RELEASE" = "x2.9" -o "x$WX_RELEASE" = "x3.0" -o "x$WX_RELEASE" = "x3.1" ; then
    DUMMY_OUT=`$WX_CONFIG_PROG --unicode=yes` && WX_DO_UNICODE=yes

    if test "x$WX_DO_UNICODE" = "xyes"; then
       # Replaced by --libs $WXLIBS_ALL 
       WX_UNICODE_LDFLAGS=""
       WX_UNICODE_LIBS=`$WX_CONFIG_PROG --unicode=yes --libs $WXLIBS_ALL | sed -e 's_-L /_-L/_g'`
       WX_UNICODE_CXXFLAGS=`$WX_CONFIG_PROG --unicode=yes --cxxflags`
       if test "x$CXX" == "x"; then
         CXX=`$WX_CONFIG_PROG --unicode=yes --cxx`
       fi
       if test "x$CC" == "x"; then
         CC=`$WX_CONFIG_PROG --unicode=yes --cc`
       fi
    fi
  else
    AC_MSG_CHECKING([wxWidgets version])
    AC_MSG_ERROR([Error: this configure script does not know how to 
    build Emdros with wxWidgets greater than 3.1 or less than 2.8.])
  fi

  if test x$HOSTISDARWIN = xyes; then
    WX_REZFLAGS=`$WX_CONFIG_PROG --unicode=yes --rezflags`
    WX_UNICODE_LIBS=`$WX_CONFIG_PROG  --unicode=yes --libs $WXLIBS_ALL | sed -e 's_-framework _-Wl,-framework -Wl,_g;' `
    WX_DO_UNICODE=yes
  fi
fi

AC_MSG_CHECKING([wxWidgets RELEASE version])
AC_MSG_RESULT($WX_RELEASE)

AC_MSG_CHECKING([whether to do wxWidgets UNICODE])
AC_MSG_RESULT($WX_DO_UNICODE)


AM_CONDITIONAL(WX_DO_UNICODE, test "x$WX_DO_UNICODE" = "xyes")

AC_SUBST(WX_UNICODE_CXXFLAGS)
AC_SUBST(WX_UNICODE_LDFLAGS)
AC_SUBST(WX_UNICODE_LIBS)
AC_SUBST(WX_REZFLAGS)


AM_CONDITIONAL(WITH_WX_PROGRAMS, test x$DO_WXWIDGETS = xyes)
AC_SUBST(WITH_WX_PROGRAMS)

AC_SUBST(WITH_WXWIDGETS)


])
