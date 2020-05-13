#
# This is qa2h.py, a program for converting mqlqa.h.in to mqlqa.h.
#
# The purpose of mqlqa.h is to be #included in mqltry.cpp.
#
# The contents of mqlqa.h.in, transformed to mqlqa.h, is an ordered list
# of queries and their expected results.
#
# The format is as follows:
#
# - Lines beginning with # are ignored, regardless of where they are in the file.
#
# - Lines beginning with "++++ query" start a new QueryAnswer block, and also start
#   the query within the QueryAnswer block.
#
#   - After the word "query", there may optionally be the keyword "newdb", which instructs
#     mqltry.cpp to drop the database, create it anew, and use it.
#
#   - After the word "query" (and an optional 'newdb'), there can be an
#     optional name for the query. Whitespace must be present between
#     the word "query"/"newdb" and the name of the query.
#
# - Lines beginning with "++++ answer" start a new 'expected result' block, and also
#   end the query within the QueryAnswerBlock.
#
#   - After the word "anwer", there may optionally be one of the
#     keywords "faildb" (indicating that the query is expected to fail
#     with a DB failure), EXCLUSIVE OR "failcompiler" (indicating that
#     the query is expected to fail with a compiler failure).
#
# - Lines beginning with "---- answer" end the 'expected result' block.
#
# - All other lines are ignored.
#
from __future__ import unicode_literals, print_function
import sys


def emit_definition():
    print("""class QueryAnswer {
      public:
      QueryAnswer(bool bNewDB,
                  const char *query_name, 
                  const char *query, 
                  bool bDBSuccessExpected,
                  bool bCompilerSuccessExpected,
		  const char *answer) {
		  m_bNewDB = bNewDB;
		  m_query_name = query_name;
		  m_query = query;
		  m_bDBSuccessExpected = bDBSuccessExpected;
		  m_bCompilerSuccessExpected = bCompilerSuccessExpected;
		  m_answer = answer;
      };
      std::string m_query_name, m_query, m_answer;
      bool m_bNewDB;
      bool m_bDBSuccessExpected, m_bCompilerSuccessExpected;
      };""")


def emit_preamble():
    print("""QueryAnswer mqlqa[] = {""")


def emit_postamble():
    qa = QueryAnswer()
    qa.query_name = "STOP"
    qa.emit()
    print("""};""")


def get_escaped_string(s):
    result = ""
    arr = s.split("\n")
    for index in range(0, len(arr)):
        line = arr[index]
        result += "  \"" + line.replace("\\", "\\\\").replace("\"", "\\\"")
        if index < (len(arr) - 1):
            result += "\\n\"\n"
        else:
            result += "\""
    return result


sBeforeQuery = 0
sInQuery = 1
sInAnswer = 2


def pybool(b):
    if b:
        return "true"
    else:
        return "false"


class QueryAnswer:
    def __init__(self):
        self.bNewDB = False
        self.bDBSuccessExpected = True
        self.bCompilerSuccessExpected = True
        self.query_name = ""
        self.query = []
        self.answer = []
        self.state = sBeforeQuery

    def parse_line(self, line):
        """Returns True on the answer is over, False otherwise."""

        if line[0] == "#":
            return False
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
                    self.bNewDB = True
                    if len(arr) > 3:
                        self.query_name = " ".join(arr[3:])
                else:
                    self.query_name = " ".join(arr[2:])
        else:
            if self.state == sBeforeQuery:
                pass
            elif self.state == sInQuery:
                self.query.append(line)  # Include newline
            elif self.state == sInAnswer:
                self.answer.append(line)  # Include newline
        return False

    def emit(self):
        print("\n\n//%s" % self.query_name)
        print("QueryAnswer(%s, \"%s\"," %
              (pybool(self.bNewDB), (self.query_name)))
        print("%s," % get_escaped_string("".join(self.query)))
        print("  %s, %s," % (pybool(self.bDBSuccessExpected),
                             pybool(self.bCompilerSuccessExpected)))
        print("%s)," % get_escaped_string("".join(self.answer)))


def read_mqlqa(fin):
    emit_definition()
    emit_preamble()
    qa = QueryAnswer()
    for line in fin.readlines():
        if qa.parse_line(line):
            qa.emit()
            del qa
            qa = QueryAnswer()

    emit_postamble()


read_mqlqa(sys.stdin)
