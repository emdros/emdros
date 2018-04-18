/*
 * tigerxmlimporter.cpp
 *
 * A class to import TIGER XML files into Emdros
 *
 * Created: 4/4-2007
 * Last update: 1/16-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2013  Ulrik Sandborg-Petersen
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

#include <tigerxmlimporter.h>
#include <string_func.h>
    
//////////////////////////////////////////////////////////////
//
// TIGERTerminal
// 
//////////////////////////////////////////////////////////////

TIGERTerminal::TIGERTerminal(const std::string& t_id, id_d_t id_d, monad_m monad)
	: EmdrosMemObject(SetOfMonads(monad), id_d), m_t_id(t_id)
{
	
}

//////////////////////////////////////////////////////////////
//
// TIGERNonTerminal
// 
//////////////////////////////////////////////////////////////

TIGERNonTerminal::TIGERNonTerminal(const std::string& nt_id, id_d_t id_d)
	: EmdrosMemObject(SetOfMonads(), id_d), 
	  m_nt_id(nt_id),
	  m_object_type_name("Nonterminal")
{
}



//////////////////////////////////////////////////////////////
//
// TIGERXMLImporter
// 
//////////////////////////////////////////////////////////////

TIGERXMLImporter::TIGERXMLImporter(monad_m starting_monad, id_d_t starting_id_d,
				   const std::string& nonterminal_object_type_feature, 
				   const std::string& terminal_object_type_name)
	: EmdrosImporterBase(starting_monad, starting_id_d),
	  m_nonterminal_object_type_feature(nonterminal_object_type_feature),
	  m_terminal_object_type_name(terminal_object_type_name)
{
	m_pDH = new TIGERXMLDocHandler(starting_monad, starting_id_d, this);
	if (m_terminal_object_type_name.empty()) {
		m_terminal_object_type_name = "Terminal";
	} else {
		m_terminal_object_type_name = m_pDH->renameNonTerminalObjectTypeFromFeature(m_terminal_object_type_name);
	}
}


TIGERXMLImporter::~TIGERXMLImporter()
{
	delete m_pDH;
	std::map<std::string, PEMOList>::iterator it;
	it = m_objects.begin();
	while (it != m_objects.end()) {
		cleanObjects(it->second);
		++it;
	}
	m_objects.erase(m_objects.begin(), m_objects.end());
}


bool TIGERXMLImporter::readDocumentFromStream(std::istream *pIn)
{
	QDParser parser;

	parser.parse(m_pDH, pIn);

	// Set local copies
	m_cur_monad = m_pDH->getCurMonad();
	m_cur_id_d = m_pDH->getCurID_D();

	return true;
}


void TIGERXMLImporter::putSchema(std::ostream *pOut)
{
	m_pDH->putSchema(pOut);
}


void TIGERXMLImporter::putCorpusAsMQL(std::ostream *pOut)
{
	std::list<FeatureInfo> terminal_fis, nonterminal_fis, sentence_fis;
	m_pDH->getFeatureInfos(terminal_fis, nonterminal_fis, sentence_fis);

	std::map<std::string, PEMOList>::iterator it = m_objects.begin();
	while (it != m_objects.end()) {
		std::string object_type_name = it->first;
		PEMOList& mylist = it->second;

		if (object_type_name == "Sentence") {
			emitMQLObjects(pOut, 
				       sentence_fis,
				       mylist, 
				       "Sentence");
		} else if (object_type_name == "Terminal") {
			emitMQLObjects(pOut, 
				       terminal_fis,
				       mylist, 
				       m_terminal_object_type_name);
		} else {
			// Nonterminals
			emitMQLObjects(pOut, 
				       nonterminal_fis,
				       mylist, 
				       object_type_name);
		}
		
		++it;
	}

}


void TIGERXMLImporter::cleanObjects(PEMOList& pemo_list)
{
	PEMOList::iterator it = pemo_list.begin();
	PEMOList::iterator itend = pemo_list.end();

	while (it != itend) {
		EmdrosMemObject *pObj = *it;
		delete pObj;
		++it;
	}

	pemo_list.erase(pemo_list.begin(), pemo_list.end());
}


void TIGERXMLImporter::emitMQLObjects(std::ostream *pOut, const std::list<FeatureInfo>& feature_infos, PEMOList& pemo_list, std::string object_type_name)
{
	int count = 0;
	PEMOList::iterator it = pemo_list.begin();
	PEMOList::iterator itend = pemo_list.end();

	while (it != itend) {
		if (count == 0) {
			(*pOut) << "BEGIN TRANSACTION GO\n"
				<< "CREATE OBJECTS WITH OBJECT TYPE [" << object_type_name << "]\n";
		}
		count += (*it)->putMQL(pOut, feature_infos, "");
		if (count >= 50000) {
			count = 0;
			(*pOut) << "GO\n"
				<< "COMMIT TRANSACTION GO\n";
		}
		++it;
	}
	if (count != 0) {
		(*pOut) << "GO\n"
			<< "COMMIT TRANSACTION GO\n";
	}
}



void TIGERXMLImporter::addObject(const std::string& object_type_name, EmdrosMemObject *pObj)
{
	if (m_objects.find(object_type_name) == m_objects.end()) {
		m_objects.insert(std::make_pair(object_type_name, PEMOList()));
	}

	m_objects[object_type_name].push_back(pObj);
}







//////////////////////////////////////////////////////////////
//
// TIGERXMLDocHandler
// 
//////////////////////////////////////////////////////////////


TIGERXMLDocHandler::TIGERXMLDocHandler(monad_m starting_monad, id_d_t starting_id_d, TIGERXMLImporter *pImp)
	: m_state(st_start),
	  m_max_secedges(0),
	  m_curid_d(starting_id_d),
	  m_curmonad(starting_monad),
	  m_pImp(pImp)
{
	if (m_pImp->renamesNonterminals()) {
		// Make sure that we don't get Nonterminals with these names...
		m_nonterminal_renaming_set.insert(std::string("nonterminal"));
		m_nonterminal_renaming_set.insert(std::string("terminal"));
		m_nonterminal_renaming_set.insert(std::string("sentence"));
		m_nonterminal_renaming_map.insert(std::make_pair(std::string("Nonterminal"), std::string("Nonterminal")));
		m_nonterminal_renaming_map.insert(std::make_pair(std::string("Terminal"), std::string("Terminal")));
		m_nonterminal_renaming_map.insert(std::make_pair(std::string("Sentence"), std::string("Sentence")));
	}
}


TIGERXMLDocHandler::~TIGERXMLDocHandler()
{
	purgeSentence();
	eraseCharStack();
}

void TIGERXMLDocHandler::addFeatures(EmdrosMemObject *pObj, const std::string& domain, const AttributeMap& attributes)
{
	// Do FREC features first	
	bool bHasNonterminalRenaming = m_pImp->renamesNonterminals()
		&& domain == "NT";

	std::string object_type_name = "Nonterminal";

	FeatureMap::const_iterator ci, cend;
	ci = m_FREC_features.begin();
	cend = m_FREC_features.end();
	while (ci != cend) {
		std::string key = ci->first;
		std::string value = std::string(getAttribute(attributes, key));
		if (bHasNonterminalRenaming
		    && m_pImp->m_nonterminal_object_type_feature == key) {
			object_type_name = getNonterminalObjectTypeNameFromFeature(value);
		} else {
			pObj->setFeature(key, value);
		}
		++ci;
	}

	// Do domain features then
	if (domain == "T") {
		ci = m_T_features.begin();
		cend = m_T_features.end();
	} else if (domain == "NT") {
		ci = m_NT_features.begin();
		cend = m_NT_features.end();
	} else {
		throw QDException("Unknown domain " + domain + " in TGERXMLDocHandler::addFeatures().");
	}

	while (ci != cend) {
		std::string key = ci->first;
		if (attributes.find(key) != attributes.end()) {
			std::string value = std::string(getAttribute(attributes, key));
			if (bHasNonterminalRenaming
			    && m_pImp->m_nonterminal_object_type_feature == key) {
				object_type_name = getNonterminalObjectTypeNameFromFeature(value);
			} else {
				pObj->setFeature(key, value);
			}
		}
		++ci;
	}
	if (domain == "NT" &&
	    bHasNonterminalRenaming) {
		((TIGERNonTerminal*)pObj)->setObjectTypeName(object_type_name);
	}
}

void TIGERXMLDocHandler::addEdge(EdgeMap& edge_map, const std::string& tigerid, const std::string& label, const std::string idref)
{
	if (edge_map.find(tigerid) == edge_map.end()) {
		edge_map.insert(std::make_pair(tigerid, std::list<std::pair<std::string, std::string> >()));
	}
	edge_map[tigerid].push_back(std::make_pair(label, idref));
}

	
void TIGERXMLDocHandler::startElement(const std::string& tag, const AttributeMap& attributes)
{
	std::string chars = joinListEmptyBetween(m_charstack);
	eraseCharStack();

        if (tag == "corpus") {
		if (attributes.find("id") == attributes.end()) {
			m_corpusid = "NOTSET";
		} else {
			m_corpusid = std::string(getAttribute(attributes, "id"));
		}
        } else if (tag == "meta") {
		m_state = st_in_head_meta;
        } else if (tag == "annotation") {
		m_state = st_in_head_annotation;
        } else if (tag == "feature") {
		m_state = st_in_head_annotation_feature;
		m_curfeature = std::string(getAttribute(attributes, "name"));
		m_curdomain = std::string(getAttribute(attributes, "domain"));
		if (m_curdomain == "T") {
			m_T_features[m_curfeature] = std::map<std::string, std::string>();
		} else if (m_curdomain == "NT") {
			m_NT_features[m_curfeature] = std::map<std::string, std::string>();

		} else if (m_curdomain == "FREC"){
			m_FREC_features[m_curfeature] = std::map<std::string, std::string>();
		} else {
			throw QDException("While doing tag <feature>, unknown domain: '" + m_curdomain + "'.");
		}
        } else if (tag == "edgelabel") {
		m_state = st_in_head_edgelabel;
        } else if (tag == "secedgelabel") {
		m_state = st_in_head_secedgelabel;
        } else if (tag == "value") {
		if (m_state == st_in_head_annotation_feature
		    || m_state == st_in_head_edgelabel
		    || m_state == st_in_head_secedgelabel) {
			m_curvalue = std::string(getAttribute(attributes, "name"));
		} else {
			std::ostringstream ostr;
			ostr << "Error: Unknown state " << m_state << " while doing start-tag <value>.";
			throw QDException(ostr.str());
		}
        } else if (tag == "head") {
		;
        } else if (tag == "body") {
		m_state = st_in_body;
        } else if (tag == "s") {
		m_sentence_id = std::string(getAttribute(attributes, "id"));
		m_sentence_id_d = m_curid_d;
		m_curid_d++;
        } else if (tag == "graph") {
		m_graph_root = std::string(getAttribute(attributes, "root"));
        } else if (tag == "terminals") {
            ;
        } else if (tag == "nonterminals") {
            ;
        } else if (tag == "t") {
		std::string t_id = std::string(getAttribute(attributes, "id"));
		EmdrosMemObject *pT = new TIGERTerminal(t_id, m_curid_d, m_curmonad);
		m_curnode_id = t_id;
		m_curmonad++;
		m_curid_d++;
		m_terminals.insert(std::make_pair(t_id, pT));
		addFeatures(pT, "T", attributes);
        } else if (tag == "nt") {
		std::string nt_id(getAttribute(attributes, "id"));
		m_curnode_id = nt_id;
		EmdrosMemObject *pNT = new TIGERNonTerminal(nt_id, m_curid_d);
		m_curid_d++;
		m_nonterminals.insert(std::make_pair(nt_id, pNT));
		addFeatures(pNT, "NT", attributes);
        } else if (tag == "edge") {
		std::string label = "";
		if (attributes.find("label") != attributes.end()) {
			label = getAttribute(attributes, "label");
		}
		addEdge(m_edges, m_curnode_id, label, getAttribute(attributes, "idref"));
        } else if (tag == "secedge") { 
		addEdge(m_secedges, m_curnode_id, getAttribute(attributes, "label"), getAttribute(attributes, "idref"));
	} else if (m_state == st_in_head_meta) {
		; // Ignore all meta-information
	} else {
		std::ostringstream ostr;
		ostr << "Warning: Unknown start-tag <" << tag << ">";
		std::cerr << ostr.str() << '\n';
		// throw QDException(ostr.str());
	}
}


void TIGERXMLDocHandler::endElement(const std::string& tag)
{
	std::string chars = joinListEmptyBetween(m_charstack);
	eraseCharStack();

	if (tag == "corpus") {
		; // This is where we could have dumped the schema!
	} else if (tag == "meta") {
		;
	} else if (tag == "annotation") {
		;
        } else if (tag == "feature") {
		;
        } else if (tag == "value") {
		if (m_state == st_in_head_annotation_feature) {
			if (m_curdomain == "T") {
				m_T_features[m_curfeature].insert(std::make_pair(m_curvalue, chars));
			} else if (m_curdomain == "NT") {
				m_NT_features[m_curfeature].insert(std::make_pair(m_curvalue, chars));
			} else if (m_curdomain == "FREC") {
				m_FREC_features[m_curfeature].insert(std::make_pair(m_curvalue, chars));
			} else {
				throw QDException("While doing tag </value>: Unknown m_curdomain '" + m_curdomain + "'.");
			}
		} else if (m_state == st_in_head_edgelabel) {
			m_edgelabels.insert(std::make_pair(m_curvalue,chars));
		} else if (m_state == st_in_head_secedgelabel) {
			m_secedgelabels.insert(std::make_pair(m_curvalue,chars));
		} else {
			std::ostringstream ostr;
			ostr << "Error: Unknown state " << m_state << " while doing </value> end-tag.";
			throw QDException(ostr.str());
		}
        } else if (tag == "edgelabel") {
		;
        } else if (tag == "secedgelabel") {
		;
        } else if (tag == "head") {
		;
        } else if (tag == "body") {
		;
        } else if (tag == "s") {
		commitSentence();
        } else if (tag == "graph") {
		;
        } else if (tag == "terminals") {
		;
        } else if (tag == "nonterminals") {
		;
        } else if (tag == "t") {
		;
        } else if (tag == "nt") {
		;
        } else if (tag == "edge") {
		;
        } else if (tag == "secedge") { 
		;
	} else if (m_state == st_in_head_meta) {
		; // Ignore all meta-information
	} else {
		std::ostringstream ostr;
		ostr << "Warning: Unknown end-tag = </" << tag << ">";
		std::cerr << ostr.str() << '\n';
		//throw QDException(ostr.str());
	}
}


void TIGERXMLDocHandler::startDocument(void)
{
	// Nothing to do!
}


void TIGERXMLDocHandler::endDocument(void)
{

	// Nothing to do!
}


void TIGERXMLDocHandler::text(const std::string& str)
{
	m_charstack.push_back(str);
}



void TIGERXMLDocHandler::eraseCharStack(void)
{
	m_charstack.erase(m_charstack.begin(), m_charstack.end());
}

void TIGERXMLDocHandler::commitSentence(void)
{
        findMaxSecEdges();
        
	assignMonads(m_graph_root);
	completelyAssignMonads();
	
	assignParents(m_graph_root, m_sentence_id_d, "--");
	
        assignSecEdges(m_graph_root);
	
	pushToImporter();

        purgeSentence();
}

void TIGERXMLDocHandler::purgeSentence(void)
{
        m_terminals.erase(m_terminals.begin(), m_terminals.end());
        m_nonterminals.erase(m_nonterminals.begin(), m_nonterminals.end());
        m_edges.erase(m_edges.begin(), m_edges.end());
        m_secedges.erase(m_secedges.begin(), m_secedges.end());

        m_curnt_id.clear();
	m_sentence_id.clear();
        m_graph_root.clear();
}

void TIGERXMLDocHandler::writeNonterminalObjectType(const std::string& object_type_name,
						    std::ostream *pOut)
{
	bool bHasNonterminalRenaming = m_pImp->renamesNonterminals();
	(*pOut) << "CREATE OBJECT TYPE\n"
		<< "["
		<< object_type_name
		<< "\n"
		<< "  parent : ID_D;\n"
		<< "  edge : STRING FROM SET;\n";
	
	for (int i = 1; i <= m_max_secedges; ++i) {
		(*pOut) << "  secedge_label" << i << " : STRING FROM SET;\n"
			<< "  secedge_parent" << i << " : ID_D;\n";
	}

	FeatureMap::const_iterator ci;
	FeatureMap::const_iterator cend;
	
	ci = m_FREC_features.begin();
	cend = m_FREC_features.end();
	while (ci != cend) {
		std::string key = ci->first;
		if (!bHasNonterminalRenaming
		    || key != m_pImp->m_nonterminal_object_type_feature) {
			(*pOut) << "  " << key << " : STRING FROM SET;\n";
		}
		++ci;
	}
	
	ci = m_NT_features.begin();
	cend = m_NT_features.end();
	while (ci != cend) {
		std::string key = ci->first;
		if (!bHasNonterminalRenaming
		    || key != m_pImp->m_nonterminal_object_type_feature) {
			(*pOut) << "  " << key << " : STRING FROM SET;\n";
		}
		++ci;
	}
	
	(*pOut) << "]\nGO\n\n\n";
}


void TIGERXMLDocHandler::putSchema(std::ostream *pOut)
{
	// Terminal
	(*pOut) << "CREATE OBJECT TYPE\n"
		<< "WITH SINGLE MONAD OBJECTS\n"
		<< "HAVING UNIQUE FIRST MONADS\n"
		<< "["
		<< m_pImp->m_terminal_object_type_name
		<< "\n"
		<< "  parent : ID_D;\n"
		<< "  edge : STRING FROM SET;\n";
	for (int i = 1; i <= m_max_secedges; ++i) {
		(*pOut) << "  secedge_label" << i << " : STRING FROM SET;\n"
			<< "  secedge_parent" << i << " : ID_D;\n";
	}
	FeatureMap::const_iterator ci;
	FeatureMap::const_iterator cend;

	ci = m_FREC_features.begin();
	cend = m_FREC_features.end();
	while (ci != cend) {
		std::string key = ci->first;
		(*pOut) << "  " << key << " : STRING FROM SET;\n";
		++ci;
	}

	ci = m_T_features.begin();
	cend = m_T_features.end();
	while (ci != cend) {
		std::string key = ci->first;
		(*pOut) << "  " << key << " : STRING FROM SET;\n";
		++ci;
	}

	(*pOut) << "]\nGO\n\n\n";
		


	// Nonterminal
	if (m_pImp->renamesNonterminals()) {
		String2StringMap::const_iterator cintrm = m_nonterminal_renaming_map.begin();
		while (cintrm != m_nonterminal_renaming_map.end()) {
			std::string oldvalue = cintrm->first;
			if (oldvalue != "Terminal"
			    && oldvalue != "Sentence"
			    && oldvalue != "Nonterminal") {
				writeNonterminalObjectType(cintrm->second, pOut);
			}
			++cintrm;
		}
	}

	// We should always have this, in case some nonterminal 
	// did not have an "ntotf" feature.
	writeNonterminalObjectType("Nonterminal", pOut);


	// Sentence
	(*pOut) << "CREATE OBJECT TYPE\n"
		<< "WITH SINGLE RANGE OBJECTS\n"
		<< "HAVING UNIQUE FIRST MONADS\n"
		<< "[Sentence\n"
		<< "   id : STRING;\n"
		<< "]\nGO\n\n";
	(*pOut) << std::endl;
}

void TIGERXMLDocHandler::completelyAssignMonads(void)
{
        while(true) {
		bool bDidOne = false;
		String2PEMOMap::iterator it = m_nonterminals.begin();
		while (it != m_nonterminals.end()) {
			std::string TIGERid = it->first;
			EmdrosMemObject *pObj = it->second;
			if (pObj->getMonads().isEmpty()
			    && m_edges[TIGERid].size() > 0) {
				assignMonads(TIGERid);
				if (!pObj->getMonads().isEmpty()) {
					bDidOne = true;
				}
			}
			++it;
		}
		if (!bDidOne) {
			break;
		}
	}
}

SetOfMonads TIGERXMLDocHandler::assignMonads(const std::string& TIGERid)
{
        if (m_terminals.find(TIGERid) != m_terminals.end()) {
		return m_terminals[TIGERid]->getMonads();
	} else {
		EmdrosMemObject *pObj = m_nonterminals[TIGERid];

		StringPairList::const_iterator ci = m_edges[TIGERid].begin();
		StringPairList::const_iterator cend = m_edges[TIGERid].end();
		while (ci != cend) {
			std::string label = ci->first;
			std::string idref = ci->second;
			pObj->addMonads(this->assignMonads(idref));
			++ci;
		}

		return pObj->getMonads();
	}
}


void TIGERXMLDocHandler::assignParents(const std::string& TIGERid, id_d_t parent_id_d, const std::string& edge_label)
{
	if (m_terminals.find(TIGERid) != m_terminals.end()) {
		EmdrosMemObject *pObj = m_terminals[TIGERid];
		pObj->setFeature("parent", id_d2string(parent_id_d));
		pObj->setFeature("edge", edge_label);
	} else {
		EmdrosMemObject *pObj = m_nonterminals[TIGERid];
		pObj->setFeature("parent", id_d2string(parent_id_d));
		pObj->setFeature("edge", edge_label);

		id_d_t nt_id_d = pObj->getID_D();
	
		StringPairList::const_iterator ci = m_edges[TIGERid].begin();
		StringPairList::const_iterator cend = m_edges[TIGERid].end();
		while (ci != cend) {
			std::string label = ci->first;
			std::string idref = ci->second;
			assignParents(idref, nt_id_d, label);
			++ci;
		}
	}
}

void TIGERXMLDocHandler::assignSecEdges(const std::string& TIGERid)
{
	// Note that secedges point UPWARDS,
	// Whereas edges point DOWNWARDS!
	if (m_secedges.find(TIGERid) != m_secedges.end()) {

		EmdrosMemObject *pObj = getNode(TIGERid);

		StringPairList::const_iterator ci = m_secedges[TIGERid].begin();
		StringPairList::const_iterator cend = m_secedges[TIGERid].end();
		long index = 1;
		while (ci != cend) {
			std::string label = ci->first;
			std::string idref = ci->second;
			EmdrosMemObject *parent_node = getNode(idref);


			std::string feature_name = "secedge_label" + long2string(index);
			pObj->setFeature(feature_name, label);


			feature_name = "secedge_parent" + long2string(index);
			pObj->setFeature(feature_name, id_d2string(parent_node->getID_D()));


			++ci;
			++index;
		}
	}

        if (m_terminals.find(TIGERid) != m_terminals.end()) {
		; // Work already done!
	} else {
		// We must recurse down...
		StringPairList::const_iterator ci = m_edges[TIGERid].begin();
		StringPairList::const_iterator cend = m_edges[TIGERid].end();
		while (ci != cend) {
			std::string label = ci->first;
			std::string idref = ci->second;
			assignSecEdges(idref);
			++ci;
		}
	}
}

EmdrosMemObject *TIGERXMLDocHandler::getNode(const std::string& TIGERid)
{
        if (m_terminals.find(TIGERid) != m_terminals.end()) {
		return m_terminals[TIGERid];
	} else {
		return m_nonterminals[TIGERid];
	}
}


void TIGERXMLDocHandler::findMaxSecEdges(void)
{
	EdgeMap::const_iterator ci1 = m_secedges.begin();
	while (ci1 != m_secedges.end()) {
		int curlength = (int) ci1->second.size();
		if (curlength > m_max_secedges) {
			m_max_secedges = curlength;
		}
		++ci1;
	}
}

void TIGERXMLDocHandler::pushToImporter(void)
{
	// Sort terminals
	std::map<monad_m, EmdrosMemObject*> terminal_map;
	String2PEMOMap::iterator it1 = m_terminals.begin();
	while (it1 != m_terminals.end()) {
		EmdrosMemObject *pObj = it1->second;
		monad_m monad = pObj->getMonads().first();
		terminal_map.insert(std::make_pair(monad, pObj));
		++it1;
	}

	// Add terminals
	std::map<monad_m, EmdrosMemObject*>::iterator it11 = terminal_map.begin();
	while (it11 != terminal_map.end()) {
		
		m_pImp->addObject("Terminal", it11->second);
		++it11;
	}

	// Sort non-terminals
	std::multimap<monad_m, EmdrosMemObject*> nonterminal_map;
	String2PEMOMap::iterator it2 = m_nonterminals.begin();
	while (it2 != m_nonterminals.end()) {
		EmdrosMemObject *pObj = it2->second;
		monad_m monad = pObj->getMonads().first();
		nonterminal_map.insert(std::multimap<monad_m, EmdrosMemObject*>::value_type(monad, pObj));
		++it2;
	}

	std::multimap<monad_m, EmdrosMemObject*>::iterator it21 = nonterminal_map.begin();
	while (it21 != nonterminal_map.end()) {
		EmdrosMemObject *pObj = it21->second;
		m_pImp->addObject(((TIGERNonTerminal*)pObj)->getObjectTypeName(), pObj);
		++it21;
	}
	


	// Sentence
	monad_m fm = terminal_map.begin()->second->getMonads().first();
	monad_m lm = terminal_map.rbegin()->second->getMonads().last();
	EmdrosMemObject *pSentence = new EmdrosMemObject(fm, lm, m_sentence_id_d);
	pSentence->setFeature("id", m_sentence_id);
	m_pImp->addObject("Sentence", pSentence);
}


void TIGERXMLDocHandler::getFeatureInfos(std::list<FeatureInfo>& terminal_fis,
			     std::list<FeatureInfo>& nonterminal_fis,
			     std::list<FeatureInfo>& sentence_fis) const
{
	// Build feature_infos
	std::list<FeatureInfo> feature_infos_T;
	std::list<FeatureInfo> feature_infos_NT;

	FeatureInfo parent_fi("parent",
			      FEATURE_TYPE_ID_D,
			      "0",  // default
			      false); // Not computed
	feature_infos_T.push_back(parent_fi);
	feature_infos_NT.push_back(parent_fi);

	FeatureInfo edge_fi("edge",
			      FEATURE_TYPE_STRING
			      | FEATURE_TYPE_FROM_SET,
			      "",  // default
			      false); // Not computed
	feature_infos_T.push_back(edge_fi);
	feature_infos_NT.push_back(edge_fi);

	for (long i = 1; i <= m_max_secedges; ++i) {
		FeatureInfo secedge_label_fi = 
			FeatureInfo("secedge_label" + long2string(i),
				    FEATURE_TYPE_STRING
				    | FEATURE_TYPE_FROM_SET,
				    "",  // default
				    false); // Not computed
		
		feature_infos_T.push_back(secedge_label_fi);
		feature_infos_NT.push_back(secedge_label_fi);

		FeatureInfo secedge_parent_fi = 
			FeatureInfo("secedge_parent" + long2string(i),
				    FEATURE_TYPE_ID_D,
				    "0",  // default
				    false); // Not computed
		
		feature_infos_T.push_back(secedge_parent_fi);
		feature_infos_NT.push_back(secedge_parent_fi);
	}
	FeatureMap::const_iterator ci;
	FeatureMap::const_iterator cend;

	ci = m_FREC_features.begin();
	cend = m_FREC_features.end();
	bool bHasNonterminalRenaming = m_pImp->renamesNonterminals();
	while (ci != cend) {
		std::string key = ci->first;

		FeatureInfo feature_fi = 
			FeatureInfo(key,
				    FEATURE_TYPE_STRING
				    | FEATURE_TYPE_FROM_SET,
				    "",  // default
				    false); // Not computed
		
		feature_infos_T.push_back(feature_fi);
		if (bHasNonterminalRenaming) {
			if (key != m_pImp->m_nonterminal_object_type_feature) {
				feature_infos_NT.push_back(feature_fi);
			}
		} else {
			feature_infos_NT.push_back(feature_fi);
		}

		
		++ci;
	}

	// Make a copy of the common features...
	terminal_fis = feature_infos_T;

	ci = m_T_features.begin();
	cend = m_T_features.end();
	while (ci != cend) {
		std::string key = ci->first;
		FeatureInfo feature_fi = 
			FeatureInfo(key,
				    FEATURE_TYPE_STRING
				    | FEATURE_TYPE_FROM_SET,
				    "",  // default
				    false); // Not computed
		
		terminal_fis.push_back(feature_fi);
		++ci;
	}

	

	// Make a copy of the common features...
	nonterminal_fis = feature_infos_NT;

	ci = m_T_features.begin();
	cend = m_T_features.end();
	while (ci != cend) {
		std::string key = ci->first;
		FeatureInfo feature_fi = 
			FeatureInfo(key,
				    FEATURE_TYPE_STRING
				    | FEATURE_TYPE_FROM_SET,
				    "",  // default
				    false); // Not computed

		if (bHasNonterminalRenaming) {
			if (key != m_pImp->m_nonterminal_object_type_feature) {
				nonterminal_fis.push_back(feature_fi);
			}
		} else {
		        nonterminal_fis.push_back(feature_fi);
		}

		++ci;
	}

	FeatureInfo sentence_id_fi = 
		FeatureInfo("id",
			    FEATURE_TYPE_STRING,
			    "",   // default
			    false); // not computed

	sentence_fis.push_back(sentence_id_fi);
}

std::string TIGERXMLDocHandler::getNonterminalObjectTypeNameFromFeature(const std::string& value)
{
	String2StringMap::const_iterator ci = m_nonterminal_renaming_map.find(value);
	if (ci != m_nonterminal_renaming_map.end()) {
		return ci->second;
	} else {
		std::string new_name = renameNonTerminalObjectTypeFromFeature(value);
		m_nonterminal_renaming_map.insert(std::make_pair(value, new_name));
		m_nonterminal_renaming_set.insert(normalizeOTN(new_name));
		return new_name;
	}
}

std::string TIGERXMLDocHandler::renameNonTerminalObjectTypeFromFeature(const std::string& value)
{
	std::string new_name = value;

	if (!is_identifier(new_name)) {
		new_name = convert_to_identifier(new_name);

	}
	while (m_nonterminal_renaming_set.find(normalizeOTN(new_name)) != m_nonterminal_renaming_set.end()) {
		new_name += "_";
	}
	return new_name;
}

std::string TIGERXMLDocHandler::normalizeOTN(const std::string& inname)
{
	std::string outname;
	str_tolower(inname, outname);
	return outname;
}
