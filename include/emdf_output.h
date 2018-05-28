/*
 * emdf_output.h
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
 *   Copyright (C) 2001-2018 Ulrik Sandborg-Petersen
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

/**@file emdf_output.h
 *@brief Header file for EMdFOutput (EMdF layer)
 */



#ifndef EMDF_OUTPUT__H__
#define EMDF_OUTPUT__H__

#ifndef SWIG
#include "emdros-config.h"
#include "emdros-lconfig.h"
#include "exception_emdros.h"

#include <ostream>
#include <string>
#include <list>

#include "llist.h"
#include "emdf_enums.h"
#include "emdf_exception.h"
#endif

#ifndef SWIG
/** Attribute name-value pair
 * @internal
 *
 * This is a std::pair<std::string, std::string>, where
 * the first member of the pair is an attribute name, and the
 * second is its value.
 *
 * This is used for XML.
 */
typedef std::pair<std::string, std::string> AttributePair;

/** Attribute name-value pair list.
 * @internal
 *
 * This is a LVlist<AttributePair>, and is used for XML.
 *
 * @see AttributePair, and, e.g., EMdFOutput::startTag().
 */
typedef LVlist<AttributePair> AttributePairList;

/** Attribute name-value pair list const iterator.
 * @internal
 *
 * This is a LVlistConstIterator<AttributePair>, and is used for XML.
 *
 * @see AttributePair, and, e.g., EMdFOutput::startTag().
 */
typedef LVlistConstIterator<AttributePair> AttributePairListConstIterator;
#endif

/** Output abstraction for the EMdF layer.
 *@ingroup EMdF
 *
 * You should probably not construct an object of this class directly,
 * but rather let EmdrosEnv create and manage the EMdFOutput object.
 * See EMdFOutput::EMdFOutput() for more information.
 *
 */
class EMdFOutput {
 protected:
	eCharsets m_charset;
	eOutputKind m_output_kind;
	std::ostream *m_pStream;
	int m_current_indent;
	int m_indent_chars;
    bool m_emit_comma;
 public:
	EMdFOutput(eCharsets charset, std::ostream *pStream, eOutputKind output_kind, int indent_chars = 3);
 private:
	// Leave unimplemented, so as to prevent compiler from
	// creating crashing ones.
	EMdFOutput(const EMdFOutput& other);
	EMdFOutput& operator=(const EMdFOutput& other);
 public:
	~EMdFOutput();
	// Getting
	/** Getting the eOutputKind.
	 *
	 * @return The eOutputKind of the EMdFOutput object (XML or Console).
	 */
	eOutputKind getOutputKind(void) const { return m_output_kind; }
	void newline();
	bool isXML(void) const { return m_output_kind == kOKXML || m_output_kind == kOKCompactXML; };
	bool isJSON(void) const { return m_output_kind == kOKJSON || m_output_kind == kOKCompactJSON; };
#ifndef SWIG
	// Output
	void increaseIndent();
	void decreaseIndent();
	/** Output a string
	 *\internal
	 *
	 * Outputs a string raw on the stream (doesn't do any processing of
	 * the string).
	 *
	 * @param s The string to output.
	 */
	void out(std::string s) { *m_pStream << s; };
#ifndef SWIG
	/** Output a character (char)
	 *\internal
	 *
	 * Outputs a char raw on the stream.
	 *
	 * @param c The char to output.
	 */
	void out(char c) { *m_pStream << c; };
	/** Output a const char*
	 *\internal
	 *
	 * Outputs a const char* raw on the stream (doesn't do any
	 * processing of the string).
	 *
	 * @param s The string to output.
	 */
	void out(const char* s) { *m_pStream << s; };
#endif
	void outCharData(const std::string& s);
    void jsonOutValue(const std::string& s);
    void jsonOutValue(const char* s);
    void jsonOutValue(bool b);
    void jsonOutValue(long i);
    void jsonOutValue();
    void jsonOutValueOrNull(long i);
	/** flush stream.
	 * @internal
	 */
	void flush() { *m_pStream << std::flush; };
	// XML members
	void printXMLDecl();
	void printDTDstart(const char *root_element);
	void printDTDend();
	void startTag(const char* name, bool newline_before = false);
	void startTag(const char* name, const AttributePairList& attributes, bool newline_before = false); // Must have pairs of (attribute name, attribute value) 
	void startSingleTag(const char* name, bool newline_before = false); // for tags of type <tag/>
	void startSingleTag(const char* name, const AttributePairList& attributes, bool newline_before = false); // for tags of type <tag/>
	void endTag(const char* name, bool newline_before = false);
 protected:
	void emitAttributes(const AttributePairList& attributes);

    // JSON members
  public:
    void jsonStartStruct();
	void jsonStartArray();
	void jsonLabel(const char* name);
	void jsonEndStruct();
	void jsonEndArray();
    void optComma();
  protected:
    void space();


#endif // !defined SWIG
};

#endif /* EMDF_OUTPUT__H__ */
