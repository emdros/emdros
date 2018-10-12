# -*- coding: utf-8 -*-
#from __future__ import unicode_literals
#from builtins import bytes
#
# Written by Ulrik Sandborg-Petersen
#
# Copyright 2018  Sandborg-Petersen Holding ApS
#
# Made available under the MIT License.
#
# See the file COPYING in the root of the sources for details.
#
import xml.sax
import codecs
import sys
import re
import io

tag_re = re.compile(r'<[^>]+>')

punct_re = re.compile(r'[\.,;:\"?!()[]/\-]')

xml_entity_re = re.compile(r'(&quot;)|(&gt;)|(&lt;)|(&amp;)')

token_re = re.compile(r'(\s*)([^\s]*)(\s*)')

surface_re = re.compile(r'([^\w]*)(\w*)([^\w]*)')

whitespace_re = re.compile(r'^\s*$')

def usage():
    sys.stderr.write("Usage:\n     python osis2mql.py (--NT|--OT|--all) infilename outfilename\n")


def getBasename(pathname):
    filename = pathname.split("/")[-1]
    if "." in filename:
        basename = ".".join(filename.split(".")[:-1])
    else:
        basename = filename
    return basename
    

def mangleMQLString(ustr):
    result = u""
    for myint in bytes(ustr.encode('utf-8')):
        if type(myint) != type(0):
            myint = ord(myint)
            x = myint
        else:
            x = chr(myint)
            
        if x == b"\n":
            result += u"\\n"
        elif x == b"\t":
            result += u"\\t"
        elif x == b"\"":
            result += u"\\\""
        elif x == b"\\":
            result += u"\\\\"
        elif myint > 127:
            result += u"\\x%02x" % myint
        else:
            result += chr(myint)
    return result
    

def mangle_XML_entities(s):
    r = s.replace("&", "&amp;")
    r = r.replace("<", "&lt;")
    r = r.replace(">", "&gt;")
    r = r.replace("\"", "&quot;")
    return r

class Token:
    def __init__(self, monad, prefix, surface, suffix, xmlindex):
        self.monad = monad
        self.wholesurface = prefix + surface + suffix
        self.prefix = prefix
        self.surface = surface
        self.suffix = suffix
        self.xmlindex = xmlindex

    def dumpMQL(self, f):
        surface_lowcase = self.surface.lower();
        surface_stripped_lowcase = surface_re.findall(surface_lowcase)[0][1]

        f.write((u"CREATE OBJECT FROM MONADS={%d}\n" % self.monad).encode('utf-8'))
        f.write((u"[surface_stripped_lowcase:=\"%s\";\n" % (mangleMQLString(surface_stripped_lowcase))).encode('utf-8'))
        f.write((u"wholesurface:=\"%s\";xmlindex:=%d;\n]\n" % (mangleMQLString(self.wholesurface), self.xmlindex)).encode('utf-8'))


class SRObject:
    def __init__(self, objectTypeName, starting_monad):
        self.objectTypeName = objectTypeName
        self.fm = starting_monad
        self.lm = starting_monad
        self.stringFeatures = {}
        self.nonStringFeatures = {}
        self.id_d = 0

    def setID_D(self, id_d):
        self.id_d = id_d

    def setStringFeature(self, name, value):
        self.stringFeatures[name] = value

    def setNonStringFeature(self, name, value):
        self.nonStringFeatures[name] = value

    def getStringFeature(self, name):
        return self.stringFeatures[name]

    def setLastMonad(self, ending_monad):
        if ending_monad < self.fm:
            self.lm = self.fm
        else:
            self.lm = ending_monad

    def dumpMQL(self, fout):
        fout.write((u"CREATE OBJECT FROM MONADS={%d-%d}" % (self.fm, self.lm)).encode('utf-8'))
        if self.id_d != 0:
            fout.write((u"WITH ID_D=%d" % self.id_d).encode('utf-8'))
        fout.write((u"[").encode('utf-8'))
        for (key,value) in self.nonStringFeatures.items():
            fout.write((u"  %s:=%s;\n" % (key, value)).encode('utf-8'))
        for (key,value) in self.stringFeatures.items():
            fout.write((u"  %s:=\"%s\";\n" % (key, mangleMQLString(value))).encode('utf-8'))
        fout.write((u"]\n").encode('utf-8'))


class OSISHandler(xml.sax.ContentHandler):
    def __init__(self, first_monad):
        self.elemstack = []
        self.charstack = []
        self.objects = {} # objType --> [obj1,obj2,...]
        self.tokens = [] # list of Token objects
        self.objstack = []

        self.divtypestack = [""] # "" to always have one

        self.bInChapter = False
        self.bNixNote = False

        self.nixed_elements = set(["header", "work", "date", "type", "identifier", "language", "rights", "refSystem"])
        self.ignored_elements = set(["osis", "osisText", "w", "seg", "divineName", "inscription", "foreign", "q"])

        self.nixing_stack = []

        self.handled_elements = set(["div", "title", "chapter", "verse", "lb", "milestone", "note", "p"])
        self.simple_SR_elements = set(["transChange", "lg", "l"])

        self.start_monad = first_monad
        self.curmonad = first_monad
        self.xmlindex = 1

        self.tag2objectTypeName = {
            "p" : "paragraph",
            "title" : "title",
            "chapter" : "chapter",
            "verse" : "verse",
            "note" : "note",
            "transChange" : "transadd",
            "lg" : "linegroup",
            "l" : "line",
            }

        self.curBook = None # SRObject("book")
        self.curChapter = None # SRObject("chapter")
        self.curVerse = None # SRObject("verse")
        self.curParagraph = None # SRObject("paragraph")
        
    def startDocument(self):
        pass

    def endDocument(self):
        self.endVerse()
        self.endChapter()
        self.endBook()


    def characters(self, data):
        self.charstack.append(data)

    def createObject(self, objectTypeName):
        obj = SRObject(objectTypeName, self.curmonad)
        obj.setNonStringFeature("xmlindex", self.xmlindex)
        self.xmlindex += 1
        self.objstack.append(obj)
        return obj

    def createObjectSingleTag(self, objectTypeName):
        # The reason we subtract 1 for single tags (such as <br/>) is
        # that they do not anticipate something inside of them, and
        # so, since we have already added 1 to self.curmonad, we need
        # to associate it, not with the following token, but with the
        # previous token.
        obj = SRObject(objectTypeName, self.curmonad - 1)
        obj.setNonStringFeature("xmlindex", self.xmlindex)
        self.xmlindex += 1
        self.objstack.append(obj)
        return obj

    def endObject(self, objectTypeName):
        obj = self.objstack.pop()
        assert obj.objectTypeName == objectTypeName
        self.objects.setdefault(objectTypeName, []).append(obj)
        
    def addCurMonadToObjects(self):
        for obj in self.objstack:
            obj.setLastMonad(self.curmonad)

    def getCurElement(self):
        if len(self.elemstack) == 0:
            return ""
        else:
            return self.elemstack[-1]

    def handleChars(self, chars_before):
        if len(self.nixing_stack) > 0:
            bDoIt = False
        elif self.bNixNote:
            bDoIt = False
        elif self.getCurElement() == "title":
            bDoIt = True
        elif self.bInChapter:
            bDoIt = True
        else:
            bDoIt = False

        if not bDoIt:
            return

        chars = chars_before
        if whitespace_re.match(chars):
            #myToken = Token(self.curmonad, " ")
            #self.tokens.append(myToken)
            #self.curmonad += 1
            if len(self.tokens) != 0:
                t = self.tokens[-1]
                if len(t.wholesurface) == 0:
                    t.wholesurface += " "
                elif t.wholesurface[-1] in " \n\r\t":
                    pass
                else:
                    t.wholesurface += u" "
                    
        else:
            st_prefix = 1
            st_token = 2
            st_suffix = 3

            t = ""
            state = st_prefix

            for c in chars:
                if c in " \n\r\t":
                    if state == st_prefix:
                        t += c
                    elif state == st_token:
                        t += c
                        state = st_suffix
                    else:
                        t += c
                else:
                    if state == st_prefix:
                        t += c
                        state = st_token
                    elif state == st_token:
                        t += c
                    else:
                        self.addToken(t)
                        t = c
                        state = st_token
            if t != "":
                self.addToken(t)

    def addToken(self, prefix_surface_suffix):
        (prefix, surface, suffix) = token_re.findall(prefix_surface_suffix)[0]

        # Normalize prefix and suffix to be just one space
        if prefix != "":
            prefix = " "

        if suffix != "":
            suffix = " "

        myToken = Token(self.curmonad, prefix, surface, suffix, self.xmlindex)
        self.xmlindex += 1
        self.tokens.append(myToken)

        self.curmonad += 1
        


    def startElement(self, tag, attributes):
        self.elemstack.append(tag)

        chars = "".join(self.charstack)
        del self.charstack
        self.charstack = []

        self.handleChars(chars)

        if tag in self.nixed_elements:
            self.nixing_stack.append(tag)
        elif len(self.nixing_stack) != 0:
            pass
        elif tag in self.simple_SR_elements:
            objectTypeName = self.tag2objectTypeName[tag]
            self.createObject(objectTypeName)
        elif tag in self.handled_elements:
            self.handleElementStart(tag, attributes)
        elif tag in self.ignored_elements:
            pass
        else:
            raise Exception((u"Error: Unknown start-tag '<" + tag + ">'").encode('utf-8'))

        self.addCurMonadToObjects()

    def handleElementStart(self, tag, attributes):
        if tag == "verse":
            if u"eID" in attributes:
                pass
            else:
                self.endVerse()
                self.startVerse(attributes["osisID"])
        elif tag == u"milestone":
            if u"type" in attributes:
                mytype = attributes["type"]
                if mytype in set([u"x-extra-p", u"x-p"]):
                    self.endParagraph()
                    self.startParagraph()
                elif mytype == u"x-strongsMarkup":
                    pass
                else:
                    sys.stderr.write((u"milestone type=\"%s\"\n" % mytype).encode('utf-8'))
            else:
                pass
        elif tag == u"p":
            self.endParagraph()
            self.startParagraph()
        elif tag == "chapter":
            if u"eID" in attributes:
                pass
            else:
                self.endVerse()
                self.endChapter()
                self.startChapter(attributes["osisID"])
        elif tag == "title":
            if self.divtypestack[-1] == "bookGroup":
                obj = self.createObject("title")
                obj.setNonStringFeature("divtype", "bookGroup")
            elif self.divtypestack[-1] == "book":
                if self.bInChapter:
                    obj = self.createObject("title")
                    obj.setNonStringFeature("divtype", "chapter")
                else:
                    obj = self.createObject("title")
                    obj.setNonStringFeature("divtype", "book")
        elif tag == "div":
            self.divtypestack.append(attributes["type"])
            if self.divtypestack[-1] == "bookGroup":
                pass
            elif self.divtypestack[-1] == "book":
                self.endVerse()
                self.endChapter()
                self.endBook()
                self.startBook(attributes["osisID"])
        elif tag == u"note":
            if attributes.has_key("type"):
                if attributes["type"] in [u"study"]:
                    self.bNixNote = False
                    self.createObject("note")
                elif attributes["type"] in [u"x-strongsMarkup"]:
                    self.bNixNote = True
                else:
                    self.bNixNote = True
            else:
                self.bNixNote = True
        elif tag == u"lb":
            # Line break
            obj = self.createObjectSingleTag("br")
            

    def handleElementEnd(self, tag):
        if tag == "verse":
            pass # All done at start
        elif tag == "chapter":
            pass # All done at start
        elif tag == "title":
            if self.divtypestack[-1] == "bookGroup":
                self.endObject("title")
            elif self.divtypestack[-1] == "book":
                self.endObject("title")
        elif tag == "div":
            self.divtypestack.pop()
            if self.divtypestack[-1] == "bookGroup":
                pass # All done at start
            elif self.divtypestack[-1] == "book":
                pass # All done at start
        elif tag == u"note":
            if self.bNixNote:
                pass
            else:
                self.endObject("note")
            self.bNixNote = False
        elif tag == u"lb":
            # Line break
            obj = self.endObject("br")
        elif tag == u"milestone":
            pass
        elif tag == u"p":
            pass



    def startBook(self, osisID):
        #if osisID == "Gen":
        #    self.curmonad = 1000
        #elif osisID == "Matt":
        #    self.curmonad = 1000000

        obj = SRObject("book", self.curmonad)
        obj.setNonStringFeature("xmlindex", self.xmlindex)
        self.xmlindex += 1
        obj.setStringFeature("osisID", osisID)
        self.curBook = obj

    def endBook(self):
        if self.curBook != None:
            self.curBook.setLastMonad(self.curmonad-1)
            self.objects.setdefault("book", []).append(self.curBook)
            self.curBook = None

    def endParagraph(self):
        if self.curParagraph != None:
            self.curParagraph.setLastMonad(self.curmonad-1)
            self.objects.setdefault("paragraph", []).append(self.curParagraph)
            self.curParagraph = None

    def startParagraph(self):
        self.endParagraph()
        obj = SRObject("paragraph", self.curmonad)
        obj.setNonStringFeature("xmlindex", self.xmlindex)
        self.xmlindex += 1
        self.curParagraph = obj
        

    def startChapter(self, osisID):
        self.startParagraph()
        self.bInChapter = True
        obj = SRObject("chapter", self.curmonad)
        obj.setNonStringFeature("xmlindex", self.xmlindex)
        self.xmlindex += 1
        obj.setStringFeature("osisID", osisID)
        (osisBook, osisChapterStr) = osisID.split(".")
        obj.setStringFeature("osisBook", osisBook)
        obj.setNonStringFeature("chapter", osisChapterStr)
        self.curChapter = obj

    def endChapter(self):
        self.endParagraph()
        self.bInChapter = False
        if self.curChapter != None:
            self.curChapter.setLastMonad(self.curmonad-1)
            self.objects.setdefault("chapter", []).append(self.curChapter)
            self.curChapter = None


    def startVerse(self, osisID):
        obj = SRObject("verse", self.curmonad)
        obj.setStringFeature("osisID", " %s " % osisID)
        (osisBook, osisChapterStr, osisVerseStr) = osisID.split(".")
        obj.setStringFeature("osisBook", osisBook)
        obj.setNonStringFeature("chapter", osisChapterStr)
        obj.setNonStringFeature("verse", osisVerseStr)
        obj.setNonStringFeature("xmlindex", self.xmlindex)
        self.xmlindex += 1
        self.curVerse = obj

    def endVerse(self):
        if self.curVerse != None:
            self.curVerse.setLastMonad(self.curmonad-1)
            self.objects.setdefault("verse", []).append(self.curVerse)
            self.curVerse = None

    def endElement(self, tag):
        chars = "".join(self.charstack)
        del self.charstack
        self.charstack = []

        self.handleChars(chars)

        self.curmonad -= 1
        self.addCurMonadToObjects()
        self.curmonad += 1


        if tag in self.nixed_elements:
            oldTag = self.nixing_stack.pop()
            assert tag == oldTag
        elif len(self.nixing_stack) != 0:
            pass
        elif tag in self.simple_SR_elements:
            objectTypeName = self.tag2objectTypeName[tag]
            self.endObject(objectTypeName)
        elif tag in self.handled_elements:
            self.handleElementEnd(tag)
        elif tag in self.ignored_elements:
            pass
        else:
            raise ("Error: Unknown end-tag " + tag).encode('utf-8')

        self.elemstack.pop()


    def weed_object_types(self, object_type_list):
        for otn in object_type_list:
            new_list = []
            first_monad_dict = {} # fm --> [objlist]
            for obj in self.objects[otn]:
                first_monad_dict.setdefault(obj.fm, []).append(obj)
            
            for fm in sorted(first_monad_dict):
                if len(first_monad_dict[fm]) == 1:
                    new_list.extend(first_monad_dict[fm])
                else:
                    for obj in first_monad_dict[fm]:
                        if obj.fm == obj.lm:
                            pass
                        else:
                            new_list.append(obj)
            del self.objects[otn]
            self.objects[otn] = new_list

    def dumpMQL(self, fout):
        self.weed_object_types(["paragraph"])

        for objectTypeName in sorted(self.objects):
            count = 0

            sys.stderr.write("Now dumping [%s] ...\n" % objectTypeName)

            fout.write((u"BEGIN TRANSACTION GO\n").encode('utf-8'))
            fout.write((u"CREATE OBJECTS WITH OBJECT TYPE [%s]\n" % objectTypeName).encode('utf-8'))
            for obj in self.objects[objectTypeName]:
                obj.dumpMQL(fout)
                count += 1
                if count == 50000:
                    fout.write((u"GO COMMIT TRANSACTION GO\nBEGIN TRANSACTION GO\n").encode('utf-8'))
                    fout.write((u"CREATE OBJECTS WITH OBJECT TYPE [%s]\n" % objectTypeName).encode('utf-8'))
                    count = 0
            fout.write(u"""GO
COMMIT TRANSACTION GO
""".encode('utf-8'))


        count = 0

        sys.stderr.write("Now dumping [Token] ...\n")

        fout.write((u"BEGIN TRANSACTION GO\n").encode('utf-8'))
        fout.write((u"CREATE OBJECTS WITH OBJECT TYPE [Token]\n").encode('utf-8'))
        for obj in self.tokens:
            obj.dumpMQL(fout)
            count += 1
            if count == 50000:
                fout.write((u"GO COMMIT TRANSACTION GO\nBEGIN TRANSACTION GO\n").encode('utf-8'))
                fout.write((u"CREATE OBJECTS WITH OBJECT TYPE [Token]\n").encode('utf-8'))
                count = 0
        fout.write((u"GO\n").encode('utf-8'))
        fout.write((u"COMMIT TRANSACTION GO\n").encode('utf-8'))

        
        fout.write((u"VACUUM DATABASE ANALYZE GO\n").encode('utf-8'))

        sys.stderr.write("Finished dumping!\n")
        



if len(sys.argv) != 4:
    usage()
    sys.exit(1)
else:
    if sys.argv[1] == "--OT":
        first_monad = 1
    elif sys.argv[1] == "--NT":
        first_monad = 1000000
    elif sys.argv[1] == "--all":
        first_monad = 1
    else:
        usage()
        sys.exit(1)
        
    filename = sys.argv[2]

    handler = OSISHandler(first_monad)

    fin = io.open(filename, "rb")
    sys.stderr.write("Now reading: %s ...\n" % filename)
    xml.sax.parse(fin, handler)

    fout = open(sys.argv[3], "wb")
    handler.dumpMQL(fout)
    fout.close()

