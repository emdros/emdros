/*
 * pennimporter.cpp
 *
 * Functions and classes for importing Penn Treebank-style data
 * Created: 2/18-2006
 * Last update: 7/11-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2013  Ulrik Sandborg-Petersen
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

/**@file pennimporter.cpp
 *@brief Implementation of a Penn Treebank importer.
 */

#include "pennparser.h"
#include <penn_yylex.h>
#include <pennimporter.h>
#include "pennschema.h"
#include <fstream>

// Prototypes
extern void *PennParserAlloc(void *(*mallocProc)(size_t));
extern void PennParserFree(void *p, void (*freeProc)(void*));
extern void PennParser(
		       void *yyp,                       /* The parser */
		       int yymajor,                     /* The major token code number */
		       PennToken *yyminor,              /* The value for the token */
		       PennImporterEnv *pArg            /* optional argument */
		       );

PennToken *newPennToken()
{
	PennToken *pToken = new PennToken;
	pToken->pString = 0;
	return pToken;
}

extern void PennParserTrace(FILE *TraceFILE, char *zTracePrompt);

int penn_yyparse(PennImporterEnv *pEE)
{
	void *pParser;
	PennToken *pToken;
	int hTokenId;

	// PennParserTrace(stdout, "TRACE: ");
  
	pEE->bSyntaxError = false,
		pParser = PennParserAlloc( malloc );
	pToken = newPennToken();
	while((hTokenId = penn_yylex(pToken, pEE))
	      && !pEE->bSyntaxError) {
	  
		PennParser(pParser, hTokenId, pToken, pEE);
		pToken = newPennToken();
	}
	PennParser(pParser, 0, pToken, pEE);
	PennParserFree(pParser, free );
	deletePennToken(pToken);
	if (pEE->bSyntaxError) {
		return 1;
	} else {
		return 0;
	}
}


/////////////////////////////////////////////////////////////////
//
// EmdrosMemObject
//
/////////////////////////////////////////////////////////////////


EmdrosMemObject::EmdrosMemObject()
	: m_id_d(0)
{
}


EmdrosMemObject::EmdrosMemObject(const SetOfMonads& monads, id_d_t id_d)
	: m_monads(monads),
	  m_id_d(id_d)
{
}


EmdrosMemObject::EmdrosMemObject(monad_m monad, id_d_t id_d)
	: m_monads(monad),
	  m_id_d(id_d)
{
}


EmdrosMemObject::EmdrosMemObject(monad_m first_m, monad_m last_m, id_d_t id_d)
	: m_monads(first_m, last_m),
	  m_id_d(id_d)
{
}


void EmdrosMemObject::addMonads(const SetOfMonads& monads)
{
	m_monads.unionWith(monads);
}

 // Does a union-with on m_monads
void EmdrosMemObject::setMonads(const SetOfMonads& monads)
{
	m_monads = monads;
}

 // Does an assignment to m_monads
const SetOfMonads& EmdrosMemObject::getMonads(void) const
{
	return m_monads;
}


const std::string& EmdrosMemObject::getFeature(const std::string& feature_name) const
{
	const static std::string empty_string("");
	strstrmap::const_iterator ci = m_features.find(feature_name);
	if (ci == m_features.end()) {
		return empty_string;
	} else {
		return ci->second;
	}
}


void EmdrosMemObject::setFeature(const std::string& feature_name, const std::string& value)
{
	m_features[feature_name] = value;
}

bool EmdrosMemObject::hasFeature(const std::string& feature_name) const
{
	return m_features.find(feature_name) != m_features.end();
}


bool EmdrosMemObject::putMQL(std::ostream *pOut, const std::list<FeatureInfo>& feature_infos, const std::string& object_type_name) const
{
	if (m_monads.isEmpty()) {
		return false;
	} else {
		(*pOut) << "CREATE OBJECT FROM MONADS=" << m_monads.toString();
		if (m_id_d != NIL) {
			(*pOut) << "WITH ID_D=" << m_id_d;
		}
		(*pOut) << "[" << object_type_name << '\n';

		std::list<FeatureInfo>::const_iterator ci, cend;
		ci = feature_infos.begin();
		cend = feature_infos.end();
		while (ci != cend) {
			bool bHasValue = hasFeature(ci->getName());
			if (bHasValue) {
				std::string value;
				std::string default_value;
				id_d_t feature_type = ci->getType();
				
				switch(feature_type & FEATURE_TYPE_TYPE_MASK) {
				case FEATURE_TYPE_INTEGER:
				case FEATURE_TYPE_ID_D:
					value = long2string(string2long(getFeature(ci->getName())));
					default_value = ci->getDefaultValue();
					break;
				case FEATURE_TYPE_ENUM:
					value = getFeature(ci->getName());
					default_value = ci->getDefaultValue();
					break;
				case FEATURE_TYPE_LIST_OF_INTEGER:
				case FEATURE_TYPE_LIST_OF_ID_D:
				case FEATURE_TYPE_LIST_OF_ENUM:
					value = getFeature(ci->getName());
					default_value = ci->getDefaultValue();
					break;
				case FEATURE_TYPE_STRING:
				case FEATURE_TYPE_ASCII:
					value = "\"" + encodeSTRINGstring(getFeature(ci->getName())) + "\"";
					default_value = "\"" + ci->getDefaultValue() + "\"";
					break;
				default:
					ASSERT_THROW(false, "Unknown FEATURE_TYPE");
					break;
				}
				if (value != default_value) {
					(*pOut) << "  " << ci->getName() << ":=" << value << ";\n";
				}
			}
			++ci;
		}
		(*pOut) << "]\n";

		// Only emit GO if this is a CREATE OBJECT statement
		// and not a CREATE OBJECTS WITH OBJECT TYPE statement.
		if (!object_type_name.empty()) {
			(*pOut) << "GO\n\n";
		}

		return true;
	}
}


/////////////////////////////////////////////////////////////////
//
// EmdrosImporterBase
//
/////////////////////////////////////////////////////////////////


bool EmdrosImporterBase::readDocumentsFromFiles(const std::list<std::string>& filename_list)
{
	std::list<std::string>::const_iterator ci, cend;
	ci = filename_list.begin();
	cend = filename_list.end();

	bool bResult = true;
	int nDocumentsReadSinceLastEmit = 0;
	while (ci != cend
	       && bResult) {
		std::cerr << "Now reading: " << *ci << " ..." << std::endl;
		bResult = readDocumentFromFile(*ci);
		++ci;
		++nDocumentsReadSinceLastEmit;

		if (m_pMQLOut != 0
		    && m_nDocumentsToGatherBeforeEmitting != 0
		    && nDocumentsReadSinceLastEmit >= m_nDocumentsToGatherBeforeEmitting) {
			putCorpusAsMQL(m_pMQLOut);
			
			clearDocuments();
			
			nDocumentsReadSinceLastEmit = 0;
		}
	}
      
	return bResult;
}

bool EmdrosImporterBase::readDocumentFromFile(const std::string& filename)
{
	bool bResult = false;
	std::ifstream fin;
	fin.open(filename.c_str());
	if (!fin) {
		std::cerr << "FAILURE: Could not open file " << filename << " for reading." << std::endl;
		std::cerr << "FAILURE: Action aborted." << std::endl;
		bResult = false;
	} else {
		bResult = readDocumentFromStream(&fin);
	}

	return bResult;
}



void PennTreeNode::setNodeType(const std::string& strIn)
{
	if (strIn == "-NONE-") {
		m_strNodeType = strIn;
	} else if (strIn == "") {
		m_strNodeType = strIn;
	} else if (strIn.find_first_not_of("-#") == std::string::npos) {
		m_strNodeType = strIn; // For things like '--' in TIGER corpus
	} else {
		std::list<std::string> outlist;
		split_string(strIn, "-#", outlist);
		std::list<std::string>::size_type outlist_size = outlist.size();
		if (outlist_size <= 1) {
			m_strNodeType = strIn;
		} else {
			std::string last_element = *outlist.rbegin();
			if (string_is_number(last_element)) {
				m_strCoref = last_element;
				if (outlist_size == 2) {
					m_strNodeType = *outlist.begin();
				} else {
					m_strNodeType = *outlist.begin();
					
					//
					// Copy the ones between the first and the last
					//
					std::list<std::string>::const_iterator ci, cendminus1;
					
					// Get the last element
					cendminus1 = outlist.end();
					--cendminus1;
					
					// Get one past the beginning
					ci = outlist.begin();
					++ci;
					
					std::list<std::string> innerlist;
					while (ci != cendminus1) {
						innerlist.push_back(*ci);
						++ci;
					}
					
					m_strFunction = joinList("-", innerlist);
				}
			} else {
				m_strNodeType = *outlist.begin();

				//
				// Copy the ones from one past the first to the last
				//
				std::list<std::string>::const_iterator ci, cend;
				
				// Get the last element
				cend = outlist.end();
				
				// Get one past the beginning
				ci = outlist.begin();
				++ci;
				
				std::list<std::string> innerlist;
				while (ci != cend) {
					innerlist.push_back(*ci);
					++ci;
				}
				
				m_strFunction = joinList("-", innerlist);
			}
		}
	}
}

void PennTreeNode::setTerminal(const std::string& strIn)
{
	if (strIn.find_first_of("*") != std::string::npos) {
		if (strIn.find_first_of("-") != std::string::npos) {
			std::string::size_type last_index_dash = strIn.find_last_of("-");
			std::string supposedCoref = strIn.substr(last_index_dash+1);
			if (string_is_number(supposedCoref)) {
				m_strCoref = supposedCoref;
				m_strTerminal = strIn.substr(0,last_index_dash);
			} else {
				m_strTerminal = strIn;
			}
		} else {
			m_strTerminal = strIn;
		}
	} else {
		m_strTerminal = strIn;
	}
}


PennImporterEnv::~PennImporterEnv() 
{
	delete pLexer;
}

void PennImporterEnv::clean() 
{
	delete pLexer;
	pLexer = 0;
	strErrorMessage = "";
	bSyntaxError = false;
	pCorpus = 0;
}




PennTreebankImporter::PennTreebankImporter(bool bEmitNonTerminalsAsDistinctObjectTypes, monad_m start_monad, id_d_t start_id_d, std::ostream *pMQLOut, int nDocumentsToGatherBeforeEmitting, bool bUseIntegerDocIDs)
	: EmdrosImporterBase(start_monad, start_id_d, pMQLOut, nDocumentsToGatherBeforeEmitting),
	  m_pCorpus(0),
	  m_bEmitNonTerminalsAsDistinctObjectTypes(bEmitNonTerminalsAsDistinctObjectTypes),
	  m_bUseIntegerDocIDs(bUseIntegerDocIDs)
{
	m_pEE = new PennImporterEnv();
}


PennTreebankImporter::~PennTreebankImporter()
{
	delete m_pEE;
	delete m_pCorpus;
}


bool PennTreebankImporter::readDocumentFromStream(std::istream *pIn)
{
	bool bResult = false;
	m_pEE->pLexer = new PennScanner(pIn);
	int nResult = penn_yyparse(m_pEE);
	if (nResult != 0) {
		std::cerr << "FAILURE: Syntax error parsing Penn Treebank document\n"
			  << "FAILURE: Error message was: " << m_pEE->strErrorMessage << std::endl;
		delete m_pEE->pCorpus;
		m_pEE->pCorpus = 0;
		m_pEE->clean();
		bResult = false;
	} else {
		m_pCorpus = m_pEE->pCorpus;
		m_pEE->pCorpus = 0;
		m_pEE->clean();
		bResult = true;
	}


	if (bResult && m_pCorpus != 0 && !m_pCorpus->isEmpty()) {
		// Reserve id_d and first monad for doc
		id_d_t doc_id_d = m_cur_id_d;
		++m_cur_id_d;
		m_cur_docid = 0;
		long doc_docid = m_cur_docid;
		++m_cur_docid;
		monad_m doc_first_monad = m_cur_monad;

		// Assign monads and id_ds
		PennTreeNodeList *pCorpus = m_pCorpus;

		PennTreeNodeListConstIterator corpus_CI = pCorpus->const_iterator();
		while (corpus_CI.hasNext()) {
			// Ugly, I know, but I'm lazy and don't want to 
			// implement ParserListIterator.
			PennTreeNode *pTopTree = const_cast<PennTreeNode*>(corpus_CI.next());
			SetOfMonads sibling_monads;
			assignMonadsAndID_Ds(pTopTree, sibling_monads, doc_id_d, doc_docid);
		}

		// Resolve coreferences
		CorefMap mymap;
		corpus_CI = pCorpus->const_iterator();
		while (corpus_CI.hasNext()) {
			// Ugly, I know, but I'm lazy and don't want to 
			// implement ParserListIterator.
			PennTreeNode *pTopTree = const_cast<PennTreeNode*>(corpus_CI.next());
			resolveCorefs(pTopTree, mymap);
		}
		
		// Create objects
		corpus_CI = pCorpus->const_iterator();
		while (corpus_CI.hasNext()) {
			// Ugly, I know, but I'm lazy and don't want to 
			// implement ParserListIterator.
			PennTreeNode *pTopTree = const_cast<PennTreeNode*>(corpus_CI.next());
			createObjects(pTopTree);
		}

		// Create document
		createDoc(doc_first_monad, m_cur_monad-1, doc_id_d, doc_docid);
	}

	if (bResult && m_pCorpus != 0) {
		// Clean up...
		clearAST();
	}


	return bResult;
}


void PennTreebankImporter::putSchema(std::ostream *pOut)
{
	if (m_bUseIntegerDocIDs) {
		std::cerr << "UP231" << std::endl;
		(*pOut) << penn_treebank_schema << std::endl;
	} else {
		std::cerr << "UP232" << std::endl;
		(*pOut) << penn_treebank_id_d_schema << std::endl;
	}
}

std::string nodeType2OTN(const std::string& nodeType)
{
	std::string result;
	char c;
	for (unsigned int index = 0;
	     index < nodeType.size();
	     ++index) {
		c = nodeType[index];
		if ((c >= 'A' && c <= 'Z')
		    || (c >= 'a' && c <= 'z')
		    || (c >= '0' && c <= '9')
		    || c == '_') {
			result += c;
		} else {
			result += "x" + char2hex(c);
		}
	}
	return result;
}

void PennTreebankImporter::putCorpusAsMQL(std::ostream *pOut)
{
	PEMOList::const_iterator ci, cend, ctmp;
	bool bTransactionInProgress = false;
	long counter = 1;

	// Documents
	if (!m_docs.empty()) {
		(*pOut) << "BEGIN TRANSACTION GO\n";
		(*pOut) << "////////////////////////////////\n"
			<< "//\n"
			<< "// Documents\n"
			<< "//\n"
			<< "////////////////////////////////\n";
		std::list<FeatureInfo> document_feature_infos;
		if (m_bUseIntegerDocIDs) {
			document_feature_infos.push_back(FeatureInfo("docid", 
								     FEATURE_TYPE_INTEGER, 
								     "0", 
								     false));
		}
		(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [Document]\n";
		for (ci = m_docs.begin();
		     ci != m_docs.end();
		     ++ci) {
			(*ci)->putMQL(pOut, document_feature_infos, "");
		}
		(*pOut) << "GO\n\n";
	}


	// Roots
	if (!m_root_trees.empty()) {
		(*pOut) << "////////////////////////////////\n"
			<< "//\n"
			<< "// DocumentRoots\n"
			<< "//\n"
			<< "////////////////////////////////\n";
		std::list<FeatureInfo> root_feature_infos; // This is empty!
		if (m_bUseIntegerDocIDs) {
			root_feature_infos.push_back(FeatureInfo("docid", 
								 FEATURE_TYPE_INTEGER, 
								 "0", 
								 false));
			root_feature_infos.push_back(FeatureInfo("parent", 
								 FEATURE_TYPE_INTEGER, 
								 "0", 
								 false));
		} else {
			root_feature_infos.push_back(FeatureInfo("parent", 
								 FEATURE_TYPE_ID_D, 
								 "nil", 
								 false));
		}
		(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [DocumentRoot]\n";
		for (ci = m_root_trees.begin();
		     ci != m_root_trees.end();
		     ++ci) {
			(*ci)->putMQL(pOut, root_feature_infos, "");
		}
		(*pOut) << "GO\n\n";
	}

	(*pOut) << "COMMIT TRANSACTION GO\n\n";

	// Nonterminals
	if (!m_nonterminals.empty()) {
		if (m_bEmitNonTerminalsAsDistinctObjectTypes) {
			// Distribute into several PEMOList's based on
			// node type
			typedef std::map<std::string, PEMOList> NTName2PEMOListMap;
			NTName2PEMOListMap ntmap;
			for (ci = m_nonterminals.begin();
			     ci != m_nonterminals.end();
			     ++ci) {
				EmdrosMemObject *pNonTerminal = *ci;
				std::string nodeType = pNonTerminal->getFeature("nttype");
				
				NTName2PEMOListMap::iterator it = ntmap.find(nodeType);
				if (it == ntmap.end()) {
					ntmap.insert(std::make_pair(nodeType, PEMOList()));
					it = ntmap.find(nodeType);
				}
				it->second.push_back(pNonTerminal);
			}
			NTName2PEMOListMap::const_iterator ci2 = ntmap.begin();
			NTName2PEMOListMap::const_iterator cend2 = ntmap.end();
			while (ci2 != cend2) {
				std::string nodeType = ci2->first;
				std::string objectTypeName = nodeType2OTN(nodeType);
				(*pOut) << "CREATE OBJECT TYPE\n"
					<< "IF NOT EXISTS\n"
					<< "WITH SINGLE RANGE OBJECTS\n"
					<< "[" << objectTypeName << '\n';
				if (m_bUseIntegerDocIDs) {
					(*pOut) << "   docid : INTEGER WITH INDEX;\n"
						<< "   parent : INTEGER;\n"
						<< "   coref : LIST OF INTEGER; // Coreference links\n";
				} else {
					(*pOut) << "   parent : ID_D;\n"
						<< "   coref : LIST OF ID_D; // Coreference links\n";
				}
				
				(*pOut) << "   function : STRING FROM SET WITH INDEX; // The rest of the nonterminal name (except for coreference pointer)\n"
					<< "]\nGO\n";
				if (m_DroppedIndexOTNs.find(objectTypeName) == m_DroppedIndexOTNs.end()) {
					// Only drop indexes on object
					// types for which we have not
					// already dropped indexes in
					// this session.
					(*pOut) << "DROP INDEXES ON OBJECT TYPE [" << objectTypeName << "]GO\n\n";
					m_DroppedIndexOTNs.insert(objectTypeName);
				}
				
				const PEMOList& pemolist = ci2->second;
				
				putNonterminals(pOut, pemolist, objectTypeName);
				
				++ci2;
			}
		} else {
			putNonterminals(pOut, m_nonterminals, "Nonterminal");
		}
	}
	
	/*
	if (bTransactionInProgress) {
	(*pOut) << "COMMIT TRANSACTION GO\n\n";
	}
	*/


	// Tokens
	if (!m_terminals.empty()) {
		(*pOut) << "////////////////////////////////\n"
			<< "//\n"
			<< "// Tokens\n"
			<< "//\n"
			<< "////////////////////////////////\n";
		(*pOut) << "BEGIN TRANSACTION GO\n\n";
		bTransactionInProgress = true;

		std::list<FeatureInfo> token_feature_infos;
		if (m_bUseIntegerDocIDs) {
			token_feature_infos.push_back(FeatureInfo("docid", 
								  FEATURE_TYPE_INTEGER, 
								  "0", 
								  false));
			token_feature_infos.push_back(FeatureInfo("parent", 
								  FEATURE_TYPE_INTEGER, 
								  "0", 
								  false));
			token_feature_infos.push_back(FeatureInfo("coref", 
								  FEATURE_TYPE_LIST_OF_INTEGER, 
								  "()", 
								  false));
		} else {
			token_feature_infos.push_back(FeatureInfo("parent", 
								  FEATURE_TYPE_ID_D, 
								  "nil", 
								  false));
			token_feature_infos.push_back(FeatureInfo("coref", 
								  FEATURE_TYPE_LIST_OF_ID_D, 
								  "()", 
								  false));
		}
		token_feature_infos.push_back(FeatureInfo("surface", 
							  FEATURE_TYPE_STRING, 
							  "", 
							  false));
		token_feature_infos.push_back(FeatureInfo("mytype", 
							  FEATURE_TYPE_STRING, 
							  "", 
							  false));
		token_feature_infos.push_back(FeatureInfo("function", 
							  FEATURE_TYPE_STRING, 
							  "", 
							  false));
		bTransactionInProgress = true;
		//(*pOut) << "BEGIN TRANSACTION GO\n";
		(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [Token]\n";
		
		//counter = 1;
		for (ci = m_terminals.begin();
		     ci != m_terminals.end();
		     ++ci) {
			(*ci)->putMQL(pOut, token_feature_infos, "");
			++counter;
			if ((counter % 100000) == 1) {
				(*pOut) << "GO\nCOMMIT TRANSACTION GO\n";
				ctmp = ci;
				++ctmp;
				if (ctmp != m_terminals.end()) {
					(*pOut) << "BEGIN TRANSACTION GO\n";
					(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [Token]\n";
					bTransactionInProgress = true;
				} else {
					bTransactionInProgress = false;
				}
				
			}
		}
		(*pOut) << "GO\n";
	}
	if (bTransactionInProgress) {
		(*pOut) << "COMMIT TRANSACTION GO\n\n";
	}
}

void doAddCorefToMap(PennTreeNode *pNode, CorefMap& mymap, bool bUseIntegerDocIDs)
{
	if (pNode->getStrCoref() != "") {
		long coref = string2long(pNode->getStrCoref());
		if (mymap.find(coref) == mymap.end()) {
			mymap[coref] = std::list<id_d_t>();
		}
		if (bUseIntegerDocIDs) {
			long mydocid = pNode->getDocID();
			mymap[coref].push_back(mydocid);
		} else {
			long mydocid = pNode->getID_D();
			mymap[coref].push_back(mydocid);
		}
	} 
}

void doAddCorefsToNode(PennTreeNode *pNode, CorefMap& mymap, bool bUseIntegerDocIDs)
{
	if (pNode->getStrCoref() != "") {
		long coref = string2long(pNode->getStrCoref());
		long mydocid;
		if (bUseIntegerDocIDs) {
			mydocid = pNode->getDocID();
		} else {
			mydocid = pNode->getID_D();
		}
		const std::list<long>& mylist = mymap[coref];
		std::list<long>::const_iterator ci = mylist.begin();
		std::list<long>::const_iterator cend = mylist.end();
		while (ci != cend) {
			if (*ci != mydocid) {
				pNode->getCorefs().push_back(*ci);
			}
			++ci;
		}
	}
}

void addCorefsToMap(PennTreeNode *pNode, CorefMap& mymap, bool bUseIntegerDocIDs)
{
	if (pNode->getType() == kPennTerminal) {
		doAddCorefToMap(pNode, mymap, bUseIntegerDocIDs);
	} else { // kPennRoot and kPennNonTerminal
		doAddCorefToMap(pNode, mymap, bUseIntegerDocIDs);

		if (pNode->getChild() != 0) {
			addCorefsToMap(pNode->getChild(), mymap, bUseIntegerDocIDs);
		}

		PennTreeNode *pSiblingNode = pNode->getNextSibling();
		if (pSiblingNode != 0) {
			addCorefsToMap(pSiblingNode, mymap, bUseIntegerDocIDs);
		}
	}
}

void addCorefsToNodes(PennTreeNode *pNode, CorefMap& mymap, bool bUseIntegerDocIDs)
{
	if (pNode->getType() == kPennTerminal) {
		doAddCorefsToNode(pNode, mymap, bUseIntegerDocIDs);
	} else { // kPennRoot and kPennNonTerminal
		doAddCorefsToNode(pNode, mymap, bUseIntegerDocIDs);

		if (pNode->getChild() != 0) {
			addCorefsToNodes(pNode->getChild(), mymap, bUseIntegerDocIDs);
		}

		PennTreeNode *pSiblingNode = pNode->getNextSibling();
		if (pSiblingNode != 0) {
			addCorefsToNodes(pSiblingNode, mymap, bUseIntegerDocIDs);
		}
	}

}

void PennTreebankImporter::putNonterminals(std::ostream *pOut, const PEMOList& nonterminals, const std::string& objectTypeName)
{	
	PEMOList::const_iterator ci, cend, ctmp;
	bool bTransactionInProgress = false;
	long counter = 1;
	(*pOut) << "////////////////////////////////\n"
		<< "//\n";
	if (m_bEmitNonTerminalsAsDistinctObjectTypes) {
		(*pOut) << "// Nonterminals: " << objectTypeName << "\n";
	} else {
		(*pOut) << "// Nonterminals\n";
	}
	(*pOut) << "//\n"
		<< "////////////////////////////////\n";
	std::list<FeatureInfo> nonterminal_feature_infos;
	if (m_bUseIntegerDocIDs) {
		nonterminal_feature_infos.push_back(FeatureInfo("docid", 
								FEATURE_TYPE_INTEGER, 
								"0", 
								false));
		nonterminal_feature_infos.push_back(FeatureInfo("parent", 
								FEATURE_TYPE_INTEGER, 
								"0", 
								false));
		nonterminal_feature_infos.push_back(FeatureInfo("coref", 
								FEATURE_TYPE_LIST_OF_INTEGER, 
								"()", 
								false));
	} else {
		nonterminal_feature_infos.push_back(FeatureInfo("parent", 
								FEATURE_TYPE_ID_D, 
								"nil", 
								false));
		nonterminal_feature_infos.push_back(FeatureInfo("coref", 
								FEATURE_TYPE_LIST_OF_ID_D, 
								"()", 
								false));
	}
	if (!m_bEmitNonTerminalsAsDistinctObjectTypes) {
		nonterminal_feature_infos.push_back(FeatureInfo("nttype", 
								FEATURE_TYPE_STRING, 
								"", 
								false));
	}
	nonterminal_feature_infos.push_back(FeatureInfo("function", 
							FEATURE_TYPE_STRING, 
							"", 
							false));
	(*pOut) << "BEGIN TRANSACTION GO\n";
	bTransactionInProgress = true;
	(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [" << objectTypeName << "]\n";
	
	counter = 1;
	for (ci = nonterminals.begin();
	     ci != nonterminals.end();
	     ++ci) {
		(*ci)->putMQL(pOut, nonterminal_feature_infos, "");
		++counter;
		if ((counter % 100000) == 1) {
			(*pOut) << "GO\nCOMMIT TRANSACTION GO\n";
			ctmp = ci;
			++ctmp;
			if (ctmp != m_nonterminals.end()) {
				(*pOut) << "BEGIN TRANSACTION GO\n";
				(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [" << objectTypeName << "]\n";
				bTransactionInProgress = true;
			} else {
				bTransactionInProgress = false;
			}
			
		}
	}
	(*pOut) << "GO\n";
	if (bTransactionInProgress) {
		(*pOut) << "\nCOMMIT TRANSACTION GO\n";
	}
}

void PennTreebankImporter::resolveCorefs(PennTreeNode *pNode, CorefMap& mymap)
{
	addCorefsToMap(pNode, mymap, m_bUseIntegerDocIDs);
	addCorefsToNodes(pNode, mymap, m_bUseIntegerDocIDs);
}

void PennTreebankImporter::createObjects(PennTreeNode *pNode)
{
	if (pNode->getType() == kPennRoot) {
		createRootTree(pNode);
	} else if (pNode->getType() == kPennNonTerminal) {
		PennTreeNode *pChild = pNode->getChild();
		if (pChild != 0 && pChild->getType() == kPennTerminal) {
			createTerminal(pChild, pNode);
		} else {
			createNonTerminal(pNode);
		}
	} else {
		; // Skip over terminals!
	}

	if (pNode->getType() == kPennTerminal) {
		; // Do nothing further
	} else {
		if (pNode->getChild() != 0) {
			createObjects(pNode->getChild());
		}

		PennTreeNode *pSiblingNode = pNode->getNextSibling();
		if (pSiblingNode != 0) {
			createObjects(pSiblingNode);
		}
	}
}

EmdrosMemObject *PennTreebankImporter::createTerminal(PennTreeNode *pNode, PennTreeNode *pParent)
{
	EmdrosMemObject *pObj = new EmdrosMemObject(pParent->getMonads(), pParent->getID_D());
	if (m_bUseIntegerDocIDs) {
		pObj->setFeature("docid", long2string(pParent->getDocID()));
		pObj->setFeature("parent", long2string(pParent->getParentDocID()));
	} else {
		pObj->setFeature("parent", long2string(pParent->getParentDocID()));
	}
	if (!pNode->getCorefs().empty()) {
		pObj->setFeature("coref", std::string("(") + joinList(",", pNode->getCorefs()) + ")");
	}
	pObj->setFeature("surface", pNode->getTerminal());
	pObj->setFeature("mytype", pParent->getNodeType());
	pObj->setFeature("function", pParent->getFunction());
	m_terminals.push_back(pObj);
	return pObj;
}


EmdrosMemObject *PennTreebankImporter::createNonTerminal(PennTreeNode *pNode)
{
	EmdrosMemObject *pObj = new EmdrosMemObject(pNode->getMonads(), pNode->getID_D());
	if (m_bUseIntegerDocIDs) {
		pObj->setFeature("docid", long2string(pNode->getDocID()));
		pObj->setFeature("parent", long2string(pNode->getParentDocID()));
	} else {
		pObj->setFeature("parent", long2string(pNode->getParentDocID()));
	}
	if (!pNode->getCorefs().empty()) {
		pObj->setFeature("coref", std::string("(") + joinList(",", pNode->getCorefs()) + ")");
	}
	pObj->setFeature("nttype", pNode->getNodeType());
	pObj->setFeature("function", pNode->getFunction());
	m_nonterminals.push_back(pObj);
	return pObj;
}


EmdrosMemObject *PennTreebankImporter::createRootTree(PennTreeNode *pNode)
{
	EmdrosMemObject *pObj = new EmdrosMemObject(pNode->getMonads(), pNode->getID_D());
	if (m_bUseIntegerDocIDs) {
		pObj->setFeature("docid", long2string(pNode->getDocID()));
		pObj->setFeature("parent", long2string(pNode->getParentDocID()));
	} else {
		pObj->setFeature("parent", long2string(pNode->getParentDocID()));
	}
	m_root_trees.push_back(pObj);
	return pObj;
}

void PennTreebankImporter::clearAST(void)
{
	delete m_pCorpus;
	m_pCorpus = 0;
}

void PennTreebankImporter::clearDocuments(void)
{
	clearPEMOList(m_terminals);
	clearPEMOList(m_nonterminals);
	clearPEMOList(m_root_trees);
	clearPEMOList(m_docs);
}

void PennTreebankImporter::clearPEMOList(PEMOList& mylist)
{
	PEMOList::iterator it = mylist.begin();
	while  (it != mylist.end()) {
		EmdrosMemObject *pObj = *it;
		delete pObj;
		++it;
	}
	mylist.clear();
}


EmdrosMemObject *PennTreebankImporter::createDoc(monad_m first, monad_m last, id_d_t id_d, long doc_docid)
{
	SetOfMonads monads(first, last);
	EmdrosMemObject *pObj = new EmdrosMemObject(monads, id_d);
	if (m_bUseIntegerDocIDs) {
		pObj->setFeature("docid", long2string(doc_docid));
	}
	m_docs.push_back(pObj);
	return pObj;
}

SetOfMonads PennTreebankImporter::assignMonadsAndID_Ds(PennTreeNode *pNode, SetOfMonads& sibling_monads, id_d_t parent_id_d, long parent_docid)
{
	if (pNode->getType() == kPennTerminal) {
		// Don't increase m_cur_id_d here, 
		// since we won't create an object from the terminal.
		// Instead, we will create one object from this
		// and its parent NonTerminal.
		//
		// We therefore set the id_d of this node
		// to that of the parent NonTerminal.
		pNode->setID_D(parent_id_d);
		if (m_bUseIntegerDocIDs) {
			pNode->setDocID(parent_docid);
			pNode->setParentDocID(parent_docid);
		} else {
			pNode->setParentDocID(parent_id_d);
		}
		pNode->addMonad(m_cur_monad);
		++m_cur_monad;
		return pNode->getMonads();
	} else { // kRoot and kNonTerminal
		// Set id_d
		pNode->setID_D(m_cur_id_d);
		++m_cur_id_d;

		// Set docid
		if (m_bUseIntegerDocIDs) {
			pNode->setDocID(m_cur_docid);
		}
		++m_cur_docid;

		if (m_bUseIntegerDocIDs) {
			pNode->setParentDocID(parent_docid);
		} else {
			pNode->setParentDocID(parent_id_d);
		}

		SetOfMonads child_monads;
		if (pNode->getChild() != 0) {
			SetOfMonads mysibling_monads;
			child_monads = assignMonadsAndID_Ds(pNode->getChild(), mysibling_monads, pNode->getID_D(), pNode->getDocID());
			pNode->addMonads(child_monads);
			pNode->addMonads(mysibling_monads);
		}

		PennTreeNode *pSiblingNode = pNode->getNextSibling();
		if (pSiblingNode != 0) {
			assignMonadsAndID_Ds(pSiblingNode, sibling_monads, parent_id_d, parent_docid);
			sibling_monads.unionWith(pSiblingNode->getMonads());
		}

		return pNode->getMonads();
	}
}
