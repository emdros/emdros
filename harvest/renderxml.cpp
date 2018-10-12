/*
 * renderxml.cpp
 *
 * Classes and functions to render XML into something else,
 * based on a JSON template.
 *
 * Ulrik Sandborg-Petersen 
 * Created: 4/22-2007
 * Last update: 10/12-2017
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




#include <renderxml.h>
#include <templatelang_classes.h>

#include <fstream>
#include <sstream>
#include <list>

#include "string_func.h"
#include "qdxml.h"
#include "arena.h"





/*
 * The UTF8-code was adapted from the code found in utf.c from SQLite 3.
 * The original code was in the Public Domain.
 *
 * Adapted by Ulrik Sandborg-Petersen.
 *
 */

/*
** This table maps from the first byte of a UTF-8 character to the number
** of trailing bytes expected. A value '4' indicates that the table key
** is not a legal first byte for a UTF-8 character.
*/
static const unsigned char extra_utf8_bytes[256]  = {
/* 0xxxxxxx */
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,

/* 10wwwwww */
4, 4, 4, 4, 4, 4, 4, 4,     4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4,     4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4,     4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4, 4, 4, 4, 4,     4, 4, 4, 4, 4, 4, 4, 4,

/* 110yyyyy */
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,

/* 1110zzzz */
2, 2, 2, 2, 2, 2, 2, 2,     2, 2, 2, 2, 2, 2, 2, 2,

/* 11110yyy */
3, 3, 3, 3, 3, 3, 3, 3,     4, 4, 4, 4, 4, 4, 4, 4,
};

/*
** This table maps from the number of trailing bytes in a UTF-8 character
** to an integer constant that is effectively calculated for each character
** read by a naive implementation of a UTF-8 character reader. The code
** in the READ_UTF8 macro explains things best.
*/
static const unsigned int extra_utf8_bits[] =  {
  0,
  12416,          /* (0xC0 << 6) + (0x80) */
  925824,         /* (0xE0 << 12) + (0x80 << 6) + (0x80) */
  63447168        /* (0xF0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
};

/*
** If a UTF-8 character contains N bytes extra bytes (N bytes follow
** the initial byte so that the total character length is N+1) then
** masking the character with utf8_mask[N] must produce a non-zero
** result.  Otherwise, we have an (illegal) overlong encoding.
*/
static const unsigned int utf_mask[] = {
  0x00000000,
  0xffffff80,
  0xfffff800,
  0xffff0000,
};

#define READ_UTF8(zIn, c) { \
  int extra;                                           \
  c = (unsigned int) *(zIn)++;			       \
  extra = extra_utf8_bytes[c];                         \
  switch( extra ){                                     \
    case 4: c = (int)0xFFFD; break;                    \
    case 3: c = (c<<6) + *(zIn)++;  /* fall through */ \
    case 2: c = (c<<6) + *(zIn)++;  /* fall through */ \
    case 1: c = (c<<6) + *(zIn)++;                     \
    c -= extra_utf8_bits[extra];                       \
    if( (utf_mask[extra]&c)==0                         \
        || (c&0xFFFFF800)==0xD800                      \
        || (c&0xFFFFFFFE)==0xFFFE ){  c = 0xFFFD; }    \
  }                                                    \
}

std::string utf8ToEntity(const std::string& inHTML)
{
	std::ostringstream ostr;
	const unsigned char *zIn = (const unsigned char*) inHTML.c_str();
	while (*zIn != '\0') {
		int c = 0;
		READ_UTF8(zIn, c);
		if (c < 0x80) {
			ostr << (char) c;
		} else {
			ostr << "&#" << c << ";";
		}
	}
	return ostr.str();
}

class ElementInfo {
private:
	std::string m_tag;
	TemplateASTNode *m_pStartTemplate;
	TemplateASTNode *m_pEndTemplate;
	bool m_bNix;
public:
	ElementInfo();
	~ElementInfo();
	bool fill(TemplateLangExecEnv *pEE, const std::string& tag, const JSONValue* pStart, const JSONValue *pEnd, bool bNix, std::string& /* out */ strErrorOutput);
	TemplateASTNode *getStartTemplate(void) { return m_pStartTemplate; };
	TemplateASTNode *getEndTemplate(void) { return m_pEndTemplate; };
	bool getBNix(void) const { return m_bNix; };
	const std::string& getTag(void) const { return m_tag; };
};

ElementInfo::ElementInfo()
{
	m_tag = "";
	m_pStartTemplate = 0;
	m_pEndTemplate = 0;
}

ElementInfo::~ElementInfo()
{
	delete m_pStartTemplate;
	delete m_pEndTemplate;
}

bool ElementInfo::fill(TemplateLangExecEnv *pEE, const std::string& tag, const JSONValue* pStart, const JSONValue *pEnd, bool bNix, std::string& /* out */ strErrorOutput)
{
	m_bNix = bNix;
	m_tag = tag;

	if (pStart != 0) {
		if (pStart->getKind() != kJSONString) {
			strErrorOutput += "ElementInfo::fill(): for tag '" + tag + "', start is not a JSON String.";
			return false;
		}
		    
		bool bCompileResult;
		std::string start = pStart->getString();
		if (!start.empty()) {
			TemplateASTNode *pNode = parseTemplate(pEE, start, bCompileResult);
			if (!bCompileResult) {
				strErrorOutput += "ElementInfo::fill(): parsing template 'start' for tag '" + tag + "' failed with the following error:\n" + pEE->getError() + '\n';
				delete pNode;
				return false;
			} else {
				m_pStartTemplate = pNode;
			}
		}
	}

	if (pEnd != 0) {
		if (pEnd->getKind() != kJSONString) {
			strErrorOutput += "ElementInfo::fill(): for tag '" + tag + "', end is not a JSON String.";
			return false;
		}

		if (!pEnd->getString().empty()) {
			bool bCompileResult;
			TemplateASTNode *pNode = parseTemplate(pEE, pEnd->getString(), bCompileResult);
			if (!bCompileResult) {
				strErrorOutput += "ElementInfo::fill(): parsing template 'end' for tag '" + tag + "' failed with the following error:\n" + pEE->getError() + '\n';
				delete pNode;
				return false;
			} else {
				m_pEndTemplate = pNode;
			}
		}
	}
	return true;
}

class XML2HTMLDocHandler : public QDDocHandler {
protected:
	Bigstring m_chars;
	typedef std::list<std::string> ListOfHTMLString;
	typedef std::list<std::string> ListOfXMLTag;
	ListOfXMLTag m_nixing_stack;
	TemplateLangExecEnv *m_pTemplateEE;
	typedef std::map<std::string, ElementInfo*> String2ElementInfoMap;
	String2ElementInfoMap m_elements;
        bool parseStyleSheet(const JSONValue *pStyleSheet, const std::string& render_stylesheet, std::string& strError);
 public:
	Bigstring m_html;
        XML2HTMLDocHandler(const JSONValue *pRenderInfo, const std::string& render_stylesheet, const TemplateLangVariableMap& variables, bool& /* out */ bCompilationOK, std::string& /* out */ strError);
        virtual ~XML2HTMLDocHandler() {
        };
        virtual void startElement(const std::string& tag, const AttributeMap& attrs);
        virtual void endElement(const std::string& tag);
        virtual void startDocument(void);
        virtual void endDocument(void);
        virtual void text(const std::string& str);
	std::string getOutput(void);
	virtual bool isNixing() const { return !m_nixing_stack.empty() || m_pTemplateEE->getNixing() != kTNTOff; }
protected:
	void addText(const std::string& str, bool bSpaceAfter = false);
	void emit(const std::string& str);
	void emit(char c);
	void emit(Bigstring *pBigstring);
	void handleChars();
};

XML2HTMLDocHandler::XML2HTMLDocHandler(const JSONValue *pRenderInfo, const std::string& render_stylesheet, const TemplateLangVariableMap& variables, bool&  /* out */ bCompilationOK, std::string& /* out */ strError)
{
	m_pTemplateEE = new TemplateLangExecEnv();
	m_pTemplateEE->setTopStylesheet(pRenderInfo);

	bCompilationOK = parseStyleSheet(pRenderInfo, render_stylesheet, strError);
	if (bCompilationOK) {
		m_pTemplateEE->setVariables(variables);
	}
}


bool XML2HTMLDocHandler::parseStyleSheet(const JSONValue *pRenderInfo, const std::string& render_stylesheet, std::string& /* out */ strError)
{
	bool bResult = true;
	if (pRenderInfo->getKind() != kJSONObject) {
		strError += "XML2HTMLDocHandler::parseStyleSheet: pRenderInfo is not a JSON Object.";
		return false;
	}
		     
	if (!pRenderInfo->hasObjectKey(render_stylesheet)) {
		strError += "XML2HTMLDocHandler::parseStyleSheet: pRenderInfo does not have '" + render_stylesheet + "' key.";
		return false;
	}

	const JSONValue *pStyleSheet = (*pRenderInfo)[render_stylesheet];

	if (pStyleSheet->getKind() != kJSONObject) {
		strError += "XML2HTMLDocHandler::parseStyleSheet: pStyleSheet is not a JSON Object.";
		return false;
	}
	if (!pStyleSheet->hasObjectKey("elements")) {
		strError += "XML2HTMLDocHandler::parseStyleSheet: pStyleSheet does not have 'elements' key.";
		return false;
	}
	const JSONValue *pElements = (*pStyleSheet)["elements"];
	
	if (pElements->getKind() != kJSONObject) {
		strError += "XML2HTMLDocHandler::parseStyleSheet: 'elements' value is not a JSON Object.";
		return false;
	}

	if (!pStyleSheet->hasObjectKey("extends")) {
		; // Nothing to do
	} else {
		const JSONValue *pExtends = pStyleSheet->getObjectValue("extends");
		if (pExtends->getKind() == kJSONString) {
			std::string extends = pExtends->getString();
			bResult = parseStyleSheet(pRenderInfo, extends, strError);
		} else if (pExtends->getKind() == kJSONNull) {
			; // That's OK, it can be 'null'.
		} else {
			strError += "XML2HTMLDocHandler::parseStyleSheet: 'renderinfo'['" + render_stylesheet + "']['extends'] does not map to a JSON string.";
			bResult = false;
		}
	}

	std::list<std::string> elements_keys = pElements->getObjectKeys();
	std::list<std::string>::const_iterator elemkeyci = 
		elements_keys.begin();
	while (elemkeyci != elements_keys.end()) {
		std::string elemkey = *elemkeyci;

		const JSONValue *pValue = (*pElements)[elemkey];

		String2ElementInfoMap::iterator it = m_elements.find(elemkey);
		if (it != m_elements.end()) {
			delete it->second;
			m_elements.erase(it);
		}

		if (pValue->getKind() != kJSONObject) {
			strError += "XML2HTMLDocHandler::parseStyleSheet: for elements member '"
				+ elemkey + "', value is not a JSON Object.";
			bResult = false;
		} else {
			const JSONValue *pStartValue = 0;
			const JSONValue *pEndValue = 0;
			
			if (pValue->hasObjectKey("start")) {
				pStartValue = (*pValue)["start"];
			}
			
			if (pValue->hasObjectKey("end")) {
				pEndValue = (*pValue)["end"];
			}

			bool bNix = false;
			if (pValue->hasObjectKey("nix")) {
				bNix = (*pValue)["nix"]->getBoolean();
			}
			
			ElementInfo *pElementInfo = new ElementInfo();
			
			if (!pElementInfo->fill(m_pTemplateEE,
						elemkey,
						pStartValue,
						pEndValue,
						bNix,
						strError)) {
				
				bResult = false;
				delete pElementInfo;
			} else {
				m_elements.insert(std::make_pair(elemkey, pElementInfo));
			}
			
			++elemkeyci;
		}
	}

	return bResult;
}

void XML2HTMLDocHandler::emit(char c)
{
	if (!isNixing()) {
		m_html.addChar(c);
	}
}


void XML2HTMLDocHandler::emit(Bigstring *pBigstring)
{
	if (!isNixing()) {
		m_html.addBigstring(pBigstring);
	}
}


void XML2HTMLDocHandler::emit(const std::string& str)
{
	if (!isNixing()) {
		m_html.addChars(str.data(), str.length());
	}
}



void XML2HTMLDocHandler::startElement(const std::string& tag, const AttributeMap& attrs)
{
	handleChars();

	String2ElementInfoMap::iterator it = m_elements.find(tag);
	if (it != m_elements.end()) {
		ElementInfo *pElementInfo = it->second;
		bool bNix = pElementInfo->getBNix();
		if (bNix) {
			m_nixing_stack.push_back(tag);
		} else {
			TemplateASTNode *pTemplate = pElementInfo->getStartTemplate();
			if (pTemplate != 0) {
				m_pTemplateEE->setAttributes(attrs);
				pTemplate->exec(m_pTemplateEE);
				emit(m_pTemplateEE->getOutputBigstring());	
				m_pTemplateEE->clearOutput();
			}
		}
	} else {
		throw QDException("Error: Unknown start-tag '<" + tag + ">'");
	}
}


void XML2HTMLDocHandler::endElement(const std::string& tag)
{
	handleChars();

	String2ElementInfoMap::iterator it = m_elements.find(tag);
	if (it != m_elements.end()) {
		ElementInfo *pElementInfo = it->second;
		bool bNix = pElementInfo->getBNix();
		if (bNix) {
			std::string nixing_top = *m_nixing_stack.rbegin();
			m_nixing_stack.pop_back();
			if (nixing_top != tag) {
				throw QDException("Nixing stack pop didn't match push");
			}
		} else {
			TemplateASTNode *pTemplate = pElementInfo->getEndTemplate();
			if (pTemplate != 0) {
				pTemplate->exec(m_pTemplateEE);
				emit(m_pTemplateEE->getOutputBigstring());	
				m_pTemplateEE->clearOutput();
			}
		}
	} else {
		throw QDException("Error: Unknown end-tag '</" + tag + ">'");
	}

}


void XML2HTMLDocHandler::startDocument(void)
{
}


void XML2HTMLDocHandler::endDocument(void)
{
}


void XML2HTMLDocHandler::text(const std::string& str)
{
	m_chars.addChars(str.data(), str.length());
}

void XML2HTMLDocHandler::addText(const std::string& str, bool bSpaceAfter)
{
	if (!isNixing()) {
		std::string mystr(escapeXMLEntities(str));

		emit(mystr);

		if (bSpaceAfter) {
			m_html.addChar(' ');
		}
	}
}



void XML2HTMLDocHandler::handleChars()
{
	if (m_chars.empty()) {

	} else {
		std::string chars(m_chars.toString());
		
		// Clean up for next text() call.
		m_chars.clear();
		
		if (chars.empty()) {
			return; // Nothing to do
		} else if (isNixing()) {
			return;
		} else {
			if (hasXMLCharsToMangle(chars)) {
				emit(escapeXMLEntities(chars));
			} else {
				emit(chars);
			}
		}
	}
	/*
	  } else if (!is_other_than_whitespace(chars)) {
	  //		if (m_bInParagraph) {
	  m_html.addChar(' ');
	  return;
	  } else {
	  if (is_whitespace(chars[0])) {
	  m_html.addChar(' ');
	  }
	  std::list<std::string> token_list;
	  split_string(chars, " \n\t\r", token_list);
	  std::list<std::string>::const_iterator ci = token_list.begin();
	  std::list<std::string>::const_iterator cend = token_list.end();
	  while (ci != cend) {
	  std::string text = *ci;
	  ++ci;
	  bool bAddSpaceAfter = true;
	  if (ci == cend) {
	  if (is_whitespace(chars[chars.length()-1])) {
	  ; // Leave it as true
	  } else {
	  bAddSpaceAfter = false;
	  }
	  }
	  addText(text, bAddSpaceAfter);
	  }
	  }
	*/
}



std::string XML2HTMLDocHandler::getOutput(void)
{
	return m_html.toString();
}


std::string RenderXML(const std::string& xml, const JSONValue *pTopStyleSheet, const std::string& render_stylesheet, const TemplateLangVariableMap& variables, bool& bProcessOK, std::string& strError)
{
	bProcessOK = true;

	ASSERT_THROW(pTopStyleSheet->getKind() == kJSONObject,
		     "RenderXML: Top stylesheet is not a JSON Object");
	ASSERT_THROW(pTopStyleSheet->hasObjectKey("renderinfo"),
		     "RenderXML: Top stylesheet does not have 'renderinfo' member.");
	const JSONValue *pRenderInfo = (*pTopStyleSheet)["renderinfo"];

	ASSERT_THROW(pRenderInfo->getKind() == kJSONObject,
		     "RenderXML: renderinfo stylesheet is not a JSON OBject");
	ASSERT_THROW(pRenderInfo->hasObjectKey(render_stylesheet),
		     "RenderXML: renderinfo stylesheet does not have '" + render_stylesheet + "' member.");

	const JSONValue *pStyleSheet = (*pRenderInfo)[render_stylesheet];

	std::string newxml = xml;
	if (pStyleSheet->hasObjectKey("preprocess")) {
		const JSONValue *pPreprocessValue = (*pStyleSheet)["preprocess"];
		if (pPreprocessValue->getKind() == kJSONList) {
			const std::list<JSONValue*>& preprocess_list 
				= pPreprocessValue->getList();
			std::list<JSONValue*>::const_iterator ci;
			ci = preprocess_list.begin();
			while (ci != preprocess_list.end()) {
				const JSONValue *pPreprocessObject
					= *ci;
				if (pPreprocessObject->getKind() == kJSONObject) {
					if (pPreprocessObject->hasObjectKey("instring")
					    && pPreprocessObject->hasObjectKey("outstring")) {
						std::string instring = (*pPreprocessObject)["instring"]->getString();
						std::string outstring = (*pPreprocessObject)["outstring"]->getString();
						newxml = replace_substring(newxml, instring, outstring);

		 			} else if (pPreprocessObject->hasObjectKey("prefix")) {
						std::string prefix = (*pPreprocessObject)["prefix"]->getString();
						newxml = replace_substring(newxml, "?>", "?>" + prefix);

					} else if (pPreprocessObject->hasObjectKey("suffix")) {
						std::string suffix = (*pPreprocessObject)["suffix"]->getString();
						newxml = newxml + suffix;

					} else {
						// std::cerr << "UP271: JSON object for 'preprocess' renderinfo stylesheet member\ndoes not have both 'instring' and 'outstring'." << std::endl;
					}
				} else {
					// std::cerr << "UP270: JSON value for 'preprocess' renderinfo stylesheet member\nis not a JSON Object or a JSON String." << std::endl;

				}
				++ci;
			}
		}
	}
	std::istringstream istr(newxml);


        XML2HTMLDocHandler dh(pRenderInfo, render_stylesheet, variables, bProcessOK, strError);
	if (!bProcessOK) {
		return "";
	}

	std::string output_string;
	try {
		QDParser parser;
		parser.parse(&dh, &istr);
		output_string = dh.getOutput();
	} catch (QDException& e) {
		strError += e.what() + "\n";
		bProcessOK = false;
	}

	
	return output_string;
}

std::string RenderXML(const std::string& xml, const JSONValue *pTopStyleSheet, const std::string& render_stylesheet, bool& bProcessOK, std::string& strError)
{
	TemplateLangVariableMap empty_vars;
	return RenderXML(xml, pTopStyleSheet, render_stylesheet, empty_vars, bProcessOK, strError);
}
