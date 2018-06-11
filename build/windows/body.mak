#
# Header for all make files
#

CPP=cl.exe
RSC=rc.exe


#
# Debug/Non-debug 
#
!IF "$(DEBUG)" == "1"
OUTDIR=$(WIN32DIR)\Debug
OUTDIR_UNICODE = $(WIN32DIR)\DebugUnicode
INTDIR=$(WIN32DIR)\Debug$(INTDIR_SUFFIX)
INTDIR_UNICODE = $(WIN32DIR)\DebugUnicode$(INTDIR_SUFFIX)
INTDIR_UNICODE2 = $(WIN32DIR)\DebugUnicode2$(INTDIR_SUFFIX)
INTDIR_UNICODE3 = $(WIN32DIR)\DebugUnicode3$(INTDIR_SUFFIX)
INTDIR_UNICODE4 = $(WIN32DIR)\DebugUnicode4$(INTDIR_SUFFIX)
INTDIR_UNICODE5 = $(WIN32DIR)\DebugUnicode5$(INTDIR_SUFFIX)
INTDIR_UNICODE6 = $(WIN32DIR)\DebugUnicode6$(INTDIR_SUFFIX)
!ELSE
OUTDIR=$(WIN32DIR)\Release
OUTDIR_UNICODE = $(WIN32DIR)\ReleaseUnicode
INTDIR=$(WIN32DIR)\Release$(INTDIR_SUFFIX)
INTDIR_UNICODE = $(WIN32DIR)\ReleaseUnicode$(INTDIR_SUFFIX)
INTDIR_UNICODE2 = $(WIN32DIR)\ReleaseUnicode2$(INTDIR_SUFFIX)
INTDIR_UNICODE3 = $(WIN32DIR)\ReleaseUnicode3$(INTDIR_SUFFIX)
INTDIR_UNICODE4 = $(WIN32DIR)\ReleaseUnicode4$(INTDIR_SUFFIX)
INTDIR_UNICODE5 = $(WIN32DIR)\ReleaseUnicode5$(INTDIR_SUFFIX)
INTDIR_UNICODE6 = $(WIN32DIR)\ReleaseUnicode6$(INTDIR_SUFFIX)
!ENDIF

PROGRAMS = $(PROGRAM1) $(PROGRAM2) $(PROGRAM3) $(PROGRAM4) \
           $(PROGRAM5) $(PROGRAM6) $(PROGRAM7) $(PROGRAM8) \
           $(PROGRAM9) $(PROGRAM10) $(PROGRAM11) $(PROGRAM12) \
           $(PROGRAM12) $(PROGRAM13) $(PROGRAM14) $(PROGRAM15) \
           $(PROGRAM16) $(PROGRAM17) $(PROGRAM18) $(PROGRAM19) \
           $(WXUNICODE_PROGRAM1) \
           $(WXUNICODE_PROGRAM2) \
           $(WXUNICODE_PROGRAM3) \
           $(WXUNICODE_PROGRAM4) \
           $(WXUNICODE_PROGRAM5) \
           $(WXUNICODE_PROGRAM6) 


!IF "$(COMPILE_EMDROS)" == "1"


!IF "$(PGSQL)" == "1"

DBINCLUDE_PG = -I "$(PGTOP)\include"
DBLIB_PG = "$(PGTOP)\lib\libpq.lib" 
CBS_PGSQL = _pg
CPPFLAGS_PGSQL = /DUSE_POSTGRESQL=1

!ELSE

CPPFLAGS_PGSQL = /DUSE_POSTGRESQL=0

!ENDIF

!IF "$(MYSQL)" == "1"

DBINCLUDE_MY = /I "$(WIN32DIR)\mysql_include"
DBLIB_MY = "$(MYSQLTOP)\lib\libmysql.lib"
CBS_MYSQL = _my
CPPFLAGS_MYSQL = /DUSE_MYSQL=1

!ELSE
CPPFLAGS_MYSQL = /DUSE_MYSQL=0

!ENDIF

!IF "$(SQLITE3)" == "1"

DBINCLUDE_SQLITE3 = /I "$(WIN32DIR)"
DBLIB_SQLITE3 = "$(OUTDIR)\libsqlite3_emdros.lib" 
CBS_SQLITE3 = _sq3
CPPFLAGS_SQLITE3 = /DUSE_SQLITE3=1

!ELSE

CPPFLAGS_SQLITE3 = /DUSE_SQLITE3=0

!ENDIF

!IF "$(BPT)" == "1"
CBS_BPT = _bpt
CPPFLAGS_BPT = /DUSE_BPT=1

!ELSE

CPPFLAGS_BPT = /DUSE_BPT=0

!ENDIF

!IF "$(BPT2)" == "1"
CBS_BPT2 = _bpt2
CPPFLAGS_BPT2 = /DUSE_BPT2=1

!ELSE

CPPFLAGS_BPT2 = /DUSE_BPT2=0

!ENDIF



DBINCLUDE = $(DBINCLUDE_PG) $(DBINCLUDE_MY) \
            $(DBINCLUDE_SQLITE3)
 
DB_CPPFLAGS = $(CPPFLAGS_PGSQL) $(CPPFLAGS_MYSQL) $(CPPFLAGS_SQLITE3) $(CPPFLAGS_BPT) $(CPPFLAGS_BPT2) 

DBLIB = $(DBLIB_PG) $(DBLIB_MY) \
        $(DBLIB_SQLITE3)

CHARACTERISTIC_BACKEND_STRING = $(CBS_SQLITE3)$(CBS_PGSQL)$(CBS_MYSQL)$(CBS_BPT)$(CBS_BPT2)



#-----------------------------
#DEFAULT_BACKEND_ENUM
#-----------------------------
!IF "$(POSTGRESQL)" == "1"

DEFAULT_BACKEND_ENUM = kPostgreSQL

!ELSE
!IF "$(MYSQL)" == "1"

DEFAULT_BACKEND_ENUM = kMySQL
!ELSE
!IF "$(SQLITE3)" == "1"

DEFAULT_BACKEND_ENUM = kSQLite3
!ENDIF
!ENDIF
!ENDIF



CPPFLAGS_EXTRA = $(WINFLAGS) $(DB_CPPFLAGS) /EHsc


PCRELIB="$(OUTDIR)\libpcre2_emdros.lib"


!ENDIF   # "$(COMPILE_EMDROS)" == "1"





!IF "$(TARGET_CPU)" == "X86"
APPVER=6.0
!ELSE
APPVER=6.0
!ENDIF



!IF "$(WITH_WXWIN)" == "yes"

WXDIR=$(WXWIN_HOME)

#
# Find WXLIBDIR
#
!IF "$(DEBUG)" == "1"

!IF "$(TARGET_CPU)" == "X86"


!IF EXIST("$(WXDIR)\lib\vc_lib\wxmsw28ud.lib")
WXLIBDIR=$(WXDIR)\lib\vc_lib
WXVERSION=28
!ELSEIF EXIST("$(WXDIR)\lib\vc_lib\wxbase29ud.lib")
WXLIBDIR=$(WXDIR)\lib\vc_lib
WXVERSION=29
!ELSEIF EXIST("$(WXDIR)\lib\vc_lib\wxbase30ud.lib")
WXLIBDIR=$(WXDIR)\lib\vc_lib
WXVERSION=30
!ELSEIF EXIST("$(WXDIR)\lib\vc_lib\wxbase31ud.lib")
WXLIBDIR=$(WXDIR)\lib\vc_lib
WXVERSION=31
!ELSE
!ERROR Could not find wxWidgets library in $(WXDIR)
!ENDIF 

!ELSE

# TARGET_CPU == X64


!IF EXIST("$(WXDIR)\lib\vc_amd64_lib\wxmsw28ud.lib")
WXLIBDIR=$(WXDIR)\lib\vc_amd64_lib
WXVERSION=28
!ELSEIF EXIST("$(WXDIR)\lib\vc_x64_lib\wxbase29ud.lib")
WXLIBDIR=$(WXDIR)\lib\vc_x64_lib
WXVERSION=29
!ELSEIF EXIST("$(WXDIR)\lib\vc_x64_lib\wxbase30ud.lib")
WXLIBDIR=$(WXDIR)\lib\vc_x64_lib
WXVERSION=30
!ELSEIF EXIST("$(WXDIR)\lib\vc_x64_lib\wxbase31ud.lib")
WXLIBDIR=$(WXDIR)\lib\vc_x64_lib
WXVERSION=31
!ELSE
!ERROR Could not find wxWidgets library in $(WXDIR)
!ENDIF 

!ENDIF # TARGET_CPU = X86/X64


!ELSE
# Non-debug build

!IF "$(TARGET_CPU)" == "X86"

!IF EXIST("$(WXDIR)\lib\vc_lib\wxmsw28u.lib")
WXLIBDIR=$(WXDIR)\lib\vc_lib
WXVERSION=28
!ELSEIF EXIST("$(WXDIR)\lib\vc_lib\wxbase29u.lib")
WXLIBDIR=$(WXDIR)\lib\vc_lib
WXVERSION=29
!ELSEIF EXIST("$(WXDIR)\lib\vc_lib\wxbase30u.lib")
WXLIBDIR=$(WXDIR)\lib\vc_lib
WXVERSION=30
!ELSEIF EXIST("$(WXDIR)\lib\vc_lib\wxbase31u.lib")
WXLIBDIR=$(WXDIR)\lib\vc_lib
WXVERSION=31
!ELSE
!ERROR Could not find wxWidgets library in $(WXDIR)
!ENDIF 

!ELSE

# TARGET_CPU == X64


!IF EXIST("$(WXDIR)\lib\vc_amd64_lib\wxmsw28u.lib")
WXLIBDIR=$(WXDIR)\lib\vc_amd64_lib
WXVERSION=28

!ELSEIF EXIST("$(WXDIR)\lib\vc_x64_lib\wxbase29u.lib")
WXLIBDIR=$(WXDIR)\lib\vc_x64_lib
WXVERSION=29
!ELSEIF EXIST("$(WXDIR)\lib\vc_x64_lib\wxbase30u.lib")
WXLIBDIR=$(WXDIR)\lib\vc_x64_lib
WXVERSION=30
!ELSEIF EXIST("$(WXDIR)\lib\vc_x64_lib\wxbase31u.lib")
WXLIBDIR=$(WXDIR)\lib\vc_x64_lib
WXVERSION=31
!ELSE
!ERROR Could not find wxWidgets library in $(WXDIR)
!ENDIF 

!ENDIF # TARGET_CPU = X86/X64

!ENDIF # Debug/Non-debug build





#
# Find most wxWidgets variables
#

!IF "$(TARGET_CPU)" == "X86"
WINVERSION=-DWINVER=0x0601
WINFLAGS=/Gy -c -DWIN32 -D__WIN32__ \
         -D_WINDOWS $(WINVERSION) -DSTRICT
WINLINKFLAGS=/INCREMENTAL:NO -machine:i386 -subsystem:windows /MANIFEST
WINLIBS=kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib comctl32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib 

guilibsdll= 
WX_LIBS=/LIBPATH:$(WXLIBDIR) /LIBPATH:$(WXDIR)\contrib\lib\vc_lib 
!ELSE

WINVERSION=-DWINVER=0x0601
WINFLAGS=/Gy -c -DWIN64 -D__WIN64__ \
         -D_WINDOWS $(WINVERSION) -DSTRICT
WINLINKFLAGS=/INCREMENTAL:NO -machine:X64 -subsystem:windows /MANIFEST
WINLIBS=kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib comctl32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib 

guilibsdll= 
WX_LIBS=/LIBPATH:$(WXLIBDIR) /LIBPATH:$(WXDIR)\contrib\lib\vc_x64_lib

!ENDIF # TARGET_CPU == X86




#
# Find WX_LIBS_UNICODE and WX_DEBUG_SENSITIVE_UNICODE_CPPFLAGS
#
!IF "$(DEBUG)" == "1"
WX_DEBUG_SENSITIVE_UNICODE_CPPFLAGS = -I$(WXLIBDIR)\mswud

WX_LINKFLAGS=/DEBUG

!IF "$(WXVERSION)" == "28"
WX_LIBS_UNICODE=$(WX_LIBS) \
   $(WXLIBDIR)\wxregexud.lib \
   $(WXLIBDIR)\wxmsw28ud.lib \
   $(WXLIBDIR)\wxzlibd.lib $(WXLIBDIR)\wxjpegd.lib $(WXLIBDIR)\wxpngd.lib $(WXLIBDIR)\wxtiffd.lib   

!ELSEIF "$(WXVERSION)" == "29"
WX_LIBS_UNICODE=$(WX_LIBS) \
   wxbase29ud.lib wxmsw29ud_gl.lib wxmsw29ud_richtext.lib \
   wxbase29ud_net.lib wxmsw29ud_html.lib wxmsw29ud_stc.lib \
   wxbase29ud_xml.lib wxmsw29ud_media.lib wxmsw29ud_webview.lib \
   wxmsw29ud_adv.lib wxmsw29ud_propgrid.lib wxmsw29ud_xrc.lib \
   wxmsw29ud_aui.lib wxmsw29ud_qa.lib wxregexud.lib \
   wxmsw29ud_core.lib wxmsw29ud_ribbon.lib \
   wxzlibd.lib wxjpegd.lib wxpngd.lib wxtiffd.lib   
   
!ELSEIF "$(WXVERSION)" == "30"
WX_LIBS_UNICODE=$(WX_LIBS) \
   wxbase30ud.lib wxmsw30ud_gl.lib wxmsw30ud_richtext.lib \
   wxbase30ud_net.lib wxmsw30ud_html.lib wxmsw30ud_stc.lib \
   wxbase30ud_xml.lib wxmsw30ud_media.lib wxmsw30ud_webview.lib \
   wxmsw30ud_adv.lib wxmsw30ud_propgrid.lib wxmsw30ud_xrc.lib \
   wxmsw30ud_aui.lib wxmsw30ud_qa.lib wxregexud.lib \
   wxmsw30ud_core.lib wxmsw30ud_ribbon.lib \
   wxzlibd.lib wxjpegd.lib wxpngd.lib wxtiffd.lib   

!ELSEIF "$(WXVERSION)" == "31"
WX_LIBS_UNICODE=$(WX_LIBS) \
   wxbase31ud.lib wxmsw31ud_gl.lib wxmsw31ud_richtext.lib \
   wxbase31ud_net.lib wxmsw31ud_html.lib wxmsw31ud_stc.lib \
   wxbase31ud_xml.lib wxmsw31ud_media.lib wxmsw31ud_webview.lib \
   wxmsw31ud_adv.lib wxmsw31ud_propgrid.lib wxmsw31ud_xrc.lib \
   wxmsw31ud_aui.lib wxmsw31ud_qa.lib wxregexud.lib \
   wxmsw31ud_core.lib wxmsw31ud_ribbon.lib \
   wxzlibd.lib wxjpegd.lib wxpngd.lib wxtiffd.lib
   
!ELSE
!ERROR Unknown WXVERSION value: $(WXVERSION)
!ENDIF

!ELSE
# Non-debug build

WX_DEBUG_SENSITIVE_UNICODE_CPPFLAGS = -I$(WXLIBDIR)\mswu
WX_LINKFLAGS=/RELEASE

!IF "$(WXVERSION)" == "28"
WX_LIBS_UNICODE=$(WX_LIBS) \
   $(WXLIBDIR)\wxregexu.lib \
   $(WXLIBDIR)\wxmsw28u.lib \
   wxzlib.lib wxjpeg.lib wxpng.lib wxtiff.lib

!ELSEIF "$(WXVERSION)" == "29"
WX_LIBS_UNICODE=$(WX_LIBS) \
   wxbase29u.lib wxmsw29u_gl.lib wxmsw29u_richtext.lib \
   wxbase29u_net.lib wxmsw29u_html.lib wxmsw29u_stc.lib \
   wxbase29u_xml.lib wxmsw29u_media.lib wxmsw29u_webview.lib \
   wxmsw29u_adv.lib wxmsw29u_propgrid.lib wxmsw29u_xrc.lib \
   wxmsw29u_aui.lib wxmsw29u_qa.lib wxregexu.lib \
   wxmsw29u_core.lib wxmsw29u_ribbon.lib \
   wxzlib.lib wxjpeg.lib wxpng.lib wxtiff.lib
   
!ELSEIF "$(WXVERSION)" == "30"
WX_LIBS_UNICODE=$(WX_LIBS) \
   wxbase30u.lib wxmsw30u_gl.lib wxmsw30u_richtext.lib \
   wxbase30u_net.lib wxmsw30u_html.lib wxmsw30u_stc.lib \
   wxbase30u_xml.lib wxmsw30u_media.lib wxmsw30u_webview.lib \
   wxmsw30u_adv.lib wxmsw30u_propgrid.lib wxmsw30u_xrc.lib \
   wxmsw30u_aui.lib wxmsw30u_qa.lib wxregexu.lib \
   wxmsw30u_core.lib wxmsw30u_ribbon.lib \
   wxzlib.lib wxjpeg.lib wxpng.lib wxtiff.lib

!ELSEIF "$(WXVERSION)" == "31"
WX_LIBS_UNICODE=$(WX_LIBS) \
   wxbase31u.lib wxmsw31u_gl.lib wxmsw31u_richtext.lib \
   wxbase31u_net.lib wxmsw31u_html.lib wxmsw31u_stc.lib \
   wxbase31u_xml.lib wxmsw31u_media.lib wxmsw31u_webview.lib \
   wxmsw31u_adv.lib wxmsw31u_propgrid.lib wxmsw31u_xrc.lib \
   wxmsw31u_aui.lib wxmsw31u_qa.lib wxregexu.lib \
   wxmsw31u_core.lib wxmsw31u_ribbon.lib \
   wxzlib.lib wxjpeg.lib wxpng.lib wxtiff.lib

!ELSE
!ERROR Unknown WXVERSION value: $(WXVERSION)
!ENDIF


# Debug/Non-debug build
!ENDIF



WX_LINKFLAGS_UNICODE = $(WX_LINKFLAGS)

EXTRADLLFLAGS=
CRTFLAG=
OVERRIDEFLAGS=
WXINC=$(WXDIR)\include
INC=-I$(WXINC) -I$(WXDIR)/contrib/include #-I$(WXDIR)/src/regex -I$(WXDIR)/src/png -I$(WXDIR)/src/zlib -I$(WXDIR)/src/jpeg -I$(WXDIR)/src/tiff
WX_CPPFLAGS=$(WINFLAGS) /DwxUSE_NO_MANIFEST=1 /D__WXMSW__ $(INC) $(OPT) $(EXTRADLLFLAGS)  $(CRTFLAG) /D__WINDOWS__ $(OVERRIDEFLAGS) /EHsc
WX_CPPFLAGS_UNICODE = $(WX_CPPFLAGS) \
                      $(WX_DEBUG_SENSITIVE_UNICODE_CPPFLAGS) \
                      /DwxUSE_UNICODE=1
!ENDIF 


ALL: $(OUTDIR) $(INTDIR) $(OUTDIR_UNICODE) $(INTDIR_UNICODE) $(INTDIR_UNICODE2) $(INTDIR_UNICODE3) $(INTDIR_UNICODE4) $(INTDIR_UNICODE5) $(INTDIR_UNICODE6) $(TARGETS)


!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR_UNICODE)" :
    if not exist "$(OUTDIR_UNICODE)/$(NULL)" mkdir "$(OUTDIR_UNICODE)"

!IF "$(INTDIR)" != "$(OUTDIR)"
"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"
!ENDIF

!IF "$(INTDIR_UNICODE)" != "$(OUTDIR_UNICODE)"
"$(INTDIR_UNICODE)" :
    if not exist "$(INTDIR_UNICODE)/$(NULL)" mkdir "$(INTDIR_UNICODE)"
!ENDIF

!IF "$(INTDIR_UNICODE2)" != "$(OUTDIR_UNICODE2)"
"$(INTDIR_UNICODE2)" :
    if not exist "$(INTDIR_UNICODE2)/$(NULL)" mkdir "$(INTDIR_UNICODE2)"
!ENDIF

!IF "$(INTDIR_UNICODE3)" != "$(OUTDIR_UNICODE3)"
"$(INTDIR_UNICODE3)" :
    if not exist "$(INTDIR_UNICODE3)/$(NULL)" mkdir "$(INTDIR_UNICODE3)"
!ENDIF

!IF "$(INTDIR_UNICODE4)" != "$(OUTDIR_UNICODE4)"
"$(INTDIR_UNICODE4)" :
    if not exist "$(INTDIR_UNICODE4)/$(NULL)" mkdir "$(INTDIR_UNICODE4)"
!ENDIF


!IF "$(INTDIR_UNICODE5)" != "$(OUTDIR_UNICODE5)"
"$(INTDIR_UNICODE5)" :
    if not exist "$(INTDIR_UNICODE5)/$(NULL)" mkdir "$(INTDIR_UNICODE5)"
!ENDIF


!IF "$(INTDIR_UNICODE6)" != "$(OUTDIR_UNICODE6)"
"$(INTDIR_UNICODE6)" :
    if not exist "$(INTDIR_UNICODE6)/$(NULL)" mkdir "$(INTDIR_UNICODE6)"
!ENDIF




!IF "$(DEBUGINFO)" == "1"
!IF "$(TARGETNAME)" == ""
DEBUGINFO_FLAGS=
DEBUGINFO_FLAGS_UNICODE=
!ELSE
DEBUGINFO_FLAGS=/Zi /Fd"$(OUTDIR)\$(TARGETNAME)"
DEBUGINFO_FLAGS_UNICODE=/Zi /Fd"$(OUTDIR_UNICODE)\$(TARGETNAME)"
!ENDIF
!ENDIF

!IF "$(DEBUG)" == "1"
# DEBUG build
DEBUG_SENSITIVE_CPPFLAGS = /MTd /W4 /Od /Zi \
   /D"_DEBUG" /D"__STL_DEBUG" /D_ITERATOR_DEBUG_LEVEL=2 /D_HAS_ITERATOR_DEBUGGING=1 /D_SECURE_SCL=1 /D_CRT_SECURE_NO_WARNINGS=1  /D_SCL_SECURE_NO_WARNINGS=1

!ELSE

# non-debug build
DEBUG_SENSITIVE_CPPFLAGS = /MT /W4 /O2 /D"NDEBUG=1"   /D_CRT_SECURE_NO_WARNINGS=1  /D_SCL_SECURE_NO_WARNINGS=1
!ENDIF


CPP_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 /I "$(WIN32DIR)\..\..\include" \
 $(DBINCLUDE) \
 /I "$(WIN32DIR)\..\.." \
 /D "WIN32" /D "_WINDOWS" \
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" \
 /c  /D "HAVE_VSNPRINTF" /D "HAVE_STRDUP" \
 $(LOCAL_CPPFLAGS) $(DEBUGINFO_FLAGS) 


CPP_UNICODE_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 /I "." /I "$(WIN32DIR)\..\..\include" \
 $(DBINCLUDE) \
 /I "$(WIN32DIR)\..\.." \
 /D "WIN32" /D "_WINDOWS" /Y- /GR \
 /Fo"$(INTDIR_UNICODE)/" /Fd"$(INTDIR_UNICODE)/" \
 /c  /D "HAVE_VSNPRINTF" /D "HAVE_STRDUP" \
 $(LOCAL_UNICODE_CPPFLAGS) $(DEBUGINFO_FLAGS_UNICODE) 


CPP_UNICODE2_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 /I "." /I "$(WIN32DIR)\..\..\include" \
 $(DBINCLUDE) \
 /I "$(WIN32DIR)\..\.." \
 /D "WIN32" /D "_WINDOWS" /Yc /GR \
 /Fo"$(INTDIR_UNICODE2)/" /Fd"$(INTDIR_UNICODE2)/" \
 /c  /D "HAVE_VSNPRINTF" /D "HAVE_STRDUP" \
 $(LOCAL_UNICODE2_CPPFLAGS) $(DEBUGINFO_FLAGS_UNICODE) 

CPP_UNICODE3_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 /I "." /I "$(WIN32DIR)\..\..\include" \
 $(DBINCLUDE) \
 /I "$(WIN32DIR)\..\.." \
 /D "WIN32" /D "_WINDOWS" /Yc /GR \
 /Fo"$(INTDIR_UNICODE3)/" /Fd"$(INTDIR_UNICODE3)/" \
 /c  /D "HAVE_VSNPRINTF" /D "HAVE_STRDUP" \
 $(LOCAL_UNICODE3_CPPFLAGS) $(DEBUGINFO_FLAGS) 

CPP_UNICODE4_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 /I "." /I "$(WIN32DIR)\..\..\include" \
 $(DBINCLUDE) \
 /I "$(WIN32DIR)\..\.." \
 /D "WIN32" /D "_WINDOWS" /Yc /GR \
 /Fo"$(INTDIR_UNICODE4)/" /Fd"$(INTDIR_UNICODE4)/" \
 /c  /D "HAVE_VSNPRINTF" /D "HAVE_STRDUP" \
 $(LOCAL_UNICODE4_CPPFLAGS) $(DEBUGINFO_FLAGS) 

CPP_UNICODE5_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 /I "." /I "$(WIN32DIR)\..\..\include" \
 $(DBINCLUDE) \
 /I "$(WIN32DIR)\..\.." \
 /D "WIN32" /D "_WINDOWS" /Yc /GR \
 /Fo"$(INTDIR_UNICODE5)/" /Fd"$(INTDIR_UNICODE5)/" \
 /c  /D "HAVE_VSNPRINTF" /D "HAVE_STRDUP" \
 $(LOCAL_UNICODE5_CPPFLAGS) $(DEBUGINFO_FLAGS) 


CPP_UNICODE6_PROJ=$(CPPFLAGS_EXTRA) $(DEBUG_SENSITIVE_CPPFLAGS) \
 /I "." /I "$(WIN32DIR)\..\..\include" \
 $(DBINCLUDE) \
 /I "$(WIN32DIR)\..\.." \
 /D "WIN32" /D "_WINDOWS" /Yc /GR \
 /Fo"$(INTDIR_UNICODE6)/" /Fd"$(INTDIR_UNICODE6)/" \
 /c  /D "HAVE_VSNPRINTF" /D "HAVE_STRDUP" \
 $(LOCAL_UNICODE6_CPPFLAGS) $(DEBUGINFO_FLAGS) 




CPP_OBJS=$(INTDIR)
CPP_UNICODE_OBJS=$(INTDIR_UNICODE)
CPP_UNICODE2_OBJS=$(INTDIR_UNICODE2)
CPP_UNICODE3_OBJS=$(INTDIR_UNICODE3)
CPP_UNICODE4_OBJS=$(INTDIR_UNICODE4)
CPP_UNICODE5_OBJS=$(INTDIR_UNICODE5)
CPP_UNICODE6_OBJS=$(INTDIR_UNICODE6)
CPP_SBRS=.

LIB32=link.exe -lib
LIB32_FLAGS= $(LIBFLAGS)

DLL32=link.exe /DLL
DLL32_FLAGS= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib \
 wsock32.lib odbccp32.lib $(LIBFLAGS) $(LOCAL_LIBFLAGS)

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib \
 wsock32.lib odbccp32.lib /subsystem:console $(LIBFLAGS) \
 $(LOCAL_LIBFLAGS)

LINK32_WXFLAGS_UNICODE = $(WINLINKFLAGS) \
   $(WINLIBS) \
   $(guilibsdll) \
   $(WX_LIBS_UNICODE) \
   $(WX_LINKFLAGS_UNICODE) \
   $(LINK32_WXFLAGS_UNICODE_LOCALFLAGS)


!IF "$(LIBTARGET)" != ""
"$(OUTDIR)\$(LIBTARGET)" : $(LIBTARGET_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) /out:"$(OUTDIR)\$(LIBTARGET)" $(LIBTARGET_OBJS)
<<
!ENDIF


!IF "$(LIB_WX_UNICODE_TARGET)" != ""
"$(OUTDIR_UNICODE)\$(LIB_WX_UNICODE_TARGET)" : $(LIB_WX_UNICODE_TARGET_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) /out:"$(OUTDIR_UNICODE)\$(LIB_WX_UNICODE_TARGET)" $(LIB_WX_UNICODE_TARGET_OBJS)
<<
!ENDIF


!IF "$(DLLTARGET)" != ""
"$(OUTDIR)\$(DLLTARGET)" : $(DLLTARGET_OBJS)
    $(DLL32) @<<
  $(DLL32_FLAGS) /out:"$(OUTDIR)\$(DLLTARGET)" $(DLLTARGET_OBJS)
<<
!ENDIF


!IF "$(PROGRAM1)" != ""
$(PROGRAM1) : $(PROGRAM1_OBJS) $(PROGRAM1_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM1) $(PROGRAM1_OBJS)
<<
!ENDIF

    
!IF "$(PROGRAM2)" != ""
$(PROGRAM2) : $(PROGRAM2_OBJS) $(PROGRAM2_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM2) $(PROGRAM2_OBJS)
<<
!ENDIF

    
!IF "$(PROGRAM3)" != ""
$(PROGRAM3) : $(PROGRAM3_OBJS) $(PROGRAM3_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM3) $(PROGRAM3_OBJS)
<<
!ENDIF

    
!IF "$(PROGRAM4)" != ""
$(PROGRAM4) : $(PROGRAM4_OBJS) $(PROGRAM4_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM4) $(PROGRAM4_OBJS)
<<
!ENDIF

!IF "$(PROGRAM5)" != ""
$(PROGRAM5) : $(PROGRAM5_OBJS) $(PROGRAM5_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM5) $(PROGRAM5_OBJS)
<<
!ENDIF

!IF "$(PROGRAM6)" != ""
$(PROGRAM6) : $(PROGRAM6_OBJS) $(PROGRAM6_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM6) $(PROGRAM6_OBJS)
<<
!ENDIF

!IF "$(PROGRAM7)" != ""
$(PROGRAM7) : $(PROGRAM7_OBJS) $(PROGRAM7_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM7) $(PROGRAM7_OBJS)
<<
!ENDIF

!IF "$(PROGRAM8)" != ""
$(PROGRAM8) : $(PROGRAM8_OBJS) $(PROGRAM8_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM8) $(PROGRAM8_OBJS)
<<
!ENDIF

!IF "$(PROGRAM9)" != ""
$(PROGRAM9) : $(PROGRAM9_OBJS) $(PROGRAM9_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM9) $(PROGRAM9_OBJS)
<<
!ENDIF

!IF "$(PROGRAM10)" != ""
$(PROGRAM10) : $(PROGRAM10_OBJS) $(PROGRAM10_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM10) $(PROGRAM10_OBJS)
<<
!ENDIF

!IF "$(PROGRAM11)" != ""
$(PROGRAM11) : $(PROGRAM11_OBJS) $(PROGRAM11_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM11) $(PROGRAM11_OBJS)
<<
!ENDIF

!IF "$(PROGRAM12)" != ""
$(PROGRAM12) : $(PROGRAM12_OBJS) $(PROGRAM12_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM12) $(PROGRAM12_OBJS)
<<
!ENDIF

!IF "$(PROGRAM13)" != ""
$(PROGRAM13) : $(PROGRAM13_OBJS) $(PROGRAM13_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM13) $(PROGRAM13_OBJS)
<<
!ENDIF

!IF "$(PROGRAM14)" != ""
$(PROGRAM14) : $(PROGRAM14_OBJS) $(PROGRAM14_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM14) $(PROGRAM14_OBJS)
<<
!ENDIF

!IF "$(PROGRAM15)" != ""
$(PROGRAM15) : $(PROGRAM15_OBJS) $(PROGRAM15_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM15) $(PROGRAM15_OBJS)
<<
!ENDIF

!IF "$(PROGRAM16)" != ""
$(PROGRAM16) : $(PROGRAM16_OBJS) $(PROGRAM16_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM16) $(PROGRAM16_OBJS)
<<
!ENDIF

!IF "$(PROGRAM17)" != ""
$(PROGRAM17) : $(PROGRAM17_OBJS) $(PROGRAM17_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM17) $(PROGRAM17_OBJS)
<<
!ENDIF

!IF "$(PROGRAM18)" != ""
$(PROGRAM18) : $(PROGRAM18_OBJS) $(PROGRAM18_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM18) $(PROGRAM18_OBJS)
<<
!ENDIF

!IF "$(PROGRAM19)" != ""
$(PROGRAM19) : $(PROGRAM19_OBJS) $(PROGRAM19_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_FLAGS) /out:$(PROGRAM19) $(PROGRAM19_OBJS)
<<
!ENDIF




!IF "$(WXUNICODE_PROGRAM1)" != ""
$(WXUNICODE_PROGRAM1) : $(WXUNICODE_PROGRAM1_OBJS) $(WXUNICODE_PROGRAM1_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_WXFLAGS_UNICODE) /pdb:$(WXUNICODE_PROGRAM1:.exe=.pdb) /out:$(WXUNICODE_PROGRAM1)  $(WXUNICODE_PROGRAM1_OBJS) $(WXUNICODE_PROGRAM1_DEPENDENCIES)
<<
!ENDIF

!IF "$(WXUNICODE_PROGRAM2)" != ""
$(WXUNICODE_PROGRAM2) : $(WXUNICODE_PROGRAM2_OBJS) $(WXUNICODE_PROGRAM2_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_WXFLAGS_UNICODE) /pdb:$(WXUNICODE_PROGRAM2:.exe=.pdb) /out:$(WXUNICODE_PROGRAM2)  $(WXUNICODE_PROGRAM2_OBJS) $(WXUNICODE_PROGRAM2_DEPENDENCIES)
<<
!ENDIF

!IF "$(WXUNICODE_PROGRAM3)" != ""
$(WXUNICODE_PROGRAM3) : $(WXUNICODE_PROGRAM3_OBJS) $(WXUNICODE_PROGRAM3_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_WXFLAGS_UNICODE) /pdb:$(WXUNICODE_PROGRAM3:.exe=.pdb) /out:$(WXUNICODE_PROGRAM3)  $(WXUNICODE_PROGRAM3_OBJS) $(WXUNICODE_PROGRAM3_DEPENDENCIES)
<<
!ENDIF

!IF "$(WXUNICODE_PROGRAM4)" != ""
$(WXUNICODE_PROGRAM4) : $(WXUNICODE_PROGRAM4_OBJS) $(WXUNICODE_PROGRAM4_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_WXFLAGS_UNICODE) /pdb:$(WXUNICODE_PROGRAM4:.exe=.pdb) /out:$(WXUNICODE_PROGRAM4)  $(WXUNICODE_PROGRAM4_OBJS) $(WXUNICODE_PROGRAM4_DEPENDENCIES)
<<
!ENDIF

!IF "$(WXUNICODE_PROGRAM5)" != ""
$(WXUNICODE_PROGRAM5) : $(WXUNICODE_PROGRAM5_OBJS) $(WXUNICODE_PROGRAM5_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_WXFLAGS_UNICODE) /pdb:$(WXUNICODE_PROGRAM5:.exe=.pdb) /out:$(WXUNICODE_PROGRAM5)  $(WXUNICODE_PROGRAM5_OBJS) $(WXUNICODE_PROGRAM5_DEPENDENCIES)
<<
!ENDIF

!IF "$(WXUNICODE_PROGRAM6)" != ""
$(WXUNICODE_PROGRAM6) : $(WXUNICODE_PROGRAM6_OBJS) $(WXUNICODE_PROGRAM6_DEPENDENCIES)
    $(LINK32) @<<
  $(LINK32_WXFLAGS_UNICODE) /pdb:$(WXUNICODE_PROGRAM6:.exe=.pdb) /out:$(WXUNICODE_PROGRAM6)  $(WXUNICODE_PROGRAM6_OBJS) $(WXUNICODE_PROGRAM6_DEPENDENCIES)
<<
!ENDIF


    

!IF "$(NO_C_IN_THIS_DIRECTORY)" != "1"
.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!ENDIF

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $(CPP_PROJ_EXTRA)  $< 
<<

.c{$(CPP_UNICODE_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_UNICODE_PROJ) $(CPP_PROJ_EXTRA) $< 
<<

.cpp{$(CPP_UNICODE_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_UNICODE_PROJ) $(CPP_PROJ_EXTRA) $< 
<<

.cc{$(CPP_UNICODE_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_UNICODE_PROJ) $(CPP_PROJ_EXTRA) $< 
<<

.cpp{$(CPP_UNICODE2_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_UNICODE2_PROJ) $(CPP_PROJ_EXTRA) $< 
<<

.cpp{$(CPP_UNICODE3_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_UNICODE3_PROJ) $(CPP_PROJ_EXTRA) $< 
<<

.cpp{$(CPP_UNICODE4_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_UNICODE4_PROJ) $(CPP_PROJ_EXTRA) $< 
<<

.cpp{$(CPP_UNICODE5_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_UNICODE5_PROJ) $(CPP_PROJ_EXTRA) $< 
<<

.cpp{$(CPP_UNICODE6_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_UNICODE6_PROJ) $(CPP_PROJ_EXTRA) $< 
<<



.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


CLEAN :
	-FOR %F in ($(PROGRAMS)) DO erase /Q /S /F %F%
!IF "$(CLEANFILES)" != ""
	-FOR %F in ($(CLEANFILES)) DO erase /Q /S /F %F%
!ENDIF
!IF "$(LIBTARGET)" != ""
	-@erase "$(OUTDIR)\$(LIBTARGET)"
!ENDIF
	-FOR %F in ($(LIBTARGET_OBJS)) DO erase /Q /S /F %F%
!IF "$(LIB_WX_UNICODE_TARGET)" != ""
	-@erase "$(OUTDIR_UNICODE)\$(LIB_WX_UNICODE_TARGET)"
!ENDIF
!IF "$(DLLTARGET)" != ""
	-@erase "$(OUTDIR)\$(DLLTARGET)"
!ENDIF
	-FOR %F in ($(DLLTARGET_OBJS)) DO erase /Q /S /F %F%
	-FOR %F in ($(LIB_WX_UNICODE_TARGET_OBJS)) DO erase /Q /S /F %F%
	-FOR %F in ($(WXPROGRAM1_OBJS) $(WXPROGRAM1)) DO erase /Q /S /F %F%
	-FOR %F in ($(WXUNICODE_PROGRAM1_OBJS) $(WXUNICODE_PROGRAM1)) DO erase /Q /S /F %F%
	-FOR %F in ($(WXPROGRAM2_OBJS) $(WXPROGRAM2)) DO erase /Q /S /F %F%
	-FOR %F in ($(WXUNICODE_PROGRAM2_OBJS) $(WXUNICODE_PROGRAM2)) DO erase /Q /S /F %F%
	-FOR %F in ($(WXUNICODE_PROGRAM3_OBJS) $(WXUNICODE_PROGRAM3)) DO erase /Q /S /F %F%
	-FOR %F in ($(WXUNICODE_PROGRAM4_OBJS) $(WXUNICODE_PROGRAM4)) DO erase /Q /S /F %F%
	-FOR %F in ($(WXUNICODE_PROGRAM5_OBJS) $(WXUNICODE_PROGRAM5)) DO erase /Q /S /F %F%
	-FOR %F in ($(WXUNICODE_PROGRAM6_OBJS) $(WXUNICODE_PROGRAM6)) DO erase /Q /S /F %F%


