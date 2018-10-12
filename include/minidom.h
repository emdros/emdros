/*
 * minidom.h
 *
 * DOM-like interface to XML
 *
 * Ulrik Petersen
 * Created: 5/2-2007
 * Last update: 9/8-2009
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


/**@file qdxml.h
 *@brief Header file for Quick'n Dirty XML SAX-like parser
 */

#ifndef MINIDOM__H__
#define MINIDOM__H__


#include "qdxml.h"


typedef enum {
	ELEMENT_NODE = 1,
	TEXT_NODE = 3,
	DOCUMENT_NODE = 9,
	COMMENT_NODE = 27
} eMiniDOMNodeType;

class MiniDOMNode;

typedef Llist<MiniDOMNode> MiniDOMNodeList;
typedef LlistIterator<MiniDOMNode> MiniDOMNodeListIterator;
typedef LlistConstIterator<MiniDOMNode> MiniDOMNodeListConstIterator;

class MiniDOMDocument;

class MiniDOMNode {
 protected:
	eMiniDOMNodeType m_nodeType;
	MiniDOMNode *m_parentNode;
	MiniDOMNodeList *m_childNodes;
	AttributeMap m_attributes;
	MiniDOMDocument *m_ownerDocument;
	friend class MiniDOMDocument;
	MiniDOMNode(eMiniDOMNodeType nodeType,
		    MiniDOMNode *pParent,
		    MiniDOMDocument *ownerDocument);
 public:
	virtual ~MiniDOMNode();
	MiniDOMNode *insertBefore(MiniDOMNode *newChild,
				  MiniDOMNode *refChild);
	MiniDOMNode *removeChild(MiniDOMNode *oldChild);
	MiniDOMNode *appendChild(MiniDOMNode *newChild);
	bool hasChildNodes() const;
	MiniDOMNode *cloneNode(bool deep);
  
	// Accessors
	bool getAttribute(const std::string& name, std::string& value) const;
	MiniDOMDocument *getOwnerDocument();
	eMiniDOMNodeType getNodeType() const;
	MiniDOMNode *getParentNode();
	MiniDOMNodeList *getChildren();
	virtual bool mayHaveChildren() const { return true; };
	virtual void pretty(std::ostream &ostr) const;
	MiniDOMNode *getSiblingBeforeThisChild(MiniDOMNode *pChild);

	// Mutators
	void setAttribute(const std::string& name, const std::string& value);
	void removeAttribute(const std::string& name);
	void setParent(MiniDOMNode *pNewParent);
};


class MiniDOMElement : public MiniDOMNode {
 protected:
	std::string m_tagName;
	friend class MiniDOMDocument;
	MiniDOMElement(const std::string& tagname, MiniDOMNode *pParent, MiniDOMDocument *ownerDocument);
 public:
	virtual ~MiniDOMElement();
	std::string getTagName() const;
	MiniDOMElement *getDescendantElementByName(const std::string& tagname);
	virtual void pretty(std::ostream &ostr) const;
};

class MiniDOMText : public MiniDOMNode {
 protected:
	std::string m_data;
	MiniDOMText(const std::string& data, MiniDOMNode *pParent, MiniDOMDocument *ownerDocument);
	friend class MiniDOMDocument;
 public:
	virtual ~MiniDOMText();
	std::string getData() const;
	void setData(const std::string& data);
	virtual bool mayHaveChildren() const { return false; };
	virtual void pretty(std::ostream &ostr) const;
};

class MiniDOMComment : public MiniDOMText {
 public:
	MiniDOMComment(const std::string& data, MiniDOMNode *pParent, MiniDOMDocument *ownerDocument);
	virtual ~MiniDOMComment();

	virtual void pretty(std::ostream &ostr) const;
};

class MiniDOMDocument : public MiniDOMNode {
 public:
	MiniDOMDocument();
	virtual ~MiniDOMDocument();

	virtual MiniDOMElement *getDocumentElement();
	virtual void pretty(std::ostream &ostr) const;

	// Factories
	virtual MiniDOMElement *createElement(const std::string& tagName);
	virtual MiniDOMText *createText(const std::string& data);
	virtual MiniDOMComment *createComment(const std::string& data);
};

extern MiniDOMDocument *MiniDOMParseStream(std::istream *pIn, MiniDOMDocument *pDoc = 0);
extern MiniDOMDocument *MiniDOMParseString(const std::string& str, MiniDOMDocument *pDoc = 0);
#endif
