/*
 * mql_enumeration_statements.cpp
 *
 * MQL ENUMERATION statements
 *
 * Ulrik Petersen
 * Created: 3/6-2001 (March 6, 2001)
 * Last update: 11/30-2018
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


/**@file mql_enumeration_statements.cpp
 *@brief Implementation of MQL ENUMERATION statements (MQL layer)
 */

#include <emdfdb.h>
#include <mql_enumeration_statements.h>
#include <debug.h>

//////////////////////////////////////////////////////////////
//
// ECDeclaration 
//
//////////////////////////////////////////////////////////////

ECDeclaration::ECDeclaration(bool is_default, 
			     std::string* ec_name, 
			     emdf_ivalue* opt_ec_initialization, 
			     ECDeclaration* next)
{
	m_is_default = is_default;
	m_ec_name = ec_name;
	m_opt_ec_initialization = opt_ec_initialization;
	m_next = next;
}

ECDeclaration::~ECDeclaration()
{
	delete m_next;
	delete m_ec_name;
	delete m_opt_ec_initialization;
}

bool ECDeclaration::getIsDefault()
{
	return m_is_default;
}

emdf_ivalue ECDeclaration::getOptEcInitialization()
{
	return *m_opt_ec_initialization;
}

const std::string& ECDeclaration::getEcName()
{
	return *m_ec_name;
}

void ECDeclaration::weedDefault(MQLExecEnv *pEE, bool& bResult)
{
	bool has_met_default = false;
	weedDefaultPrivate(pEE, bResult, has_met_default);
	return;
}

//
// - Check that no constant exists already in the same enumeration
// 
bool ECDeclaration::symbolExistenceOfConstants(MQLExecEnv *pEE,id_d_t enum_id, bool& bResult)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->symbolExistenceOfConstants(pEE, enum_id, bResult))
			return false;
		if (!bResult)
			return true;
	}


	// Then deal with ourselves
	bool bEnumConstExists;
	emdf_ivalue dummy_value;
	bool dummy_is_default;
	if (!pEE->pDB->enumConstExists(*m_ec_name,
				       enum_id,
				       bEnumConstExists,
				       dummy_value,
				       dummy_is_default))
		return false;

	// If it existed, we return with a fail
	bResult = !bEnumConstExists;
	if (!bResult) {
		std::string mql_error;
		mql_error = "The constant " + *m_ec_name + " exists already.\n";
		pEE->pError->appendError(mql_error);
	}

	// Return no database error
	return true;
}

//
// - Assign a value to each ec_declaration, either based on its 
//   position in the sequence, or based on its optional initialization
// 
void ECDeclaration::symbolAssignValues(void)
{
	emdf_ivalue previous = 0;
	symbolAssignValuesPrivate(previous);
}

void ECDeclaration::symbolAssignValuesPrivate(emdf_ivalue& previous)
{
	// Deal with next before we deal with ourselves
	if (m_next != 0) {
		m_next->symbolAssignValuesPrivate(previous);
	} 

	// Then deal with ourselves:
	// - If we have an explicit initialization, use that.
	// - Else use previous + 1
	if (m_opt_ec_initialization != 0) {
		m_value = *m_opt_ec_initialization;
	} else {
		m_value = previous + 1;
	}

	// Set previous so that the next one in the list can use it
	previous = m_value;

	// Return
	return;
}

void ECDeclaration::symbolValueMoreThanOnce(MQLExecEnv *pEE, bool& bResult)
{
	std::set<emdf_ivalue> value_set;
	std::set<std::string> const_set;
	symbolValueMoreThanOncePrivate(pEE, bResult, value_set, const_set);
	return;
}

void ECDeclaration::symbolValueMoreThanOncePrivate(MQLExecEnv *pEE, bool& bResult, std::set<emdf_ivalue>& value_set, std::set<std::string>& const_set)
{
	// Deal with next before we deal with ourselves
	if (m_next != 0) {
		m_next->symbolValueMoreThanOncePrivate(pEE, bResult, value_set, const_set);
		if (!bResult)
			return;
	} 

	// Then deal with ourselves
	if (value_set.find(m_value) != value_set.end()) {
		bResult = false;
		pEE->pError->appendError("Value " + int2string(m_value) + " is assigned to more than one label.\nThis is not allowed.\n");
		return;
	} else {
		bResult = true;
		value_set.insert(m_value);
	}

	if (const_set.find(*m_ec_name) != const_set.end()) {
		bResult = false;
		pEE->pError->appendError("Enumeration constant " + *m_ec_name + " is declared more than once.\nThis is not allowed.\n");
		return;
	} else {
		bResult = true;
		const_set.insert(*m_ec_name);
	}

}


//
// - Check that at most one declaration has the DEFAULT keyword.
//
// - If none has the DEFAULT keyword, set the m_is_default
//   of the first one to true.
//
void ECDeclaration::weedDefaultPrivate(MQLExecEnv *pEE, bool& bResult, bool& has_met_default)
{
	// If we have already met the default, and this is a default too, 
	// return with a fail
	if (has_met_default && m_is_default) {
		bResult = false;
		std::string mql_error;
		mql_error = "You cannot have more than one default\n";
		pEE->pError->appendError(mql_error);
		return;
	}

	// Set has_met_default
	if (m_is_default)
		has_met_default = true;

	// Deal with next before we deal with ourselves
	if (m_next != 0) {
		m_next->weedDefaultPrivate(pEE, bResult, has_met_default);
		if (!bResult)
			return;
	}

	// Then deal with ourselves:
	// If this is the first one, and we haven't met the default yet,
	// set ourselves to be the default
	if (m_next == 0 && !has_met_default)
		m_is_default = true;

	// Set bResult and return
	bResult = true;
	return;
}

bool ECDeclaration::createConstants(MQLExecEnv *pEE, id_d_t enum_id, const std::string& enum_name)
{
	// Deal with next before we deal with ourselves
	if (m_next != 0) {
		if (!m_next->createConstants(pEE, enum_id, enum_name))
			return false;
	}

	// Then deal with ourselves:
	EnumConstInfo ec_info(*m_ec_name, m_value, m_is_default);
	if (!pEE->pDB->createEnumConstant(enum_id, enum_name, ec_info))
		return false;

	// Return with no database error
	return true;
}



//////////////////////////////////////////////////////////////
//
// ECUpdate 
//
//////////////////////////////////////////////////////////////

ECUpdate::ECUpdate(bool is_default, 
		   std::string* ec_name, 
		   emdf_ivalue ec_initialization, 
		   eAddUpdateRemove kind, 
		   ECUpdate* next)
{
	m_is_default = is_default;
	m_ec_name = ec_name;
	m_ec_initialization = ec_initialization;
	m_kind = kind;
	m_next = next;
}

ECUpdate::~ECUpdate()
{
	delete m_next;
	delete m_ec_name;
}



void ECUpdate::weedDefault(MQLExecEnv *pEE, bool& bResult)
{
	bool has_met_default = false;
	weedDefaultPrivate(pEE, bResult, has_met_default);
	return;
}

//
// - Check that at most one declaration has the DEFAULT keyword.
//
void ECUpdate::weedDefaultPrivate(MQLExecEnv *pEE, bool& bResult, bool& has_met_default)
{
	// If we have already met the default, and this is a default too, 
	// return with a fail
	if (has_met_default && m_is_default) {
		bResult = false;
		std::string mql_error;
		mql_error = "You cannot have more than one default\n";
		pEE->pError->appendError(mql_error);
		return;
	}

	// Set has_met_default
	if (m_is_default)
		has_met_default = true;

	// Deal with next 
	if (m_next != 0) {
		m_next->weedDefaultPrivate(pEE, bResult, has_met_default);
		if (!bResult)
			return;
	}


	// Set bResult and return
	bResult = true;
	return;
}

bool SymbolConstExists(MQLExecEnv *pEE, const std::string& ec_name, id_d_t enum_id, bool& bConstExists)
{
	emdf_ivalue dummy_value;
	bool dummy_is_default;
	return pEE->pDB->enumConstExists(ec_name,
					 enum_id,
					 bConstExists,
					 dummy_value,
					 dummy_is_default);
}

//
// - Check that for all additions, the constants to be added 
//   do not exist
//
// - Check that for all removals and updates, the constants
//   involved do exist.
//
bool ECUpdate::symbolExistenceOfConstants(MQLExecEnv *pEE, id_d_t enum_id, bool& bResult)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->symbolExistenceOfConstants(pEE, enum_id, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with ourselves
	bool bConstExists;
	switch (m_kind) {
	case kAdd:
		if (!SymbolConstExists(pEE, *m_ec_name, enum_id, bConstExists))
			return false;
		if (bConstExists) {
			bResult = false;
			std::string mql_error;
			mql_error = "Enumeration constant " + *m_ec_name + " exists already.  Cannot add.\n";
			pEE->pError->appendError(mql_error);
			return true;
		}
		break;
	case kUpdate:
		if (!SymbolConstExists(pEE, *m_ec_name, enum_id, bConstExists))
			return false;
		if (!bConstExists) {
			bResult = false;
			std::string mql_error;
			mql_error = "Enumeration constant " + *m_ec_name + " does not exist.  Cannot update.\n";
			pEE->pError->appendError(mql_error);
			return true;
		}
		break;
	case kRemove:
		if (!SymbolConstExists(pEE, *m_ec_name, enum_id, bConstExists))
			return false;
		if (!bConstExists) {
			bResult = false;
			std::string mql_error;
			mql_error = "Enumeration constant " + *m_ec_name + " does not exist.  Cannot remove.\n";
			pEE->pError->appendError(mql_error);
			return true;
		}
		break;
	}
	bResult = true;
	return true;
}

//
// - Check whether the current default is being removed.
//
bool ECUpdate::symbolDefaultIsRemoved(MQLExecEnv *pEE, id_d_t enum_id, bool& bDefaultIsRemoved)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->symbolDefaultIsRemoved(pEE, enum_id, bDefaultIsRemoved))
			return false;
		if (bDefaultIsRemoved)
			return true;
	}

	// Then deal with ourselves
	if (m_kind == kRemove) {
		bool dummy_const_exists;
		emdf_ivalue dummy_value;
		bool is_default;
		if (!pEE->pDB->enumConstExists(*m_ec_name,
					       enum_id,
					       dummy_const_exists,
					       dummy_value,
					       is_default))
			return false;
		if (is_default) {
			bDefaultIsRemoved = true;
			return true;
		}
	}
  
	// Set result and return
	bDefaultIsRemoved = false;
	return true;
}

//
// Returns true on one of the ec_updates in the list
// has a "default" modifier.
//
bool ECUpdate::hasDefault(void)
{
	// First deal with next
	if (m_next != 0) {
		bool bResult;
		bResult = m_next->hasDefault();
		if (bResult)
			return true;
	} 

	// Then deal with ourselves
	return m_is_default;
}

// Returns a reference to the ec_name that is the default.
// Assumes that one is the default.
const std::string& ECUpdate::getDefault(void) const
{
	// If we are the default, return the ec_name
	if (m_is_default)
		return *m_ec_name;

	if (m_next != 0) {
		return m_next->getDefault();
	} else {
		// We reached the end of the list without finding the default.
		ASSERT_THROW(false,
			     "We reached the end of the list without finding the default");
		return *m_ec_name;  // Just so we don't get a warning
	}
}

bool ECUpdate::symbolNoTwoValuesSame(MQLExecEnv *pEE, id_d_t enum_id, bool& bResult)
{
	// Get enum from DB
	std::list<EnumConstInfo> enum_list;
	if (!pEE->pDB->getEnumConstants(enum_id, enum_list)) {
		pEE->pError->appendError("Error getting enumeration constant list from database.\n");
		return false;
	}

	// Do update
	symbolDoLocalUpdatePrivate(enum_list);

	// Check that it doesn't have two labels with the same value
	std::set<emdf_ivalue> value_set;
	symbolCheckNoTwoValuesSamePrivate(pEE, enum_list, value_set, bResult);
	return true;
}

void ECUpdate::symbolDoLocalUpdatePrivate(std::list<EnumConstInfo>& enum_list)
{
	// Deal with next first
	if (m_next != 0) {
		m_next->symbolDoLocalUpdatePrivate(enum_list);
	}

	// Then deal with ourselves
	std::list<EnumConstInfo>::iterator it;
	std::list<EnumConstInfo>::iterator itend;
	it = enum_list.begin();
	itend = enum_list.end();
	switch (m_kind) {
	case kAdd:
		enum_list.push_back(EnumConstInfo(*m_ec_name, m_ec_initialization, m_is_default));
		break;
	case kUpdate:
		for (; it != itend; ++it) {
			if (it->getName() == *m_ec_name) {
				it->setValue(m_ec_initialization);
				it->setIsDefault(m_is_default);
			}
		}
		break;
	case kRemove:
		while (it != itend) {
			if (it->getName() == *m_ec_name) {
				enum_list.erase(it);

				// We start at the beginning again,
				// because Microsoft's STL doesn't
				// behave as expected, w.r.t.
				// the return value of erase.
				it = enum_list.begin();
			} else {
				++it;
			}
		}
		break;
	}
}

void ECUpdate::symbolCheckNoTwoValuesSamePrivate(MQLExecEnv *pEE, const std::list<EnumConstInfo>& enum_list, std::set<emdf_ivalue>& value_set, bool& bResult)
{
	std::list<EnumConstInfo>::const_iterator ci = enum_list.begin();
	std::list<EnumConstInfo>::const_iterator cend = enum_list.end();
	for (; ci != cend; ++ci) {
		emdf_ivalue value = ci->getValue();
		if (value_set.find(value) != value_set.end()) {
			bResult = false;
			pEE->pError->appendError("Value " + int2string(value) + " would exist for more than one label after update.\nThis is not allowed.\n");
		} else {
			value_set.insert(value);
		}
	}
}

//
// - Add all the constants to be added.
//
// - Update all the constants to be updated.
//
// - Remove all the constants to be removed.
//
bool ECUpdate::execAddUpdateRemove(MQLExecEnv *pEE, id_d_t enum_id, const std::string& enum_name)
{
	// First deal with next
	if (m_next != 0) {
		if (!m_next->execAddUpdateRemove(pEE, enum_id, enum_name))
			return false;
	}

	// Then deal with ourselves
	EnumConstInfo ec_info(*m_ec_name, m_ec_initialization, m_is_default);
	emdf_ivalue value;      // Current value of enum const
	bool dummy_is_default; // Dummy variable
	bool bExists;

	// Get current value (if constant exists)
	if (!pEE->pDB->enumConstExists(*m_ec_name, enum_id, bExists, 
				       value, dummy_is_default))
		return false;

	switch (m_kind) {
	case kAdd:
		if (!pEE->pDB->createEnumConstant(enum_id, enum_name, ec_info))
			return false;
		break;
	case kUpdate:
		// If it doesn't exist, something may be gruesomely wrong,
		// since have checked for existence previously.
		// However, since Emdros is a multi-user system,
		// it could be that someone has removed it before us
		// after we have checked for existence, so we should handle 
		// this gracefully.
		if (bExists) {
			if (!pEE->pDB->updateEnumConst(*m_ec_name, enum_id, value, m_ec_initialization))
				return false;
		}
		break;
	case kRemove:
		// If it doesn't exist, something may be gruesomely wrong,
		// since have checked for existence previously.
		// However, since Emdros is a multi-user system,
		// it could be that someone has removed it before us
		// after we have checked for existence, so we should handle 
		// this gracefully.
		if (bExists) {
			// Drop it!
			if (!pEE->pDB->dropEnumConst(enum_id, *m_ec_name, value))
				return false;
		}
		break;
	}

	// Return no database error
	return true;
}






/////////////////////////////////////////////////////////////
//
// EnumerationStatement
//
// base class
//
////////////////////////////////////////////////////////////
EnumerationStatement::~EnumerationStatement()
{
	delete m_enumeration_name;
}

//
// - Checks that the enumeration exists.  Result in bResult.
//
// - If it exists, gets its enum_id into m_enum_id.
//
bool EnumerationStatement::symbolEnumerationExists(bool& bResult, bool bReportError)
{
	// Ask DB
	if (!m_pEE->pDB->enumExists(*m_enumeration_name, bResult, m_enum_id)) {
		// Return with DB error
		return false;
	} 

	if (!bResult && bReportError) {
		m_pEE->pError->appendError("Enumeration " + *m_enumeration_name + " did not exist.\n");
	}

	// If we got this far, there were no DB errors
	return true;
}




/////////////////////////////////////////////////////////////
//
// CREATE ENUMERATION
//
// CreateEnumerationStatement
//
////////////////////////////////////////////////////////////
CreateEnumerationStatement::CreateEnumerationStatement(
						       MQLExecEnv *pEE, 
						       std::string* enumeration_name, 
						       ECDeclaration* ec_declaration_list) 
	: EnumerationStatement(pEE, enumeration_name), 
	  m_ec_declarations(ec_declaration_list)
{
}

CreateEnumerationStatement::~CreateEnumerationStatement()
{
	delete m_ec_declarations;
}

void CreateEnumerationStatement::weed(bool& bResult)
{
	m_ec_declarations->weedDefault(m_pEE, bResult);
	return;
}

bool CreateEnumerationStatement::symbol(bool& bResult)
{
	// Check that enumeration does not exist
	bool bEnumerationExists;
	if (!symbolEnumerationExists(bEnumerationExists, false)) {
		return false;
	}

	if (bEnumerationExists) {
		bResult = false;
		m_pEE->pError->appendError("Enumeration " + *m_enumeration_name + " exists already!\n");
		return true;
	}

	// Check that no enumeration constant already exists in 
	// the same enumeration by the name given in any of the 
	// ec_declarations
	//
	// NOTE: We should not do this, since we have just
	// proven that the enumeration does not exist,
	// so the m_enum_id is not even valid!
	//if (!m_ec_declarations->symbolExistenceOfConstants(m_pEE, m_enum_id, bResult))
	//	return false;
	//if (!bResult) 
	//	return true;

	// Assign a value to each ec_declaration, either
	// based on its position in the sequence, or based
	// on its initialization
	m_ec_declarations->symbolAssignValues();

	// Check that no value is assigned more than once
	m_ec_declarations->symbolValueMoreThanOnce(m_pEE, bResult);
	if (!bResult)
		return true;

	// Return with success
	bResult = true;
	return true;
}

bool CreateEnumerationStatement::exec()
{
	// Create enumeration and get enum_id
	id_d_t enum_id;
	if (!m_pEE->pDB->createEnum(*m_enumeration_name, enum_id)) {
		m_pEE->pError->appendError("Could not create numeration.\n");
		return false;
	}

	if (!m_ec_declarations->createConstants(m_pEE, enum_id, *m_enumeration_name)) {
		m_pEE->pError->appendError("Could not create constants.\n");
		return false;
	}

	return true;
}



/////////////////////////////////////////////////////////////
//
// UPDATE ENUMERATION
//
// UpdateEnumerationStatement
//
////////////////////////////////////////////////////////////
UpdateEnumerationStatement::UpdateEnumerationStatement(
						       MQLExecEnv *pEE, 
						       std::string* enumeration_name,
						       ECUpdate* ec_update_list) : EnumerationStatement(pEE, enumeration_name),
										   m_ec_update_list(ec_update_list)
{
}

UpdateEnumerationStatement::~UpdateEnumerationStatement()
{
	delete m_ec_update_list;
}

//
// - Check that at most one declaration has the DEFAULT keyword.
//
void UpdateEnumerationStatement::weed(bool& bResult)
{
	m_ec_update_list->weedDefault(m_pEE, bResult);
	return;
}

//
// - Check that the enumeration exists already
//
// - Check that for all additions, the constants to be added 
//   do not exist
//
// - Check that for all removals and updates, the constants
//   involved do exist.
//
// - Check whether the current default is being removed.
//   If it is, then another default should be specified,
//   either as an update or as an addition.
//
bool UpdateEnumerationStatement::symbol(bool& bResult)
{
	// Check that enumeration does exist
	bool bEnumerationExists;
	if (!symbolEnumerationExists(bEnumerationExists, true))
		return false;

	if (!bEnumerationExists) {
		bResult = false;
		return true;
	}

	// - Check that for all additions, the constants to be added 
	//   do not exist
	//
	// - Check that for all removals and updates, the constants
	//   involved do exist.
	if (!m_ec_update_list->symbolExistenceOfConstants(m_pEE, m_enum_id, bResult))
		return false;
	if (!bResult)
		return true;

	//
	// - Check whether the current default is being removed.
	//   If it is, then another default should be specified,
	//   either as an update or as an addition.
	bool bDefaultIsRemoved;
	if (!m_ec_update_list->symbolDefaultIsRemoved(m_pEE, m_enum_id, bDefaultIsRemoved))
		return false;
	if (bDefaultIsRemoved) {
		if (!m_ec_update_list->hasDefault()) {
			bResult = false;
			m_pEE->pError->appendError("Default is removed without specifying new default.\n");
			return true;
		}
	}

	// Check that there are no two labels with the same value
	// after the update.
	if (!m_ec_update_list->symbolNoTwoValuesSame(m_pEE, m_enum_id, bResult)) {
		return false;
	}
	if (!bResult) {
		return true;
	}

	// Set bResult to no compiler error and return no database error
	bResult = true;
	return true;
}

//
// - Add all the constants to be added.
// 
// - Update all the constants to be updated.
//
// - Remove all the constants to be removed.
//
// - If there was a new specification of "default":
//   - Remove the "is_default" flag from the current default.
//   - Set the new default.
// 
bool UpdateEnumerationStatement::exec()
{
	// - Add all the constants to be added.
	// - Update all the constants to be updated.
	// - Remove all the constants to be removed.
	if (!m_ec_update_list->execAddUpdateRemove(m_pEE, m_enum_id, *m_enumeration_name)) 
		return false;

	//
	// - If there was a new specification of "default":
	//   - Remove the "is_default" flag from the current default.
	//   - Set the new default.
	if (m_ec_update_list->hasDefault()) {
		if (!m_pEE->pDB->setDefaultEnumConst(m_enum_id,
						     m_ec_update_list->getDefault()))
			return false;
	}

	return true;
}



/////////////////////////////////////////////////////////////
//
// DROP ENUMERATION
//
// DropEnumerationStatement
//
////////////////////////////////////////////////////////////
DropEnumerationStatement::DropEnumerationStatement(MQLExecEnv *pEE, 
						   std::string* enumeration_name) : EnumerationStatement(pEE, enumeration_name)
{
}

DropEnumerationStatement::~DropEnumerationStatement()
{
}


//
// - Checks that the enumeration exists.  Result in bResult.
//
// - If it exists, gets its enum_id into m_enum_id.
//
bool DropEnumerationStatement::symbol(bool& bResult)
{
	return symbolEnumerationExists(bResult, true);
}

bool DropEnumerationStatement::exec()
{
	// Drop in database
	return m_pEE->pDB->dropEnum(m_enum_id);
}
