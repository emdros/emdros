TEMPLATE = lib

TARGET = mql

CONFIG = staticlib

VERSION = 3.1.1


SOURCES = mql_yylex.cpp \
       mql_parser.cpp \
       mql_lexer.cpp \
       mql_get_query.cpp \
       mql_query.cpp \
       mql_R.cpp \
       emdros_environment.cpp \
       mql_database_statements.cpp \
       mql_enumeration_statements.cpp \
       mql_error.cpp \
       mql_execute.cpp \
       mql_execution_environment.cpp \
       mql_features_statements.cpp \
       mql_helper_classes.cpp \
       mql_meta_statements.cpp \
       mql_monads_statements.cpp \
       mql_object.cpp \
       mql_object_statements.cpp \
       mql_object_type_statements.cpp \
       mql_result.cpp \
       mql_segment_statements.cpp \
       mql_select_statements.cpp \
       mql_sheaf.cpp \
       mql_statement.cpp \
       mql_utility.cpp 

INCLUDEPATH = ../include

win32:contains(MAKEFILE_GENERATOR, MSVC)|contains(MAKEFILE_GENERATOR, MSVC.NET) {
    QMAKE_CXXFLAGS += -EHsc
}



#macx:QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
macx:CONFIG += x86_64



