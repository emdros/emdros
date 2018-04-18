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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2009  Ulrik Sandborg-Petersen
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
