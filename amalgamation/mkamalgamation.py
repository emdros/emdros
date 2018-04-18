# -*- coding: utf-8 -*-

# Still TODO:
#
# - do emdros.h

import sys
import os
import re

ignored_header_files = [
    "emdros-config.h",
    "emdros-lconfig.h",
    ]

source_files_C_list = [
    #{
    #    'language' : 'C++',
    #    'prefix' : '',
    #    'suffix' : '',
    #    'header_files' : ['include/emdros-lconfig.h'],
    #    'source_files' : ['EMdF/emdros-lconfig.cpp'],
    #},
    
    #{
    #    'language' : 'C',
    #    'prefix' : """
    ##ifndef PCRE2_CODE_UNIT_WIDTH
    ##define PCRE2_CODE_UNIT_WIDTH 8
    ##endif
    #""",
    #    'suffix' : '',
    #    'header_files' : [
    #        'pcre2-10.10/src/config.h',
    #        'pcre2-10.10/src/pcre2.h',
    #        'pcre2-10.10/src/pcre2_internal.h',
    #        'pcre2-10.10/src/pcre2_intmodedep.h',
    #        'pcre2-10.10/src/pcre2posix.h',
    #        'pcre2-10.10/src/pcre2_ucp.h',
    #        'pcre2-10.10/src/pcre2_jit_match.c',
    #        'pcre2-10.10/src/pcre2_jit_misc.c',
    #    ],
    #    'source_files' : [
    #        'pcre2-10.10/src/pcre2_compile.c',
    #        'pcre2-10.10/src/pcre2_config.c',
    #        'pcre2-10.10/src/pcre2_chartables.c',
    #        'pcre2-10.10/src/pcre2_auto_possess.c',
    #        'pcre2-10.10/src/pcre2_context.c',
    #        'pcre2-10.10/src/pcre2_dfa_match.c',
    #        'pcre2-10.10/src/pcre2_error.c',
    #        'pcre2-10.10/src/pcre2_jit_compile.c',
    #        'pcre2-10.10/src/pcre2_maketables.c',
    #        'pcre2-10.10/src/pcre2_match.c',
    #        'pcre2-10.10/src/pcre2_match_data.c',
    #        'pcre2-10.10/src/pcre2_newline.c',
    #        'pcre2-10.10/src/pcre2_ord2utf.c',
    #        'pcre2-10.10/src/pcre2_pattern_info.c',
    #        'pcre2-10.10/src/pcre2_serialize.c',
    #        'pcre2-10.10/src/pcre2_string_utils.c',
    #        'pcre2-10.10/src/pcre2_study.c',
    #        'pcre2-10.10/src/pcre2_substitute.c',
    #        'pcre2-10.10/src/pcre2_substring.c',
    #        'pcre2-10.10/src/pcre2_tables.c',
    #        'pcre2-10.10/src/pcre2_ucd.c',
    #        'pcre2-10.10/src/pcre2_valid_utf.c',
    #        'pcre2-10.10/src/pcre2_xclass.c',
    #    ],
    #    'code_prefix' : {
    #        'pcre2-10.10/src/pcre2_match.c' : """
    ##undef NLBLOCK
    ##undef PSSTART
    ##undef PSEND
    #        """,
    #        
    #        'pcre2-10.10/src/pcre2_dfa_match.c' : """
    ##undef NLBLOCK
    ##undef PSSTART
    ##undef PSEND
    #        """,
    #        
    #        'pcre2-10.10/src/pcre2_auto_possess.c' : """
    #
    #/* opcode_possessify is defined in pcre2_auto_possess.c,
    #but not used here. It is also defined in
    #pcre2_compile.c. Redefine here so as to avoid a name
    #clash. */
    ##define opcode_possessify  auto_possess_opcode_possessify
    # 
    # """
    #     },
    # },

    {
        'language' : 'C',
        'prefix' : """
#ifndef USE_LOCAL_PCRE
#define USE_LOCAL_PCRE 1
#endif

#if USE_LOCAL_PCRE
        """,
        'suffix' : '''
#endif /* if USE_LOCAL_PCRE */
''',
        'header_files' : [
            'pcre/config.h',
            'pcre/pcre.h',
            'pcre/internal.h',
            'pcre/pcre_internal.h',
            'pcre/pcreposix.h',
            'pcre/ucp.h',
            'pcre/ucpinternal.h',
        ],
        'source_files' : [
            'pcre/pcre_byte_order.c',
            'pcre/pcre_chartables.c',
            'pcre/pcre_compile.c',
            'pcre/pcre_config.c',
            'pcre/pcre_dfa_exec.c',
            'pcre/pcre_exec.c',
            'pcre/pcre_fullinfo.c',
            'pcre/pcre_get.c',
            'pcre/pcre_globals.c',
            'pcre/pcre_jit_compile.c',
            'pcre/pcre_maketables.c',
            'pcre/pcre_newline.c',
            'pcre/pcre_ord2utf8.c',
            'pcre/pcre_refcount.c',
            'pcre/pcre_string_utils.c',
            'pcre/pcre_study.c',
            'pcre/pcre_tables.c',
            'pcre/pcre_ucd.c',
            'pcre/pcre_valid_utf8.c',
            'pcre/pcre_version.c',
            'pcre/pcre_xclass.c',
        ],
        'code_prefix' : {
            'pcre/pcre_compile.c' : """
    #undef NLBLOCK
    #undef PSSTART
    #undef PSEND
            """,
            'pcre/pcre_dfa_exec.c' : """
    #undef NLBLOCK
    #undef PSSTART
    #undef PSEND
            """,
            
            'pcre/pcre_exec.c' : """
    #undef NLBLOCK
    #undef PSSTART
    #undef PSEND
            """,
        },
    },
    {
        'language' : 'C',
        'prefix' : """
    #ifndef USE_SQLITE3
    #define USE_SQLITE3 1
    #endif

    #ifndef USE_SYSTEM_SQLITE3
    #define USE_SYSTEM_SQLITE3 0
    #endif

    #undef TK_SEMI
    #undef TK_EXPLAIN
    #undef TK_BEGIN
    #undef TK_TRANSACTION
    #undef TK_DEFERRED
    #undef TK_IMMEDIATE
    #undef TK_COMMIT
    #undef TK_END
    #undef TK_ROLLBACK
    #undef TK_TABLE
    #undef TK_CREATE
    #undef TK_NOT
    #undef TK_TEMP
    #undef TK_LP
    #undef TK_RP
    #undef TK_AS
    #undef TK_COMMA
    #undef TK_ID
    #undef TK_ABORT
    #undef TK_AFTER
    #undef TK_ASC
    #undef TK_ATTACH
    #undef TK_BEFORE
    #undef TK_BY
    #undef TK_CASCADE
    #undef TK_CONFLICT
    #undef TK_DATABASE
    #undef TK_DESC
    #undef TK_DETACH
    #undef TK_EACH
    #undef TK_FAIL
    #undef TK_FOR
    #undef TK_IGNORE
    #undef TK_INITIALLY
    #undef TK_INSTEAD
    #undef TK_MATCH
    #undef TK_KEY
    #undef TK_OF
    #undef TK_OFFSET
    #undef TK_PRAGMA
    #undef TK_RAISE
    #undef TK_REPLACE
    #undef TK_RESTRICT
    #undef TK_ROW
    #undef TK_TRIGGER
    #undef TK_VACUUM
    #undef TK_VIEW
    #undef TK_OR
    #undef TK_AND
    #undef TK_IS
    #undef TK_BETWEEN
    #undef TK_IN
    #undef TK_ISNULL
    #undef TK_NOTNULL
    #undef TK_NE
    #undef TK_EQ
    #undef TK_GT
    #undef TK_LE
    #undef TK_LT
    #undef TK_GE
    #undef TK_BITAND
    #undef TK_BITOR
    #undef TK_LSHIFT
    #undef TK_RSHIFT
    #undef TK_PLUS
    #undef TK_MINUS
    #undef TK_STAR
    #undef TK_SLASH
    #undef TK_REM
    #undef TK_CONCAT
    #undef TK_COLLATE
    #undef TK_BITNOT
    #undef TK_STRING
    #undef TK_JOIN_KW
    #undef TK_CONSTRAINT
    #undef TK_DEFAULT
    #undef TK_NULL
    #undef TK_PRIMARY
    #undef TK_UNIQUE
    #undef TK_CHECK
    #undef TK_REFERENCES
    #undef TK_ON
    #undef TK_INSERT
    #undef TK_DELETE
    #undef TK_UPDATE
    #undef TK_SET
    #undef TK_DEFERRABLE
    #undef TK_FOREIGN
    #undef TK_DROP
    #undef TK_UNION
    #undef TK_ALL
    #undef TK_EXCEPT
    #undef TK_INTERSECT
    #undef TK_SELECT
    #undef TK_VALUES
    #undef TK_DISTINCT
    #undef TK_DOT
    #undef TK_FROM
    #undef TK_JOIN
    #undef TK_USING
    #undef TK_ORDER
    #undef TK_GROUP
    #undef TK_HAVING
    #undef TK_LIMIT
    #undef TK_WHERE
    #undef TK_INTO
    #undef TK_INTEGER
    #undef TK_FLOAT
    #undef TK_VARIABLE
    #undef TK_CASE
    #undef TK_WHEN
    #undef TK_THEN
    #undef TK_ELSE
    #undef TK_INDEX
    #undef TK_END_OF_FILE
    #undef TK_ILLEGAL
    #undef TK_SPACE
    #undef TK_UNCLOSED_STRING
    #undef TK_FUNCTION
    #undef TK_COLUMN
    #undef TK_AGG_FUNCTION
    #undef TK_UMINUS
    #undef TK_UPLUS
    #undef SCHEMA_TABLE
    #undef ArraySize
    #undef DbHasProperty
    #undef DbHasAnyProperty
    #undef DbSetProperty
    #undef DbClearProperty
    #undef DB_SchemaLoaded
    #undef DB_UnresetViews
    #undef SQLITE_InternChanges
    #undef OE_Default
    #undef EP_FromJoin
    #undef ExprHasProperty
    #undef JT_NATURAL
    #undef JT_LEFT
    #undef JT_RIGHT
    #undef JT_OUTER
    #undef JT_ERROR
    #undef SRT_Union
    #undef SRT_Except
    #undef SRT_Discard
    #undef SRT_Mem
    #undef SRT_Set
    #undef SRT_Table
    #undef OPFLAG_NCHANGE
    #undef OPFLAG_LASTROWID
    #undef MEM_Dyn
    #undef MEM_Static
    #undef MEM_Ephem
    #undef etSQLESCAPE
    #undef etSQLESCAPE2
    #undef etTOKEN
    #undef etSRCLIST
    #undef etBUFSIZE
    #undef JOURNAL_PG_SZ
    #undef JOURNAL_HDR_SZ
    #undef MX_CELL
    #undef EXTRA_SIZE
    #undef Stringify
    #undef Deephemeralize
    #undef initMaskSet
    #undef tkSEMI
    #undef tkWS
    #undef tkOTHER
    #undef tkEXPLAIN
    #undef tkCREATE
    #undef tkTEMP
    #undef tkTRIGGER
    #undef tkEND
    #undef TOKEN

    /* #define'd in pcre. */
    #undef SETBIT


    #if (USE_SQLITE3) && !(USE_SYSTEM_SQLITE3)
        """,
        'suffix' : """
    #endif /* if USE_SQLITE3 */
        """,
        'header_files' : [
            'sqlite3/src/sqlite3.h',
            'sqlite3/src/sqlite3ext.h',
        ],
        'source_files' : [
            'sqlite3/src/sqlite3_localamalgam.c',
        ],
    },
    #{
    #    'language' : 'C',
    #    'prefix' : """
    #    """,
    #    'suffix' : """
    #    """,
    #    'header_files' : {
    #        'zlib/crc32.h',
    #        'zlib/gzguts.h',
    #        'zlib/inffixed.h',
    #        'zlib/inftrees.h',
    #        'zlib/zconf.h',
    #        'zlib/zutil.h',
    #        'zlib/deflate.h',
    #        'zlib/inffast.h',
    #        'zlib/inflate.h',
    #        'zlib/trees.h',
    #        'zlib/zlib.h',
    #    },
    #    'source_files' : [
    #        'zlib/adler32.c',
    #        'zlib/crc32.c',
    #        'zlib/deflate.c',
    #        'zlib/infback.c',
    #        'zlib/inffast.c',
    #        'zlib/inflate.c',
    #        'zlib/inftrees.c',
    #        'zlib/trees.c',
    #        'zlib/zutil.c',
    #        'zlib/compress.c',
    #        'zlib/uncompr.c',
    #        'zlib/gzclose.c',
    #        'zlib/gzlib.c',
    #        'zlib/gzread.c',
    #        'zlib/gzwrite.c',
    #    ],
    #},
]

source_files_Cpp_list = [
    {
        'language' : 'C++',
        'prefix' : '''
#ifndef SQLITE3_EMDFDB_NO_FSYNC
#define SQLITE3_EMDFDB_NO_FSYNC
#endif

''',
        'suffix' : '',
        'header_files' : [
            'include/emdf.h',
            'include/emdf_primes.h',
            'include/arena.h',
            'include/bpt2dumper.h',
            'include/bpt2emdfdb.h',
            'include/bptdumper.h',
            'include/bptemdfdb.h',
            'include/conn.h',
            'include/crc32.h',
            'include/debug.h',
            'include/emdfdb.h',
            'include/emdf_enums.h',
            'include/emdf_exception.h',
            'include/emdf_ffeatures.h',
            'include/emdf_hash.h',
            'include/emdf_output.h',
            'include/emdf_value.h',
            'include/emdros_environment.h',
            'include/emdros_reserved_words.h',
            'include/encryption.h',
            'include/enum_const_cache.h',
            'include/environment_emdros.h',
            'include/exception_emdros.h',
            'include/harvest_fts.h',
            'include/harvest_fts2.h',
            'include/harvest_fts3.h',
            'include/infos.h',
            'include/inst.h',
            'include/inst_object.h',
            'include/json_classes.h',
            'include/json_lexer.h',
            'include/llist.h',
            'include/logging.h',
            'include/md5_emdros.h',
            'include/memobject.h',
            'include/messages.h',
            'include/minidom.h',
            'include/monads.h',
            'include/mql_database_statements.h',
            'include/mql_enumeration_statements.h',
            'include/mql_enums.h',
            'include/mql_error.h',
            'include/mql_execute.h',
            'include/mql_execution_environment.h',
            'include/mql_exporter.h',
            'include/mql_extern.h',
            'include/mql_features_statements.h',
            'include/mql_gq.h',
            'include/mql_lexer.h',
            'include/mql_meta_statements.h',
            'include/mql_monads_statements.h',
            'include/mql_object.h',
            'include/mql_object_statements.h',
            'include/mql_object_type_statements.h',
            'include/mqlpars.h',
            'include/mql_query.h',
            'include/mql_result.h',
            'include/mql_R.h',
            'include/mql_segment_statements.h',
            'include/mql_select_statements.h',
            'include/mql_sheaf.h',
            'include/mql_types.h',
            'include/mql_utility.h',
            'include/mql_yylex.h',
            'include/mysqlconn.h',
            'include/mysqlemdfdb.h',
            'include/pcre_config.h',
            'include/pcre_emdros.h',
            'include/pgconn.h',
            'include/pgemdfdb.h',
            'include/prefix_emdros.h',
            'include/qdxml.h',
            'include/renderobjects.h',
            'include/renderxml.h',
            'include/smart_vector.h',
            'include/sqlite3conn.h',
            'include/sqlite3emdfdb.h',
            'include/sqlite3_emdros.h',
            'include/string_func.h',
            'include/string_list.h',
            'include/string_set_cache.h',
            'include/table.h',
            'include/templatelang_classes.h',
            'include/templatelang_lexer.h',
            'include/utils.h',
            'include/version-emdros.h',
            'include/zlib_emdros.h',
            'EMdF/jsonpars.h',
            'harvest/templatelangpars.h',
            'include/opt.h',
            'include/messages.h',
        ],
        'source_files' : [
            'EMdF/crc32.cpp',
            'EMdF/emdf_primes.cpp',
            'EMdF/logging.cpp',
            'EMdF/encryption.cpp',
            'EMdF/arena.cpp',
            'EMdF/monads.cpp',
            'EMdF/minidom.cpp',
            'EMdF/qdxml.cpp',
            'EMdF/infos.cpp',
            'EMdF/table.cpp',
            'EMdF/utils.cpp',
            'EMdF/string_func.cpp',
            'EMdF/string_list.cpp',
            'EMdF/string_set_cache.cpp',
            'EMdF/emdf_hash.cpp',
            'EMdF/emdf_output.cpp',
            'EMdF/emdf_value.cpp',
            'EMdF/emdf_ffeatures.cpp',
            'EMdF/enum_const_cache.cpp',
            'EMdF/inst_object.cpp',
            'EMdF/inst.cpp',
            'EMdF/json_classes.cpp',
            'EMdF/json_lexer.cpp',
            'EMdF/json_parser.cpp',
            'EMdF/conn.cpp',
            'EMdF/emdfdb.cpp',
            'EMdF/pgconn.cpp',
            'EMdF/pgemdfdb.cpp',
            'EMdF/mysqlconn.cpp',
            'EMdF/mysqlemdfdb.cpp',
            'EMdF/sqlite3conn.cpp',
            'EMdF/sqlite3emdfdb.cpp',
            'EMdF/bptemdfdb.cpp',
            'EMdF/bpt2emdfdb.cpp',

            'MQL/mql_error.cpp',
            'MQL/mql_sheaf.cpp',
            'MQL/mql_result.cpp',
            'MQL/mql_execution_environment.cpp',
            'MQL/mql_helper_classes.cpp',
            'MQL/mql_object.cpp',
            'MQL/mql_statement.cpp',
            'MQL/mql_segment_statements.cpp',
            'MQL/mql_select_statements.cpp',
            'MQL/mql_utility.cpp',
            'MQL/mql_database_statements.cpp',
            'MQL/mql_enumeration_statements.cpp',
            'MQL/mql_features_statements.cpp',
            'MQL/mql_meta_statements.cpp',
            'MQL/mql_monads_statements.cpp',
            'MQL/mql_object_statements.cpp',
            'MQL/mql_object_type_statements.cpp',
            'MQL/mql_get_query.cpp',
            'MQL/mql_yylex.cpp',
            'MQL/mql_lexer.cpp',
            'MQL/mql_parser.cpp',
            'MQL/mql_query.cpp',
            'MQL/mql_R.cpp',
            'MQL/mql_execute.cpp',
            'MQL/emdros_environment.cpp',

            'harvest/memobject.cpp',
            'harvest/templatelang_classes.cpp',
            'harvest/templatelang_lexer.cpp',
            'harvest/templatelang_parser.cpp',
            'harvest/renderobjects.cpp',
            'harvest/renderxml.cpp',
            'harvest/harvest_fts.cpp',
            'harvest/harvest_fts2.cpp',
            'harvest/harvest_fts3.cpp',

            #'util/agxml.cpp',
            #'util/bpt2dumper.cpp',
            #'util/bptdumper.cpp',
            #'util/conf.cpp',
            #'util/md5_emdros.cpp',
            'util/messages.cpp',
            #'util/mql_exporter.cpp',
            #'util/negraimporter.cpp',
            'util/opt.cpp',
            #'util/pennimporter.cpp',
            #'util/pennlexer.cpp',
            #'util/pennparser.cpp',
            #'util/penn_yylex.cpp',
            #'util/plaintextimporter.cpp',
            #'util/schema.cpp',
            #'util/sfm.cpp',
            #'util/sfmimporter.cpp',
            #'util/slashedtextimporter.cpp',
            #'util/tigerxmlimporter.cpp',
            #'util/unboundbibleimporter.cpp',
        ],
        'code_prefix' : {
            'harvest/harvest_fts.cpp' : """
    #undef MYMAX
    #undef MYMIN
            """,

            'harvest/harvest_fts2.cpp' : """
    #undef MYMAX
    #undef MYMIN
            """,
        },
    },
    
]

def make_headers_available():
    result = {}
    for source_files_list in [source_files_C_list, source_files_Cpp_list]:
        for obj in source_files_list:
            for header_path in obj['header_files']:
                header_filename = os.path.split(header_path)[-1]
                result.setdefault(header_filename, header_path)

    # Ignore these, and elide their inclusion.
    for header_filename in ignored_header_files:
        result[header_filename] = None        

    return result

include_re = re.compile(ur'^\s*#\s*include\s*[<"]([^>"]+)[>"].*$')


def copy_file(fout, current_language, headers_available, source_filename):
    fout.write("/**************** A copy of %s *****************/\n" % source_filename)
    if current_language == 'C':
        fout.write("""
#ifdef __cplusplus
extern "C" {\n
#endif
""")

    fout.write("""

#line 1 "%(source_filename)s"
        """ % {
            "source_filename" : source_filename,
        })
    line_count = 0
    for line in open(os.path.join("..", source_filename), "rb"):
        line_count += 1
        
        uline = line.decode('utf-8')

        include_mo = include_re.match(uline)

        if include_mo == None:
            fout.write(uline.encode('utf-8'))
        else:
            header_filename = include_mo.group(1)
            if header_filename not in headers_available:
                fout.write(uline.encode('utf-8'))
            elif headers_available[header_filename] != None:
                fout.write("/**************** leaving %s temporarily *****************/\n" % source_filename)

                header_path = headers_available[header_filename]

                # Make sure we don't emit this header again.
                headers_available[header_filename] = None

                # Copy the header
                copy_file(fout, current_language, headers_available, header_path)

                fout.write("/**************** continuing %s where we left off *****************/\n" % source_filename)
                fout.write("""

#line %(line_count)d "%(source_filename)s"
        """ % {
            "source_filename" : source_filename,
            "line_count" : line_count,
        })
            else:
                fout.write("/**************** already included %s -- not including again *****************/\n" % header_filename)
                fout.write("""

#line %(line_count)d "%(source_filename)s"
        """ % {
            "source_filename" : source_filename,
            "line_count" : line_count,
        })
                

    if current_language == 'C':
        fout.write("""
#ifdef __cplusplus
} /* End of 'extern "C"'. */
#endif
""")
        

def make_sourcecode_amalgamation(outfilename, match_language):
    fout = open(outfilename, "wb")

    if match_language == "C++":
        source_files_list = source_files_Cpp_list
        
        fout.write("""
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 1999-2015  Ulrik Sandborg-Petersen
 *
 ***********************************************************************/

/****
 * _GNU_SOURCE MUST be defined before _any_ standard headers are included. 
 * Hence, we define it at the very top.
 ****/
#if defined(__GNUC__) && !defined(_GNU_SOURCE)
# define _GNU_SOURCE
#endif

#include "emdros-lconfig.h"
#include "emdros-config.h"

""")
    elif match_language == "C":
        source_files_list = source_files_C_list

        fout.write("""

/****
 * _GNU_SOURCE MUST be defined before _any_ standard headers are included. 
 * Hence, we define it at the very top.
 ****/
#if defined(__GNUC__) && !defined(_GNU_SOURCE)
# define _GNU_SOURCE
#endif


#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H  (1)
#endif

#include "emdros-lconfig.h"
#include "emdros-config.h"

""")

    headers_available = make_headers_available()

    for obj in source_files_list:
        current_language = obj['language']

        if 'code_prefix' in obj:
            code_prefix_dict = obj['code_prefix']
        else:
            code_prefix_dict = {}

        if current_language == match_language:

            if 'prefix' in obj:
                fout.write(obj['prefix'])
        
            for source_filename in obj['source_files']:
                if source_filename in code_prefix_dict:
                    fout.write(code_prefix_dict[source_filename])
                    
                copy_file(fout, current_language, headers_available, source_filename)

            if 'suffix' in obj:
                fout.write(obj['suffix'])

    fout.close()

def make_h_amalgamation(outfilename):
    fout = open(outfilename, "wb")

    fout.write("""
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 1999-2015  Ulrik Sandborg-Petersen
 *
 ***********************************************************************/

#ifndef EMDROS_H_
#define EMDROS_H_

/****
 * _GNU_SOURCE MUST be defined before _any_ standard headers are included. 
 * Hence, we define it at the very top.
 ****/
#if defined(__GNUC__) && !defined(_GNU_SOURCE)
# define _GNU_SOURCE
#endif

#include "emdros-lconfig.h"
#include "emdros-config.h"

""")
    
    headers_available = make_headers_available()

    for obj in source_files_Cpp_list:
        current_language = obj['language']

        if 'code_prefix' in obj:
            code_prefix_dict = obj['code_prefix']
        else:
            code_prefix_dict = {}

        if 'prefix' in obj:
            fout.write(obj['prefix'])
        
        for source_filename in obj['header_files']:
            header_filename = os.path.split(source_filename)[-1]
            if header_filename in headers_available:
                bDoIt = headers_available[header_filename] != None
                headers_available[header_filename] = None
            else:
                bDoIt = False

            if bDoIt:
                if source_filename in code_prefix_dict:
                    fout.write(code_prefix_dict[source_filename])
                    
                copy_file(fout, current_language, headers_available, source_filename)
            else:
                pass

        if 'suffix' in obj:
            fout.write(obj['suffix'])
            
            
    fout.write("""

#endif /* !defined(EMDROS_H_) */
""")
    fout.close()


def doIt():
    make_sourcecode_amalgamation('emdros_c_amalgamation.c', 'C')
    make_sourcecode_amalgamation('emdros_amalgamation.cpp', 'C++')
    make_h_amalgamation('emdros.h')


doIt()

