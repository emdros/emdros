/*
 * sfmimporter.h
 *
 * A class to import SIL SFM (Standard Format Marker) text files.
 * into Emdros
 * Created: 8/8-2006
 * Last update: 10/3-2011
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


#ifndef SFM_IMPORTER_EMDROS_H_
#define SFM_IMPORTER_EMDROS_H_

#include "emdf.h"
#include <list>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include "monads.h"
#include "plaintextimporter.h" // For EmdrosImporterbase 

typedef std::map<std::string, std::vector<std::string> > String2StringVectorMap;
typedef std::map<std::string, std::string> String2StringMap;

class SFMImporter : public EmdrosImporterBase {
 protected:
	std::map<std::string, PEMOList> m_objects; // Maps object types to lists of objects
	String2StringVectorMap m_schema;

	// OT --> OT modifier (e.g., WITH SINGLE MONAD OBJECTS)
	String2StringMap m_OT_to_otm;

	// Vector must be in schema order.
	String2StringVectorMap m_sfm_to_record_begin;

	// Shows what OT.feature's are set by this sfm.
	String2StringVectorMap m_sfm_to_OT_slash_feature_name;

	// Current SFM values for each possible SFM.
	String2StringMap m_cur_sfm_values;
	
	// What is the object type that causes monad-increments?
	std::string m_base_object_type;
public:
	SFMImporter(monad_m starting_monad, id_d_t starting_id_d, const std::string& schema_sfm_filename);
	virtual ~SFMImporter();
	virtual bool readDocumentFromStream(std::istream *pIn);
	virtual void putSchema(std::ostream *pOut);
	virtual void putCorpusAsMQL(std::ostream *pOut);
 private:
	void cleanObjects(PEMOList& pemo_list);
	void emitMQLObjects(std::ostream *pOut, const std::list<FeatureInfo>& feature_infos, PEMOList& pemo_list, std::string object_type_name);
	void readSFMSchema(const std::string& sfm_schema_filename);
	void createObject(const std::string& object_type_name);
	void setFeature(const std::string& object_type_name,
			const std::string& feature_name, 
			const std::string& feature_value);
};


#endif
