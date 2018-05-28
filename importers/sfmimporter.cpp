/*
 * sfmimporter.cpp
 *
 * A class to import SFM files
 * into Emdros
 * Created: 8/8-2006
 * Last update: 12/26-2006
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006  Ulrik Petersen
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

#include <sfmimporter.h>
#include <sfm.h>
#include <emdf.h>
#include <list>
#include <map>
#include <sstream>
#include <iostream>
#include <string>
#include <string_func.h>
#include <fstream>
#include <vector>
#include <emdfdb.h>

typedef std::vector<std::string> StringVector;

// m_schema looks like this:
// the key std::string is an object type.
// the value is a std::vector<std::string> denoting the features.
// This vector is precisely 2*n long, where n is the number
// of features.
// Each feature at index i has the following structure:
//  - m_schema[object_type_name][i] == feature-name
//  - m_schema[object_type_name][i+1] == sfm setting the feature.
//  - m_schema[object_type_name][i+2] == "integer" or "string", 
//                                        i.e., the type of the feature.
#define NO_OF_STRINGS_PER_FEATURE (3)

    
//////////////////////////////////////////////////////////////
//
// SFMImporter
// 
//////////////////////////////////////////////////////////////

std::string strip_leading_backslash(const std::string& field_value,  const char *field_id)
{
	std::string result;
	if (field_value.length() == 0) {
		throw EmdrosException(std::string("Error: \\") + field_id + " must have a value which is not the empty string.");
	} else if (field_value.length() > 0) {
		if (field_value[0] == '\\') {
			// cut off initial backslash
			result = field_value.substr(1);
			if (field_value.length() == 0) {
				throw EmdrosException(std::string("Error: \\") + field_id + " given with only a single backslash as the value.\nThat is not allowed.");
			}
		} else {
			result = field_value;
		}
	}
	return result;
}


SFMImporter::SFMImporter(monad_m starting_monad, 
			 id_d_t starting_id_d,
			 const std::string& schema_sfm_filename)
	: EmdrosImporterBase(starting_monad, starting_id_d)
{
	readSFMSchema(schema_sfm_filename);
}

SFMImporter::~SFMImporter()
{
}

void SFMImporter::createObject(const std::string& object_type_name)
{
	if (object_type_name == m_base_object_type) {
		// Only increment if at least one has been added before
		if (!m_objects[object_type_name].empty()) {
			++m_cur_monad;
		}
	}

	EmdrosMemObject *pEMO = new EmdrosMemObject(SetOfMonads(), m_cur_id_d);
	++m_cur_id_d;
	m_objects[object_type_name].push_back(pEMO);

	// This assumes we've integrity-checked the schema!
	const std::vector<std::string>& features = m_schema[object_type_name];

	// Set all features to their default value...
	// The default value is whatever is in m_cur_sfm_values
	// for all of its sfm features.
	for (std::vector<std::string>::size_type index = 0;
	     index < features.size();
	     index += NO_OF_STRINGS_PER_FEATURE) {
		std::string feature_name = features[index];
		std::string sfm = features[index+1];
		std::string feature_value;
		if (m_cur_sfm_values.find(sfm) == m_cur_sfm_values.end()) {
			feature_value = "";
		} else {
			feature_value = m_cur_sfm_values[sfm];
		}
		setFeature(object_type_name, feature_name, feature_value);
	}

	if (object_type_name == m_base_object_type) {
		// Add current monad to all objects which are at 
		// the end of each vector of EmdrosMemObjects.
		std::map<std::string, PEMOList>::iterator it, itend;
		it = m_objects.begin();
		itend = m_objects.end();
		SetOfMonads som_cur_monad(m_cur_monad);
		while (it != itend) {
			if (it->second.size() > 0) {
				(*(it->second.rbegin()))->addMonads(som_cur_monad);
			}
			++it;
		}
	}
}


bool SFMImporter::readDocumentFromStream(std::istream *pIn)
{
	std::string big_string;
	while (true) {
		std::string line;
		getline(*pIn, line);
		big_string += line + '\n';
		
		if (pIn->eof()) {
			break;
		}
	}

	big_string = remove_char(big_string, '\r');

	StringVector line_arr;
	split_string_on_string(big_string, "\n", line_arr);
	if (line_arr.size() == 1
	    || big_string.length() == 0) {
		return true;
	} 

	StringVector::const_iterator line_ci = line_arr.begin();
	StringVector::const_iterator line_cend = line_arr.end();
	while (line_ci != line_cend) {
		std::string line = *line_ci;
		if (is_other_than_whitespace(line)
		    && line.length() > 1
		    && line[0] == '\\') {
			std::string field_id;
			std::string field_value;

			sfm_extract_id_and_value(line, field_id, field_value);

			// Cut off beginning '\\'
			field_id = field_id.substr(1);

			std::vector<std::string>::size_type index;
			String2StringVectorMap::const_iterator ci;

			// Start objects...
			ci = m_sfm_to_record_begin.find(field_id);
			if (ci != m_sfm_to_record_begin.end()) {
				for (index = 0;
				     index < ci->second.size();
				     ++index) {
					std::string object_type_to_begin_here = (ci->second)[index];
					createObject(object_type_to_begin_here);
				}
			}

			// Add object features
			ci = m_sfm_to_OT_slash_feature_name.find(field_id);
			if (ci != m_sfm_to_OT_slash_feature_name.end()) {
				m_cur_sfm_values[field_id] = field_value;
				
				const std::vector<std::string>& OT_slash_feature_vec = ci->second;
				for (index = 0;
				     index < OT_slash_feature_vec.size();
				     index += 2) {
					std::string object_type_name = 
						OT_slash_feature_vec[index];
					std::string feature_name = 
						OT_slash_feature_vec[index+1];
					if (m_objects[object_type_name].size() == 0) {
						// This just means that we have not
						// met the \\otrb or its equivalent.
					} else {
						setFeature(object_type_name, feature_name, field_value);
					}
				}
			}
		} // is_other_than_whitespace(line)
		
		++line_ci;
	}

	// We need to increment m_cur_monad because right now, it
	// is the same monad as that in the last base object added.
	++m_cur_monad;

	return true;
}



void SFMImporter::putCorpusAsMQL(std::ostream *pOut)
{
	String2StringVectorMap::const_iterator ci, cend;
	std::vector<std::string>::size_type index;

	// Build feature_infos
	ci = m_schema.begin();
	cend = m_schema.end();
	while (ci != cend) {
		std::string object_type_name = ci->first;
		const std::vector<std::string> features = ci->second;

		// Build feature_infos
		std::list<FeatureInfo> feature_infos;
		for (index = 0;
		     index < features.size();
		     index += NO_OF_STRINGS_PER_FEATURE) {
			std::string feature_name = features[index];
			std::string feature_type = features[index+2];
		       
			if (feature_type == "integer") {
				feature_infos.push_back(FeatureInfo(feature_name, 
								    FEATURE_TYPE_INTEGER,
								    "0", 
								    false));
			} else {
				feature_infos.push_back(FeatureInfo(feature_name, 
								    FEATURE_TYPE_STRING 
								    | FEATURE_TYPE_FROM_SET 
								    | FEATURE_TYPE_WITH_INDEX, 
								    "", 
								    false));
			}

		}

		// Do actual outputting of objects
		PEMOList& mylist = m_objects[object_type_name];
		emitMQLObjects(pOut, 
			       feature_infos, 
			       mylist, 
			       object_type_name);
		
		++ci;
	}
}

void SFMImporter::putSchema(std::ostream *pOut)
{
	String2StringVectorMap::const_iterator ci, cend;
	std::vector<std::string>::size_type index;

	// Print schema
	ci = m_schema.begin();
	cend = m_schema.end();
	while (ci != cend) {
		std::string object_type_name = ci->first;
		const std::vector<std::string> features = ci->second;

		(*pOut) << "CREATE OBJECT TYPE\n";

		if (m_OT_to_otm.find(object_type_name) != m_OT_to_otm.end()) {
			(*pOut) << m_OT_to_otm[object_type_name] << '\n';
		}
		
		(*pOut) << '[' << object_type_name << '\n';
		
		// Build feature_infos
		std::list<FeatureInfo> feature_infos;
		for (index = 0;
		     index < features.size();
		     index += NO_OF_STRINGS_PER_FEATURE) {
			std::string feature_name = features[index];
			std::string feature_type = features[index+2];

			if (feature_type == "integer") {
				(*pOut) << "  " << feature_name << " : integer;\n";
			} else {
				(*pOut) << "  " << feature_name << " : STRING FROM SET WITH INDEX;\n";
			}
		}

		(*pOut) << "]\nGO\n\n";
		
		++ci;
	}
}


void SFMImporter::emitMQLObjects(std::ostream *pOut, const std::list<FeatureInfo>& feature_infos, PEMOList& pemo_list, std::string object_type_name)
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


void SFMImporter::readSFMSchema(const std::string& schema_filename)
{
	std::ifstream fin(schema_filename.c_str());

	if (!fin) {
		throw EmdrosException(std::string("Could not open '") + schema_filename + "' for reading.");
	}

	// Read schema file
	std::string big_string;
	while (true) {
		std::string line;
		getline(fin, line);
		big_string += line + '\n';
		
		if (fin.eof()) {
			break;
		}
	}

	fin.close();

	big_string = remove_char(big_string, '\r');

	StringVector line_arr;
	split_string_on_string(big_string, "\n", line_arr);
	if (line_arr.size() == 1
	    || big_string.length() == 0) {
		throw EmdrosException(std::string("File '") + schema_filename + "' is empty.");
	}

	std::vector<std::pair<std::string, std::string> > keys_values;

	StringVector::const_iterator line_ci = line_arr.begin();
	StringVector::const_iterator line_cend = line_arr.end();
	while (line_ci != line_cend) {
		std::string line = *line_ci;
		if (is_other_than_whitespace(line)
		    && line.length() > 1
		    && line[0] == '\\') {
			std::string field_id;
			std::string field_value;

			sfm_extract_id_and_value(line, field_id, field_value);

			// Cut off beginning '\\'
			field_id = field_id.substr(1);

			// Strip leading and trailing whitespace
			field_value = strip(field_value);

			keys_values.push_back(std::pair<std::string,std::string>(field_id, field_value));

		}
		++line_ci;
	}

	std::vector<std::pair<std::string, std::string> >::size_type myindex;

	// Get object types
	std::string object_type_name;
	std::string feature_name;
	std::string feature_type;
	for (myindex = 0;
	     myindex < keys_values.size();
	     ++myindex) {
		std::string field_id, field_value;
		field_id = keys_values[myindex].first;
		field_value = keys_values[myindex].second;

		if (field_id == "otn") {
			object_type_name = normalizeOTName(field_value);

			// Set m_base_object_type to whatever is the current,
			// which means (since we are iterating in vector-order)
			// that the last value it'll hold is the last one in
			// the schema.
			m_base_object_type = object_type_name;

			// Initialize schema for object_type_name
			// to the empty vector.
			m_schema[object_type_name] = std::vector<std::string>();

			// Initialize the "Object Type Marker" 
			// for this object_type_name to the empty
			// string.  That way, it will always exist.
			m_OT_to_otm[object_type_name] = "";
		} else if (field_id == "otm") {
			if (object_type_name == "") {
				throw EmdrosException("Error: \\otm given before any \\otn had been given.");
			} else {
				m_OT_to_otm[object_type_name] = field_value;
			}
		} else if (field_id == "otfn") {
			if (feature_name == "") {
				str_tolower(field_value, feature_name);
			} else {
				throw EmdrosException(std::string("Error: \\otfn ") + field_value + " given after another \\otfn, without an intervening \\otfm.");
			}
			feature_type = "string";
		} else if (field_id == "integer") {
			feature_type = "integer";
		} else if (field_id == "otfm") {
			if (feature_name == "") {
				throw EmdrosException(std::string("Error: \\otfm ") + field_value + " given out of sequence, or before any \\otfn had been given.");
			} else {
				field_value = strip_leading_backslash(field_value, "otfm");

				// If map hasn't been initialized, initialize it.
				if (m_sfm_to_OT_slash_feature_name.find(field_value) == m_sfm_to_OT_slash_feature_name.end()) {
					m_sfm_to_OT_slash_feature_name[field_value] = std::vector<std::string>();
				}

				// If map hasn't been initialized, initialize it.
				if (m_sfm_to_record_begin.find(field_value) == m_sfm_to_record_begin.end()) {
					m_sfm_to_record_begin[field_value] = std::vector<std::string>();
				}

				// If map hasn't been initialized, initialize it.
				if (m_cur_sfm_values.find(field_value) == m_cur_sfm_values.end()) {
					m_cur_sfm_values[field_value] = "";
				}

				m_sfm_to_OT_slash_feature_name[field_value].push_back(object_type_name);
				m_sfm_to_OT_slash_feature_name[field_value].push_back(feature_name);

				m_schema[object_type_name].push_back(feature_name);
				m_schema[object_type_name].push_back(field_value);
				m_schema[object_type_name].push_back(feature_type);
				

				// Make it ready for the next round.
				feature_name = "";
			}
		} else {
			// Ignore all other fields.
		}
	}


	// Get \\otrb
	object_type_name = "";
	std::string feature_marker = "";
	std::string otrb = "";
	for (myindex = 0;
	     myindex < keys_values.size();
	     ++myindex) {
		std::string field_id, field_value;
		field_id = keys_values[myindex].first;
		field_value = keys_values[myindex].second;

		if (field_id == "otn") {
			if (object_type_name != "") {
				// Commit previous
				if (feature_marker == "") {
					if (otrb == "") {
						throw EmdrosException(std::string("Error: Object type '") + object_type_name + "' has neither \\otrb nor \\otfn+\\otfm. It must have at least one of these.");
					} else {
						m_sfm_to_record_begin[otrb].push_back(object_type_name);
					}
				} else {
					// Feature_marker is non-empty,
					if (otrb != "") {
						// But otrb wins...
						m_sfm_to_record_begin[otrb].push_back(object_type_name);
					} else {
						// No otrb; feature_marker wins.
						m_sfm_to_record_begin[feature_marker].push_back(object_type_name);
					}
				}
			}
			object_type_name = normalizeOTName(field_value);
			feature_marker = "";
			otrb = "";
		} else if (field_id == "otfm") {
			// Only set feature-marker if this is
			// the first in otn.
			if (feature_marker == "") {
				feature_marker = strip_leading_backslash(field_value, "otfm");
			}
		} else if (field_id == "otrb") {
			if (object_type_name == "") {
				throw EmdrosException("Error: \\otrb used before any \\otn");
			} else {
				otrb = strip_leading_backslash(field_value, "otrb");
			}
		} else {
			// Ignore all other fields.
		}
	}

	// Commit last...
	if (object_type_name != "") {
		if (feature_marker == "") {
			if (otrb == "") {
				throw EmdrosException(std::string("Error: Object type '") + object_type_name + "' has neither \\otrb nor \\otfn+\\otfm. It must have at least one of these.");
			} else {
				m_sfm_to_record_begin[otrb].push_back(object_type_name);
			}
		} else {
			// Feature_marker is non-empty,
			if (otrb != "") {
				// But otrb wins...
				m_sfm_to_record_begin[otrb].push_back(object_type_name);
			} else {
				// No otrb; feature_marker wins.
				m_sfm_to_record_begin[feature_marker].push_back(object_type_name);
			}
		}
	}
}


void SFMImporter::setFeature(const std::string& object_type_name,
			     const std::string& feature_name, 
			     const std::string& feature_value)
{
	EmdrosMemObject *pEMO = *(m_objects[object_type_name].rbegin());
	pEMO->setFeature(feature_name, feature_value);
}
