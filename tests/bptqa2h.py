#
# This is bptqa2h.py, a program for converting bptqa.h.in to bptqa.h.
#
# The purpose of bptqa.h is to be #included in bpttry.cpp.
#
# The contents of bptqa.h.in, transformed to bptqa.h, is an ordered
# list of queries and their expected results.
#
# The format is as follows:
#
#   - Lines beginning with # are ignored, regardless of where they are
#     in the file.
#
#   - Lines beginning with ++++ dumpdb will cause bpttry to drop the
#     current BPT database, dump the current parent database as a BPT
#     database, and use the new BPT database.
#
#   - Lines beginning with "++++ query" start a new QueryAnswer block,
#     and also start the query within the QueryAnswer block.
#
#   - After the word "query", the keyword "nobpt" may optionally
#     appear. This instructs bpttry to apply the query only to the
#     parent database. This is useful, e.g., when creating the parent
#     database.
#
#   - Alternatively, instead of the word "nobpt", the keyword "newdb"
#     may optionally appear, which instructs bpttry.cpp to drop the
#     parent database as well as the BPT database, and to create a new
#     parent database and use it. The BPT database remains closed
#     until a ++++ dumpdb line occurs, and so all subsequent queries
#     until the next ++++ dumpdb will apply only to the parent
#     database. Therefore, all such queries between a newdb and the
#     ++++ dumpdb must have the "nobpt" keyword. The bptqa2h.py
#     program enforces this.
#
#   - After the word "query" (and an optional 'newdb' or 'nobpt'),
#     there can be an optional name for the query. Whitespace must be
#     present between the word "query"/"newdb"/"dumpdb" and the name
#     of the query.
#
#   - Lines beginning with "++++ answer" start a new 'expected result'
#     block, and also end the query within the QueryAnswerBlock.
#
#   - After the word "anwer", there may optionally be one of the
#     keywords "faildb" (indicating that the query is expected to fail
#     with a DB failure), EXCLUSIVE OR "failcompiler" (indicating that
#     the query is expected to fail with a compiler failure).
#
#   - Lines beginning with "---- answer" end the 'expected result'
#     block.
#
#   - All other lines are ignored.
#
import sys


def get_escaped_string(s):
    result = ""
    arr = s.split("\n")
    for index in xrange(0, len(arr)):
        line = arr[index]
	result += "  \"" + line.replace("\\", "\\\\").replace("\"", "\\\"")
        if index < (len(arr) - 1):
            result += "\\n\"\n"
        else:
            result += "\""
    return result

def get_escaped_string_string(s):
    tmp_arr = []
    tmp_arr.append([])
    arr = s.split("\n")
    count = 0
    for index in xrange(0, len(arr)):
        line = arr[index]
	result = "  \"" + line.replace("\\", "\\\\").replace("\"", "\\\"")
        if index < (len(arr) - 1):
            result += "\\n\"\n"
        else:
            result += "\""
        tmp_arr[-1].append(result)
        count += 1
        if count == 800 and index != len(arr)-1:
            tmp_arr.append([])
            count = 0
            
    result_arr = []
    for inner_arr in tmp_arr:
        result_arr.append("std::string(%s)" % "".join(inner_arr))
    return "+\n".join(result_arr) + "\n"

#
# This madness is necessary on Win32, where a const char* string
# cannot exceed 65536 bytes...
#
def postprocess_const_char(instr):
    arr = instr.strip().split("\n")
    tmp_arr = []
    tmp_arr.append([])

    count = 0
    for line in arr:
        tmp_arr[-1].append(line + "\n")
        count += 1
        if count == 800:
            tmp_arr.append([])
            count = 0

    # Chop off the last one if it is empty
    if len(tmp_arr[-1]) == 0:
        tmp_arr.pop()

    result_arr = []
    for inner_arr in tmp_arr:
        result_arr.append("std::string(%s)" % "\n".join(["\"%s\"" % x.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n") for x in inner_arr]))
    return "+\n".join(result_arr) + "\n"


def emit_definition():
    print """
#include <string>

class QueryAnswer {
      public:
      QueryAnswer(bool bNewDB,
                  bool bNoBPT,
                  bool bBPTDumpDB,
                  const std::string& query_name, 
                  const std::string& query, 
                  bool bDBSuccessExpected,
                  bool bCompilerSuccessExpected,
		  const std::string& answer) {
		  m_bNewDB = bNewDB;
                  m_bNoBPT = bNoBPT;
                  m_bBPTDumpDB = bBPTDumpDB;
		  m_query_name = query_name;
		  m_query = query;
		  m_bDBSuccessExpected = bDBSuccessExpected;
		  m_bCompilerSuccessExpected = bCompilerSuccessExpected;
		  m_answer = answer;
      };
      std::string m_query_name, m_query, m_answer;
      bool m_bNewDB;
      bool m_bNoBPT;
      bool m_bBPTDumpDB;
      bool m_bDBSuccessExpected, m_bCompilerSuccessExpected;
      };"""

def emit_preamble():
    print """std::list<QueryAnswer*> bptqa_list;

"""

def emit_postamble(function_number_list):
    qa = QueryAnswer()
    qa.query_name = "STOP";
    qa.emit();
    print """
}

void init_bptqa_list()
{
"""
    print u"\n".join(["    init_bptqa_list_%02d();" % function_number for function_number in function_number_list])
    
    print """}

void teardown_bptqa_list()
{
    std::list<QueryAnswer*>::iterator it = bptqa_list.begin();
    std::list<QueryAnswer*>::iterator itend = bptqa_list.end();

    while (it != itend) {
        QueryAnswer *pQA = *it;

        delete pQA;

        ++it;
    }

    bptqa_list.clear();
}
"""

sBeforeQuery = 0
sInQuery = 1
sInAnswer = 2

def pybool(b):
    if b:
	return "true"
    else:
	return "false"

bNewDBHasNotBeenDumped = False

class QueryAnswer:
    def __init__(self):
	self.bNewDB = False
        self.bNoBPT = False
        self.bDumpDB = False
	self.bDBSuccessExpected = True
	self.bCompilerSuccessExpected = True
        self.query_name = ""
	self.query = []
	self.answer = []
	self.state = sBeforeQuery

    def parse_line(self, line):
	"""Returns True on the answer is over, False otherwise."""

        global bNewDBHasNotBeenDumped

	if line[0] == "#":
	    return False
	elif line.rstrip().lower() == "++++ dumpdb":
            assert self.state == sBeforeQuery
            self.bNoBPT = True
            self.bDumpDB = True
            bNewDBHasNotBeenDumped = False
	    return True
	elif line.rstrip().lower() == "---- answer":
	    return True
        elif line.rstrip()[0:11].lower() == "++++ answer":
	    self.state = sInAnswer
	    arr = line.strip().split()
            if len(arr) > 2:
                if arr[2].lower() == "faildb":
                    self.bDBSuccessExpected = False
                elif arr[2].lower() == "failcompiler":
                    self.bCompilerSuccessExpected = False
                else:
                    raise "Error: Unknown failure method in line %s" % line
	elif line.rstrip()[0:10].lower() == "++++ query":
	    self.state = sInQuery
	    arr = line.strip().split()
            if len(arr) > 2:
                if arr[2].lower() == "newdb":
                    bNewDBHasNotBeenDumped = True
                    self.bNewDB = True
                    self.bNoBPT = True
                    if len(arr) > 3:
                        self.query_name = " ".join(arr[3:])
                elif arr[2].lower() == "nobpt":
                    self.bNoBPT = True
                    if len(arr) > 3:
                        self.query_name = " ".join(arr[3:])
                else:
                    self.query_name = " ".join(arr[2:])
            if bNewDBHasNotBeenDumped and self.bNoBPT == False:
                raise Exception("For query with query name:\n%s\n... bNewDBHasNotBeenDumped is True, yet self.bNoBPT is False.\nYou must add nobpt to the query." % self.query_name)
	else:
	    if self.state == sBeforeQuery:
		pass
	    elif self.state == sInQuery:
		self.query.append(line) # Include newline
	    elif self.state == sInAnswer:
		self.answer.append(line) # Include newline
	return False
	
    def emit(self):
        print "\n\n//%s" % self.query_name
	print "bptqa_list.push_back(new QueryAnswer(%s, %s, %s, \"%s\"," % (pybool(self.bNewDB), pybool(self.bNoBPT), pybool(self.bDumpDB), (self.query_name))
	print "%s," % get_escaped_string_string("".join(self.query))
	print "  %s, %s," % (pybool(self.bDBSuccessExpected), pybool(self.bCompilerSuccessExpected))
	print "%s));" % get_escaped_string_string("".join(self.answer))
	

def read_bptqa(fin):
    emit_definition()
    emit_preamble()
    qa = QueryAnswer()
    function_number_list = []
    function_number = 1
    print """
void init_bptqa_list_%02d()
{
""" % function_number
    function_statement_count = 0
    function_number_list.append(function_number)
    for line in fin.readlines():
	if qa.parse_line(line):
	    qa.emit()
	    del qa
	    qa = QueryAnswer()
            function_statement_count += 1

            if function_statement_count == 10:
                function_number += 1
                print """}

void init_bptqa_list_%02d()
{
""" % function_number
                function_number_list.append(function_number)
                function_statement_count = 0

    if function_statement_count != 0:
        function_number += 1
        print """}

void init_bptqa_list_%02d()
{
""" % function_number
        function_number_list.append(function_number)

    emit_postamble(function_number_list)

read_bptqa(sys.stdin)


