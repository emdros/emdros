/*
 * emdf_output.cpp
 *
 * Class to output something either in XML or in console-format.
 *
 * Ulrik Petersen
 * Created: 10/21-2001
 * Last update: 5/28-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, license version 2.  
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307 USA
 *
 *
 *   Special exception
 *   =================
 * 
 *   In addition, as a special exception, Ulrik Petersen, the
 *   copyright holder of Emdros, gives permission to link Emdros, in
 *   whole or in part, with the libraries which are normally
 *   distributed with:
 *   
 *   a) Sun's Java platform,
 *   b) Python, 
 *   c) Jython,
 *   d) Ruby, and/or 
 *   e) Perl 
 *   f) PostgreSQL
 *   g) OpenSSL
 *
 *   (or with modified versions of these), and to distribute linked
 *   combinations including both Emdros, in whole or in part, and one
 *   or more of the libraries normally distributed with (a)-(g) above.
 *
 *   Please note: This gives you special rights concerning the
 *   libraries which normally accompany the above pieces of software.
 *   It gives you no special rights concerning software that you write
 *   yourself.  You must obey the GNU General Public License in all
 *   respects for all of the code used other than the libraries
 *   normally distributed with (a)-(g) above.
 *
 *   If you modify this file, you may extend this exception to your
 *   version of the file, but you are not obligated to do so. If you
 *   do not wish to do so, delete this exception statement from your
 *   version.
 *
 *
 *   Other licensing forms
 *   =====================
 *
 *   If you wish to negotiate commercial licensing, please contact
 *   Ulrik Petersen at ulrikp[at]users.sourceforge.net.
 *
 *   Licensing can also be negotiated if your organization is an
 *   educational, non-profit, charity, missionary or similar
 *   organization.
 *
 *
 *   Website
 *   =======
 *
 *   Emdros has a website here:
 *
 *   http://emdros.org
 *
 *
 *
 **************************************************************************/

/**@file emdf_output.cpp
 *@brief Implementation of EMdFOutput (EMdF layer)
 */


#include <emdf_output.h>
#include <string_func.h>
#include <debug.h>

/** Names of charsets.
 *\internal
 */
static const char* s_charset_names[] = {
	"UTF-8",       // kCSASCII
	"ISO-8859-1",  // kCSISO_8859_1
	"ISO-8859-8",  // kCSISO_8859_8
	"UTF-8",       // kCSUTF8
	"ISO-8859-2",  // kCSISO_8859_2
	"ISO-8859-3",  // kCSISO_8859_3
	"ISO-8859-4",  // kCSISO_8859_4
	"ISO-8859-5",  // kCSISO_8859_5
	"ISO-8859-6",  // kCSISO_8859_6
	"ISO-8859-7",  // kCSISO_8859_7
	"ISO-8859-9",  // kCSISO_8859_9
	"ISO-8859-10", // kCSISO_8859_10
	"ISO-8859-13", // kCSISO_8859_13
	"ISO-8859-14", // kCSISO_8859_14
	"ISO-8859-15", // kCSISO_8859_15
	"ISO-8859-16"  // kCSISO_8859_16

	// NOTE: I tried including UTF-16, but Solaris doesn't do 
	// std::ostream << std::wstring, so I dropped the idea.
};

/** Emit attribute from AttributePairList.
 *\internal
 *
 * This is an internal utility function for emitting an
 * AttributePairList.    This is for XML output.
 */
void EMdFOutput::emitAttributes(const AttributePairList& attributes)
{
	AttributePairListConstIterator ci = attributes.const_iterator();
	while (ci.hasNext()) {
		const AttributePair& p = ci.next();

		// Emit name
		out(' ');
		out(p.first.c_str());
		out("=\"");

		// output value
		outCharData(p.second);

		out('\"');
	}
}

////////////////////////////////////////////////////////
//
// EMdFOutput
//
///////////////////////////////////////////////////////

/** Constructor
 *
 * Use directly only if you are also using an EMdFDB-derived class
 * directly.  Otherwise, use EmdrosEnv to create and manage the
 * EMdFOutput object.
 *
 * @param charset The charset to assume.
 * @param pStream A pointer to the output stream, e.g., std::cout.
 * @param output_kind output kind (XML or Console)
 * @param indent_chars number of chars to indent XML nested levels.
 */
EMdFOutput::EMdFOutput(eCharsets charset, std::ostream *pStream, eOutputKind output_kind, int indent_chars) : m_charset(charset), 
													      m_output_kind(output_kind),
													      m_pStream(pStream), 
													      m_current_indent(0), 
													      m_indent_chars(indent_chars),
													      m_emit_comma(false)
{
	// This causes the C++ library (at least on Linux) not to interleave
	// C and C++ output, which causes a performance increase (at least
	// on Linux).  See B. Stroustrup, "The C++ Programming language",
	// 3rd edition, p. 651.
	std::ios::sync_with_stdio(false);
}

/** Destructor
 */
EMdFOutput::~EMdFOutput()
{
	// Don't destroy pStream!  It could be cout!

	// But flush stream so we don't have anything pending
	flush();
}

/** Increase indent.
 *\internal
 *
 * This is for internal use, and increases the XML indent.
 */
void EMdFOutput::increaseIndent()
{
	// Don't do it if it is compact XML or compact JSON
	if (m_output_kind == kOKXML || m_output_kind == kOKJSON) {
		++m_current_indent;
	}
}

/** Decrease indent.
 *\internal
 *
 * This is for internal use, and decreases the XML indent.
 */
void EMdFOutput::decreaseIndent()
{
	// Don't do it if it is compact XML or compact JSON
	if (m_output_kind == kOKXML || m_output_kind == kOKJSON) {
		m_current_indent--;
		ASSERT_THROW(m_current_indent >= 0,
			     "Current indent less than 0");
	}
}

// Convert '&' to '&amp;' 
//     and '<' to '&lt;'
// before out'ing. 
// Useful for outputting character data which may contain these
// characters.
/** Output as XML CDATA
 *\internal
 *
 * Output a string as XML CDATA.  Converts '&' to '&amp', and '<' to
 * '&lt;' before outputting.
 *
 * @param s The string to output.
 */
void EMdFOutput::outCharData(const std::string& s)
{
	/*
	// NOTE: This is much slower...
	std::string::const_iterator p(s.begin());
	std::string::const_iterator pend(s.end());
	std::string sout;
	while (p != pend) {
	if (*p == '&') {
	sout += "&amp;";
	} else if (*p == '<') {
	sout += "&lt;";
	} else {
	sout += *p;
	}
	++p;
	}
	*/
	if (s.find_first_of("&<\"") != std::string::npos) {
		std::string::size_type length = s.length();
		for (std::string::size_type i = 0; i < length; ++i) {
			char p = s[i];
			if (p == '&') {
				out("&amp;");
			} else if (p == '\"') {
				out("&quot;");
			} else if (p == '<') {
				out("&lt;");
			} else if (p == '>') {
				out("&gt;");
			} else {
				out(p);
			}
		}
	} else {
		out(s.c_str());
	}
}

/** Emit a newline and ident
 *
 * Emits a newline, then outputs as many spaces as are necessary for the
 * indent.
 */
void EMdFOutput::newline()
{
	// Output newline
	*m_pStream << '\n';

	// NOTE: The following also flushes the buffer (on Linux, at least)
	// therefore, this is slower than writing a '\n' as above.
	// *m_pStream << std::endl;

	// Output spaces for indent
	if (m_output_kind == kOKXML || m_output_kind == kOKJSON) {
		int spaces = m_current_indent * m_indent_chars;
		for (int i = 0; i < spaces; ++i) {
			*m_pStream << ' ';
		}
	}
}

/** Print XML declaration.
 *\internal
 *
 * Prints the "<?xml version='1.0' encoding=... >" shebang.
 */
void EMdFOutput::printXMLDecl()
{
	out("<?xml "); 
	out("version='1.0' encoding='");
	out(s_charset_names[m_charset]);
	out("' standalone='yes' ?>");
}

/** Print XML start of DTD
 * \internal
 *
 * Prints the XML start-of-DTD ("<!DOCTYP \em root_element ["),
 * and emits \p root_element as the root element.
 *
 * @param root_element The root element to use for the DTD.
 */
void EMdFOutput::printDTDstart(const char* root_element)
{
	out("<!DOCTYPE ");
	out(root_element);
	out(" [\n");
}

/** Emits the XML DTD end
 * \internal
 *
 * Emits "]>" and emits a newline.
 */
void EMdFOutput::printDTDend()
{
	out("]>\n");
}


/** Emit XML start tag
 * \internal
 *
 * @param name The name of the tag
 * @param newline_before Set this to true if you want
 * EMdFOutput::newline() to be called before the tag is
 * emitted. Otherwise, set it to false.
 */
void EMdFOutput::startTag(const char* name, bool newline_before)
{
	if (m_output_kind == kOKXML) {
		if (newline_before) {
			newline();
		}
		increaseIndent();
	}

	// NOTE: It is faster to do '<' + name than to do "<" + name,
	// since the latter needs a length(const char*) call for the "<".
	//out('<' + name + '>');
	out('<');
	out(name);
	out('>');
}

// Must have pairs of (attribute name, attribute value) 
/** Start XML tag with attributes
 * \internal
 *
 * @param name The name of the tag
 * @param attributes The attributes to emit.
 * @param newline_before Set this to true if you want
 * EMdFOutput::newline() to be called before the tag is
 * emitted. Otherwise, set it to false.
 */
void EMdFOutput::startTag(const char* name, const AttributePairList& attributes, bool newline_before)
{
	if (m_output_kind == kOKXML) {
		if (newline_before)
			newline();
		increaseIndent();
	}
	out('<');
	out(name);
	emitAttributes(attributes);
	out('>');
}

// for tags of type <tag/>

/** Emit XML start tag for single tag
 * \internal
 *
 * This is used for XML tags of the form &lt;tag/&gt;.
 *
 * @param name The name of the tag
 * @param newline_before Set this to true if you want
 * EMdFOutput::newline() to be called before the tag is
 * emitted. Otherwise, set it to false.
 */
void EMdFOutput::startSingleTag(const char* name, bool newline_before) 
{
	if (m_output_kind == kOKXML) {
		if (newline_before)
			newline();
	}
	out('<');
	out(name);
	out("/>");
}

// for tags of type <tag/>
/** Emit XML start tag for single tag
 * \internal
 *
 * This is used for XML tags of the form &lt;tag/&gt;, and also tages an
 * attribute list.
 *
 * @param name The name of the tag
 * @param attributes The attributes to emit.
 * @param newline_before Set this to true if you want
 * EMdFOutput::newline() to be called before the tag is
 * emitted. Otherwise, set it to false.
 */
void EMdFOutput::startSingleTag(const char* name, const AttributePairList& attributes, bool newline_before) 
{
	if (m_output_kind == kOKXML) {
		if (newline_before)
			newline();
	}
	out('<');
	out(name);
	emitAttributes(attributes);
	out("/>");
}

/** End XML tag
 *\internal
 *
 * Decreases indent, and emits a newline if \p newline_before is true,
 * then emits a &lt;/tag&gt; end-tag with name \p name.
 *
 * @param name Name of the tag to end.
 * @param newline_before Set this to true if you want
 * EMdFOutput::newline() to be called before the tag is
 * emitted. Otherwise, set it to false.
 */
void EMdFOutput::endTag(const char* name, bool newline_before)
{
	if (m_output_kind == kOKXML) {
		decreaseIndent();
		if (newline_before)
			newline();
	}

	out("</");
	out(name);
	out('>');
}

/** Emit space into JSON output
 * \internal
 *
 */
void EMdFOutput::space()
{
    if (m_output_kind == kOKJSON)
        out(' ');
}

/** Emit a comma into the JSON output, if required
 * \internal
 */
void EMdFOutput::optComma()
{
    if (m_emit_comma) {
        out(',');
        if (m_output_kind == kOKJSON)
            newline();
    }
}

/** Emit JSON struct start
 * \internal
 *
 */
void EMdFOutput::jsonStartStruct()
{
    optComma();

    out('{');

    if (m_output_kind == kOKJSON) {
        increaseIndent();
        newline();
    }

    m_emit_comma = false;
}

/** Emit JSON array termination
 * \internal
 *
 */
void EMdFOutput::jsonStartArray()
{
    optComma();

    out('[');

    if (m_output_kind == kOKJSON) {
        increaseIndent();
        newline();
    }

    m_emit_comma = false;
}



/** Emit JSON label
 * \internal
 *
 * @param name The name of the tag. Set to 0 to emit an anonymous structure
 */
void EMdFOutput::jsonLabel(const char* name)
{
    optComma();

    out('"');
    out(name);
    out("\":");
    space();

    m_emit_comma = false;
}


/** Emit JSON struct termination
 * \internal
 *
 */
void EMdFOutput::jsonEndStruct()
{
    if (m_output_kind == kOKJSON) {
        decreaseIndent();
        newline();
    }
    out('}');
    
    m_emit_comma = true;
}

/** Emit JSON array termination
 * \internal
 *
 */
void EMdFOutput::jsonEndArray()
{
    if (m_output_kind == kOKJSON) {
        decreaseIndent();
        newline();
    }
    out(']');

    m_emit_comma = true;
}

void EMdFOutput::jsonOutValue(const std::string& s)
{
    out('"');

    if (s.find_first_of("/\"\\\n") != std::string::npos) {
        std::string::size_type length = s.length();
        for (std::string::size_type i = 0; i < length; ++i) {
            char p = s[i];
            if (p == '/') {
                out("\\/");
            } else if (p == '\"') {
                out("\\\"");
            } else if (p == '\\') {
                out("\\\\");
            } else if (p == '\n') {
                out("\\n");
            } else {
                out(p);
            }
        }
    } else {
        out(s);
    }

    out('"');
    m_emit_comma = true;
}

void EMdFOutput::jsonOutValue(const char* s)
{
    jsonOutValue(std::string(s));
}


void EMdFOutput::jsonOutValue(bool b)
{
    out(b ? "true" : "false");
    m_emit_comma = true;
}

void EMdFOutput::jsonOutValue()
{
    out("null");
    m_emit_comma = true;
}

void EMdFOutput::jsonOutValue(long i)
{
    out(long2string(i));
    m_emit_comma = true;
}

void EMdFOutput::jsonOutValueOrNull(long i)
{
    if (i == NIL)
        jsonOutValue();
    else
        jsonOutValue(i);
}


