#
# Emdros Win32 Makefile for wx/ directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library, libemdrosguiu.lib

WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak

!IF "$(WITH_WXWIN)" == "yes"
LIB_WX_UNICODE_TARGET = libemdrosguiu.lib
!ENDIF


TARGETS = "$(OUTDIR_UNICODE)\$(LIB_WX_UNICODE_TARGET)" 


LOCAL_UNICODE_CPPFLAGS = $(WX_CPPFLAGS_UNICODE) -I$(WXDIR)/contrib/include 


EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.lib
MQL_DEPENDENCIES = $(OUTDIR)\libmql.lib

CLEANFILES = 


LIB_WX_UNICODE_TARGET_OBJS = \
      "$(INTDIR_UNICODE)\dropdown_widget.obj" \
      "$(INTDIR_UNICODE)\wxorder_chooser.obj" \
      "$(INTDIR_UNICODE)\browsewidget.obj" \
      "$(INTDIR_UNICODE)\configwizard.obj" \
      "$(INTDIR_UNICODE)\leftrightchooser.obj" \
      "$(INTDIR_UNICODE)\conndlg.obj" \
      "$(INTDIR_UNICODE)\genericdlg.obj" \
      "$(INTDIR_UNICODE)\layout.obj" \
      "$(INTDIR_UNICODE)\treeitem.obj" \
      "$(INTDIR_UNICODE)\viewmetrics.obj" \
      "$(INTDIR_UNICODE)\vtreecanvas.obj" \
      "$(INTDIR_UNICODE)\htreecanvas.obj" \
      "$(INTDIR_UNICODE)\importerwizard.obj" \
      "$(INTDIR_UNICODE)\wxutil_emdros.obj"
      

!INCLUDE $(WIN32DIR)\body.mak


