#
# Emdros Win32 Makefile for util directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libemdros_importers.lib

WIN32DIR = ..\win32


!IF "$(BUILD_FOR_DEVEL)" == "1"
LIBTARGET = libemdros_importers.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
!ELSE
LIBTARGET =
TARGETS = 
!ENDIF
CLEANFILES = 

LIBTARGET_OBJS= \
        "$(INTDIR)\unboundbibleimporter.obj" \
        "$(INTDIR)\pennparser.obj" \
        "$(INTDIR)\pennlexer.obj" \
        "$(INTDIR)\penn_yylex.obj" \
        "$(INTDIR)\pennimporter.obj" \
        "$(INTDIR)\negraimporter.obj" \
        "$(INTDIR)\tigerxmlimporter.obj" \
        "$(INTDIR)\plaintextimporter.obj" \
        "$(INTDIR)\slashedtextimporter.obj" \
        "$(INTDIR)\sfm.obj" \
        "$(INTDIR)\sfmimporter.obj" 


!INCLUDE $(WIN32DIR)\config.mak
!INCLUDE $(WIN32DIR)\body.mak


