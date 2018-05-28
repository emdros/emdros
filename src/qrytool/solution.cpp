/*
 * solution.cpp
 *
 * Class representing one solution.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 11/10-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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

#include <emdros.h>

#include "solution.h"


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
