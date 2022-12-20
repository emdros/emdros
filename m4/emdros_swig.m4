
AC_DEFUN([EMDROS_SWIG_SUPPORT], [
DO_AT_LEAST_ONE_SWIG=no

dnl
dnl Check swig version. Version 2.0.12 or higher is necessary for use with
dnl Emdros.
dnl
dnl SWIG 3.0 is required for PHP7 support.
dnl SWIG 4.1 is required for PHP8 support
dnl
AC_CHECK_PROGS(SWIG_PROGRAM, [swig swig4.1 swig4.0 swig3.0], [no])
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
  if (ver_major_minor >= 4.1) {
    print "swigok php7 php8"; 
  } else if (ver_major_minor > 3.0) {
    print "swigok php7 nophp8"; 
  } else if (ver_major_minor == 3.0) {
    if (mini >= 11) { 
      print "swigok php7 nophp8";
    } else {
      print "swigok nophp7 nophp8";
    }
  } else if (ver_major_minor == 2.0) {
    if (mini >= 12) {
      print "swigok nophp7 nophp8";
    } else {
      print "swignotok nophp7 nophp8";
    }
  } else {
    print "swignotok nophp7 nophp8";
  }
}
EOF
   SWIG_VERSION_STRING=`${SWIG_PROGRAM} -version | grep "SWIG Version"`
   SWIGVERSIONOK_PHP7_PHP8=`echo ${SWIG_VERSION_STRING} | awk -f ${SWIG_AWK_SCRIPT_FILENAME}`
   SWIGVERSIONOK=`echo ${SWIGVERSIONOK_PHP7_PHP8} | awk '{a=1; print $a;}'`
   SWIGVERSION_PHP7_OK=`echo ${SWIGVERSIONOK_PHP7_PHP8} | awk '{a=2; print $a;}'`
   SWIGVERSION_PHP8_OK=`echo ${SWIGVERSIONOK_PHP7_PHP8} | awk '{a=3; print $a;}'`

   if test "x$SWIGVERSIONOK" != "xswigok"; then 
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





dnl Check if SWIG program can do PHP8
AC_MSG_CHECKING([whether swig program can do PHP8...])
if test "x$SWIGVERSION_PHP8_OK" = "xphp8"; then
   CAN_SWIG_DO_PHP8="yes";
else
   CAN_SWIG_DO_PHP8="no";
fi
AC_MSG_RESULT(${CAN_SWIG_DO_PHP8});
AC_SUBST(CAN_SWIG_DO_PHP8)





dnl support for Python2

AC_ARG_WITH(swig-language-python2,
[  --with-swig-language-python2  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_PYTHON2=yes ;;
       no)  DO_SWIG_PYTHON2=no ;;
       maybe) DO_SWIG_PYTHON2=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-python2) ;;
     esac],
[DO_SWIG_PYTHON2=maybe],
)
ORIGINAL_DO_SWIG_PYTHON2=$DO_SWIG_PYTHON2

dnl If we can't do SWIG, don't do SWIG Python bindings.
if test x$CAN_DO_SWIG == xno; then
   if test -f SWIG/python/pyemdros_wrap.cxx -a -f SWIG/python/EmdrosPy.py; then
     if test "x$DO_SWIG_PYTHON2" != "xno"; then
       DO_SWIG_PYTHON2=yes;
     fi
   else
     DO_SWIG_PYTHON2=no;
   fi
fi


AC_MSG_CHECKING([Whether we are to do python2...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_PYTHON2 = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, Python2)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_PYTHON2=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_PYTHON2=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi





if test x$DO_SWIG_PYTHON2 != xno; then
  dnl python (2) program
  AM_PATH_PYTHON
  AC_CHECK_PROGS(PYTHON, [python2.7 python27 python2.6 python26 python2.5 python25 python2.4 python24 python2.3 python23 python], no)
  if test x$PYTHON = xno; then
    if test x$DO_SWIG_PYTHON2 = xyes; then
      AC_MSG_RESULT([
  Could not find python2 interpreter in path. Not doing Python2 bindings.]);
      DO_SWIG_PYTHON2=no;
    else
      DO_SWIG_PYTHON2=no
    fi
  fi    
fi

dnl Check that it really is Python 2
if test x$DO_SWIG_PYTHON2 != xno; then
  AC_MSG_CHECKING([Python 2 version... ])

  dnl Python 2 emits the version on stderr.
  PYTHON2_VERSION=`$PYTHON --version 2>&1`
  AC_MSG_RESULT($PYTHON2_VERSION)

  AC_MSG_CHECKING([Python 2 availability... ])
  PYTHON2_AVAILABLE=`echo $PYTHON2_VERSION | awk '{a=2; print $a;}' | awk -F '.' '{a=1; VERNUM=$a+0; res=VERNUM == 2; print res;}'`

  if test "x$PYTHON2_AVAILABLE" = "x0"; then
     AC_MSG_WARN(["no, Python 2 not available. Can't do SWIG Python 2 bindings."])
     DO_SWIG_PYTHON2=no
  else
     AC_MSG_RESULT([yes])
  fi
fi

PYTHON2_INCLUDES=""
if test x$DO_SWIG_PYTHON2 != xno; then
  dnl Python include-dir
  EMDROS_PYTHON2_TMP="emdros_python_include_dir.py"
  cat > ${EMDROS_PYTHON2_TMP} <<EOF
from distutils import sysconfig
print sysconfig.get_python_inc()
EOF
  PYTHON2_INCLUDE_DIR_TMP=`$PYTHON ${EMDROS_PYTHON2_TMP}`
  if test -f $PYTHON2_INCLUDE_DIR_TMP/Python.h; then
    PYTHON2_INCLUDE_DIR=$PYTHON2_INCLUDE_DIR_TMP;
  fi
  rm -f ${EMDROS_PYTHON2_TMP}

  dnl Set PYTHON2_INCLUDES or report error.
  AC_MSG_CHECKING([for python include directory])
  if test -f $PYTHON2_INCLUDE_DIR/Python.h; then
    AC_MSG_RESULT($PYTHON2_INCLUDE_DIR);
    PYTHON2_INCLUDES="$PYTHON2_INCLUDES -I$PYTHON2_INCLUDE_DIR"
  else
    if test x$DO_SWIG_PYTHON2 = xyes; then
      AC_MSG_WARN([
  Could not find the python.h include-file.
  Please set the PYTHON2_INCLUDE_DIR environment variable and run ./configure
  again.  For example, if you are running Linux, python.h could be in
  PYTHON2_INCLUDE_DIR=/usr/local/include/python2.2])
      DO_SWIG_PYTHON2=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Python backend.])
      DO_SWIG_PYTHON2=no
    fi
  fi
fi
AC_SUBST(PYTHON2_INCLUDES)

if test x$DO_SWIG_PYTHON2 != xno; then
  dnl Python site-packages directory
  cat >${EMDROS_PYTHON2_TMP} <<EOF
import sys
print sys.prefix + "/lib/python" + sys.version[[:3]] + "/site-packages"
EOF
  PYTHON2_SITE_PACKAGES_DIR_TMP=`$PYTHON ${EMDROS_PYTHON2_TMP}`
  if test -d $PYTHON2_SITE_PACKAGES_DIR_TMP; then
    PYTHON2_SITE_PACKAGES_DIR=$PYTHON2_SITE_PACKAGES_DIR_TMP;
  fi
  rm -f ${EMDROS_PYTHON2_TMP}

  dnl Report and AC_SUBST or report error.
  AC_MSG_CHECKING([for python site-packages directory])
  if test -d $PYTHON2_SITE_PACKAGES_DIR; then
    AC_MSG_RESULT($PYTHON2_SITE_PACKAGES_DIR);
    AC_SUBST(PYTHON2_SITE_PACKAGES_DIR)
    DO_SWIG_PYTHON2=yes
  else
    if test x$DO_SWIG_PYTHON2 = xyes; then
      AC_MSG_WARN([
  Could not find the Python site-packages directory.
  Please set the PYTHON2_SITE_PACKAGES_DIR environment variable 
  and rerun ./configure.  For example, if you are running Linux, 
  the Python site-packages could be in
  PYTHON2_SITE_PACKAGES_DIR=/usr/local/lib/python2.2/site-packages])
      DO_SWIG_PYTHON2=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Python frontend.])
      DO_SWIG_PYTHON2=no
    fi
  fi
fi

AC_MSG_CHECKING([Whether to do SWIG Python2 frontend])
AC_MSG_RESULT($DO_SWIG_PYTHON2)


if test x$DO_SWIG_PYTHON2 = xyes; then
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
     if test "x$DO_SWIG_PYTHON3" != "xno"; then
        DO_SWIG_PYTHON3=yes;
     fi
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
  dnl
  dnl NOTE: We check for python3 first, because pyenv can, on some platforms,
  dnl cause this to fail, yet AC_CHECK_PROGS succeeds, resulting in an
  dnl unusable PYTHON3 program.
  AC_CHECK_PROGS(PYTHON3, [python3 python3.7 python3.6 python3.5 python3.4 python3.3 python3.2 python3.1 python], no)
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

dnl Check that it really is Python 3
if test x$DO_SWIG_PYTHON3 != xno; then
  AC_MSG_CHECKING([Python 3 version... ])

  PYTHON3_VERSION=`$PYTHON3 --version`
  AC_MSG_RESULT($PYTHON3_VERSION)

  AC_MSG_CHECKING([Python 3 availability... ])
  PYTHON3_AVAILABLE=`echo $PYTHON3_VERSION | awk '{a=2; print $a;}' | awk -F '.' '{a=1; VERNUM=$a+0; res= VERNUM == 3; print res;}'`

  if test "x$PYTHON3_AVAILABLE" = "x0"; then
     AC_MSG_WARN(["no, Python 3 not available. Can't do SWIG Python 3 bindings."])
     DO_SWIG_PYTHON3=no
  else
     AC_MSG_RESULT([yes])
  fi
fi

PYTHON3_INCLUDES=""
if test x$DO_SWIG_PYTHON3 != xno; then
  dnl Python3 include-dir
  EMDROS_PYTHON3_TMP="emdros_python_include_dir.py"
  cat >${EMDROS_PYTHON3_TMP} <<EOF
from distutils import sysconfig
print(sysconfig.get_python_inc())
EOF
  PYTHON3_INCLUDE_DIR_TMP=`$PYTHON3 ${EMDROS_PYTHON3_TMP}`
  if test -f $PYTHON3_INCLUDE_DIR_TMP/Python.h; then
    PYTHON3_INCLUDE_DIR=$PYTHON3_INCLUDE_DIR_TMP;
  fi
  rm -f ${EMDROS_PYTHON3_TMP}

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
  cat >${EMDROS_PYTHON3_TMP} <<EOF
import sys
print(sys.prefix + "/lib/python" + sys.version[[:3]] + "/site-packages")
EOF
  PYTHON3_SITE_PACKAGES_DIR_TMP=`$PYTHON3 ${EMDROS_PYTHON3_TMP}`
  if test -d $PYTHON3_SITE_PACKAGES_DIR_TMP; then
    PYTHON3_SITE_PACKAGES_DIR=$PYTHON3_SITE_PACKAGES_DIR_TMP;
  fi
  rm -f ${EMDROS_PYTHON3_TMP}

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
     if test "x$DO_SWIG_JAVA" != "xno"; then
        DO_SWIG_JAVA=yes;
     fi
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
     if test "x$DO_SWIG_CSHARP" != "xno"; then
        DO_SWIG_CSHARP=yes;
     fi
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
   if test -f SWIG/php7/php7emdros_wrap.cxx -a -f SWIG/php7/EmdrosPHP7.php; then
     if test "x$DO_SWIG_PHP7" != "xno"; then
        DO_SWIG_PHP7=yes;
     fi
   else
     DO_SWIG_PHP7=no;
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
  AC_CHECK_PROGS(PHP7_CONFIG, [php-config7.4 php-config7.3 php-config7.2 php-config7.1 php-config7.0 php-config], [no])
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

  dnl PHP7 ldflags
  PHP7_LDFLAGS=`$PHP7_CONFIG --ldflags`
  echo "PHP7_LDFLAGS = $PHP7_LDFLAGS"
  AC_SUBST(PHP7_LDFLAGS)

  dnl PHP7 ini-path
  PHP7_INI_PATH=`$PHP7_CONFIG --ini-path`
  echo "PHP7_INI_PATH = $PHP7_INI_PATH"
  AC_SUBST(PHP7_INI_PATH)

  dnl PHP7 ini-dir
  PHP7_INI_DIR=`$PHP7_CONFIG --ini-dir`
  echo "PHP7_INI_DIR = $PHP7_INI_DIR"
  AC_SUBST(PHP7_INI_DIR)

  dnl PHP7 VERNUM MAJOR / MINOR / MAJOR_DOT_MINOR
  PHP7_VERNUM_MAJOR=`echo $PHP7_VERNUM | cut -c 1`;
  PHP7_VERNUM_MINOR=`echo $PHP7_VERNUM | cut -c 2-3 | awk '{a=1; MINOR_VERNUM=$a+0; print MINOR_VERNUM;}'`
  PHP7_VERNUM_MAJOR_DOT_MINOR="${PHP7_VERNUM_MAJOR}.${PHP7_VERNUM_MINOR}"

  AC_SUBST(PHP7_VERNUM_MAJOR_DOT_MINOR)

fi

AC_MSG_CHECKING([Whether to do SWIG PHP7 frontend])
AC_MSG_RESULT($DO_SWIG_PHP7)

if test x$DO_SWIG_PHP7 = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi








dnl SWIG support for php8

AC_ARG_WITH(swig-language-php8,
[  --with-swig-language-php8  Use swig scripting language wrappers],
[case "${withval}" in
       yes) DO_SWIG_PHP8=yes ;;
       no)  DO_SWIG_PHP8=no ;;
       maybe) DO_SWIG_PHP8=maybe ;;
       *)   AC_MSG_ERROR(Bad value ${withval} for --with-swig-language-php8) ;;
     esac],
[DO_SWIG_PHP8=maybe],
)
ORIGINAL_DO_SWIG_PHP8=$DO_SWIG_PHP8

dnl If SWIG can't do PHP8, don't do them, unless we have the sources already.
if test x$CAN_SWIG_DO_PHP8 == xno; then
   AC_MSG_CHECKING([... well, the installed version of SWIG can't do PHP8. Let's see if we have the sources necessary available already...])
   if test -f SWIG/php8/php8emdros_wrap.cxx; then
     if test "x$DO_SWIG_PHP8" != "xno"; then
        DO_SWIG_PHP8=yes;
     fi
   else
     DO_SWIG_PHP8=no;
   fi
   AC_MSG_RESULT([${DO_SWIG_PHP8}])
fi



AC_MSG_CHECKING([Whether we are to do php8...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_PHP8 = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, PHP8)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_PHP8=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_PHP8=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi






if test x$DO_SWIG_PHP8 != xno; then
  dnl php8 php-config program
  AC_MSG_CHECKING([for PHP8 php-config program...])
  AC_CHECK_PROGS(PHP8_CONFIG, [php-config8.2 php-config8.1 php-config8.0 php-config8 php-config], [no])
  AC_SUBST(PHP8_CONFIG)
  if test x$PHP8_CONFIG = xno; then
    if test x$DO_SWIG_PHP8 = xyes; then
      AC_MSG_WARN([
  Could not find php8-config program in path. Not doing SWIG PHP8 bindings.])
      DO_SWIG_PHP8=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG PHP8 frontend.])
      DO_SWIG_PHP8=no
    fi
  fi    
fi

dnl Test for PHP version 8.x.  
if test x$DO_SWIG_PHP8 != xno; then
  AC_MSG_CHECKING([PHP8 vernum... ])

  PHP8_VERNUM=`$PHP8_CONFIG --vernum`
  AC_MSG_RESULT($PHP8_VERNUM)

  AC_MSG_CHECKING([PHP 8 availability... ])
  PHP8_AVAILABLE=`echo $PHP8_VERNUM | awk '{a=1; VERNUM=$a+0; print VERNUM >= 80000 && VERNUM < 90000;}'`

  if test "x$PHP8_AVAILABLE" = "x0"; then
     AC_MSG_WARN(["no, PHP8 not available. Can't do SWIG PHP8 bindings."])
     DO_SWIG_PHP8=no
  else
     AC_MSG_RESULT([yes])
  fi
fi

if test x$DO_SWIG_PHP8 != xno; then
  # 
  # If we came this far, then 'maybe' should be 'yes', unconditionally.
  #
  DO_SWIG_PHP8=yes

  dnl PHP8 include-dir
  PHP8_INCLUDES=`$PHP8_CONFIG --includes`
  echo "PHP8_INCLUDES = $PHP8_INCLUDES"
  AC_SUBST(PHP8_INCLUDES)

  dnl PHP8 extension-dir
  PHP8_EXTENSION_DIR=`$PHP8_CONFIG --extension-dir`
  echo "PHP8_EXTENSION_DIR = $PHP8_EXTENSION_DIR"
  AC_SUBST(PHP8_EXTENSION_DIR)

  dnl PHP8 libs
  PHP8_LIBS=`$PHP8_CONFIG --libs`
  echo "PHP8_LIBS = $PHP8_LIBS"
  AC_SUBST(PHP8_LIBS)

  dnl PHP8 ldflags
  PHP8_LDFLAGS=`$PHP8_CONFIG --ldflags`
  echo "PHP8_LDFLAGS = $PHP8_LDFLAGS"
  AC_SUBST(PHP8_LDFLAGS)

  dnl PHP8 ini-path
  PHP8_INI_PATH=`$PHP8_CONFIG --ini-path`
  echo "PHP8_INI_PATH = $PHP8_INI_PATH"
  AC_SUBST(PHP8_INI_PATH)

  dnl PHP8 ini-dir
  PHP8_INI_DIR=`$PHP8_CONFIG --ini-dir`
  echo "PHP8_INI_DIR = $PHP8_INI_DIR"
  AC_SUBST(PHP8_INI_DIR)

  dnl PHP8 VERNUM MAJOR / MINOR / MAJOR_DOT_MINOR
  PHP8_VERNUM_MAJOR=`echo $PHP8_VERNUM | cut -c 1`;
  PHP8_VERNUM_MINOR=`echo $PHP8_VERNUM | cut -c 2-3 | awk '{a=1; MINOR_VERNUM=$a+0; print MINOR_VERNUM;}'`
  PHP8_VERNUM_MAJOR_DOT_MINOR="${PHP8_VERNUM_MAJOR}.${PHP8_VERNUM_MINOR}"

  AC_SUBST(PHP8_VERNUM_MAJOR_DOT_MINOR)

fi


AC_MSG_CHECKING([Whether to do SWIG PHP8 frontend])
AC_MSG_RESULT($DO_SWIG_PHP8)

if test x$DO_SWIG_PHP8 = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi










dnl SWIG support for nodejs

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



AC_MSG_CHECKING([Whether we are to do nodejs...])
if test x$enable_shared != xyes; then
   if test x$DO_SWIG_NODEJS = xyes; then
      AC_MSG_WARN([
WARNING: You cannot do SWIG backends (in this case, Node.js)
if you are not also doing shared libraries. This means
that you must not use --disable-shared when wishing to
do a SWIG backend.])
      DO_SWIG_NODEJS=no
   else
      AC_MSG_RESULT([no, since we are not doing shared libraries.])
      DO_SWIG_NODEJS=no
   fi
else
   AC_MSG_RESULT([maybe... let's check some more...])
fi






if test x$DO_SWIG_NODEJS != xno; then
  dnl nodejs node program
  AC_MSG_CHECKING([for Node.js node program...])
  AC_CHECK_PROGS(NODE_PROGRAM, [node], [no])
  AC_SUBST(NODE_PROGRAM)
  if test x$NODE_PROGRAM = xno; then
    if test x$DO_SWIG_NODEJS = xyes; then
      AC_MSG_WARN([
  Could not find Node.js node program in path. Not doing SWIG Node.js bindings.])
      DO_SWIG_NODEJS=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Node.js frontend.])
      DO_SWIG_NODEJS=no
    fi
  fi    
fi


if test x$DO_SWIG_NODEJS != xno; then
  dnl nodejs node-gyp program
  AC_MSG_CHECKING([for Node.js node-gyp program...])
  AC_CHECK_PROGS(NODE_GYP_PROGRAM, [node-gyp], [no])
  AC_SUBST(NODE_GYP_PROGRAM)
  if test x$NODE_GYP_PROGRAM = xno; then
    if test x$DO_SWIG_NODEJS = xyes; then
      AC_MSG_WARN([
  Could not find Node.js node-gyp program in path. Not doing SWIG Node.js bindings.])
      DO_SWIG_NODEJS=no
    else
      AC_MSG_RESULT([Not found. Not doing SWIG Node.js frontend.])
      DO_SWIG_NODEJS=no
    fi
  fi    
fi


dnl Test for NodeJS version
if test x$DO_SWIG_NODEJS != xno; then
  AC_MSG_CHECKING([Node.js version... ])

  NODEJS_VERSION=`$NODE_PROGRAM --version | sed -e 's_v__g'`
  AC_MSG_RESULT($NODEJS_VERSION)
fi

if test x$DO_SWIG_NODEJS != xno; then
  # 
  # If we came this far, then 'maybe' should be 'yes', unconditionally.
  #
  DO_SWIG_NODEJS=yes

  dnl NODEJS VERSION MAJOR / MINOR / MAJOR_DOT_MINOR
  NODEJS_VERSION_MAJOR=`echo $NODEJS_VERSION | awk -F '\.' '{a=1; MAJOR_VERSION = $a + 0; print MAJOR_VERSION; }' `
  NODEJS_VERSION_MINOR=`echo $NODEJS_VERSION |  | awk -F '\.' '{a=2; MINOR_VERSION = $a + 0; print MINOR_VERSION; }' `

  dnl NODEJS_VERSION_MAJOR_DOT_MINOR="${NODEJS_VERSION_MAJOR}.${NODEJS_VERSION_MINOR}"

  AC_SUBST(NODEJS_VERSION_MAJOR)

  dnl AC_SUBST(NODEJS_VERSION_MAJOR_DOT_MINOR)

fi


AC_MSG_CHECKING([Whether to do SWIG Node.js frontend])
AC_MSG_RESULT($DO_SWIG_NODEJS)

if test x$DO_SWIG_NODEJS = xyes; then
  DO_AT_LEAST_ONE_SWIG=yes
fi













WITH_SWIG_CSHARP="--with-swig-language-csharp=$ORIGINAL_DO_SWIG_CSHARP"
WITH_SWIG_JAVA="--with-swig-language-java=$ORIGINAL_DO_SWIG_JAVA"
WITH_SWIG_PHP7="--with-swig-language-php7=$ORIGINAL_DO_SWIG_PHP7"
WITH_SWIG_PHP8="--with-swig-language-php8=$ORIGINAL_DO_SWIG_PHP8"
WITH_SWIG_PYTHON2="--with-swig-language-python2=$ORIGINAL_DO_SWIG_PYTHON2"
WITH_SWIG_PYTHON3="--with-swig-language-python3=$ORIGINAL_DO_SWIG_PYTHON3"
WITH_SWIG_NODEJS="--with-swig-language-nodejs=$ORIGINAL_DO_SWIG_NODEJS"

AC_SUBST(SWIG_PROGRAM)
AC_SUBST(WITH_SWIG_CSHARP)
AC_SUBST(WITH_SWIG_JAVA)
AC_SUBST(WITH_SWIG_PHP7)
AC_SUBST(WITH_SWIG_PHP8)
AC_SUBST(WITH_SWIG_PYTHON2)
AC_SUBST(WITH_SWIG_PYTHON3)
AC_SUBST(WITH_SWIG_NODEJS)

AM_CONDITIONAL(SWIG_WITH_CSHARP_WRAPPERS, test x$DO_SWIG_CSHARP = xyes)
AC_SUBST(SWIG_WITH_CSHARP_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_JAVA_WRAPPERS, test x$DO_SWIG_JAVA = xyes)
AC_SUBST(SWIG_WITH_JAVA_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PHP7_WRAPPERS, test x$DO_SWIG_PHP7 = xyes)
AC_SUBST(SWIG_WITH_PHP7_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PHP8_WRAPPERS, test x$DO_SWIG_PHP8 = xyes)
AC_SUBST(SWIG_WITH_PHP8_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PYTHON2_WRAPPERS, test x$DO_SWIG_PYTHON2 = xyes)
AC_SUBST(SWIG_WITH_PYTHON2_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_PYTHON3_WRAPPERS, test x$DO_SWIG_PYTHON3 = xyes)
AC_SUBST(SWIG_WITH_PYTHON3_WRAPPERS)

AM_CONDITIONAL(SWIG_WITH_NODEJS_WRAPPERS, test x$DO_SWIG_NODEJS = xyes)
AC_SUBST(SWIG_WITH_NODEJS_WRAPPERS)

AM_CONDITIONAL(CAN_DO_SWIG, test x$CAN_DO_SWIG = xyes)
AC_SUBST(CAN_DO_SWIG)

AM_CONDITIONAL(CAN_SWIG_DO_PHP7, test x$CAN_SWIG_DO_PHP7 = xyes)
AC_SUBST(CAN_SWIG_DO_PHP7)

AM_CONDITIONAL(CAN_SWIG_DO_PHP8, test x$CAN_SWIG_DO_PHP8 = xyes)
AC_SUBST(CAN_SWIG_DO_PHP8)

AC_SUBST(DO_SWIG_CSHARP)
AC_SUBST(DO_SWIG_JAVA)
AC_SUBST(DO_SWIG_PHP7)
AC_SUBST(DO_SWIG_PHP8)
AC_SUBST(DO_SWIG_PYTHON2)
AC_SUBST(DO_SWIG_PYTHON3)
AC_SUBST(DO_SWIG_NODEJS)



])
