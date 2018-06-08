#
# Emdros Win32 Makefile for util directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libutil_emdros.lib

WIN32DIR = ..\build\windows


LIBTARGET = libutil_emdros.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
CLEANFILES = 

LIBTARGET_OBJS= \
        "$(INTDIR)\conf.obj" \
        "$(INTDIR)\md5_emdros.obj" \
        "$(INTDIR)\opt.obj" \
        "$(INTDIR)\messages.obj" \
        "$(INTDIR)\agxml.obj" \
        "$(INTDIR)\erw.obj" \
        "$(INTDIR)\schema.obj" \
        "$(INTDIR)\mql_exporter.obj" \
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
        "$(INTDIR)\sfmimporter.obj" \
        "$(INTDIR)\bptdumper.obj" \
        "$(INTDIR)\bpt2dumper.obj"



!INCLUDE $(WIN32DIR)\config.mak
!INCLUDE $(WIN32DIR)\body.mak


