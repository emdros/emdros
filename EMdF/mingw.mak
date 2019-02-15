#
# Emdros Win32 Makefile for EMdF directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)
#
# Will build a MinGW


#
# Custom macros
#
WIN32DIR = ..\win32

include $(WIN32DIR)\config.mak


LIBTARGET = libemdf.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
CLEANFILES = 

ifeq ($(SQLITE3_NO_SYNC),1)
DEFINE_SQLITE3_EMDFDB_NO_FSYNC = -DSQLITE3_EMDFDB_NO_FSYNC
else
DEFINE_SQLITE3_EMDFDB_NO_FSYNC = 
endif

LOCAL_CPPFLAGS = $(DEFINE_SQLITE3_EMDFDB_NO_FSYNC) 



LIBPGEMDF_OBJS= \
	pgconn.o

LIBMYEMDF_OBJS= \
	mysqlconn.o

LIBSQLITE3EMDF_OBJS= \
	sqlite3conn.o

ifeq ($(PGSQL),1)
LIB_EXTRA_OBJS_PG = $(LIBPGEMDF_OBJS)
endif
ifeq ($(MYSQL),1)
LIB_EXTRA_OBJS_MY = $(LIBMYEMDF_OBJS)
endif
ifeq ($(SQLITE3),1)
LIB_EXTRA_OBJS_SQLITE3 = $(LIBSQLITE3EMDF_OBJS)
endif

LIB_EXTRA_OBJS = $(LIB_EXTRA_OBJS_PG) \
                 $(LIB_EXTRA_OBJS_MY) \
                 $(LIB_EXTRA_OBJS_SQLITE3)

LIBTARGET_OBJS= \
	arena.o \
	conn.o \
	emdfdb.o \
	pgemdfdb.o \
	mysqlemdfdb.o \
	sqlite3emdfdb.o \
        crc32_emdros.o \
        emdf_primes.o \
        emdf_ffeatures.o \
	emdf_hash.o \
        emdf_output.o \
        emdf_value.o \
        encryption.o \
        enum_const_cache.o \
	infos.o \
	inst.o \
	inst_object.o \
	logging.o \
	minidom.o \
	monads.o \
	qdxml.o \
	emdf_enums.o \
	string_func.o \
	string2som_map.o \
	string_list.o \
	string_set_cache.o \
	table.o \
	utils.o \
	json_classes.o \
	json_parser.o \
	json_lexer.o \
	lzma_emdros.o \
	bptemdfdb.o \
        $(LIB_EXTRA_OBJS)




include $(WIN32DIR)\mingw-body.mak

