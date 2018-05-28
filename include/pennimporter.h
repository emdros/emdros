/*
 * pennimporter.h
 *
 * Classes to do Penn Treebank importing
 *
 * Ulrik Sandborg-Petersen
 * Created: 2/17-2006
 * Last update: 11/4-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2017  Ulrik Sandborg-Petersen
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

/**@file pennimporter.h
 *@brief Header file for Penn Treebank import (util library) 
 */


#ifndef PENNIMPORTER__H__
#define PENNIMPORTER__H__

#include <string>
#include <list>
#include <set>
#include <istream>

#include "monads.h"
#include "infos.h"
#include "string_list.h"
#include "penn_yylex.h"



class PennScanner {
protected:
	std::istream *pIn;
	PennToken *yylval;
	unsigned char *bot, *tok, *ptr, *cur, *pos, *lim, *top, *eof;
public:
	PennScanner(std::istream *pIn);
	virtual ~PennScanner();
	int scan(PennToken *yylval);
	bool noMoreInput() { return eof != 0 && cur == eof; };
protected:
	unsigned char *fill(unsigned char* cursor);
	void addToken(void);
private:
	PennScanner(const PennScanner&); // Copy constructor unimplemented
	PennScanner& operator=(const PennScanner&); // Assignment operator unimplemented
};



typedef std::map<std::string, std::string> strstrmap;

class SetOfMonads; // Forward declaration

class EmdrosMemObject {
	SetOfMonads m_monads;
	strstrmap m_features;
	id_d_t m_id_d;
public:
	EmdrosMemObject();
	EmdrosMemObject(const SetOfMonads& monads, id_d_t id_d = NIL);
	EmdrosMemObject(monad_m monad, id_d_t id_d = NIL);
	EmdrosMemObject(monad_m first_m, monad_m last_m, id_d_t id_d = NIL);
	~EmdrosMemObject() {};
	id_d_t getID_D(void) const { return m_id_d; };
	void addMonads(const SetOfMonads& monads); // Does a union-with on m_monads
	void setMonads(const SetOfMonads& monads); // Does an assignment to m_monads
	const SetOfMonads& getMonads(void) const;
	const std::string& getFeature(const std::string& feature_name) const; // Returns empty string if not there...
	void setFeature(const std::string& feature_name, const std::string& value);
	bool putMQL(std::ostream *pOut, const std::list<FeatureInfo>& feature_infos, const std::string& object_type_name = "") const;
	bool hasFeature(const std::string& feature_name) const;
};

typedef std::list<EmdrosMemObject> EMOList;


enum ePennTreeNodeType {
	kPennTerminal, kPennNonTerminal, kPennRoot
};

class PennTreeNode {
	ePennTreeNodeType m_type;
	std::string m_strNodeType;
	std::string m_strFunction;
	std::string m_strCoref;
	std::string m_strTerminal;
	PennTreeNode *m_pChild, *m_pNextSibling;
	SetOfMonads m_monads;
	id_d_t m_id_d;
	emdros_int64 m_docid;
	std::list<emdros_int64> m_corefs;
	emdros_int64 m_parent_docid;
 public:
	PennTreeNode(PennTreeNode *pChild){
		m_type = kPennRoot;
		m_pChild = pChild;
		m_pNextSibling = 0;
		m_docid = 0;
		m_parent_docid = NIL;
	};
	PennTreeNode(std::string *pNodeType, PennTreeNode *pChild){
		m_type = kPennNonTerminal;
		setNodeType(*pNodeType);
		delete pNodeType;
		m_pChild = pChild;
		m_pNextSibling = 0;
		m_docid = 0;
		m_parent_docid = 0;
	};
	PennTreeNode(std::string *pTerminalString) {
		m_type = kPennTerminal;
		setTerminal(*pTerminalString);
		delete pTerminalString;
		m_pChild = 0;
		m_pNextSibling = 0;
		m_docid = 0;
		m_parent_docid = 0;
	};
	~PennTreeNode() {
		delete m_pChild;
		delete m_pNextSibling;
	};
	void setNextSibling(PennTreeNode *pNode) { m_pNextSibling = pNode; };
	ePennTreeNodeType getType(void) const { return m_type; };
	const std::string& getTerminal(void) const { return m_strTerminal; };
	const std::string& getNodeType(void) const { return m_strNodeType; };
	const std::string& getStrCoref(void) const { return m_strCoref; };
	const std::string& getFunction(void) const { return m_strFunction; };
	SetOfMonads getMonads(void) const { return m_monads; };
	id_d_t getID_D(void) const { return m_id_d; };
	emdros_int64 getDocID(void) const { return m_docid; };
	emdros_int64 getParentDocID(void) const { return m_parent_docid; };
	PennTreeNode *getChild(void) const { return m_pChild; };
	PennTreeNode *getNextSibling(void) const { return m_pNextSibling; };
	std::list<emdros_int64>& getCorefs(void) { return m_corefs; };

	void setNodeType(const std::string& strIn);
	void setTerminal(const std::string& strIn);
	void addMonads(const SetOfMonads& monads) { m_monads.unionWith(monads); };
	void addMonad(monad_m m) { m_monads.add(m); };
	void setID_D(id_d_t id_d) { m_id_d = id_d; };
	void setDocID(emdros_int64 docid) { m_docid = docid; };
	void setParentDocID(emdros_int64 parent_docid) { m_parent_docid = parent_docid; };
	void pretty() const {
		std::cout << " typ=" << m_type<< "', nodetype='" << getNodeType()  << ", terminal='" << getTerminal() << "' id=" << m_id_d << std::endl; 
	};
};

typedef ParserListNode<PennTreeNode*> PennTreeNodeListNode;
typedef ParserList<PennTreeNode*> PennTreeNodeList;
typedef ParserListConstIterator<PennTreeNode*> PennTreeNodeListConstIterator;


class PennImporterEnv {
 public:
	PennImporterEnv() {
		pLexer = 0;
		pCorpus = 0;
		bSyntaxError = false;
	};
	~PennImporterEnv();  
	PennScanner *pLexer;
	bool bSyntaxError;
	std::string strErrorMessage;
	PennTreeNodeList *pCorpus;
	void clean();
};

typedef std::list<EmdrosMemObject*> PEMOList;

typedef std::map<emdros_int64, std::list<id_d_t> > CorefMap;


class EmdrosImporterBase {
 protected:
	monad_m m_cur_monad;
	id_d_t m_cur_id_d;
	std::ostream *m_pMQLOut;
	int m_nDocumentsToGatherBeforeEmitting;
 public:
	EmdrosImporterBase(monad_m start_monad, id_d_t start_id_d) {
		m_cur_monad = start_monad;
		m_cur_id_d = start_id_d;
		m_pMQLOut = 0;
		m_nDocumentsToGatherBeforeEmitting = 0;
	};
	EmdrosImporterBase(monad_m start_monad, id_d_t start_id_d, std::ostream *pMQLOut, int nDocumentsToGatherBeforeEmitting) {
		m_cur_monad = start_monad;
		m_cur_id_d = start_id_d;
		m_pMQLOut = pMQLOut;
		m_nDocumentsToGatherBeforeEmitting = nDocumentsToGatherBeforeEmitting;
		
	};
	virtual ~EmdrosImporterBase() { clearDocuments(); };
	virtual bool readDocumentFromStream(std::istream *pIn) { /* Override! */ UNUSED(pIn); return false; };
	virtual bool readDocumentsFromFiles(const std::list<std::string>& filename_list);
	virtual bool readDocumentFromFile(const std::string& filename);
	virtual void putSchema(std::ostream *pOut) { /* Override! */ UNUSED(pOut); };
	virtual void putCorpusAsMQL(std::ostream *pOut) { /* Override! */ UNUSED(pOut); };
	virtual monad_m getNextMonad(void) const { return m_cur_monad; };
	virtual id_d_t getNextID_D(void) const { return m_cur_id_d; };
	virtual void clearDocuments(void) { /* Override! */ };
};


class PennTreebankImporter : public EmdrosImporterBase {
 protected:
	PennImporterEnv *m_pEE;
	PennTreeNodeList *m_pCorpus;
	PEMOList m_terminals;
	PEMOList m_nonterminals;
	PEMOList m_root_trees;
	PEMOList m_docs;
	bool m_bEmitNonTerminalsAsDistinctObjectTypes;
	bool m_bUseIntegerDocIDs;
	emdros_int64 m_cur_docid;
	std::set<std::string> m_DroppedIndexOTNs;
 public:
	PennTreebankImporter(bool bEmitNonTerminalsAsDistinctObjectTypes, 
			     monad_m start_monad = 1, id_d_t start_id_d = 1,
			     std::ostream *pMQLOut = 0,
			     int nDocumentsToGatherBeforeEmitting = 100,
			     bool bUseIntegerDocIDs = false);
	virtual ~PennTreebankImporter();
	virtual bool readDocumentFromStream(std::istream *pIn);
	virtual void putSchema(std::ostream *pOut);
	virtual void putCorpusAsMQL(std::ostream *pOut);
 protected:
	void putNonterminals(std::ostream *pOut, const PEMOList& nonterminals, const std::string& objectTypeName);
	void createObjects(PennTreeNode *pNode);
	virtual void clearDocuments(void);
	void clearAST(void);
	void clearPEMOList(PEMOList& mylist);
	EmdrosMemObject *createTerminal(PennTreeNode *pNode, PennTreeNode *pParent);
	EmdrosMemObject *createNonTerminal(PennTreeNode *pNode);
	EmdrosMemObject *createRootTree(PennTreeNode *pNode);
	EmdrosMemObject *createDoc(monad_m first, monad_m last, id_d_t id_d, emdros_int64 doc_docid);
	SetOfMonads assignMonadsAndID_Ds(PennTreeNode *pNode, SetOfMonads& sibling_monads, id_d_t parent_id_d, emdros_int64 parent_docid);
	void resolveCorefs(PennTreeNode *pNode, CorefMap& map);
	
};


#endif /* PENNIMPORTER__H__ */
