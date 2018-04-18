/*
 * sfmimporter.h
 *
 * A class to import SIL SFM (Standard Format Marker) text files.
 * into Emdros
 * Created: 8/8-2006
 * Last update: 10/3-2011
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2011  Ulrik Sandborg-Petersen
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
