/*
 * string_func.h
 *
 * String helper functions
 *
 * Ulrik Petersen
 * Created: 3/1-2001
 * Last update: 6/8-2018
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

/**@file string_func.h
 *@brief String utility functions (EMdF layer)
 */


#ifndef STRING_FUNC__H__
#define STRING_FUNC__H__

#include <string>
#include <cstdio>
#include <sstream>
#include <list>
#include <vector>

#include "emdf.h"
#include "emdf_enums.h"

class IntegerList; // Forward declaration

extern void str_toupper(const std::string& in, std::string& result);
extern void str_tolower(const std::string& in, std::string& result);
extern void str_tolower(std::string& in);
extern int strcmp_nocase(const std::string& str1, const std::string& str2);
extern int strcmp_case(const std::string& str1, const std::string& str2);
extern std::string bool2string(bool b);
extern bool string2bool(const std::string& str);
extern std::string bool_alpha2string(bool b);
extern bool string2bool_alpha(const std::string& str);
extern std::string int2string(int i);
extern std::string id_d2string(id_d_t i);
extern std::string id_d2number_string(id_d_t i);
extern id_d_t string2id_d(const std::string& str);
extern std::string double2string(double d);
extern std::string monad_m2string(monad_m m);
extern monad_m string2monad_m(const std::string& str);

inline bool is_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r'; // NOTE: '\v' is missing, but I don't think it is ever used!
}

inline emdros_int64 sz2emdros_int64(const char *szBuffer)
{
	emdros_int64 result = 0;
	const char *p = szBuffer;

	while (*p && is_whitespace(*p)) {
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

inline void emdros_int64ToSzNonReversing(emdros_int64 l, char *szBuffer, int nBufLength, char **szResult, unsigned int *pStringLength)
{
	// Build up string
	char *p = szBuffer + nBufLength;
	--p;

	// Terminate string
	*p-- = '\0';

	*pStringLength = 0;
	
	emdros_int64 v = l;
	do {
		emdros_int64 tmp = v;
		v /= 10;
		*p-- = "9876543210123456789" [9+(tmp - v*10)];

		++(*pStringLength);
	} while (v);
	
	// Add negative sign at the end if necessary	
	if (l < 0) {
		*p = '-';
		++(*pStringLength);
	} else {
		// Point it at the beginning
		++p;
	}
	
	*szResult = p;
}

inline void long2szNonReversing(long l, char *szBuffer, int nBufLength, char **szResult, unsigned int *pStringLength)
{
	// Build up string
	char *p = szBuffer + nBufLength;
	--p;

	// Terminate string
	*p-- = '\0';

	*pStringLength = 0;
	
	long v = l;
	do {
		long tmp = v;
		v /= 10;
		*p-- = "9876543210123456789" [9+(tmp - v*10)];

		++(*pStringLength);
	} while (v);
	
	// Add negative sign at the end if necessary	
	if (l < 0) {
		*p = '-';
		++(*pStringLength);
	} else {
		// Point it at the beginning
		++p;
	}
	
	*szResult = p;
}


extern std::string emdros_int64ToString(emdros_int64 l);
extern std::string emdros_int64ToString_format(emdros_int64 l, const std::string& format);
extern emdros_int64 string2emdros_int64(const std::string& str);

extern std::string long2string(long l);
extern std::string long2string_format(long l, const std::string& format);
extern long string2long(const std::string& str);
extern std::string encodeSTRINGstring(const std::string& str);
extern bool is_other_than_whitespace(const std::string& str);

inline bool is_self(const std::string& str)
{
	if (str.length() == 4) {
		char c1 = str[0];
		if (c1 == 's' || c1 == 'S') {
			char c2 = str[1];
			if (c2 == 'e' || c1 == 'E') {
				char c3 = str[2];
				if (c3 == 'l' || c1 == 'L') {
					char c4 = str[3];
					if (c4 == 'f' || c1 == 'F') {
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

extern bool has_double_quote(const std::string& str);
extern bool is_hex(const std::string& str);
extern bool is_identifier(const std::string& str);
extern std::string convert_to_identifier(const std::string& instr);

extern void replace_chars(std::string &s, const char in, const char out);

// Turns _underscores into spaces
extern void replace_underscores(std::string& s);

// Turns spaces into _underscores
extern void replace_spaces(std::string& s);

// Replaces substrings with other strings
extern std::string replace_substring(const std::string& instring, const std::string& substring_to_replace, const std::string& replacement_string);

extern std::string reverse_string(const std::string& in);

// Checks whether string is an integer.
extern bool string_is_number(const std::string& in);

extern bool has_double_quote(const std::string& str);

extern std::string convert_to_hex(const std::string& instr);

extern long hex2long(const std::string& instr);

extern std::string ulong2hex(unsigned long in);

extern std::string remove_char(std::string instr, char char_to_remove);

extern std::string remove_from_substring_and_out(const std::string& instr, const std::string& substr);

extern std::string remove_chars_in_string(const std::string& instr, const std::string& chars_to_remove);

extern std::string remove_punct(const std::string& instr);

extern bool string_ends_with(const std::string& input, const std::string& end);





/** Convert two characters representing a hex value to char.
 *
 * @param a The most significant nybble.
 *
 * @param b The least significant nybble.
 *
 * @return The parameters as a hex value.
 */
inline unsigned char hex2char(unsigned char a, unsigned char b)
{ 
	unsigned int result;
	switch (a) {
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		result = (a - 'W')*16;
		break;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		result = (a - '7')*16;
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		result = (a - '0')*16;
		break;
	default:
		result = 0;
	}

	switch (b) {
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		result += (b - 'W');
		break;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		result += (b - '7');
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		result += (b - '0');
		break;
	}
	return (unsigned char) result;
}

/** Convert a two-character string in HEX to char.
 * 
 * @param h The string to convert.
 *
 * @return The value of the hex-based string h.
 */
inline unsigned char hex2char(const char* h)
{
  return hex2char(h[0], h[1]);
}

/** Convert a char to a two-char hex value with lower-case letters.
 * 
 * @param c The char to convert.
 *
 * @return The string representation of the char c.
 */
inline std::string char2hex(unsigned char c)
{
	char szResult[3];
	szResult[0] = "0123456789abcdef"[(c >> 4) & 0x0F];
	szResult[1] = "0123456789abcdef"[c & 0x0F];
	szResult[2] = '\0';
	return szResult;
}

inline void char2hex(unsigned char c, char szResult[3])
{
	szResult[0] = "0123456789abcdef"[(c >> 4) & 0x0F];
	szResult[1] = "0123456789abcdef"[c & 0x0F];
	szResult[2] = '\0';
}

/** Convert a char to a two-char hex value with upper-case letters.
 * 
 * @param c The char to convert.
 *
 * @return The string representation of the char c.
 */
inline std::string char2upperhex(unsigned char c)
{
	char szResult[3];
	szResult[0] = "0123456789ABCDEF"[(c >> 4) & 0x0F];
	szResult[1] = "0123456789ABCDEF"[c & 0x0F];
	szResult[2] = '\0';
	return szResult;
}

/** Convert an octal string to a number (char).
 *
 * @param o The NUL-terminated string in octal to convert.
 *
 * @return The value of the octal-based string o.
 */
inline unsigned char octal2char(const char* o)
{
	unsigned int result = 0;
	const char *p = o;
	while (*p != '\0') {
		result <<= 3;
		char c = *p++;
		if (c >= '0' && c <= '7') {
			result += c - '0';
		}
	}
	return (unsigned char) result;
}

/** Convert a character to 3-character octal octal.
 *
 * @param c The character to convert.
 *
 * @return The character as an octal string.
 */
inline std::string char2octal(unsigned char c)
{
  char szResult[4];

  szResult[0] = "0123"[(c >> 6) & 0x03];
  szResult[1] = "01234567"[(c >> 3) & 0x07];
  szResult[2] = "01234567"[c & 0x07];
  szResult[3] = '\0';
  
  return szResult;
}

extern void print_indent(std::ostream *pOut, int indent);

extern bool string2charset(const std::string& input, eCharsets& charset);
extern bool charset2string(eCharsets charset, std::string& output);

extern bool string2backend_kind(const std::string& input, eBackendKind& backend_kind);
extern std::string backend_kind2string(eBackendKind backend_kind);

extern void long2utf8(long input, std::string& output);
extern unsigned int readOneUTF8Char(const std::string& instr, std::string::size_type instr_length, std::string::size_type& index);
extern void codepoint2slashu(std::string& result, unsigned int c); 
extern std::string escape_UTF8_string_with_slashu(const std::string& instr);


/** A function to join a list of std::string with the string between
 *  inbetween each list member.
 */
extern std::string joinList(const std::string& between, const std::list<std::string>& l, unsigned int nBigStringSize = 131072);

/** A function to join a vector of std::string with the string between
 *  inbetween each list member.
 */
extern std::string joinVector(const std::string& between, const std::vector<std::string>& v, unsigned int nBigStringSize = 131072);

/** A function to join a list of id_d_t with the string between
 *  inbetween each list member.
 */
extern std::string joinList(const std::string& between, const std::list<id_d_t>& l);


/** A template to join a list of T into one long string
 */
/*
template<class T> std::string joinListEmptyBetween(const std::list<T>& l)
{
	std::ostringstream str;
	typename std::list<T>::const_iterator ci = l.begin();
	typename std::list<T>::const_iterator cend = l.end();
	while (ci != cend) {
		str << *ci;
		++ci;
	}
	return str.str();
}
*/

extern std::string joinListEmptyBetween(const std::list<std::string>& l, unsigned int nBigStringSize = 131072);

class Bigstring; // Forward

extern void joinListInBigstring(Bigstring *pBigstring, const std::string& between, const std::list<std::string>& l);

extern void joinListEmptyBetweenInBigstring(Bigstring *pBigstring, const std::list<std::string>& l);







extern void split_string(const std::string& instring, const std::string& splitchars, std::list<std::string>& outlist);

extern void split_string(const std::string& instring, const std::string& splitchars, std::vector<std::string>& outlist);

extern void split_string_on_string(const std::string& instring, const std::string& splitstring, std::list<std::string>& outlist);

extern void split_string_on_string(const std::string& instring, const std::string& splitstring, std::vector<std::string>& outlist);

extern std::string parse_C_backslashes(const std::string& instring);

extern std::string strip(const std::string& instring, const std::string& stripchars);

// strip " \n\r\v\t"
extern std::string strip(const std::string& instring);


extern std::string emdros_int64ToCompact_string7bits(emdros_int64 input);
extern void compact_string7bits2emdros_int64(const std::string& input, emdros_int64& output);
extern void IntegerList2compact_string7bits(const IntegerList& intlist, std::string& result);
extern void compact_string7bits2IntegerList(const std::string& inStr, IntegerList& intlist);

inline bool hasXMLCharsToMangle(const std::string& input)
{
	return input.find_first_of("&\"<>", 0, 4) != std::string::npos;
}

inline std::string escapeXMLEntities(const std::string& input)
{
	std::string result;
	result.reserve(input.length() + 4);
	std::string::const_iterator ci = input.begin();
	std::string::const_iterator cend = input.end();
	while (ci != cend) {
		char c = *ci++;
		switch (c) {
		case '&':
			result += "&amp;";
			break;
		case '\"':
			result += "&quot;";
			break;
		case '<':
			result += "&lt;";
			break;
		case '>':
			result += "&gt;";
			break;
		default:
			result += c;
			break;
		}
	}
	return result;
}

inline bool hasJSONCharsToMangle(const std::string& input)
{
	return input.find_first_of("\"'\\\b\f\n\r\t", 0, 8) != std::string::npos;
}

inline std::string escapeJSONChars(const std::string& input, bool bEscapeAsUnicode)
{
	std::string result;
	result.reserve(input.length() + 4);
	std::string::const_iterator ci = input.begin();
	std::string::const_iterator cend = input.end();
	while (ci != cend) {
		char c = *ci++;
		switch (c) {
		case '\"':
			result += "\\\"";
			break;
		case '\\':
			result += "\\\\";
			break;
		case '\b':
			result += "\\b";
			break;
		case '\f':
			result += "\\f";
			break;
		case '\n':
			result += "\\n";
			break;
		case '\r':
			result += "\\r";
			break;
		case '\t':
			result += "\\t";
			break;
		default:
			result += c;
			break;
		}
	}
	if (bEscapeAsUnicode) {
		std::string real_result = escape_UTF8_string_with_slashu(result);
		return real_result;
	} else {
		return result;
	}
}





#endif // STRING_FUNC__H__

