#
# Emdros Visual C++ Makefile for amalgamation directory
#
# Makefile for Microsoft Visual C++ 2010 (or compatible)

TOPSRCDIR = ..

WIN32DIR = $(TOPSRCDIR)\win32

!INCLUDE $(WIN32DIR)\config.mak


LOCAL_CPPFLAGS = -I. -DHAVE_CONFIG_H -DDEFAULT_BACKEND_ENUM=$(DEFAULT_BACKEND_ENUM)
LIBTARGET = libemdros_amalgamation.lib
TARGETS = $(PROGRAM1) emdros_amalgamation_1_emdros.cpp emdros-config.h  emdros-lconfig.h  "$(OUTDIR)\$(LIBTARGET)"

LIBTARGET_OBJS= \
          "$(INTDIR)\emdros_amalgamation_1_emdros.obj" \
          "$(INTDIR)\emdros_amalgamation_2_importers.obj" \
          "$(INTDIR)\emdros_c_amalgamation_1.obj" \
          "$(INTDIR)\emdros_c_amalgamation_2.obj" 


#PROGRAM_DEPENDENCIES = "$(OUTDIR)\libemdf.lib"

LOCAL_LIBFLAGS = \
 $(DBLIB) $(PROGRAM_DEPENDENCIES)

PROGRAM1 = .\mkamalgamation.exe
PROGRAM1_OBJS = "$(INTDIR)\mkamalgamation.obj"
PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)


!INCLUDE $(TOPSRCDIR)\win32\body.mak


emdros_amalgamation_1_emdros.cpp: $(PROGRAM1) amalgamation.xml
	$(PROGRAM1) amalgamation.xml



emdros_config_h: emdros-config.h emdros-lconfig.h

emdros-config.h: $(TOPSRCDIR)\win32\config.h.win32
	copy /Y /B $(TOPSRCDIR)\win32\config.h.win32 .\emdros-config.h

emdros-lconfig.h: $(TOPSRCDIR)\win32\lconfig.h.win32
	copy /Y /B $(TOPSRCDIR)\win32\lconfig.h.win32  .\emdros-lconfig.h
