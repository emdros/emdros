CXXFLAGS=-I../include -O3
#CXXFLAGS=-I../include -g


#
# Sources
#

mqllib_src = \
       mql_yylex.cpp \
       mql_parser.cpp \
       mql_lexer.cpp \
       mql_get_query.cpp \
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
       mql_query.cpp \
       mql_R.cpp \
       mql_result.cpp \
       mql_segment_statements.cpp \
       mql_select_statements.cpp \
       mql_sheaf.cpp \
       mql_statement.cpp \
       mql_symboltable.cpp \
       mql_utility.cpp 


#
# Objects
#
objects = $(mqllib_src:.cpp=.o)

CLEANFILES = *.O  $(TARGETS)

TARGETS = libmql.a

all: $(TARGETS)

libmql.a: $(objects)
	ar r $@ $(objects)


        

clean:
	for %f in ($(CLEANFILES)) DO del %f

