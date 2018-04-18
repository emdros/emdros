LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := emdros
LOCAL_SRC_FILES := \
        emdros/sqlite3/src/sqlite3_localamalgam.c \
        emdros/pcre/pcre_byte_order.c \
        emdros/pcre/pcre_chartables.c \
        emdros/pcre/pcre_compile.c \
        emdros/pcre/pcre_config.c \
        emdros/pcre/pcre_dfa_exec.c \
        emdros/pcre/pcre_exec.c \
        emdros/pcre/pcre_fullinfo.c \
        emdros/pcre/pcre_get.c \
        emdros/pcre/pcre_globals.c \
        emdros/pcre/pcre_jit_compile.c \
        emdros/pcre/pcre_maketables.c \
        emdros/pcre/pcre_newline.c \
        emdros/pcre/pcre_ord2utf8.c \
        emdros/pcre/pcre_refcount.c \
        emdros/pcre/pcre_string_utils.c \
        emdros/pcre/pcre_study.c \
        emdros/pcre/pcre_tables.c \
        emdros/pcre/pcre_ucd.c \
        emdros/pcre/pcre_valid_utf8.c \
        emdros/pcre/pcre_version.c \
        emdros/pcre/pcre_xclass.c \
        emdros/EMdF/arena.cpp \
        emdros/EMdF/conn.cpp \
        emdros/EMdF/emdf_primes.cpp \
        emdros/EMdF/bptemdfdb.cpp \
        emdros/EMdF/bpt2emdfdb.cpp \
        emdros/EMdF/emdfdb.cpp \
        emdros/EMdF/pgemdfdb.cpp \
        emdros/EMdF/mysqlemdfdb.cpp \
        emdros/EMdF/sqliteemdfdb.cpp \
        emdros/EMdF/sqlite3emdfdb.cpp \
        emdros/EMdF/sqlite3conn.cpp \
        emdros/EMdF/crc32_emdros.cpp \
        emdros/EMdF/emdf_ffeatures.cpp \
        emdros/EMdF/emdf_hash.cpp \
        emdros/EMdF/emdf_output.cpp \
        emdros/EMdF/emdf_value.cpp \
        emdros/EMdF/encryption.cpp \
        emdros/EMdF/enum_const_cache.cpp \
        emdros/EMdF/infos.cpp \
        emdros/EMdF/inst.cpp \
        emdros/EMdF/inst_object.cpp \
        emdros/EMdF/logging.cpp \
        emdros/EMdF/minidom.cpp \
        emdros/EMdF/monads.cpp \
        emdros/EMdF/qdxml.cpp \
        emdros/EMdF/string_func.cpp \
        emdros/EMdF/string2som_map.cpp \
        emdros/EMdF/string_list.cpp \
        emdros/EMdF/bpa.cpp \
        emdros/EMdF/string_set_cache.cpp \
        emdros/EMdF/table.cpp \
        emdros/EMdF/utils.cpp \
        emdros/EMdF/json_classes.cpp \
        emdros/EMdF/json_parser.cpp \
        emdros/EMdF/json_lexer.cpp \
        emdros/MQL/mql_yylex.cpp \
        emdros/MQL/mql_parser.cpp \
        emdros/MQL/mql_lexer.cpp \
        emdros/MQL/mql_get_query.cpp \
        emdros/MQL/mql_query.cpp \
        emdros/MQL/mql_R.cpp \
        emdros/MQL/emdros_environment.cpp \
        emdros/MQL/mql_database_statements.cpp \
        emdros/MQL/mql_enumeration_statements.cpp \
        emdros/MQL/mql_error.cpp \
        emdros/MQL/mql_execute.cpp \
        emdros/MQL/mql_execution_environment.cpp \
        emdros/MQL/mql_features_statements.cpp \
        emdros/MQL/mql_helper_classes.cpp \
        emdros/MQL/mql_meta_statements.cpp \
        emdros/MQL/mql_monads_statements.cpp \
        emdros/MQL/mql_object.cpp \
        emdros/MQL/mql_object_statements.cpp \
        emdros/MQL/mql_object_type_statements.cpp \
        emdros/MQL/mql_result.cpp \
        emdros/MQL/mql_segment_statements.cpp \
        emdros/MQL/mql_select_statements.cpp \
        emdros/MQL/mql_sheaf.cpp \
        emdros/MQL/mql_statement.cpp \
        emdros/MQL/mql_utility.cpp \
        emdros/harvest/templatelang_classes.cpp \
        emdros/harvest/templatelang_parser.cpp \
        emdros/harvest/templatelang_lexer.cpp \
        emdros/harvest/renderxml.cpp \
        emdros/harvest/memobject.cpp \
        emdros/harvest/renderobjects.cpp \
        emdros/harvest/harvest_fts.cpp \
        emdros/harvest/harvest_fts2.cpp \
        emdros/harvest/harvest_fts3.cpp 



LOCAL_CPPFLAGS := -DUSE_SYSTEM_SQLITE3=0  -DTHREADSAFE=1  -Iemdros/include/ -IEMdF/ -IMQL/ -Iharvest/ 
LOCAL_CFLAGS := -DTHREADSAFE=1  -Iemdros/include/ -DHAVE_CONFIG_H
LOCAL_EXPORT_LDLIBS := -lz
LOCAL_CPP_FEATURES += exceptions
include $(BUILD_STATIC_LIBRARY)
