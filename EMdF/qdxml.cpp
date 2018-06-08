/*
 * qdxml.cpp
 *
 * Quick'n Dirty XML SAX-like parser
 *
 * Ulrik Petersen
 * Created: 4/4-2007
 * Last update: 6/9-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2018  Ulrik Sandborg-Petersen
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

#include <qdxml.h>
#include <cstring>


/*
 * This hack is necessary because:
 *
 * - Visual C++ doesn't support std::map<_K,_V>::at().
 *
 * - The AttributeMap formal parameter is always const (and rightly so!)
 * 
 * - We can't use [] because it is not const.
 *
 * - Hence, we need another method.  This seems cleanest.
 *
 */
std::string getAttribute(const AttributeMap& attrs, const std::string& key)
{
	AttributeMap::const_iterator ci = attrs.find(key);
	if (ci == attrs.end()) {
		throw QDException("Could not find attribute with name '" + key + "'");
	} else {
		return ci->second;
	}
}






const std::string QDParser::NOTAG = "<>";

std::string QDParser::state2string(eQDStates e)
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
		throw QDException(str.str());
	}
}
	
#define APPEND_CHAR(s, c) s += (char) c
#define SET_CHAR(s, c) s = (char) c

void QDParser::parse(QDDocHandler *pDH, std::istream *pStream) 
{
	bot = cur = lim = top = eof = 0;
	m_pInStream = pStream;
	m_pDH = pDH;
	m_tag_depth = 0;
	eQDStates state = BEFORE_XMLDECLARATION;
	m_encoding = kCSUTF8; // This is the default for XML
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
					if (m_attributes.find("encoding") != m_attributes.end()) {
						std::string strencoding = m_attributes["encoding"];
						eCharsets tmpcharset;
						if (string2charset(strencoding, tmpcharset)) {
							m_encoding = tmpcharset;
						} else {
							// Use UTF-8 if charset unknown.
							// FIXME: Should we do an exception?
							m_encoding = kCSUTF8;
						}
					} else {
						m_encoding = kCSUTF8;
					}

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
				if (string_ends_with(m_text, "-->")) {
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
				if (string_ends_with(m_text, "]]")) {
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
	
void QDParser::except(const std::string& s)
{
	std::ostringstream ostr;
	ostr << s << " near line " << m_line << ", column " << m_column << '!' << std::endl;
	throw QDException(ostr.str());
}

void QDParser::handle_entity(eQDStates state)
{
	if (!m_entity.empty() && m_entity[0] == '#') {
		long l = 0;
		if (m_entity.length() > 1
		    && (m_entity[1] == 'x' 
			|| m_entity[1] == 'X')) {
			std::string hexstring = m_entity.substr(2, std::string::npos);
			if (is_hex(hexstring)) {
				l = hex2long(hexstring);
			} else {
				except("Invalid hex-string in entity &" + m_entity + ";.");
			}
		} else {
			std::string decimalstring = m_entity.substr(1, std::string::npos);
			l = string2long(decimalstring);
		}

		if (m_encoding == kCSUTF8) {
			std::string utf8str;
			long2utf8(l, utf8str);
			if (state == QUOTE) {
				m_rvalue += utf8str;
			} else {
				m_text += utf8str;
			}
		} else {
			if (l <= 255) {
				if (state == QUOTE) {
					APPEND_CHAR(m_rvalue, l);
				} else {
					APPEND_CHAR(m_text, l);
				}
			} else {
				except("Entity &" + m_entity + "; out of range; document is non-UTF8, and so we can handle only single-byte data.");
			}
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

int QDParser::handleEOL(int c)
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

int QDParser::getNextChar()
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


void QDParser::readIntoBuffer()
{
	if ((top - lim) < READ_SIZE) {
		throw QDException("Error: (top - lim) < READ_SIZE");
	}
	
	m_pInStream->read(lim, READ_SIZE);
	unsigned long long cnt = m_pInStream->gcount();
	if ((cnt != READ_SIZE) || m_pInStream->eof()) {
		eof = &lim[cnt]; *eof++ = '\0';
	}
	lim += cnt;
}

void QDParser::fillBuffer()
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
		} else if (cnt < 0) {
			// std::cerr << "UP200: lim = " << (void*) lim << ", cur = " << (void*) cur << ", cnt = " << cnt << std::endl;
			ASSERT_THROW_X(false, QDException, "QDParser::fillBuffer(): cnt < 0");
		} else {
			lim = bot;
			cur = bot;
		}
		
		readIntoBuffer();
	}
}

#undef READ_SIZE
#undef BUF_SIZE

QDParser::QDParser()
{
	bot = cur = lim = top = eof = 0;
}

QDParser::~QDParser()
{
	cleanUp();
}


