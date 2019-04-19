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
[  --enable-universal_binary     turns macOS Universal Binaries on
  --disable-universal_binary    turns macOS Universal Binaries off],
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

if test "x$HOSTISDARWIN" = "xyes"; then
  AC_MSG_CHECKING([Current macOS build version (for setting macOS minimum version)...])

  ISYSROOT=""
  dnl macOS 10.4:
  if test "x$DARWINMAJORVERSION" = "x8"; then
    AC_MSG_RESULT([10.4])
    MACOS_VERSION_MIN="10.4"
    # SQLite3 must have the -DSQLITE_WITHOUT_ZONEMALLOC when compiling with macOS minimum version < 10.5.
    MACOS_CFLAGS="-DSQLITE_WITHOUT_ZONEMALLOC"
    ISYSROOT="-isysroot /Developer/SDKs/MacOSX10.4u.sdk"
  dnl macOS 10.5: XCode can't compile 64-bit code
  elif test "x$DARWINMAJORVERSION" = "x9"; then
    AC_MSG_RESULT([10.5])
    MACOS_VERSION_MIN="10.4"
    # SQLite3 must have the -DSQLITE_WITHOUT_ZONEMALLOC when compiling with macOS minimum version < 10.5.
    MACOS_CFLAGS="-DSQLITE_WITHOUT_ZONEMALLOC"
    ISYSROOT="-isysroot /Developer/SDKs/MacOSX10.4u.sdk"
  dnl macOS 10.6
  elif test "x$DARWINMAJORVERSION" = "x10"; then
    AC_MSG_RESULT([10.6]) 
    MACOS_VERSION_MIN="10.4"
    # SQLite3 must have the -DSQLITE_WITHOUT_ZONEMALLOC when compiling with macOS minimum version < 10.5.
    MACOS_CFLAGS="-DSQLITE_WITHOUT_ZONEMALLOC"
    ISYSROOT="-isysroot /Developer/SDKs/MacOSX10.4u.sdk"
  dnl macOS 10.7: XCode can't do PPC binaries
  elif test "x$DARWINMAJORVERSION" = "x11"; then
    AC_MSG_RESULT([10.7]) 
    MACOS_VERSION_MIN="10.6"
    MACOS_CFLAGS=""
    ISYSROOT="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk"
  dnl macOS 10.8: XCode can't do PPC binaries
  elif test "x$DARWINMAJORVERSION" = "x12"; then
    AC_MSG_RESULT([10.8]) 
    MACOS_VERSION_MIN="10.7"
    MACOS_CFLAGS=""
  dnl macOS 10.8: XCode can't do PPC binaries
  elif test "x$DARWINMAJORVERSION" = "x13"; then
    AC_MSG_RESULT([10.9]) 
    MACOS_VERSION_MIN="10.8"
    MACOS_CFLAGS=""
  dnl macOS 10.10.
  elif test "x$DARWINMAJORVERSION" = "x14"; then
    AC_MSG_RESULT([10.10]) 
    MACOS_VERSION_MIN="10.9"
    MACOS_CFLAGS=""
  dnl macOS 10.11.
  elif test "x$DARWINMAJORVERSION" = "x15"; then
    AC_MSG_RESULT([10.11]) 
    MACOS_VERSION_MIN="10.10"
    MACOS_CFLAGS=""
  dnl macOS 10.12.
  elif test "x$DARWINMAJORVERSION" = "x16"; then
    AC_MSG_RESULT([10.12]) 
    MACOS_VERSION_MIN="10.11"
    MACOS_CFLAGS=""
  dnl macOS 10.13.
  elif test "x$DARWINMAJORVERSION" = "x17"; then
    AC_MSG_RESULT([10.13]) 
    MACOS_VERSION_MIN="10.12"
    MACOS_CFLAGS=""
  dnl macOS 10.14.
  elif test "x$DARWINMAJORVERSION" = "x18"; then
    AC_MSG_RESULT([10.14]) 
    MACOS_VERSION_MIN="10.13"
    MACOS_CFLAGS=""
  dnl macOS 10.15.
  elif test "x$DARWINMAJORVERSION" = "x19"; then
    AC_MSG_RESULT([10.15]) 
    MACOS_VERSION_MIN="10.14"
    MACOS_CFLAGS=""
  else
    AC_MSG_RESULT([unknown... Using 10.16.]) 
    MACOS_VERSION_MIN="10.15"
    MACOS_CFLAGS=""
  fi

  MACOS_CFLAGS=" -mmacosx-version-min=$MACOS_VERSION_MIN  $MACOS_CFLAGS $ISYSROOT"
  MACOS_CXXFLAGS=" -mmacosx-version-min=$MACOS_VERSION_MIN $ISYSROOT"

  CFLAGS="$CFLAGS $MACOS_CFLAGS"
  CXXFLAGS="$CXXFLAGS $MACOS_CXXFLAGS"
  export CFLAGS
  export CXXFLAGS
  

  AC_MSG_CHECKING([macOS ISYSROOT for macOS minimum build version ${MACOS_VERSION_MIN} ...])
  AC_MSG_RESULT([${ISYSROOT}])

  AC_MSG_CHECKING([macOS minimum version...])
  AC_MSG_RESULT([${MACOS_VERSION_MIN}]) 

  AC_MSG_CHECKING([macOS required CFLAGS...])
  AC_MSG_RESULT(["${MACOS_CFLAGS}"])

  AC_MSG_CHECKING([macOS required CXXFLAGS...])
  AC_MSG_RESULT(["${MACOS_CXXFLAGS}"])
fi


dnl UNIVERSAL_BINARY
if test "x$UNIVERSAL_BINARY" = "xyes"; then
  if test x$HOSTISDARWIN != xyes; then
    AC_MSG_ERROR([Error: Cannot make Universal Binaries except on macOS. 
This is not macOS. Aborting.])
  fi

  AC_MSG_CHECKING([dependency tracking status...])

  if test "x$enable_dependency_tracking" = "x"; then
    enable_dependency_tracking="no";
    AC_MSG_RESULT(["empty. Setting to 'no', as required for --enable-universal_binary"])
  else
    AC_MSG_RESULT(["already set to '$enable_dependency_tracking'"])
  fi
  
  if test "x$enable_dependency_tracking" != "xno"; then
    AC_MSG_ERROR([

Error: In order to make Universal Binaries, you must also 
give configure the
   --disable-dependency-tracking
switch.

Please do so, and try again.])
  fi

  dnl Default to nothing
  MACOS_ARCH_FLAGS=""

  AC_MSG_CHECKING([macOS minimum build version...])

  dnl macOS 10.4
  if test "x$MACOS_VERSION_MIN" = "x10.4"; then
    AC_MSG_RESULT([10.4])
    MACOS_ARCH_FLAGS="-arch i386 -arch ppc"
  dnl macOS 10.5
  elif test "x$MACOS_VERSION_MIN" = "x10.5"; then
    AC_MSG_RESULT([10.5])
    MACOS_ARCH_FLAGS="-arch i386 -arch ppc"
  dnl macOS 10.6
  elif test "x$MACOS_VERSION_MIN" = "x10.6"; then
    AC_MSG_RESULT([10.6]) 
    MACOS_ARCH_FLAGS="-arch i386 -arch ppc"
  dnl macOS 10.7: macOS 10.7 doesn't support PPC binaries (?),
  dnl but does support x86_64 binaries.
  elif test "x$MACOS_VERSION_MIN" = "x10.7"; then
    AC_MSG_RESULT([10.7]) 
    MACOS_ARCH_FLAGS="-arch i386 -arch x86_64"
  dnl macOS 10.8: 
  elif test "x$MACOS_VERSION_MIN" = "x10.8"; then
    AC_MSG_RESULT([10.8]) 
    MACOS_ARCH_FLAGS="-arch i386 -arch x86_64"
  dnl macOS 10.9: 
  elif test "x$MACOS_VERSION_MIN" = "x10.9"; then
    AC_MSG_RESULT([10.9]) 
    MACOS_ARCH_FLAGS="-arch i386 -arch x86_64"
  dnl macOS 10.10: 
  elif test "x$MACOS_VERSION_MIN" = "x10.10"; then
    AC_MSG_RESULT([10.10]) 
    MACOS_ARCH_FLAGS="-arch i386 -arch x86_64"
  dnl macOS 10.11: 
  elif test "x$MACOS_VERSION_MIN" = "x10.11"; then
    AC_MSG_RESULT([10.11]) 
    MACOS_ARCH_FLAGS="-arch i386 -arch x86_64"
  dnl macOS 10.12: XCode can't do PPC binaries
  elif test "x$MACOS_VERSION_MIN" = "x10.12"; then
    AC_MSG_RESULT([10.12]) 
    MACOS_ARCH_FLAGS="-arch i386 -arch x86_64"
  dnl macOS 10.13:
  dnl
  dnl The minimum version is 10.13. That means, we are
  dnl building on macOS 10.14, which uses
  dnl Xcode 10.2.  This version of Xcode does not
  dnl support building i386 binaries.
  dnl
  dnl macOS 10.13 was the last macOS version to support
  dnl 32 bit binaries.
  dnl
  dnl Hence, if you need to build i386 binaries
  dnl for 10.13, you will need to build on macOS 10.13
  dnl or earlier.
  elif test "x$MACOS_VERSION_MIN" = "x10.13"; then
    AC_MSG_RESULT([10.13]) 
    MACOS_ARCH_FLAGS="-arch x86_64"
  dnl macOS 10.14: macOS 10.14 doesn't support i386
  elif test "x$MACOS_VERSION_MIN" = "x10.14"; then
    AC_MSG_RESULT([10.14]) 
    MACOS_ARCH_FLAGS="-arch x86_64"
  dnl macOS 10.15: macOS 10.15 doesn't support i386
  elif test "x$MACOS_VERSION_MIN" = "x10.15"; then
    AC_MSG_RESULT([10.15]) 
    MACOS_ARCH_FLAGS="-arch x86_64"
  dnl macOS 10.15: macOS 10.16 doesn't support i386
  else
    AC_MSG_RESULT([Unknown... defaulting to x86_64])
    MACOS_ARCH_FLAGS="-arch x86_64"
  fi

  AC_MSG_CHECKING([macOS MACOS_ARCH_FLAGS for macOS minimum build version ${MACOS_VERSION_MIN} ...])
  AC_MSG_RESULT([${MACOS_ARCH_FLAGS}])

  CFLAGS="$CFLAGS $MACOS_ARCH_FLAGS"
  CXXFLAGS="$CXXFLAGS $MACOS_ARCH_FLAGS"
  LDFLAGS="$LDFLAGS $MACOS_ARCH_FLAGS"
  export CFLAGS
  export CXXFLAGS
  export LDFLAGS

  AC_MSG_CHECKING([CFLAGS])
  AC_MSG_RESULT(["$CFLAGS"])
  AC_MSG_CHECKING([CXXFLAGS])
  AC_MSG_RESULT(["$CXXFLAGS"])
  AC_MSG_CHECKING([LDFLAGS])
  AC_MSG_RESULT(["$LDFLAGS"])
fi

dnl libstdc++ is deprecated on macOS. Use libc++ instead
dnl CXX needs to be modified, not CXXFLAGS or LDFLAGS.
if test "x$HOSTISDARWIN" = xyes; then
   if test "$HAVE_CXX11" = "1"; then
      eval "CXX=\"$CXX --stdlib=libc++\""
   fi
fi



])
