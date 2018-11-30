/*
 * mql_execution_environment.cpp
 *
 * MQL execution environment
 *
 * Ulrik Petersen
 * Created: 5/1-2001 (1st of May, 2001)
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
	node_number_t node_number = (node_number_t) m_node_vector.size() - 1;
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
