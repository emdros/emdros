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


WX_NONUNICODE_CXXFLAGS=
WX_NONUNICODE_LDFLAGS=
WX_NONUNICODE_LIBS=
WX_UNICODE_CXXFLAGS=
WX_UNICODE_LDFLAGS=
WX_UNICODE_LIBS=
WX_DO_UNICODE=no
WX_DO_NONUNICODE=no
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


dnl libstdc++ is deprecated on Mac OS X. Use libc++ instead
dnl CXX needs to be modified, not CXXFLAGS or LDFLAGS.
if test "x$HOSTISDARWIN" = xyes; then
   if test "$HAVE_CXX11" = "1"; then
      eval "CXX=\"$CXX --stdlib=libc++\""
   fi
fi

# No we've converted --with-wx=maybe to --with-wx=[no|yes].
# That is, we know whether it's on the system.
WX_DO_NONUNICODE=no
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

    # Don't ever do non-unicode
    #DUMMY_OUT=`$WX_CONFIG_PROG --unicode=no` && WX_DO_NONUNICODE=yes
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
    if test "x$WX_DO_NONUNICODE" = "xyes"; then
       # Replaced by --libs $WXLIBS_ALL 
       WX_NONUNICODE_LDFLAGS=""
       WX_NONUNICODE_LIBS=`$WX_CONFIG_PROG --unicode=no --libs $WXLIBS_ALL | sed -e 's_-L /_-L/_g'`
       WX_NONUNICODE_CXXFLAGS=`$WX_CONFIG_PROG --unicode=no --cxxflags`
    fi
  else
    AC_MSG_CHECKING([wxWidgets version])
    AC_MSG_ERROR([Error: this configure script does not know how to 
    build Emdros with wxWidgets greater than 3.1 or less than 2.8.])
  fi

  if test x$HOSTISDARWIN = xyes; then
    WX_REZFLAGS=`$WX_CONFIG_PROG --unicode=yes --rezflags`
    WX_UNICODE_LIBS=`$WX_CONFIG_PROG  --unicode=yes --libs $WXLIBS_ALL | sed -e 's_-framework _-Wl,-framework -Wl,_g;' `
    WX_DO_NONUNICODE=no
    WX_DO_UNICODE=yes
  fi
fi

AC_MSG_CHECKING([wxWidgets RELEASE version])
AC_MSG_RESULT($WX_RELEASE)

AC_MSG_CHECKING([whether to do wxWidgets NON-UNICODE])
AC_MSG_RESULT($WX_DO_NONUNICODE)
AC_MSG_CHECKING([whether to do wxWidgets UNICODE])
AC_MSG_RESULT($WX_DO_UNICODE)


AM_CONDITIONAL(WX_DO_UNICODE, test "x$WX_DO_UNICODE" = "xyes")
AM_CONDITIONAL(WX_DO_NONUNICODE, test "x$WX_DO_NONUNICODE" = "xyes")

AC_SUBST(WX_NONUNICODE_CXXFLAGS)
AC_SUBST(WX_NONUNICODE_LDFLAGS)
AC_SUBST(WX_NONUNICODE_LIBS)
AC_SUBST(WX_UNICODE_CXXFLAGS)
AC_SUBST(WX_UNICODE_LDFLAGS)
AC_SUBST(WX_UNICODE_LIBS)
AC_SUBST(WX_REZFLAGS)


AM_CONDITIONAL(WITH_WX_PROGRAMS, test x$DO_WXWIDGETS = xyes)
AC_SUBST(WITH_WX_PROGRAMS)

if test "x$HOSTISDARWIN" = "xyes"; then
    AC_MSG_CHECKING([Current build version (for setting Mac OS X minimum version)...])
    if test "x$WX_RELEASE" = "x2.8" -o "x$WX_RELEASE" = "xnone"; then
      dnl Mac OSX 10.4
      if test "x$DARWINMAJORVERSION" = "x8"; then
        AC_MSG_RESULT([10.4])
        MACOSX_VERSION_MIN="10.4"
        # SQLite3 must have the -DSQLITE_WITHOUT_ZONEMALLOC when compiling with Mac OS X minimum version < 10.5.
        MACOSX_CFLAGS="-DSQLITE_WITHOUT_ZONEMALLOC"
      dnl Mac OS X 10.5: XCode can't compile 64-bit code
      elif test "x$DARWINMAJORVERSION" = "x9"; then
        AC_MSG_RESULT([10.5])
        MACOSX_VERSION_MIN="10.4"
        # SQLite3 must have the -DSQLITE_WITHOUT_ZONEMALLOC when compiling with Mac OS X minimum version < 10.5.
        MACOSX_CFLAGS="-DSQLITE_WITHOUT_ZONEMALLOC"
      dnl Mac OS X 10.6
      elif test "x$DARWINMAJORVERSION" = "x10"; then
        AC_MSG_RESULT([10.6]) 
        MACOSX_VERSION_MIN="10.4"
        # SQLite3 must have the -DSQLITE_WITHOUT_ZONEMALLOC when compiling with Mac OS X minimum version < 10.5.
        MACOSX_CFLAGS="-DSQLITE_WITHOUT_ZONEMALLOC"
      dnl Mac OS X 10.7: XCode can't do PPC binaries
      elif test "x$DARWINMAJORVERSION" = "x11"; then
        AC_MSG_RESULT([10.7]) 
        MACOSX_VERSION_MIN="10.6"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.8: XCode can't do PPC binaries
      elif test "x$DARWINMAJORVERSION" = "x12"; then
        AC_MSG_RESULT([10.8]) 
        MACOSX_VERSION_MIN="10.7"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.8: XCode can't do PPC binaries
      elif test "x$DARWINMAJORVERSION" = "x13"; then
        AC_MSG_RESULT([10.9]) 
        MACOSX_VERSION_MIN="10.8"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.10.
      elif test "x$DARWINMAJORVERSION" = "x14"; then
        AC_MSG_RESULT([10.10]) 
        MACOSX_VERSION_MIN="10.9"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.11.
      elif test "x$DARWINMAJORVERSION" = "x15"; then
        AC_MSG_RESULT([10.11]) 
        MACOSX_VERSION_MIN="10.10"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.12.
      elif test "x$DARWINMAJORVERSION" = "x16"; then
        AC_MSG_RESULT([10.12]) 
        MACOSX_VERSION_MIN="10.11"
        MACOSX_CFLAGS=""
      else
        AC_MSG_RESULT([unknown... Using 10.11.]) 
        MACOSX_VERSION_MIN="10.11"
      fi
    elif test "x$WX_RELEASE" == "x2.9" -o "x$WX_RELEASE" == "x3.0"; then
      if test "x$DARWINMAJORVERSION" = "x9"; then
        AC_MSG_RESULT([10.5])
        MACOSX_VERSION_MIN="10.5"
        # SQLite3 must have the -DSQLITE_WITHOUT_ZONEMALLOC when compiling with Mac OS X minimum version < 10.5.
        MACOSX_CFLAGS="-DSQLITE_WITHOUT_ZONEMALLOC"
      dnl Mac OS X 10.6
      elif test "x$DARWINMAJORVERSION" = "x10"; then
        AC_MSG_RESULT([10.6]) 
        MACOSX_VERSION_MIN="10.6"
        # SQLite3 must have the -DSQLITE_WITHOUT_ZONEMALLOC when compiling with Mac OS X minimum version < 10.5.
        #MACOSX_CFLAGS="-DSQLITE_WITHOUT_ZONEMALLOC"
      dnl Mac OS X 10.7: XCode can't do PPC binaries
      elif test "x$DARWINMAJORVERSION" = "x11"; then
        AC_MSG_RESULT([10.7]) 
        MACOSX_VERSION_MIN="10.6"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.8: XCode can't do PPC binaries
      elif test "x$DARWINMAJORVERSION" = "x12"; then
        AC_MSG_RESULT([10.8]) 
        MACOSX_VERSION_MIN="10.7"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.9: XCode can't do PPC binaries
      elif test "x$DARWINMAJORVERSION" = "x13"; then
        AC_MSG_RESULT([10.9]) 
        MACOSX_VERSION_MIN="10.8"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.10.
      elif test "x$DARWINMAJORVERSION" = "x14"; then
        AC_MSG_RESULT([10.10]) 
        MACOSX_VERSION_MIN="10.9"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.11.
      elif test "x$DARWINMAJORVERSION" = "x15"; then
        AC_MSG_RESULT([10.11]) 
        MACOSX_VERSION_MIN="10.10"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.12.
      elif test "x$DARWINMAJORVERSION" = "x16"; then
        AC_MSG_RESULT([10.12]) 
        MACOSX_VERSION_MIN="10.11"
        MACOSX_CFLAGS=""
      else
        AC_MSG_RESULT([unknown... Using 10.11.]) 
        MACOSX_VERSION_MIN="10.11"
      fi
    else dnl wxRELEASE != 2.8, 2.9, or 3.0, so probably 3.1
      dnl Mac OS X 10.7: XCode can't do PPC binaries
      if test "x$DARWINMAJORVERSION" = "x11"; then
        AC_MSG_RESULT([10.7]) 
        MACOSX_VERSION_MIN="10.7"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.8: XCode can't do PPC binaries
      elif test "x$DARWINMAJORVERSION" = "x12"; then
        AC_MSG_RESULT([10.8]) 
        MACOSX_VERSION_MIN="10.7"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.9.
      elif test "x$DARWINMAJORVERSION" = "x13"; then
        AC_MSG_RESULT([10.9]) 
        MACOSX_VERSION_MIN="10.8"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.10.
      elif test "x$DARWINMAJORVERSION" = "x14"; then
        AC_MSG_RESULT([10.10]) 
        MACOSX_VERSION_MIN="10.9"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.11.
      elif test "x$DARWINMAJORVERSION" = "x15"; then
        AC_MSG_RESULT([10.11]) 
        MACOSX_VERSION_MIN="10.10"
        MACOSX_CFLAGS=""
      dnl Mac OS X 10.12.
      elif test "x$DARWINMAJORVERSION" = "x16"; then
        AC_MSG_RESULT([10.12]) 
        MACOSX_VERSION_MIN="10.11"
        MACOSX_CFLAGS=""
      else
        AC_MSG_RESULT([unknown... Using 10.11.]) 
        MACOSX_VERSION_MIN="10.11"
      fi
    fi

    CFLAGS="$CFLAGS -mmacosx-version-min=$MACOSX_VERSION_MIN $MACOSX_CFLAGS"
    CXXFLAGS="$CXXFLAGS -mmacosx-version-min=$MACOSX_VERSION_MIN"
    export CFLAGS
    export CXXFLAGS
    AC_MSG_CHECKING([Mac OS X minimum version...])
    AC_MSG_RESULT(["$MACOSX_VERSION_MIN"])
    AC_SUBST(MACOSX_VERSION_MIN)
fi



if test "x$UNIVERSAL_BINARY" = "xyes"; then
  if test x$HOSTISDARWIN != xyes; then
    AC_MSG_ERROR([Error: Cannot make Universal Binaries except on Mac OS X. 
This is not Mac OS X. Aborting.])
  fi
  if test "x$enable_dependency_tracking" != "xno"; then
    AC_MSG_ERROR([

Error: In order to make Universal Binaries, you must also 
give configure the --disable-dependency-tracking switch.
Please do so, and try again.])
  fi
    
  if test "x$WX_RELEASE" = "x2.8"; then
    AC_MSG_CHECKING([Current Mac OS X build version on wxWidgets 2.8...])
    dnl Mac OSX 10.4
    if test "x$DARWINMAJORVERSION" = "x8"; then
      AC_MSG_RESULT([10.4])
      ARCH_FLAGS="-arch i386 -arch ppc"
      ISYSROOT="-isysroot /Developer/SDKs/MacOSX10.4u.sdk"
    dnl Mac OS X 10.5: XCode can't compile 64-bit code
    elif test "x$DARWINMAJORVERSION" = "x9"; then
      AC_MSG_RESULT([10.5])
      ARCH_FLAGS="-arch i386 -arch ppc"
      ISYSROOT="-isysroot /Developer/SDKs/MacOSX10.4u.sdk"
    dnl Mac OS X 10.6
    elif test "x$DARWINMAJORVERSION" = "x10"; then
      AC_MSG_RESULT([10.6]) 
      ARCH_FLAGS="-arch i386 -arch ppc"
      ISYSROOT="-isysroot /Developer/SDKs/MacOSX10.4u.sdk"
    dnl Mac OS X 10.7: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x11"; then
      AC_MSG_RESULT([10.7]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
      ISYSROOT="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk/"
    dnl Mac OS X 10.8: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x12"; then
      AC_MSG_RESULT([10.8]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.9: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x13"; then
      AC_MSG_RESULT([10.9]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
      ISYSROOT="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/"
    else
      AC_MSG_RESULT([unknown...]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    fi
    LDFLAGS="$LDFLAGS $ARCH_FLAGS"
    export LDFLAGS
    CFLAGS="$CFLAGS $ARCH_FLAGS $ISYSROOT"
    CXXFLAGS="$CXXFLAGS $ARCH_FLAGS $ISYSROOT"
    AC_MSG_CHECKING([Universal build arch flags...])
    AC_MSG_RESULT(["$ARCH_FLAGS"])
    AC_MSG_CHECKING([CFLAGS])
    AC_MSG_RESULT(["$CFLAGS"])
    AC_MSG_CHECKING([CXXFLAGS])
    AC_MSG_RESULT(["$CXXFLAGS"])
    AC_MSG_CHECKING([LDFLAGS])
    AC_MSG_RESULT(["$LDFLAGS"])
  elif test "x$WX_RELEASE" = "x2.9" -o "x$WX_RELEASE" = "x3.0"; then
    AC_MSG_CHECKING([Current Mac OS X build version...])
    dnl Mac OS X 10.5: XCode can't compile 64-bit code
    if test "x$DARWINMAJORVERSION" = "x9"; then
      AC_MSG_RESULT([10.5])
      ARCH_FLAGS="-arch i386 -arch ppc"
    dnl Mac OS X 10.6
    elif test "x$DARWINMAJORVERSION" = "x10"; then
      AC_MSG_RESULT([10.6]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
      ISYSROOT="-isysroot /Developer/SDKs/MacOSX10.6.sdk"
    dnl Mac OS X 10.7: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x11"; then
      AC_MSG_RESULT([10.7]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
      ISYSROOT="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk"
    dnl Mac OS X 10.8: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x12"; then
      AC_MSG_RESULT([10.8]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.9: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x13"; then
      AC_MSG_RESULT([10.9]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.10: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x14"; then
      AC_MSG_RESULT([10.10]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.11: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x15"; then
      AC_MSG_RESULT([10.11]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.12: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x16"; then
      AC_MSG_RESULT([10.12]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    else
      AC_MSG_RESULT([Unknown... defaulting to i386 and x86_64]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    fi
    LDFLAGS="$LDFLAGS $ARCH_FLAGS"
    export LDFLAGS
    CFLAGS="$CFLAGS $ARCH_FLAGS  $ISYSROOT"
    CXXFLAGS="$CXXFLAGS $ARCH_FLAGS  $ISYSROOT"
    export CFLAGS
    export CXXFLAGS
    AC_MSG_CHECKING([Universal build arch flags...])
    AC_MSG_RESULT(["$ARCH_FLAGS"])
    AC_MSG_CHECKING([CFLAGS])
    AC_MSG_RESULT(["$CFLAGS"])
    AC_MSG_CHECKING([CXXFLAGS])
    AC_MSG_RESULT(["$CXXFLAGS"])
    AC_MSG_CHECKING([LDFLAGS])
    AC_MSG_RESULT(["$LDFLAGS"])
  else
    dnl Not 2.8, 2.9, or 3.0. So probably 3.1
    AC_MSG_CHECKING([Current Mac OS X build version...])
    dnl Mac OS X 10.7: XCode can't do PPC binaries
    if test "x$DARWINMAJORVERSION" = "x11"; then
      AC_MSG_RESULT([10.7]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
      ISYSROOT="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk"
    dnl Mac OS X 10.8: XCode can't do PPC binaries
    dnl Mac OS X 10.8: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x12"; then
      AC_MSG_RESULT([10.8]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.9: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x13"; then
      AC_MSG_RESULT([10.9]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.10: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x14"; then
      AC_MSG_RESULT([10.10]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.11: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x15"; then
      AC_MSG_RESULT([10.11]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    dnl Mac OS X 10.12: XCode can't do PPC binaries
    elif test "x$DARWINMAJORVERSION" = "x16"; then
      AC_MSG_RESULT([10.12]) 
      ARCH_FLAGS="-arch i386 -arch x86_64"
    else
      AC_MSG_RESULT([Unknown... defaulting to i386 and x86_64])
      ARCH_FLAGS="-arch i386 -arch x86_64"
    fi
    LDFLAGS="$LDFLAGS $ARCH_FLAGS"
    export LDFLAGS
    CFLAGS="$CFLAGS $ARCH_FLAGS  $ISYSROOT"
    CXXFLAGS="$CXXFLAGS $ARCH_FLAGS  $ISYSROOT"
    export CFLAGS
    export CXXFLAGS
    AC_MSG_CHECKING([Universal build arch flags...])
    AC_MSG_RESULT(["$ARCH_FLAGS"])
    AC_MSG_CHECKING([CFLAGS])
    AC_MSG_RESULT(["$CFLAGS"])
    AC_MSG_CHECKING([CXXFLAGS])
    AC_MSG_RESULT(["$CXXFLAGS"])
    AC_MSG_CHECKING([LDFLAGS])
    AC_MSG_RESULT(["$LDFLAGS"])
  fi
fi

AC_SUBST(WITH_WXWIDGETS)


])
