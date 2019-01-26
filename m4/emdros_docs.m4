AC_DEFUN([EMDROS_DOCS], [


dnl
dnl pdflatex dependency
dnl
AC_CHECK_PROGS([PROG_PDFLATEX], [pdflatex], [no])

AC_MSG_CHECKING([whether to do PDFLaTeX docs...])
if test "x$PROG_PDFLATEX" = "xno"; then
   AC_MSG_RESULT([no: That part of the docs won't be made.])
   DO_PDFLATEX="no"
else
   AC_MSG_RESULT([yes.])
   DO_PDFLATEX="yes"
fi

AM_CONDITIONAL(DO_PROG_PDFLATEX, test "x$DO_PDFLATEX" = "xyes")



dnl
dnl groff dependency
dnl
AC_CHECK_PROGS([PROG_GROFF], [groff], [no])

AC_MSG_CHECKING([whether to do Groff docs...])
if test "x$PROG_GROFF" = "xno"; then
   AC_MSG_RESULT([no: That part of the docs won't be made.])
   DO_GROFF="no"
else
   AC_MSG_RESULT([yes.])
   DO_GROFF="yes"
fi

AM_CONDITIONAL(DO_PROG_GROFF, test "x$DO_GROFF" = "xyes")


dnl
dnl zip dependency
dnl
AC_CHECK_PROGS([PROG_ZIP], [zip], [no])

AC_MSG_CHECKING([whether to do Zip docs...])
if test "x$PROG_ZIP" = "xno"; then
   AC_MSG_RESULT([no: That part of the docs won't be made.])
   DO_ZIP="no"
else
   AC_MSG_RESULT([yes.])
   DO_ZIP="yes"
fi

AM_CONDITIONAL(DO_PROG_ZIP, test "x$DO_ZIP" = "xyes")


])
