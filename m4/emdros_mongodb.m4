AC_DEFUN([EMDROS_MONGODB], [

dnl
dnl The MongoDB backend is proprietary, and not available in the Open
dnl Source version of Emdros.
dnl
dnl For more information, please contact the author of Emdros,
dnl Ulrik Sandborg-Petersen, at ulrikp -at- scripturesys -dot- com
dnl
AC_ARG_WITH(mongodb,
[  --with-mongodb           Use MongoDB as a backend (default: no)],
[case "${withval}" in
       yes) DO_MONGODB=yes ;;
       maybe) DO_MONGODB=maybe ;;
       no)  DO_MONGODB=no ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-mongodb) ;;
     esac],
[DO_MONGODB=maybe])
ORIGINAL_DO_MONGODB=$DO_MONGODB

dnl
dnl If DO_MONGODB is not "no", test whether we should, in fact, build it.
dnl
if test x$DO_MONGODB != xno; then
  MONGODBNOTREAL=`grep "This is not the real thing" EMdF/mongoemdfdb.cpp`;
  if test "x$MONGODBNOTREAL" == "x"; then
    DO_MONGODB=yes;
  else
    DO_MONGODB=no;
  fi
fi


AS_IF([test "x$DO_MONGODB" = "xyes"], [
  dnl Add --keep-system-libs to the invocation of pkg-config.
  dnl This is because, otherwise we don't get system -L paths
  dnl for the --libs part, which is required for libbson.
  PKG_CONFIG="$PKG_CONFIG --keep-system-libs "
  PKG_CHECK_MODULES([LIBMONGOC], [libmongoc],
  [AC_DEFINE([HAVE_LIBMONGOC], [1], [Use libmongoc])  libmongoc=1; LIBMONGOC="libmongoc"],
    [PKG_CHECK_MODULES([LIBMONGOC], [libmongoc-1.0],
       [AC_DEFINE([HAVE_LIBMONGOC], [1], [Use libmongoc-1.0])  libmongoc=1; LIBMONGOC="libmongoc-1.0"],
       [AC_DEFINE([HAVE_LIBMONGOC], [0], [libmongoc not present]  libmongoc=0)
    ])
  ])
])

if test "x$DO_MONGODB" = "x1"; then
   AC_MSG_CHECKING([that HAVE_LIBMONGOC is 1])

   AS_IF([test "x$libmongoc" = "x1"],
     [AC_MSG_RESULT([yes])],
     [AC_MSG_ERROR([Error: Could not find libmongoc with pkg-config.
libmongoc is required for the --with-mongodb option
(and the MongoDB Emdros backend) to work.])]
   )
fi


AS_IF([test "x$DO_MONGODB" = "xyes"], [
  PKG_CHECK_MODULES([LIBBSON], [libbson],
  [AC_DEFINE([HAVE_LIBBSON], [1], [Use libbson])  libbson=1; LIBBSON="libbson"],
    [PKG_CHECK_MODULES([LIBBSON], [libbson-1.0],
       [AC_DEFINE([HAVE_LIBBSON], [1], [Use libbson-1.0])  libbson=1; LIBBSON="libbson-1.0"],
       [AC_DEFINE([HAVE_LIBBSON], [0], [libbson not present]  libbson=0)
    ])
  ])
])

if test "x$DO_MONGODB" = "x1"; then
   AC_MSG_CHECKING([that HAVE_LIBBSON is 1])

   AS_IF([test "x$libbson" = "x1"],
     [AC_MSG_RESULT([yes])],
     [AC_MSG_ERROR([Error: Could not find libbson with pkg-config.
libbson is required for the --with-mongodb option
(and the MongoDB Emdros backend) to work.])]
   )
fi

USE_MONGODB=0

if test "x$DO_MONGODB" != "xno"; then
  USE_MONGODB=1
else
  USE_MONGODB=0
fi

AM_CONDITIONAL(MONGODB, test x$DO_MONGODB != xno)

if test "x$DO_MONOGDB" != "xno"; then
  BACKENDLDADD="$BACKENDLDADD $LIBMONGOC_LIBS $LIBBSON_LIBS";
  BACKENDLDADDSWIG="$BACKENDLDADDSWIG $LIBMONGOC_LIBS $LIBBSON_LIBS";
  dnl BACKEND_LIBDIRS_AMALGAMATION="$BACKEND_LIBDIRS_AMALGAMATION $PG_LIBDIRS"
  BACKEND_LIBS_AMALGAMATION="$BACKEND_LIBS_AMALGAMATION $LIBMONGOC_LIBS $LIBBSON"
  BACKEND_LDADD_AMALGAMATION="$BACKEND_LDADD_AMALGAMATION $LIBMONGOC_LIBS $LIBBSON_LIBS"
fi


if test "xDO_MONGODB" != "xno"; then
  ADDITIONAL_CXXFLAGS="$ADDITIONAL_CXXFLAGS $LIBMONGOC_CFLAGS $LIBBSON_CFLAGS"
  MONGODB_DASH_INCLUDE="$LIBMONGOC_CFLAGS $LIBBSON_CFLAGS"
  PKGCONFIG_REQUIRED_AMALGAMATION="$PKGCONFIG_REQUIRED_AMALGAMATION $LIBMONGOC $LIBBSON"
fi

WITH_MONGODB=--with-mongodb=$ORIGINAL_DO_MONGODB

AC_SUBST(USE_MONGODB)

AC_SUBST(WITH_MONGODB)

AC_SUBST(MONGODB_DASH_INCLUDE)


])
