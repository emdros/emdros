/*
 * string_func.cpp
 *
 * String helper functions
 *
 * Ulrik Petersen
 * Created: 3/1-2001
 * Last update: 7/12-2019
 *
 */

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


/**@file string_func.cpp
 *@brief Implementation of string functions (EMdF layer)
 */

#include <arena.h>
#include <string_func.h>
#include <string_list.h>
#include <debug.h>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <emdros_reserved_words.h>

#ifdef WIN32
#ifndef snprintf
/* In MSVC++ 2008, snprintf isn't defined, but _snprintf is. */
#define snprintf _snprintf
#endif
#endif


/** Upper-case a string.
 *
 * @param in The input string (not changed).
 *
 * @param result The output string.
 */
void str_toupper(const std::string& in, std::string& result)
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









/** Lower-case a string.
 *
 * Only ASCII upper-case characters are converted to their lower-case
 * equivalents.  ASCII values above 127 are not lower-cased.
 *
 * @param in The input string (not changed).
 *
 * @param result The output string.
 */
void str_tolower(const std::string& in, std::string& result)
{
	result = in;
	std::string::iterator it = result.begin();
	std::string::iterator iend = result.end();
	while (it != iend) {
		char c = *it;
		if (c >= 'A' && c <= 'Z') {
			*it = (c + 32); // Lowercase it
		} 
		++it;
	}
}

/** Lower-case a string.
 *
 * Only ASCII upper-case characters are converted to their lower-case
 * equivalents.  ASCII values above 127 are not lower-cased.
 *
 * @param in The input string, morphed into an output string that is
 * lower-cased.
 */
void str_tolower(std::string& in)
{
	std::string::iterator it = in.begin();
	std::string::iterator iend = in.end();
	while (it != iend) {
		char c = *it;
		if (c >= 'A' && c <= 'Z') {
			*it = (c + 32); // Lowercase it
		} 
		++it;
	}
}

/** Compare two strings case-INsensitively.
 *
 * See Bjarne Stroustrup, 
 * The C++ Programming Language,
 * 3rd Edition, 1997
 * p. 591
 *
 * Only ASCII upper-case characters are converted to their lower-case
 * equivalents.  ASCII values above 127 are not lower-cased before
 * comparison.
 *
 * @param str1 The first string.
 *
 * @param str2 The second string.
 *
 * @return 0 if they are equal, -1 if str1 < str2, 1 if str1 > str2,
 * all less case-sensitivity.
 */

int strcmp_nocase(const std::string& str1, const std::string& str2)
{
	std::string::const_iterator p = str1.begin();
	std::string::const_iterator str1end = str1.end();
	std::string::const_iterator q = str2.begin();
	std::string::const_iterator str2end = str2.end();
	while (p != str1end && q != str2end) {
		char tolop = *p++;
		char toloq = *q++;
		if (tolop >= 'A' && tolop <= 'Z') {
			tolop += 32;
		}
		if (toloq >= 'A' && toloq <= 'Z') {
			toloq += 32;
		}
		if (tolop != toloq)
			return (tolop < toloq) ? -1 : 1;
	}
	std::string::size_type str1size = str1.size();
	std::string::size_type str2size = str2.size();
	return (str2size == str1size) ? 0 : (str1size < str2size) ? -1 : 1;
}

/** Compare two strings with case-sensitivity.
 *
 * See Bjarne Stroustrup, 
 * The C++ Programming Language,
 * 3rd Edition, 1997
 * p. 591
 *
 * @param str1 The first string.
 *
 * @param str2 The second string.
 *
 * @return 0 if they are equal, -1 if str1 < str2, 1 if str1 > str2.
 */
int strcmp_case(const std::string& str1, const std::string& str2)
{
	std::string::const_iterator p = str1.begin();
	std::string::const_iterator str1end = str1.end();
	std::string::const_iterator q = str2.begin();
	std::string::const_iterator str2end = str2.end();
	while (p != str1end && q != str2end) {
		unsigned char cp = *p;
		unsigned char cq = *q;
		if (cp != cq)
			return (cp < cq) ? -1 : 1;
		++p;
		++q;
	}
	std::string::size_type str1size = str1.size();
	std::string::size_type str2size = str2.size();
	return (str2size == str1size) ? 0 : (str1size < str2size) ? -1 : 1;
}

/** Convert a bool to a string (either "0" or "1").
 *
 * @param b The bool to convert.
 *
 * @return "1" if b is true, "0" otherwise.
 */
std::string bool2string(bool b)
{
	if (b)
		return "1";
	else
		return "0";
}

/** Convert an eBackendKind enumeration constant to a string.
 *
 * @param backend_kind The backend kind to convert.
 *
 * @return Either "PostgreSQL", "MySQL", "SQLite", or "BackendNone"
 */
std::string backend_kind2string(eBackendKind backend_kind)
{
	switch (backend_kind) {
	case kPostgreSQL:
		return "PostgreSQL";
	case kMySQL:
		return "MySQL";
	case kSQLite3:
		return "SQLite3";
	case kBPT:
		return "bpt";
	case kBackendNone:
		return "BackendNone";
	default:
		ASSERT_THROW(false, "Unknown backend.");

		// Just to fool the compiler into not giving a warning.
		return "Unknown backend."; 
	}

}

/** Convert a string to a bool (assuming it is either "0" or "1").
 *
 * @param str The input string.
 *
 * @return True if and only if str is "1".
 */
bool string2bool(const std::string& str)
{
	return str == "1";
}

/** Convert a boolean to one of the strings "true" or "false".
 *
 * @param b The bool to convert.
 *
 * @return If b is true, then "true", otherwise, "false".
 */
std::string bool_alpha2string(bool b)
{
	if (b)
		return "true";
	else
		return "false";
}

/** Convert a string to a bool.
 *
 * @param str The input string.
 *
 * @return True if and only if the input string is "true".
 */
bool string2bool_alpha(const std::string& str)
{
	return str == "true";
}

/** Convert an emdf_ivalue to a string, base 10.
 *
 * @param i The emdf_ivalue to convert.
 *
 * @return The input emdf_ivalue, converted to a string.
 */
std::string emdf_ivalue2string(emdf_ivalue i)
{
	return longlong2string(i);
}

/** Convert an int to a string, base 10.
 *
 * @param i The int to convert.
 *
 * @return The input int, converted to a string.
 */
std::string int2string(int i)
{
	return longlong2string(i);
}

/** Convert an unsigned int to a string, base 10.
 *
 * @param i The unsigned int to convert.
 *
 * @return The input unsigned int, converted to a string.
 */
std::string uint2string(unsigned int i)
{
	return longlong2string((long long) i);
}

/** Convert a double to a string, base 10.
 *
 * @param d The double to convert.
 *
 * @return The input double, converted to a string, base 10.
 */
std::string double2string(double d)
{
	char szResult[60];
	snprintf(szResult, 60, "%lf", d);
	return szResult;
}


/** Convert a monad_m to a string, base 10.
 *
 * @param m The monad_m to convert.
 *
 * @return The input monad, converted to a string, base 10.
 */
std::string monad_m2string(monad_m m)
{
	return longlong2string(m);
}

/** Convert a string to a monad_m.
 *
 * @param str The input string (having the monad in base-10).
 *
 * @return The monad_m represented by the string.
 */
monad_m string2monad_m(const std::string& str)
{
	return (monad_m) strtoll(str.c_str(), (char **)NULL, 10);
}

/** Convert an id_d to a string, that has "NIL" for the numerical
 *  value of nil.
 *
 * @see id_d2number_string(id_d_t)
 *
 * @param i The id_d to convert.
 *
 * @return The input id_d as a string which has "NIL" for the
 * numerical value of nil.
 */
std::string id_d2string(id_d_t i)
{
	if (i == NIL) {
		return NIL_AS_VISIBLE_STRING;
	} else {
		return longlong2string(i);
	}
}

/** Convert an id_d to a string that does not contain "NIL" for the
 *  nil id_d, but instead its numerical value.
 *
 * @see id_d2string(id_d_t).
 *
 * @param i The id_d to convert.
 *
 * @return The input id_d as a numerical string.
 */
std::string id_d2number_string(id_d_t i)
{
	return longlong2string(i);
}

inline void longlong2sz(long long l, char *szResult)
{
	// Build up string in reverse
	char *p = szResult;
	long long v = l;
	do {
		long long tmp = v;
		v /= 10;
		*p++ = "9876543210123456789" [9+(tmp - v*10)];
	} while (v);
	
	// Add negative sign at the end if necessary	
	if (l < 0) {
		*p++ = '-';
	}
	
	// Terminate string
	*p-- = '\0';
	
	// Reverse string
	char *q = szResult;
	while (q < p) {
		char swap_c = *p;
		*p-- = *q;
		*q++ = swap_c;
	}
}

/** Convert a long long to a string.
 *
 * @param l The long long to convert.
 *
 * @return The input long long, converted to a string (base 10).
 */
std::string longlong2string(long long l)
{
	const int nBufLength = 30;
	// Some ideas taken from http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
	char szBuffer[nBufLength];
	
	char *szResult;

	unsigned int nLength;

	longlong2szNonReversing(l, szBuffer, nBufLength, &szResult, &nLength);

	return std::string(szResult, nLength);
}

/** Convert a long to a string.
 *
 * @param l The long to convert.
 *
 * @return The input long, converted to a string (base 10).
 */
std::string long2string(long l)
{
	return longlong2string(l);
}

void decode_format_string(const std::string& format, char& cPad, int& nMinLength)
{
	cPad = '@';
	nMinLength = -1;
	if (!format.empty()) {
		std::string::size_type nLength = format.length();
		cPad = format[0];
		if (nLength > 1) {
			nMinLength = string2long(format.substr(1, std::string::npos));
		}
	}
}

/** Convert a long long to a string using a format string.
 *
 * @param l The long long to convert.
 *
 * @param format The format string to use.
 *
 * @return The input long long, converted to a string (base 10).
 */
std::string longlong2string_format(long long l, const std::string& format)
{
	char cPad = '@';
	int nMinLength = -1;
	decode_format_string(format, cPad, nMinLength);

	const int nBufLength = 30;
	// Some ideas taken from http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
	char szBuffer[nBufLength];
	
	char *szResult;

	unsigned int nLength;

	longlong2szNonReversing(l, szBuffer, nBufLength, &szResult, &nLength);
	std::string tmp_result = std::string(szResult, nLength);

	if (nMinLength > -1) {
		int numPad = nMinLength - (int) tmp_result.length();
		if (numPad > 0) {
			std::string real_result;
			for (int i = 0; i < numPad; ++i) {
				real_result += cPad;
			}
			real_result += tmp_result;
			return real_result;
		} else{
			return tmp_result;
		}
	} else {

		return tmp_result;
	}
}

/** Convert a long to a string using a format string.
 *
 * @param l The long to convert.
 *
 * @param format The format string to use.
 *
 * @return The input long, converted to a string (base 10).
 */
std::string long2string_format(long l, const std::string& format)
{
	return longlong2string_format(l, format);
}


/** Convert a string to a long long.
 *
 * @param str The input string, base 10.
 *
 * @return The input string, converted to a long long.
 */
long long string2longlong(const std::string& str)
{
	return sz2longlong(str.c_str());
}

/** Convert a string to a long.
 *
 * @param str The input string, base 10.
 *
 * @return The input string, converted to a long.
 */
long string2long(const std::string& str)
{
	return (long) sz2longlong(str.c_str());
}

/** Convert a string to an int.
 *
 * @param str The input string, base 10.
 *
 * @return The input string, converted to an int.
 */
int string2int(const std::string& str)
{
	return (int) sz2longlong(str.c_str());
}

/** Convert a string to an emdf_ivalue.
 *
 * @param str The input string, base 10.
 *
 * @return The input string, converted to an emdf_ivalue.
 */
emdf_ivalue string2emdf_ivalue(const std::string& str)
{
	return (emdf_ivalue) sz2longlong(str.c_str());
}

/** Convert a string to an ID_D.
 *
 * "NIL" is converted to the numeric value of NIL.
 *
 * @param str The input string.
 *
 * @return The input string, converted to an id_d.
 */
id_d_t string2id_d(const std::string& str)
{
	id_d_t id_d;
	if (str == NIL_AS_VISIBLE_STRING) {
		id_d = NIL;
	} else {
		id_d = (id_d_t) strtoll(str.c_str(), (char **)NULL, 10);
	}
	return id_d;
}

/** Encode a string so it can be included in "double quotes" in an MQL
 *  script.
 *
 * @param str The input string (this is not changed).
 *
 * @return The input string, encoded so as to be able to be included
 * in an MQL script in "double quotes".
 */
std::string encodeSTRINGstring(const std::string& str)
{
	std::string::size_type strLen = str.length();
	std::string result;
	result.reserve(strLen<<2); // *4 because of \\x + char2hex(c)
	for (std::string::size_type i = 0; i < strLen; ++i) {
		unsigned char c = (unsigned char) str[i];
		if (c < 0x20 
		    || c >= 0x80) {
			result.append("\\x", 2);
			char szResult[3];
			char2hex(c, szResult);
			result.append(szResult, 2);
		} else {
			switch (c) {
			case '\\':
				//result.append("\\\\", 2);
				result += "\\\\";
				break;
			case '\"':
				//result.append("\\\"", 2);
				result += "\\\"";
				break;
			default:
				result.push_back(c);
				break;
			}
		}
	}
	return result;
}

/** Finds out whether a string is anything other than whitespace.
 *
 * @param str The string to search (is not changed).
 *
 * @return True if and only if str consists solely of
 * whitespace-characters. (" \t\n\r\v").
 */
bool is_other_than_whitespace(const std::string& str)
{
	return str.find_first_not_of(std::string(" \t\n\r\v"), 0) != std::string::npos;
}

/** Finds out whether a string has a double quote (").
 *
 * @param str The string to search (is not changed).
 *
 * @return True if and only if str has a double quote.
 */
bool has_double_quote(const std::string& str)
{
	return str.find_first_of('"') != std::string::npos;
}

/** Checks whether string is an identifier
 *
 * An identifer starts with an underscore, a lowercase-letter (a-z) or
 * an upper-case letter (A-Z).  If it is longer than one character,
 * the rest consists of either the characters just mentioned or digits
 * (0-9).
 *
 * @param str the string to be checked for identifierhood.
 * @return true if str is an identifier, false otherwise.
 */
bool is_identifier(const std::string& str)
{
	std::string non_digit = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::string digit = "0123456789";
	std::string allowed_chars = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	if (str.empty()) {
		return false;
	} else {
		if (non_digit.find(str[0]) == std::string::npos) {
			return false;
		} else {
			if (str.find_first_not_of(allowed_chars) != std::string::npos) {
				return false;
			} else {
				return true;
			}
		}
	}
}

/** Check whether a string is a hexadecimal string.
 *
 * This just checks whether the string only contains [0-9A-Fa-f].
 *
 * @param str The string to check for hexadecimal-hood.
 *
 * @return True iff str is a hexadecimal string.
 */
bool is_hex(const std::string& str)
{
	const std::string hex_digits = "0123456789abcdefABCDEF";
	return !str.empty() && str.find_first_not_of(hex_digits) == std::string::npos;
}

/** Returns the long value of an arbitrary hex string.
 *
 * The input is assumed to have been sanity-checked before hand, e.g.,
 *  with is_hex().
 *
 * @param str The string to convert
 *
 * @return The long representing the input hex string.
 */
long hex2long(const std::string& str)
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

/** Convert an unsigned long to a hexadecimal string of length 8 characters.
 *
 * Only the lowest 32 bits are converted.
 *
 * @param in The unsigned long to convert.
 *
 * @return The hexadecimal representation of the unsigned long.
 */
std::string ulong2hex(unsigned long in)
{
	std::string result;
	result.reserve(8);
	result += char2hex((in >> 24) & 0xFF);
	result += char2hex((in >> 16) & 0xFF);
	result += char2hex((in >> 8) & 0xFF);
	result += char2hex(in & 0xFF);
	return result;
}

/** Add \u10FFFFF hex-representation representation of a Unicode code point.
 *
 * Adds \uxxxx if <= 0xFFFF.
 *
 * Adds \uxxxxx if <= 0xFFFFF.
 *
 * Adds \uxxxxxx if <= 0xFFFFFF.
 *
 * Note that the highest possible Unicode code point is 0x0010FFFF.
 *
 * @param result The std::string into which to append the character.
 *
 * @param c The Unicode code point to add.
 *
 */
void codepoint2slashu(std::string& result, unsigned int c)
{
	char szResult[2+6+1];
	char *begin = &(szResult[0]);
	char *p = begin;
	*p++ = '\\';
	*p++ = 'u';
	if (c > 0x000FFFFF) {
		*p++ = "0123456789abcdef"[(c >> 20) & 0x0F];
	}
	if (c > 0x0000FFFF) {
		*p++ = "0123456789abcdef"[(c >> 16) & 0x0F];
	}
	*p++ = "0123456789abcdef"[(c >> 12) & 0x0F];
	*p++ = "0123456789abcdef"[(c >> 8) & 0x0F];
	*p++ = "0123456789abcdef"[(c >> 4) & 0x0F];
	*p++ = "0123456789abcdef"[c & 0x0F];
	*p = '\0';
	
	result+= std::string(begin, (p - begin));
}

/** Replace occurrences of one character with another character in a
 *  string.
 *
 * @param s The input (and output) string.
 *
 * @param in The character to change from.
 *
 * @param out The character to change to.
 */
void replace_chars(std::string &s, const char in, const char out)
{
	std::string::size_type i = 0;
	while ((i = s.find(in, i)) != std::string::npos)
		s[i] = out;
}

/** Replace underscores with spaces.
 *
 * @param s The input (and output) string.
 */
void replace_underscores(std::string& s)
{
	replace_chars(s, '_', ' ');
}


/** Replace spaces with underscores.
 *
 * @param s The input (and output) string.
 */
void replace_spaces(std::string& s)
{
	replace_chars(s, ' ', '_');
}

/** Reverse a string.
 *
 * @param in The input string (is not changed).
 *
 * @return The input string, reversed.
 */
std::string reverse_string(const std::string& in)
{
	std::string::size_type i;
	std::string result;
	if (in.length() > 0) {
		i = in.length();
		do {
			i--;
			result.push_back(in[i]);
		} while (i != 0);
	}

	return result;
}

/** Finds out whether a string is an integer or not.
 *
 * @param in The input string.
 *
 * @return True if this is an integer, False if it is not.
 */
bool string_is_number(const std::string& in)
{
	return (in.size() > 0) 
		&& (in.find_first_not_of("0123456789", 0) == std::string::npos);
}


/** Convert a string to URL-encoded (%-hex-encoded) strings.
 *
 * Only characters above 0x7F (and the %-character) are converted to
 * %xx-hex-format.
 * 
 * @param instr The input string (is not changed).
 *
 * @return The input string, converted to hex where necessary.
 */
std::string convert_to_hex(std::string instr)
{
	std::string outstr;
	for (unsigned int i = 0; i < instr.length(); i++) {
		if (isspace((unsigned char)instr[i]) 
		    || instr[i] == '%' 
		    || ((unsigned char) instr[i]) >= 0x80) {
			outstr.push_back('%');

			// Get hexadecimal representation
			std::string hex;
			std::ostringstream ostream(hex);
			ostream.setf(std::ios_base::hex, std::ios_base::basefield);
			ostream.precision(2);
			ostream << (int) ((unsigned char) instr[i]);
			outstr.append(ostream.str());
		} else {
			outstr.push_back(instr[i]);
		}
	}
	return outstr;
}


/** Remove all instances of a given character from a string, and return
 * the result.
 *
 * @param instr The input string (is not changed).
 *
 * @param char_to_remove The character to remove.
 *
 * @return The input string, less any instances of the given character.*
 */
std::string remove_char(std::string instr, char char_to_remove)
{
	std::string outstr;
	for (unsigned int i = 0; i < instr.length(); i++) {
		if (instr[i] != char_to_remove) {
			outstr.push_back(instr[i]);
		}
	}
	return outstr;
}

/** Remove all instances of characters given string in a from a
 * string, and return the result.
 *
 * @param instr The input string (is not changed).
 *
 * @param chars_to_remove The characters to remove.
 *
 * @return The input string, less any instances of the given characters.
 */
std::string remove_chars_in_string(const std::string& instr, const std::string& chars_to_remove)
{
	std::string result;
	for (std::string::size_type i = 0;
	     i < instr.length();
	     ++i) {
		char c = instr[i];
		if (chars_to_remove.find(c, 0) != std::string::npos) {
			// skipt his char
		} else {
			// add this char
			result.append(1,c);
		}
	}
	return result;
}

/** Remove punctuation from a string.
 *
 * Punctuation is defined as being one of the characters
 * "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~".
 *
 * @param instr The input string (is not changed).

 * @return The input string, less any instances of the given
 * punctuation characters.
 */
std::string remove_punct(const std::string& instr)
{
	return remove_chars_in_string(instr, "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
}


/** Convert a string to an eCharsets enumeration.
 *
 * Find out which eCharsets enumeration constant most closely matches
 * a given string.
 *
 *
 * @param input The string-representation (probably from the user)
 * which is to be converted to eCharsets.
 *
 * @param charset The output charset.  Is only defined if the function
 * returns true.
 *
 * @return True iff we were able to recognize and thus convert the
 * charset.
 *
 */
bool string2charset(const std::string& input, eCharsets& charset)
{
	if (strcmp_nocase(input, "ascii") == 0) {
		charset = kCSASCII;       /**< 7-bit ASCII.  */
		return true;
	} else if (strcmp_nocase(input, "utf8") == 0
		   || strcmp_nocase(input, "utf-8") == 0) {
 		charset = kCSUTF8;        /**< UTF-8. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-1") == 0
		   || strcmp_nocase(input, "iso_8859_1") == 0
		   || strcmp_nocase(input, "iso-8859_1") == 0
		   || strcmp_nocase(input, "iso-8859-1") == 0) {
		charset = kCSISO_8859_1;  /**< ISO-8859-1. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-2") == 0
		   || strcmp_nocase(input, "iso_8859_2") == 0
		   || strcmp_nocase(input, "iso-8859_2") == 0
		   || strcmp_nocase(input, "iso-8859-2") == 0) {
		charset = kCSISO_8859_2;  /**< ISO-8859-2. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-3") == 0
		   || strcmp_nocase(input, "iso_8859_3") == 0
		   || strcmp_nocase(input, "iso-8859_3") == 0
		   || strcmp_nocase(input, "iso-8859-3") == 0) {
		charset = kCSISO_8859_3;  /**< ISO-8859-3. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-4") == 0
		   || strcmp_nocase(input, "iso_8859_4") == 0
		   || strcmp_nocase(input, "iso-8859_4") == 0
		   || strcmp_nocase(input, "iso-8859-4") == 0) {
		charset = kCSISO_8859_4;  /**< ISO-8859-4. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-5") == 0
		   || strcmp_nocase(input, "iso_8859_5") == 0
		   || strcmp_nocase(input, "iso-8859_5") == 0
		   || strcmp_nocase(input, "iso-8859-5") == 0) {
		charset = kCSISO_8859_5;  /**< ISO-8859-5. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-6") == 0
		   || strcmp_nocase(input, "iso_8859_6") == 0
		   || strcmp_nocase(input, "iso-8859_6") == 0
		   || strcmp_nocase(input, "iso-8859-6") == 0) {
		charset = kCSISO_8859_6;  /**< ISO-8859-6. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-7") == 0
		   || strcmp_nocase(input, "iso_8859_7") == 0
		   || strcmp_nocase(input, "iso-8859_7") == 0
		   || strcmp_nocase(input, "iso-8859-7") == 0) {
		charset = kCSISO_8859_7;  /**< ISO-8859-7. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-8") == 0
		   || strcmp_nocase(input, "iso_8859_8") == 0
		   || strcmp_nocase(input, "iso-8859_8") == 0
		   || strcmp_nocase(input, "iso-8859-8") == 0) {
		charset = kCSISO_8859_8;  /**< ISO-8859-8. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-9") == 0
		   || strcmp_nocase(input, "iso_8859_9") == 0
		   || strcmp_nocase(input, "iso-8859_9") == 0
		   || strcmp_nocase(input, "iso-8859-9") == 0) {
		charset = kCSISO_8859_9;  /**< ISO-8859-9. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-10") == 0
		   || strcmp_nocase(input, "iso_8859_10") == 0
		   || strcmp_nocase(input, "iso-8859_10") == 0
		   || strcmp_nocase(input, "iso-8859-10") == 0) {
		charset = kCSISO_8859_10;  /**< ISO-8859-10. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-13") == 0
		   || strcmp_nocase(input, "iso_8859_13") == 0
		   || strcmp_nocase(input, "iso-8859_13") == 0
		   || strcmp_nocase(input, "iso-8859-13") == 0) {
		charset = kCSISO_8859_13;  /**< ISO-8859-13. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-14") == 0
		   || strcmp_nocase(input, "iso_8859_14") == 0
		   || strcmp_nocase(input, "iso-8859_14") == 0
		   || strcmp_nocase(input, "iso-8859-14") == 0) {
		charset = kCSISO_8859_14;  /**< ISO-8859-14. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-15") == 0
		   || strcmp_nocase(input, "iso_8859_15") == 0
		   || strcmp_nocase(input, "iso-8859_15") == 0
		   || strcmp_nocase(input, "iso-8859-15") == 0) {
		charset = kCSISO_8859_15;  /**< ISO-8859-15. */
		return true;
	} else if (strcmp_nocase(input, "iso_8859-16") == 0
		   || strcmp_nocase(input, "iso_8859_16") == 0
		   || strcmp_nocase(input, "iso-8859_16") == 0
		   || strcmp_nocase(input, "iso-8859-16") == 0) {
		charset = kCSISO_8859_16;  /**< ISO-8859-16. */
		return true;
	} else {
		return false;
	}
}

bool charset2string(eCharsets charset, std::string& output)
{
	switch (charset) {
	case kCSASCII:
		output = "ascii";
		return true;
		break;
	case kCSISO_8859_1:
		output = "iso-8859-1";
		return true;
		break;
	case kCSISO_8859_8:
		output = "iso-8859-8";
		return true;
		break;
	case kCSUTF8:
		output = "utf8";
		return true;
		break;
	case kCSISO_8859_2:
		output = "iso-8859-2";
		return true;
		break;
	case kCSISO_8859_3:
		output = "iso-8859-3";
		return true;
		break;
	case kCSISO_8859_4:
		output = "iso-8859-4";
		return true;
		break;
	case kCSISO_8859_5:
		output = "iso-8859-5";
		return true;
		break;
	case kCSISO_8859_6:
		output = "iso-8859-6";
		return true;
		break;
	case kCSISO_8859_7:
		output = "iso-8859-7";
		return true;
		break;
	case kCSISO_8859_9:
		output = "iso-8859-9";
		return true;
		break;
	case kCSISO_8859_10:
		output = "iso-8859-10";
		return true;
		break;
	case kCSISO_8859_13:
		output = "iso-8859-13";
		return true;
		break;
	case kCSISO_8859_14:
		output = "iso-8859-14";
		return true;
		break;
	case kCSISO_8859_15:
		output = "iso-8859-15";
		return true;
		break;
	case kCSISO_8859_16:
		output = "iso-8859-16";
		return true;
		break;
	default:
		output.clear();
		return false;
	}
}





/** Convert a string to an eBackendKind enumeration.
 *
 * Find out which eBackendKind enumeration constant most closely matches
 * a given string.
 *
 * @param input The string-representation (probably from the user)
 * which is to be converted to eBackendKind.
 *
 * @param backend The output backend kind.  Is only defined if the
 * function returns true.
 *
 * @return True iff we were able to recognize and thus convert the
 * backend kind.
 *
 */
bool string2backend_kind(const std::string& input, eBackendKind& backend)
{
	if (strcmp_nocase(input, "pg") == 0
	    || strcmp_nocase(input, "p") == 0
	    || strcmp_nocase(input, "postgresql") == 0
	    || strcmp_nocase(input, "postgres") == 0) {
		backend = kPostgreSQL;
		return true;
	} else if (strcmp_nocase(input, "my") == 0
		   || strcmp_nocase(input, "m") == 0
		   || strcmp_nocase(input, "mysql") == 0) {
		backend = kMySQL;
		return true;
	} else if (strcmp_nocase(input, "3") == 0
		   || strcmp_nocase(input, "lt3") == 0
		   || strcmp_nocase(input, "sqlite3") == 0
		   || strcmp_nocase(input, "s3") == 0) {
		backend = kSQLite3;
		return true;
	} else if (strcmp_nocase(input, "b") == 0
		   || strcmp_nocase(input, "bpt") == 0) {
		backend = kBPT;
		return true;
	} else {
		return false;
	}
}


/** Split a string into a list of strings, based on a string of
 * characters to split on.
 *
 * The characters are not included in the split strings.
 *
 * If none of the characters are found, then the list containing
 * instring is returned in outlist.
 *
 * If the string begins with one or more characters from the
 * splitchars string, then an empty string is prepended the list.
 *
 * If the string ends with one or more characters from the splitchars
 * string, and the string also contains non-splitchars characters,
 * then an empty string is appended to the list.
 *
 * @param instring The input string.
 *
 * @param splitchars The string of characters on which to split.
 *
 * @param outlist The output list of strings.*/
void split_string(const std::string& instring, const std::string& splitchars, std::list<std::string>& outlist)
{
	if (instring.find_first_not_of(splitchars, 0) == std::string::npos) {
		// The input contains only chars from
		// splitchars. Append the empty string.
		outlist.push_back("");
		return;
	} else {
		std::string::size_type nbegin, nend;
		nend = instring.find_first_of(splitchars, 0);
		nbegin = instring.find_first_not_of(splitchars, 0);
		if (nend == 0 && nbegin == std::string::npos) {
			// We have only splitchars chars. Append an empty
			// string and return.
			outlist.push_back("");
			return;
		} else if (nend == 0 && nbegin != std::string::npos) {
			// We start with at least one splitchars char.
			// Start the list with an empty string.
			outlist.push_back("");
		}

		// Now search for the next splitchars after nbegin
		nend = instring.find_first_of(splitchars, nbegin);
		if (nend == std::string::npos) {
			outlist.push_back(instring.substr(nbegin, instring.length()-nbegin));
		} else {
			outlist.push_back(instring.substr(nbegin, nend-nbegin));
			nbegin = instring.find_first_not_of(splitchars, nend);
			
			while (nbegin != std::string::npos
			       && nend != std::string::npos) {
				nend = instring.find_first_of(splitchars, nbegin);
				if (nend == std::string::npos) {
					outlist.push_back(instring.substr(nbegin, instring.length()-nbegin));
					break;
				} else {
					outlist.push_back(instring.substr(nbegin, nend-nbegin));
				}
				nbegin = instring.find_first_not_of(splitchars, nend);
			}

			if (nbegin == std::string::npos
			    && nend != std::string::npos) {
				// We end with one or more splitchars.
				// Append an empty string.
				outlist.push_back("");
			}
		}
	}
}


/** Split a string into a list of strings, based on a substring to split on.
 *
 * The substring is not included in the split strings.
 *
 * If the substring is not found, then the list containing only
 * instring is returned in outlist.
 *
 * @param instring The input string.
 *
 * @param splitstring The string on which to split.
 *
 * @param outlist The output list of strings.*/
void split_string_on_string(const std::string& instring, const std::string& splitstring, std::list<std::string>& outlist)
{
	if (instring.find(splitstring, 0) == std::string::npos) {
		// Add instring as sole output in list
		outlist.push_back(instring);
		return;
	} else {
		std::string::size_type nbegin, nend;
		if (instring.find(splitstring, 0) == 0) {
			nbegin = splitstring.length();
		} else {
			nbegin = 0;
		}
		nend = instring.find(splitstring, nbegin + splitstring.length());
		if (nend == std::string::npos) {
			outlist.push_back(instring.substr(nbegin, instring.length()-nbegin));
		} else {
			outlist.push_back(instring.substr(nbegin, nend-nbegin));
			nbegin = nend + splitstring.length();
			while (nbegin != std::string::npos
			       && nend != std::string::npos
			       && nbegin != instring.length()) {
				nend = instring.find(splitstring, nbegin);
				if (nend == std::string::npos) {
					outlist.push_back(instring.substr(nbegin, instring.length()-nbegin));
					break;
				} else {
					outlist.push_back(instring.substr(nbegin, nend-nbegin));
				}
				nbegin = nend + splitstring.length();
			}
		}
	}
}


/** Split a string into a vector of string, based on a string of
 *  characters to split on.
 *
 * The characters are not included in the split strings.
 *
 * If none of the characters are found, then the list containing only
 * instring is returned in outlist.
 *
 * This just calls the list equivalent, and converts the list to a
 * vector.
 *
 * @param instring The input string.
 *
 * @param splitchars The string of characters on which to split.
 *
 * @param outlist The output vector of strings.*/
void split_string(const std::string& instring, const std::string& splitchars, std::vector<std::string>& outlist)
{
	std::list<std::string> result_list;
	split_string(instring, splitchars, result_list);
	outlist.resize(result_list.size());
	std::list<std::string>::const_iterator ci = result_list.begin();
	std::list<std::string>::const_iterator cend = result_list.end();
	std::vector<std::string>::size_type i = 0;
	while (ci != cend) {
		outlist[i] = *ci;
		++i;
		++ci;
	}
}

/** Split a string into a vector of string, based on a string 
 * to split on.
 *
 * The split-string is not included in the split strings.
 *
 * If the split-string is not found, then the list containing only
 * instring is returned in outlist.
 *
 * This just calls the list equivalent, and converts the list to a
 * vector.
 *
 * @param instring The input string.
 *
 * @param splitstring The string of characters on which to split.
 *
 * @param outlist The output vector of strings.*/
void split_string_on_string(const std::string& instring, const std::string& splitstring, std::vector<std::string>& outlist)
{
	std::list<std::string> result_list;
	split_string_on_string(instring, splitstring, result_list);
	outlist.resize(result_list.size());
	std::list<std::string>::const_iterator ci = result_list.begin();
	std::list<std::string>::const_iterator cend = result_list.end();
	std::vector<std::string>::size_type i = 0;
	while (ci != cend) {
		outlist[i] = *ci;
		++i;
		++ci;
	}
}

/** Remove from the beginning of a substring and to the end of the string.
 *
 * Removes everything in a string starting from the beginning of a
 * given substring to the end of the string.  If it is not found, then
 * the input string is returned.
 *
 *@param instr The input string (is not changed).
 *
 *@param substr The substring to find.
 *
 *@return The resulting string.
 */
std::string remove_from_substring_and_out(const std::string& instr, const std::string& substr)
{
	std::string::size_type pos = instr.find(substr);
	if (pos == std::string::npos) {
		return instr;
	} else if (pos == 0) {
		return "";
	} else {
		return instr.substr(0,pos);
	}
}


/** Convert a string containing C escapes to the string with the 
 * escapes translated to chraracters.
 *
 * @param instring The string to convert.
 *
 * @return instring, with C escape sequences converted to their 
 * character equivalents.
 */
std::string parse_C_backslashes(const std::string& instring)
{
	std::string result;
	std::string::size_type size = instring.length();
	for (std::string::size_type i = 0;
	     i < size;
	     ++i) {
		char c = instring[i];
		if (c == '\\') {
			if (i < size-1) {
				++i;
				char c2 = instring[i];
				switch (c2) {
				case 'n':
					result += '\n';
					break;
				case 't':
					result += '\t';
					break;
				case 'v':
					result += '\v';
					break;
				case 'b':
					result += '\b';
					break;
				case 'a':
					result += '\a';
					break;
				case 'r':
					result += '\r';
					break;
				case 'f':
					result += '\f';
					break;
				case '\\':
					result += '\\';
					break;
				case '?':
					result += '\?';
					break;
				case '\'':
					result += '\'';
					break;
				case '"':
					result += '"';
					break;
				case 'x':
					if (i < size-3) {
						std::string hex = std::string(1, instring[i+1]) + instring[i+2];
						if (is_hex(hex)) {
							i += 2;
							result += hex2char(hex.c_str());
						} else {
							result += c2;
						}
					} else {
						result += c2;
					}
					break;
				case '0':
				case '1':
				case '2':
				case '3':
					if (i < size-3) {
						if (isdigit(instring[i])
						    && isdigit(instring[i+1])
						    && isdigit(instring[i+2])) {
							std::string oct = std::string(1, instring[i]) + std::string(1, instring[i+1]) + instring[i+2];
							i += 2;
							result += octal2char(oct.c_str());
						} else {
							result += c2;
						}
					} else {
						result += c2;
					}
					break;
				default:
					result += c2;
					break;
				}
			} else {
				result += c;
			}
		} else {
			result += c;
		}
	}
	return result;
}



/** Replace a substring with another string in a string.
 *
 * @param instring the input string
 *
 * @param substring_to_replace the substring of instring to replace
 *
 * @param replacement_string the string to replce substring_to_replace by
 *
 * @return instring, with all non-overlapping instances of
 * substring_to_replace replaced by replacement_string.
 */
std::string replace_substring(const std::string& instring, const std::string& substring_to_replace, const std::string& replacement_string)
{
	if (instring.find(substring_to_replace, 0) == std::string::npos
	    || substring_to_replace.empty()) {
		return instring; // Return string without changing it
	} else if (instring == substring_to_replace) {
		return replacement_string;
	} else {
		std::string result;
		std::string::size_type nbegin, nend;
		if ((nbegin = instring.find(substring_to_replace, 0)) == 0) {
			// Start next search just after substring_to_replace
			nbegin = substring_to_replace.length();
			// Replace [0:len(substring_to_replace)] 
			// with replacement_string
			result = replacement_string;
		} else {
			// Add original chars up to the beginning
			// of the substring to replace
			result = instring.substr(0, nbegin);
		}
		nend = instring.find(substring_to_replace, nbegin);
		if (nend == std::string::npos) {
			result += instring.substr(nbegin);
		} else {
			result += instring.substr(nbegin, nend-nbegin);
			result += replacement_string;
			nbegin = nend + substring_to_replace.length();
			while (nbegin != std::string::npos
			       && nend != std::string::npos
			       && nbegin != instring.length()) {
				nend = instring.find(substring_to_replace, nbegin);
				if (nend == std::string::npos) {
					result += instring.substr(nbegin);
					break;
				} else {
					result += instring.substr(nbegin, nend-nbegin);
					result += replacement_string;
				}
				nbegin = nend + substring_to_replace.length();
			}
		}
		return result;
	}
}

/** Strip characters from begining and end of a string.
 *
 * @param instring The input string.
 *
 * @param stripchars The string containing the characters to
 * strip. Must not be empty.
 *
 * @return The result string, that is, instring with any characters
 * from stripchars removed from the beginning and end.
 */
std::string strip(const std::string& instring, const std::string& stripchars)
{
	std::string::size_type i = instring.find_first_not_of(stripchars);
	if (i == std::string::npos) {
		return "";
	}
	std::string::size_type iend = instring.find_last_not_of(stripchars);
	if (iend == std::string::npos) {
		return instring.substr(i, std::string::npos);
	} else {
		return instring.substr(i, (iend-i)+1);
	}
}



/** Strip whitespace from beginning and end of instring.
 *
 * Removes all whitespace characters from beginning and end of
 * instring.
 *
 * @param instring The string to strip.
 *
 * @return The string with whitespace removed from both ends.
 */
std::string strip(const std::string& instring)
{
	std::string::size_type length = instring.length();
	if (length == 0) {
		return instring;
	}
	std::string::size_type p = 0;
	std::string::size_type q = length - 1;
	bool bNotFound = true;
	while (p < length
	       && bNotFound) {
		bNotFound = false;
		switch (instring[p]) {
		case ' ':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			++p;
			bNotFound = true;
		}
	}
	// If p is the length of the string at this point,
	// it was all whitespace.
	if (p == length) {
		return "";
	}

	bNotFound = true;

	// We now know that even if p == q,
	// instring[q] won't be whitespace.
	while (q > p
	       && bNotFound) {
		char cQ = instring[q];
		switch (cQ) {
		case ' ':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			--q;
			break;
		default:
			bNotFound = false;
		}
	}
	if (p == 0 && (q == length-1)) {
		return instring;
	} else {
		return instring.substr(p, (q-p)+1);
	}
}

std::string long2compact_string7bits(long input)
{
	std::string result;
	result.reserve(5);
	char m0,m1,m2,m3,m4;
	m0 = input & 0x7F;
	m1 = (input >> 7) & 0x7f;
	m2 = (input >> 14) & 0x7f;
	m3 = (input >> 21) & 0x7f;
	m4 = (input >> 28) & 0x7f;
	bool bBegun = false;
	if ((m4 > 0) || bBegun) {
		result += (m4 + '0');
		bBegun = true;
	}
	if ((m3 > 0) || bBegun) {
		result += (m3 + '0');
		bBegun = true;
	}
	if ((m2 > 0) || bBegun) {
		result += (m2 + '0');
		bBegun = true;
	}
	if ((m1 > 0) || bBegun) {
		result += (m1 + '0');
	}
	result += (m0 + '0');
	return result;
}


void compact_string7bits2long(const std::string& inStr, long& output)
{
	std::string::size_type i = 0, end = inStr.size();
	output = 0;
	if (end == 0) {
		return;
	}
	char c = inStr[i++];
	output |= c - '0';
	while (i != end) {
		c = inStr[i++];
		output <<= 7;
		output |= c - '0';
	}
}

void IntegerList2compact_string7bits(const IntegerList& intlist, std::string& result)
{
	result = " ";
	IntegerListConstIterator cit = intlist.const_iterator();
	while (cit.hasNext()) {
		long curlong = cit.next();
		std::string this_compact_string;
		result += long2compact_string7bits(curlong);
	}
	result += " ";
}

void compact_string7bits2IntegerList(const std::string& inStr, IntegerList& intlist)
{
	std::string::size_type i = 1; // We must skip the initial space
	std::string::size_type end = inStr.size();
	if (end == 0) {
		return;
	}
	--end; // We must skip the last space.
	char c;
	while (i != end) {
		long cur_long = 0;
		bool bContinue = true;
		while (bContinue) {
			c = inStr[i++];
			if (c == ' ') {
				bContinue = false;
			} else {
				cur_long <<= 7;
				cur_long |= c - '0';
			}
			if (i == end) {
				bContinue = false;
			}
		}
		intlist.addIntegerBack(cur_long);
	}
}

/** Check whether input string ends with end string.
 *
 * @param input The string to test
 *
 * @param end The string which may or may not be at the end of the
 * input string.
 *
 * @return True if input ends with end , false otherwise.
 */
bool string_ends_with(const std::string& input, const std::string& end)
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


/** Convert a single Unicode character (a long) to UTF-8.
 *
 * "Stolen" from utf.c in SQLite 3, which is Public Domain code.
 *
 * @param input the Unicode character
 *
 * @param output The input encoded as a single UTF-8 character.
 */
void long2utf8(long c, std::string& output)
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

/*
** This lookup table is used to help decode the first byte of
** a multi-byte UTF8 character.
**
** "Stolen" from utf.c from SQLite3, which is Public Domain code.
**
** Adapted by Ulrik Sandborg-Petersen on 2016-06-18.
*/
static const unsigned char emdrosUtf8Trans[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x00, 0x00,
};


/*
**
** Translate a single UTF-8 character.  Return the unicode value.
**
** "Stolen" from utf.c from SQLite3, which is Public Domain code.
**
** Adapted by Ulrik Sandborg-Petersen on 2016-06-18.
**
**
** During translation, assume that the byte that zTerm points
** is a 0x00.
**
** Write a pointer to the next unread byte back into *pzNext.
**
** Notes On Invalid UTF-8:
**
**  *  This routine never allows a 7-bit character (0x00 through 0x7f) to
**     be encoded as a multi-byte character.  Any multi-byte character that
**     attempts to encode a value between 0x00 and 0x7f is rendered as 0xfffd.
**
**  *  This routine never allows a UTF16 surrogate value to be encoded.
**     If a multi-byte character attempts to encode a value between
**     0xd800 and 0xe000 then it is rendered as 0xfffd.
**
**  *  Bytes in the range of 0x80 through 0xbf which occur as the first
**     byte of a character are interpreted as single-byte characters
**     and rendered as themselves even though they are technically
**     invalid characters.
**
**  *  This routine accepts over-length UTF8 encodings
**     for unicode values 0x80 and greater.  It does not change over-length
**     encodings to 0xfffd as some systems recommend.
*/
#define EMDROS_READ_UTF8_C(zIn, zTerm, c)                           \
  c = *(zIn++);                                            \
  if( c>=0xc0 ){                                           \
    c = emdrosUtf8Trans[c-0xc0];                         \
    while( zIn!=zTerm && (*zIn & 0xc0)==0x80 ){            \
      c = (c<<6) + (0x3f & *(zIn++));                      \
    }                                                      \
    if( c<0x80                                             \
        || (c&0xFFFFF800)==0xD800                          \
        || (c&0xFFFFFFFE)==0xFFFE ){  c = 0xFFFD; }        \
  }
#define EMDROS_READ_UTF8_CPLUSPLUS(instr, instr_length, index, c)   \
  c = instr[index++]; \
  if( c>=0xc0 ){                                           \
    c = emdrosUtf8Trans[(c-0xc0) & 0xFF];				\
    while( index != instr_length && (instr[index] & 0xc0)==0x80 ){            \
      c = (c<<6) + (0x3f & instr[index++]);                      \
    }                                                      \
    if( c<0x80                                             \
        || (c&0xFFFFF800)==0xD800                          \
        || (c&0xFFFFFFFE)==0xFFFE ){  c = 0xFFFD; }        \
  }
unsigned int readOneUTF8Char(const std::string& instr, std::string::size_type instr_length, std::string::size_type& index)
{
  unsigned int c;

  if (index >= instr_length) {
	  return 0;
  }
  c = instr[index++];
  if( c>=0xc0 ){
    c = emdrosUtf8Trans[(c-0xc0) & 0xFF];
    while( (instr[index] & 0xc0)==0x80 ){
      c = (c<<6) + (0x3f & instr[index++]);
    }
    if( c<0x80
        || (c&0xFFFFF800)==0xD800
        || (c&0xFFFFFFFE)==0xFFFE ){  c = 0xFFFD; }
  }
  return c;
}


std::string escape_UTF8_string_with_slashu(const std::string& instr)
{
	std::string result;
	std::string::size_type index = 0;
	std::string::size_type instr_length = instr.length();
	while (index < instr_length) {
		unsigned int c = 0;
		EMDROS_READ_UTF8_CPLUSPLUS(instr, instr_length, index, c);
		if (c < 0x80) {
			result += c;
		} else {
			codepoint2slashu(result, c);
		}
	}
	return result;
}


// Convert all non-identifier characters to underscore..
std::string convert_to_identifier(const std::string& instr)
{
	std::string result;
	
	std::string::size_type instr_size = instr.size();
	for (std::string::size_type i = 0; i < instr_size; ++i) {
		char c = instr[i];
		if ((c >= 'A' && c <= 'Z')
		    || (c >= 'a' && c <= 'z')
		    || (c >= '0' && c <= '9')
		    || (c == '_')) {
			result += c;
		} else {
			result += '_';
		}
	}

	// Prepend an underscore if it starts with a number.
	if (!result.empty()) {
		char first_char = result[0];
		if (first_char >= '0'
		    && first_char <= '9') {
			result = std::string("_") + result;
		}
	}

	for (int index = 0; emdros_reserved_words[index] != NULL; ++index) {
		if (strcmp_nocase(result, emdros_reserved_words[index]) == 0) {
			result = result + std::string("_");
			break;
		}
	}

	return result;
}



void joinListEmptyBetweenInBigstring(Bigstring *pBigstring, const std::list<std::string>& l)
{
	std::list<std::string>::const_iterator ci = l.begin();
	std::list<std::string>::const_iterator cend = l.end();
	while (ci != cend) {
		pBigstring->addChars(ci->data(), ci->length());
		++ci;
	}
}

std::string joinListEmptyBetween(const std::list<std::string>& l, unsigned int nBigStringSize)
{
	Bigstring bigstr(nBigStringSize);
	joinListEmptyBetweenInBigstring(&bigstr, l);
	return bigstr.toString();
}


void joinListInBigstring(Bigstring *pBigstring, const std::string& between, const std::list<std::string>& l)
{
	std::list<std::string>::const_iterator ci = l.begin();	
	std::list<std::string>::const_iterator cend = l.end();
	std::string::size_type between_length = between.length();
	if (ci != cend) {
		pBigstring->addChars(ci->data(), ci->length());
		++ci;
	}
	while (ci != l.end()) {
		pBigstring->addChars(between.data(), between_length);
		pBigstring->addChars(ci->data(), ci->length());
		++ci;
	}
}

std::string joinList(const std::string& between, const std::list<std::string>& l, unsigned int nBigStringSize)
{
	Bigstring bigstring(nBigStringSize);
	joinListInBigstring(&bigstring, between, l);
	return bigstring.toString();
}

std::string joinVector(const std::string& between, const std::vector<std::string>& v, unsigned int nBigStringSize)
{
	std::list<std::string> l;
	for (std::vector<std::string>::size_type i = 0;
	     i < v.size();
	     ++i) {
		l.push_back(v[i]);
	}
	return joinList(between, l, nBigStringSize);
}



std::string joinList(const std::string& between, const std::list<id_d_t>& l)
{
	Bigstring bigstring;
	std::list<id_d_t>::const_iterator ci = l.begin();	
	std::list<id_d_t>::const_iterator cend = l.end();
	std::string::size_type between_length = between.length();
	char szLongDigits[30];
	if (ci != cend) {
		long long l = *ci;

		longlong2sz(l, szLongDigits);

		bigstring.addsz(szLongDigits);
		
		++ci;
	}
	while (ci != cend) {
		bigstring.addChars(between.data(), between_length);

		long long l = *ci;

		longlong2sz(l, szLongDigits);

		bigstring.addsz(szLongDigits);

		++ci;
	}
	return bigstring.toString();
}

void print_indent(std::ostream *pOut, int indent)
{
	if (pOut != 0) {
		for (int i = 0;
		     i < indent;
		     ++i) {
			(*pOut) << "   ";
		}
	}
}
