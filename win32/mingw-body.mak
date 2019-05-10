#
# Header for all make files
#

RSC=windres.exe


#
# Debug/Non-debug 
#
ifeq "$(DEBUG)"  "1"
OUTDIR=$(WIN32DIR)\Debug
OUTDIR_UNICODE = $(WIN32DIR)\DebugUnicode
INTDIR=$(WIN32DIR)\Debug$(INTDIR_SUFFIX)
INTDIR_UNICODE = $(WIN32DIR)\DebugUnicode$(INTDIR_SUFFIX)
INTDIR_UNICODE2 = $(WIN32DIR)\DebugUnicode2$(INTDIR_SUFFIX)
INTDIR_UNICODE3 = $(WIN32DIR)\DebugUnicode3$(INTDIR_SUFFIX)
INTDIR_UNICODE4 = $(WIN32DIR)\DebugUnicode4$(INTDIR_SUFFIX)
else
OUTDIR=$(WIN32DIR)\Release
OUTDIR_UNICODE = $(WIN32DIR)\ReleaseUnicode
INTDIR=$(WIN32DIR)\Release$(INTDIR_SUFFIX)
INTDIR_UNICODE = $(WIN32DIR)\ReleaseUnicode$(INTDIR_SUFFIX)
INTDIR_UNICODE2 = $(WIN32DIR)\ReleaseUnicode2$(INTDIR_SUFFIX)
INTDIR_UNICODE3 = $(WIN32DIR)\ReleaseUnicode3$(INTDIR_SUFFIX)
INTDIR_UNICODE4 = $(WIN32DIR)\ReleaseUnicode4$(INTDIR_SUFFIX)
endif

PROGRAMS = $(CPROGRAM1) $(CPROGRAM2) $(CPROGRAM3) \
           $(PROGRAM1) $(PROGRAM2) $(PROGRAM3) $(PROGRAM4) \
           $(PROGRAM5) $(PROGRAM6) $(PROGRAM7) $(PROGRAM8) \
           $(PROGRAM9) $(PROGRAM10) $(PROGRAM11) $(PROGRAM12) \
           $(PROGRAM12) $(PROGRAM13) $(PROGRAM14) $(PROGRAM15) \
           $(PROGRAM16) $(PROGRAM17) $(PROGRAM18) $(PROGRAM19) \
           $(WXUNICODE_PROGRAM1) \
           $(WXUNICODE_PROGRAM2) \
           $(WXUNICODE_PROGRAM3) \
           $(WXUNICODE_PROGRAM4) 



ifeq "$(PGSQL)"  "1"

DBINCLUDE_PG = -I$(PGTOP)\include
DBLIB_PG = "$(PGTOP)\lib\libpq.lib" 
CBS_PGSQL = _pg

else

endif

ifeq "$(MYSQL)"  "1"

DBINCLUDE_MY = -I..\win32\mysql
DBLIB_MY = "$(MYSQLTOP)\lib\libmysql.lib"
CBS_MYSQL = _my

else

endif

ifeq "$(SQLITE3)"  "1"

DBINCLUDE_SQLITE3 = -I..\win32
DBLIB_SQLITE3 = $(OUTDIR)\libsqlite3_emdros.a
CBS_SQLITE3 = _sq3

endif

DBINCLUDE = $(DBINCLUDE_PG) $(DBINCLUDE_MY) \
            $(DBINCLUDE_SQLITE3)

DBLIB = $(DBLIB_PG) $(DBLIB_MY) \
        $(DBLIB_SQLITE3)

CHARACTERISTIC_BACKEND_STRING = $(CBS_SQLITE3)$(CBS_PGSQL)$(CBS_MYSQL)



#-----------------------------
#DEFAULT_BACKEND_ENUM
#-----------------------------
ifeq "$(POSTGRESQL)"  "1"

DEFAULT_BACKEND_ENUM = kPostgreSQL

else ifeq "$(MYSQL)"  "1"

DEFAULT_BACKEND_ENUM = kMySQL
else ifeq "$(SQLITE3)"  "1"

DEFAULT_BACKEND_ENUM = kSQLite3
endif


# This is necessary for successful runtime of TECkit
# on Visual Studio (Express) 2005.  The reason is that
# Visual Studio (Express) 2005 implements checks on
# iterators, and TECkit relies on being able to have iterators 
# be outside the bounds of the vector.
TECKIT_CPPFLAGS_EXTRA = -D_HAS_ITERATOR_DEBUGGING=0 -D_SECURE_SCL=0

CPPFLAGS_EXTRA = $(TECKIT_CPPFLAGS_EXTRA) 


PCRELIB=$(OUTDIR)\libpcre_emdros.a

ifeq "$(TARGET_CPU)"  "X86"
APPVER=5.0
else
APPVER=5.02
endif



ifeq "$(WITH_WXWIN)"  "yes"

WXDIR=$(WXWIN_HOME)

#
# Find WXLIBDIR
#
ifeq "$(DEBUG)"  "1"

ifeq "$(TARGET_CPU)"  "X86"


ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxmsw28ud.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=28
else ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxbase29ud.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=29
else ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxbase30ud.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=30
else ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxbase31ud.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=31
else ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxbase32ud.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=32
else
$(error Could not find wxWidgets library in $(WXDIR))
endif 

else

# TARGET_CPU == X64


ifneq ($(wildcard $(WXDIR)/lib\vc_amd64_lib\libwxmsw28ud.a),)
WXLIBDIR=$(WXDIR)/lib\vc_amd64_lib
WXVERSION=28
else ifneq ($(wildcard $(WXDIR)/lib\vc_x64_lib\libwxbase29ud.a),)
WXLIBDIR=$(WXDIR)/lib\vc_x64_lib
WXVERSION=29
else ifneq ($(wildcard $(WXDIR)/lib\vc_x64_lib\libwxbase30ud.a),)
WXLIBDIR=$(WXDIR)/lib\vc_x64_lib
WXVERSION=30
else ifneq ($(wildcard $(WXDIR)/lib\vc_x64_lib\libwxbase31ud.a),)
WXLIBDIR=$(WXDIR)/lib\vc_x64_lib
WXVERSION=31
else ifneq ($(wildcard $(WXDIR)/lib\vc_x64_lib\libwxbase32ud.a),)
WXLIBDIR=$(WXDIR)/lib\vc_x64_lib
WXVERSION=32
else
$(error Could not find wxWidgets library in $(WXDIR))
endif 

endif # TARGET_CPU = X86/X64


else
# Non-debug build

ifeq "$(TARGET_CPU)"  "X86"

ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxmsw28u.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=28
else ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxbase29u.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=29
else ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxbase30u.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=30
else ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxbase31u.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=31
else ifneq ($(wildcard $(WXDIR)\lib\gcc_lib\libwxbase32u.a),)
WXLIBDIR=$(WXDIR)/lib/gcc_lib
WXVERSION=32
else
$(error Could not find wxWidgets library in $(WXDIR). Perhaps define WITH_WXWIN=no to disable wxWidgets?)
endif 

else

# TARGET_CPU == X64


ifneq ($(wildcard $(WXDIR)/lib\vc_amd64_lib\wxmsw28u.a),)
WXLIBDIR=$(WXDIR)/lib\vc_amd64_lib
WXVERSION=28

else ifneq ($(wildcard $(WXDIR)/lib\vc_x64_lib\wxbase29u.a),)
WXLIBDIR=$(WXDIR)/lib\vc_x64_lib
WXVERSION=29
else ifneq ($(wildcard $(WXDIR)/lib\vc_x64_lib\wxbase30u.a),)
WXLIBDIR=$(WXDIR)/lib\vc_x64_lib
WXVERSION=30
else ifneq ($(wildcard $(WXDIR)/lib\vc_x64_lib\wxbase31u.a),)
WXLIBDIR=$(WXDIR)/lib\vc_x64_lib
WXVERSION=31
else ifneq ($(wildcard $(WXDIR)/lib\vc_x64_lib\wxbase32u.a),)
WXLIBDIR=$(WXDIR)/lib\vc_x64_lib
WXVERSION=32
else
$(error Could not find wxWidgets library in $(WXDIR))
endif 

endif # TARGET_CPU = X86/X64

endif # Debug/Non-debug build





#
# Find most wxWidgets variables
#

ifeq "$(TARGET_CPU)"  "X86"
WINFLAGS=-c -DWIN32 -D__WIN32__ \
         -D_WINDOWS -DSTRICT
#WINLINKFLAGS=/INCREMENTAL:NO -machine:i386 -subsystem:windows,$(APPVER) /MANIFEST
WINLINKFLAGS=
WINLIBS=  -lversion -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwinspool -lwinmm -lcomctl32 -lshell32 -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 -lwsock32 -lwininet -lshlwapi -luxtheme -loleacc

guilibsdll= 
WX_LIBS=-L$(WXLIBDIR) -L$(WXDIR)/contrib\lib\gcc_lib 
#WX_LINKFLAGS=/RELEASE 
WX_LINKFLAGS=
else

WINFLAGS=-c -DWIN64 -D__WIN64__ \
         -D_WINDOWS -DSTRICT
#WINLINKFLAGS=/INCREMENTAL:NO -machine:X64 -subsystem:windows,$(APPVER) /MANIFEST
WINLINKFLAGS=
WINLIBS=  -lversion -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwinspool -lwinmm-lcomctl32 -lshell32 -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 -lwsock32 -lwininet -lshlwapi  -luxtheme -loleacc

guilibsdll= 
WX_LIBS=-L$(WXLIBDIR) -L$(WXDIR)/contrib\lib\vc_x64_lib 
#WX_LINKFLAGS=/RELEASE 
WX_LINKFLAGS=

endif # TARGET_CPU == X86


#
# Find WX_LIBS_UNICODE and WX_DEBUG_SENSITIVE_UNICODE_CPPFLAGS
#
ifeq "$(DEBUG)"  "1"
WX_DEBUG_SENSITIVE_UNICODE_CPPFLAGS = -I$(WXLIBDIR)\mswud

ifeq "$(WXVERSION)"  "28"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxregexud \
   -lwxmsw28ud

else ifeq "$(WXVERSION)"  "29"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxbase29ud -lwxmsw29ud_gl -lwxmsw29ud_richtext \
   -lwxbase29ud_net -lwxmsw29ud_html -lwxmsw29ud_stc \
   -lwxbase29ud_xml -lwxmsw29ud_media -lwxmsw29ud_webview \
   -lwxmsw29ud_adv -lwxmsw29ud_propgrid -lwxmsw29ud_xrc \
   -lwxmsw29ud_aui -lwxmsw29ud_qa -lwxregexud \
   -lwxmsw29ud_core -lwxmsw29ud_ribbon \
   -lwxscintilla
else ifeq "$(WXVERSION)"  "30"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxbase30ud -lwxmsw30ud_gl -lwxmsw30ud_richtext \
   -lwxbase30ud_net -lwxmsw30ud_html -lwxmsw30ud_stc \
   -lwxbase30ud_xml -lwxmsw30ud_media -lwxmsw30ud_webview \
   -lwxmsw30ud_adv -lwxmsw30ud_propgrid -lwxmsw30ud_xrc \
   -lwxmsw30ud_aui -lwxregexud \
   -lwxmsw30ud_core -lwxmsw30ud_ribbon \
   -lwxscintillad
else ifeq "$(WXVERSION)"  "31"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxbase31ud -lwxmsw31ud_gl -lwxmsw31ud_richtext \
   -lwxbase31ud_net -lwxmsw31ud_html -lwxmsw31ud_stc \
   -lwxbase31ud_xml -lwxmsw31ud_media -lwxmsw31ud_webview \
   -lwxmsw31ud_adv -lwxmsw31ud_propgrid -lwxmsw31ud_xrc \
   -lwxmsw31ud_aui -lwxregexud \
   -lwxmsw31ud_core -lwxmsw31ud_ribbon \
   -lwxscintillad
else ifeq "$(WXVERSION)"  "32"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxbase32ud -lwxmsw32ud_gl -lwxmsw32ud_richtext \
   -lwxbase32ud_net -lwxmsw32ud_html -lwxmsw32ud_stc \
   -lwxbase32ud_xml -lwxmsw32ud_media -lwxmsw32ud_webview \
   -lwxmsw32ud_adv -lwxmsw32ud_propgrid -lwxmsw32ud_xrc \
   -lwxmsw32ud_aui -lwxregexud \
   -lwxmsw32ud_core -lwxmsw32ud_ribbon \
   -lwxscintillad
else
$(error Unknown WXVERSION value: $(WXVERSION))
endif

else
# Non-debug build

WX_DEBUG_SENSITIVE_UNICODE_CPPFLAGS = -I$(WXLIBDIR)\mswu

ifeq "$(WXVERSION)"  "28"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxregexu \
   -lwxmsw28u

else ifeq "$(WXVERSION)"  "29"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxbase29u -lwxmsw29u_gl -lwxmsw29u_richtext \
   -lwxbase29u_net -lwxmsw29u_html -lwxmsw29u_stc \
   -lwxbase29u_xml -lwxmsw29u_media -lwxmsw29u_webview \
   -lwxmsw29u_adv -lwxmsw29u_propgrid -lwxmsw29u_xrc \
   -lwxmsw29u_aui -lwxmsw29u_qa -lwxregexu \
   -lwxmsw29u_core -lwxmsw29u_ribbon \
   -lwxscintilla
else ifeq "$(WXVERSION)"  "30"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxmsw30u_html -lwxmsw30u_stc \
   -lwxmsw30u_media -lwxmsw30u_webview \
   -lwxmsw30u_gl -lwxmsw30u_richtext \
   -lwxmsw30u_adv -lwxmsw30u_propgrid -lwxmsw30u_xrc \
   -lwxmsw30u_aui -lwxmsw30u_ribbon -lwxmsw30u_core \
   -lwxbase30u_xml -lwxbase30u_net -lwxbase30u \
   -lwxscintilla \
   -lwxjpeg -lwxpng -lwxtiff -lwxzlib -lwxregexu
else ifeq "$(WXVERSION)"  "31"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxmsw31u_html -lwxmsw31u_stc \
   -lwxmsw31u_media -lwxmsw31u_webview \
   -lwxmsw31u_gl -lwxmsw31u_richtext \
   -lwxmsw31u_adv -lwxmsw31u_propgrid -lwxmsw31u_xrc \
   -lwxmsw31u_aui -lwxmsw31u_ribbon -lwxmsw31u_core \
   -lwxbase31u_xml -lwxbase31u_net -lwxbase31u \
   -lwxscintilla \
   -lwxjpeg -lwxpng -lwxtiff -lwxzlib -lwxregexu
else ifeq "$(WXVERSION)"  "32"
WX_LIBS_UNICODE=$(WX_LIBS) \
   -lwxmsw32u_html -lwxmsw32u_stc \
   -lwxmsw32u_media -lwxmsw32u_webview \
   -lwxmsw32u_gl -lwxmsw32u_richtext \
   -lwxmsw32u_adv -lwxmsw32u_propgrid -lwxmsw32u_xrc \
   -lwxmsw32u_aui -lwxmsw32u_ribbon -lwxmsw32u_core \
   -lwxbase32u_xml -lwxbase32u_net -lwxbase32u \
   -lwxscintilla \
   -lwxjpeg -lwxpng -lwxtiff -lwxzlib -lwxregexu
else
$(error Unknown WXVERSION value: $(WXVERSION))
endif


# Debug/Non-debug build
endif



WX_LINKFLAGS_UNICODE = $(WX_LINKFLAGS)

EXTRADLLFLAGS=
CRTFLAG=
OVERRIDEFLAGS=
WXINC=$(WXDIR)/include
INC=-I$(WXINC) -I$(WXDIR)/contrib/include #-I$(WXDIR)/src/regex -I$(WXDIR)/src/png -I$(WXDIR)/src/zlib -I$(WXDIR)/src/jpeg -I$(WXDIR)/src/tiff
WX_CPPFLAGS=$(WINFLAGS) -DwxUSE_NO_MANIFEST=1 -D__WXMSW__ $(INC) $(OPT) $(EXTRADLLFLAGS)  $(CRTFLAG) -D__WINDOWS__ $(OVERRIDEFLAGS) #-DWXUSINGDLL
WX_CPPFLAGS_UNICODE = $(WX_CPPFLAGS) \
                      $(WX_DEBUG_SENSITIVE_UNICODE_CPPFLAGS) \
                      -DwxUSE_UNICODE=1
endif 


all: $(OUTDIR) $(INTDIR) $(OUTDIR_UNICODE) $(INTDIR_UNICODE) $(INTDIR_UNICODE2) $(INTDIR_UNICODE3) $(INTDIR_UNICODE4) $(TARGETS)


ifeq "$(OS)"  "Windows_NT"
NULL=
else 
NULL=nul
endif 

$(OUTDIR):
	if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

$(OUTDIR_UNICODE):
	if not exist "$(OUTDIR_UNICODE)/$(NULL)" mkdir "$(OUTDIR_UNICODE)"

ifneq "$(INTDIR)"  "$(OUTDIR)"
$(INTDIR):
	if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"
endif

ifneq "$(INTDIR_UNICODE)"  "$(OUTDIR_UNICODE)"
$(INTDIR_UNICODE):
	if not exist "$(INTDIR_UNICODE)/$(NULL)" mkdir "$(INTDIR_UNICODE)"
endif

ifneq "$(INTDIR_UNICODE2)"  "$(OUTDIR_UNICODE2)"
$(INTDIR_UNICODE2):
	if not exist "$(INTDIR_UNICODE2)/$(NULL)" mkdir "$(INTDIR_UNICODE2)"
endif

ifneq "$(INTDIR_UNICODE3)"  "$(OUTDIR_UNICODE3)"
$(INTDIR_UNICODE3):
	if not exist "$(INTDIR_UNICODE3)/$(NULL)" mkdir "$(INTDIR_UNICODE3)"
endif

ifneq "$(INTDIR_UNICODE4)"  "$(OUTDIR_UNICODE4)"
$(INTDIR_UNICODE4):
	if not exist "$(INTDIR_UNICODE4)/$(NULL)" mkdir "$(INTDIR_UNICODE4)"
endif




ifeq "$(DEBUGINFO)"  "1"
ifeq "$(TARGETNAME)"  ""
DEBUGINFO_FLAGS=
DEBUGINFO_FLAGS_UNICODE=
else
DEBUGINFO_FLAGS=-g 
DEBUGINFO_FLAGS_UNICODE=-g 
endif
endif

ifeq "$(DEBUG)"  "1"
# DEBUG build
DEBUG_SENSITIVE_CPPFLAGS = -Wall \
   -D_DEBUG -D__STL_DEBUG -D_ITERATOR_DEBUG_LEVEL=2 -D_HAS_ITERATOR_DEBUGGING=1 -D_SECURE_SCL=1

else

# non-debug build
DEBUG_SENSITIVE_CPPFLAGS = -O2 -Wall -DNDEBUG=1 
endif


CPP_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 -I$(WIN32DIR)\..\include $(DBINCLUDE) \
 -DWIN32 -D_WINDOWS \
 -c  -DHAVE_VSNPRINTF -DHAVE_STRDUP \
 $(LOCAL_CPPFLAGS) $(DEBUGINFO_FLAGS) 


CPP_UNICODE_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 -I. -I$(WIN32DIR)\..\include -I$(WIN32DIR)\..\zlib \
 $(DBINCLUDE) \
 -DWIN32 -D_WINDOWS -D_UNICODE -DUNICODE \
 -c  -DHAVE_VSNPRINTF -DHAVE_STRDUP \
 $(LOCAL_CPPFLAGS) $(LOCAL_UNICODE_CPPFLAGS) $(DEBUGINFO_FLAGS_UNICODE) 

CPP_UNICODE2_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 -I. -I$(WIN32DIR)\..\include \
 $(DBINCLUDE) \
 -DWIN32 -D_WINDOWS /YX /GR \
 -c  -DHAVE_VSNPRINTF -DHAVE_STRDUP \
 $(LOCAL_UNICODE2_CPPFLAGS) $(DEBUGINFO_FLAGS) 

CPP_UNICODE3_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 -I. -I$(WIN32DIR)\..\include \
 $(DBINCLUDE) \
 -DWIN32 -D_WINDOWS /YX /GR \
 -c  -DHAVE_VSNPRINTF -DHAVE_STRDUP \
 $(LOCAL_UNICODE3_CPPFLAGS) $(DEBUGINFO_FLAGS) 

CPP_UNICODE4_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 -I. -I$(WIN32DIR)\..\include \
 $(DBINCLUDE) \
 -DWIN32 -D_WINDOWS \
 -c  -DHAVE_VSNPRINTF -DHAVE_STRDUP \
 $(LOCAL_UNICODE4_CPPFLAGS) $(DEBUGINFO_FLAGS) 




CPP_OBJS=$(INTDIR)
CPP_UNICODE_OBJS=$(INTDIR_UNICODE)
CPP_UNICODE2_OBJS=$(INTDIR_UNICODE2)
CPP_UNICODE3_OBJS=$(INTDIR_UNICODE3)
CPP_UNICODE4_OBJS=$(INTDIR_UNICODE4)
CPP_SBRS=.

LIB32=$(AR) rcs 
LIB32_FLAGS= $(LIBFLAGS)

DLL32=$(LD) -dll
DLL32_FLAGS= -lversion -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32\
 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 \
 -lwsock32 -lodbccp32 -lshlwapi  $(LIBFLAGS) $(LOCAL_LIBFLAGS)

LINK32=$(CXX) -static -static-libgcc 
CLINK32=$(CC) -static -static-libgcc 
LINK32_FLAGS=  -lversion -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32\
 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 \
 -lwsock32 -lodbccp32 -lshlwapi $(LIBFLAGS) \
 $(LOCAL_LIBFLAGS)

LINK32_WXFLAGS_UNICODE = $(WINLINKFLAGS) \
   $(guilibsdll) \
   $(WX_LIBS_UNICODE) \
   $(WX_LINKFLAGS_UNICODE) \
   $(LINK32_WXFLAGS_UNICODE_LOCALFLAGS) \
   $(WINLIBS) 



ifneq "$(LIBTARGET)"  ""
"$(OUTDIR)\$(LIBTARGET)": $(LIBTARGET_OBJS)
	$(LIB32) $(LIB32_FLAGS) "$(OUTDIR)\$(LIBTARGET)" $(LIBTARGET_OBJS)
endif


ifneq "$(LIB_WX_UNICODE_TARGET)"  ""
"$(OUTDIR_UNICODE)\$(LIB_WX_UNICODE_TARGET)": $(LIB_WX_UNICODE_TARGET_OBJS)
	$(LIB32) $(LIB32_FLAGS) "$(OUTDIR_UNICODE)\$(LIB_WX_UNICODE_TARGET)" $(LIB_WX_UNICODE_TARGET_OBJS)
endif


ifneq "$(DLLTARGET)"  ""
"$(OUTDIR)\$(DLLTARGET)": $(DLLTARGET_OBJS)
	$(DLL32) $(DLL32_FLAGS) -o "$(OUTDIR)\$(DLLTARGET)" $(DLLTARGET_OBJS)
endif


ifneq "$(CPROGRAM1)"  ""
$(CPROGRAM1): $(CPROGRAM1_OBJS) $(CPROGRAM1_DEPENDENCIES)
	$(CLINK32)  -o $(CPROGRAM1) $(CPROGRAM1_OBJS) $(LINK32_FLAGS) $(CPROGRAM1_DEPENDENCIES)
endif

ifneq "$(CPROGRAM2)"  ""
$(CPROGRAM2): $(CPROGRAM2_OBJS) $(CPROGRAM2_DEPENDENCIES)
	$(CLINK32)  -o $(CPROGRAM2) $(CPROGRAM2_OBJS) $(LINK32_FLAGS) $(CPROGRAM2_DEPENDENCIES)
endif

ifneq "$(CPROGRAM3)"  ""
$(CPROGRAM3): $(CPROGRAM3_OBJS) $(CPROGRAM3_DEPENDENCIES)
	$(CLINK32)  -o $(CPROGRAM3) $(CPROGRAM3_OBJS) $(LINK32_FLAGS) $(CPROGRAM3_DEPENDENCIES)
endif

ifneq "$(PROGRAM1)"  ""
$(PROGRAM1): $(PROGRAM1_OBJS) $(PROGRAM1_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM1) $(PROGRAM1_OBJS) $(LINK32_FLAGS) $(PROGRAM1_DEPENDENCIES)
endif

ifneq "$(PROGRAM2)"  ""
$(PROGRAM2): $(PROGRAM2_OBJS) $(PROGRAM2_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM2) $(PROGRAM2_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM3)"  ""
$(PROGRAM3): $(PROGRAM3_OBJS) $(PROGRAM3_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM3) $(PROGRAM3_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM4)"  ""
$(PROGRAM4): $(PROGRAM4_OBJS) $(PROGRAM4_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM4) $(PROGRAM4_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM5)"  ""
$(PROGRAM5): $(PROGRAM5_OBJS) $(PROGRAM5_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM5) $(PROGRAM5_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM6)"  ""
$(PROGRAM6): $(PROGRAM6_OBJS) $(PROGRAM6_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM6) $(PROGRAM6_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM7)"  ""
$(PROGRAM7): $(PROGRAM7_OBJS) $(PROGRAM7_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM7) $(PROGRAM7_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM8)"  ""
$(PROGRAM8): $(PROGRAM8_OBJS) $(PROGRAM8_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM8) $(PROGRAM8_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM9)"  ""
$(PROGRAM9): $(PROGRAM9_OBJS) $(PROGRAM9_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM9) $(PROGRAM9_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM10)"  ""
$(PROGRAM10): $(PROGRAM10_OBJS) $(PROGRAM10_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM10) $(PROGRAM10_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM11)"  ""
$(PROGRAM11): $(PROGRAM11_OBJS) $(PROGRAM11_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM11) $(PROGRAM11_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM12)"  ""
$(PROGRAM12): $(PROGRAM12_OBJS) $(PROGRAM12_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM12) $(PROGRAM12_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM13)"  ""
$(PROGRAM13): $(PROGRAM13_OBJS) $(PROGRAM13_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM13) $(PROGRAM13_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM14)"  ""
$(PROGRAM14): $(PROGRAM14_OBJS) $(PROGRAM14_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM14) $(PROGRAM14_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM15)"  ""
$(PROGRAM15): $(PROGRAM15_OBJS) $(PROGRAM15_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM15) $(PROGRAM15_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM16)"  ""
$(PROGRAM16): $(PROGRAM16_OBJS) $(PROGRAM16_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM16) $(PROGRAM16_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM17)"  ""
$(PROGRAM17): $(PROGRAM17_OBJS) $(PROGRAM17_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM17) $(PROGRAM17_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM18)"  ""
$(PROGRAM18): $(PROGRAM18_OBJS) $(PROGRAM18_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM18) $(PROGRAM18_OBJS) $(LINK32_FLAGS)
endif

ifneq "$(PROGRAM19)"  ""
$(PROGRAM19): $(PROGRAM19_OBJS) $(PROGRAM19_DEPENDENCIES)
	$(LINK32)  -o $(PROGRAM19) $(PROGRAM19_OBJS) $(LINK32_FLAGS)
endif




ifneq "$(WXUNICODE_PROGRAM1)"  ""
$(WXUNICODE_PROGRAM1): $(WXUNICODE_PROGRAM1_OBJS) $(WXUNICODE_PROGRAM1_DEPENDENCIES)
	$(LINK32) -mwindows -o $(WXUNICODE_PROGRAM1)  $(WXUNICODE_PROGRAM1_OBJS) $(WXUNICODE_PROGRAM1_DEPENDENCIES) $(LINK32_WXFLAGS_UNICODE) 
endif

ifneq "$(WXUNICODE_PROGRAM2)"  ""
$(WXUNICODE_PROGRAM2): $(WXUNICODE_PROGRAM2_OBJS) $(WXUNICODE_PROGRAM2_DEPENDENCIES)
	$(LINK32) -mwindows -o $(WXUNICODE_PROGRAM2)  $(WXUNICODE_PROGRAM2_OBJS) $(WXUNICODE_PROGRAM2_DEPENDENCIES) $(LINK32_WXFLAGS_UNICODE) 
endif

ifneq "$(WXUNICODE_PROGRAM3)"  ""
$(WXUNICODE_PROGRAM3): $(WXUNICODE_PROGRAM3_OBJS) $(WXUNICODE_PROGRAM3_DEPENDENCIES)
	$(LINK32) -mwindows -o $(WXUNICODE_PROGRAM3)  $(WXUNICODE_PROGRAM3_OBJS) $(WXUNICODE_PROGRAM3_DEPENDENCIES) $(LINK32_WXFLAGS_UNICODE) 
endif

ifneq "$(WXUNICODE_PROGRAM4)"  ""
$(WXUNICODE_PROGRAM4): $(WXUNICODE_PROGRAM4_OBJS) $(WXUNICODE_PROGRAM4_DEPENDENCIES)
	$(LINK32) -mwindows -o $(WXUNICODE_PROGRAM4)  $(WXUNICODE_PROGRAM4_OBJS) $(WXUNICODE_PROGRAM4_DEPENDENCIES) $(LINK32_WXFLAGS_UNICODE) 
endif




ifneq "$(NO_C_IN_THIS_DIRECTORY)"  "1"
%.o: %.c
	$(CC) -c $(CPP_UNICODE_PROJ) $<

endif

%.o: %.cpp
	$(CXX) -c $(CPP_UNICODE_PROJ) $(WX_CPPFLAGS_UNICODE) $(CPP_UNICODE_PROJ_EXTRA)  $< 


clean:
	-erase /Q /S /F $(PROGRAMS)
ifneq "$(CLEANFILES)"  ""
	-erase /Q /S /F $(CLEANFILES)
endif
ifneq "$(LIBTARGET)"  ""
	-@erase "$(OUTDIR)\$(LIBTARGET)"
endif
	-erase /Q /S /F $(LIBTARGET_OBJS)
ifneq "$(LIB_WX_UNICODE_TARGET)"  ""
	-@erase "$(OUTDIR_UNICODE)\$(LIB_WX_UNICODE_TARGET)"
endif
ifneq "$(DLLTARGET)"  ""
	-@erase "$(OUTDIR)\$(DLLTARGET)"
endif
ifneq "$(DLLTARGET_OBJS)"  ""
	-erase /Q /S /F $(DLLTARGET_OBJS)
endif
ifneq "$(WXPROGRAM1_OBJS)"  ""
	-erase /Q /S /F $(WXPROGRAM1_OBJS) $(WXPROGRAM1)
endif
ifneq "$(WXUNICODE_PROGRAM1_OBJS)"  ""
	-erase /Q /S /F $(WXUNICODE_PROGRAM1_OBJS) $(WXUNICODE_PROGRAM1)
endif
ifneq "$(WXPROGRAM2_OBJS)"  ""
      -erase /Q /S /F $(WXPROGRAM2_OBJS) $(WXPROGRAM2)
endif
ifneq "$(WXUNICODE_PROGRAM2_OBJS)"  ""
	-erase /Q /S /F $(WXUNICODE_PROGRAM2_OBJS) $(WXUNICODE_PROGRAM2)
endif
ifneq "$(WXUNICODE_PROGRAM3_OBJS)"  ""
	-erase /Q /S /F $(WXUNICODE_PROGRAM3_OBJS) $(WXUNICODE_PROGRAM3)
endif
ifneq "$(WXUNICODE_PROGRAM4_OBJS)"  ""
	-erase /Q /S /F $(WXUNICODE_PROGRAM4_OBJS) $(WXUNICODE_PROGRAM4)
endif


