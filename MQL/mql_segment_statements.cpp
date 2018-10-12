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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
