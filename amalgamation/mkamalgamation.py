# -*- coding: utf-8 -*-
#
# This Python script is not very Pythonic.
# This is because it was written as a prototype
# for later translation into C++.
#

from __future__ import unicode_literals
import sys
import os
import xml.sax
import io

def match_include(uline):
    START_OF_LINE = 0
    EXPECT_INCLUDE = 1
    EXPECT_OPEN_FILENAME = 2
    READ_FILENAME = 3
    
    FAILURE = ""
    
    state = START_OF_LINE

    index = 0
    length = len(uline)

    header_filename_start_index = -1
    include_end_char = ' '
    
    while index < length:
        c = uline[index]

        if state == START_OF_LINE:
            if c == " " or c == "\t":
                state = START_OF_LINE
            elif c == "#":
                state = EXPECT_INCLUDE
            else:
                # Line does not match.
                return FAILURE
        elif state == EXPECT_INCLUDE:
            if c == " " or c == "\t":
                state = EXPECT_INCLUDE
            elif c == "i":
                if length - index > len("nclude <.>"):
                    if uline[index:index+7] == "include":
                        index += 7
                        state = EXPECT_OPEN_FILENAME
                    else:
                        return FAILURE # Line does not match
                else:
                    # Not enough characters. Line does not match
                    return FAILURE
            else:
                # Expecting include, not found.
                return FAILURE
        elif state == EXPECT_OPEN_FILENAME:
            if c == ' ' or c == '\t':
                state = EXPECT_OPEN_FILENAME
            elif c == '<':
                header_filename_start_index = index+1
                include_end_char = '>'
                state = READ_FILENAME
            elif c == '"':
                header_filename_start_index = index+1
                include_end_char = '"'
                state = READ_FILENAME
            else:
                return FAILURE
        elif state == READ_FILENAME:
            if c == include_end_char:
                header_filename = uline[header_filename_start_index:index]
                return header_filename
            else:
                pass
        else:
            assert False, "Unknown state = %d" % state

        index += 1

    # If we got here, the line did not match.
    return FAILURE
                
        


class TargetFile:
    def __init__(self, language, name):
        self.language = language
        self.name = name
        self.code_prefixes = {} # code_filename -> "code_prefix"
        self.source_files = [] # list of filename-string
        self.header_files = [] # list of filename-string
        self.unrenamed_files = [] # list of filename-string
        self.target_prefix = ""
        self.target_suffix = ""

    def getLanguage(self):
        return self.language

    def getName(self):
        return self.name

    def getCodePrefix(self, code_filename):
        if code_filename in self.code_prefixes:
            return self.code_prefixes[code_filename]
        else:
            return ""

    def addCodePrefix(self, code_filename, code_prefix):
        self.code_prefixes[code_filename] = code_prefix

    def addSourceFile(self, source_filename):
        self.source_files.append(source_filename)

    def addHeaderFile(self, header_filename):
        self.header_files.append(header_filename)

    def addUnrenamedFile(self, unrenamed_filename):
        self.unrenamed_files.append(unrenamed_filename)

    def setTargetPrefix(self, target_prefix):
        self.target_prefix = target_prefix

    def setTargetSuffix(self, target_suffix):
        self.target_suffix = target_suffix
        
    def getTargetPrefix(self):
        return self.target_prefix

    def getTargetSuffix(self):
        return self.target_suffix

    def make_headers_available(self, result):
        for header_path in self.header_files:
            if header_path in self.unrenamed_files:
                result.setdefault(header_path, header_path)
            else:
                header_filename = os.path.split(header_path)[-1]
                result.setdefault(header_filename, header_path)

    def copy_file(self, fout, language, headers_available, source_filename):
        fout.write("/**************** A copy of %s *****************/\n" % source_filename)
        if language == 'C':
            fout.write("#ifdef __cplusplus\nextern \"C\" {\n#endif\n")

        fout.write("\n\n#line 1 \"%s\"\n" % source_filename)

        line_count = 0
        for line in open(os.path.join("..", source_filename), "rb"):
            line_count += 1
        
            uline = line.decode('utf-8')

            include_filename = match_include(uline)

            if include_filename == "":
                fout.write(uline.encode('utf-8'))
            else:
                if include_filename not in headers_available:
                    fout.write(uline.encode('utf-8'))
                elif headers_available[include_filename] != "":
                    fout.write("/**************** leaving %s temporarily *****************/\n" % source_filename)

                    header_path = headers_available[include_filename]

                    # Make sure we don't emit this header again.
                    headers_available[include_filename] = ""

                    # Copy the header
                    self.copy_file(fout, self.language, headers_available, header_path)

                    fout.write("/**************** continuing %s where we left off *****************/\n" % source_filename)
                    fout.write("\n\n#line %d \"%s\"\n" % (line_count, source_filename))
                else:
                    fout.write("/**************** already included %s -- not including again *****************/\n" % include_filename)
                    fout.write("\n\n#line %d \"%s\"\n" % (line_count, source_filename))
                

        if language == 'C':
            fout.write("#ifdef __cplusplus\n} /* End of 'extern \"C\"'. */\n#endif\n")

    def write_h_amalgamation(self, fout, headers_available):
        fout.write(self.target_prefix)
        
        for source_filename in self.header_files:
            if source_filename in self.unrenamed_files:
                header_filename = source_filename
            else:
                header_filename = os.path.split(source_filename)[-1]
                
            if header_filename in headers_available:
                bDoIt = headers_available[header_filename] != ""
                headers_available[header_filename] = ""
            else:
                bDoIt = False

            if bDoIt:
                code_prefix = self.getCodePrefix(source_filename)
                fout.write(code_prefix)
                    
                self.copy_file(fout, self.language, headers_available, source_filename)
            else:
                pass

        fout.write(self.target_suffix)


    def process(self, preamble, headers_available):
        fout = open(self.name, "wb")

        fout.write(preamble)

        prefix = self.getTargetPrefix()
        fout.write(prefix)
        
        for source_filename in self.source_files:
            code_prefix = self.getCodePrefix(source_filename)
            fout.write(code_prefix)
                
            self.copy_file(fout, self.language, headers_available, source_filename)


        suffix = self.getTargetSuffix()
        fout.write(suffix)

        fout.close()
        

    

class AmalgamationCreator:
    def __init__(self):
        self.target_files = [] # List of TargetFile objects
        self.target_header_files = {} # language-string -> target_header_filename-string
        self.ignored_header_files = [] # list of ignored_header_filename-strings
        self.preambles = {} # language-string -> preamble-string

    def addTargetHeaderFile(self, language, name):
        self.target_header_files.setdefault(language, name)

    def addIgnoredHeaderFile(self, name):
        self.ignored_header_files.append(name)

    def addTargetFile(self, target_file_obj):
        self.target_files.append(target_file_obj)

    def make_headers_available(self, language):
        result = {} # header_filename-string -> header_path-string

        # Gather header filenames from each TargetFile object
        for target_file_obj in self.target_files:
            language = target_file_obj.getLanguage()
            
            target_file_obj.make_headers_available(result)

        # Ignore these, and elide their inclusion.
        for header_filename in self.ignored_header_files:
            result[header_filename] = ""
        
        return result

    def getPreamble(self, language):
        if language in self.preambles:
            return self.preambles[language]
        else:
            return ""

    def addPreamble(self, language, preamble):
        self.preambles[language] = preamble
        
    def process(self):
        self.make_source_amalgamations()

        self.make_h_amalgamations()

    def make_source_amalgamations(self):
        for target_file_obj in self.target_files:
            language = target_file_obj.getLanguage()

            preamble = self.getPreamble(language)
            
            headers_available = self.make_headers_available(language)

            target_file_obj.process(preamble, headers_available)

    def make_h_amalgamations(self):
        for language in self.target_header_files:
            headers_available = self.make_headers_available(language)

            target_header_filename = self.target_header_files[language]

            fout = open(target_header_filename, "wb")

            fout.write("#ifndef EMDROS_H_\n#define EMDROS_H_\n\n")

            preamble = self.getPreamble(language)
            fout.write(preamble)

            for target_file_obj in self.target_files:
                target_file_language = target_file_obj.getLanguage()

                if target_file_language == language:
                    target_file_obj.write_h_amalgamation(fout, headers_available)

            fout.write("\n#endif /* !defined(EMDROS_H_) */\n")

            fout.close()



        
class AmalgamationXMLHandler(xml.sax.ContentHandler):
    def __init__(self):
        self.elemstack = []
        self.charstack = []
        self.objstack = []

        self.nixed_elements = set([])
        self.ignored_elements = set(["amalgamation",
                                     "target_preambles",
                                     "target_header_files",
                                     "ignored_header_files",
                                     "unrenamed_files",
                                     "target_files",
                                     "code_prefixes",
                                     "header_files",
                                     "source_files",
                                     "target_prefix",
                                     "target_suffix"])

        self.nixing_stack = []

        self.handled_elements = set(["target_preamble",
                                     "target_header_file",
                                     "ignored_header_file",
                                     "unrenamed_file",
                                     "target_file",
                                     "code_prefix",
                                     "header_file",
                                     "source_file"])

        self.char_handled_end_tags = set(["target_preamble",   
                                          "code_prefix",
                                          "target_prefix",
                                          "target_suffix"])

        self.ac_obj = AmalgamationCreator()
                                  
        
    def startDocument(self):
        pass

    def endDocument(self):
        pass

    def characters(self, data):
        self.charstack.append(data)

    def getCurElement(self):
        if len(self.elemstack) == 0:
            return ""
        else:
            return self.elemstack[-1]

    def handleChars(self, chars_before, bIsEndTag):
        if len(self.nixing_stack) > 0:
            bDoIt = False
        else:
            cur_element = self.getCurElement()

            if bIsEndTag and cur_element in self.char_handled_end_tags:
                bDoIt = True
            else:
                bDoIt = False
                
        if not bDoIt:
            return

        if cur_element == "code_prefix":
            self.cur_target_file.addCodePrefix(self.cur_code_prefix_name, chars_before)
        elif cur_element == "target_prefix":
            self.cur_target_file.setTargetPrefix(chars_before)
        elif cur_element == "target_suffix":
            self.cur_target_file.setTargetSuffix(chars_before)
        elif cur_element == "target_preamble":
            self.ac_obj.addPreamble(self.curlang, chars_before)
        else:
            raise Exception("cur end-tag </" + cur_element + "> is not handled in handleChars().")
            

    def startElement(self, tag, attributes):
        self.elemstack.append(tag)

        chars = "".join(self.charstack)
        del self.charstack
        self.charstack = []

        self.handleChars(chars, False)

        if tag in self.nixed_elements:
            self.nixing_stack.append(tag)
        elif len(self.nixing_stack) != 0:
            pass
        elif tag in self.handled_elements:
            self.handleElementStart(tag, attributes)
        elif tag in self.ignored_elements:
            pass
        else:
            raise Exception("Error: Unknown start-tag '<" + tag + ">'")


    def handleElementStart(self, tag, attributes):
        if tag == "target_preamble":
            self.curlang = attributes["language"]
        elif tag == "target_header_file":
            language = attributes["language"]
            name = attributes["name"]
            self.ac_obj.addTargetHeaderFile(language, name)
        elif tag == "ignored_header_file":
            name = attributes["name"]
            self.ac_obj.addIgnoredHeaderFile(name)
        elif tag == "unrenamed_file":
            name = attributes["name"]
            self.cur_target_file.addUnrenamedFile(name)
        elif tag == "target_file":
            language = attributes["language"]
            name = attributes["name"]
            self.cur_target_file = TargetFile(language, name)
        elif tag == "code_prefix":
            self.cur_code_prefix_name = attributes["name"]
        elif tag == "header_file":
            name = attributes["name"]
            self.cur_target_file.addHeaderFile(name)
        elif tag == "source_file":
            name = attributes["name"]
            self.cur_target_file.addSourceFile(name)
        else:
            raise Exception("Unhandled element start: " + tag)

    def handleElementEnd(self, tag):
        if tag == "target_preamble":
            pass # All done at start, and in handleChars()
        elif tag == "target_header_file":
            pass # All done at start
        elif tag == "ignored_header_file":
            pass # All done at start
        elif tag == "unrenamed_file":
            pass # All done at start
        elif tag == "target_file":
            self.ac_obj.addTargetFile(self.cur_target_file)
            self.cur_target_file = None
        elif tag == "code_prefix":
            self.cur_code_prefix_name = ""
        elif tag == "header_file":
            pass # All done at start
        elif tag == "source_file":
            pass # All done at start
        else:
            raise Exception("Unhandled element end: " + tag)




    def endElement(self, tag):
        chars = "".join(self.charstack)
        del self.charstack
        self.charstack = []

        self.handleChars(chars, True)

        if tag in self.nixed_elements:
            oldTag = self.nixing_stack.pop()
            assert tag == oldTag
        elif len(self.nixing_stack) != 0:
            pass
        elif tag in self.handled_elements:
            self.handleElementEnd(tag)
        elif tag in self.ignored_elements:
            pass
        else:
            raise Exception("Error: Unknown end-tag " + tag)

        self.elemstack.pop()

    def process(self):
        self.ac_obj.process()

def doIt(infilename):
    handler = AmalgamationXMLHandler()

    fin = io.open(infilename, "rb")
    sys.stderr.write("Now reading: %s ...\n" % infilename)
    xml.sax.parse(fin, handler)

    handler.process()

def usage(fout):
    fout.write("\nUSAGE:\n    python mkamalgamation.py <amalgamation-xml-filename>\n\n")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        usage(sys.stderr)
        sys.exit(1)
    else:
        arg = sys.argv[1]
        if arg == "--help":
            usage(sys.stderr)
        else:
            doIt(arg)
        sys.exit(0)

    




