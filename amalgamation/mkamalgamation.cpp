/*
 * mkamalgamation.cpp
 *
 * A program to create the Emdros amalgamation.
 *
 * Created: 4/12-2016 
 * Last update: 2/14-2019
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Something went wrong.
 * 3 : An exception occurred
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2016-2018  Ulrik Sandborg-Petersen
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


#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <stack>

#include <cstring>


///////////////////////////////////////////////////////
//
// AmalgamationQDException
//
///////////////////////////////////////////////////////


class AmalgamationQDException {
protected:
	std::string m_message;
public:
	AmalgamationQDException(const std::string& msg) { m_message = msg; };
	~AmalgamationQDException() {};
	std::string what() { return m_message; };
};


///////////////////////////////////////////////////////
//
// Utility functions
//
///////////////////////////////////////////////////////

#define AMALGAMATION_UNUSED(V) ((void) V)

void amalgamation_replace_chars(std::string &s, const char in, const char out)
{
	std::string::size_type i = 0;
	while ((i = s.find(in, i)) != std::string::npos)
		s[i] = out;
}


void amalgamation_str_toupper(const std::string& in, std::string& result)
{
	result.clear();
	result.reserve(in.length());
	std::string::const_iterator ci(in.begin());
	std::string::const_iterator cend(in.end());
	while (ci != cend) {
		char c = *ci++;
		if (c >= 'a' && c <= 'z') {
			result.push_back(c - 32);
		} else {
			result.push_back(c);
		}
	}
}
	

std::string amalgamation_path_join(const std::string& path, const std::string& filename)
{
	std::string result = path;
#ifdef WIN32
	result += "\\";
#else
	result += "/";
#endif
	result += filename;

	return result;
}

std::string amalgamation_get_filename_from_path(const std::string& path)
{
	std::string::size_type index = path.find_last_of("/");
	if (index == std::string::npos) {
		return path;
	} else {
		// Get the part just after the last '/'
		return path.substr(index+1);
	}
}

void amalgamation_long2utf8(long c, std::string& output)
{
	output.clear();
	if (c<0x00080) {
		output += (c&0xFF);
	} else if (c<0x00800) {
		output += (0xC0 + ((c>>6)&0x1F));
		output += (0x80 + (c & 0x3F));
	} else if (c<0x10000) {
		output += (0xE0 + ((c>>12)&0x0F));
		output += (0x80 + ((c>>6) & 0x3F));
		output += (0x80 + (c & 0x3F));
	} else {
		output += (0xF0 + ((c>>18) & 0x07));
		output += (0x80 + ((c>>12) & 0x3F));
		output += (0x80 + ((c>>6) & 0x3F));
		output += (0x80 + (c & 0x3F));
	}
}

bool amalgamation_string_ends_with(const std::string& input, const std::string& end)
{
	// Does not work with UTF-8 in end...
	std::string::size_type input_length = input.length();
	std::string::size_type end_length = end.length();
	if (end_length > input_length) {
		return false;
	} else {
		std::string end_string = input.substr(input_length - end_length , end_length);
		return end_string == end;
	}
}

std::string amalgamation_joinListEmptyBetween(const std::list<std::string>& l)
{
	std::ostringstream str;
	std::list<std::string>::const_iterator ci = l.begin();	
	std::list<std::string>::const_iterator cend = l.end();
	while (ci != cend) {
		str << *ci++;
	}
	return str.str();
}


bool amalgamation_is_hex(const std::string& str)
{
	const std::string hex_digits = "0123456789abcdefABCDEF";
	return !str.empty() && str.find_first_not_of(hex_digits) == std::string::npos;
}

long amalgamation_hex2long(const std::string& str)
{
	long result = 0;
	std::string::size_type strlength = str.length();
	bool bHasMetFirstNonZero = false;
	for (std::string::size_type i = 0;
	     i < strlength;
	     ++i) {
		char c = str[i];
		// Skip leading zeros
		if (c != '0') {
			bHasMetFirstNonZero = true;
		}
		if (bHasMetFirstNonZero) {
			long tmp = 0;
			if (c >= 'a'
			    && c <= 'f') {
				tmp = c - ('a' - 10);
			} else if (c >= 'A'
				   && c <= 'F') {
				tmp = c - ('A' - 10);
			} else if (c >= '0'
				   && c <= '9') {
				tmp = c - '0';
			}
			result = (result << 4) | tmp;
		}
	}
	return result;
}

inline bool amalgamation_is_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r'; // NOTE: '\v' is missing, but I don't think it is ever used!
}


long amalgamation_sz2long(const char *szBuffer)
{
	long result = 0;
	const char *p = szBuffer;

	while (*p && amalgamation_is_whitespace(*p)) {
		++p;
	};
	bool bIsNegative = false;	
	if (*p == '-') {
		bIsNegative = true;
		++p;
	} else if (*p == '+') {
		++p;
	}

	while (*p) {
		int c = *p++ - '0';
		if (c < 0
		    || c > 9) {
			break;
		} else {
			result *= 10;
			result += c;
		}
	}

	return (bIsNegative) ? -result : result;
}


long amalgamation_string2long(const std::string& str)
{
	return (long) amalgamation_sz2long(str.c_str());

}


std::string amalgamation_match_include(const std::string& uline)
{
	typedef enum {
		START_OF_LINE,
		EXPECT_INCLUDE,
		EXPECT_OPEN_FILENAME,
		READ_FILENAME
	} eMatchState;

	std::string FAILURE;
    
	eMatchState state = START_OF_LINE;

	std::string::size_type index = 0;
	std::string::size_type length = uline.length();

	std::string::size_type header_filename_start_index = -1;
	char include_end_char = ' ';
    
	while (index < length) {
		char c = uline[index];

		if (state == START_OF_LINE) {
			if (c == ' ' || c == '\t') {
				state = START_OF_LINE;
			} else if (c == '#') {
				state = EXPECT_INCLUDE;
			} else {
				// Line does not match.
				return FAILURE;
			}
		} else if (state == EXPECT_INCLUDE) {
			if (c == ' ' || c == '\t') {
				state = EXPECT_INCLUDE;
			} else if (c == 'i') {
				if ((length - index) > std::string("nclude <.>").length()) {
					if (uline.substr(index, 7) == "include") {
						index += 7;
						state = EXPECT_OPEN_FILENAME;
					} else {
						// Line does not match
						return FAILURE;
					}
				} else {
					// Not enough characters. Line does not match
					return FAILURE;
				}
			} else {
				// Expecting include, not found.
				return FAILURE;
			}
		} else if (state == EXPECT_OPEN_FILENAME) {
			if (c == ' ' || c == '\t') {
				state = EXPECT_OPEN_FILENAME;
			} else if (c == '<') {
				header_filename_start_index = index+1;
				include_end_char = '>';
				state = READ_FILENAME;
			} else if (c == '"') {
				header_filename_start_index = index+1;
				include_end_char = '"';
				state = READ_FILENAME;
			} else {
				return FAILURE;
			}
		} else if (state == READ_FILENAME) {
			if (c == include_end_char) {
				std::string header_filename = uline.substr(header_filename_start_index, index - header_filename_start_index);
				return header_filename;
			}  else {
				; // Nothing to do, except to advance index below
			}
		} else {
			throw AmalgamationQDException("Unknown state");
		}
		
		++index;
	}

	return FAILURE;
}

std::string amalgamation_make_define_name(const std::string& target_header_filename)
{
	std::string result;
	amalgamation_str_toupper(target_header_filename, result);
	amalgamation_replace_chars(result, '.', '_');
	return result;
}
			  


///////////////////////////////////////////////////////
//
// TargetFile
//
///////////////////////////////////////////////////////

class TargetFile {
protected:
	std::string m_language;
	std::string m_name;
	std::map<std::string, std::string> m_code_prefixes; // code_filename -> "code_prefix"
	std::list<std::string> m_source_files;
	std::list<std::string> m_header_files;
	std::set<std::string> m_external_header_files;
	std::set<std::string> m_unrenamed_files;

	std::string m_target_prefix;
	std::string m_target_suffix;
	
public:
	TargetFile(const std::string& language, const std::string& name);

	~TargetFile();


	std::string getLanguage() const { return m_language; };
	std::string getName() const { return m_name; };

	std::string getCodePrefix(const std::string& code_filename) const;

	void addCodePrefix(const std::string& code_filename,
			   const std::string& code_prefix) {
		m_code_prefixes[code_filename] = code_prefix;
	}

	void addSourceFile(const std::string& source_filename) {
		m_source_files.push_back(source_filename);
	}

	void addHeaderFile(const std::string& header_filename) {
		m_header_files.push_back(header_filename);
	}
	
	void addExternalFile(const std::string& external_filename) {
		m_external_header_files.insert(external_filename);
	}
	
	void addUnrenamedFile(const std::string& unrenamed_filename) {
		m_unrenamed_files.insert(unrenamed_filename);
	}

	void setTargetPrefix(const std::string& target_prefix) {
		m_target_prefix = target_prefix;
	}

	void setTargetSuffix(const std::string& target_suffix) {
		m_target_suffix = target_suffix;
	}

	std::string getTargetPrefix() const {
		return m_target_prefix;
	}

	std::string getTargetSuffix() const {
		return m_target_suffix;
	}

	void make_headers_available(std::map<std::string, std::string>& result);
	
	void copy_file(std::ostream& fout,
		       const std::string& language,
		       std::map<std::string, std::string>& headers_available,
		       const std::string& source_filename);

	void write_h_amalgamation(std::ostream& fout, std::map<std::string, std::string>& headers_available);

	void process(const std::string& preamble, std::map<std::string, std::string>& headers_available);
};


TargetFile::TargetFile(const std::string& language, const std::string& name)
{
	m_language = language;
	m_name = name;
}

TargetFile::~TargetFile()
{
}


std::string TargetFile::getCodePrefix(const std::string& code_filename) const
{
	std::map<std::string, std::string>::const_iterator ci =
		m_code_prefixes.find(code_filename);
	if (ci != m_code_prefixes.end()) {
		return ci->second;
	} else {
		return "";
	}
}


	

void TargetFile::make_headers_available(std::map<std::string, std::string>& result)
{
	std::list<std::string>::const_iterator ci = m_header_files.begin();
	std::list<std::string>::const_iterator ciend = m_header_files.end();
	while (ci != ciend) {
		std::string header_path = *ci++;
		if (m_unrenamed_files.find(header_path) != m_unrenamed_files.end()) {
			result.insert(std::make_pair(header_path, header_path));
		} else {
			std::string header_filename = amalgamation_get_filename_from_path(header_path);
			if (result.find(header_filename) == result.end()) {
				result.insert(std::make_pair(header_filename, header_path));
			}
		}
	}
}


void TargetFile::copy_file(std::ostream& fout,
			   const std::string& language,
			   std::map<std::string, std::string>& headers_available,
			   const std::string& source_filename)
{
	fout << "/**************** A copy of " 
<< source_filename << " *****************/\n";
	if (language == "C") {
		fout << "#ifdef __cplusplus\nextern \"C\" {\n#endif\n";
	}
	
	fout << "\n\n#line 1 \"" << source_filename << "\"\n";

        int line_count = 0;
	std::ifstream fin;
	std::string source_pathname = amalgamation_path_join("..", source_filename);
	fin.open(source_pathname.c_str());
	if (!fin) {
		throw AmalgamationQDException("ERROR: Could not open " + source_pathname + " for reading.\n");
	}
	while (fin) {
		std::string line;
		getline(fin, line);
		++line_count;

		std::string include_filename = amalgamation_match_include(line);

		if (include_filename.empty()) {
			fout << line << '\n';
		} else {
			if (m_external_header_files.find(include_filename) != m_external_header_files.end()) {
				fout << line << '\n';
			} else if (headers_available.find(include_filename) == headers_available.end()) {
				// include_filename not in headers_available.
				fout << line << '\n';
			} else if (headers_available.find(include_filename)->second != "") {
				fout << "/**************** leaving " << source_filename << " temporarily *****************/\n";
				
				std::string header_path = headers_available[include_filename];

				// Make sure we don't emit this header again.
				headers_available[include_filename] = "";

				// Copy the header
				this->copy_file(fout, m_language, headers_available, header_path);

				fout << "/**************** continuing " << source_filename << " where we left off *****************/\n";
				fout << "\n\n#line " << line_count << " \"" << source_filename << "\"\n";
			} else {
				fout << "/**************** already included " << source_filename << " -- not including again *****************/\n";
				fout << "\n\n#line " << line_count << " \"" << source_filename << "\"\n";
			}
		}
	}

	fin.close();

	if (language == "C") {
		fout << "#ifdef __cplusplus\n} /* End of 'extern \"C\"'. */\n#endif\n";
	}
}

void TargetFile::write_h_amalgamation(std::ostream& fout, std::map<std::string, std::string>& headers_available)
{
	fout << m_target_prefix;

	std::list<std::string>::const_iterator ci = m_header_files.begin();
	std::list<std::string>::const_iterator ciend = m_header_files.end();
	while (ci != ciend) {
		std::string source_filename = *ci++;
		bool bDoIt = false;
		if (m_external_header_files.find(source_filename) != m_external_header_files.end()) {
			bDoIt = false;
		} else {
			std::string header_filename;
			if (m_unrenamed_files.find(source_filename) != m_unrenamed_files.end()) {
				header_filename = source_filename;
			} else {
				header_filename = amalgamation_get_filename_from_path(source_filename);
			}

			if (headers_available.find(header_filename) != headers_available.end()) {
				bDoIt = headers_available.find(header_filename)->second != "";
				headers_available[header_filename] = "";
			} else {
				bDoIt = false;
			}
		}

		if (bDoIt) {
			std::string code_prefix = this->getCodePrefix(source_filename);
			fout << code_prefix;
			
			this->copy_file(fout, m_language, headers_available, source_filename);
		} else {
			; // Nothing to do for this header file.
		}
	}
	
        fout << m_target_suffix;
}


			
void TargetFile::process(const std::string& preamble, std::map<std::string, std::string>& headers_available)
{
	std::ofstream fout;
	fout.open(m_name.c_str());
	if (!fout) {
		throw AmalgamationQDException("ERROR: Could not open " + m_name + " for writing.\n");
	}

        fout << preamble;

	std::string prefix = getTargetPrefix();
        fout << prefix;

	std::list<std::string>::const_iterator ci = m_source_files.begin();
	std::list<std::string>::const_iterator ciend = m_source_files.end();
	while (ci != ciend) {
		std::string source_filename = *ci++;

		std::string code_prefix = getCodePrefix(source_filename);
		fout << code_prefix;
                
		this->copy_file(fout, m_language, headers_available, source_filename);
	}


	std::string suffix = getTargetSuffix();
        fout << suffix;

        fout.close();
        
}

///////////////////////////////////////////////////////
//
// AmalgamationCreator
//
///////////////////////////////////////////////////////

class AmalgamationCreator {
protected:
	std::list<TargetFile*> m_target_files; // # List of TargetFile objects
	std::map<std::string, std::string> m_target_header_files; // language-string -> target_header_filename-string
	std::set<std::string> m_ignored_header_files; // set of ignored_header_filename-strings
	std::map<std::string, std::string> m_preambles; // language-string -> preamble-string
public:

	AmalgamationCreator();
	~AmalgamationCreator();
	
	void addTargetHeaderFile(const std::string& language, const std::string& name) {
		m_target_header_files.insert(std::make_pair(language, name));
	}

	void addIgnoredHeaderFile(const std::string& name) {
		m_ignored_header_files.insert(name);
	}

	void addTargetFile(TargetFile *target_file_obj) {
		m_target_files.push_back(target_file_obj);
	}

	std::string getPreamble(const std::string& language) {
		if (m_preambles.find(language) != m_preambles.end()) {
			return m_preambles.find(language)->second;
		} else {
			return "";
		}
	}

	void addPreamble(const std::string& language, const std::string& preamble) {
		m_preambles[language] = preamble;
	}
        
	void process() {
		make_source_amalgamations();

		make_h_amalgamations();
	}

	void make_headers_available(std::map<std::string, std::string>& result);

	void make_source_amalgamations();

	void make_h_amalgamations();
};


AmalgamationCreator::AmalgamationCreator()
{
}


AmalgamationCreator::~AmalgamationCreator()
{
	std::list<TargetFile*>::iterator it = m_target_files.begin();
	std::list<TargetFile*>::iterator itend = m_target_files.end();
	while (it != itend) {
		TargetFile *pTargetFileObj = *it;

		delete pTargetFileObj;

		++it;
	}
	m_target_files.clear();
}


void AmalgamationCreator::make_source_amalgamations()
{
	std::list<TargetFile*>::iterator it = m_target_files.begin();
	std::list<TargetFile*>::iterator itend = m_target_files.end();
	while (it != itend) {
		TargetFile *pTargetFileObj = *it;

		std::string language = pTargetFileObj->getLanguage();

		std::string preamble = getPreamble(language);

		std::map<std::string, std::string> headers_available;
            
		make_headers_available(headers_available);

		pTargetFileObj->process(preamble, headers_available);

		++it;
	}
}

void AmalgamationCreator::make_h_amalgamations()
{
	std::map<std::string, std::string>::iterator it =
		m_target_header_files.begin();
	std::map<std::string, std::string>::iterator itend =
		m_target_header_files.end();
	while (it != itend) {
		std::string language = it->first;

		std::map<std::string, std::string> headers_available;
		make_headers_available(headers_available);

		std::string target_header_filename = it->second;
		    
		std::ofstream fout;
		fout.open(target_header_filename.c_str());
		if (!fout) {
			std::string message =
				"ERROR: Could not open "
				+ target_header_filename
				+ " for writing. Aborting.\n";
			std::cerr << message << std::endl;
			throw AmalgamationQDException(message);
		}

		std::string define_name = amalgamation_make_define_name(target_header_filename);

		fout << "#ifndef " << define_name << "\n"
		     << "#define " << define_name << "\n\n";

		std::string preamble = getPreamble(language);
		fout << preamble;

		std::list<TargetFile*>::iterator it2 = m_target_files.begin();
		std::list<TargetFile*>::iterator it2end = m_target_files.end();
		while (it2 != it2end) {
			TargetFile *pTargetFileObj = *it2;

			std::string target_file_language = pTargetFileObj->getLanguage();

			if (target_file_language == language) {
				pTargetFileObj->write_h_amalgamation(fout, headers_available);
			}
			
			++it2;
		}

		fout << "\n#endif /* !defined(" << define_name << ") */\n";

		fout.close();
		
		++it;
	}

}

void AmalgamationCreator::make_headers_available(std::map<std::string, std::string>& result)
{
	
	// result : header_filename-string -> header_path-string
			
	// Gather header filenames from each TargetFile object
	std::list<TargetFile*>::iterator it = m_target_files.begin();
	std::list<TargetFile*>::iterator itend = m_target_files.end();
	while (it != itend) {
		TargetFile *pTargetFileObj = *it;

		pTargetFileObj->make_headers_available(result);

		++it;
	}

        // Ignore these, and elide their inclusion.
	std::set<std::string>::const_iterator ci = m_ignored_header_files.begin();
	std::set<std::string>::const_iterator ciend = m_ignored_header_files.end();

	while (ci != ciend) {
		std::string header_filename = *ci;
		result[header_filename] = "";
		++ci;
	}
}


///////////////////////////////////////////////////////
//
// AttributeMap
//
///////////////////////////////////////////////////////



typedef std::map<std::string, std::string> AttributeMap;


std::string getAttribute(const AttributeMap& attrs, const std::string& key)
{
	AttributeMap::const_iterator ci = attrs.find(key);
	if (ci == attrs.end()) {
		throw AmalgamationQDException("Could not find attribute with name '" + key + "'");
	} else {
		return ci->second;
	}
}
	

///////////////////////////////////////////////////////
//
// AmalgamationQDDocHandler
//
///////////////////////////////////////////////////////


class AmalgamationQDDocHandler {
 public:
	AmalgamationQDDocHandler() {};
	virtual ~AmalgamationQDDocHandler() {};
	virtual void startElement(const std::string& tag, const AttributeMap& attrs) { AMALGAMATION_UNUSED(tag); AMALGAMATION_UNUSED(attrs); };
	virtual void endElement(const std::string& tag) { AMALGAMATION_UNUSED(tag); };
	virtual void startDocument(void) {};
	virtual void endDocument(void) {};
	virtual void text(const std::string& str) { AMALGAMATION_UNUSED(str); };
	virtual void comment(const std::string& str) { AMALGAMATION_UNUSED(str); };
};

///////////////////////////////////////////////////////
//
// AmalgamationQDParser
//
///////////////////////////////////////////////////////



class AmalgamationQDParser {
 protected:
	const static std::string NOTAG;
	int m_line;
	int m_column;
	int m_tag_depth;
	AttributeMap m_attributes;
	std::istream *m_pInStream;
	AmalgamationQDDocHandler *m_pDH;
	std::string m_tag_name;
	std::string m_lvalue;
	std::string m_rvalue;
	bool m_end_of_line;
	int m_quote_char;
	std::string m_entity;
	std::string m_text;
	

	typedef enum {
		TEXT,
		ENTITY,
		OPEN_TAG,
		CLOSE_TAG,
		START_TAG,
		ATTRIBUTE_LVALUE,
		ATTRIBUTE_EQUAL,
		ATTRIBUTE_RVALUE,
		QUOTE,
		IN_TAG,
		SINGLE_TAG,
		COMMENT,
		DONE,
		DOCTYPE,
		IN_XMLDECLARATION,
		BEFORE_XMLDECLARATION,
		OPEN_XMLDECLARATION,
		CDATA
	} eQDStates;


	typedef std::stack<eQDStates> StateStack;
	StateStack m_stack;
 public:
	AmalgamationQDParser();
	~AmalgamationQDParser();
	void parse(AmalgamationQDDocHandler *pDH, std::istream *pStream);

 protected:
	void cleanUp() 
	{
		if (bot) {
			delete[] bot;
			bot = 0;
		}
	}
	void eraseAttributes()
	{
		m_attributes.clear();
	}

	bool myisspace(int nc)
	{
		return nc == (int) ' ' 
			|| nc == (int) '\n' 
			// || nc == (int) '\r'  // '\r' is converted to '\n' by getNextChar().
			|| nc == (int) '\t' 
			|| nc == (int) '\x0b' // Vertical space
			|| nc == (int) '\f';
	}

	void pushState(eQDStates state) {
		m_stack.push(state);
	}
		

	eQDStates popState(void)
	{
		if (!m_stack.empty()) {
			eQDStates result = m_stack.top();
			m_stack.pop();
			return result;
		} else {
			except("AmalgamationQDParser::popState: Attempted to pop off of an empty StateStack.");
			return TEXT; // Just to fool the compiler into giving now warning!
		}
	}

	void handle_entity(eQDStates mode);

	std::string state2string(eQDStates e);

	void except(const std::string& s);

	int getNextChar(void);
	int handleEOL(int c);

	void fillBuffer(void);
 protected:
	void readIntoBuffer();
	bool hasMoreInput() { return eof == 0 || cur != eof; };

 private:
	char *bot, *cur, *lim, *top, *eof;
};


const std::string AmalgamationQDParser::NOTAG = "<>";

std::string AmalgamationQDParser::state2string(eQDStates e)
{
	if (e == TEXT) {
		return "TEXT";
	} else if (e == ENTITY) {
		return "ENTITY";
	} else if (e == OPEN_TAG) {
		return "OPEN_TAG";
	} else if (e == CLOSE_TAG) {
		return "CLOSE_TAG";
	} else if (e == START_TAG) {
		return "START_TAG";
	} else if (e == ATTRIBUTE_LVALUE) {
		return "ATTRIBUTE_LVALUE";
	} else if (e == ATTRIBUTE_EQUAL) {
		return "ATTRIBUTE_EQUAL";
	} else if (e == ATTRIBUTE_RVALUE) {
		return "ATTRIBUTE_RVALUE";
	} else if (e == QUOTE) {
		return "QUOTE";
	} else if (e == IN_TAG) {
		return "IN_TAG";
	} else if (e == SINGLE_TAG) {
		return "SINGLE_TAG";
	} else if (e == COMMENT) {
		return "COMMENT";
	} else if (e == DONE) {
		return "DONE";
	} else if (e == DOCTYPE) {
		return "DOCTYPE";
	} else if (e == IN_XMLDECLARATION) {
		return "IN_XMLDECLARATION";
	} else if (e == BEFORE_XMLDECLARATION) {
		return "BEFORE_XMLDECLARATION";
	} else if (e == OPEN_XMLDECLARATION) {
		return "OPEN_XMLDECLARATION";
	} else if (e == CDATA) {
		return "CDATA";
	} else {
		std::ostringstream str;
		str << "Unknown state: " << (int) e;
		throw AmalgamationQDException(str.str());
	}
}
	
#define APPEND_CHAR(s, c) s += (char) c
#define SET_CHAR(s, c) s = (char) c

void AmalgamationQDParser::parse(AmalgamationQDDocHandler *pDH, std::istream *pStream) 
{
	bot = cur = lim = top = eof = 0;
	m_pInStream = pStream;
	m_pDH = pDH;
	m_tag_depth = 0;
	eQDStates state = BEFORE_XMLDECLARATION;
	m_tag_name = NOTAG;
	m_line = 1;
	m_column = 0;
	m_pDH->startDocument();
	m_end_of_line = false;

	int c = getNextChar();

	while (hasMoreInput()) {
		//std::cerr << "UP200: state = " << state2string(state) << ", c = '" << c << "'" << std::endl;

		switch (state) {
		case TEXT:
			// We are collecting text between tags.
			switch (c) {
			case '&':
				pushState(TEXT);
				m_entity.clear();
				state = ENTITY;
				break;
			case '<':
				if (!m_text.empty()) {
					m_pDH->text(m_text);
					m_text.clear();
				}
				pushState(TEXT);
				state = START_TAG;
				m_tag_name.clear();
				break;
			default:
				APPEND_CHAR(m_text, c);
				break;
			}
			break;
		case QUOTE:
			switch (c) {
			case '\n':
			case '\t':
				// See XML Spec section 3.3.3
				// on normalization.
				// '\r' isn't metioned here because
				// it is converted to '\n' by getNextChar().
				APPEND_CHAR(m_rvalue, ' ');
				break;
			case '&':
				pushState(QUOTE);
				state = ENTITY;
				m_entity.clear();
				break;
			default:
				if (c == m_quote_char) {
					m_attributes.insert(AttributeMap::value_type(m_lvalue, m_rvalue));
					state = popState();
				} else {
					APPEND_CHAR(m_rvalue, c);
				}
				break;
			}
			break;
		case ATTRIBUTE_LVALUE:
			switch (c) {
			case '=':
				state = ATTRIBUTE_RVALUE;
				break;
			case ' ':
			case '\n':
				// case '\r':// Gets converted to '\n' by getNextChar()
			case '\x0b':
			case '\f':
				state = ATTRIBUTE_EQUAL;
				break;
			default:
				APPEND_CHAR(m_lvalue, c);
				break;
			}
			break;
		case OPEN_TAG:
			switch (c) {
			case '/':
				state = SINGLE_TAG;
				break;
			case '>':
				++m_tag_depth;
				m_pDH->startElement(m_tag_name, m_attributes);
				eraseAttributes();
				m_tag_name.clear();
				state = popState();
				break;
			case ' ':
			case '\n':
				// case '\r': // Gets converted to '\n' by getNextChar()
			case '\x0b':
			case '\f':
				state = IN_TAG;
				break;
			default:
				APPEND_CHAR(m_tag_name, c);

				switch (c) {
				case '-':
					if (m_tag_name == "!--") {
						//pushState(TEXT);
						state = COMMENT;
					}
					break;
				case '[':
					if (m_tag_name == "![CDATA[") {
						m_text.clear();
						//pushState(TEXT);
						state = CDATA;
					}
					break;
				case 'E':
					if (m_tag_name == "!DOCTYPE") {
						//pushState(TEXT);
						state = DOCTYPE;
						m_text.clear();
						m_tag_name.clear();
					}
					break;
				} 
				break;
			}
			break;
		case IN_TAG:
			switch (c) {
			case '/':
				state = SINGLE_TAG;
				break;
			case '>':
				++m_tag_depth;
				m_pDH->startElement(m_tag_name, m_attributes);
				eraseAttributes();
				m_tag_name.clear();
				state = popState();
				break;
			case ' ':
			case '\n':
				// case '\r': // Not possible: Converted to '\n' by getNextChar()
			case '\x0b':
			case '\f':
				; // Eat up
				break;
			default:
				pushState(IN_TAG);
				state = ATTRIBUTE_LVALUE;
				SET_CHAR(m_lvalue, c);
				m_rvalue.clear();
				break;
			}
			break;
		case START_TAG:
			{
				// The last char was '<'. Now
				// find out whether we are doing <starttag>,
				// </endtag>, <!-- comment -->,
				// <!DOCTYPE, or <![CDATA
				eQDStates topState = popState();
				switch (c) {
				case '/':
					pushState(topState);
					state = CLOSE_TAG;
					break;
				case '?':
					pushState(topState);
					state = OPEN_XMLDECLARATION;
					eraseAttributes();
					break;
				default:
					pushState(topState);
					state = OPEN_TAG;
					SET_CHAR(m_tag_name, c);
					eraseAttributes();
					break;
				}
			}
			break;
		case ATTRIBUTE_RVALUE:
			switch (c) {
			case '\'':
			case '\"':
				m_quote_char = c;
				state = QUOTE;
				break;
			case ' ':
			case '\n':
				// case '\r': // Gets converted to '\n' by getNextChar()
			case '\x0b':
			case '\f':
				; // Eat up
				break;
			default:
				except("Error while processing attribute: After =, there should only be whitespace or '\\'' or '\\\"'.");
				break;
			}
			break;
		case SINGLE_TAG:
			// We are have seen 
			// something like
			// <barefoot attr1="value" ... /
			// Now we need the final >
			if (c != '>') {
				except("While processing single tag: Expected > for tag: <" + m_tag_name + "/>");
			} else {
				m_pDH->startElement(m_tag_name, m_attributes);
				m_pDH->endElement(m_tag_name);
				if (m_tag_depth == 0) {
					state = DONE;
				} else {
					state = popState();
					m_tag_name.clear();
					eraseAttributes();
				}
			}
			break;
		case CLOSE_TAG:
			// We have seen '</'
			if (c == '>') {
				m_pDH->endElement(m_tag_name);
				state = popState();
				--m_tag_depth;
				if (m_tag_depth == 0) {
					state = DONE;
				}
				m_tag_name.clear();
			} else {
				APPEND_CHAR(m_tag_name, c);
			}
			
			break;
		case IN_XMLDECLARATION:
			// We are inside <?xml .... ?>
			if (c == '?') {
				c = getNextChar();
				if (c != '>') {
					except("XML declaration must end with '?>'");
				} else {
					// IGNORE encoding.
					
					state = TEXT;
				}
			} else if (myisspace(c)) {
				; // Eat up
			} else {
				pushState(IN_XMLDECLARATION);
				state = ATTRIBUTE_LVALUE;
				SET_CHAR(m_lvalue, c);
				m_rvalue.clear();
			}
			break;
		case BEFORE_XMLDECLARATION:
			if (c == '<') {
				c = getNextChar();
				if (c == '?') {
					c = getNextChar();
					if (c != 'x') {
						except("Document must start with '<?x'. It does not.");
					}
					c = getNextChar();
					if (c != 'm') {
						except("Document must start with '<?xm'. It does not.");
					}
					c = getNextChar();
					if (c != 'l') {
						except("Document must start with '<?xml'. It does not.");
					}
					c = getNextChar();
					if (!myisspace((int) c)) {
						except("Document must start with '<?xml' + <whitespace>. It does not.");
					}
					state = IN_XMLDECLARATION;
				} else {
					except("Document must start with <?; it does not.");
				}
			}
			break;
		case ATTRIBUTE_EQUAL:
			switch (c) {
			case '=':
				state = ATTRIBUTE_RVALUE;
				break;
			case ' ':
			case '\n':
				// case '\r': // Gets converted to '\n' by getNextChar()
			case '\x0b':
			case '\f':
				; // Eat up
				break;
			default:
				except("Error while processing attribute: After lvalue, there should only be whitespace or '='.");
				break;
			}
			break;
		case ENTITY:
			if (c == ';') {
				state = popState();
				handle_entity(state);
			} else {
				APPEND_CHAR(m_entity, c);
			}
			break;
		case COMMENT:
			if (c == '>') {
				APPEND_CHAR(m_text, c);
				if (amalgamation_string_ends_with(m_text, "-->")) {
					m_pDH->comment(m_text);

					m_text.clear();
					state = popState();
				} else {
					; // Nothing to do
				}
			} else {
				APPEND_CHAR(m_text, c);
			}
			break;
		case CDATA:
			if (c == '>') {
				if (amalgamation_string_ends_with(m_text, "]]")) {
					m_pDH->text(m_text.substr(0, m_text.length() - 2));
					state = popState();
					m_text.clear();
				} else {
					APPEND_CHAR(m_text, c);
				}
			} else {
				APPEND_CHAR(m_text, c);
			}
			break;
		case DOCTYPE:
			APPEND_CHAR(m_tag_name, c);

			if (c == '<') {
				m_tag_name.clear();
			} else if (c == '-' && m_tag_name == "!--") {
				pushState(DOCTYPE);
				state = COMMENT;
			} else if (c == ']') {
				SET_CHAR(m_tag_name, c);
			} else if (c == '>') {
				if (m_tag_name == "]>") {
					// Is TEXT
					state = popState();
				}
			} else {
				; // Eat up
			}
			break;
		case DONE:
			eraseAttributes();
			m_pDH->endDocument();
			cleanUp();
			return;
			break;
		default:
			except(std::string("Error: Unknown state: ") + state2string(state));
		}

		// Read next char
		
		c = getNextChar();

		// Alternate implementation:
	}
	
	if (state == DONE) {
		m_pDH->endDocument();
	} else {
		except("missing end tag");
	}
	eraseAttributes();
	cleanUp();
}
	
void AmalgamationQDParser::except(const std::string& s)
{
	std::ostringstream ostr;
	ostr << s << " near line " << m_line << ", column " << m_column << '!' << std::endl;
	throw AmalgamationQDException(ostr.str());
}

void AmalgamationQDParser::handle_entity(eQDStates state)
{
	if (!m_entity.empty() && m_entity[0] == '#') {
		long l = 0;
		if (m_entity.length() > 1
		    && (m_entity[1] == 'x' 
			|| m_entity[1] == 'X')) {
			std::string hexstring = m_entity.substr(2, std::string::npos);
			if (amalgamation_is_hex(hexstring)) {
				l = amalgamation_hex2long(hexstring);
			} else {
				except("Invalid hex-string in entity &" + m_entity + ";.");
			}
		} else {
			std::string decimalstring = m_entity.substr(1, std::string::npos);
			l = amalgamation_string2long(decimalstring);
		}

		std::string utf8str;
		amalgamation_long2utf8(l, utf8str);
		if (state == QUOTE) {
			m_rvalue += utf8str;
		} else {
			m_text += utf8str;
		}
	} else {
		char entity_char = '\0';
		if (m_entity == "lt") {
			entity_char = '<';
		} else if (m_entity == "gt") {
			entity_char = '>';
		} else if (m_entity == "amp") {
			entity_char = '&';
		} else if (m_entity == "quot") {
			entity_char = '"';
		} else if (m_entity == "apos") {
			entity_char = '\'';
		} else {
			except("Unknown entity: &"+m_entity+";");
		}
		
		if (state == QUOTE) {
			APPEND_CHAR(m_rvalue, entity_char);
		} else {
			APPEND_CHAR(m_text, entity_char);
		}
	}
}

int AmalgamationQDParser::handleEOL(int c)
{
	switch (c) {
	case '\r':
		m_end_of_line = true;
		m_column = 0;
		++m_line;
		c = '\n';
		break;
	case '\n':
		if (m_end_of_line) {
			// We have already read \r, and this is \n that follows.
			// \r returned \n, since we need to translate
			// all of \r\n, \r, and \n to \n 
			// (see Section 2.11 of the XML spec)
			// So now we return the next character, since this
			// method is called 'getNextChar()'.
			if (hasMoreInput()) {
				if (!(lim - cur)) {
					fillBuffer();
				}
				c = *cur++;
			}
			++m_column;
			m_end_of_line = false;
		} else {
			m_column = 0;
			++m_line;
		}
		break;
		/*
	default:
		m_end_of_line = false; // We might be doing a file with \r as the end-of-line marker.
		++m_column;
		break;
		*/
	}
	return c;
}

int AmalgamationQDParser::getNextChar()
{
	if (!(lim - cur)) fillBuffer();
	int c = (int) (*cur++);
	if (c == '\n' || c == '\r') {
		return handleEOL(c);
	} else {
		m_end_of_line = false;
		++m_column;
		return c;
	}
}

#define READ_SIZE   (32*1024)
#define BUF_SIZE    (2*READ_SIZE)


void AmalgamationQDParser::readIntoBuffer()
{
	if ((top - lim) < READ_SIZE) {
		throw AmalgamationQDException("Error: (top - lim) < READ_SIZE");
	}
	
	m_pInStream->read(lim, READ_SIZE);
	unsigned long long cnt = m_pInStream->gcount();
	if ((cnt != READ_SIZE) || m_pInStream->eof()) {
		eof = &lim[cnt]; *eof++ = '\0';
	}
	lim += cnt;
}

void AmalgamationQDParser::fillBuffer()
{
	if (lim == 0) {
		bot = new char[BUF_SIZE];
		cur = bot;
		lim = bot;
		top = bot + BUF_SIZE;
		readIntoBuffer();
	} else if (!eof) {
                size_t cnt = lim - cur;
		if (cnt > 0) {
                        memcpy(bot, cur, cnt);
			lim = bot + cnt;
			cur = bot;
		} else {
			lim = bot;
			cur = bot;
		}
		
		readIntoBuffer();
	}
}

#undef READ_SIZE
#undef BUF_SIZE

AmalgamationQDParser::AmalgamationQDParser()
{
	bot = cur = lim = top = eof = 0;
}

AmalgamationQDParser::~AmalgamationQDParser()
{
	cleanUp();
}




///////////////////////////////////////////////////////
//
// More utility functions
//
///////////////////////////////////////////////////////





void print_usage(std::ostream& ostr)
{
	ostr << "Usage:" << std::endl;
	ostr << "   mkamalgamation";
#ifdef WIN32
	ostr << ".exe";
#endif
	ostr << " <amalgamation_xml_filename>" << std::endl;
}


///////////////////////////////////////////////////////
//
// AmalgamationXMLDocHandler
//
///////////////////////////////////////////////////////


typedef std::list<std::string> ListOfString;

class AmalgamationXMLDocHandler : public AmalgamationQDDocHandler {
protected:
	ListOfString m_charstack;
	ListOfString m_elemstack;
	std::set<std::string> m_ignored_elements;
	std::set<std::string> m_handled_elements;
	std::set<std::string> m_char_handled_end_tags;
	AmalgamationCreator *m_ac_obj;
	TargetFile *m_cur_target_file;
	std::string m_cur_code_prefix_name;
	std::string m_cur_language;
public:
	AmalgamationXMLDocHandler();
	virtual ~AmalgamationXMLDocHandler();
	virtual void startElement(const std::string& tag, const AttributeMap& attrs);
	virtual void endElement(const std::string& tag);
	virtual void startDocument(void);
	virtual void endDocument(void);
	virtual void text(const std::string& str);
	virtual void comment(const std::string& str);

	virtual void process();

	
protected:
	void eraseCharStack(void);
	void handleChars(bool bIsEndTag);
	std::string getCurElement() const;
	void handleElementStart(const std::string& tag, const AttributeMap& attrs);
	void handleElementEnd(const std::string& tag);
};


AmalgamationXMLDocHandler::AmalgamationXMLDocHandler()
{
	m_ignored_elements.insert("amalgamation");
	m_ignored_elements.insert("target_preambles");
	m_ignored_elements.insert("target_header_files");
	m_ignored_elements.insert("ignored_header_files");
	m_ignored_elements.insert("external_header_files");
	m_ignored_elements.insert("unrenamed_files");
	m_ignored_elements.insert("target_files");
	m_ignored_elements.insert("code_prefixes");
	m_ignored_elements.insert("header_files");
	m_ignored_elements.insert("source_files");
	m_ignored_elements.insert("target_prefix");
	m_ignored_elements.insert("target_suffix");

	m_handled_elements.insert("target_preamble");
	m_handled_elements.insert("target_header_file");
	m_handled_elements.insert("ignored_header_file");
	m_handled_elements.insert("external_header_file");
	m_handled_elements.insert("unrenamed_file");
	m_handled_elements.insert("target_file");
	m_handled_elements.insert("code_prefix");
	m_handled_elements.insert("header_file");
	m_handled_elements.insert("source_file");

	m_char_handled_end_tags.insert("target_preamble");
	m_char_handled_end_tags.insert("code_prefix");
	m_char_handled_end_tags.insert("target_prefix");
	m_char_handled_end_tags.insert("target_suffix");
		
	
	m_ac_obj = new AmalgamationCreator();
	m_cur_target_file = 0;
	m_cur_language = "";
}


AmalgamationXMLDocHandler::~AmalgamationXMLDocHandler()
{
	eraseCharStack();
	delete m_ac_obj;
}


void AmalgamationXMLDocHandler::startElement(const std::string& tag, const AttributeMap& attributes)
{
	m_elemstack.push_back(tag);
	
	handleChars(false);

	if (m_handled_elements.find(tag) != m_handled_elements.end()) {
		handleElementStart(tag, attributes);
	} else if (m_ignored_elements.find(tag) != m_ignored_elements.end()) {
		; // Nothing to do
	} else {
		std::string message = "Error: Unknown start-tag '<" + tag + ">'";
		throw AmalgamationQDException(message);
	}
}


void AmalgamationXMLDocHandler::endElement(const std::string& tag)
{
	handleChars(true);

	if (m_handled_elements.find(tag) != m_handled_elements.end()) {
		handleElementEnd(tag);
	} else if (m_ignored_elements.find(tag) != m_ignored_elements.end()) {
		; // Nothing to do
	} else {
		std::string message = "Error: Unknown end-tag '</" + tag + ">'";
		throw AmalgamationQDException(message);
	}

	m_elemstack.pop_back();
}

void AmalgamationXMLDocHandler::handleElementStart(const std::string& tag, const AttributeMap& attributes)
{
        if (tag == "target_preamble") {
		m_cur_language = getAttribute(attributes, "language");
	} else if (tag == "target_header_file") {
		std::string language = getAttribute(attributes, "language");
		std::string name = getAttribute(attributes, "name");
		m_ac_obj->addTargetHeaderFile(language, name);
	} else if (tag == "ignored_header_file") {
		std::string name = getAttribute(attributes, "name");
		m_ac_obj->addIgnoredHeaderFile(name);
	} else if (tag == "unrenamed_file") {
		std::string name = getAttribute(attributes, "name");
		m_cur_target_file->addUnrenamedFile(name);
	} else if (tag == "external_header_file") {
		std::string name = getAttribute(attributes, "name");
		m_cur_target_file->addExternalFile(name);
	} else if (tag == "target_file") {
		std::string language = getAttribute(attributes, "language");
		std::string name = getAttribute(attributes, "name");
		m_cur_target_file = new TargetFile(language, name);
	} else if (tag == "code_prefix") {
		std::string name = getAttribute(attributes, "name");
		m_cur_code_prefix_name = name;
	} else if (tag == "header_file") {
		std::string name = getAttribute(attributes, "name");
		m_cur_target_file->addHeaderFile(name);
	} else if (tag == "source_file") {
		std::string name = getAttribute(attributes, "name");
		m_cur_target_file->addSourceFile(name);
	} else {
		std::string message = "Unhandled element start: " + tag;
		throw AmalgamationQDException(message);
	}
}


void AmalgamationXMLDocHandler::handleElementEnd(const std::string& tag)
{
        if (tag == "target_preamble") {
		// All done at start, and in handleChars()
	} else if (tag == "target_header_file") {
		// All done at start
	} else if (tag == "ignored_header_file") {
            // All done at start
	} else if (tag == "external_header_file") {
            // All done at start
        } else if (tag == "unrenamed_file") {
            // All done at start
        } else if (tag == "target_file") {
		m_ac_obj->addTargetFile(m_cur_target_file);
		m_cur_target_file = 0;
	} else if (tag == "code_prefix") {
		m_cur_code_prefix_name = "";
	} else if (tag == "header_file") {
		// All done at start
	} else if (tag == "source_file") {
            // All done at start
	} else {
		std::string message = "Unhandled element end: " + tag;
		throw AmalgamationQDException(message);
	}
}


std::string AmalgamationXMLDocHandler::getCurElement() const
{
	if (m_elemstack.empty()) {
		return "";
	} else {
		return *m_elemstack.rbegin();
	}
}

void AmalgamationXMLDocHandler::handleChars(bool bIsEndTag)
{
	std::string chars_before;
	if (!m_charstack.empty()) {
		chars_before = amalgamation_joinListEmptyBetween(m_charstack);
	} else {
		// chars_before remains empty.
	}

	eraseCharStack();

	std::string cur_element = getCurElement();

	bool bDoIt = false;
	if (bIsEndTag
	    && m_char_handled_end_tags.find(cur_element) != m_char_handled_end_tags.end()) {
		bDoIt = true;
	} else {
		bDoIt = false;
	}

	if (!bDoIt) {
		return;
	}

	if (cur_element == "code_prefix") {
		m_cur_target_file->addCodePrefix(m_cur_code_prefix_name, chars_before);
	} else if (cur_element == "target_prefix") {
		m_cur_target_file->setTargetPrefix(chars_before);
	} else if (cur_element == "target_suffix") {
		m_cur_target_file->setTargetSuffix(chars_before);
	} else if (cur_element == "target_preamble") {
		m_ac_obj->addPreamble(m_cur_language, chars_before);
	} else {
		std::string message = "cur end-tag </" + cur_element + "> is not handled in handleChars().";
		throw AmalgamationQDException(message);
	}
}


void AmalgamationXMLDocHandler::startDocument(void)
{
	// Nothing to do
}


void AmalgamationXMLDocHandler::endDocument(void)
{
	// Nothing to do
}



void AmalgamationXMLDocHandler::text(const std::string& str)
{
	m_charstack.push_back(str);
}

void AmalgamationXMLDocHandler::comment(const std::string& str)
{
	(void) str;
}

void AmalgamationXMLDocHandler::process()
{
	m_ac_obj->process();
}



void AmalgamationXMLDocHandler::eraseCharStack(void)
{
	m_charstack.clear();
}


///////////////////////////////////////////////////////
//
// Driver function
//
///////////////////////////////////////////////////////


bool doIt(const std::string& xml_filename)
{
	std::ifstream fin;
	fin.open(xml_filename.c_str());
	if (!fin) {
		std::cerr << "FAILURE: Could not open file " << xml_filename << " for reading." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		return false;
	}

	AmalgamationQDParser parser;
	AmalgamationXMLDocHandler handler;

	bool bResult = false;
	try {
		parser.parse(&handler, &fin);

		handler.process();

		bResult = true;
	} catch (AmalgamationQDException& e) {
		std::cerr << "EXCEPTION:\n" << e.what() << "\n" << std::endl;
		bResult = false;
	}

	fin.close();
	
	return bResult;
}


int main(int argc, char* argv[])
{
	// Set defaults
	std::string filename;

	if (argc == 2) {
		filename = argv[1];
	} else {
		print_usage(std::cerr);
		return 1;
	}

	int nResult = 3;
	try {
		bool bResult = doIt(filename);
		if (!bResult) {
			nResult = 2;
		} else {
			nResult = 0;
		}
	} catch (AmalgamationQDException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception occurred.  Program aborted." << std::endl;
	} 

	return nResult;
}


