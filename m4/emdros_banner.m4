AC_DEFUN([EMDROS_BANNER], [

dnl Banner Message
echo "********************************************"
echo "Using C compiler:     $CC"
echo "Using C++ compiler:   $CXX"
echo "Doing shared libs:    $enable_shared"
echo ""
echo "Compiling with the following backend(s) and libraries:"
if test "x$DO_POSTGRESQL" = "xno"; then
  echo "PostgreSQL:           no"
else
  echo "PostgreSQL:           yes"
fi
if test "x$DO_MYSQL" = "xno"; then
  echo "MySQL:                no"
else
  echo "MySQL:                yes"
fi

echo "SQLite 3.X.X:         $DO_SQLITE3"
dnl echo "MongoDB:              $DO_MONGODB"
echo "BPT:                  $DO_BPT"
echo ""

if test "x$USE_CONFIGURE_DEFAULT_BACKEND_ENUM" != "x0"; then
echo "Default backend:      $DEFAULT_BACKEND"
else
echo "Default backend:      Chosen by include/emdros-lconfig.h"
fi

echo ""

if test "x$DO_WXWIDGETS" = "xno"; then
  echo "wxWidgets:            no"
else
  echo "wxWidgets:            yes: version = $WX_VERSION"
fi
echo ""
if test "x$USE_AMALGAMATION" = "xyes"; then
  echo "Use amalgamation lib: yes"
else
  echo "Use amalgamation lib: no"
fi
if test "x$USE_LOCAL_PCRE" = "xyes"; then
  echo "PCRE library:         local"
else
  echo "PCRE library:         system"
fi
if test "x$USE_LOCAL_ZLIB" = "xyes"; then
  echo "ZLIB library:         local"
else
  echo "ZLIB library:         system"
fi

echo ""


if test "x$DO_SWIG_CSHARP" = "xyes"; then
  echo "SWIG Backend:         csharp"
fi
if test "x$DO_SWIG_JAVA" = "xyes"; then
  echo "SWIG Backend:         java"
fi
if test "x$DO_SWIG_PHP5" = "xyes"; then
  echo "SWIG Backend:         php5"
fi
if test "x$DO_SWIG_PHP7" = "xyes"; then
  echo "SWIG Backend:         php7"
fi
if test "x$DO_SWIG_PYTHON2" = "xyes"; then
  echo "SWIG Backend:         python2"
fi
if test "x$DO_SWIG_PYTHON3" = "xyes"; then
  echo "SWIG Backend:         python3"
fi

echo ""

if test "x$DEVEL" = "xyes"; then
  echo "This is a devel build, so the individual source files will be compiled."
else
  echo "This is not a devel build, so the amalgamation will be used,"
  echo "and the source code will not be compiled separately."
  echo "NOTE: This is a Good Thing(TM), unless you want to develop Emdros :-)."
fi

echo ""

])
