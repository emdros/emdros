#
# Emdros Win32 Makefile for util directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libutil_emdros.lib

WIN32DIR = ..\build\windows


LIBTARGET = libimporters_emdros.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
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


