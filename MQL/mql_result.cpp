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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2013  Ulrik Sandborg-Petersen
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
