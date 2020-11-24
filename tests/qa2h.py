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
import sys


def emit_definition(fout):
    fout.write("""class QueryAnswer {
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
      };""".encode('utf-8'))


def emit_preamble(fout):
    fout.write("""QueryAnswer mqlqa[] = {\n""".encode('utf-8'))


def emit_postamble(fout):
    qa = QueryAnswer()
    qa.query_name = "STOP"
    qa.emit(fout)
    fout.write("""};\n""".encode('utf-8'))


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

        if type(line) == type(b''):
            line = line.decode('utf-8')

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

    def emit(self, fout):
        fout.write(("\n\n//%s\n" % self.query_name).encode('utf-8'))
        fout.write(("QueryAnswer(%s, \"%s\",\n" %
              (pybool(self.bNewDB), (self.query_name))).encode('utf-8'))
        query_string = "".join(self.query)
        escaped_query_string = "%s," % get_escaped_string(query_string)
        fout.write(escaped_query_string.encode('utf-8'))
        fout.write(("  %s, %s,\n" % (pybool(self.bDBSuccessExpected),
                             pybool(self.bCompilerSuccessExpected))).encode('utf-8'))
        fout.write(("%s),\n" % get_escaped_string("".join(self.answer))).encode('utf-8'))


def read_mqlqa(input_filename, output_filename):
    fin = open(input_filename, "rb")
    fout = open(output_filename, "wb")
    
    emit_definition(fout)
    emit_preamble(fout)
    qa = QueryAnswer()
    for line in fin.readlines():
        if qa.parse_line(line):
            qa.emit(fout)
            del qa
            qa = QueryAnswer()

    emit_postamble(fout)

    fout.close()
    fin.close()

def usage():
    print("""USAGE:
python qa2h.py <input-filename> <output-filename>

""")

if len(sys.argv) != 3:
    usage()
    exit()
else:
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]
    read_mqlqa(input_filename, output_filename)
