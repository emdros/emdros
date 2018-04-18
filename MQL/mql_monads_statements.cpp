/*
 * mql_monads_statements.cpp
 *
 * MQL MONADS statements
 *
 * Ulrik Petersen
 * Created: 3/6-2001 (March 6, 2001)
 * Last update: 3/1-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2017  Ulrik Sandborg-Petersen
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


/**@file mql_monads_statements.cpp
 *@brief Implementation of MQL MONADS statements (MQL layer)
 */

#include <emdfdb.h>
#include <mql_monads_statements.h>
#include <string_func.h>
#include <debug.h>

/////////////////////////////////////////////////////////////
//
// MonadsStatement
//
// base class
//
////////////////////////////////////////////////////////////
MonadsStatement::~MonadsStatement()
{
	delete m_AST_monads;
}

void MonadsStatement::weed(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return;
}

bool MonadsStatement::symbol(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}



/////////////////////////////////////////////////////////////
//
// INSERT MONADS
//
// InsertMonadsStatement
//
////////////////////////////////////////////////////////////
void InsertMonadsStatement::weed(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return;
}

bool InsertMonadsStatement::symbol(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}

bool InsertMonadsStatement::exec()
{
	// TODO: Implement
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}



/////////////////////////////////////////////////////////////
//
// DELETE MONADS
//
// DeleteMonadsStatement
//
////////////////////////////////////////////////////////////
void DeleteMonadsStatement::weed(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return;
}

bool DeleteMonadsStatement::symbol(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}

bool DeleteMonadsStatement::exec()
{
	// TODO: Implement
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}


/////////////////////////////////////////////////////////////
//
// GET MONADS
//
// GetMonadsStatement
//
////////////////////////////////////////////////////////////
GetMonadsStatement::GetMonadsStatement(MQLExecEnv *pEE, 
				       ID_D *pID_Ds, 
				       std::string* object_type_name)
	: ObjectTypeStatement(pEE, object_type_name), m_id_ds(pID_Ds)
{
}

GetMonadsStatement::~GetMonadsStatement()
{
	delete m_id_ds;
}

bool GetMonadsStatement::symbol(bool& bResult) 
{
	//
	// - Check that the object type already exists.  Store the object 
	//   type_id locally, as well as whether it is a WITH SINGLE RANGE
	//   OBJECTS object type.
	if (!symbolObjectTypeExists(bResult))
		return false;

	// In any case, if we got this far, we succeeded.
	return true;
}


bool GetMonadsStatement::exec()
{
	// Make som
	SetOfMonads id_ds_set;
	m_id_ds->execMakeSOM(id_ds_set);

	// Make table
	m_result = new MQLResult();

	// Build header
	m_result->appendHeader("object_id_d", kTCID_D);
	m_result->appendHeader("mse_first", kTCMonad_m);
	m_result->appendHeader("mse_last", kTCMonad_m);

	// Ask EMdFDB
	if (!m_pEE->pDB->getMonadsFromID_Ds(id_ds_set, 
					    *m_object_type_name, 
					    this->getObjectRangeType(),
					    m_result)) {
		// Failure
		return false;
	}

	// If we got this far, there was nothing wrong.
	return true;
}


/////////////////////////////////////////////////////////////
//
// MonadSetCalculationStatement
//
////////////////////////////////////////////////////////////

MonadSetCalculationStatement::MonadSetCalculationStatement(MQLExecEnv *pEE, MonadSetChainElement *chain) : Statement(pEE), m_chain(chain) 
{
}

MonadSetCalculationStatement::~MonadSetCalculationStatement()
{
	delete m_chain;
}

void MonadSetCalculationStatement::weed(bool& bResult)
{
	// weed monotonic and positive
	m_chain->weed(m_pEE, bResult);

	// bResult has been set, which is all we "return"

	return;
}

bool MonadSetCalculationStatement::exec()
{
	// Calculate result
	SetOfMonads monads;
	m_chain->exec(monads);

	// Make table
	m_result = new MQLResult();

	// Build header
	m_result->appendHeader("mse_first", kTCMonad_m);
	m_result->appendHeader("mse_last", kTCMonad_m);

	// Make table
	SOMConstIterator ci = monads.const_iterator();
	while (ci.hasNext()) {
		MonadSetElement mse = ci.next();
		m_result->startNewRow();
		m_result->append(monad_m2string(mse.first()));
		m_result->append(monad_m2string(mse.last()));
	}

	// There can be no DB errors
	return true;
}


/////////////////////////////////////////////////////////////
//
// GetMonadSetsStatement
//
////////////////////////////////////////////////////////////

GetMonadSetsStatement::~GetMonadSetsStatement()
{
	delete m_pMonad_set_names;
}


bool GetMonadSetsStatement::symbol(bool& bResult)
{
	// Make list of monad sets
	if (m_bAll) {
		// Get all monad sets into m_monad_set_names
		if (!m_pEE->pDB->selectMonadSets(m_monad_set_names)) {
			m_pEE->pError->appendError("Could not get monad set names.\n");
			return false;
		} 
	} else {
		// This is not the ALL incarnation.  We must transform the
		// StringListNode list to a std::list<std::string>.
		StringList strlist(m_pMonad_set_names);
		StringListConstIterator ci = strlist.const_iterator();
		while (ci.hasNext()) {
			m_monad_set_names.push_back(ci.next());
		}
	}

	// Now check the existence of each, and get them
	std::list<std::string>::const_iterator ci;
	for (ci = m_monad_set_names.begin(); ci != m_monad_set_names.end(); ++ci) {
		std::string monad_set_name = *ci;
		SetOfMonads som;
		bool bExists;
		if (!m_pEE->pDB->monadSetExists(monad_set_name, bExists, som)) {
			m_pEE->pError->appendError("DB error getting monad set with name '" + monad_set_name + "'.\n");
			return false;
		} else {
			if (bExists) {
				// If it exists, push it to the back of the list
				m_monad_sets.push_back(som);
			} else {
				// If it does not exist, complain and return with compilation error
				m_pEE->pError->appendError("monad set with name '" + monad_set_name + "' does not exist.\n");
				bResult = false;
				return true;
			}
		}
	}

	// If we got this far, there were no DB errors and no compilation errors
	bResult = true;
	return true;
}


bool GetMonadSetsStatement::exec()
{
	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("monad_set_name", kTCString);
	m_result->appendHeader("mse_first", kTCMonad_m);
	m_result->appendHeader("mse_last", kTCMonad_m);

	std::list<std::string>::const_iterator strci = m_monad_set_names.begin();
	std::list<std::string>::const_iterator strciend = m_monad_set_names.end();
	std::list<SetOfMonads>::const_iterator som_list_ci = m_monad_sets.begin();

	while (strci != strciend) {
		SOMConstIterator mseci = som_list_ci->const_iterator();
		while (mseci.hasNext()) {
			MonadSetElement mse = mseci.next();
			m_result->startNewRow();
			m_result->append(*strci);
			m_result->append(monad_m2string(mse.first()));
			m_result->append(monad_m2string(mse.last()));
		}
		++strci;
		++som_list_ci;
	}

	// If we got this far, there were no DB errors
	return true;
}


/////////////////////////////////////////////////////////////
//
// MonadSetStatement
//
////////////////////////////////////////////////////////////

MonadSetStatement::~MonadSetStatement()
{
	delete m_pMonadSetName;
}


bool MonadSetStatement::symbol(bool& bResult)
{
	if (m_bCheckExistence) {
		bool bExists;
		if (!m_pEE->pDB->monadSetExists(*m_pMonadSetName, bExists, m_som)) {
			m_pEE->pError->appendError("DB error checking existence of monad set with name '" + *m_pMonadSetName + "'.\n");
			return false;
		} else {
			if (!bExists) {
				// If it does not exist, complain and return with compilation error
				m_pEE->pError->appendError("monad set with name '" + *m_pMonadSetName + "' does not exist.\n");
				bResult = false;
				return true;
			} else {
				// It exists.  Everything is fine.
			}
		}
	}

	// If we came this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}




/////////////////////////////////////////////////////////////
//
// CreateOrUpdateMonadSetStatement
//
////////////////////////////////////////////////////////////

CreateOrUpdateMonadSetStatement::~CreateOrUpdateMonadSetStatement()
{
	delete m_pOtherMonadSetName;
	delete m_pMonadSet;
}

void CreateOrUpdateMonadSetStatement::weed(bool& bResult)
{
	// weed monotonic and positive
	if (m_pMonadSet != 0) {
		m_pMonadSet->weed(m_pEE, true, bResult);
		if (!bResult)
			return;
	}

	// If we came this far, there were no DB errors and no compiler errors
	bResult = true;
	return;
}


bool CreateOrUpdateMonadSetStatement::symbol(bool& bResult)
{
	// First call base-class
	if (!MonadSetStatement::symbol(bResult)) {
		return false;
	} else {
		if (!bResult) {
			return true;
		}
	}

	// Then see if we should get other monad set 
	if (m_pOtherMonadSetName != 0) {
		// We cannot have both an other monad set name and an other monad set
		ASSERT_THROW(m_pMonadSet == 0,
			     "We cannot have both an other monad set name and an other monad set");
		bool bExists;
		if (!m_pEE->pDB->monadSetExists(*m_pOtherMonadSetName, bExists, m_other_som)) {
			m_pEE->pError->appendError("DB error checking existence of monad set with name '" + *m_pOtherMonadSetName + "'.\n");
			return false;
		} else {
			if (!bExists) {
				// If it does not exist, complain and return with compilation error
				m_pEE->pError->appendError("monad set with name '" + *m_pOtherMonadSetName + "' does not exist.\n");
				bResult = false;
				return true;
			} else {
				// It exists.  Everything is fine.
			}
		}
	}

	// If we came this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}


bool CreateOrUpdateMonadSetStatement::monads(bool& bResult)
{
	// Only do it if it is there
	if (m_pMonadSet != 0) {
		// We cannot have both an other monad set name and an other monad set
		ASSERT_THROW(m_pOtherMonadSetName == 0,
			     "We cannot have both an other monad set name and an other monad set");

		// It is here; make it into m_other_som
		m_pMonadSet->monadsBuildSet(m_other_som);
	}
  
	// If we came this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}




/////////////////////////////////////////////////////////////
//
// CreateMonadSetStatement
//
////////////////////////////////////////////////////////////

bool CreateMonadSetStatement::exec()
{
	// Call DB to do it
	if (!m_pEE->pDB->createMonadSet(*m_pMonadSetName, m_other_som)) {
		m_pEE->pError->appendError("DB error creating monad set with name '" + *m_pMonadSetName + "'\n");
		return false;
	}

	// If we came this far, there were no DB errors
	return true;
}




/////////////////////////////////////////////////////////////
//
// UpdateMonadSetStatement
//
////////////////////////////////////////////////////////////

bool UpdateMonadSetStatement::exec()
{
	// Calculate new monad set
	SetOfMonads newsom = m_som;
	switch (m_op) {
	case kSOUnion:
		newsom.unionWith(m_other_som);
		break;
	case kSOIntersect:
		newsom = SetOfMonads::intersect(m_som, m_other_som);
		break;
	case kSODifference:
		newsom.difference(m_other_som);
		break;
	case kSOReplace:
		newsom = m_other_som;
		break;
	case kSONone:
		ASSERT_THROW(false,
			     "KSONone found");
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown kind");
		break;
	}

	// start transaction
	bool bDoCommit = m_pEE->pDB->beginTransaction();

	// Drop som
	if (!m_pEE->pDB->dropMonadSet(*m_pMonadSetName)) {
		if (bDoCommit) {
			m_pEE->pDB->abortTransaction();
		}
		m_pEE->pError->appendError("Could not drop monad set with name '" + *m_pMonadSetName + "'\n");
		return false;
	}
  
	// Recreate som
	if (!m_pEE->pDB->createMonadSet(*m_pMonadSetName, newsom)) {
		if (bDoCommit) {
			m_pEE->pDB->abortTransaction();
		}
		m_pEE->pError->appendError("Could not recreate monad set with name '" + *m_pMonadSetName + "'\n");
		return false;
	}

	// Commit transaction if necessary
	if (bDoCommit) {
		if (!m_pEE->pDB->commitTransaction()) {
			m_pEE->pError->appendError("UpdateMonadSetStatement::exec: Could not commit transaction.");
			return false;
		}
	}

	// If we got this far, there were no DB errors
	return true;
}




/////////////////////////////////////////////////////////////
//
// DropMonadSetStatement
//
////////////////////////////////////////////////////////////

bool DropMonadSetStatement::exec()
{
	// Call DB
	return m_pEE->pDB->dropMonadSet(*m_pMonadSetName);
}


