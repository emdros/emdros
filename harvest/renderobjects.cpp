/*
 * renderobjects.cpp
 *
 * Classes and functions to render Emdros objects into XML (or
 * something else), based on a JSON template.
 *
 * Ulrik Sandborg-Petersen
 * Created: 22/4-2007
 * Last update: 3/1-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2017  Ulrik Sandborg-Petersen
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

#include <renderobjects.h>
#include <string_func.h>
#include <mql_sheaf.h>
#include <emdf_value.h>
#include <algorithm>




/**
 * RenderObjects constructor, taking a JSON object instead of a
 * std::string for the pFetchInfo stylsheet.
 *
 * @param pEnv The Emdros environment to use. Must be USEing the right
 * database.
 *
 * @param db_name The database name to use when emitting the result of
 * {{ db_name }}.  Does not necessarily have to be the same as the
 * database name connected to in the pEnv parameter.
 *
 * @param pFetchInfo The JSON object to use for the stylesheets.  Can
 * either be: a) the outer JSON object containing a "fetchinfo" key
 * pointing to a JSON object containing the stylesheet given in the
 * "stylesheet" parameter, or b) can be the inner JSON object pointed
 * to by "fetchinfo", again containing the stylesheet given in the
 * "stylesheet" parameter
 *
 * @param stylesheet The stylesheet name to use within the "fetchinfo"
 * part of the JSON object passed in pFetchInfo.  By convention,
 * "base" is normally used as the default name.
 *
 */
RenderObjects::RenderObjects(EmdrosEnv *pEnv, const std::string& db_name, const JSONValue *pFetchInfo, const std::string& stylesheet)
{
	m_bOwnsFetchInfo = false;
	if (setDefaults(pEnv, db_name, pFetchInfo, stylesheet)) {
		m_bIsOK = true;
		calculatePriorities();
	} else {
		m_bIsOK = false;
	}
}



/**
 * RenderObjects constructor, taking a std::string instead of a JSON
 * object for the pFetchInfo stylsheet.
 *
 * @param pEnv The Emdros environment to use. Must be USEing the right
 * database.
 *
 * @param db_name The database name to use when emitting the result of
 * {{ db_name }}.  Does not necessarily have to be the same as the
 * database name connected to in the pEnv parameter.
 *
 * @param std::string a string which must parse to a JSON object
 * containing the stylesheets.  When parswed, can either be: a) the
 * outer JSON object containing a "fetchinfo" key pointing to a JSON
 * object containing the stylesheet given in the "stylesheet"
 * parameter, or b) can be the inner JSON object pointed to by
 * "fetchinfo", again containing the stylesheet given in the
 * "stylesheet" parameter
 *
 * @param stylesheet The stylesheet name to use within the "fetchinfo"
 * part of the JSON object passed in pFetchInfo.  By convention,
 * "base" is normally used as the default name.
 *
 */
RenderObjects::RenderObjects(EmdrosEnv *pEnv, const std::string& db_name, const std::string& JSON_stylesheet, const std::string& stylesheet)
{
	std::string error_message;
	const JSONValue *pFetchInfo = readAndParseJSONFromString(JSON_stylesheet, error_message);
	m_bOwnsFetchInfo = true;
	if (pFetchInfo == 0) {
		m_bIsOK = false;
		appendError(error_message);
	} else if (setDefaults(pEnv, db_name, pFetchInfo, stylesheet)) {
		m_bIsOK = true;
		calculatePriorities();
	} else {
		m_bIsOK = false;
	}
}




RenderObjects::~RenderObjects()
{
	clean();
	delete m_pTemplateEE;
	if (m_bOwnsFetchInfo) {
		delete m_pFetchInfo;
	}

	m_OTNPs.clear();

	m_xml.clear();

	Long2TemplateASTNodeMap::iterator templci;

	templci = m_startTemplates.begin();
	while (templci != m_startTemplates.end()) {
		delete (templci->second);
		++templci;
	}
	m_startTemplates.clear();

	templci = m_endTemplates.begin();
	while (templci != m_endTemplates.end()) {
		delete (templci->second);
		++templci;
	}
	m_endTemplates.clear();

	String2TemplateASTNodeMap::iterator templci2 = m_postprocessTemplates.begin();
	while (templci2 != m_postprocessTemplates.end()) {
		delete (templci2->second);
		++templci2;
	}
	m_postprocessTemplates.clear();
}


/** 
 * Do the rendering process.
 *
 * @param first_monad The first monad of the monad range to harvest.
 *
 * @param last_monad The last monad of the monad range to harvest.
 *
 * @return true iff the render process succeeded, false otherwise.
 */
bool RenderObjects::process(monad_m fm, monad_m lm)
{
	TemplateLangVariableMap empty_variables;
	return process(fm, lm, empty_variables);
}

/** 
 * Do the rendering process.
 *
 * @param first_monad The first monad of the monad range to harvest.
 *
 * @param last_monad The last monad of the monad range to harvest.
 *
 * @param variables A TemplateLangVariableMap (defined in
 * templatelang_classes.h), which can contain
 * variable-name-string/value-string variables which are set just
 * prior to running the stylesheet.  The values are not compiled as
 * templates, but are used as-is.  This is useful, e.g., for passing
 * parameters to the stylesheets.
 *
 * @return true iff the render process succeeded, false otherwise.
 */
bool RenderObjects::process(monad_m fm, monad_m lm, const TemplateLangVariableMap& variables)
{
	m_fm = fm;
	m_lm = lm;


	m_pTemplateEE->clean();

	if (!m_bIsOK) {
		return false;
	}


	startDocument();

	if (!readObjects()) {
		return false;
	}

	m_pTemplateEE->setVariables(variables);

	objects2Document();

	return true;
}


/**
 * Return the document created by calling
 * RenderObjects::process(monad_m, monad_m).
 *
 * Also does post-processing, i.e., executing the "postprocess" part
 * of the stylesheet, if any.
 *
 * At the end, it prepends the following string to the output:
 * 
 * "<?xml version='1.0' encoding='utf-8'?>"
 *
 * This is always done unless there exists in the stylesheet used a
 * key called "prepend_XML_declaration" which points to a JSON Boolean
 * which is false.
 *
 * Thus you can turn off this "feature" by having the following in
 * your stylesheet:
 *
 * "prepend_XML_declaration" : false,
 *
 * This must occur at the same level as the "object_types" and
 * "priority_list" and "postprocess" etc. keys.
 *
 * On error in the post_process process, an EmdrosException is thrown.
 *
 * @return The document created.
 */
std::string RenderObjects::getDocument()
{
	std::string newxml(m_xml.toString());

	Bigstring *pBigstring = new Bigstring();
	m_pTemplateEE->setBigstring(pBigstring);

	if (m_pStyleSheet->hasObjectKey("postprocess")) {
		const JSONValue *pPostprocessValue = (*m_pStyleSheet)["postprocess"];
		if (pPostprocessValue->getKind() == kJSONList) {
			const std::list<JSONValue*>& postprocess_list 
				= pPostprocessValue->getList();
			std::list<JSONValue*>::const_iterator ci;
			ci = postprocess_list.begin();
			while (ci != postprocess_list.end()) {
				const JSONValue *pPostprocessObject
					= *ci;
				if (pPostprocessObject->getKind() == kJSONObject) {
					if (pPostprocessObject->hasObjectKey("instring")
					    && pPostprocessObject->hasObjectKey("outstring")) {
						std::string instring = (*pPostprocessObject)["instring"]->getString();
						std::string outstring = (*pPostprocessObject)["outstring"]->getString();
						newxml = replace_substring(newxml, instring, outstring);

		 			} else if (pPostprocessObject->hasObjectKey("prefix")) {
						TemplateASTNode *pNode = m_postprocessTemplates["prefix"];
						pNode->exec(m_pTemplateEE);
						std::string real_prefix = m_pTemplateEE->getOutput();
						m_pTemplateEE->clearOutput();
						
						if (!real_prefix.empty()) {
							newxml = real_prefix + newxml;
						}
					} else if (pPostprocessObject->hasObjectKey("suffix")) {
						TemplateASTNode *pNode = m_postprocessTemplates["suffix"];
						pNode->exec(m_pTemplateEE);
						std::string real_suffix = m_pTemplateEE->getOutput();
						m_pTemplateEE->clearOutput();

						if (!real_suffix.empty()) {
							newxml = newxml + real_suffix;
						}
					} else {
						// std::cerr << "UP271: JSON object for 'postprocess' fetchinfo stylesheet member\ndoes not have either 'prefix' or 'suffix'." << std::endl;
					}
				} else {
					appendError("JSON value for 'postprocess' fetchinfo stylesheet member\nis not a JSON Object or a JSON String\n");
					throw EmdrosException("Error in postprocessing");
				}
				++ci;
			}
		}
	}

	// Move the m_pTemplateEE's Bigstring back to m_xml, after the
	// temporary move to the local pBigstring.
	m_pTemplateEE->setBigstring(&m_xml);

	delete pBigstring;

	bool bPrependXMLDeclaration = true;
	if (m_pStyleSheet->hasObjectKey("prepend_XML_declaration")) {
		const JSONValue *pPrependXMLDeclaration = (*m_pStyleSheet)["prepend_XML_declaration"];
		if (pPrependXMLDeclaration->getKind() == kJSONBoolean) {
			bPrependXMLDeclaration = pPrependXMLDeclaration->getBoolean();
		}
	}

	if (bPrependXMLDeclaration) {
		return "<?xml version='1.0' encoding='utf-8'?>" + newxml;
	} else {
		return newxml;

	}
}



void RenderObjects::startDocument()
{
	m_xml.clear();
}


void RenderObjects::calculatePriorities()
{
	if (!m_bUseDocumentIndexFeatureInsteadOfPriorityList) {
		int start_index = 0;
		int length = (int) m_OTHandlingPriorityList.size();
		ListOfString::const_iterator ci = m_OTHandlingPriorityList.begin();
		while (ci != m_OTHandlingPriorityList.end()) {
			std::string OTN = *ci;
			long OTN_surrogate = m_OTN2Surrogate[OTN];
			int end_index = length - start_index;
			
			m_startPriority.insert(std::make_pair(OTN_surrogate, start_index));
			m_endPriority.insert(std::make_pair(OTN_surrogate, end_index));
			
			++start_index;
			++ci;
		}
	}
}


bool RenderObjects::readObjects()
{
	Long2StringMap::const_iterator ci = m_OTNPs.begin();
	SetOfMonads som(m_fm, m_lm);

	while (ci != m_OTNPs.end()) {
		long OTN_surrogate = ci->first;
		const std::string& OTN = ci->second;

		if (!loadObjectsFromMonads(OTN_surrogate, OTN, som, m_featuresToGetMap[OTN_surrogate])) {
			return false;
		}

		++ci;
	}

	return true;
}


void RenderObjects::objects2Document()
{
	Monad2MSEObjectPairListMap::const_iterator itfm_end = m_fms.end();
	Monad2MSEObjectPairListMap::const_iterator itlm_end = m_lms.end();
	FastSetOfMonads som;

	Monad2MSEObjectPairListMap::const_iterator itfm = m_fms.begin();
	while (itfm != itfm_end) {
		som.add(itfm->first);
		++itfm;
	}

	Monad2MSEObjectPairListMap::const_iterator itlm = m_lms.begin();
	while (itlm != itlm_end) {
		som.add(itlm->first);
		++itlm;
	}
	
	FastSOMConstIterator som_ci = som.const_iterator();
	while (som_ci.hasNext()) {
		MonadSetElement mse = som_ci.next();

		monad_m monad;
		monad_m mse_last_monad = mse.last();
		for (monad = mse.first(); monad <= mse_last_monad; ++monad) {
			itfm = m_fms.find(monad);
			if (itfm != itfm_end) {
				startElements(*(itfm->second), monad);
			} 


			itlm = m_lms.find(monad);
			if (itlm != itlm_end) {
				endElements(*(itlm->second), monad);
			} 
		}
	}
}


typedef std::map<int, std::list<MemObject*> > Priority2ObjectMap;
typedef std::map<monad_m, Priority2ObjectMap > Monad2priority2objectMapMap;

void RenderObjects::startElements(const ListOfMSEPObjectPair& mse_pobject_list, monad_m monad)
{
	UNUSED(monad); // FIXME: Document why it is not used, and remove it if the reason is good.
	
	if (mse_pobject_list.empty()) {
		return;
	}
	if (mse_pobject_list.size() == 1) {
		MSEPObjectPair *pMPO = *mse_pobject_list.begin();
			
		MemObject *pObj = pMPO->m_pObj;

		startElement(pObj, pObj->getObjectTypeSurrogate());
	} else {
		if (m_bUseDocumentIndexFeatureInsteadOfPriorityList) {	
			Priority2ObjectMap priorityMap;
			
			ListOfMSEPObjectPair::const_iterator id_dci = mse_pobject_list.begin();
			while (id_dci != mse_pobject_list.end()) {	
				MSEPObjectPair *pMPO = *id_dci;
			
				MemObject *pObj = pMPO->m_pObj;
				
				int priority;
				// The document index feature is
				// always the last one (guaranteed by
				// setDefaults()).
				priority = pObj->getSpecialLong();
				
				priorityMap[priority].push_back(pObj);
				
				++id_dci;
			}

			Priority2ObjectMap::iterator priorityci = priorityMap.begin();
			while (priorityci != priorityMap.end()) {
				std::list<MemObject*>& memobjlist = priorityci->second;
				std::list<MemObject*>::iterator moit = memobjlist.begin();
				while (moit != memobjlist.end()) {
					MemObject *pObj = *moit;
					
					startElement(pObj, pObj->getObjectTypeSurrogate());
					
					++moit;
				}
				
				++priorityci;
			}
		} else {
			Monad2priority2objectMapMap startDict;
			
			ListOfMSEPObjectPair::const_iterator id_dci = mse_pobject_list.begin();
			while (id_dci != mse_pobject_list.end()) {	
				MSEPObjectPair *pMPO = *id_dci;
			
				MemObject *pObj = pMPO->m_pObj;

				long OTN_surrogate = pObj->getObjectTypeSurrogate();
				monad_m last;
				
				int priority;
				last = pMPO->m_mse_last;
				priority = m_startPriority[OTN_surrogate];
				
				startDict[last][priority].push_back(pObj);
				
				++id_dci;
			}
			
			Monad2priority2objectMapMap::reverse_iterator lmci = startDict.rbegin();
			while (lmci != startDict.rend()) {
				Priority2ObjectMap& priorityMap = lmci->second;
				Priority2ObjectMap::iterator priorityci = priorityMap.begin();
				while (priorityci != priorityMap.end()) {
					std::list<MemObject*>& memobjlist = priorityci->second;
					std::list<MemObject*>::iterator moit = memobjlist.begin();
					while (moit != memobjlist.end()) {
						MemObject *pObj = *moit;
						
						startElement(pObj, pObj->getObjectTypeSurrogate());
						
						++moit;
					}
					
					++priorityci;
				}
				++lmci;
			}
		}
	}
}


void RenderObjects::endElements(const ListOfMSEPObjectPair& mse_pobject_list, monad_m monad)
{
	UNUSED(monad); // FIXME: Document why it is not used.
	
	if (mse_pobject_list.empty()) {
		return;
	}

	if (mse_pobject_list.size() == 1) {
		MSEPObjectPair *pMPO = *mse_pobject_list.begin();
			
		MemObject *pObj = pMPO->m_pObj;

		endElement(pObj, pObj->getObjectTypeSurrogate());

		return;
	}

	if (m_bUseDocumentIndexFeatureInsteadOfPriorityList) {	
		Priority2ObjectMap priorityMap;

		// The document index feature is
		// always the last one (guaranteed by
		// setDefaults()).

		ListOfMSEPObjectPair::const_iterator id_dci = mse_pobject_list.begin();
		while (id_dci != mse_pobject_list.end()) {
			MSEPObjectPair *pMPO = *id_dci;
			
			MemObject *pObj = pMPO->m_pObj;

			long priority;
			priority = MAX_MONAD - pObj->getSpecialLong();

			priorityMap[priority].push_back(pObj);

			++id_dci;
		}

		Priority2ObjectMap::iterator priorityci = priorityMap.begin();
		while (priorityci != priorityMap.end()) {
			std::list<MemObject*>& memobjlist = priorityci->second;
			std::list<MemObject*>::iterator moit = memobjlist.begin();
			while (moit != memobjlist.end()) {
				MemObject *pObj = *moit;
				
				endElement(pObj, pObj->getObjectTypeSurrogate());
				
				++moit;
			}
			
			++priorityci;
		}
	} else {
		Monad2priority2objectMapMap endDict;

		ListOfMSEPObjectPair::const_iterator id_dci = mse_pobject_list.begin();
		while (id_dci != mse_pobject_list.end()) {
			MSEPObjectPair *pMPO = *id_dci;
			
			MemObject *pObj = pMPO->m_pObj;

			long OTN_surrogate = pObj->getObjectTypeSurrogate();
			monad_m first;
		
			int priority;
			priority = m_endPriority[OTN_surrogate];
			first = pMPO->m_mse_first;

			endDict[first][priority].push_back(pObj);

			++id_dci;
		}

		Monad2priority2objectMapMap::reverse_iterator fmci = endDict.rbegin();
		while (fmci != endDict.rend()) {
			Priority2ObjectMap& priorityMap = fmci->second;
			Priority2ObjectMap::iterator priorityci = priorityMap.begin();
			while (priorityci != priorityMap.end()) {
				std::list<MemObject*>& memobjlist = priorityci->second;
				std::list<MemObject*>::iterator moit = memobjlist.begin();
				while (moit != memobjlist.end()) {
					MemObject *pObj = *moit;

					endElement(pObj, pObj->getObjectTypeSurrogate());
				
					++moit;
				}

				++priorityci;
			}
			++fmci;
		}
	}
}


void RenderObjects::startElement(MemObject *pObj, long OTN_surrogate)
{
	TemplateASTNode *pTemplate = m_startTemplates[OTN_surrogate];
	if (pTemplate != 0) {
		m_pTemplateEE->setObject(pObj);
		pTemplate->exec(m_pTemplateEE);
		/*
		// Obsoleted by m_pTemplateEE->setBigstring(&m_xml)
		m_xml.addBigstring(m_pTemplateEE->getOutputBigstring());
		m_pTemplateEE->clearOutput();
		*/
	} 
}


void RenderObjects::endElement(MemObject *pObj, long OTN_surrogate)
{
	TemplateASTNode *pTemplate = m_endTemplates[OTN_surrogate];
	if (pTemplate != 0) {
		m_pTemplateEE->setObject(pObj);
		pTemplate->exec(m_pTemplateEE);
		/*
		// Obsoleted by m_pTemplateEE->setBigstring(&m_xml)
		m_xml.addBigstring(m_pTemplateEE->getOutputBigstring());
		m_pTemplateEE->clearOutput();
		*/
	}
}


bool RenderObjects::loadObjectsFromMonads(long OTN_surrogate, const std::string& OTN, const SetOfMonads& som, const std::list<std::string>& featuresToGet)
{
	std::string getClause;
	int nNoOfFeaturesToGet = featuresToGet.size();

	if (nNoOfFeaturesToGet > 0) {
		// Join list with ',' in between.
		std::ostringstream getClauseStream;
		getClauseStream << " GET ";
		std::list<std::string>::const_iterator ci = featuresToGet.begin();
		if (ci != featuresToGet.end()) {
			getClauseStream << *ci;
			++ci;
		}
		while (ci != featuresToGet.end()) {
			getClauseStream << ',' << *ci;
			++ci;
		}
		getClause = getClauseStream.str();
	} else {
		getClause = "";
	}

	std::string query = "GET OBJECTS HAVING MONADS IN";
	query += som.toString();
	query += "[" + OTN + getClause + "]GO";

	bool bCompilerResult;
	if (!m_pEE->executeString(query, bCompilerResult, false, false)) {
		appendError("RenderObjects::loadObjectsFromMonads: DB error executing query: " + query);
		return false;
	}
	if (!bCompilerResult) {
		appendError("RenderObjects::loadObjectsFromMonads: Query made a bad compiler result:" + query);
		return false;
	}
	if (!m_pEE->isFlatSheaf()) {
		appendError("RenderObjects::loadObjectsFromMonads: Query did not return a flat sheaf: " + query);
		return false;
	}


	FlatSheaf *pFlatSheaf = m_pEE->takeOverFlatSheaf();
	FlatSheafConstIterator flatSheafCI = pFlatSheaf->const_iterator();
	while (flatSheafCI.hasNext()) {
		FlatStraw *pFlatStraw = flatSheafCI.next();
		FlatStrawConstIterator flatStrawCI = pFlatStraw->const_iterator();
		while (flatStrawCI.hasNext()) {
			MatchedObject *pMO = flatStrawCI.next();

			id_d_t id_d = pMO->getID_D();

			monad_m first = pMO->getFirst();
			monad_m last = pMO->getLast();

			MemObject *pObj = (MemObject*) m_obj_arena.allocate(sizeof(MemObject));

			pObj = new(pObj) MemObject(nNoOfFeaturesToGet);
			
			pObj->setFirstLast(first, last);
			pObj->setObjectTypeSurrogate(OTN_surrogate);
			pObj->setID_D(id_d);
			if (nNoOfFeaturesToGet > 0) {
				int index = 0;
				while (index < nNoOfFeaturesToGet) {
					if (m_bUseDocumentIndexFeatureInsteadOfPriorityList
					    && index == nNoOfFeaturesToGet - 1) {
						pObj->setSpecialLong(pMO->getFeatureAsLong(index));
					} else {
						pObj->setFeature(index, pMO->getFeatureAsString(index));
					}

					++index;
				}
			}

			//m_objs.insert(std::make_pair(id_d, pObj));
			
			// The subscript[] operator is faster than
			// insert()!
			//m_objs[id_d] = pObj;
			if (pMO->somHasOnlyOneMSE()) {
				addToFMSandLMS(first, last, id_d, pObj);
			} else {
				// This is necessary for some strange
				// reason...  otherwise, we lose the
				// anonymous SOM somehow.
				SetOfMonads som = pMO->getMonads();
				SOMConstIterator somci = som.const_iterator();

				while (somci.hasNext()) {
					MonadSetElement mse = somci.next();
					addToFMSandLMS(mse.first(), mse.last(), id_d, pObj);

				}
			}
		}
	}

	delete pFlatSheaf;

	return true;
}


void RenderObjects::clean()
{
	/*
	MemObjectVector::iterator moit = m_obj_vec.begin();
	while (moit != m_obj_vec.end()) {
		delete *moit;
		++moit;
	}
	m_obj_vec.clear();
	*/
	ArenaConstIterator moit = m_obj_arena.const_iterator(sizeof(MemObject));
	while (moit.hasNext()) {
		MemObject *pObj = (MemObject*) moit.next();
		pObj->~MemObject();
	}
	m_obj_arena.clear();

	Monad2MSEObjectPairListMap::iterator fmit = m_fms.begin();
	while (fmit != m_fms.end()) {
		delete fmit->second;
		++fmit;
	}
	m_fms.clear();

	Monad2MSEObjectPairListMap::iterator lmit = m_lms.begin();
	while (lmit != m_lms.end()) {
		delete lmit->second;
		++lmit;
	}
	m_lms.clear();

	/*
	ListOfMSEPObjectPair::iterator mse_object_list_it = m_MSEPObjectPair_list.begin();
	while (mse_object_list_it != m_MSEPObjectPair_list.end()) {
		delete *mse_object_list_it;
		++mse_object_list_it;
	}
	m_MSEPObjectPair_list.clear();
	*/

	ArenaConstIterator msepmoit = m_MSEPObjectPair_arena.const_iterator(sizeof(MSEPObjectPair));
	while (msepmoit.hasNext()) {
		MSEPObjectPair *pObj = (MSEPObjectPair*) msepmoit.next();
		pObj->~MSEPObjectPair();
	}
	m_MSEPObjectPair_arena.clear();
	
	m_error_message = "";
}



bool RenderObjects::setDefaults(EmdrosEnv *pEnv, const std::string& db_name, const JSONValue *pFetchInfo, const std::string& stylesheet)
{
	m_pTemplateEE = new TemplateLangExecEnv();
	m_pTemplateEE->setBigstring(&m_xml);
	m_pTemplateEE->setDBName(db_name);

	m_bUseDocumentIndexFeatureInsteadOfPriorityList = false;
	m_db_name = db_name;
	m_pEE = pEnv;
	m_pFetchInfo = pFetchInfo;
	m_stylesheet = stylesheet;

	if (m_pFetchInfo->getKind() != kJSONObject) {
		appendError("fetchinfo is not a JSON object.");
		return false;
	}

	if (!m_pFetchInfo->hasObjectKey(m_stylesheet)) {
		if (m_pFetchInfo->hasObjectKey("fetchinfo")) {
			m_pFetchInfo = (*m_pFetchInfo)["fetchinfo"];

			if (m_pFetchInfo->getKind() != kJSONObject) {
				appendError("fetchinfo is not a JSON object.");
				return false;
			}
		} else {
			appendError("The outer stylesheet must have a \"fetchinfo\" key.");
			return false;
		}
	}

	if (!m_pFetchInfo->hasObjectKey(m_stylesheet)) {
		
		appendError(std::string("fetchinfo does not have key '") + m_stylesheet + "'.");
		return false;
	}

	m_pTemplateEE->setTopStylesheet(pFetchInfo);
	m_pStyleSheet = (*m_pFetchInfo)[m_stylesheet];

	bool bResult = true;

	if (m_pStyleSheet->hasObjectKey("priority_list")) {
		m_bUseDocumentIndexFeatureInsteadOfPriorityList = false;
	} else {
		m_bUseDocumentIndexFeatureInsteadOfPriorityList = true;
	}

	if (!m_pStyleSheet->hasObjectKey("object_types")) {
		appendError(std::string("stylesheet does not have key 'object_types'."));
		return false;
	}

	m_pObjectTypes = (*m_pStyleSheet)["object_types"];

	if (m_pObjectTypes == 0
	    || m_pObjectTypes->getKind() != kJSONObject) {
		appendError(std::string("stylesheet's \"object_types\" value is not a JSON Object, as it should be."));
		return false;
	}

	// Handle m_pObjectTypes JSON Object
	std::list<std::string> OTN_list = m_pObjectTypes->getObjectKeys();
	std::list<std::string>::const_iterator otnci = OTN_list.begin();
	long OTN_surrogate = 0;
	while (otnci != OTN_list.end()) {
		std::string OTN = *otnci;

		m_OTNPs.insert(std::make_pair(OTN_surrogate, OTN));
		m_OTN2Surrogate.insert(std::make_pair(OTN, OTN_surrogate));
		
		if (!m_pObjectTypes->hasObjectKey(OTN)) {
			appendError("Error: \"object_types\" object does not have object type name\n\"" + OTN + "\", even though it is in \"priority_list\".");
			bResult = false;
		} else {
			const JSONValue *pOTNObject = (*m_pObjectTypes)[OTN];
				
			std::string start;
			if (pOTNObject->hasObjectKey("start")) {
				const JSONValue *pStart = (*pOTNObject)["start"];
				if (pStart->getKind() != kJSONString) {
					appendError("Error: for object type '" + OTN + "', \"start\" does not point to a string.");
					bResult = false;
				} else {
					start = (*pOTNObject)["start"]->getString();
				}
			}
			
			if (start.empty()) {
				m_startTemplates.insert(std::make_pair(OTN_surrogate, (TemplateASTNode*)0));
			} else {
				bool bCompileResult;
				TemplateASTNode *pNode = parseTemplate(m_pTemplateEE, start, bCompileResult);
				if (!bCompileResult) {
					appendError("Error: parsing template 'start' for object type '" + OTN + "':\nparsing of template failed with the following error:\n" + m_pTemplateEE->getError());
					bResult = false;
				}
				m_startTemplates.insert(std::make_pair(OTN_surrogate, (TemplateASTNode*)pNode));
			}
			
			std::string end;
			if (pOTNObject->hasObjectKey("end")) {
				const JSONValue *pEnd = (*pOTNObject)["end"];
				if (pEnd->getKind() != kJSONString) {
					appendError("Error: for object type '" + OTN + "', \"end\" does not point to a string.");
					bResult = false;
				} else {
					end = (*pOTNObject)["end"]->getString();
				}
			}
			
			if (end.empty()) {
				m_endTemplates.insert(std::make_pair(OTN_surrogate, (TemplateASTNode*)0));
			} else {
				bool bCompileResult;
				TemplateASTNode *pNode = parseTemplate(m_pTemplateEE, end, bCompileResult);
				if (!bCompileResult) {
					appendError("Error: parsing template 'end' for object type '" + OTN + "':\nparsing of template failed with the following error:\n" + m_pTemplateEE->getError());
					bResult = false;
				}
				m_endTemplates.insert(std::make_pair((long)OTN_surrogate, (TemplateASTNode*) pNode));

			}

			std::list<std::string> featuresToGet;
			const JSONValue *pGetList = 0;
			if (pOTNObject->hasObjectKey("get")) {
				pGetList = (*pOTNObject)["get"];
			}

			if (pGetList != 0) {
				if (pGetList->getKind() != kJSONList) {
					appendError("Error: for object type '" + OTN + "', \"get\" is not a list.");
					pGetList = 0;
					bResult = false;
				}
				if (pGetList != 0) {
					const std::list<JSONValue*>& getlist = pGetList->getList();
					std::list<JSONValue*>::const_iterator ci = getlist.begin();
					while (ci != getlist.end()) {
						const JSONValue *pGetString = *ci;
						if (pGetString->getKind() != kJSONString) {
							appendError("Error: for object type '" + OTN + "', not all members of the \"get\" list are strings.");
							bResult = false;
						} else {
							featuresToGet.push_back(pGetString->getString());
						}
						++ci;
					}

				}
			}

			if (m_bUseDocumentIndexFeatureInsteadOfPriorityList) {
				const JSONValue *pDocIndexFeature = 0;
				if (pOTNObject->hasObjectKey("docindexfeature")) {
					pDocIndexFeature = (*pOTNObject)["docindexfeature"];
				}
				
				if (pDocIndexFeature == 0) {
					appendError(std::string("stylesheet's object type \"") + OTN + std::string("\" does not have 'docindexfeature' member, even though priority_list is absent."));
					bResult = false;
				} else if (pDocIndexFeature->getKind() != kJSONString) {
					appendError("Error: for object type '" + OTN + "', docindexfeature member is not a string");
					bResult = false;
				} else {
					featuresToGet.push_back(pDocIndexFeature->getString());
				}
			}
		

			m_featuresToGetMap.insert(std::make_pair(OTN_surrogate, featuresToGet));
		}

		++otnci;
		++OTN_surrogate;
	}


	if (!m_bUseDocumentIndexFeatureInsteadOfPriorityList) {
		const JSONValue *pPriorityList = (*m_pStyleSheet)["priority_list"];

		if (pPriorityList->getKind() != kJSONList) {
			appendError(std::string("stylesheet's \"prioritylist\" value is not a list, as it should be."));
			return false;
		}
		
		const std::list<JSONValue*>& priority_list = pPriorityList->getList();
		std::list<JSONValue*>::const_iterator plci = priority_list.begin();
		while (plci != priority_list.end()) {
			const JSONValue *pOTValue = *plci;
			
			if (pOTValue->getKind() != kJSONString) {
				std::ostringstream ostr;
				pOTValue->pretty(&ostr, 0); // The 0 is the indent level.
				appendError(std::string("stylesheet's \"prioritylist\" has a value which is not a JSON String.\nThe value looks like this:\n") + ostr.str());
				bResult = false;
			} else {
				std::string OTN = pOTValue->getString();

				m_OTHandlingPriorityList.push_back(OTN);
			}
			++plci;
		}
	}

	if (m_pStyleSheet->hasObjectKey("postprocess")) {
		const JSONValue *pPostprocessValue = (*m_pStyleSheet)["postprocess"];
		if (pPostprocessValue->getKind() == kJSONList) {
			const std::list<JSONValue*>& postprocess_list 
				= pPostprocessValue->getList();
			std::list<JSONValue*>::const_iterator ci;
			ci = postprocess_list.begin();
			while (ci != postprocess_list.end()) {
				const JSONValue *pPostprocessObject
					= *ci;
				if (pPostprocessObject->getKind() == kJSONObject) {

		 			if (pPostprocessObject->hasObjectKey("prefix")) {
						if (m_postprocessTemplates.find("prefix") != m_postprocessTemplates.end()) {
							appendError("Error: parsing template 'prefix':\nYou can have at most 1 prefix template, and you have got more than one.\n");
							bResult = false;

						} else {
							std::string uncompiled_prefix = (*pPostprocessObject)["prefix"]->getString();

							bool bCompileResult = false;
							TemplateASTNode *pNode = parseTemplate(m_pTemplateEE, uncompiled_prefix, bCompileResult);
							if (!bCompileResult) {
								appendError("Error: parsing template 'prefix':\nparsing of template failed with the following error:\n" + m_pTemplateEE->getError());
								bResult = false;

							} else {
								m_postprocessTemplates.insert(std::make_pair(std::string("prefix"), (TemplateASTNode*) pNode));
							}
						}
					} else if (pPostprocessObject->hasObjectKey("suffix")) {
						if (m_postprocessTemplates.find("suffix") != m_postprocessTemplates.end()) {
							appendError("Error: parsing template 'suffix':\nYou can have at most 1 suffix template, and you have got more than one.\n");
							bResult = false;
						} else {
							std::string uncompiled_suffix = (*pPostprocessObject)["suffix"]->getString();
							
							bool bCompileResult = false;
							TemplateASTNode *pNode = parseTemplate(m_pTemplateEE, uncompiled_suffix, bCompileResult);
							if (!bCompileResult) {
								appendError("Error: parsing template 'suffix':\nparsing of template failed with the following error:\n" + m_pTemplateEE->getError());
								bResult = false;
							} else {
								m_postprocessTemplates.insert(std::make_pair(std::string("suffix"), (TemplateASTNode*) pNode));
							}
						}
					} else {
						// std::cerr << "UP271: JSON object for 'postprocess' fetchinfo stylesheet member\ndoes not have either 'prefix' or 'suffix'." << std::endl;
					}
				} else {
					appendError("JSON value for 'postprocess' fetchinfo stylesheet member\nis not a JSON Object\n");
					bResult = false;
				}
				++ci;
			}
		}
	}


	return bResult;
}

void RenderObjects::appendError(const std::string& error)
{
    m_error_message += error;
    m_error_message += '\n';
}


/**
 * Render objects using the RenderObjects class.
 *
 * - Create a RenderObjects object.
 *
 * - Call RenderObjects::process() with the monads given.
 *
 * - Call RenderObjects::getDocument().
 *
 * - Return the document (or error message).
 *
 *
 * @param pEnv The Emdros environment to use. Must be USEing the right
 * database.
 *
 * @param db_name The database name to use when emitting the result of
 * {{ db_name }}.  Does not necessarily have to be the same as the
 * database name connected to in the pEnv parameter.
 *
 * @param pFetchInfo The JSON object to pass to the RenderObjects
 * constructor.
 *
 * @param stylesheet The stylesheet name to use within the "fetchinfo"
 * part of the JSON object passed in pFetchInfo.  By convention,
 * "base" is normally used as the default name.
 *
 * @param first_monad The first monad of the monad range to harvest.
 *
 * @param last_monad The last monad of the monad range to harvest.
 *
 * @param bResult An output parameter.  On return, this is true if and
 * only if the harvesting process succeeded.
 *
 * @return If bResult is true, then this contains the document fetched
 * by RenderObjects::getDocument after the RenderObjects::process()
 * call.  If bResult is false, then an error message may be returned.
 * 
 */
std::string render_objects(EmdrosEnv *pEnv, const std::string& db_name, const JSONValue *pFetchInfo, const std::string& stylesheet, monad_m first_monad, monad_m last_monad, bool& bResult)
{
	bResult = false;
	RenderObjects *pRO = 0;
	try {
		pRO = new RenderObjects(pEnv, db_name, pFetchInfo, stylesheet);
		if (!pRO->process(first_monad, last_monad)) {
			throw EmdrosException(pRO->getError());
		} else {
			bResult = true;
			std::string result = pRO->getDocument();
			delete pRO;
			return result;
		}
	} catch (EmdrosException e) {
		delete pRO;
		bResult = false;
		return e.what();
	}
}

/**
 * Render objects using the RenderObjects class.
 *
 * The following steps are performed:
 *
 * - parse the JSON_stylesheet.
 *
 * - Create a RenderObjects object.
 *
 * - Call RenderObjects::process() with the monads given.
 *
 * - Call RenderObjects::getDocument().
 *
 * - Return the document (or error message).
 *
 * @param pEnv The Emdros environment to use. Must be USEing the right
 * database.
 *
 * @param db_name The database name to use when emitting the result of
 * {{ db_name }}.  Does not necessarily have to be the same as the
 * database name connected to in the pEnv parameter.
 *
 * @param JSON_stylesheet The JSON stylesheet, expressed as a string,
 * to be passed to the RenderObjects constructor.
 *
 * @param stylesheet The stylesheet name to use within the "fetchinfo"
 * part of the JSON object passed in JSON_stylesheet.  By convention,
 * "base" is normally used as the default name.
 *
 * @param first_monad The first monad of the monad range to harvest.
 *
 * @param last_monad The last monad of the monad range to harvest.
 *
 * @param bResult An output parameter.  On return, this is true if and
 * only if the harvesting process succeeded.
 *
 * @return If bResult is true, then this contains the document fetched
 * by RenderObjects::getDocument after the RenderObjects::process()
 * call.  If bResult is false, then an error message may be returned.
 * 
 */
std::string render_objects(EmdrosEnv *pEnv, const std::string& db_name, const std::string& JSON_stylesheet, const std::string& stylesheet, monad_m first_monad, monad_m last_monad, bool& bResult)
{
	std::string error_message;
	const JSONValue *pFetchInfo = readAndParseJSONFromString(JSON_stylesheet, error_message);
	if (pFetchInfo == 0) {
		bResult = false;
		return "Could not parse JSON stylesheet. Error message from JSON parser:\n" + error_message;
	} else {
		std::string result = render_objects(pEnv, db_name, pFetchInfo, stylesheet, first_monad, last_monad, bResult);
		delete pFetchInfo;
		return result;
	}
}
