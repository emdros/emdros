# -*- coding: utf-8 -*-
import os
import re
import sys
from config import *

LOOKING_FOR_START_OF_PAGE = 1
PARSING_PAGE = 2

def addFileIfExists(fout, infilename):
    if os.access(infilename, os.F_OK):
        fin = open(infilename, "rb")
        for line in fin.readlines():
            fout.write(line)
        fin.close()
    else:
        # Only add if file exists and is accessible 
        pass

class FilesEntry:
    def __init__(self):
        self.mylist = []

    def readFromLine(self, line):
        self.mylist = line.strip().decode('utf-8').split("\t")

    def getID(self):
        print(self.mylist[0])
        return int(self.mylist[0])

    def getTitle(self):
        return self.mylist[1]

    def getNext(self):
        return int(self.mylist[2])

    def getUp(self):
        return int(self.mylist[3])

    def getType(self):
        return self.mylist[4]

    def __cmp__(self, other):
        return cmp(self.getID(), other.getID())

class Hierarchy:
    def __init__(self):
        self.entries = {}
        self.nextrelation = {}
        self.prevrelation = {}
        self.docline = []
        self.levels = {}

    def readFromFile(self, f):
        for line in f.readlines():
            fe = FilesEntry()
            fe.readFromLine(line)
            self.entries[fe.getID()] = fe
        self.calculateNextPrevRelations()
        self.calculateDocLine()
        self.calculateLevels()

    def calculateDocLine(self):
        id = 1000
        while id != -1:
            self.docline.append(id)
            id = self.getNext(id)
            print("UP201: id =", id)

    def calculateLevels(self):
        self.levels[1000] = 0
        for id in self.docline:
            if id != -1 and id != 1000:
                self.levels[id] = self.levels[self.entries[id].getUp()] + 1

    def calculateNextPrevRelations(self):
        for id in self.entries.keys():
            fe = self.entries[id]
            self.nextrelation[id] = fe.getNext()
        for id in self.nextrelation.keys():
            next = self.nextrelation[id]
            self.prevrelation[next] = id

    def getEntry(self, id):
        return self.entries[id]

    def getNext(self, id):
        try:
            return self.nextrelation[id]
        except:
            return -1

    def getImmediateChildren(self, id):
        result = []
        for childid in self.docline:
            upid = self.getUp(childid)
            if upid == id:
                result.append(childid)
        return result

    def getPrev(self, id):
        try:
            return self.prevrelation[id]
        except Exception:
            return -1

    def getUp(self, id):
        return self.entries[id].getUp()

    def getIDs(self):
        return self.entries.keys()

    def getLinkFromId(self, id, extension):
        if id == -1:
            return "none"
        else:
            fe = self.entries[id]
            return "<A HREF=\"%s\">%s</A>" % (str(id) + str(extension), fe.getTitle())
    def idIsBelowOther(self, other_id, id):
        up_id = self.getUp(id)

        if up_id == -1:
            return False
        elif up_id == other_id:
            return True
        else:
            return self.idIsBelowOther(other_id, up_id)


class Pages:
    def __init__(self):
        self.pages = {}
        self.myindex = {}

    def readFromFile(self, f):
        pagestartre = re.compile(r'^<page[ ]+ID="([0-9]+)">')
        pageendre = re.compile(r'^</page>')
        iState = LOOKING_FOR_START_OF_PAGE
        id = 0
        for line in f:
            line = line.decode('utf-8')
            if iState == LOOKING_FOR_START_OF_PAGE:
                mo = pagestartre.match(line)
                if mo != None:
                    id = int(line[mo.start(1):mo.end(1)])
                    self.pages[id] = ""
                    iState = PARSING_PAGE
            elif iState == PARSING_PAGE:
                mo = pageendre.match(line)
                if mo != None:
                    iState = LOOKING_FOR_START_OF_PAGE
                else:
                    self.pages[id] = self.pages[id] + line

    def getPage(self, id):
        return self.pages[id]

    def writeHTMLPages(self, hierarchy, extension, bWithNavigation):
        ids = list(sorted(hierarchy.getIDs()))
        for id in ids:
            self.writeHTMLFile(id, hierarchy, extension, bWithNavigation)

    def writeHTMLFile(self, id, hierarchy, extension, bWithNavigation):
        filename = str(id) + str(extension)
        print(filename)
        f = open(filename, "wb")
        if bWithNavigation:
            css_header = "    <link href=\"cms.css\" rel=\"stylesheet\" type=\"text/css\">"
        else:
            css_header = ""
        css_header += "\n        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n"

        self.writeHTMLHeader(f, id, hierarchy, css_header)
        self.writeHTMLPage(f, id, hierarchy, extension, bWithNavigation)
        self.writeHTMLFooter(f, id, hierarchy, extension)
        f.close()

    def writeHTMLHeader(self, f, id, hierarchy, css_header):
        f.write("""<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" 
        "http://www.w3.org/TR/REC-html40/strict.dtd">\n""".encode('utf-8'))
        f.write(("<html>\n").encode('utf-8'))
        f.write(("  <head>\n").encode('utf-8'))
        f.write(("     <title>" + hierarchy.getEntry(id).getTitle() + "</title>\n").encode('utf-8'))
        f.write((css_header + "\n").encode('utf-8'))
        f.write("  </head>\n".encode('utf-8'))
        f.write("<body>)\n".encode('utf-8'))

    def writeHTMLPage(self, f, id, hierarchy, extension, bWithNavigation):
        if bWithNavigation:
            f.write(("<TABLE cellspacing=\"0\" cellpadding=\"0\" class=\"page\" width=\"100%\">\n").encode('utf-8'))
            f.write(("<TR>\n").encode('utf-8'))
            f.write(("<TD CLASS=\"menu\">\n").encode('utf-8'))
            f.write(("<DIV CLASS=\"hjem\"><A CLASS=\"menu\" HREF=\"http://emdros.org/\">Home</A></DIV>\n").encode('utf-8'))
            f.write(("<BR><BR><DIV CLASS=\"PDF\"><A CLASS=\"menu\" HREF=\"%s.pdf\">PDF Version</A></DIV>\n" % PDF_PREFIX).encode('utf-8'))
            f.write(("<br><br><DIV CLASS=\"indhold\">Contents:</DIV>\n").encode('utf-8'))
            self.writeNavigationLeft(f, 1000, id, hierarchy, extension)
            f.write(("<br>\n").encode('utf-8'))
            f.write(("<DIV CLASS=\"search\">Search:</DIV><br>\n").encode('utf-8'))
            f.write(("<FORM ACTION=\"search.php\" METHOD=\"GET\">\n").encode('utf-8'))
            f.write(("<INPUT TYPE=\"HIDDEN\" NAME=\"static_links\" VALUE=\"true\">\n").encode('utf-8'))
            f.write(("<INPUT TYPE=\"TEXT\" NAME=\"terms\" SIZE=\"23\" ALT=\"Write search terms\">\n").encode('utf-8'))
            f.write(("<INPUT TYPE=\"SUBMIT\" VALUE=\"Search\">\n").encode('utf-8'))
            f.write(("</FORM>\n").encode('utf-8'))
            f.write(("\n").encode('utf-8'))
            addFileIfExists(f, "sidebar.inc")
            f.write(("</TD><TD class=\"main\">\n").encode('utf-8'))
            f.write(("\n").encode('utf-8'))
        f.write(("<H1>%s</H1>\n" % hierarchy.getEntry(id).getTitle()).encode('utf-8'))
        pagestring = self.pages[id]
        pagestring = self.expandTags(pagestring, id, hierarchy, extension)
        f.write(pagestring.encode('utf-8'))
        if bWithNavigation:
            f.write(("</TD>\n").encode('utf-8'))
            f.write(("</TR>\n").encode('utf-8'))
            f.write(("</TABLE>\n").encode('utf-8'))

    def writeNavigationLeft(self, f, main_id, focus_id, hierarchy, extension):
        arr_subsections = hierarchy.getImmediateChildren(main_id)

        if len(arr_subsections) != 0:
            bIsNotTOC = main_id != 1000

            if bIsNotTOC:
                f.write(("<DIV class=\"menu\">\n").encode('utf-8'))

            for child_id in arr_subsections:
                entry = hierarchy.getEntry(child_id)
                child_title = entry.getTitle()

                if entry.getType() == "Part":
                    bChildIsPart = True
                else:
                    bChildIsPart = False

                if bChildIsPart:
                    f.write(("<br>").encode('utf-8'))
                    f.write(("<DIV class=\"part\">").encode('utf-8'))
                else:
                    f.write(("<DIV class=\"menu-item\">").encode('utf-8'))
                    f.write(("<IMG SRC=\"dot.gif\">&nbsp;").encode('utf-8'))

                if child_id == focus_id:
                    f.write(("<em>%s</em>" % child_title).encode('utf-8'))
                else:
                    child_link = ("<A class=\"menu\" HREF=\"%d%s\">%s</A>" % (child_id, extension, child_title))
                    f.write(child_link.encode('utf-8'))

                f.write(("</DIV>").encode('utf-8'))

                bFocusDocIsBelow = hierarchy.idIsBelowOther(child_id, focus_id)

                if bFocusDocIsBelow or focus_id == child_id:
                    self.writeNavigationLeft(f, child_id, focus_id, hierarchy, extension)
                    
            if bIsNotTOC:
                f.write(("</DIV>").encode('utf-8'))


    def writeHTMLFooter(self, f, id, hierarchy, extension):
        f.write(("<hr>\n").encode('utf-8'))
        if id != 1000 and hierarchy.getPrev(id) != 1000:
            f.write((("<strong>Previous:</strong> " + hierarchy.getLinkFromId(hierarchy.getPrev(id), extension) + "<br>\n").encode('utf-8')))
        if id != 1000 and hierarchy.getUp(id) != 1000:
            f.write((("<strong>Up:</strong> " + hierarchy.getLinkFromId(hierarchy.getUp(id), extension) + "<br>\n").encode('utf-8')))
        f.write((("<strong>Next:</strong> " + hierarchy.getLinkFromId(hierarchy.getNext(id), extension) + "<br>\n").encode('utf-8')))
        f.write(("</body>\n").encode('utf-8'))
        f.write(("</html>").encode('utf-8'))

    def writeOneBigHTMLPage(self, bookname, title, subtitle, hierarchy):
        filename = "%s.htm" % bookname
        f = open(filename, "wb")
        f.write(("""<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" 
        "http://www.w3.org/TR/REC-html40/strict.dtd">\n""").encode('utf-8'))
        f.write(("<html>\n").encode('utf-8'))
        f.write(("  <head>\n").encode('utf-8'))
        f.write(("     <title>" + title + "</title>\n").encode('utf-8'))
        f.write(("  </head>\n").encode('utf-8'))
        f.write(("<body>\n").encode('utf-8'))

        for id in hierarchy.docline:
            if id != 1000:
                f.write(("<H%d><A NAME=\"%s\">%s</A></H%d>\n" % (hierarchy.levels[id],str(id),hierarchy.getEntry(id).getTitle(),hierarchy.levels[id])).encode('utf-8'))
                pagestring = self.pages[id]
                pagestring = self.expandTags(pagestring, id, hierarchy, None)
                f.write((pagestring).encode('utf-8'))
                #f.write(("<br><br><hr><br><br>").encode('utf-8'))
        f.close()

    def getSection(self, level, bEscapeSlash = False):
        if bEscapeSlash:
            slash = "\\\\"
        else:
            slash = "\\"
            
        if level == 1:
            return "%ssection" % slash
        elif level == 2:
            return "%ssubsection" % slash
        elif level == 3:
            return "%ssubsubsection"  % slash
        else:
            return "%sparagraph"  % slash

    def writeOneBigLaTeXPage(self, bookname, title, subtitle, hierarchy):
        filename = "%s.tex" % bookname
        f = open(filename, "wb")
        f.write(("\\documentclass[a4paper]{article}\n").encode('utf-8'))
        f.write(("\\usepackage{graphicx}\n").encode('utf-8'))
        f.write(("\\usepackage{ucs}\n").encode('utf-8'))
        f.write(("\\usepackage{html}\n").encode('utf-8'))
        f.write(("\\title{%s\\newline\n%s}\n" % (title, subtitle)).encode('utf-8'))
        f.write(("\\author{Ulrik Sandborg-Petersen}\n").encode('utf-8'))
        try:
            f.write(("\\date{%s}\n" % DOC_DATE).encode('utf-8'))
        except:
            pass # If DOC_DATE isn't available, print distribution date by default
        f.write(("\\begin{document}\n").encode('utf-8'))
        f.write(("""\\newenvironment{precode}
        {\\begin{list}{}{
        \\setlength{\\rightmargin}{\\leftmargin}
        \\setlength{\\listparindent}{0pt}% needed for AMS classes
        \\raggedright
        \\setlength{\\itemsep}{0pt}
        \\setlength{\\parsep}{0pt}
        \\normalfont\\ttfamily}%
        \\item[]}
        {\\end{list}}\n""").encode('utf-8'))
        f.write(("\\maketitle\n").encode('utf-8'))

        f.write(("\\tableofcontents\n").encode('utf-8'))

        for id in hierarchy.docline:
            if id != 1000:
                f.write(("%s{%s}\n" % (self.getSection(hierarchy.levels[id]),hierarchy.getEntry(id).getTitle().replace("&quot;","\""))).encode('utf-8'))
                pagestring = self.pages[id]
                pagestring = self.expandTagsLaTeX(pagestring, id, hierarchy)
                f.write((pagestring).encode('utf-8'))
        f.write(("\n\\end{document}\n").encode('utf-8'))
        f.close()


    def expandTags(self, pagestring, id, hierarchy, extension):
        page_anchor_re = re.compile(r'<page_anchor[ \t\n]+ID=\"([0-9]+)">')
        page_anchor_end_re = re.compile(r'</page_anchor>')
        index_tag_re = re.compile(r'<index[ \t\n]+term="[^"]+">')
        tmpstr = page_anchor_end_re.sub("</A>", pagestring)

        # Take care of page_ref
        page_ref_re = re.compile(r'<page_ref[ \t\n]+ID=\"([0-9]+)"[ \t\n]*[/]?>')
        ref_list = re.split(r'(<page_ref[ \t\n]+ID=\"([0-9]+)"[ \t\n]*[/]?>)', tmpstr)
        tmpstr = ref_list[0]
        if len(ref_list) == 1:
            pass
        else:
            for index in range(1, len(ref_list), 3):
                ref_id = int(ref_list[index+1])
                ref_title = hierarchy.getEntry(int(ref_id)).getTitle()
                tmpstr = tmpstr + r'<A HREF="%d%s">%s</A>' % (ref_id, extension, ref_title)
                if index + 2 < len(ref_list):
                    tmpstr = tmpstr + ref_list[index+2]
            
        
        if extension is None:
            h2_re = re.compile(r'<[Hh]2>')
            h2_re_end = re.compile(r'</[Hh]2>')
            tmpstr = page_anchor_re.sub(r'<A HREF="#\1">', tmpstr)
            level = hierarchy.levels[id] + 1
            tmpstr = h2_re.sub("<h%d>" % level, tmpstr)
            tmpstr = h2_re_end.sub("</h%d>" % level, tmpstr)
        else:
            tmpstr = page_anchor_re.sub(r'<A HREF="\1%s">' % extension, tmpstr)
        tmpstr = index_tag_re.sub("", tmpstr)
        tmpstr = tmpstr.replace("{PREBACKSLASH}", "\\")
        tmpstr = tmpstr.replace("{PREUNDERSCORE}", "_")
        tmpstr = tmpstr.replace("{PREHASH}", "#")
        tmpstr = tmpstr.replace("{OPENBRACE}", "{")
        tmpstr = tmpstr.replace("{PREOPENBRACE}", "{")
        tmpstr = tmpstr.replace("{CLOSEBRACE}", "}")
        tmpstr = tmpstr.replace("{PRECLOSEBRACE}", "}")
        tmpstr = tmpstr.replace("{PRELT}", "&lt;")
        tmpstr = tmpstr.replace("{PREGT}", "&gt;")
        tmpstr = tmpstr.replace("{PREAMP}", "&amp;")
        tmpstr = tmpstr.replace("{PREDOLLAR}","$")
        tmpstr = tmpstr.replace("{PREPIPE}", "|")
        return tmpstr

    def expandTagsLaTeX(self, pagestring, id, hierarchy):
        # To strip...
        to_strip_re = re.compile(r'(<index[ \t\n]+term="[^"]+">)|(<A[^>]+>)|(</[Aa]>)|(<page_anchor[ \t\n]+ID=\"([0-9]+)">)|(</page_anchor>)')
        tmpstr = to_strip_re.sub("", pagestring)

        # Take care of page_ref
        page_ref_re = re.compile(r'<page_ref[ \t\n]+ID=\"([0-9]+)"[ \t\n]*[/]?>')
        ref_list = re.split(r'(<page_ref[ \t\n]+ID=\"([0-9]+)"[ \t\n]*[/]?>)', tmpstr)
        tmpstr = ref_list[0]
        if len(ref_list) == 1:
            pass
        else:
            for index in range(1, len(ref_list), 3):
                ref_id = int(ref_list[index+1])
                ref_title = hierarchy.getEntry(int(ref_id)).getTitle()
                tmpstr = tmpstr + "%s" % ref_title
                if index + 2 < len(ref_list):
                    tmpstr = tmpstr + ref_list[index+2]

        # Simple replacements
        tmpstr = tmpstr.replace("&quot;", "\"")
        tmpstr = tmpstr.replace("$","{DOLLAR}")
        tmpstr = tmpstr.replace("\\","$\\backslash$")
        tmpstr = tmpstr.replace("_","\\_")
        tmpstr = tmpstr.replace("&nbsp;", "\\ ")
        tmpstr = tmpstr.replace("&lt;", "$<$")
        tmpstr = tmpstr.replace("&gt;", "$>$")
        tmpstr = tmpstr.replace("&amp;", "&")
        tmpstr = tmpstr.replace("&","\&")
        tmpstr = tmpstr.replace("{PREBACKSLASH}", "\\")
        tmpstr = tmpstr.replace("{PREUNDERSCORE}","_")
        tmpstr = tmpstr.replace("#", "\\#")
        tmpstr = tmpstr.replace("{DOLLAR}", "\\$")
        tmpstr = tmpstr.replace("{PREAMP}", "&")
        tmpstr = tmpstr.replace("{PREDOLLAR}","$")
        tmpstr = tmpstr.replace("^", "\\^")
        tmpstr = tmpstr.replace("{OPENBRACE}", "\\{")
        tmpstr = tmpstr.replace("{CLOSEBRACE}", "\\}")
        tmpstr = tmpstr.replace("|", "$|$")
        tmpstr = tmpstr.replace("{PREPIPE}", "|")
        tmpstr = tmpstr.replace("æ", "{\\ae}").replace("ø", "{\\o}").replace("å", "{\\aa}")
        tmpstr = tmpstr.replace("Æ", "{\\AE}").replace("Ø", "{\\O}").replace("Å", "{\\AA}")
        tmpstr = tmpstr.replace("é", "{\\'e}").replace("É", "{\\'E}")
        tmpstr = tmpstr.replace("à", "{\\`a}")
        tmpstr = tmpstr.replace("í", "{\\'i}")
        tmpstr = tmpstr.replace("ñ", "{\\~n}")


        # IMG
        p_img_re = re.compile(r'<[Pp]><[Ii][Mm][Gg][^>]+[Ss][Rr][Cc]="([^"]+)"[^>]*></[Pp]>')
        tmpstr = p_img_re.sub(r'\n\n\\bigskip\n\n\\noindent\\includegraphics[scale=0.4]{\1}\n\\bigskip\n', tmpstr)
        img_re = re.compile(r'<[Ii][Mm][Gg][^>]+[Ss][Rr][Cc]="([^"]+)"[^>]*>')
        tmpstr = img_re.sub(r'\\noindent\\includegraphics[scale=0.4]{\1}', tmpstr)
        
        # H2
        h2_re = re.compile(r'<[Hh]2>')
        h2_re_end = re.compile(r'</[Hh]2>')
        level = hierarchy.levels[id] + 1
        tmpstr = h2_re.sub("%s{" % self.getSection(level, True), tmpstr)
        tmpstr = h2_re_end.sub("}\n", tmpstr)

        # H3
        h3_re = re.compile(r'<[Hh]3>')
        h3_re_end = re.compile(r'</[Hh]3>')
        level = hierarchy.levels[id] + 2
        tmpstr = h3_re.sub("%s{" % self.getSection(level, True), tmpstr)
        tmpstr = h3_re_end.sub("}\n\n", tmpstr)

        # BR
        br_re = re.compile(r'<[Bb][Rr]>')
        tmpstr = br_re.sub("\\\\newline\n", tmpstr)
        # PRE
        preMQLExample_re = re.compile(r'<[Pp][Rr][Ee][ \n\t]+class="MQLExample"[^>]*>([^<]+)</[Pp][Rr][Ee]>')
        tmpstr = preMQLExample_re.sub(r'\\begin{verbatim}\n\1\n\\end{verbatim}', tmpstr)
        preCODE_re = re.compile(r'<[Pp][Rr][Ee][ \n\t]+class="code"[^>]*>([^<]+)</[Pp][Rr][Ee]><!-- widthincm[^:]*:[^0-9]*([0-9]+)[^-]*-->')
        tmpstr = preCODE_re.sub(r'\\begin{verbatim}\n\1\n\\end{verbatim}', tmpstr)
        preINTERFACE_re = re.compile(r'<[Pp][Rr][Ee][ \n\t]+class="interface"[^>]*>([^<]+)</[Pp][Rr][Ee]><!-- widthincm[^:]*:[^0-9]*([0-9]+)[^-]*-->')
        tmpstr = preINTERFACE_re.sub(r'\\begin{verbatim}\n\1\n\\end{verbatim}', tmpstr)
        pre_re = re.compile(r'<[Pp][Rr][Ee][^>]*>([^<]+)</[Pp][Rr][Ee]>[^<]*<!-- widthincm[^:]*:[^0-9]*([0-9]+)[^-]*-->')
        tmpstr = pre_re.sub(r'\\begin{minipage}{\2cm}\\begin{verbatim}\n\1\n\\end{verbatim}\\end{minipage}', tmpstr)
        

        # UL / OL / LI
        ul_re = re.compile(r'<[Uu][Ll]>')
        ul_re_end = re.compile(r'</[Uu][Ll]>')
        tmpstr = ul_re.sub(r'\\begin{itemize}', tmpstr)
        tmpstr = ul_re_end.sub(r'\\end{itemize}', tmpstr)
        ol_re = re.compile(r'<[Oo][Ll]>')
        ol_re_end = re.compile(r'</[Oo][Ll]>')
        tmpstr = ol_re.sub(r'\\begin{enumerate}', tmpstr)
        tmpstr = ol_re_end.sub(r'\\end{enumerate}', tmpstr)
        il_re = re.compile(r'<[Ll][Ii]>')
        tmpstr = il_re.sub(r'\n\\item ', tmpstr)

        # STRONG
        strong_re = re.compile(r'<[Ss][Tt][Rr][Oo][Nn][Gg]>')
        strong_re_end = re.compile(r'</[Ss][Tt][Rr][Oo][Nn][Gg]>')
        tmpstr = strong_re.sub(r'{\\bf ', tmpstr)
        tmpstr = strong_re_end.sub(r'}', tmpstr)

        # B
        b_re = re.compile(r'<[Bb]>')
        b_re_end = re.compile(r'</[Bb]>')
        tmpstr = b_re.sub(r'{\\bf ', tmpstr)
        tmpstr = b_re_end.sub(r'}', tmpstr)

        # TABLE / TR / TD
        table_re = re.compile(r'<[Tt][Aa][Bb][Ll][Ee][^>]*>[^<]*<!-- columns[^:]*:[^0-9]*([0-9]+)[^-]*-->')
        table_re_end = re.compile(r'</[Tt][Aa][Bb][Ll][Ee]>')
        tr_td_re = re.compile(r'<[Tt][Rr]>[^<]*<[Tt][DdHh]>')
        td_td_re_end = re.compile(r'[ \n]*</[Tt][DdHh]>[^<]*<[Tt][DdHh]>')
        td_tr_re_end = re.compile(r'</[Tt][DdHh]>[^<]*</[Tt][Rr]>')
        tmpstr = table_re.sub(r'\n\\bigskip\n\\begin{tabular}{*{\1}{|l}|}\n\\hline', tmpstr)
        tmpstr = table_re_end.sub(r'\n\\end{tabular}\n\\bigskip\n', tmpstr)
        tmpstr = tr_td_re.sub(r'\\begin{minipage}[t]{4cm}', tmpstr)
        tmpstr = td_td_re_end.sub(r'\\end{minipage} & \\begin{minipage}[t]{4cm}', tmpstr)
        tmpstr = td_tr_re_end.sub(r'\\end{minipage}\\\\\\hline', tmpstr)



        # P align="center"

        # P
        p_re = re.compile(r'<[Pp]>')
        p_re_end = re.compile(r'</[Pp]>')
        p_center_re_remove = re.compile(r'<[Pp][ \t\n]+align="center"[ \t\n]*>')
        tmpstr = p_re_end.sub(r'\n', tmpstr)
        tmpstr = p_center_re_remove.sub(r'\n', tmpstr)
        tmpstr = p_re.sub(r'\n\n', tmpstr)

        # EM
        em_re = re.compile(r'<[Ee][Mm]>')
        em_re_end = re.compile(r'</[Ee][Mm]>')
        tmpstr = em_re.sub(r'{\\em ', tmpstr)
        tmpstr = em_re_end.sub(r'}', tmpstr)

        # BIG
        big_re = re.compile(r'<[Bb][Ii][Gg]>')
        big_re_end = re.compile(r'</[Bb][Ii][Gg]>')
        tmpstr = big_re.sub("{\\\\Large ", tmpstr)
        tmpstr = big_re_end.sub("}", tmpstr)

        # CENTER
        center_re = re.compile(r'<[Cc][Ee][Nn][Tt][Ee][Rr]>')
        center_re_end = re.compile(r'</[Cc][Ee][Nn][Tt][Ee][Rr]>')
        tmpstr = center_re.sub(r'\n\\begin{center}\n', tmpstr)
        tmpstr = center_re_end.sub(r'\n\\end{center}\n', tmpstr)



        tmpstr = tmpstr.replace("{PREHASH}", "#").replace("{PRELT}","<").replace("{PREGT}",">").replace("{PREOPENBRACE}","{").replace("{PRECLOSEBRACE}","}")

        tmpstr = tmpstr.replace("%", "\\%")

        return tmpstr

    def writeHHC(self, bookname, hierarchy, extension):
        f = open(bookname + ".hhc", "wb")
        top_level_ids = hierarchy.getImmediateChildren(1000)
        self.writeHHCList(f, top_level_ids, hierarchy, extension)
        f.close()

    def writeHHCList(self, f, idlist, hierarchy, extension):
        if len(idlist) > 0:
            f.write(("<ul>\n").encode('utf-8'))
            for id in idlist:
                f.write(("  <li><object type=\"text/sitemap\">\n").encode('utf-8'))
                f.write(("          <param name=\"Name\" value=\"%s\">\n" % hierarchy.getEntry(id).getTitle()).encode('utf-8'))
                f.write(("          <param name=\"ID\" value=%d>\n" % id).encode('utf-8'))
                f.write(("          <param name=\"Local\" value=\"%d%s\">\n" % (id, str(extension))).encode('utf-8'))
                f.write(("      </object>\n").encode('utf-8'))
                id_children = hierarchy.getImmediateChildren(id)
                self.writeHHCList(f, id_children, hierarchy, extension)
            f.write(("</ul>\n").encode('utf-8'))

    def writeHHK(self, bookname, hierarchy, extension):
        self.produceIndex(hierarchy)
        f = open(bookname + ".hhk", "wb")
        topics = list(sorted(self.myindex.keys()))
        if len(topics) > 0:
            f.write(("<ul>\n").encode('utf-8'))
            for topic in topics:
                idlist = self.myindex[topic]
                idlist.sort()
                for id in idlist:
                    f.write(("  <li><object type=\"text/sitemap\">\n").encode('utf-8'))
                    f.write(("      <param name=\"Name\" value=\"%s\">\n" % topic).encode('utf-8'))
                    f.write(("      <param name=\"Local\" value=\"%d%s\">\n" % (id, extension)).encode('utf-8'))
                    f.write(("      </object>\n").encode('utf-8'))
                    
            f.write(("</ul>\n").encode('utf-8'))
        f.close()

    def produceIndex(self, hierarchy):
        index_tag_re = re.compile(r'<index[ \t\n]+term="([^"]+)">')
        for pageid in self.pages.keys():
            title = hierarchy.getEntry(pageid).getTitle()
            if title not in self.myindex.keys():
                self.myindex[title] = []
            self.myindex[title].append(pageid)
            pagestring = self.pages[pageid]
            indexterms = index_tag_re.findall(pagestring)
            if len(indexterms) > 0:
                for topic in indexterms:
                    if topic not in self.myindex.keys():
                        self.myindex[topic] = []
                    self.myindex[topic].append(pageid)
                

    
def doit(bookname, booktitle, subtitle, contents_filename, files_filename, purpose):
    # Read "files.txt"
    f = open(files_filename, "rb")
    hierarchy = Hierarchy()
    hierarchy.readFromFile(f)
    f.close()

    # Read "Content.txt"
    f = open(contents_filename, "rb")
    pages = Pages()
    pages.readFromFile(f)
    f.close()

    if purpose == "htb":
        pages.writeHTMLPages(hierarchy, ".htm", False)
        pages.writeHHC(bookname, hierarchy, ".htm")
        pages.writeHHK(bookname, hierarchy, ".htm")
    elif purpose == "single":
        pages.writeOneBigHTMLPage(bookname, booktitle, subtitle, hierarchy)
    elif purpose == "latex":
        pages.writeOneBigLaTeXPage(bookname, booktitle, subtitle, hierarchy)
    elif purpose == "web":
        pages.writeHTMLPages(hierarchy, ".html", True)
    else:
        raise Exception("Error: Unknown purpose '%s'" % purpose)

# Do htb by default
purpose = "htb"

if len(sys.argv) >= 2:
    arg1 = sys.argv[1]
    if arg1[0:2] == "--":
        purpose = arg1[2:]
    else:
        print("Unknown switch: %s" % arg1)
        sys.exit(1)


        

doit(PDF_PREFIX, DOC_TITLE, "Version %s" % VERSION, "Content.txt", "files.txt", purpose)

    
    
