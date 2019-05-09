#
# Emdros Win32 Makefile for wx/ directory
#
# Makefile for MinGW

# Will build a Win32 library, libemdrosguiu.a

WIN32DIR = ..\win32

include $(WIN32DIR)\mingw-config.mak

ifeq ($(WITH_WXWIN), yes)
LIB_WX_UNICODE_TARGET = libemdrosguiu.a
endif


TARGETS = "$(OUTDIR_UNICODE)\$(LIB_WX_UNICODE_TARGET)" 


LOCAL_UNICODE_CPPFLAGS = $(WX_CPPFLAGS_UNICODE) -I$(WXDIR)/contrib/include 


EMDF_DEPENDENCIES = -lemdf
MQL_DEPENDENCIES = -lmql

CLEANFILES = 


LIB_WX_UNICODE_TARGET_OBJS = \
      dropdown_widget.o \
      wxorder_chooser.o \
      browsewidget.o \
      configwizard.o \
      leftrightchooser.o \
      conndlg.o \
      genericdlg.o \
      layout.o \
      treeitem.o \
      viewmetrics.o \
      vtreecanvas.o \
      htreecanvas.o \
      importerwizard.o \
      wxutil_emdros.o
      

include $(WIN32DIR)\mingw-body.mak


