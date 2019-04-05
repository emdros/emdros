#
# Emdros Win32 Makefile for MQL directory
#
# Makefile for MinGW

# Will build a Win32 static library (non-debug) libmql.a


WIN32DIR = ..\win32

ifeq ("$(BUILD_FOR_DEVEL)","1")
LIBTARGET = libmql.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
else
LIBTARGET = 
TARGETS = 
endif

CLEANFILES = 

LIBTARGET_OBJS= \
	emdros_environment.o \
	mql_meta_statements.o \
	mql_helper_classes.o \
	mql_execute.o \
	mql_parser.o \
	mql_lexer.o \
	mql_get_query.o \
	mql_features_statements.o \
	mql_query.o \
	mql_object_statements.o \
	mql_error.o \
	mql_result.o \
	mql_sheaf.o \
	mql_database_statements.o \
	mql_object_type_statements.o \
	mql_monads_statements.o \
	mql_enumeration_statements.o \
	mql_segment_statements.o \
	mql_select_statements.o \
	mql_utility.o \
	mql_R.o \
	mql_execution_environment.o \
	mql_yylex.o \
	mql_statement.o \
	mql_object.o 


include $(WIN32DIR)\config.mak
include $(WIN32DIR)\mingw-body.mak

#
# Dependencies
#
mql_parser.o : mql_parser.cpp 
mql_lexer.o : ..\include\unistd.h
mql_get_query.o : ..\include\unistd.h

..\include\unistd.h : unistd.h.win32
	copy unistd.h.win32 ..\include\unistd.h
