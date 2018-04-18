/*
 * tigerxmlimporter.h
 *
 * A class to import TIGER XML into Emdros.
 *
 * into Emdros
 * Created: 4/4-2007
 * Last update: 10/3-2011
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2011  Ulrik Sandborg-Petersen
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

#ifndef TIGERXML_IMPORTER_EMDROS_H_
#define TIGERXML_IMPORTER_EMDROS_H_

#include <list>
#include <set>

#include "qdxml.h"
#include "emdf.h"
#include "pennimporter.h" // For EmdrosImporterbase 

typedef std::map<std::string, std::string> String2StringMap;
typedef std::map<std::string, std::map<std::string, std::string> > FeatureMap;
typedef std::map<std::string, EmdrosMemObject*> String2PEMOMap;
typedef std::list<std::string> ListOfString;

class TIGERTerminal : public EmdrosMemObject {
 protected:
	std::string m_t_id;
 public:
	TIGERTerminal(const std::string& t_id, id_d_t id_d, monad_m monad);
};

class TIGERNonTerminal : public EmdrosMemObject {
 protected:
	std::string m_nt_id;
	std::string m_object_type_name;
 public:
	TIGERNonTerminal(const std::string& nt_id, id_d_t id_d);
	void setObjectTypeName(const std::string& new_object_type_name) { m_object_type_name = new_object_type_name; };
	const std::string& getObjectTypeName(void) const { return m_object_type_name; };
};

class TIGERXMLDocHandler; // Forward declaration

class TIGERXMLImporter : public EmdrosImporterBase {
 protected:
	TIGERXMLDocHandler *m_pDH;
	std::map<std::string, PEMOList> m_objects; // Maps object types to lists of objects
	std::string m_nonterminal_object_type_feature;
	std::string m_terminal_object_type_name;
	friend class TIGERXMLDocHandler;
 public:
	TIGERXMLImporter(monad_m starting_monad, id_d_t starting_id_d, const std::string& nonterminal_object_type_feature, const std::string& terminal_object_type_name);
	virtual ~TIGERXMLImporter();
	virtual bool readDocumentFromStream(std::istream *pIn);
	virtual void putSchema(std::ostream *pOut);
	virtual void putCorpusAsMQL(std::ostream *pOut);
	bool renamesNonterminals(void) const { return !m_nonterminal_object_type_feature.empty(); };
 private:
	void cleanObjects(PEMOList& pemo_list);
	void emitMQLObjects(std::ostream *pOut, const std::list<FeatureInfo>& feature_infos, PEMOList& pemo_list, std::string object_type_name);
	void addObject(const std::string& object_type_name, EmdrosMemObject *pObj);
};


typedef std::list<std::pair<std::string, std::string> > StringPairList;
typedef std::map<std::string, StringPairList > EdgeMap;

class TIGERXMLDocHandler : public QDDocHandler {
 protected:
	typedef enum {
		st_start = 0,
		st_in_head_meta = 1,
		st_in_head_annotation = 2,
		st_in_head_annotation_feature = 3,
		st_in_head_edgelabel = 4,
		st_in_head_secedgelabel = 5,
		st_in_body = 6
	} eTIGERStates;
	
	eTIGERStates m_state;
	int m_max_secedges;
	id_d_t m_curid_d;
	monad_m m_curmonad;
	TIGERXMLImporter *m_pImp;

	std::string m_curfeature;
	std::string m_curdomain;
	std::string m_curvalue;
	std::string m_graph_root;
	std::string m_sentence_id;
	id_d_t m_sentence_id_d;
	std::string m_curnt_id;
	std::string m_curnode_id;
	std::string m_corpusid;
	ListOfString m_charstack;

	FeatureMap m_T_features;
	FeatureMap m_NT_features;
	FeatureMap m_FREC_features;
	String2StringMap m_edgelabels;
	String2StringMap m_secedgelabels;
	String2PEMOMap m_terminals; 
	String2PEMOMap m_nonterminals;
	String2StringMap m_nonterminal_renaming_map;
	std::set<std::string> m_nonterminal_renaming_set;

	EdgeMap m_edges;
	EdgeMap m_secedges;

 public:
	TIGERXMLDocHandler(monad_m starting_monad, id_d_t starting_id_d, TIGERXMLImporter *pImp);
	virtual ~TIGERXMLDocHandler();
	virtual void startElement(const std::string& tag, const AttributeMap& attrs);
	virtual void endElement(const std::string& tag);
	virtual void startDocument(void);
	virtual void endDocument(void);
	virtual void text(const std::string& str);
	id_d_t getCurID_D(void) const { return m_curid_d; };
	monad_m getCurMonad(void) const { return m_curmonad; };
	void putSchema(std::ostream *pOut);
	void getFeatureInfos(std::list<FeatureInfo>& terminal_fis,
			     std::list<FeatureInfo>& nonterminal_fis,
			     std::list<FeatureInfo>& sentence_fis) const;
	std::string renameNonTerminalObjectTypeFromFeature(const std::string& value);
	std::string normalizeOTN(const std::string& inname);
 protected:
	std::string getNonterminalObjectTypeNameFromFeature(const std::string& value);
	void eraseCharStack(void);
	void addFeatures(EmdrosMemObject *pObj, const std::string& domain, const AttributeMap& attributes);
	void addEdge(EdgeMap& edge_map, const std::string& tigerid, const std::string& label, const std::string idref);
	void commitSentence(void);
	void purgeSentence(void);
	SetOfMonads assignMonads(const std::string& TIGERid);
	void completelyAssignMonads(void);
	void assignParents(const std::string& TIGERid, id_d_t parent_id_d, const std::string& edge_label);
	void assignSecEdges(const std::string& TIGERid);
	EmdrosMemObject *getNode(const std::string& TIGERid);
	void findMaxSecEdges(void);
	void pushToImporter(void);
	void writeNonterminalObjectType(const std::string& object_type_name,
					std::ostream *pOut);
};


#endif
