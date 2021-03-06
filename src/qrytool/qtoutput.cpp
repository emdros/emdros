/*
 * qtoutput.cpp
 *
 * Classes to do output from the query tool.
 *
 * Ulrik Petersen
 * Created: 5/15-2005
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


#include "qtoutput.h"
#include <string_func.h>
#include <prefix_emdros.h>
#include "conf.h"
#include "schema.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstring>
#include <cstdlib>

bool loadFile(std::string filename, std::string& outstring, std::string& message)
{
	std::ostringstream ostr;
	std::ifstream fin;
	fin.open(filename.c_str());
	if (!fin) {
		std::string newFileName = app_prefix() + filename;
		std::ifstream fin2;
		fin2.open(newFileName.c_str());
		if (!fin2) {
		        message += "ERROR: Could not open file " + filename +" for reading.\n\n";
			return false;
		} else {
	                char c;
			while (fin2.get(c)) ostr.put(c);
			fin2.close();
		}
	} else {
	        char c;
	        while (fin.get(c)) ostr.put(c);
	        fin.close();
	}
		
	outstring = ostr.str();
	return true;
}

/*
bool string2encoding(const std::string& encoding_name, unsigned int& output)
{
	std::string locase_name;
	str_tolower(encoding_name, locase_name);
	if (encoding_name == "byte"
	    || encoding_name == "bytes") {
		output = kForm_Bytes;
		return true;
	} else if (encoding_name == "unicode"
		   || encoding_name == "utf8"
		   || encoding_name == "utf-8"
		   || encoding_name == "utf_8") {
		output = kForm_UTF8;
		return true;
	} else {
		return false;
	}
}
*/

/*
void clear_OTFeatureTECkitTableMapMap(OTFeatureTECkitTableMapMap &TECkitMap)
{
	OTFeatureTECkitTableMapMap::iterator it1 = TECkitMap.begin();
	while (it1 != TECkitMap.end()) {
		FeatureTECkitTableMap &myfttmap = it1->second;
		FeatureTECkitTableMap::iterator it2 = myfttmap.begin();
		while (it2 != myfttmap.end()) {
			TECkit_DisposeConverter(it2->second);
			++it2;
		}
		++it1;
	}
}
*/

QTOutputBase::~QTOutputBase()
{
	// clear_OTFeatureTECkitTableMapMap(m_TECkitMap);
}

/*
bool QTOutputBase::compileTECkits(Configuration *pConf, const std::string& object_type_name, const std::string& feature_name, std::string& message)
{
	if (!pConf->has_key("data_feature_teckit_mapping")) {
		m_bHasTECkitTables = false;
		return true;
	} else {
		if (!pConf->has_subsubkey("data_feature_teckit_mapping", object_type_name, feature_name)) {
			return true;
		} else {
			std::string filename = pConf->getSubSubkeyValues("data_feature_teckit_mapping", object_type_name, feature_name)[0];
			std::string mapping;
			if (!loadFile(filename, mapping, message)) {
				return false;
			} 

			char *szBuf;
			szBuf = new char[mapping.length() + 1];
			strcpy(szBuf, mapping.c_str());
			bool bResult;
			UInt32 lenArrTECkit;
			Byte *arrTECkit;
			if (TECkit_CompileOpt(szBuf, (UInt32) mapping.length(),
					      0,
					      0,
					      &arrTECkit,
					      &lenArrTECkit,
					      kForm_Bytes) == kStatus_NoError) {
				bResult = true;
			} else {
				message += "Error in compiling TECkit from filename " + filename + "\n";
				bResult = false;
			}
			TECkit_Converter cnvTECkitConverter;
			if (bResult) {
				// Use these defaults
				unsigned int input_encoding = kForm_Bytes;
				unsigned int output_encoding = kForm_UTF8;
	
				if (pConf->has_subsubkey("data_feature_teckit_in_encoding", object_type_name, feature_name)) {
					std::string input_encoding_name = pConf->getSubSubkeyValues("data_feature_teckit_in_encoding", object_type_name, feature_name)[0];
					if (!string2encoding(input_encoding_name, input_encoding)) {
						message += "unknown encoding for data_feature_teckit_in_encoding with object type\n'" + object_type_name + "' and feature name '" + feature_name + "'.\nValid values are: \"byte\" or \"unicode\".";
						bResult = false;
					}
				}
	
				if (bResult 
				    && pConf->has_subsubkey("data_feature_teckit_out_encoding", object_type_name, feature_name)) {
					std::string output_encoding_name = pConf->getSubSubkeyValues("data_feature_teckit_out_encoding", object_type_name, feature_name)[0];
					if (!string2encoding(output_encoding_name, output_encoding)) {
						message += "unknown encoding for data_feature_teckit_out_encoding with object type\n'" + object_type_name + "' and feature name '" + feature_name + "'.\nValid values are: \"byte\" or \"unicode\".";
						bResult = false;
					}
				}
	
				if (bResult) {
					TECkit_Status status = TECkit_CreateConverter(arrTECkit,
										      lenArrTECkit,
										      1,  // Direction is forward
										      input_encoding,
										      output_encoding | kForm_NFC,
										      &cnvTECkitConverter);
					if (status  == kStatus_NoError) {
						bResult = true;
						m_bHasTECkitTables = true;
	    
						// Add to m_TECkitMap
						if (m_TECkitMap.find(object_type_name) == m_TECkitMap.end()) {
							FeatureTECkitTableMap ftctmap;
							m_TECkitMap[object_type_name] = ftctmap;
						}
	    
						if (m_TECkitMap[object_type_name].find(feature_name) != m_TECkitMap[object_type_name].end()) {
							message += "Error: TECkitmap for object type '" + object_type_name + "' and feature-name '" + feature_name + "' already defined.\n";
						} else {
							m_TECkitMap[object_type_name][feature_name] = cnvTECkitConverter;
						}
					}
				} else {
					message += "Error in creating TECkit converter for filename " + filename + "\n";
					bResult = false;
				}
			}
			TECkit_DisposeCompiled(arrTECkit);
			delete[] szBuf;
      
			return bResult;
		}
	}
}
*/

/*
std::string QTOutputBase::applyTECkit(const std::string& object_type_name, unsigned int feature_index, const std::string& feature_name, const std::string& feature_value)
{
	UNUSED(feature_index);
	
	if (!m_bHasTECkitTables) {
		return feature_value;
	} else {
		OTFeatureTECkitTableMapMap::const_iterator ci = m_TECkitMap.find(object_type_name);
		if (ci == m_TECkitMap.end()) {
			return feature_value;
		} else {
			FeatureTECkitTableMap::const_iterator ci2 = ci->second.find(feature_name);
			if (ci2 == ci->second.end()) {
				return feature_value;
			} else {
				// Calculate lengths
				std::string::size_type length = feature_value.length();
				std::string::size_type nInBufferLength = length + 1;
				std::string::size_type nOutBufferLength	= nInBufferLength*4 + 1;

				// Allocate and zero buffers
				Byte *szInBuffer = (Byte*) malloc(nInBufferLength);
				Byte *szOutBuffer = (Byte*) malloc(nOutBufferLength);
				memset(szInBuffer, 0, nInBufferLength);
				memset(szOutBuffer, 0, nOutBufferLength);

				// Copy feature-value to inBuffer
				memcpy(szInBuffer, feature_value.c_str(), length);
				UInt32 inUsed, outUsed, lookaheadCount;
				TECkit_Status status = 
					TECkit_ConvertBufferOpt(ci2->second,
								szInBuffer,
								(UInt32) length,
								&inUsed,
								szOutBuffer,
								(UInt32) nOutBufferLength,
								&outUsed,
								kOptionsUnmapped_UseReplacementCharWithWarning
								| kOptionsMask_InputComplete,
								&lookaheadCount); // input is complete

				std::string result;
				if (status == kStatus_NoError) {
					result = std::string((char*) szOutBuffer, outUsed);
				} else {
					result = feature_value;
				}
				free(szInBuffer);
				free(szOutBuffer);
				return result;
			}
		}
	}
}

*/

int QTOutputBase::getIndexOfFeature(const std::string& object_type_name,
				    const std::string& feature_name)
{
	std::string OTFName = object_type_name + "." + feature_name;
	if (m_OTFeature2Index_map.find(OTFName)
	    == m_OTFeature2Index_map.end()) {
		return 0;
	} else {
		// It has already been "vetted" for out-of-bounds
		// access at insertion time.
		return m_OTFeature2Index_map[OTFName];
	}
}




bool QTOutputBase::getConfigurationValues(Configuration *pConf, EmdrosSchema *pSchema, std::string& message)
{
	UNUSED(message); // Was used when we had compileTECkits
	
	bool bResult = true;

	// Number of interlinear lines
	int nInterlinearLines = 1;
	
	m_OTFeature2Index_map.clear();
	m_data_names.clear();

	if (pSchema != 0) {
		const EmdrosObjectTypes& object_types = pSchema->getObjectTypes();
		StringVec object_type_names = object_types.getObjectTypeNames();
		for (StringVec::const_iterator ot = object_type_names.begin();
		     ot != object_type_names.end();
		     ++ot) {
			std::string object_type_name;
			str_tolower(*ot, object_type_name);

			// Get data_left_boundary
			if (pConf->has_subkey("data_left_boundary", object_type_name)) {
				m_data_left_boundary_map[object_type_name] = pConf->getSubkeyValues("data_left_boundary", object_type_name)[0];
			}

			// Get data_right_boundary
			if (pConf->has_subkey("data_right_boundary", object_type_name)) {
				m_data_right_boundary_map[object_type_name] = pConf->getSubkeyValues("data_right_boundary", object_type_name)[0];
			}


			if (pConf->has_subkey("data_unit_name", object_type_name)) {
				StringVec data_unit_names = pConf->getSubkeyValues("data_unit_name", object_type_name);
				for (unsigned int index = 0; index < data_unit_names.size(); ++index) {
					std::string unit_name = data_unit_names[index];
					if (m_data_names.find(object_type_name) == m_data_names.end()) {
						m_data_names.insert(std::make_pair(object_type_name, std::list<std::string>()));
					}
					m_data_names[object_type_name].push_back(unit_name);
				}
			}

			if (pConf->has_subkey("data_feature", object_type_name)) {
				int nNoOfUnitNames = 0;
				int nLine = 0;
				if (m_data_names.find(object_type_name) != m_data_names.end()) {
					nNoOfUnitNames = (int) m_data_names[object_type_name].size();
					nLine += nNoOfUnitNames;
					
				}
				StringVec data_features = pConf->getSubkeyValues("data_feature", object_type_name);
				for (unsigned int index = 0; index < data_features.size(); ++index) {
					std::string feature_name = data_features[index];
					m_OTFeature2Index_map[object_type_name + 
							      "." + feature_name] = index + nNoOfUnitNames;

					++nLine;
					
					if (nLine > nInterlinearLines) {
						nInterlinearLines = nLine;
					}
				}
			}

			std::list<FeatureNameAndType> features 
				= object_types.getFeaturesOfObjectType(object_type_name);

			for (std::list<FeatureNameAndType>::const_iterator featntci = features.begin();
			     featntci != features.end();
			     ++featntci) {
				std::string feature_name;
				str_tolower(featntci->name, feature_name);
	
				// Compile any TECkits for this particular OT/feature-name
				/*
				bool bCompilationResult = compileTECkits(pConf, object_type_name, feature_name, message);
				if (!bCompilationResult) {
					message += "Could not compile TECkit mapping.\n";
					bResult = false;
				}
				*/
			}
		}
	}

	m_nInterlinearLines = nInterlinearLines;

	return bResult;
}


void QTOutputBase::startLatinParagraph(void)
{ 
	if (m_state != ST_LATIN) {
		m_state = ST_LATIN; 
		if (m_current_paragraph_state != ST_LATIN) {
			newline();
			m_current_paragraph_state = ST_LATIN;
		}
	} 
}

void QTOutputBase::startLatin(void)
{
	m_state = ST_LATIN;
}

void QTOutputBase::startRaster(void)
{
	if (m_state != ST_RASTER) {
		m_state = ST_RASTER;
		if (m_current_paragraph_state != ST_RASTER) {
			newline();
			m_current_paragraph_state = ST_RASTER;
		}
	}
}



