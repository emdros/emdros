/*
 * mql_result.cpp
 *
 * MQL results
 *
 * Ulrik Petersen
 * Created: 3/8-2001 (8th of March, 2001)
 * Last update: 1/17-2013
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


/**@file mql_result.cpp
 *@brief Implementation of MQLResult (MQL layer)
 */


#include <mql_result.h>
#include <mql_execution_environment.h>
#include <mql_sheaf.h>
#include <emdf_output.h>
#include <debug.h>


// For lists
MQLResult::MQLResult() 
	: m_sheaf(0), 
	  m_flatSheaf(0), 
	  m_bDeleteFullSheaf(true),
	  m_bDeleteFlatSheaf(true)
{
}

// For sheafs
MQLResult::MQLResult(Sheaf* sheaf, MQLExecEnv *pEE) 
	: m_sheaf(sheaf), 
	  m_flatSheaf(0), 
	  m_bDeleteFullSheaf(true),
	  m_bDeleteFlatSheaf(true)
{
	sheaf->setOBBVec(pEE->pOBBVec);
	pEE->pOBBVec = 0;
}

// For sheafs
MQLResult::MQLResult(FlatSheaf* flat_sheaf, MQLExecEnv *pEE) 
	: m_sheaf(0), 
	  m_flatSheaf(flat_sheaf), 
	  m_bDeleteFullSheaf(true),
	  m_bDeleteFlatSheaf(true)
{
	flat_sheaf->setOBBVec(pEE->pOBBVec);
	pEE->pOBBVec = 0;
}

// Used by other methods to take over result
void MQLResult::setDeleteFullSheaf(bool bDoDelete)
{
	m_bDeleteFullSheaf = bDoDelete;
}

// Used by other methods to take over result
void MQLResult::setDeleteFlatSheaf(bool bDoDelete)
{
	m_bDeleteFlatSheaf = bDoDelete;
}

MQLResult::~MQLResult()
{
	if (m_bDeleteFullSheaf)
		delete m_sheaf;
	if (m_bDeleteFlatSheaf)
		delete m_flatSheaf;
}

bool MQLResult::isSheaf() const
{
	return m_sheaf != 0;
}

bool MQLResult::isFlatSheaf() const
{
	return m_flatSheaf != 0;
}

bool MQLResult::isTable() const
{
	return !isSheaf() && !isFlatSheaf();
}

void MQLResult::out(EMdFOutput *pOut) const
{
	switch (pOut->getOutputKind()) {
	case kOKXML:
	case kOKCompactXML:
		if (m_sheaf != 0) {
			m_sheaf->printXML(pOut);
		} else if (m_flatSheaf != 0) {
			m_flatSheaf->printXML(pOut);
		} else {
			Table::printXML(pOut);
		}
		break;
	case kOKJSON:
	case kOKCompactJSON:
		if (m_sheaf != 0) {
            m_sheaf->printJSON(pOut);
		} else if (m_flatSheaf != 0) {
            m_flatSheaf->printJSON(pOut);
		} else {
			Table::printJSON(pOut);
		}
		break;
	case kOKConsole:
		if (m_sheaf != 0) {
			m_sheaf->printConsole(pOut);
		} else if (m_flatSheaf != 0) {
			m_flatSheaf->printConsole(pOut);
		} else {
			Table::printConsole(pOut);
		}
		break;
	default:
		ASSERT_THROW(false, "Unknown eOutputKind");
		break;
	}
}

void MQLResult::printDTD(EMdFOutput* pOut)
{
	pOut->out("  <!ELEMENT mql_results (mql_result)* >");
	pOut->newline();
	pOut->out("  <!ELEMENT mql_result ((mql_progress)?, status, (error)?, (warning)?, (table|sheaf|flatsheaf)?) >");
	pOut->newline();
	pOut->out("  <!ELEMENT mql_progress (#PCDATA)* >");
	pOut->newline();
	pOut->out("  <!ELEMENT status EMPTY >");
	pOut->newline();
	pOut->out("  <!ELEMENT error (error_source, error_stage, ");
	pOut->newline();
	pOut->out("                   error_message_db, error_message_compiler) >");
	pOut->newline();
	pOut->out("  <!ELEMENT error_source EMPTY >");
	pOut->newline();
	pOut->out("  <!ELEMENT error_stage EMPTY >");
	pOut->newline();
	pOut->out("  <!ELEMENT error_message_db (#PCDATA)* >");
	pOut->newline();
	pOut->out("  <!ELEMENT error_message_compiler (#PCDATA)* >");
	pOut->newline();
	pOut->out("  <!ELEMENT warning (#PCDATA)* >");
	pOut->newline();
	pOut->out("  <!ATTLIST mql_results >");
	pOut->newline();
	pOut->out("  <!ATTLIST mql_result  >");
	pOut->newline();
	pOut->out("  <!ATTLIST status ");
	pOut->newline();
	pOut->out("                success (true|false) #REQUIRED >");
	pOut->newline();
	pOut->out("  <!ATTLIST error >");
	pOut->newline();
	pOut->out("  <!ATTLIST error_source ");
	pOut->newline();
	pOut->out("                source  (compiler|db) #REQUIRED >");
	pOut->newline();
	pOut->out("  <!ATTLIST error_stage  ");
	pOut->newline();
	pOut->out("                stage   (none|parse|weed|symbol|type|monads|exec) #REQUIRED >");
	pOut->newline();
	pOut->out("  <!ATTLIST error_message_db >");
	pOut->newline();
	pOut->out("  <!ATTLIST error_message_compiler >");
	pOut->newline();
	pOut->out("  <!ATTLIST warning >");
	pOut->newline();
}


bool MQLResult::flatten(const std::list<std::pair<id_d_t, std::string> >& object_types)
{
	m_flatSheaf = mql_flatten_sheaf(object_types, &m_sheaf);
	if (m_flatSheaf == 0) {
		return false;
	} else {
		return true;
	}
}

bool MQLResult::flatten() // For all object types in the sheaf
{
	m_flatSheaf = mql_flatten_sheaf(&m_sheaf);
	if (m_flatSheaf == 0) {
		return false;
	} else {
		return true;
	}
}

bool MQLResult::flatten(StringList *pObjectTypeNames, EMdFDB *pDB)
{
	m_flatSheaf = mql_flatten_sheaf(pObjectTypeNames, pDB, &m_sheaf);
	if (m_flatSheaf == 0) {
		return false;
	} else {
		return true;
	}
}
