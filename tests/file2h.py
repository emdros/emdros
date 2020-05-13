from __future__ import unicode_literals, print_function
import sys
import re


def usage():
    sys.stderr.write(("""
Usage:
    python file2h.py <infilename> <outfilename> <const-char-variable-name>

"""))


def read_doc(infilename):
    result = []
    for line in open(infilename, "rb"):
        uline = line.decode('utf-8')
        result.append(uline)

    return result


def process_doc(ulines):
    result = []

    for uline in ulines:
        tmp = re.sub(r'\\', r'\\\\', uline)
        tmp = re.sub(r'"', r'\\"', tmp)
        tmp = re.sub(r'\n', r'\\n', tmp)
        uline_result = "\"%s\"" % tmp
        result.append(uline_result)

    return result


def doIt(infilename, outfilename, const_char_varname):
    fout = open(outfilename, "wb")

    fout.write(("const char %s[] =\n" % const_char_varname).encode('utf-8'))

    ulines = read_doc(infilename)

    ulines2 = process_doc(ulines)

    fout.write("\n".join(ulines2).encode('utf-8'))

    fout.write(";\n".encode('utf-8'))

    fout.close()


if len(sys.argv) != 4:
    usage()
    sys.exit(1)
else:
    doIt(sys.argv[1], sys.argv[2], sys.argv[3])
    sys.exit(0)
