/*
 * renderobjects.h
 *
 * Classes and methods to render Emdros objects into something else,
 * based on a JSON template.
 *
 * Ulrik Sandborg-Petersen
 * Created: 4/22-2007
 * Last update: 4/16-2019
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



#ifndef RENDEROBJECTS__H__
#define RENDEROBJECTS__H__

#include <map>
#include <vector>
#include <string>
#include <list>

#include "environment_emdros.h"
#include "monads.h"
#include "memobject.h"
#include "json_classes.h"
#include "templatelang_classes.h"
#include "arena.h"


#ifndef SWIG
class MSEPObjectPair {
 public:
	const monad_m m_mse_first;
	const monad_m m_mse_last;
	MemObject *m_pObj;
        MSEPObjectPair(monad_m mse_first, monad_m mse_last, MemObject *pObj) : m_mse_first(mse_first),
		m_mse_last(mse_last),
		m_pObj(pObj) {};
	~MSEPObjectPair() {
		// If we do it here, we will have some double free's!
		// Instead, we do it in RenderObjects::clean().
		// delete m_pObj;
	};
};
#endif


class RenderObjects {
 private:
#ifndef SWIG
	typedef std::list<std::string> ListOfString;
	//typedef std::list<id_d_t> ListOfID_D;
	typedef std::list<MSEPObjectPair*> ListOfMSEPObjectPair;
	typedef std::map<long, int> Long2IntMap;
	typedef std::map<monad_m, ListOfMSEPObjectPair*> Monad2MSEObjectPairListMap;
	//typedef std::map<monad_m, ListOfID_D*> Monad2ID_DListMap;
	typedef std::vector<MemObject*> MemObjectVector;
	typedef std::map<id_d_t, MemObject*> ID_D2MemObjectMap;
	typedef std::map<long, std::string> Long2StringMap;
	typedef std::map<std::string, long> String2LongMap;
	typedef std::map<std::string, std::string> String2StringMap;
	typedef std::map<std::string, TemplateASTNode*> String2TemplateASTNodeMap;
	typedef std::map<long, TemplateASTNode*> Long2TemplateASTNodeMap;
	typedef std::map<long, std::list<std::string> > Long2StringListMap;

	Bigstring m_xml;
	EmdrosEnv *m_pEE;
	const JSONValue *m_pFetchInfo;
	const JSONValue *m_pStyleSheet;
	const JSONValue *m_pObjectTypes;

	std::string m_stylesheet;
	
	TemplateLangExecEnv *m_pTemplateEE;

	monad_m m_fm, m_lm;

	Long2StringMap m_OTNPs;
	String2LongMap m_OTN2Surrogate;
	Long2StringMap m_monad_featureMap;
	ListOfString m_OTHandlingPriorityList;
	Long2TemplateASTNodeMap m_startTemplates;
	Long2TemplateASTNodeMap m_endTemplates;
	String2TemplateASTNodeMap m_postprocessTemplates;
	Long2StringListMap m_featuresToGetMap; // OTN --> list of feature-strings
	bool m_bUseDocumentIndexFeatureInsteadOfPriorityList;
	Long2IntMap m_startPriority;
	Long2IntMap m_endPriority;
	Arena m_obj_arena;
	//ID_D2MemObjectMap m_objs;
	Arena m_MSEPObjectPair_arena;
	Monad2MSEObjectPairListMap m_fms, m_lms;
	String2StringMap m_variables;
	std::string m_db_name;
	std::string m_error_message;
	bool m_bIsOK;
	bool m_bOwnsFetchInfo;
#endif // ifndef SWIG
 public:
	RenderObjects(EmdrosEnv *pEnv, const std::string& db_name, const JSONValue *pFetchInfo, const std::string& stylesheet = "base");
	RenderObjects(EmdrosEnv *pEnv, const std::string& db_name, const std::string& JSON_stylesheet, const std::string& stylesheet = "base");
	virtual ~RenderObjects();
#ifndef SWIG
	bool process(monad_m fm, monad_m lm, const TemplateLangVariableMap& variables);
#endif
	bool process(monad_m fm, monad_m lm); // Convenience function: Empty variables
	std::string getDocument();
	void clean();
	std::string getError() { return m_error_message; };
 protected:
#ifndef SWIG
	virtual void startDocument();
	void appendError(const std::string& error);
	void calculatePriorities();
	bool readObjects();
	void objects2Document();
	void startElements(const ListOfMSEPObjectPair& mse_pobject_list, monad_m monad);
	void endElements(const ListOfMSEPObjectPair& mse_pobject_list, monad_m monad);
	void startElement(MemObject *pObj, long OTN_surrogate);
	void endElement(MemObject *pObj, long OTN_surrogate);
	bool loadObjectsFromMonads(long OTN_surrogate, const std::string& OTN, const SetOfMonads& som, const std::list<std::string>& featuresToGet);
	bool setDefaults(EmdrosEnv *pEnv, const std::string& db_name, const JSONValue *pFetchInfo, const std::string& stylesheet);
	void addToFMSandLMS(monad_m first, monad_m last, id_d_t id_d, MemObject *pObj)
	{
		UNUSED(id_d); // FIXME: Document why it is not used, or remove it.
		
		long OTN_surrogate = pObj->getObjectTypeSurrogate();
		MSEPObjectPair *pMPObj = 0;
		if (m_startTemplates[OTN_surrogate] != 0) {
			pMPObj = (MSEPObjectPair*) m_MSEPObjectPair_arena.allocate(sizeof(MSEPObjectPair));
			pMPObj = new(pMPObj) MSEPObjectPair(first, last, pObj);
			Monad2MSEObjectPairListMap::iterator itfm = m_fms.find(first);
			if (itfm == m_fms.end()) {
				//m_fms.insert(std::make_pair(first, ListOfID_D(1, id_d)));
				m_fms[first] = new ListOfMSEPObjectPair(1, pMPObj);
			} else {
				itfm->second->push_back(pMPObj);
			}
		}

		if (m_endTemplates[OTN_surrogate] != 0) {
			if (pMPObj == 0) {
				pMPObj = (MSEPObjectPair*) m_MSEPObjectPair_arena.allocate(sizeof(MSEPObjectPair));
				pMPObj = new(pMPObj) MSEPObjectPair(first, last, pObj);
			}
			Monad2MSEObjectPairListMap::iterator itlm = m_lms.find(last);
			if (itlm == m_lms.end()) {
				//m_lms.insert(std::make_pair(last, ListOfID_D(1, id_d)));
				m_lms[last] = new ListOfMSEPObjectPair(1, pMPObj);
			} else {
				itlm->second->push_back(pMPObj);
			}
		}
	};
#endif // ifndef SWIG
};

extern std::string render_objects(EmdrosEnv *pEnv, const std::string& db_name, const JSONValue *pFetchInfo, const std::string& stylesheet, monad_m first_monad, monad_m last_monad, bool& bResult);

extern std::string render_objects(EmdrosEnv *pEnv, const std::string& db_name, const std::string& JSON_stylesheet, const std::string& stylesheet, monad_m first_monad, monad_m last_monad, bool& bResult);


#endif
