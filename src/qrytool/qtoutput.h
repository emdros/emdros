/*
 * qtoutput.h
 *
 * Classes to do output from the query tool.
 *
 * Ulrik Petersen
 * Created: 4/16-2005
 * Last update: 3/1-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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

#ifndef QTOUTPUT__H__
#define QTOUTPUT__H__

#include "TECkit/TECkit_Compiler.h"
#include "TECkit/TECkit_Engine.h"

#include <string>
#include <set>
#include <map>
#include <emdf_value.h>
#include <monads.h>
#include <mytable.h>

typedef std::map<std::string, TECkit_Converter> FeatureTECkitTableMap;

typedef std::map<std::string, FeatureTECkitTableMap> OTFeatureTECkitTableMapMap;

class Configuration; // Forward declaration
class EmdrosSchema; // Forward declaration

typedef std::map<std::string,std::string> StringStringMap;
typedef std::map<std::string,int> StringIntMap;



typedef enum { ST_LATIN, ST_RASTER } eQTOutputState;

class QTOutputBase {
protected:
	bool m_bStop;
	bool m_bHasTECkitTables;
	eQTOutputState m_state;
	eQTOutputState m_current_paragraph_state;
	int m_nInterlinearLines;

	OTFeatureTECkitTableMapMap m_TECkitMap;

	typedef std::map<std::string, std::list<std::string> > String2StringListMap;

	StringStringMap m_data_left_boundary_map;
	StringStringMap m_data_right_boundary_map;
	String2StringListMap m_data_names;
	StringIntMap m_OTFeature2Index_map;
public:
	QTOutputBase() { 
		m_bStop = false; 
		m_bHasTECkitTables = false; 
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

	virtual std::string applyTECkit(const std::string& object_type_name, unsigned int feature_index, const std::string& feature_name, const std::string& feature_value);
	virtual bool ShouldStop(void) const { return m_bStop; };
	virtual QTOutputBase& operator<< (const std::string& toPrint) { print(toPrint); return *this; };
	virtual void MyYield(void) = 0;
 protected:
	virtual bool compileTECkits(Configuration *pConf, const std::string& object_type_name, const std::string& feature_name, std::string& message);
};


#endif // QTOUTPUT__H__
