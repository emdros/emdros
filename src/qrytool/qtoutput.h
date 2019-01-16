/*
 * qtoutput.h
 *
 * Classes to do output from the query tool.
 *
 * Ulrik Petersen
 * Created: 4/16-2005
 * Last update: 1/16-2019
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



#ifndef QTOUTPUT__H__
#define QTOUTPUT__H__

// #include "TECkit/TECkit_Compiler.h"
// #include "TECkit/TECkit_Engine.h"

#include <string>
#include <set>
#include <map>
#include <emdf_value.h>
#include <monads.h>
#include <mytable.h>

// typedef std::map<std::string, TECkit_Converter> FeatureTECkitTableMap;

// typedef std::map<std::string, FeatureTECkitTableMap> OTFeatureTECkitTableMapMap;

class Configuration; // Forward declaration
class EmdrosSchema; // Forward declaration

typedef std::map<std::string,std::string> StringStringMap;
typedef std::map<std::string,int> StringIntMap;



typedef enum { ST_LATIN, ST_RASTER } eQTOutputState;

class QTOutputBase {
protected:
	bool m_bStop;
	// bool m_bHasTECkitTables;
	eQTOutputState m_state;
	eQTOutputState m_current_paragraph_state;
	int m_nInterlinearLines;

	// OTFeatureTECkitTableMapMap m_TECkitMap;

	typedef std::map<std::string, std::list<std::string> > String2StringListMap;

	StringStringMap m_data_left_boundary_map;
	StringStringMap m_data_right_boundary_map;
	String2StringListMap m_data_names;
	StringIntMap m_OTFeature2Index_map;
public:
	QTOutputBase() { 
		m_bStop = false; 
		// m_bHasTECkitTables = false; 
		m_state = ST_LATIN;
		m_current_paragraph_state = ST_LATIN; 
		m_nInterlinearLines = 1;
	};
	virtual ~QTOutputBase();
	virtual void print(const std::string& toPrint) = 0;
	virtual void startUnit(const std::string& object_type_name) = 0;
	virtual void startUnitFeatures(const std::string& object_type_name) { UNUSED(object_type_name); };
	virtual void endUnit(const std::string& object_type_name) = 0;
	virtual void endUnitFeatures(void) {};
	virtual void endSolution(void) {};
	virtual int getIndexOfFeature(const std::string& object_type_name,
				      const std::string& feature_name);
	virtual void startRaster(void);
	virtual void startLatin(void);
	virtual void startLatinParagraph(void); 

	virtual void startTree(void) {};
	virtual void addTreeTerminal(const std::string& object_type_name,
				     id_d_t object_id_d,
				     bool bLastFeatureIsParentID_D,
				     const SetOfMonads& object_som,
				     const StringVec& features,
				     bool bFocus) {
		UNUSED(object_type_name);
		UNUSED(object_id_d);
		UNUSED(bLastFeatureIsParentID_D);
		UNUSED(object_som);
		UNUSED(features);
		UNUSED(bFocus);
	};
	virtual void addTreeNonTerminal(const std::string& object_type_name,
					id_d_t object_id_d,
					bool bLastFeatureIsParentID_D,
					const SetOfMonads& object_som,
					const StringVec& features,
					bool bFocus) {
		UNUSED(object_type_name);
		UNUSED(object_id_d);
		UNUSED(bLastFeatureIsParentID_D);
		UNUSED(object_som);
		UNUSED(features);
		UNUSED(bFocus);
	};
	virtual void endTree(void) {};

	virtual int getNoOfInterlinearLines(void) { return m_nInterlinearLines; };
	virtual bool getConfigurationValues(Configuration *pConf, EmdrosSchema *pSchema, std::string& message);
	virtual void printFeature(const std::string& object_type_name,
				  unsigned int feature_index,
				  const std::string& feature_name,
				  const std::string& feature_value) = 0;
	virtual void startFocus(void) = 0;
	virtual void endFocus(void) = 0;
	virtual void newline(void) = 0;
	virtual void endl(void) = 0;
	virtual void flush(void) = 0;
	virtual void Stop(void) { m_bStop = true; };
	virtual void UnStop(void) { m_bStop = false; };

	virtual void startTable(const std::string& CSS_class) { UNUSED(CSS_class); };
	virtual void endTable(const std::string& CSS_class) { UNUSED(CSS_class); };
	virtual void startTableRow(const std::string& CSS_class) { UNUSED(CSS_class); };
	virtual void endTableRow(const std::string& CSS_class) { UNUSED(CSS_class); };
	virtual void startTableCell(const std::string& CSS_class) { UNUSED(CSS_class); };
	virtual void endTableCell(const std::string& CSS_class) { UNUSED(CSS_class); };

	// virtual std::string applyTECkit(const std::string& object_type_name, unsigned int feature_index, const std::string& feature_name, const std::string& feature_value);
	virtual bool ShouldStop(void) const { return m_bStop; };
	virtual QTOutputBase& operator<< (const std::string& toPrint) { print(toPrint); return *this; };
	virtual void MyYield(void) = 0;
 protected:
	// virtual bool compileTECkits(Configuration *pConf, const std::string& object_type_name, const std::string& feature_name, std::string& message);
};


#endif // QTOUTPUT__H__
