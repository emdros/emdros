/*
 * mql_execution_environment.cpp
 *
 * MQL execution environment
 *
 * Ulrik Petersen
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 2/1-2014
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2014  Ulrik Sandborg-Petersen
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

/**@file mql_execution_environment.cpp
 *@brief Implementation of MQLExecEnv (MQL layer)
 */

#include <emdfdb.h>
#include <mql_execution_environment.h>
#include <mql_lexer.h>


#include <cassert>

/////////////////////////////////////////////////////////////////////
//
// MQLResultCallbackOuptut example class
//
/////////////////////////////////////////////////////////////////////

MQLResultCallbackOutput::MQLResultCallbackOutput(MQLExecEnv *pEE, bool bFlushAfterEveryResult)
	: m_pEE(pEE),
	  m_bFlushAfterEveryResult(bFlushAfterEveryResult)
{
	clean();
}


void MQLResultCallbackOutput::clean()
{
	m_bSuccess = false;
	m_bIsTopograph = false;
	m_bFirstLOSHasBeenSeen = false;
}

MQLResultCallbackOutput::~MQLResultCallbackOutput()
{
	// We don't own the m_pEE. Don't delete it.
}


void MQLResultCallbackOutput::printSheafStart(bool hasLOS)
{
	switch (m_pEE->pOut->getOutputKind()) {
	case kOKXML:
	case kOKCompactXML:
		Sheaf::printSheafStartXML(m_pEE->pOut);
		break;

	case kOKJSON:
	case kOKCompactJSON:
		Sheaf::printSheafStartJSON(m_pEE->pOut, hasLOS);
		break;

	case kOKConsole:
		Sheaf::printSheafStartConsole(m_pEE->pOut);
		break;

	default:
		ASSERT_THROW(false, "Unknown eOutputKind");
		break;
	}
}

bool MQLResultCallbackOutput::startSheaf(bool &bStoreSheafInMemory)
{
	bStoreSheafInMemory = false;
	m_bSuccess = false;
	m_bIsTopograph = true;
	return m_pEE->m_bContinueExecution;
}


void MQLResultCallbackOutput::endSheaf(bool bSheafWasFailed)
{
	if (!m_bFirstLOSHasBeenSeen) {
		printSheafStart(false);
	} else {
		if (m_pEE->pOut->isXML() || m_pEE->pOut->isJSON()) {

		} else {
			ListOfStraws::printLOSEndConsole(m_pEE->pOut);
		}
	}
	switch (m_pEE->pOut->getOutputKind()) {
	case kOKXML:
	case kOKCompactXML:
		Sheaf::printSheafEndXML(m_pEE->pOut);
		break;

	case kOKJSON:
	case kOKCompactJSON:
          Sheaf::printSheafEndJSON(m_pEE->pOut, m_bFirstLOSHasBeenSeen);
		break;

	case kOKConsole:
		if (bSheafWasFailed) {
			m_pEE->pOut->out("\n");
		} else {
			// Nothing to do
		}
		break;

	default:
		ASSERT_THROW(false, "Unknown eOutputKind");
		break;
	}
}


bool MQLResultCallbackOutput::nextLOS(const ListOfStraws* pLOS)
{
	m_bSuccess = true;
	if (!m_bFirstLOSHasBeenSeen) {
		printSheafStart(true);
		m_bFirstLOSHasBeenSeen = true;
		if (m_pEE->pOut->isXML() || m_pEE->pOut->isJSON()) {
		} else {
			ListOfStraws::printLOSStartConsole(m_pEE->pOut);
		}
	}

	switch (m_pEE->pOut->getOutputKind()) {
	case kOKXML:
	case kOKCompactXML:
		pLOS->printXML(m_pEE->pOut);
		break;

	case kOKJSON:
	case kOKCompactJSON:
		pLOS->printJSON(m_pEE->pOut);
		break;

	case kOKConsole:
		pLOS->printConsole(m_pEE->pOut, false);
		break;

	default:
		ASSERT_THROW(false, "Unknown eOutputKind");
		break;
	}

	if (m_bFlushAfterEveryResult) {
		m_pEE->pOut->flush();
	}
	return m_pEE->m_bContinueExecution;
}


bool MQLResultCallbackOutput::useResultAfterwards() const
{
	if (m_bIsTopograph) {
		return false;
	} else {
		return true;
	}
}


/////////////////////////////////////////////////////////////////////
//
// MQLExecEnv
//
/////////////////////////////////////////////////////////////////////


Sheaf MQLExecEnv::m_EmptySheaf(false);

/** Constructor
 *
 * @param pMyDB A descendant of EMdFDB (PgEMdFDB, MySQLEMdFDB, 
 *              or SQLiteEMdFDB).
 * @param pMyOut An EMdFOutput object.
 */
MQLExecEnv::MQLExecEnv(EMdFDB* pMyDB, EMdFOutput *pMyOut) 
{
	m_bContinueExecution = true;
	// We do not delete the callback, as it is not our object.
	pCallback = 0;
	pLexer = 0;
	pStatement = 0;
	pDB = pMyDB;
	pOut = pMyOut;
	pOBBVec = 0;
	pError = new MQLError();
	nCompilerStage = COMPILER_STAGE_NONE;
	bQuit = false;
	m_nProgressCount = 0;
	m_node_vector.reserve(256);
}

/** Destructor.
 * 
 * Cleans everything EXCEPT the EMdFDB and EMdFOutput objects.
 * The caller must delete them.
 * NOTE that this is done for you in EmdrosEnv, which is what you
 * should probably use rather than this object directly.
 */
MQLExecEnv::~MQLExecEnv()
{
	clean();
	delete pError;
	delete pStatement;
	delete pLexer;
	
	// Most of the time, pOBBVec will be 0 when we get here,
	// since, in MQLResult's constructor for sheaves, it will
	// have been set to 0.
	// But for the sake of the few cases where we, e.g., have
	// a DB error while doing R_topograph, we had better 
	// clean up our act here as well as in the destructor of
	// the Sheaf.
	if (pOBBVec != 0) {
		for (OBBVec::size_type index = 0; index < pOBBVec->size(); ++index) {
			delete (*pOBBVec)[index];
		}
		delete pOBBVec;
	}
}

/** Clean up and make ready for next statement.
 * \internal
 *
 * Readies the environment for the execution of the next Statement.
 *
 * Is called by EmdrosEnv::clean().  Need not be called by the user,
 * since it is called by the MQL execution machinery just before
 * executing a query.  However, it is good to call this method if you
 * are through with a query's results and there is a long time till
 * the next MQL statement is to be executed.  This frees up resources.
 *
 * @see EmdrosEnv::clean().
 */
void MQLExecEnv::clean(void)
{
	m_bContinueExecution = true;
	// We do not delete the callback, as it is not our object.
	if (pCallback != 0) {
		pCallback->clean();
	}
	delete pLexer; 
	pLexer = 0;
	delete pStatement;
	pStatement = 0;
	nCompilerStage = COMPILER_STAGE_NONE;
	bQuit = false;
	m_nProgressCount = 0;
	m_node_vector.clear();
	m_node_vector.reserve(256);
	m_ORD2NodeNumberMap.clear();
	m_arena.clear();
}

bool MQLExecEnv::updateAll_m_1()
{
	if (pDB == 0) {
		m_all_m_1 = SetOfMonads();

		// Fail silently
		return true;
	} else {
		return pDB->getAll_m_1(m_all_m_1);
	}
}



node_number_t MQLExecEnv::getNextNodeNumber(QueryNode* pNode)
{
	m_node_vector.push_back(pNode);
	node_number_t node_number = m_node_vector.size() - 1;
	pNode->setNodeNumber(node_number);
	return node_number;
}

QueryNode* MQLExecEnv::getNodeFromNodeNumber(node_number_t node_number)
{
	if ((node_number < 0) || (node_number <= (int) m_node_vector.size())) {
		return m_node_vector[node_number];
	} else {
		ASSERT_THROW(false, "MQLExecEnv::getNodeFromNodeNumber() called with node_number out of range.");
		return 0;
	}
}
