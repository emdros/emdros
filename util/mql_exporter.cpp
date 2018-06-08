/*
 * mql_exporter.cpp
 *
 * A class to dump Emdros databases in MQL
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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


#include <encryption.h>
#include <sstream>
#include <fstream>
#include <inst.h>
#include <debug.h>
#include <monads.h>
#include <emdfdb.h>
#include <emdf_output.h>
#include <mql_execution_environment.h>
#include <inst_object.h>
#include <mql_exporter.h>


MQLExporter::MQLExporter(const MQLExportEnv& de, EmdrosEnv *pEE, std::ostream* pOutstr) 
	: m_de(de),
	  m_pEE(pEE),
	  m_ostr(pOutstr),
	  m_filename("")
{
}

MQLExporter::MQLExporter(const MQLExportEnv& de, EmdrosEnv *pEE, std::string filename) // Dump to file
	: m_de(de),
	  m_pEE(pEE),
	  m_ostr(&std::cout), // This will be changed in doIt().
	  m_filename(filename)
{
}


MQLExporter::MQLExporter(const MQLExportEnv& de, EmdrosEnv *pEE) // Dump to stdout
	: m_de(de),
	  m_pEE(pEE),
	  m_ostr(&std::cout),
	  m_filename("-")
{
}


MQLExporter::~MQLExporter()
{
}


bool MQLExporter::ExecuteQuery(std::string query, bool& bCompilerResult) 
{
	return m_pEE->executeString(query, bCompilerResult, false, true);
}

#define EXECUTE_QUERY(query) {					\
		if (!ExecuteQuery(query, bCompilerResult)) {	\
			return false;				\
		} else {					\
			if (!bCompilerResult) {			\
				return true;			\
			}					\
		}						\
	}

bool MQLExporter::GetObjectTypeId(std::string object_type_name, id_d_t& object_type_id, eObjectRangeType& objectRangeType, eMonadUniquenessType& monadUniquenessType)
{
	// Get object type id
	bool bObjectTypeExists;
	if (!m_pEE->getMQLEE()->pDB->objectTypeExists(object_type_name,
						      bObjectTypeExists,
						      object_type_id,
						      objectRangeType,
						      monadUniquenessType)) {
		return false;
	} else {
		if (!bObjectTypeExists) {
			std::cerr << "ERROR: Object type " << object_type_name << " does not exist." << std::endl;
			return false;
		}
	}
  
	// If we got this far, there were no DB errors, and the object type existed
	return true;
}

bool MQLExporter::GetEnumConst(emdros_int64 value, const std::string& enum_name, std::string& outstr)
{
	bool bDBOK = false;
	outstr= m_pEE->getEnumConstNameFromValue(value,
						 enum_name,
						 bDBOK);
	if (!bDBOK) {
		std::cerr << "ERROR getting enumeration constant with value " << value << std::endl;
		std::cerr << "from enumeration " << enum_name << std::endl;
		return false;
	}

	// If we got this far, there were no DB errors
	return true;
}

bool MQLExporter::getObjectTypes(bool& bCompilerResult)
{
	// Do we have any object types from the command line?
	if (!m_de.object_types.empty()) {
		// Yes.  Check that they exist
		std::list<std::string>::const_iterator ci = m_de.object_types.begin();
		while (ci != m_de.object_types.end()) {
			// Check to see that OT exists

			bool bObjectTypeExists;
			id_d_t dummy_id_d;
			eObjectRangeType dummyObjectRangeType;
			eMonadUniquenessType monadUniquenessType;
			// Ask DB
			if (!m_pEE->getMQLEE()->pDB->objectTypeExists(*ci, 
								      bObjectTypeExists, 
								      dummy_id_d,
								      dummyObjectRangeType,
								      monadUniquenessType)) {
				return false;
			} else {
				// No DB error.
				// Does it exist?
				if (!bObjectTypeExists) {
					// No. Report error
					std::cerr << "ERROR: Object type " << *ci << " does not exist." << std::endl;
					return false;
				}
			}

			// Advance iterator
			ci++;
		}
	} else {
		// No object types from the command line.

		// Get list from DB

		// Execute SELECT OBJECT TYPES query
		std::string query;
		query = "SELECT OBJECT TYPES GO";
		EXECUTE_QUERY(query);

		// Check for sanity of result
		ASSERT_THROW(m_pEE->isTable(), "m_pEE's result is not a Table.");

		// Get result table
		Table *pTable = m_pEE->takeOverTable();

		// Iterate over result
		TableIterator it = pTable->iterator();
		while (it.hasNext()) {
			// Add contents of column 1
			m_de.object_types.push_back(it.next(1));
		}

		// Clean up (delete table)
		delete pTable;
	}

	// If we got this far, then all object type names exist in the DB.
	return true;
}


void MQLExporter::DumpCreateAndUseDB() {
	if (m_de.bVerbose) {
		std::cerr << "Dumping CREATE DATABASE and USE DATABASE..." << std::endl;
	}
	(*m_ostr) << "// Create database\n";
	(*m_ostr) << "CREATE DATABASE '" << m_de.db << "'\n";
	if (m_de.bDumpEncoding) {
		eCharsets charset;
		if (string2charset(m_de.encoding,charset)) {
			(*m_ostr) << "USING ENCODING '" << m_de.encoding << "'\n";
		} else {
			std::cerr << "Warning: Could not dump encoding '" << m_de.encoding << "', since it\nis not a valid charset." << std::endl;
		}
	}
	(*m_ostr) << "GO\n\n";
	(*m_ostr) << "// Switch to using database\n";
	(*m_ostr) << "USE DATABASE '" << m_de.db << "' GO\n\n";
}

bool MQLExporter::DumpArbitraryMonadSet(const std::string& monad_set_name, const SetOfMonads& som, bool& bCompilerResult)
{
	UNUSED(bCompilerResult);
	
	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << ">> Dumping monad set " << monad_set_name << "..." << std::flush;
	}

	// Build and execute query
	(*m_ostr) << "CREATE MONAD SET\n"
	     << monad_set_name << "\n"
	     << "WITH MONADS = " << som.toString() << "\n"
	     << "GO\n\n";

	if (m_de.bVerbose) {
		std::cerr << "done!" << std::endl;
	}

	// If we got this far, there were no DB errors
	return true;
}

bool MQLExporter::DumpArbitraryMonadSets(bool& bCompilerResult) 
{
	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << "Dumping arbitrary monad sets " << std::endl;
	}

	// Build and execute query
	std::string query = "GET MONAD SETS ALL GO";
	EXECUTE_QUERY(query);

	// Assert that it is a table
	ASSERT_THROW(m_pEE->isTable(), "m_pEE's result is not a Table.");

	// Iterate through table
	std::map<std::string, SetOfMonads> mymap;
	Table *pTable = m_pEE->takeOverTable();
	TableIterator it = pTable->iterator();
	if (it.hasNext()) {
		(*m_ostr) << "\n\n";
		(*m_ostr) << "//////////////////////////////////////////////////// \n";
		(*m_ostr) << "// \n";
		(*m_ostr) << "// Create arbitrary monad sets\n";
		(*m_ostr) << "// \n";
		(*m_ostr) << "//////////////////////////////////////////////////// \n\n";
	}
	while (it.hasNext()) {
		// Get monad set name
		std::string monad_set_name = it.getColumn(1);
    
		// Get mse_first
		monad_m mse_first = string2monad_m(it.getColumn(2));

		// Get mse_last
		monad_m mse_last = string2monad_m(it.getColumn(3));

		if (mymap.find(monad_set_name) == mymap.end()) {
			mymap[monad_set_name] = SetOfMonads(mse_first, mse_last);
		} else {
			mymap[monad_set_name].add(mse_first, mse_last);
		}

		// Advance iterator
		it.next();
	}

	// Clean up (delete table)
	delete pTable;


	// Traverse map
	std::map<std::string, SetOfMonads>::const_iterator ci = mymap.begin();
	std::map<std::string, SetOfMonads>::const_iterator cend = mymap.end();
	for (; ci != cend; ++ci) {
		if (!DumpArbitraryMonadSet(ci->first, ci->second, bCompilerResult)) {
			// DB error
			if (m_de.bVerbose) {
				std::cerr << "failure!" << std::endl;
			}
			return false;
		} else {
			// No DB error.  Compiler error?
			if (!bCompilerResult) {
				if (m_de.bVerbose) {
					std::cerr << "failure!" << std::endl;
				}
				return true;
			}
		}
	}

	if (m_de.bVerbose) {
		std::cerr << "Done dumping arbitrary monad sets!" << std::endl;
	}

	// If we got this far, there were no DB errors
	return true;
}

bool MQLExporter::DumpEnum(std::string enum_name, bool& bCompilerResult) 
{
	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << ">> Dumping enumeration " << enum_name << "..." << std::flush;
	}

	// Build and execute query
	std::string query = std::string("SELECT ENUMERATION CONSTANTS\n")
		+ "FROM ENUM " + enum_name + " GO";
	EXECUTE_QUERY(query);

	// Assert that it is a table
	ASSERT_THROW(m_pEE->isTable(), "m_pEE's result is not a Table.");

	// Emit pre-constant material
	(*m_ostr) << "// Enumeration " << enum_name << '\n';
	(*m_ostr) << "CREATE ENUMERATION " << enum_name << " = {\n";

	// Iterate through table
	Table *pTable = m_pEE->takeOverTable();
	TableIterator it = pTable->iterator();

	while (it.hasNext()) {
		// Get enum constant info info
		std::string enum_const_name = it.getColumn(1);
		emdros_int64 value = string2emdros_int64(it.getColumn(2));
		bool is_default = string2bool_alpha(it.getColumn(3));

		// Write info to ostr
		(*m_ostr) << "  ";
		if (is_default) {
			(*m_ostr) << "DEFAULT ";
		}
		(*m_ostr) << enum_const_name << " = " << value;

		// Advance iterator
		it.next();

		// If there are more, then emit ","
		if (it.hasNext()) {
			(*m_ostr) << ",";
		}

		// Emit trailing newline
		(*m_ostr) << '\n';
	}
   
	// Emit post-constant material
	(*m_ostr) << "}\n" << "GO\n\n";

	// Clean up (delete table)
	delete pTable;

	if (m_de.bVerbose) {
		std::cerr << "done!" << std::endl;
	}

	// If we got this far, there were no DB errors
	return true;
}

bool MQLExporter::DumpEnums(bool& bCompilerResult) 
{
	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << "Dumping enumerations " << std::endl;
	}

	// Build and execute query
	std::string query = "SELECT ENUMERATIONS GO";
	EXECUTE_QUERY(query);

	// Assert that it is a table
	ASSERT_THROW(m_pEE->isTable(), "m_pEE's result is not a Table.");

	// Iterate through table
	Table *pTable = m_pEE->takeOverTable();
	TableIterator it = pTable->iterator();
	if (it.hasNext()) {
		(*m_ostr) << "\n\n";
		(*m_ostr) << "//////////////////////////////////////////////////// \n";
		(*m_ostr) << "// \n";
		(*m_ostr) << "// Create enumerations\n";
		(*m_ostr) << "// \n";
		(*m_ostr) << "//////////////////////////////////////////////////// \n\n";
	}
	while (it.hasNext()) {
		// Get enumeration name
		std::string enum_name = it.getColumn(1);

		if (!DumpEnum(enum_name, bCompilerResult)) {
			// DB error
			if (m_de.bVerbose) {
				std::cerr << "failure!" << std::endl;
			}
			return false;
		} else {
			// No DB error.  Compiler error?
			if (!bCompilerResult) {
				if (m_de.bVerbose) {
					std::cerr << "failure!" << std::endl;
				}
				return true;
			}
		}


		// Advance iterator
		it.next();
	}

	// Clean up (delete table)
	delete pTable;

	if (m_de.bVerbose) {
		std::cerr << "Done dumping enums!" << std::endl;
	}

	// If we got this far, there were no DB errors
	return true;
}

bool MQLExporter::DumpObjectType(std::string object_type_name, bool& bCompilerResult) 
{
	UNUSED(bCompilerResult);
	
	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << ">> Dumping object type " << object_type_name << "..." << std::flush;
	}

	// Get object type id
	id_d_t object_type_id;
	eObjectRangeType objectRangeType;
	eMonadUniquenessType monadUniquenessType;
	if (!GetObjectTypeId(object_type_name, object_type_id, objectRangeType, monadUniquenessType)) {
		// Error has already been reported
		return false;
	}

	// Get features
	std::list<FeatureInfo> FeatureInfos;
	if (!m_pEE->getMQLEE()->pDB->getFeaturesForObjectType(object_type_id,
							      FeatureInfos)) {
		std::cerr << std::endl << "ERROR: Could not get features for object type " << object_type_name << std::endl;
		return false;
	}

	// Emit pre-feature material
	(*m_ostr) << "// Object type " << object_type_name << '\n';
	(*m_ostr) << "CREATE OBJECT TYPE\n";
	switch (objectRangeType) {
	case kORTMultipleRange:
		break;
	case kORTSingleRange:
		(*m_ostr) << "WITH SINGLE RANGE OBJECTS\n";
		break;
	case kORTSingleMonad:
		(*m_ostr) << "WITH SINGLE MONAD OBJECTS\n";
		break;
	default:
		ASSERT_THROW(false, "Unknown Object Range Type");
		break;
	}
	switch (monadUniquenessType) {
	case kMUTNonUniqueMonads:
		break;
	case kMUTUniqueFirstMonads:
		(*m_ostr) << "HAVING UNIQUE FIRST MONADS\n";
		break;
	case kMUTUniqueFirstAndLastMonads:
		(*m_ostr) << "HAVING UNIQUE FIRST AND LAST MONADS\n";
		break;
	default:
		ASSERT_THROW(false, "Unknown Monad Uniqueness Type");
		break;
	}
	(*m_ostr) << "[" << object_type_name << '\n';

	// Iterate through list of FeatureInfos
	std::list<FeatureInfo>::const_iterator ci(FeatureInfos.begin());
	std::list<FeatureInfo>::const_iterator cend(FeatureInfos.end());
	while (ci != cend) {
		// Name
		std::string feature_name = ci->getName();

		// Type
		id_d_t feature_type_id = ci->getType();
		std::string strType_id;
		bool bTypeIDGottenOK = m_pEE->getMQLEE()->pDB->typeIdToString(feature_type_id, strType_id);
		if (!bTypeIDGottenOK) {
			std::cerr << std::endl << "ERROR: Could not get string corresponding to feature " << feature_name << std::endl;
			std::cerr << "from object type " << object_type_name << std::endl;
			return false;
		}

		// FROM SET
		bool bIsFromSet = featureTypeIdIsFromSet(feature_type_id);
		std::string strFromSet = "";
		if (bIsFromSet) {
			strFromSet = " FROM SET ";
		} else {
			strFromSet = "";
		}

		// WITH INDEX
		bool bIsWithIndex = featureTypeIdIsWithIndex(feature_type_id);
		std::string strWithIndex = "";
		if (bIsWithIndex) {
			strWithIndex = " WITH INDEX ";
		} else {
			strWithIndex = "";
		}

		// Default value
		bool bAddDefault = true;
		std::string defval;
		std::string defval_unencoded = ci->getDefaultValue();

		// Get default value based on feature type
		if (featureTypeIdIsListOf(feature_type_id)) {
			bAddDefault = false;
		} else if (featureTypeIdIsINTEGER(feature_type_id)
			   || featureTypeIdIsID_D(feature_type_id)) {
			defval = defval_unencoded;
		} else if (featureTypeIdIsASCII(feature_type_id)
			   || featureTypeIdIsSTRING(feature_type_id)) {
			// This is a STRING or ASCII string
			defval = std::string("\"") + encodeSTRINGstring(defval_unencoded) + "\"";
		} else if (featureTypeIdIsSOM(feature_type_id)) {
			// Don't add DEFAULT if it is a SOM, and the
			// DEFAULT string is an empty SOM.
			std::list<std::string> tmp_list;
			split_string(defval_unencoded, " ", tmp_list);
			std::string tmp_str = joinList("", tmp_list);
			if (tmp_str == "{}") {
				bAddDefault = false;
			} else {
				bAddDefault = true;
				defval = defval_unencoded;
			}
		} else {
			defval = defval_unencoded;
		}

		// Is_computed: Only added here for future use
		// bool is_computed = string2bool(bool_alpha2string(ci->getIsComputed()));

		// Emit feature if it isn't self
		if (feature_name != "self") {
			(*m_ostr) << "  " << feature_name 
			     << " : " << strType_id
			     << strFromSet 
			     << strWithIndex;
			if (bAddDefault) {
				(*m_ostr) << " DEFAULT " << defval;
			}
			(*m_ostr) << ";\n";
		}

		// Advance iterator
		++ci;
	}

	// Emit post-feature material
	(*m_ostr) << "]\n" << "GO\n\n";

	// Emit trailing "done!" if verbose
	if (m_de.bVerbose) {
		std::cerr << "done!" << std::endl;
	}

	// If we got this far, there were no DB errors
	return true;
}


bool MQLExporter::DumpObjectTypes(bool& bCompilerResult) 
{
	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << "Dumping object types..." << std::endl;
	}

	// Emit pre-object types material
	(*m_ostr) << "\n\n";
	(*m_ostr) << "//////////////////////////////////////////////////// \n";
	(*m_ostr) << "// \n";
	(*m_ostr) << "// Create object types\n";
	(*m_ostr) << "// \n";
	(*m_ostr) << "//////////////////////////////////////////////////// \n\n";

	// Iterate through m_de.object_types
	std::list<std::string>::const_iterator ci;
	ci = m_de.object_types.begin();
	while (ci != m_de.object_types.end()) {
		if (!DumpObjectType(*ci, bCompilerResult)) {
			// DB error
			if (m_de.bVerbose) {
				std::cerr << "failure!" << std::endl;
			}

			return false;
		} else {
			// No DB error. Compiler error?
			if (!bCompilerResult) {
				// Emit trailing "failure!" if verbose
				if (m_de.bVerbose) {
					std::cerr << "failure!" << std::endl;
				}

				return true;
			}
		}
		ci++;
	}
  
	// Emit trailing "done!" if verbose
	if (m_de.bVerbose) {
		std::cerr << "Done dumping object types!" << std::endl;
	}


	// If we got this far, there were no DB errors
	return true;
}

void MQLExporter::DumpBeginTransaction()
{
	(*m_ostr) << "\nBEGIN TRANSACTION GO\n";
}


void MQLExporter::DumpCommitTransaction()
{
	(*m_ostr) << "\nCOMMIT TRANSACTION GO\n";
}


bool MQLExporter::DumpObjectDataSingleObject(std::string object_type_name, const std::list<FeatureInfo>& FeatureInfos, const InstObject *pObject, bool& bCompilerResult)
{
	UNUSED(bCompilerResult);
	
	// Emit pre-feature material
	(*m_ostr) << "CREATE OBJECT\n";
	(*m_ostr) << "FROM MONADS=";
	SetOfMonads som;
	pObject->getMonads(som);
	(*m_ostr) << som.toString() << '\n';
	(*m_ostr) << "WITH ID_D=" << id_d2string(pObject->getID_D()) << ' ';
	(*m_ostr) << "[";
	if (!m_de.bBatchCreateObjects) {
		// Only print object type name if we are not 
		// doing a batch dump (i.e., only if doing 
		// CREATE OBJECT, not CREATE OBJECTS WITH OBJECT TYPE).
		(*m_ostr) << object_type_name;
	}
	(*m_ostr) << '\n';

	// Iterate through list of FeatureInfos
	std::list<FeatureInfo>::const_iterator ci(FeatureInfos.begin());
	std::list<FeatureInfo>::const_iterator cend(FeatureInfos.end());
	unsigned int feature_index = 0;
	while (ci != cend) {
		// Feature name
		std::string feature_name = ci->getName();
    
		// EMdFValue
		const EMdFValue *pValue = pObject->getFeature(feature_index);

		// Sanity check
		ASSERT_THROW(pValue != 0, "pValue is 0");

		// Type
		id_d_t feature_type_id = ci->getType();
		id_d_t feature_type_id_for_getting_string_represenation;
		if (featureTypeIdIsListOfENUM(feature_type_id)) {
			feature_type_id_for_getting_string_represenation = GET_ENUM_FEATURE_ID_FROM_LIST_OF_ENUM_FEATURE_ID(feature_type_id);
		} else {
			feature_type_id_for_getting_string_represenation = feature_type_id;
		}
		std::string strType_id;
		bool bTypeIDGottenOK = m_pEE->getMQLEE()->pDB->typeIdToString(feature_type_id_for_getting_string_represenation, strType_id);
		if (!bTypeIDGottenOK) {
			std::cerr << "ERROR: Could not get string corresponding to feature " << feature_name << std::endl;
			std::cerr << "from object type " << object_type_name << std::endl;
			return false;
		}

		// Get value-string based on feature type
		std::string value_as_string;
		if (featureTypeIdIsASCII(feature_type_id) 
		    || featureTypeIdIsSTRING(feature_type_id)) {
			ASSERT_THROW(pValue->getKind() == kEVString, "pValue is not a kEVString");
			value_as_string = std::string("\"") + encodeSTRINGstring(pValue->getString()) + "\"";
		} else if (featureTypeIdIsListOf(feature_type_id)) {
			ASSERT_THROW(pValue->getKind() == kEVListOfInteger
				     || pValue->getKind() == kEVListOfID_D,
				     "pValue is neither kEVListOfInteger nor kEVListOfID_D");
			value_as_string = "(";
			const IntegerList *myintlist = pValue->getIntegerList();
			IntegerListConstIterator ci = myintlist->const_iterator();
			while (ci.hasNext()) {
				emdros_int64 myint = ci.next();
				std::string thisvalue;
				if (featureTypeIdIsListOfENUM(feature_type_id)) {
					if (!GetEnumConst(myint, strType_id, thisvalue)) {
						// Error has already been reported...
						return false;
					}
				} else {
					thisvalue = emdros_int64ToString(myint);
				}
				value_as_string += thisvalue;
				if (ci.hasNext())
					value_as_string += ",";
			}
			value_as_string += ")";
		} else if (featureTypeIdIsINTEGER(feature_type_id)) {
			ASSERT_THROW(pValue->getKind() == kEVInt, "pValue is not kEVInt");
			value_as_string = emdros_int64ToString(pValue->getInt());
		} else if (featureTypeIdIsID_D(feature_type_id)) {
			ASSERT_THROW(pValue->getKind() == kEVID_D, "pValue is not kEVID_D");
			value_as_string = id_d2string(pValue->getID_D());
		} else if (featureTypeIdIsENUM(feature_type_id)) {
			ASSERT_THROW(pValue->getKind() == kEVEnum, "pValue is not kEVEnum");
			if (!GetEnumConst(pValue->getEnum(), strType_id, value_as_string)) {
				// Error has already been reported...
				return false;
			}
		} else if (featureTypeIdIsSOM(feature_type_id)) {
			ASSERT_THROW(pValue->getKind() == kEVSetOfMonads, "pValue is not kEVSetOfMonads");
			value_as_string = pValue->getSOM().toString();
		} else {
			ASSERT_THROW(false, "Unknown feature_type_id: " + id_d2string(feature_type_id));
		}

		// Emit feature if it isn't self
		if (feature_name != "self") {
			(*m_ostr) << feature_name 
			     << ":=" << value_as_string
			     << ";\n";
		}

		// Advance iterators
		++ci;
		++feature_index;
	}

	// Emit post-object-data material
	(*m_ostr) << "]\n";

	// Only do a GO is we are not doing a batch creation
	if (!m_de.bBatchCreateObjects) {
		(*m_ostr) << "GO\n\n";
	}

	// If we got this far, there were no errors
	return true;
}

void MQLExporter::DumpDropIndexesOnObjectType(const std::string& object_type_name)
{
	(*m_ostr) << "// Drop indexes \n"
	     << "DROP INDEXES ON OBJECT TYPE[" << object_type_name << "] GO\n\n";
}

void MQLExporter::DumpCreateIndexesOnObjectType(const std::string& object_type_name)
{
	(*m_ostr) << "// Create indexes \n"
	     << "CREATE INDEXES ON OBJECT TYPE[" << object_type_name << "] GO\n\n";
}

void MQLExporter::DumpBatchCreateHeader(const std::string& object_type_name)
{
	(*m_ostr) << "CREATE OBJECTS\n"
	     << "WITH OBJECT TYPE[" << object_type_name << "]\n";
}

void MQLExporter::DumpCommitBeginTransaction(bool& bTransactionInProgress)
{
	// Commit transaction if this is not the first time
	if (bTransactionInProgress) {
		DumpCommitTransaction();
	}
	  
	// Begin transaction
	DumpBeginTransaction();
  
	// Set flag so we know next time is not the first time
	bTransactionInProgress = true;
}

bool MQLExporter::DumpObjectDataForObjectType(std::string object_type_name, bool& bCompilerResult)
{
	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << ">> Dumping object data for object type " << object_type_name << "..." << std::flush;
	}

	// Emit pre-object data material
	(*m_ostr) << "\n\n";
	(*m_ostr) << "//////////////////////////////////////////////////// \n";
	(*m_ostr) << "// \n";
	(*m_ostr) << "// Create object data for object type " << object_type_name << '\n';
	(*m_ostr) << "// \n";
	(*m_ostr) << "//////////////////////////////////////////////////// \n\n";

	// Drop indexes
	DumpDropIndexesOnObjectType(object_type_name);

	// Begin transaction if necessary
	bool bTransactionInProgress = false;
	if (m_de.bUseTransactions) {
		DumpBeginTransaction();
		bTransactionInProgress = true;
	}



	// Get object type id
	id_d_t object_type_id;
	eObjectRangeType objectRangeType;
	eMonadUniquenessType monadUniquenessType;
	if (!GetObjectTypeId(object_type_name, object_type_id, objectRangeType, monadUniquenessType)) {
		// Error has already been reported
		return false;
	}

	// Get features
	std::list<FeatureInfo> FeatureInfos;
	if (!m_pEE->getMQLEE()->pDB->getFeaturesForObjectType(object_type_id,
							      FeatureInfos)) {
		std::cerr << std::endl << "ERROR: Could not get features for object type " << object_type_name << std::endl;
		return false;
	}

	// Transform list to vector
	std::vector<std::string> feature_name_vec;
	std::list<FeatureInfo>::const_iterator ci(FeatureInfos.begin());
	std::list<FeatureInfo>::const_iterator cend(FeatureInfos.end());
	while (ci != cend) {
		// Name
		std::string feature_name = ci->getName();

		// Push back in feature_name_vec if it isn't self
		if (feature_name != "self") {
			feature_name_vec.push_back(feature_name);
		}

		// Advance iterator
		++ci;
	}

	// Get all_m_1
	SetOfMonads all_m_1;
	if (!m_pEE->getAll_m_1(all_m_1)) {
		std::cerr << std::endl << "ERROR: Could not get all_m_1." << std::endl;
		return false;
	}

	// Make start monad
	monad_m start;
	if (m_de.m_start == START_MONAD_NO_VALUE) {
		// The user didn't specify the start monad, so we must
		// set it to MIN_M
		start = all_m_1.first();
	} else {
		// The user specified the start monad, so we must use that.
		start = m_de.m_start;
	}

	// Make end monad
	monad_m end;
	if (m_de.m_end == END_MONAD_NO_VALUE) {
		// The user didn't specify the end monad, so we must
		// set it to MAX_M
		end = all_m_1.last();
	} else {
		// The user specified the end monad, so we must use that.
		end = m_de.m_end;
	}

	// Create Su and U to be { start-end }
	SetOfMonads Su(start,end); 
	SetOfMonads U(start,end);



	// Get inst
	std::string pre_query_string = "";
	Inst *pInst = new Inst(feature_name_vec);

	if (!m_pEE->getMQLEE()->pDB->getInst(object_type_name,
					     object_type_id,
					     Su,
					     all_m_1,
					     pre_query_string,
					     0,
					     FeatureInfos,
					     std::string("monads"),
					     kMSROPartOf,
					     *pInst)) {
		std::cerr << std::endl << "ERROR: DB error in getting list of objects for object type " << object_type_name << std::endl;
		delete pInst;
		return false;
	}

	// Iterate over inst
	pInst->set_current_universe(U);
	Inst::const_iterator inst_cit = pInst->begin();
	int object_count = 0;
	bool bBatchCreateInProgress = false;
	while (inst_cit.hasNext()) {
		// If verbose, emit "." for every 5000 objects
		if (m_de.bVerbose) {
			if ((object_count % 5000) == 0) {
				std::cerr << "." << std::flush;
			}
		}

		if (m_de.bBatchCreateObjects) {
			if ((object_count % m_de.nBatchSize) == 0) {
				if (bBatchCreateInProgress) {
					(*m_ostr) << "GO\n";
				}

				// Issue BEGIN TRANSACTION and COMMIT TRANSACTION statements
				//if (m_de.bUseTransactions) {
				//  DumpCommitBeginTransaction(bTransactionInProgress);
				//}

				// Issue CREATE OBJECTS WITH OBJECT TYPE header
				DumpBatchCreateHeader(object_type_name);

				// Set flag so we know next time is not the first time
				bBatchCreateInProgress = true;
			}
		} else { // !m_de.bBatchCreateObjects
			// Issue BEGIN TRANSACTION and COMMIT TRANSACTION statements
			// if we have done 2500 objects, and we are to do transaction.
			//if (m_de.bUseTransactions) {
			//  if ((object_count % 2500) == 0) {
			//    DumpCommitBeginTransaction(bTransactionInProgress);
			//  }
			//}
		}
    
		// Get object pointer
		const InstObject *pObject = inst_cit.current();

		

		// Process object
		if (!DumpObjectDataSingleObject(object_type_name, FeatureInfos, 
						pObject, bCompilerResult)) {
			delete pInst;
			if (m_de.bVerbose) {
				std::cerr << "failure!" << std::endl;
			}
			return false;
		} else {
			if (!bCompilerResult) {
				delete pInst;
				if (m_de.bVerbose) {
					std::cerr << "failure!" << std::endl;
				}
				return true;
			}
		}

		// Advance iterators
		inst_cit.next();
		object_count++;
	}

	// Clean up pInst
	delete pInst;

	// End CREATE OBJECTS WITH OBJECT TYPE
	// if we are doing a batch dump and a batch is in progress
	if (m_de.bBatchCreateObjects && bBatchCreateInProgress) {
		(*m_ostr) << "GO\n";
	}

	// Commit transaction if this is not the first time
	if (m_de.bUseTransactions && bTransactionInProgress) {
		DumpCommitTransaction();
	}

	// Create indexes
	DumpCreateIndexesOnObjectType(object_type_name);

	// Flush buffer
	(*m_ostr) << std::flush;

	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << "done!" << std::endl;
	}

	// If we got this far, there were no DB errors
	return true;
}

bool MQLExporter::DumpObjectData(bool& bCompilerResult) 
{
	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << "Dumping object data..." << std::endl;
	}
  
	// Emit pre-object data material
	(*m_ostr) << "\n\n";
	(*m_ostr) << "//////////////////////////////////////////////////// \n";
	(*m_ostr) << "// \n";
	(*m_ostr) << "// Create object data\n";
	(*m_ostr) << "// \n";
	(*m_ostr) << "//////////////////////////////////////////////////// \n\n";

	// Iterate through m_de.object_types
	std::list<std::string>::const_iterator ci;
	ci = m_de.object_types.begin();
	while (ci != m_de.object_types.end()) {
		if (!DumpObjectDataForObjectType(*ci, bCompilerResult)) {
			// DB error
			return false;
		} else {
			// No DB error. Compiler error?
			if (!bCompilerResult) {
				return true;
			}
		}
		ci++;
	}

	// Emit message if verbose
	if (m_de.bVerbose) {
		std::cerr << "Done dumping object data!" << std::endl;
	}
  
	// If we got this far, there were no DB errors
	return true;
}

void MQLExporter::VacuumDatabase()
{
	(*m_ostr) << "\n\n";
	(*m_ostr) << "//////////////////////////////////////////////////// \n";
	(*m_ostr) << "// \n";
	(*m_ostr) << "// VACUUM database\n";
	(*m_ostr) << "// \n";
	(*m_ostr) << "//////////////////////////////////////////////////// \n\n";
	(*m_ostr) << "VACUUM DATABASE ANALYZE GO\n\n" << std::flush;
}

bool MQLExporter::do_dump(bool& bCompilerResult) 
{

	// Find out which object types to treat
	if (!getObjectTypes(bCompilerResult)) {
		return false;
	} else {
		if (!bCompilerResult) {
			return true;
		}
	}

	// Dump CREATE DATABASE and USE DATABASE
	if (m_de.bCreateAndUseDB) {
		DumpCreateAndUseDB();
	}

	// Dump enums
	if (m_de.bCreateEnums) {
		if (!DumpEnums(bCompilerResult)) {
			return false;
		} else {
			if (!bCompilerResult) {
				return true;
			}
		}
	}

	// Dump object types
	if (m_de.bCreateObjectTypes) {
		if (!DumpObjectTypes(bCompilerResult)) {
			return false;
		} else {
			if (!bCompilerResult) {
				return true;
			}
		}
	}

	// Dump arbitrary monad sets
	if (m_de.bCreateArbitraryMonadSets) {
		if (!DumpArbitraryMonadSets(bCompilerResult)) {
			return false;
		} else {
			if (!bCompilerResult) {
				return true;
			}
		}
	}

	// Dump object data
	if (m_de.bCreateObjectData) {
		if (!DumpObjectData(bCompilerResult)) {
			return false;
		} else {
			if (!bCompilerResult) {
				return true;
			}
		}
	}

	// Dump VACUUM DATABASE statement
	VacuumDatabase();

	// If we got this far, there were no DB errors
	return true;
}

int MQLExporter::doIt(std::string& message)
{
	bool bResult = true;
	std::string strError;
	int nResult;

	// This will be closed automatically when it goes out of scope....
	std::ofstream fout;
	if (m_filename != "") {
		if (m_filename == "-") {
			m_ostr = &std::cout;
		} else {
			std::string strError;
			fout.open(m_filename.c_str());
			if (!fout) {
				message = "FAILURE: Could not open file " + m_filename + " for writing.\n"
					+ "Action aborted.\n";
				return 4;
			} else {
				m_ostr = &fout;
			}
		}
	}
	if (!do_dump(bResult)) {
		message = "FAILURE: Database error dumping.\n";
		nResult = 5;
	} else {
		if (!bResult) {
			message = "FAILURE: Compiler error dumping.\n";
			nResult = 6;
		} else { 
			message = "SUCCESS dumping.\n";
			nResult = 0;
		}
	}


	// Return result
	return nResult;
}


