/*
 * solution.cpp
 *
 * Class representing one solution.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 11/13-2009
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


#include "solution.h"
#include <debug.h>

Solution::Solution()
{
	m_monads = 0;
	m_focus = 0;
}

Solution::~Solution()
{
	delete m_monads;
	delete m_focus;
	clear_OTTableMap(m_raster);
	clear_OTTableMap(m_data);
	clear_OTTableMap(m_reference);
}

std::string Solution::toString() const
{
	return std::string("monads:") + ((m_monads == 0) ? "none" :  m_monads->toString()) + "\n"
		+ "focus :" + ((m_focus == 0) ? "none" :  m_focus->toString()) + "\n";
	/*
	  + m_raster.toString()
	  + m_data.toString()
	  + m_reference.toString();
	*/
}

void Solution::setMonads(const SetOfMonads& monads)
{
	m_monads = new SetOfMonads(monads);
}

SetOfMonads Solution::getMonads(void) const
{
	return *m_monads;
}

void Solution::setFocus(SetOfMonads  *pFocus)
{
	if (pFocus != 0) {
		m_focus = pFocus;
	}
}

SetOfMonads Solution::getFocus(void) const
{
	if (m_focus == 0) {
		return SetOfMonads();
	} else {
		return *m_focus;
	}
}

void Solution::addMyTable(eSolutionTableKind kind, const std::string& object_type, MyTable *pTable)
{
	std::string OTN;
	str_tolower(object_type, OTN);
	switch (kind) {
	case kRaster:
		m_raster[OTN] = pTable;
		break;
	case kData:
		m_data[OTN] = pTable;
		break;
	case kReference:
		m_reference[OTN] = pTable;
		break;
	default:
		ASSERT_THROW(false, "Unkown eSolutionTableKind");
		break;
	}
}


MyTable *Solution::getMyTable(eSolutionTableKind kind, const std::string& object_type)
{
	std::string OTN;
	str_tolower(object_type, OTN);
	switch (kind) {
	case kRaster:
		return m_raster[OTN];
		break;
	case kData:
		return m_data[OTN];
		break;
	case kReference:
		return m_reference[OTN];
		break;
	default:
		ASSERT_THROW(false, "Unkown eSolutionTableKind");
		return 0; // Just to fool the compiler into giving no warning.
		break;
	}
	return 0; // Fool some compilers into giving no warning.
}

const OTTableMap& Solution::getOTTableMap(eSolutionTableKind kind) const
{
	switch (kind) {
	case kRaster:
		return m_raster;
		break;
	case kData:
		return m_data;
		break;
	case kReference:
		return m_reference;
		break;
	default:
		ASSERT_THROW(false, "Unkown eSolutionTableKind");
		return m_reference; // Just to fool the compiler into giving no warning.
		break;
	}
}
