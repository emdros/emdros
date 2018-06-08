#
# Emdros Win32 Makefile for MQL directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 static library (non-debug) libmql.lib


WIN32DIR = ..\windows\build

LIBTARGET = libmql.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)" 
CLEANFILES = 

LIBTARGET_OBJS= \
	"$(INTDIR)\emdros_environment.obj" \
	"$(INTDIR)\mql_meta_statements.obj" \
	"$(INTDIR)\mql_helper_classes.obj" \
	"$(INTDIR)\mql_execute.obj" \
	"$(INTDIR)\mql_parser.obj" \
	"$(INTDIR)\mql_lexer.obj" \
	"$(INTDIR)\mql_get_query.obj" \
	"$(INTDIR)\mql_features_statements.obj" \
	"$(INTDIR)\mql_query.obj" \
	"$(INTDIR)\mql_object_statements.obj" \
	"$(INTDIR)\mql_error.obj" \
	"$(INTDIR)\mql_result.obj" \
	"$(INTDIR)\mql_sheaf.obj" \
	"$(INTDIR)\mql_database_statements.obj" \
	"$(INTDIR)\mql_object_type_statements.obj" \
	"$(INTDIR)\mql_monads_statements.obj" \
	"$(INTDIR)\mql_enumeration_statements.obj" \
	"$(INTDIR)\mql_segment_statements.obj" \
	"$(INTDIR)\mql_select_statements.obj" \
	"$(INTDIR)\mql_utility.obj" \
	"$(INTDIR)\mql_R.obj" \
	"$(INTDIR)\mql_execution_environment.obj" \
	"$(INTDIR)\mql_yylex.obj" \
	"$(INTDIR)\mql_statement.obj" \
	"$(INTDIR)\mql_object.obj" 


!INCLUDE $(WIN32DIR)\config.mak
!INCLUDE $(WIN32DIR)\body.mak

#
# Dependencies
#
"$(INTDIR)\mql_parser.obj" : mql_parser.cpp 
"$(INTDIR)\mql_lexer.obj" : ..\include\unistd.h
"$(INTDIR)\mql_get_query.obj" : ..\include\unistd.h

..\include\unistd.h : unistd.h.win32
	copy unistd.h.win32 ..\include\unistd.h
