/*
 * minidom.cpp
 *
 * DOM-like interface to XML
 *
 * Ulrik Petersen
 * Created: 5/2-2007
 * Last update: 3/2-2017
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


/**@file qdxml.cpp
 *@brief Implementation file for DOM-like parser
 */

#include <minidom.h>
#include <string_func.h>


////////////////////////////////////////////////////////////////
//
// MiniDOMDocHandler
//
////////////////////////////////////////////////////////////////

typedef std::list<std::string> ListOfString;
class MiniDOMDocHandler : public QDDocHandler {
protected:
	ListOfString m_charstack;
	MiniDOMDocument *m_pDoc;
	MiniDOMElement *m_pParent;
 public:
	MiniDOMDocHandler(MiniDOMDocument *pDoc = 0);
	virtual ~MiniDOMDocHandler();
	virtual void startElement(const std::string& tag, const AttributeMap& attrs);
	virtual void endElement(const std::string& tag);
	virtual void startDocument(void);
	virtual void endDocument(void);
	virtual void text(const std::string& str);
	virtual void comment(const std::string& str);

	MiniDOMDocument *takeOverDocument();
protected:
	void eraseCharStack(void);
	void handleChars();
};


MiniDOMDocHandler::MiniDOMDocHandler(MiniDOMDocument *pDoc)
{
	if (pDoc == 0) {
		m_pDoc = new MiniDOMDocument();
	} else {
		m_pDoc = pDoc;
	}
	m_pParent = 0;
}


MiniDOMDocHandler::~MiniDOMDocHandler()
{
	eraseCharStack();
	delete m_pDoc;
}


void MiniDOMDocHandler::startElement(const std::string& tag, const AttributeMap& attrs)
{
	handleChars();

	MiniDOMElement *pElem = m_pDoc->createElement(tag);

	// Copy attributes
	AttributeMap::const_iterator ci = attrs.begin();
	while (ci != attrs.end()) {
		std::string key = ci->first;
		std::string value = ci->second;
		pElem->setAttribute(key, value);
		ci++;
	}
	
	if (m_pParent != 0) {
		pElem->setParent(m_pParent);
		m_pParent->appendChild(pElem);
	}
	m_pParent = pElem;
}


void MiniDOMDocHandler::endElement(const std::string& tag)
{
	handleChars();
	if (tag != m_pParent->getTagName()) {
		throw QDException(std::string("MiniDOMDocHandler::endElement: parent tag name '") + m_pParent->getTagName() + "' is not the current tag: '" + tag + "'. XML malformed.");
	}
	m_pParent = (MiniDOMElement*) m_pParent->getParentNode();
}

void MiniDOMDocHandler::handleChars()
{
	if (m_charstack.empty()) {
		return;
	}
	std::string chars = joinListEmptyBetween(m_charstack);
	eraseCharStack();
	if (!chars.empty()) {
		// Only do it if we have met the first element!
		if (m_pParent != 0) {
			MiniDOMText *pText = m_pDoc->createText(chars);
			pText->setParent(m_pParent);
			m_pParent->appendChild(pText);
		} 		
	}
}


void MiniDOMDocHandler::startDocument(void)
{
	// TODO: Implement
}


void MiniDOMDocHandler::endDocument(void)
{
	// TODO: Implement
}



void MiniDOMDocHandler::text(const std::string& str)
{
	m_charstack.push_back(str);
}

void MiniDOMDocHandler::comment(const std::string& str)
{
	MiniDOMComment *pComm = m_pDoc->createComment(str);
	
	if (m_pParent != 0) {
		pComm->setParent(m_pParent);
		m_pParent->appendChild(pComm);
	} else {
		// We are between the XML declaration and
		// the first element.
		delete pComm;
	}

	// A comment cannot have children, so we don't set 
	// m_pParent = pComm !!!
}

MiniDOMDocument *MiniDOMDocHandler::takeOverDocument()
{
	MiniDOMDocument *pDoc = m_pDoc;
	m_pDoc = 0;
	m_pParent = 0;
	return pDoc;
}

void MiniDOMDocHandler::eraseCharStack(void)
{
	m_charstack.erase(m_charstack.begin(), m_charstack.end());
}


////////////////////////////////////////////////////////////////
//
// MiniDOMNode
//
////////////////////////////////////////////////////////////////

MiniDOMNode::MiniDOMNode(eMiniDOMNodeType nodeType,
			 MiniDOMNode *pParent,
			 MiniDOMDocument *ownerDocument)
	: m_nodeType(nodeType),
	  m_parentNode(pParent),
	  m_ownerDocument(ownerDocument)
{
	m_childNodes = new MiniDOMNodeList();
}

MiniDOMNode::~MiniDOMNode()
{
	m_childNodes->clear(true);
	delete m_childNodes;
}

MiniDOMNode *MiniDOMNode::insertBefore(MiniDOMNode *newChild,
				       MiniDOMNode *refChild)
{
	if (refChild == 0) {
		m_childNodes->push_back(newChild);
		return newChild;
	} else {
		MiniDOMNodeListIterator it = m_childNodes->iterator();
		while (it.hasNext()) {
			MiniDOMNode *pCurNode = it.current();
			if (pCurNode == refChild) {
				m_childNodes->insertBefore(it, newChild);
				return newChild;
			}
			it.next();
		}
		throw QDException("MiniDOMNode::insertAfter: refChild not found");
	}
}


MiniDOMNode *MiniDOMNode::removeChild(MiniDOMNode *oldChild)
{
	MiniDOMNodeListIterator it = m_childNodes->iterator();
	while (it.hasNext()) {
		MiniDOMNode *pCurNode = it.current();
		if (pCurNode == oldChild) {
			m_childNodes->erase(it);
			return oldChild;
		}
		it.next();
	}
	throw QDException("MiniDOMNode::removeChild: oldChild not found");
}


MiniDOMNode *MiniDOMNode::appendChild(MiniDOMNode *newChild)
{
	m_childNodes->push_back(newChild);
	return newChild;
}


bool MiniDOMNode::hasChildNodes() const
{
	return !m_childNodes->empty();
}

MiniDOMNode *MiniDOMNode::getSiblingBeforeThisChild(MiniDOMNode *pChild)
{
	if (!hasChildNodes()) {
		return 0;
	}
	MiniDOMNodeListIterator it = m_childNodes->iterator();
	MiniDOMNode *pSiblingBefore = 0;
	while (it.hasNext()) {
		MiniDOMNode *pCurNode = it.current();
		if (pCurNode == pChild) {
			return pSiblingBefore;
		}
		pSiblingBefore = pCurNode;
		it.next();
	}
	return 0;
}

void MiniDOMNode::pretty(std::ostream &ostr) const
{
	MiniDOMNodeListConstIterator ci = m_childNodes->const_iterator();
	while (ci.hasNext()) {
		const MiniDOMNode *pNode = ci.next();
		pNode->pretty(ostr);
	}
}



MiniDOMNode *MiniDOMNode::cloneNode(bool deep)
{
	MiniDOMNode *pNewNode = 0;
	if (m_nodeType == ELEMENT_NODE) {
		MiniDOMElement *pElement = (MiniDOMElement*) this;
		pNewNode = m_ownerDocument->createElement(pElement->getTagName());
	} else if (m_nodeType == TEXT_NODE) {
		MiniDOMText *pText = (MiniDOMText*) this;
		pNewNode = m_ownerDocument->createText(pText->getData());
	} else if (m_nodeType == COMMENT_NODE) {
		MiniDOMComment *pComment = (MiniDOMComment*) this;
		pNewNode = m_ownerDocument->createComment(pComment->getData());
	} else if (m_nodeType == DOCUMENT_NODE) {
		throw QDException("MiniDOMNode::cloneNode: Cannot clone a document");
	} else {
		throw QDException("MiniDOMNode::cloneNode: Unknown node type");
	}
	if (deep) {
		MiniDOMNodeListIterator it = m_childNodes->iterator();
		while (it.hasNext()) {
			MiniDOMNode *pChildNode = it.next();
			MiniDOMNode *pNewChildNode = pChildNode->cloneNode(true);
			pNewChildNode->setParent(pNewNode);
			pNewNode->appendChild(pChildNode);
		}
	}
	return pNewNode;
}



// Accessors
bool MiniDOMNode::getAttribute(const std::string& name, std::string& value) const
{
	if (m_attributes.find(name) == m_attributes.end()) {
		return false;
	} else {
		value = ::getAttribute(m_attributes, name);
		return true;
	}
}


MiniDOMDocument *MiniDOMNode::getOwnerDocument()
{
	return m_ownerDocument;
}


eMiniDOMNodeType MiniDOMNode::getNodeType() const
{
	return m_nodeType;
}


MiniDOMNode *MiniDOMNode::getParentNode()
{
	return m_parentNode;
}


MiniDOMNodeList *MiniDOMNode::getChildren()
{
	return m_childNodes;
}



// Mutators
void MiniDOMNode::setAttribute(const std::string& name, const std::string& value)
{
	m_attributes[name] = value;
}


void MiniDOMNode::removeAttribute(const std::string& name)
{
	if (m_attributes.find(name) != m_attributes.end()) {
		m_attributes.erase(m_attributes.find(name), ++m_attributes.find(name));
	}
}

void MiniDOMNode::setParent(MiniDOMNode *pNewParent)
{
	m_parentNode = pNewParent;
}



////////////////////////////////////////////////////////////////
//
// MiniDOMElement
//
////////////////////////////////////////////////////////////////

MiniDOMElement::MiniDOMElement(const std::string& tagname, MiniDOMNode *pParent, MiniDOMDocument *ownerDocument)
	: MiniDOMNode(ELEMENT_NODE,
		      pParent,
		      ownerDocument),
	  m_tagName(tagname)
{
}


MiniDOMElement::~MiniDOMElement()
{
}


std::string MiniDOMElement::getTagName() const
{
	return m_tagName;
}


void MiniDOMElement::pretty(std::ostream &ostr) const
{
	ostr << "<" << m_tagName;
	// Copy attributes
	AttributeMap::const_iterator ci = m_attributes.begin();
	while (ci != m_attributes.end()) {
		std::string key = ci->first;
		std::string value = ci->second;
		ostr << " " << key << "=\"";
		if (hasXMLCharsToMangle(value)) {
			ostr << escapeXMLEntities(value);
		} else {
			ostr << value;
		}
		ostr << "\"";
		ci++;
	}

	if (hasChildNodes()) {
		ostr << ">";

		// Print children
		MiniDOMNode::pretty(ostr);

		ostr << "</" << m_tagName << ">";
	} else {
		ostr << "/>";
	}
}


MiniDOMElement *MiniDOMElement::getDescendantElementByName(const std::string& tagname)
{
	if (m_tagName == tagname) {
		return this;
	} else if (!this->hasChildNodes()) {
		return 0;
	} else {
		MiniDOMNodeListIterator it = m_childNodes->iterator();
		MiniDOMElement *pResult = 0;
		while (it.hasNext()) {
			MiniDOMNode *pChild = it.next();
			if (pChild->getNodeType() == ELEMENT_NODE) {
				pResult = ((MiniDOMElement*) pChild)->getDescendantElementByName(tagname);
				if (pResult != 0) {
					return pResult;
				}
			} 
		}
		return 0; // IF we get this far, it means that we didn't find any in the depth-first walk of the children.
	}
}


////////////////////////////////////////////////////////////////
//
// MiniDOMText
//
////////////////////////////////////////////////////////////////
MiniDOMText::MiniDOMText(const std::string& data, MiniDOMNode *pParent, MiniDOMDocument *ownerDocument)
	: MiniDOMNode(TEXT_NODE,
		      pParent,
		      ownerDocument),
	  m_data(data)
{
}


MiniDOMText::~MiniDOMText()
{
}


std::string MiniDOMText::getData() const
{
	return m_data;
}


void MiniDOMText::setData(const std::string& data)
{
	m_data = data;
}

void MiniDOMText::pretty(std::ostream &ostr) const
{
	if (hasXMLCharsToMangle(m_data)) {
		ostr << escapeXMLEntities(m_data);
	} else {
		ostr << m_data;
	}
}


////////////////////////////////////////////////////////////////
//
// MiniDOMComment
//
////////////////////////////////////////////////////////////////

MiniDOMComment::MiniDOMComment(const std::string& data, MiniDOMNode *pParent, MiniDOMDocument *ownerDocument)
	: MiniDOMText(data, pParent, ownerDocument)
{
	m_nodeType = COMMENT_NODE;
}

MiniDOMComment::~MiniDOMComment()
{
}


void MiniDOMComment::pretty(std::ostream &ostr) const
{
	ostr << "<!--" << m_data;
}





////////////////////////////////////////////////////////////////
//
// MiniDOMDocument
//
////////////////////////////////////////////////////////////////
MiniDOMDocument::MiniDOMDocument()
	: MiniDOMNode(DOCUMENT_NODE,
		      0,
		      0)
{
}


MiniDOMDocument::~MiniDOMDocument()
{
}



MiniDOMElement *MiniDOMDocument::createElement(const std::string& tagName)
{
	MiniDOMElement *pNewNode = new MiniDOMElement(tagName, 0, this);
	if (m_childNodes->empty()) {
		appendChild(pNewNode);
	}
	return pNewNode;
}


MiniDOMText *MiniDOMDocument::createText(const std::string& data)
{
	MiniDOMText *pNewNode = new MiniDOMText(data, 0, this);
	return pNewNode;
}

MiniDOMComment *MiniDOMDocument::createComment(const std::string& data)
{
	MiniDOMComment *pNewNode = new MiniDOMComment(data, 0, this);
	return pNewNode;
}

MiniDOMElement *MiniDOMDocument::getDocumentElement()
{
	if (m_childNodes->empty()) {
		throw QDException("MiniDOMDocument::getDocumentElement: Document is empty.");
	} else {
		return (MiniDOMElement *) m_childNodes->getRoot()->getContent();
	}
}

void MiniDOMDocument::pretty(std::ostream &ostr) const
{
	ostr << "<?xml version='1.0' ?>";
	if (m_childNodes->empty()) {
		; // Do nothing.
	} else {
		m_childNodes->getRoot()->getContent()->pretty(ostr);;
	}
}


//////////////////////////////////////////////////////////////////////
//
// Parse functions
//
//////////////////////////////////////////////////////////////////////

MiniDOMDocument *MiniDOMParseStream(std::istream *pIn, MiniDOMDocument *pDoc)
{
	QDParser parser;
	MiniDOMDocHandler handler(pDoc);

	parser.parse(&handler, pIn);
	
	MiniDOMDocument *pResultDoc = handler.takeOverDocument();
	return pResultDoc;
}


MiniDOMDocument *MiniDOMParseString(const std::string& str, MiniDOMDocument *pDoc)
{
	std::istringstream istr(str);
	return MiniDOMParseStream(&istr, pDoc);
}

