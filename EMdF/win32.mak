#
# Emdros Win32 Makefile for EMdF directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)
#
# Will build a Win32 library libemdf.lib


#
# Custom macros
#
WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak


LIBTARGET = libemdf.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
CLEANFILES = 

!IF "$(SQLITE3_NO_SYNC)" == "1"
DEFINE_SQLITE3_EMDFDB_NO_FSYNC = /D "SQLITE3_EMDFDB_NO_FSYNC"
!ELSE
DEFINE_SQLITE3_EMDFDB_NO_FSYNC = 
!ENDIF

LOCAL_CPPFLAGS = $(DEFINE_SQLITE3_EMDFDB_NO_FSYNC) 


LIBPGEMDF_OBJS= \
	"$(INTDIR)\pgconn.obj"

LIBMYEMDF_OBJS= \
	"$(INTDIR)\mysqlconn.obj"

LIBSQLITE3EMDF_OBJS= \
	"$(INTDIR)\sqlite3conn.obj"

!IF "$(PGSQL)" == "1"
LIB_EXTRA_OBJS_PG = $(LIBPGEMDF_OBJS)
!ENDIF
!IF "$(MYSQL)" == "1"
LIB_EXTRA_OBJS_MY = $(LIBMYEMDF_OBJS)
!ENDIF
!IF "$(SQLITE3)" == "1"
LIB_EXTRA_OBJS_SQLITE3 = $(LIBSQLITE3EMDF_OBJS)
!ENDIF

LIB_EXTRA_OBJS = $(LIB_EXTRA_OBJS_PG) \
                 $(LIB_EXTRA_OBJS_MY) \
                 $(LIB_EXTRA_OBJS_SQLITE3)

LIBTARGET_OBJS= \
	"$(INTDIR)\arena.obj" \
	"$(INTDIR)\conn.obj" \
	"$(INTDIR)\emdfdb.obj" \
	"$(INTDIR)\pgemdfdb.obj" \
	"$(INTDIR)\mysqlemdfdb.obj" \
	"$(INTDIR)\sqlite3emdfdb.obj" \
        "$(INTDIR)\crc32_emdros.obj" \
        "$(INTDIR)\emdf_primes.obj" \
        "$(INTDIR)\emdf_ffeatures.obj" \
	"$(INTDIR)\emdf_hash.obj" \
        "$(INTDIR)\emdf_output.obj" \
        "$(INTDIR)\emdf_value.obj" \
        "$(INTDIR)\encryption.obj" \
        "$(INTDIR)\enum_const_cache.obj" \
	"$(INTDIR)\infos.obj" \
	"$(INTDIR)\inst.obj" \
	"$(INTDIR)\inst_object.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\minidom.obj" \
	"$(INTDIR)\monads.obj" \
	"$(INTDIR)\qdxml.obj" \
	"$(INTDIR)\emdf_enums.obj" \
	"$(INTDIR)\string_func.obj" \
	"$(INTDIR)\string2som_map.obj" \
	"$(INTDIR)\string_list.obj" \
	"$(INTDIR)\string_set_cache.obj" \
	"$(INTDIR)\table.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\json_classes.obj" \
	"$(INTDIR)\json_parser.obj" \
	"$(INTDIR)\json_lexer.obj" \
	"$(INTDIR)\lzma_emdros.obj" \
	"$(INTDIR)\bptemdfdb.obj" \
	"$(INTDIR)\bpt2emdfdb.obj" \
        $(LIB_EXTRA_OBJS)




!INCLUDE $(WIN32DIR)\body.mak

