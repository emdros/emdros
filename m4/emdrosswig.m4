AC_DEFUN([RUBY_RBCONFIG],
[`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG[['$1']] || '')"`])


AC_DEFUN([EMDROS_SWIG_SUPPORT], [
DO_AT_LEAST_ONE_SWIG=no

dnl
dnl Check for swig version 2.0.12 or higher
dnl
AC_CHECK_PROGS(SWIG_PROGRAM, [swig3.0 swig], [no])
if test "x$SWIG_PROGRAM" == "xno"; then 
   CAN_DO_SWIG="no"
else
   CAN_DO_SWIG="yes"
fi
echo "CAN_DO_SWIG = ${CAN_DO_SWIG}"
echo "SWIG_PROGRAM = ${SWIG_PROGRAM}"
if test "x$SWIG_PROGRAM" != "xno"; then 
   AC_MSG_CHECKING([that version of swig is >= 2.0.12])
   SWIG_VERSION_FILENAME=/tmp/emdros_swig_version-`date +'%Y-%m-%d'`.txt
   ${SWIG_PROGRAM} -version > ${SWIG_VERSION_FILENAME} 2>&1
   SWIG_AWK_SCRIPT_FILENAME=/tmp/emdros_swig_version-`date +'%Y-%m-%d'`.awk
   FIELD_NUMBER=3;
   cat > ${SWIG_AWK_SCRIPT_FILENAME} <<EOF
NR == 1 {
  version = \$${FIELD_NUMBER};
  split(version, version_array, ".");
  MAJOR=version_array[[1]];
  MINOR=version_array[[2]];
  MINI=version_array[[3]];

  ver_major_minor_string = MAJOR "." MINOR;
  ver_major_minor = 1.0 * ver_major_minor_string;
   mini = 1 * MINI;
  if (ver_major_minor > 3.0) {
    print "yes php7"; 
  } else if (ver_major_minor == 3.0) {
    if (mini >= 11) { 
      print "yes php7";
    } else {
      print "yes nophp7";
    }
  } else if (ver_major_minor == 2.0) {
    if (mini >= 12) {
      print "yes nophp7";
    } else {
      print "no nophp7";
    }
  } else {
    print "no nophp7";
  }
}
EOF
   SWIG_VERSION_STRING=`${SWIG_PROGRAM} -version | grep "SWIG Version"`
   SWIGVERSION_OK_PHP7=`echo ${SWIG_VERSION_STRING} | awk -f ${SWIG_AWK_SCRIPT_FILENAME}`
   SWIGVERSIONOK=`echo ${SWIGVERSION_OK_PHP7} | awk '{a=1; print $a;}'`
   SWIGVERSION_PHP7_OK=`echo ${SWIGVERSION_OK_PHP7} | awk '{a=2; print $a;}'`

   if test "x$SWIGVERSIONOK" != "xyes"; then 
     AC_MSG_WARN([swig version 2.0.12 or higher must be present for SWIG to be invoked]);
     CAN_DO_SWIG="no";
   else 
     AC_MSG_RESULT(yes); 
   fi
else
   AC_MSG_CHECKING([swig program...])
   AC_MSG_RESULT([not found.]); 
fi


dnl Check if SWIG program can do PHP7
AC_MSG_CHECKING([whether swig program can do PHP7...])
if test "x$SWIGVERSION_PHP7_OK" = "xphp7"; then
   CAN_SWIG_DO_PHP7="yes";
else
   CAN_SWIG_DO_PHP7="no";
fi
AC_MSG_RESULT(${CAN_SWIG_DO_PHP7});
AC_SUBST(CAN_SWIG_DO_PHP7)


dnl SWIG support for Node.JS

AC_ARG_WITH(swig-language-nodejs,
[  --with-swig-language-nodejs  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_NODEJS=yes ;;
       no)  DO_SWIG_NODEJS=no ;;
       maybe) DO_SWIG_NODEJS=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-nodejs) ;;
     esac],
[DO_SWIG_NODEJS=maybe],
)
ORIGINAL_DO_SWIG_NODEJS=$DO_SWIG_NODEJS

dnl If we can't do SWIG; don't do SWIG Ruby bindings
if test x$CAN_DO_SWIG == xno; then
   if test -f SWIG/nodejs/emdrosnode_wrap.cxx; then
     DO_SWIG_NODEJS=yes;
   else
     DO_SWIG_NODEJS=no;
   fi
fi

AC_MSG_CHECKING([Whether we are to do Node.JS...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_NODEJS = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, Node.JS)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.
])
      DO_SWIG_NODEJS=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.
      ])
      DO_SWIG_NODEJS=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...
   ])
fi 


if test x$DO_SWIG_NODEJS != xno; then
  AC_MSG_CHECKING([Whether we can find node-gyp ...])
  dnl nodejs program
  AC_CHECK_PROGS(NODE_GYP_PROGRAM, [node-gyp], no)
  if test x$NODE_GYP_PROGRAM = xno; then
    if test x$DO_SWIG_NODEJS = xyes; then
      AC_MSG_RESULT([Could not find node-gyp program in path. Not doing Node.JS bindings.])
      DO_SWIG_NODEJS=no
    else
      # --with-swig-language-nodejs=maybe
      AC_MSG_RESULT([Could not find node-gyp program in path. Not doing Node.JS bindings.])
      DO_SWIG_NODEJS=no
    fi
  else
      AC_MSG_RESULT([yes.])
    DO_SWIG_NODEJS=yes
  fi
fi


AC_SUBST(NODE_GYP_PROGRAM)

AC_MSG_CHECKING([Whether to do SWIG Node.JS binding...])
AC_MSG_RESULT($DO_SWIG_NODEJS)

if test x$DO_SWIG_NODEJS = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi


dnl SWIG support for ruby

AC_ARG_WITH(swig-language-ruby,
[  --with-swig-language-ruby  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_RUBY=yes ;;
       no)  DO_SWIG_RUBY=no ;;
       maybe) DO_SWIG_RUBY=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-ruby) ;;
     esac],
[DO_SWIG_RUBY=maybe],
)
ORIGINAL_DO_SWIG_RUBY=$DO_SWIG_RUBY

dnl If we can't do SWIG; don't do SWIG Ruby bindings
if test x$CAN_DO_SWIG == xno; then
   if test -f SWIG/ruby/rbemdros_wrap.cxx; then
     DO_SWIG_RUBY=yes;
   else
     DO_SWIG_RUBY=no;
   fi
fi

AC_MSG_CHECKING([Whether we are to do ruby...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_RUBY = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, Ruby)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_RUBY=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_RUBY=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi 


if test x$DO_SWIG_RUBY != xno; then
  dnl ruby program
  AC_CHECK_PROGS(RUBY, [ruby ruby2.1 ruby2.0 ruby1.9 ruby1.8 ruby1.7 ruby1.6], no)
  if test x$RUBY = xno; then
    if test x$DO_SWIG_RUBY = xyes; then
      AC_MSG_RESULT([
  Could not find ruby interpreter in path. Not doing Ruby backends.])
    else
      # --with-swig-language-ruby=maybe
      DO_SWIG_RUBY=no
    fi
  fi
fi


RUBY_INCLUDES=""
RUBY_INCLUDE_DIR=""
if test x$DO_SWIG_RUBY != xno; then  
  export RUBY
  dnl Ruby include-dir
  for d in RUBY_RBCONFIG(rubyhdrdir) `$RUBY -e '$:.join("\n")'`; do
    if test -f $d/ruby.h; then
      RUBY_INCLUDE_DIR=$d;
      break;
    fi
  done

  dnl Ruby arch header include-dir
  for d in RUBY_RBCONFIG(rubyarchhdrdir) `$RUBY -e '$:.join("\n")'`; do
    if test -f $d/ruby/config.h; then
      RUBY_ARCH_INCLUDE_DIR=$d;
      break;
    fi
  done


  dnl Set RUBY_INCLUDES or report error.
  AC_MSG_CHECKING([for ruby include directory])
  if test -f $RUBY_INCLUDE_DIR/ruby.h; then
    AC_MSG_RESULT($RUBY_INCLUDE_DIR);
    RUBY_SO_DIR=RUBY_RBCONFIG(archdir)
    RUBY_CFLAGS=RUBY_RBCONFIG(CFLAGS)
    RUBY_CCDLFLAGS=RUBY_RBCONFIG(CCDLFLAGS)
    archdir=RUBY_RBCONFIG(archdir)
    arch=RUBY_RBCONFIG(arch)
    RUBY_INCLUDES="-I$RUBY_INCLUDE_DIR -I$RUBY_INCLUDE_DIR/${arch} -I$RUBY_ARCH_INCLUDE_DIR $RUBY_CFLAGS $RUBY_CCDLFLAGS"
    AC_SUBST(RUBY_SO_DIR)
    DO_SWIG_RUBY=yes
  else
    if test x$DO_SWIG_RUBY = xyes; then
      AC_MSG_RESULT([
  Could not find 
  the ruby.h include-file.
  Please set the RUBY_INCLUDE_DIR environment variable and run ./configure
  again.  For example, if you are running Linux, ruby.h could be in
  RUBY_INCLUDE_DIR=/usr/lib/ruby/1.8/i386-linux.])
      DO_SWIG_RUBY=no
    else
      DO_SWIG_RUBY=no
    fi
  fi
fi
AC_SUBST(RUBY_INCLUDES)

AC_MSG_CHECKING([Whether to do SWIG Ruby frontend])
AC_MSG_RESULT($DO_SWIG_RUBY)

if test x$DO_SWIG_RUBY = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi


dnl SWIG support for perl

AC_ARG_WITH(swig-language-perl,
[  --with-swig-language-perl  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_PERL=yes ;;
       no)  DO_SWIG_PERL=no ;;
       maybe) DO_SWIG_PERL=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-perl) ;;
     esac],
[DO_SWIG_PERL=maybe],
)
ORIGINAL_DO_SWIG_PERL=$DO_SWIG_PERL

dnl If we can't do SWIG, don't do SWIG Perl bindings.
if test x$CAN_DO_SWIG == xno; then
   if test -f SWIG/perl/plemdros_wrap.cxx -a -f SWIG/perl/EmdrosPerl.pm; then
     DO_SWIG_PERL=yes;
   else
     DO_SWIG_PERL=no;
   fi
fi


AC_MSG_CHECKING([Whether we are to do perl...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_PERL = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, Perl)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_PERL=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_PERL=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi

dnl We cannot do Perl on the Mac when we are doing so with
dnl a C++11 compiler, since the Perl headers cause a compiler
dnl error on such a compiler.
if test x$DO_SWIG_PERL != xno; then
   if test x$HOSTISDARWIN = xyes; then
      if test "$HAVE_CXX11" = "1"; then
      	 DO_SWIG_PERL=no
	 AC_MSG_CHECKING([Whether we are to do perl...])
      	 AC_MSG_RESULT([no, since we using C++11 on the Mac.])
      fi
   fi
fi


if test x$DO_SWIG_PERL != xno; then
  dnl perl program
  AC_CHECK_PROG(PERL, perl, perl, no)
  AC_SUBST(PERL)
  if test x$PERL = xno; then
    if test x$DO_SWIG_PERL = xyes; then
      AC_MSG_RESULT([
  Could not find perl interpreter in path. Not doing Perl bindings.])
      DO_SWIG_PERL=no
    else
      # --with-swig-language-perl=maybe
      DO_SWIG_PERL=no
    fi
  fi    
fi

PERL_INCLUDES=""
if test x$DO_SWIG_PERL != xno; then
  dnl Perl include-dir
  if test x$PERL_INCLUDE_DIR = x; then
    PERL_INCLUDE_DIR=`perl -e 'use Config; print $Config{archlib};'`
  fi

  dnl Set PERL_INCLUDES or report error.
  AC_MSG_CHECKING([for perl include directory])
  perldir_found="false";
  perldir_try_dirs="$PERL_INCLUDE_DIR $PERL_INCLUDE_DIR/CORE";
  for d in $perldir_try_dirs; do
    if test -f $d/perl.h; then
      perldir_dir=$d;
      perldir_found="true";
      break;
    fi;
  done
  if test x$perldir_found = xtrue; then
    AC_MSG_RESULT($perldir_dir);
    DO_SWIG_PERL=yes
  else
    if test x$DO_SWIG_PERL = xyes; then
      AC_MSG_WARN([
  Warning: Could not find 
  the perl.h include-file.
  Please set the PERL_INCLUDE_DIR environment variable and run ./configure
  again.  For example, if you are running Linux, perl.h could be in
  PERL_INCLUDE_DIR=/usr/lib/perl5/5.8.0/i386-linux-thread-multi/CORE.
  Remember to "export" the variable if using a bourne shell.]) 
      DO_SWIG_PERL=no
    else
      AC_MSG_RESULT([Not found! Not doing SWIG Perl frontend.])
      DO_SWIG_PERL=no
    fi
  fi
  PERL_INCLUDES="$PERL_INCLUDES -I$perldir_dir"
fi
AC_SUBST(PERL_INCLUDES)

AC_MSG_CHECKING([Whether to do SWIG Perl frontend])
AC_MSG_RESULT($DO_SWIG_PERL)

if test x$DO_SWIG_PERL = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi


dnl support for Python

AC_ARG_WITH(swig-language-python,
[  --with-swig-language-python  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_PYTHON=yes ;;
       no)  DO_SWIG_PYTHON=no ;;
       maybe) DO_SWIG_PYTHON=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-python) ;;
     esac],
[DO_SWIG_PYTHON=maybe],
)
ORIGINAL_DO_SWIG_PYTHON=$DO_SWIG_PYTHON

dnl If we can't do SWIG, don't do SWIG Python bindings.
if test x$CAN_DO_SWIG == xno; then
   if test -f SWIG/python/pyemdros_wrap.cxx -a -f SWIG/python/EmdrosPy.py ; then
     DO_SWIG_PYTHON=yes;
   else
     DO_SWIG_PYTHON=no;
   fi
fi


AC_MSG_CHECKING([Whether we are to do python...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_PYTHON = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, Python)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_PYTHON=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_PYTHON=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi





if test x$DO_SWIG_PYTHON != xno; then
  dnl python program
  AM_PATH_PYTHON
  AC_CHECK_PROGS(PYTHON, [python2.4 python24 python2.3 python23 python], no)
  if test x$PYTHON = xno; then
    if test x$DO_SWIG_PYTHON = xyes; then
      AC_MSG_RESULT([
  Could not find python interpreter in path. Not doing Python bindings.]);
      DO_SWIG_PYTHON=no;
    else
      DO_SWIG_PYTHON=no
    fi
  fi    
fi

PYTHON_INCLUDES=""
if test x$DO_SWIG_PYTHON != xno; then
  dnl Python include-dir
  cat >/tmp/emdros_python_include_dir.py <<EOF
from distutils import sysconfig
print sysconfig.get_python_inc()
EOF
  PYTHON_INCLUDE_DIR_TMP=`$PYTHON /tmp/emdros_python_include_dir.py`
  if test -f $PYTHON_INCLUDE_DIR_TMP/Python.h; then
    PYTHON_INCLUDE_DIR=$PYTHON_INCLUDE_DIR_TMP;
  fi
  rm -f /tmp/emdros_python_include_dir.py

  dnl Set PYTHON_INCLUDES or report error.
  AC_MSG_CHECKING([for python include directory])
  if test -f $PYTHON_INCLUDE_DIR/Python.h; then
    AC_MSG_RESULT($PYTHON_INCLUDE_DIR);
    PYTHON_INCLUDES="$PYTHON_INCLUDES -I$PYTHON_INCLUDE_DIR"
  else
    if test x$DO_SWIG_PYTHON = xyes; then
      AC_MSG_WARN([
  Could not find the python.h include-file.
  Please set the PYTHON_INCLUDE_DIR environment variable and run ./configure
  again.  For example, if you are running Linux, python.h could be in
  PYTHON_INCLUDE_DIR=/usr/local/include/python2.2])
      DO_SWIG_PYTHON=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Python backend.])
      DO_SWIG_PYTHON=no
    fi
  fi
fi
AC_SUBST(PYTHON_INCLUDES)

if test x$DO_SWIG_PYTHON != xno; then
  dnl Python site-packages directory
  cat >/tmp/emdros_python_site_packages_dir.py <<EOF
import sys
print sys.prefix + "/lib/python" + sys.version[[:3]] + "/site-packages"
EOF
  PYTHON_SITE_PACKAGES_DIR_TMP=`$PYTHON /tmp/emdros_python_site_packages_dir.py`
  if test -d $PYTHON_SITE_PACKAGES_DIR_TMP; then
    PYTHON_SITE_PACKAGES_DIR=$PYTHON_SITE_PACKAGES_DIR_TMP;
  fi
  rm -f /tmp/emdros_python_site_packages_dir.py

  dnl Report and AC_SUBST or report error.
  AC_MSG_CHECKING([for python site-packages directory])
  if test -d $PYTHON_SITE_PACKAGES_DIR; then
    AC_MSG_RESULT($PYTHON_SITE_PACKAGES_DIR);
    AC_SUBST(PYTHON_SITE_PACKAGES_DIR)
    DO_SWIG_PYTHON=yes
  else
    if test x$DO_SWIG_PYTHON = xyes; then
      AC_MSG_WARN([
  Could not find the Python site-packages directory.
  Please set the PYTHON_SITE_PACKAGES_DIR environment variable 
  and rerun ./configure.  For example, if you are running Linux, 
  the Python site-packages could be in
  PYTHON_SITE_PACKAGES_DIR=/usr/local/lib/python2.2/site-packages])
      DO_SWIG_PYTHON=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Python frontend.])
      DO_SWIG_PYTHON=no
    fi
  fi
fi

AC_MSG_CHECKING([Whether to do SWIG Python frontend])
AC_MSG_RESULT($DO_SWIG_PYTHON)


if test x$DO_SWIG_PYTHON = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi


dnl support for Python 3

AC_ARG_WITH(swig-language-python3,
[  --with-swig-language-python3  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_PYTHON3=yes ;;
       no)  DO_SWIG_PYTHON3=no ;;
       maybe) DO_SWIG_PYTHON3=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-python3) ;;
     esac],
[DO_SWIG_PYTHON3=maybe],
)
ORIGINAL_DO_SWIG_PYTHON3=$DO_SWIG_PYTHON3

dnl If we can't do SWIG, don't do SWIG Python3 bindings.
if test x$CAN_DO_SWIG == xno; then
   if test -f SWIG/python3/py3emdros_wrap.cxx -a -f SWIG/python3/EmdrosPy3.py; then
     DO_SWIG_PYTHON3=yes;
   else
     DO_SWIG_PYTHON3=no;
   fi
fi


AC_MSG_CHECKING([Whether we are to do python3...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_PYTHON3 = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, Python3)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_PYTHON3=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_PYTHON3=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi





if test x$DO_SWIG_PYTHON3 != xno; then
  dnl python3 program
  AC_CHECK_PROGS(PYTHON3, [python3.7 python3.6 python3.5 python3.4 python3.3 python3.2 python3.1 python3 python], no)
  if test x$PYTHON3 = xno; then
    if test x$DO_SWIG_PYTHON3 = xyes; then
      AC_MSG_RESULT([
  Could not find python3 interpreter in path. Not doing Python3 bindings.]);
      DO_SWIG_PYTHON3=no;
    else
      DO_SWIG_PYTHON3=no
    fi
  fi    
fi

PYTHON3_INCLUDES=""
if test x$DO_SWIG_PYTHON3 != xno; then
  dnl Python3 include-dir
  cat >/tmp/emdros_python3_include_dir.py <<EOF
from distutils import sysconfig
print(sysconfig.get_python_inc())
EOF
  PYTHON3_INCLUDE_DIR_TMP=`$PYTHON3 /tmp/emdros_python3_include_dir.py`
  if test -f $PYTHON3_INCLUDE_DIR_TMP/Python.h; then
    PYTHON3_INCLUDE_DIR=$PYTHON3_INCLUDE_DIR_TMP;
  fi
  rm -f /tmp/emdros_python3_include_dir.py

  dnl Set PYTHON3_INCLUDES or report error.
  AC_MSG_CHECKING([for python3 include directory])
  if test -f $PYTHON3_INCLUDE_DIR/Python.h; then
    AC_MSG_RESULT($PYTHON3_INCLUDE_DIR);
    PYTHON3_INCLUDES="$PYTHON3_INCLUDES -I$PYTHON3_INCLUDE_DIR"
  else
    if test x$DO_SWIG_PYTHON3 = xyes; then
      AC_MSG_WARN([
  Could not find the Python.h include-file.
  Please set the PYTHON3_INCLUDE_DIR environment variable and run ./configure
  again.  For example, if you are running Linux, Python.h could be in
  PYTHON3_INCLUDE_DIR=/usr/local/include/python3.5])
      DO_SWIG_PYTHON3=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Python3 backend.])
      DO_SWIG_PYTHON3=no
    fi
  fi
fi
AC_SUBST(PYTHON3_INCLUDES)

if test x$DO_SWIG_PYTHON3 != xno; then
  dnl Python3 site-packages directory
  cat >/tmp/emdros_python3_site_packages_dir.py <<EOF
import sys
print(sys.prefix + "/lib/python" + sys.version[[:3]] + "/site-packages")
EOF
  PYTHON3_SITE_PACKAGES_DIR_TMP=`$PYTHON3 /tmp/emdros_python3_site_packages_dir.py`
  if test -d $PYTHON3_SITE_PACKAGES_DIR_TMP; then
    PYTHON3_SITE_PACKAGES_DIR=$PYTHON3_SITE_PACKAGES_DIR_TMP;
  fi
  rm -f /tmp/emdros_python3_site_packages_dir.py

  dnl Report and AC_SUBST or report error.
  AC_MSG_CHECKING([for python3 site-packages directory])
  if test -d $PYTHON3_SITE_PACKAGES_DIR; then
    AC_MSG_RESULT($PYTHON3_SITE_PACKAGES_DIR);
    AC_SUBST(PYTHON3_SITE_PACKAGES_DIR)
    DO_SWIG_PYTHON3=yes
  else
    if test x$DO_SWIG_PYTHON3 = xyes; then
      AC_MSG_WARN([
  Could not find the Python3 site-packages directory.
  Please set the PYTHON3_SITE_PACKAGES_DIR environment variable 
  and rerun ./configure.  For example, if you are running Linux, 
  the Python3 site-packages could be in
  PYTHON3_SITE_PACKAGES_DIR=/usr/local/lib/python3.5/site-packages])
      DO_SWIG_PYTHON3=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Python3 frontend.])
      DO_SWIG_PYTHON3=no
    fi
  fi
fi

AC_MSG_CHECKING([Whether to do SWIG Python3 frontend])
AC_MSG_RESULT($DO_SWIG_PYTHON3)


if test x$DO_SWIG_PYTHON3 = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi


dnl SWIG support for Java

AC_ARG_WITH(swig-language-java,
[  --with-swig-language-java  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_JAVA=yes ;;
       no)  DO_SWIG_JAVA=no ;;
       maybe) DO_SWIG_JAVA=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-java) ;;
     esac],
[DO_SWIG_JAVA=maybe],
)
ORIGINAL_DO_SWIG_JAVA=$DO_SWIG_JAVA

dnl If we can't do SWIG, don't do SWIG Java bindings.
if test x$CAN_DO_SWIG == xno; then
   if test -f SWIG/java/libjemdros_wrap.cxx -a -f SWIG/java/jemdros.jar; then
     DO_SWIG_JAVA=yes;
   else
     DO_SWIG_JAVA=no;
   fi
fi



AC_MSG_CHECKING([Whether we are to do java...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_JAVA = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, Java)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_JAVA=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_JAVA=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi




AC_ARG_WITH(jdk-dir,
[  --with-jdk-dir=dir         Directory of JDK],
  jdk_dir=$withval,
jdk_dir=/usr/java)


dnl Java include dir
if test x$DO_SWIG_JAVA != xno; then
  dnl check JDK dir
  AC_MSG_CHECKING([For JDK directory])
  if test -d $jdk_dir/bin; then
     AC_MSG_RESULT($jdk_dir);
  else
    jdk_found="false";
    jdk_try_dirs="/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK /usr/local/java /usr/java /usr/lib/jvm/java-8-openjdk-amd64 /usr/lib/jvm/java-1.8.0 /usr/lib/jvm/java-1.8.0-openjdk /usr/lib/jvm/java-1.8.0-openjdk-1.8.0.0 /usr/lib/jvm/java-7-openjdk-amd64 /usr/lib/jvm/java-1.7.0 /usr/lib/jvm/java-1.7.0-openjdk /usr/lib/jvm/java-1.7.0-openjdk-1.7.0.0 /usr/lib/jvm/java-6-openjdk /usr/lib/jvm/java-6-sun /usr/lib/jvm/java-1.7.0-sun /usr/lib/jvm/java-1.6.0 /usr/lib/jvm/java-1.6.0-openjdk /usr/lib/jvm/java-6-openjdk /usr/lib/jvm/java-1.6.0-openjdk-1.6.0.0 /usr/lib/jvm/java-6-sun /usr/lib/jvm/java-1.6.0-sun /usr/lib/jvm/java-1.5.0-sun-1.5.0.08 /usr/lib/jvm/java-1.5.0-sun-1.5.0.07 /usr/lib/jvm/java-1.5.0-sun-1.5.0.06 /usr/lib/jvm/java-1.5.0-sun-1.5.0.05 /usr/lib/jvm/java-1.5.0-sun-1.5.0.04 /usr/lib/jvm/java-1.5.0-sun /usr/java/jdk1.5.0_03 /usr/lib/j2sdk1.4-sun /usr/local/lib/j2sdk1.4.2_05 /opt/j2sdk1.4.2_04 /opt/j2sdk_nb/j2sdk1.4.2 /usr/java/j2sdk1.4.2 /opt/blackdown-jdk-1.4.2_rc1 /usr/java/j2sdk1.4.1_03 /usr/java/j2sdk1.4.1_02 /usr/java/j2sdk1.4.1_01";
    for d in $jdk_try_dirs; do
      if test -d $d/bin -o -d $d/Commands; then
        jdk_dir=$d;
        jdk_found="true";
        break;
      fi;
    done
    if test x$jdk_found = xtrue; then
      AC_MSG_RESULT($jdk_dir);
    else
      if test x$DO_SWIG_JAVA = xyes; then
        AC_MSG_WARN([
  Could not  find JDK.  
  Please use --with-jdk-dir=dir option to specify the correct directory.  E.g., 
  --with-jdk-dir=/usr/java/j2sdk1.4.1_01]);
        DO_SWIG_JAVA=no
      else
        AC_MSG_RESULT([Not found. Not doing SWIG Java frontend.])
        DO_SWIG_JAVA=no
      fi
    fi
  fi
fi

if test x$DO_SWIG_JAVA != xno; then    
  AC_MSG_CHECKING([for javac in JDK directory])
  if test -x $jdk_dir/bin/javac; then
    JAVAC=$jdk_dir/bin/javac;
    AC_MSG_RESULT($JAVAC);
  elif test -x $jdk_dir/Commands/javac; then
    JAVAC=$jdk_dir/Commands/javac;
    AC_MSG_RESULT($JAVAC);
  else
    AC_MSG_RESULT([not found... checking in path])
    AC_CHECK_PROG(JAVAC, javac, javac, no)
    if test x$JAVAC = xno; then
      if test x$DO_SWIG_JAVA = xyes; then
        AC_MSG_WARN([
  Could not find javac compiler either in path or in $jdk_dir/bin.
  You can try setting the JDK directory with --with-jdk-dir=dir,
  e.g. --with-jdk-dir=/usr/java/j2sdk1.4.1_01])
        DO_SWIG_JAVA=no;
      else
        AC_MSG_RESULT([Not found. Not doing SWIG Java frontend.])
        DO_SWIG_JAVA=no;
      fi
    fi
  fi
fi

if test x$DO_SWIG_JAVA != xno; then
  AC_MSG_CHECKING([for java in JDK directory])
  if test -x $jdk_dir/bin/java; then
    JAVA=$jdk_dir/bin/java;
    AC_MSG_RESULT($JAVA);
  elif test -x $jdk_dir/Commands/java; then
    JAVA=$jdk_dir/Commands/java;
    AC_MSG_RESULT($JAVA);
  else 
    AC_MSG_RESULT([not found... checking in path])
    AC_CHECK_PROG(JAVA, java, java, no)
    if test x$JAVA = xno; then
      if test x$DO_SWIG_JAVA = xyes; then
        AC_MSG_WARN([
  Could not find java interpreter either in path or in $jdk_dir/bin.
  You can try setting the JDK directory with --with-jdk-dir=dir,
  e.g. --with-jdk-dir=/usr/java/j2sdk1.4.1_01])
        DO_SWIG_JAVA=no
      else
        AC_MSG_RESULT([Not found. Not doing SWIG Java frontend.])
        DO_SWIG_JAVA=no
      fi
    fi
  fi
fi

AC_SUBST(JAVA)
if test x$DO_SWIG_JAVA != xno; then
  AC_MSG_CHECKING([for jar in JDK directory])
  if test -x $jdk_dir/bin/jar; then
    JAR=$jdk_dir/bin/jar;
    AC_MSG_RESULT($JAR);
  elif test -x $jdk_dir/Commands/jar; then
    JAR=$jdk_dir/Commands/jar;
    AC_MSG_RESULT($JAR);
  else
    AC_MSG_RESULT([not found... checking in path])
    AC_CHECK_PROG(JAR, jar, jar, no)
    if test x$JAR = xno; then
      if test x$DO_SWIG_JAVA = xyes; then
        AC_MSG_WARN([
  Could not find jar archiver either in path or in $jdk_dir/bin.
  You can try setting the JDK directory with --with-jdk-dir=dir,
  e.g. --with-jdk-dir=/usr/java/j2sdk1.4.1_01])
        DO_SWIG_JAVA=no
      else
        AC_MSG_RESULT([Not found. Not doing SWIG Java frontend.])
        DO_SWIG_JAVA=no
      fi
    fi
  fi
fi

if test x$DO_SWIG_JAVA != xno; then  
  dnl JDK lib directory
  AC_MSG_CHECKING([for JDK lib directory])
  if test -d "$jdk_dir/lib"; then
    JDK_LIB=$jdk_dir/lib
    AC_MSG_RESULT($JDK_LIB); 
    AC_SUBST(JDK_LIB)
  elif test -d "$jdk_dir/Libraries"; then
    JDK_LIB=$jdk_dir/Libraries
    AC_MSG_RESULT($JDK_LIB); 
    AC_SUBST(JDK_LIB)
  else 
    if test x$DO_SWIG_JAVA = xyes; then
      AC_MSG_WARN([
  Could not  find JDK.  
  Please use --with-jdk-dir=dir option to specify the correct directory.  E.g., 
  --with-jdk-dir=/usr/java/j2sdk1.4.1_01
  NOTE: Do not provide the full path to the lib directory.
  Only specify the top-level JDK directory.])
      DO_SWIG_JAVA=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Java frontend.])
      DO_SWIG_JAVA=no
    fi
  fi
fi

if test x$DO_SWIG_JAVA != xno; then
  dnl JDK include directory
  AC_MSG_CHECKING([for JDK include directory])
  if test -d "$jdk_dir/include"; then
    JDK_INCLUDE=$jdk_dir/include
    AC_MSG_RESULT($JDK_INCLUDE); 
  elif test -d "$jdk_dir/Headers"; then
    JDK_INCLUDE=$jdk_dir/Headers
    AC_MSG_RESULT($JDK_INCLUDE); 
  else 
    if test x$DO_SWIG_JAVA = xyes; then
      AC_MSG_WARN([
  Could not  find JDK.  
  Please use --with-jdk-dir=dir option to specify the correct directory.  E.g., 
  --with-jdk-dir=/usr/java/j2sdk1.4.1_01
  NOTE: Do not provide the full path to the include directory.
  Only specify the top-level JDK directory.])
      DO_SWIG_JAVA=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Java frontend.])
      DO_SWIG_JAVA=no
    fi
  fi
fi

if test x$DO_SWIG_JAVA != xno; then
  dnl arch-dependent Java include-dir, but only on non-Darwin!
  if test "x$HOSTISDARWIN" != "xyes"; then
    AC_MSG_CHECKING([for JDK arch-dependent include directory])
    if test x$JDK_INCLUDE_ARCH != x && -d $JDK_INCLUDE_ARCH; then
      AC_MSG_RESULT($JDK_INCLUDE_ARCH); 
    else
      if test x$HOSTISSUN = xyes; then
        JDK_INCLUDE_ARCH=$JDK_INCLUDE/solaris
      else
        if test x$HOSTISLINUX = xyes; then
          JDK_INCLUDE_ARCH=$JDK_INCLUDE/linux
        fi
      fi
      if test -d $JDK_INCLUDE_ARCH; then
        AC_MSG_RESULT($JDK_INCLUDE_ARCH); 
      else
        if test x$DO_SWIG_JAVA = xyes; then
          AC_MSG_WARN([
    Could not determine arch-dependent JDK include directory. 
    For example, if you are running Linux, it should be 
    $JDK_INCLUDE/linux
    Please set the environment variable JDK_INCLUDE_ARCH to the right
    directory and rerun ./configure.]);
          DO_SWIG_JAVA=no
        else 
          AC_MSG_RESULT([Not found. Not doing SWIG Java frontend.])
          DO_SWIG_JAVA=no
        fi
      fi
    fi
  fi
fi

if test x$DO_SWIG_JAVA = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi


dnl
dnl We need jar to build dist (see SWIG/java/Makefile.am)
dnl but not for normal building.
dnl Hence, if not set above, we just choose one that fits 
dnl the author's system, since he is the one who normally 
dnl makes dist.
if test "x$JAR" = "x"; then
  JAR=/usr/bin/jar
else
  JAR=$JAR
fi
AC_SUBST(JAR)

dnl
dnl We need javac to build dist (see SWIG/java/Makefile.am)
dnl but not for normal building, except when doing
dnl --with-swig-language-java.
dnl Hence, if not set above, we just choose one that fits 
dnl the author's system, since he is the one who normally 
dnl makes dist.
if test "x$JAVAC" = "x"; then
  JAVAC=/usr/bin/javac
else
  JAVAC=$JAVAC
fi
AC_SUBST(JAVAC)

# 
# If we came this far, and DO_SWIG_JAVA is not no, then
# we can safely assume that it's 'yes'.
#
if test x$DO_SWIG_JAVA = xmaybe; then
  DO_SWIG_JAVA=yes
fi


#
# Set Java JDK_INCLUDES
#
JDK_INCLUDES=""
if test x$DO_SWIG_JAVA = xyes; then
    if test x$JDK_INCLUDE_ARCH != x; then
      JDK_INCLUDES="$JDK_INCLUDES -I$JDK_INCLUDE_ARCH"
    fi
    JDK_INCLUDES="$JDK_INCLUDES -I$JDK_INCLUDE"
fi
AC_SUBST(JDK_INCLUDES)

AC_MSG_CHECKING([Whether to do SWIG Java frontend])
AC_MSG_RESULT($DO_SWIG_JAVA)



dnl SWIG support for C#

dnl C# compiler
AC_PATH_PROG(MCS, mcs, no)
AC_PATH_PROG(GMCS, gmcs, no)
AC_PATH_PROG(CSC, csc, no)
AC_PATH_PROG(MONO, mono, no)

AC_ARG_WITH(swig-language-csharp,
[  --with-swig-language-csharp  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_CSHARP=yes ;;
       no)  DO_SWIG_CSHARP=no ;;
       maybe) DO_SWIG_CSHARP=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-csharp) ;;
     esac],
[DO_SWIG_CSHARP=maybe],
)
ORIGINAL_DO_SWIG_CSHARP=$DO_SWIG_CSHARP

dnl If we can't do SWIG, don't do SWIG C-Sharp bindings.
if test x$CAN_DO_SWIG == xno; then
   if test -f SWIG/csharp/libcsemdros_wrap.cxx -a -f SWIG/csharp/CSharpSources.zip; then
     DO_SWIG_CSHARP=yes;
   else
     DO_SWIG_CSHARP=no;
   fi
fi



AC_MSG_CHECKING([Whether we are to do csharp...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_CSHARP = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, csharp)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_CSHARP=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_CSHARP=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi

if test x$DO_SWIG_CSHARP != xno; then
  CS="C#"
  if test "x$MCS" = "xno" -a "x$CSC" = "xno" -a "x$GMCS" = "xno" ; then
          if test x$DO_SWIG_CSHARP == xyes; then
              AC_MSG_WARN([No $CS compiler found. You need to install a C-sharp compiler if you
want to configure --with-swig-language-csharp])

             DO_SWIG_CSHARP=no
          else
             DO_SWIG_CSHARP=no
             AC_MSG_RESULT([no $CS compiler found, so no.])
          fi
  fi

  if test "x$DO_SWIG_CSHARP" != "xno" -a "x$MONO" = "xno"; then
          if test x$DO_SWIG_CSHARP == xyes; then
             AC_MSG_WARN([No mono runtime found])
             DO_SWIG_CSHARP=no
          else
             DO_SWIG_CSHARP=no
             AC_MSG_RESULT([no mono runtime found, so no.])
          fi
  fi

  if test "x$DO_SWIG_CSHARP" != "xno"; then
     DO_SWIG_CSHARP=yes
  fi
fi

# Let's set MONOCOMPILER regardless of whether
# we are to DO_SWIG_CSHARP or not.
if test "x$MCS" != "xno" ; then
     dnl choose MCS by default
     MONOCOMPILER=$MCS;
else
     if test "x$GMCS" != "xno"; then
         MONOCOMPILER=$GMCS
     else
         if test "x$CSC" != "xno"; then
             MONOCOMPILER=$CSC
         else
             MONOCOMPILER="no";
         fi
     fi
fi



AC_SUBST(MONOCOMPILER)

if test x$DO_SWIG_CSHARP = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi





dnl SWIG support for php

AC_ARG_WITH(swig-language-php,
[  --with-swig-language-php  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_PHP=yes ;;
       no)  DO_SWIG_PHP=no ;;
       maybe) DO_SWIG_PHP=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-php) ;;
     esac],
[DO_SWIG_PHP=maybe],
)
ORIGINAL_DO_SWIG_PHP=$DO_SWIG_PHP

dnl If we can't do SWIG, don't do SWIG PHP bindings.
if test x$CAN_DO_SWIG == xno; then
   AC_MSG_CHECKING([if we have PHP5 SWIG bindings already...])
   if test -f SWIG/php/phpemdros_wrap.cpp -a -f SWIG/php/EmdrosPHP.php; then
     DO_SWIG_PHP=yes;
     AC_MSG_RESULT([yes, so we don't need swig(1).]);
   else
     DO_SWIG_PHP=no;
     AC_MSG_RESULT([no, so we do need swig(1), which we don't have. Not doing SWIG PHP5 bindings.]);
   fi
fi



AC_MSG_CHECKING([Whether we are to do php...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_PHP = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, PHP)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_PHP=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_PHP=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi






if test x$DO_SWIG_PHP != xno; then
  dnl php program

  AC_CHECK_PROGS(PHP_CONFIG, [php-config5.6 php-config5.5 php-config5.4 php-config], [no])

  AC_SUBST(PHP_CONFIG)
  if test x$PHP_CONFIG = xno; then
    if test x$DO_SWIG_PHP = xyes; then
      AC_MSG_WARN([
  Could not find php-config program in path. Not doing SWIG PHP bindings.])
      DO_SWIG_PHP=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG PHP frontend.])
      DO_SWIG_PHP=no
    fi
  fi    
fi

dnl Test for PHP version 5.x.  The current SWIG can't do
dnl PHP <= 4.x or PHP >= 7.0
if test x$DO_SWIG_PHP != xno; then
  AC_MSG_CHECKING([PHP vernum... ])

  PHP_VERNUM=`$PHP_CONFIG --vernum`
  AC_MSG_RESULT($PHP_VERNUM)

  AC_MSG_CHECKING([PHP 5 availability... ])
  PHP5_AVAILABLE=`echo $PHP_VERNUM | awk '{a=1; VERNUM=$a+0; print VERNUM >= 50000 && VERNUM < 60000;}'`

  if test "x$PHP5_AVAILABLE" = "x0"; then
     AC_MSG_WARN(["no, PHP5 not available. Can't do SWIG PHP bindings."])
     DO_SWIG_PHP=no
  else
     AC_MSG_RESULT([yes])
  fi
fi

if test x$DO_SWIG_PHP != xno; then
  # 
  # If we came this far, then 'maybe' should be 'yes', unconditionally.
  #
  DO_SWIG_PHP=yes

  dnl PHP include-dir
  PHP_INCLUDES=`$PHP_CONFIG --includes`
  echo "PHP_INCLUDES = $PHP_INCLUDES"
  AC_SUBST(PHP_INCLUDES)

  dnl PHP extension-dir
  PHP_EXTENSION_DIR=`$PHP_CONFIG --extension-dir`
  echo "PHP_EXTENSION_DIR = $PHP_EXTENSION_DIR"
  AC_SUBST(PHP_EXTENSION_DIR)

  dnl PHP libs
  PHP_LIBS=`$PHP_CONFIG --libs`
  echo "PHP_LIBS = $PHP_LIBS"
  AC_SUBST(PHP_LIBS)

  dnl PHP extension-dir
  PHP_LDFLAGS=`$PHP_CONFIG --ldflags`
  echo "PHP_LDFLAGS = $PHP_LDFLAGS"
  AC_SUBST(PHP_LDFLAGS)
fi

AC_MSG_CHECKING([Whether to do SWIG PHP frontend])
AC_MSG_RESULT($DO_SWIG_PHP)

if test x$DO_SWIG_PHP = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi





dnl SWIG support for php7

AC_ARG_WITH(swig-language-php7,
[  --with-swig-language-php7  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_PHP7=yes ;;
       no)  DO_SWIG_PHP7=no ;;
       maybe) DO_SWIG_PHP7=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-php7) ;;
     esac],
[DO_SWIG_PHP7=maybe],
)
ORIGINAL_DO_SWIG_PHP7=$DO_SWIG_PHP7

dnl If SWIG can't do PHP7, don't do them, unless we have the sources already.
if test x$CAN_SWIG_DO_PHP7 == xno; then
   AC_MSG_CHECKING([if we have PHP7 SWIG bindings already...])
   if test -f SWIG/php7/php7emdros_wrap.cxx -a -f SWIG/php7/EmdrosPHP7.php; then
     DO_SWIG_PHP7=yes;
     AC_MSG_RESULT([yes, so we don't need swig(1).]);
   else
     DO_SWIG_PHP7=no;
     AC_MSG_RESULT([no, so we do need swig(1), which we don't have. Not doing SWIG PHP7 bindings.]);
   fi
fi



AC_MSG_CHECKING([Whether we are to do php7...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_PHP7 = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, PHP7)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_PHP7=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_PHP7=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi






if test x$DO_SWIG_PHP7 != xno; then
  dnl php7 program
  AC_CHECK_PROGS(PHP7_CONFIG, [php-config7.2 php-config7.1 php-config7.0 php-config], no)
  AC_SUBST(PHP7_CONFIG)
  if test x$PHP7_CONFIG = xno; then
    if test x$DO_SWIG_PHP7 = xyes; then
      AC_MSG_WARN([
  Could not find php7-config program in path. Not doing SWIG PHP7 bindings.])
      DO_SWIG_PHP7=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG PHP7 frontend.])
      DO_SWIG_PHP7=no
    fi
  fi    
fi

dnl Test for PHP version 7.x.  
if test x$DO_SWIG_PHP7 != xno; then
  AC_MSG_CHECKING([PHP7 vernum... ])

  PHP7_VERNUM=`$PHP7_CONFIG --vernum`
  AC_MSG_RESULT($PHP7_VERNUM)

  AC_MSG_CHECKING([PHP 7 availability... ])
  PHP7_AVAILABLE=`echo $PHP7_VERNUM | awk '{a=1; VERNUM=$a+0; print VERNUM >= 70000 && VERNUM < 80000;}'`

  if test "x$PHP7_AVAILABLE" = "x0"; then
     AC_MSG_WARN(["no, PHP7 not available. Can't do SWIG PHP7 bindings."])
     DO_SWIG_PHP7=no
  else
     AC_MSG_RESULT([yes])
  fi
fi

if test x$DO_SWIG_PHP7 != xno; then
  # 
  # If we came this far, then 'maybe' should be 'yes', unconditionally.
  #
  DO_SWIG_PHP7=yes

  dnl PHP7 include-dir
  PHP7_INCLUDES=`$PHP7_CONFIG --includes`
  echo "PHP7_INCLUDES = $PHP7_INCLUDES"
  AC_SUBST(PHP7_INCLUDES)

  dnl PHP7 extension-dir
  PHP7_EXTENSION_DIR=`$PHP7_CONFIG --extension-dir`
  echo "PHP7_EXTENSION_DIR = $PHP7_EXTENSION_DIR"
  AC_SUBST(PHP7_EXTENSION_DIR)

  dnl PHP7 libs
  PHP7_LIBS=`$PHP7_CONFIG --libs`
  echo "PHP7_LIBS = $PHP7_LIBS"
  AC_SUBST(PHP7_LIBS)

  dnl PHP7 extension-dir
  PHP7_LDFLAGS=`$PHP7_CONFIG --ldflags`
  echo "PHP7_LDFLAGS = $PHP7_LDFLAGS"
  AC_SUBST(PHP7_LDFLAGS)
fi

AC_MSG_CHECKING([Whether to do SWIG PHP7 frontend])
AC_MSG_RESULT($DO_SWIG_PHP7)

if test x$DO_SWIG_PHP7 = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi








WITH_SWIG_CSHARP="--with-swig-language-csharp=$ORIGINAL_DO_SWIG_CSHARP"
WITH_SWIG_JAVA="--with-swig-language-java=$ORIGINAL_DO_SWIG_JAVA"
WITH_SWIG_PERL="--with-swig-language-perl=$ORIGINAL_DO_SWIG_PERL"
WITH_SWIG_PHP="--with-swig-language-php=$ORIGINAL_DO_SWIG_PHP"
WITH_SWIG_PHP7="--with-swig-language-php7=$ORIGINAL_DO_SWIG_PHP7"
WITH_SWIG_PYTHON="--with-swig-language-python=$ORIGINAL_DO_SWIG_PYTHON"
WITH_SWIG_PYTHON3="--with-swig-language-python3=$ORIGINAL_DO_SWIG_PYTHON3"
WITH_SWIG_RUBY="--with-swig-language-ruby=$ORIGINAL_DO_SWIG_RUBY"

AC_SUBST(SWIG_PROGRAM)
AC_SUBST(WITH_SWIG_CSHARP)
AC_SUBST(WITH_SWIG_JAVA)
AC_SUBST(WITH_SWIG_PERL)
AC_SUBST(WITH_SWIG_PHP)
AC_SUBST(WITH_SWIG_PHP7)
AC_SUBST(WITH_SWIG_PYTHON)
AC_SUBST(WITH_SWIG_PYTHON3)
AC_SUBST(WITH_SWIG_RUBY)

])
