#
# Emdros Visual C++ Makefile for amalgamation directory
#
# Makefile for Microsoft Visual C++ 2010 (or compatible)

TOPSRCDIR = ..

!INCLUDE $(TOPSRCDIR)\win32\config.mak


LOCAL_CPPFLAGS = -I. -DHAVE_CONFIG_H -DDEFAULT_BACKEND_ENUM=$(DEFAULT_BACKEND_ENUM)
LIBTARGET = libemdros_amalgamation.lib
TARGETS = emdros-config.h  emdros-lconfig.h  "$(OUTDIR)\$(LIBTARGET)"

LIBTARGET_OBJS= \
          "$(INTDIR)\emdros_amalgamation_1_emdros.obj" \
          "$(INTDIR)\emdros_amalgamation_2_importers.obj" \
          "$(INTDIR)\emdros_c_amalgamation_1.obj" \
          "$(INTDIR)\emdros_c_amalgamation_2.obj" 


#PROGRAM_DEPENDENCIES = "$(OUTDIR)\libemdf.lib"

LOCAL_LIBFLAGS = \
 $(DBLIB) $(PROGRAM_DEPENDENCIES)

#PROGRAM1 = .\mkamalgamation.exe
#PROGRAM1_OBJS = "$(INTDIR)\mkamalgamation.obj"
#PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)


!INCLUDE $(TOPSRCDIR)\windows\build\body-vc.mak


#emdros_amalgamation_1.cpp: .\mkamalgamation.exe amalgamation.json
#	.\mkamalgamation.exe amalgamation.json



emdros_config_h: emdros-config.h emdros-lconfig.h

emdros-config.h: $(WINBUILDDIR)\windows-emdros-config.h
	copy /Y /B $(WINBUILDDIR)\windows-emdros-config.h .\emdros-config.h

emdros-lconfig.h: $(WINBUILDDIR)\windows-emdros-lconfig.h
	copy /Y /B $(WINBUILDDIR)\windows-emdros-lconfig.h .\emdros-lconfig.h
