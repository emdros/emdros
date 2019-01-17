AC_DEFUN([EMDROS_DOCS], [


dnl
dnl pdflatex dependency
dnl
AC_CHECK_PROGS([PDFLATEX], [pdflatex], [no])

AC_MSG_CHECKING([whether to do PDFLaTeX docs...])
if test "x$PDFLATEX" = "xno"; then
   AC_MSG_RESULT([no: That part of the docs won't be made.])
   DO_PDFLATEX="no"
else
   AC_MSG_RESULT([yes.])
   DO_PDFLATEX="yes"
fi

AM_CONDITIONAL(DO_PROG_PDFLATEX, test xDO_PDFLATEX = xyes)



dnl
dnl groff dependency
dnl
AC_CHECK_PROGS([GROFF], [groff], [no])

AC_MSG_CHECKING([whether to do Groff docs...])
if test "x$GROFF" = "xno"; then
   AC_MSG_RESULT([no: That part of the docs won't be made.])
   DO_GROFF="no"
else
   AC_MSG_RESULT([yes.])
   DO_GROFF="yes"
fi

AM_CONDITIONAL(DO_PROG_GROFF, test xDO_GROFF = xyes)


])
