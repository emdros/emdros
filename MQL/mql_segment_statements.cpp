/*
 * mql_segment_statements.cpp
 *
 * MQL SEGMENT statements
 *
 * Ulrik Petersen
 * Created: 3/6-2001 (March 6, 2001)
 * Last update: 3/14-2007
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2007  Ulrik Petersen
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

/**@file mql_segment_statements.cpp
 *@brief Implementation of MQL segment statements (MQL layer)
 */

#include <emdfdb.h>
#include <mql_segment_statements.h>



/////////////////////////////////////////////////////////////
//
// SegmentStatement
//
// base class
//
////////////////////////////////////////////////////////////
SegmentStatement::~SegmentStatement()
{
	delete m_segment_name;
}


/////////////////////////////////////////////////////////////
//
// CREATE SEGMENT
//
// CreateSegmentStatement
//
////////////////////////////////////////////////////////////
CreateSegmentStatement::CreateSegmentStatement(MQLExecEnv *pEE,
					       std::string* segment_name, 
					       MQLMonadSetElement* segment_range) 
	: SegmentStatement(pEE, segment_name),
	  m_segment_range(segment_range)
{

}

CreateSegmentStatement::~CreateSegmentStatement()
{
	delete m_segment_range;
}

//
// - Checks that the range is monotonic.
// 
// - Checks that the range is positive
//
void CreateSegmentStatement::weed(bool& bResult)
{
	//
	// - Checks that the range is monotonic and positive
	// 
	m_segment_range->weed(m_pEE, true, bResult);

	// bResult is set, which is all we "return"
	return;
}

bool CreateSegmentStatement::symbol(bool& bResult)
{
	bool bExists;
	SetOfMonads dummySOM;
	if (!m_pEE->pDB->monadSetExists(*m_segment_name, bExists, dummySOM)) {
		m_pEE->pError->appendError("DB error checking existence of monad set with name '" + *m_segment_name + "'.\n");
		return false;
	} else {
		if (bExists) {
			// It exists. Complain and return with compilation error
			m_pEE->pError->appendError("monad set with name '" + *m_segment_name + "' exists already.\nCannot create.\n");
			bResult = false;
			return true;
		} else {
			// It does not exist.  Everything is fine.
		}
	}

	// If we came this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}


//
// - Create the segment
//
bool CreateSegmentStatement::exec()
{
	//
	// - Create the segment
	//
	if (!m_pEE->pDB->createSegment(*m_segment_name,
				       m_segment_range->getMseFirst(), 
				       m_segment_range->getMseLast()))
		return false;

	// If we got this far, we are a success.
	return true;
}
