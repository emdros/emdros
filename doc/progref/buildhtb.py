# -*- coding: utf-8 -*-
import os
import re
import sys
from config import *

LOOKING_FOR_START_OF_PAGE = 1
PARSING_PAGE = 2

def addFileIfExists(fout, infilename):
    if os.access(infilename, os.F_OK):
        fin = open(infilename, "r")
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
        self.mylist = line.strip().split("\t")

    def getID(self):
        print self.mylist[0]
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
            print "UP201: id =", id

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
        for line in f.readlines():
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

    def writeHTMLPages(self, hierarchy, extension, bWithNavigation, bWithAnalyticsTracking):
        ids = hierarchy.getIDs()
        ids.sort()
        for id in ids:
            self.writeHTMLFile(id, hierarchy, extension, bWithNavigation, bWithAnalyticsTracking)

    def writeHTMLFile(self, id, hierarchy, extension, bWithNavigation, bWithAnalyticsTracking):
        filename = str(id) + str(extension)
        print filename
        f = open(filename, "w")
        if bWithNavigation:
            css_header = "    <link href=\"cms.css\" rel=\"stylesheet\" type=\"text/css\">"
        else:
            css_header = ""
        self.writeHTMLHeader(f, id, hierarchy, css_header)
        self.writeHTMLPage(f, id, hierarchy, extension, bWithNavigation)
        self.writeHTMLFooter(f, id, hierarchy, extension, bWithAnalyticsTracking)
        f.close()

    def writeHTMLHeader(self, f, id, hierarchy, css_header):
        print >>f, """<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" 
        "http://www.w3.org/TR/REC-html40/strict.dtd">"""
        print >>f, "<html>"
        print >>f, "  <head>"
        print >>f, "     <title>" + hierarchy.getEntry(id).getTitle() + "</title>"
        print >>f, css_header
        print >>f, "  </head>"
        print >>f, "<body>"

    def writeHTMLPage(self, f, id, hierarchy, extension, bWithNavigation):
        if bWithNavigation:
            print >>f, "<TABLE cellspacing=\"0\" cellpadding=\"0\" class=\"page\" width=\"100%\">\n"
            print >>f, "<TR>\n"
            print >>f, "<TD CLASS=\"menu\">\n"
            print >>f, "<DIV CLASS=\"hjem\"><A CLASS=\"menu\" HREF=\"http://emdros.org/\">Home</A></DIV>"
            print >>f, "<BR><BR><DIV CLASS=\"PDF\"><A CLASS=\"menu\" HREF=\"%s.pdf\">PDF Version</A></DIV>" % PDF_PREFIX
            print >>f, "<br><br><DIV CLASS=\"indhold\">Contents:</DIV>"
            self.writeNavigationLeft(f, 1000, id, hierarchy, extension)
            print >>f, "<br>"
            print >>f, "<DIV CLASS=\"search\">Search:</DIV><br>"
            print >>f, "<FORM ACTION=\"search.php\" METHOD=\"GET\">"
            print >>f, "<INPUT TYPE=\"HIDDEN\" NAME=\"static_links\" VALUE=\"true\">"
            print >>f, "<INPUT TYPE=\"TEXT\" NAME=\"terms\" SIZE=\"23\" ALT=\"Write search terms\">"
            print >>f, "<INPUT TYPE=\"SUBMIT\" VALUE=\"Search\">"
            print >>f, "</FORM>"
            print >>f, ""
            addFileIfExists(f, "sidebar.inc")
            print >>f, "</TD><TD class=\"main\">"
            print >>f, ""
        print >>f, "<H1>%s</H1>" % hierarchy.getEntry(id).getTitle()
        pagestring = self.pages[id]
        pagestring = self.expandTags(pagestring, id, hierarchy, extension)
        print >>f, pagestring
        if bWithNavigation:
            print >>f, "</TD>"
            print >>f, "</TR>"
            print >>f, "</TABLE>"

    def writeNavigationLeft(self, f, main_id, focus_id, hierarchy, extension):
        arr_subsections = hierarchy.getImmediateChildren(main_id)

        if len(arr_subsections) != 0:
            bIsNotTOC = main_id != 1000

            if bIsNotTOC:
                print >>f, "<DIV class=\"menu\">"

            for child_id in arr_subsections:
                entry = hierarchy.getEntry(child_id)
                child_title = entry.getTitle()

                if entry.getType() == "Part":
                    bChildIsPart = True
                else:
                    bChildIsPart = False

                if bChildIsPart:
                    print >>f, "<br>"
                    print >>f, "<DIV class=\"part\">"
                else:
                    print >>f, "<DIV class=\"menu-item\">"
                    print >>f, "<IMG SRC=\"dot.gif\">&nbsp;"

                if child_id == focus_id:
                    print >>f, "<em>%s</em>" % child_title
                else:
                    child_link = "<A class=\"menu\" HREF=\"%d%s\">%s</A>" % (child_id, extension, child_title)
                    print >>f, child_link

                print >>f, "</DIV>"

                bFocusDocIsBelow = hierarchy.idIsBelowOther(child_id, focus_id)

                if bFocusDocIsBelow or focus_id == child_id:
                    self.writeNavigationLeft(f, child_id, focus_id, hierarchy, extension)
                    
            if bIsNotTOC:
                print >>f, "</DIV>"


    def writeHTMLFooter(self, f, id, hierarchy, extension, bWithAnalyticsTracking):
        print >>f, "<hr>"
        print >>f, "<strong>Previous:</strong>" + hierarchy.getLinkFromId(hierarchy.getPrev(id), extension) + "<br>"
        print >>f, "<strong>Up:</strong>" + hierarchy.getLinkFromId(hierarchy.getUp(id), extension) + "<br>"
        print >>f, "<strong>Next:</strong>" + hierarchy.getLinkFromId(hierarchy.getNext(id), extension) + "<br>"
        if bWithAnalyticsTracking:
            print >>f, """
<!-- Start of StatCounter Code -->
<script type="text/javascript" language="javascript">
var sc_project=185724; 
var sc_invisible=1; 
var sc_partition=0; 
var sc_security=""; 
var sc_remove_link=1; 
</script>

<script type="text/javascript" language="javascript" src="http://www.statcounter.com/counter/counter.js"></script><noscript><img  src="http://c1.statcounter.com/counter.php?sc_project=185724&amp;java=0&amp;security=&amp;invisible=1" alt="free web page hit counter" border="0"> </noscript>
<!-- End of StatCounter Code -->
<!-- Google Analytics Code -->
<script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-7616455-2']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>"""
        print >>f, "</body>"
        print >>f, "</html>"

    def writeOneBigHTMLPage(self, bookname, title, hierarchy):
        filename = "%s.htm" % bookname
        f = open(filename, "w")
        print >>f, """<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" 
        "http://www.w3.org/TR/REC-html40/strict.dtd">"""
        print >>f, "<html>"
        print >>f, "  <head>"
        print >>f, "     <title>" + title + "</title>"
        print >>f, "  </head>"
        print >>f, "<body>"

        for id in hierarchy.docline:
            if id != 1000:
                print >>f, "<H%d><A NAME=\"%s\">%s</A></H%d>" % (hierarchy.levels[id],str(id),hierarchy.getEntry(id).getTitle(),hierarchy.levels[id])
                pagestring = self.pages[id]
                pagestring = self.expandTags(pagestring, id, hierarchy, None)
                print >>f, pagestring
                #print >>f, "<br><br><hr><br><br>"
        f.close()

    def getSection(self, level):
        if level == 1:
            return "\section"
        elif level == 2:
            return "\subsection"
        elif level == 3:
            return "\subsubsection"
        else:
            return "\paragraph"

    def writeOneBigLaTeXPage(self, bookname, title, hierarchy):
        filename = "%s.tex" % bookname
        f = open(filename, "w")
        print >> f, "\\documentclass[a4paper]{article}"
        print >> f, "\\usepackage{graphicx}"
        print >> f, "\\usepackage{ucs}"
        print >> f, "\\usepackage{html}"
        print >> f, "\\title{%s}" % title
        print >> f, "\\author{Ulrik Petersen}"
	try:
	    print >> f, "\\date{}" % DOC_DATE
        except:
            pass # If DOC_DATE isn't available, print distribution date by default
        print >> f, "\\begin{document}"
        print >> f, """\\newenvironment{precode}
        {\\begin{list}{}{
        \\setlength{\\rightmargin}{\\leftmargin}
        \\setlength{\\listparindent}{0pt}% needed for AMS classes
        \\raggedright
        \\setlength{\\itemsep}{0pt}
        \\setlength{\\parsep}{0pt}
        \\normalfont\\ttfamily}%
        \\item[]}
        {\\end{list}}"""

        print >> f, "\\maketitle\n"

        print >> f, "\\tableofcontents\n"

        for id in hierarchy.docline:
            if id != 1000:
                print >>f, "%s{%s}" % (self.getSection(hierarchy.levels[id]),hierarchy.getEntry(id).getTitle().replace("&quot;","\""))
                pagestring = self.pages[id]
                pagestring = self.expandTagsLaTeX(pagestring, id, hierarchy)
                print >>f, pagestring
        print >> f, "\n\\end{document}"
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


        # IMG
        p_img_re = re.compile(r'<[Pp]><[Ii][Mm][Gg][^>]+[Ss][Rr][Cc]="([^"]+)"[^>]*></[Pp]>')
        tmpstr = p_img_re.sub(r'\n\\bigskip\n\\includegraphics[scale=0.5]{\1}\n\\bigskip\n', tmpstr)
        img_re = re.compile(r'<[Ii][Mm][Gg][^>]+[Ss][Rr][Cc]="([^"]+)"[^>]*>')
        tmpstr = img_re.sub(r'\\includegraphics[scale=0.5]{\1}', tmpstr)
        
        # H2
        h2_re = re.compile(r'<[Hh]2>')
        h2_re_end = re.compile(r'</[Hh]2>')
        level = hierarchy.levels[id] + 1
        tmpstr = h2_re.sub("%s{" % self.getSection(level), tmpstr)
        tmpstr = h2_re_end.sub("}\n", tmpstr)

        # H3
        h3_re = re.compile(r'<[Hh]3>')
        h3_re_end = re.compile(r'</[Hh]3>')
        level = hierarchy.levels[id] + 2
        tmpstr = h3_re.sub("%s{" % self.getSection(level), tmpstr)
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
        tmpstr = big_re.sub("{\\Large ", tmpstr)
        tmpstr = big_re_end.sub("}", tmpstr)

        # CENTER
        center_re = re.compile(r'<[Cc][Ee][Nn][Tt][Ee][Rr]>')
        center_re_end = re.compile(r'</[Cc][Ee][Nn][Tt][Ee][Rr]>')
        tmpstr = center_re.sub(r'\n\\begin{center}\n', tmpstr)
        tmpstr = center_re_end.sub(r'\n\\end{center}\n', tmpstr)



        tmpstr = tmpstr.replace("{PREHASH}", "#").replace("{PRELT}","<").replace("{PREGT}",">").replace("{PREOPENBRACE}","{").replace("{PRECLOSEBRACE}","}")
        return tmpstr

    def writeHHC(self, bookname, hierarchy, extension):
        f = open(bookname + ".hhc", "w")
        top_level_ids = hierarchy.getImmediateChildren(1000)
        self.writeHHCList(f, top_level_ids, hierarchy, extension)
        f.close()

    def writeHHCList(self, f, idlist, hierarchy, extension):
        if len(idlist) > 0:
            print >>f, "<ul>"
            for id in idlist:
                print >>f, "  <li><object type=\"text/sitemap\">"
                print >>f, "          <param name=\"Name\" value=\"%s\">" % hierarchy.getEntry(id).getTitle()
                print >>f, "          <param name=\"ID\" value=%d>" % id
                print >>f, "          <param name=\"Local\" value=\"%d%s\">" % (id, str(extension))
                print >>f, "      </object>"
                id_children = hierarchy.getImmediateChildren(id)
                self.writeHHCList(f, id_children, hierarchy, extension)
            print >>f, "</ul>"

    def writeHHK(self, bookname, hierarchy, extension):
        self.produceIndex(hierarchy)
        f = open(bookname + ".hhk", "w")
        topics = self.myindex.keys()
        topics.sort()
        if len(topics) > 0:
            print >>f, "<ul>"
            for topic in topics:
                idlist = self.myindex[topic]
                idlist.sort()
                for id in idlist:
                    print >>f, "  <li><object type=\"text/sitemap\">"
                    print >>f, "      <param name=\"Name\" value=\"%s\">" % topic
                    print >>f, "      <param name=\"Local\" value=\"%d%s\">" % (id, extension)
                    print >>f, "      </object>"
                    
            print >>f, "</ul>"
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
                

    
def doit(bookname, booktitle, contents_filename, files_filename, purpose):
    # Read "files.txt"
    f = open(files_filename)
    hierarchy = Hierarchy()
    hierarchy.readFromFile(f)
    f.close()

    # Read "Content.txt"
    f = open(contents_filename)
    pages = Pages()
    pages.readFromFile(f)
    f.close()

    if purpose == "htb":
        pages.writeHTMLPages(hierarchy, ".htm", False, False)
        pages.writeHHC(bookname, hierarchy, ".htm")
        pages.writeHHK(bookname, hierarchy, ".htm")
    elif purpose == "single":
        pages.writeOneBigHTMLPage(bookname, booktitle, hierarchy)
    elif purpose == "latex":
        pages.writeOneBigLaTeXPage(bookname, booktitle, hierarchy)
    elif purpose == "web":
        pages.writeHTMLPages(hierarchy, ".html", True, True)
    else:
        raise Exception("Error: Unknown purpose '%s'" % purpose)

# Do htb by default
purpose = "htb"

if len(sys.argv) >= 2:
    arg1 = sys.argv[1]
    if arg1[0:2] == "--":
        purpose = arg1[2:]
    else:
	print "Unknown switch: %s" % arg1
	sys.exit(1)


        

doit(PDF_PREFIX, DOC_TITLE, "Content.txt", "files.txt", purpose)

    
    
