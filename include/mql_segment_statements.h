/*
 * mql_segment_statements.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 3/14-2007
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2002-2007  Ulrik Petersen
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

/**@file mql_segment_statements.h
 *@brief Header file for MQL segment statements (MQL layer)
 */


#ifndef MQL_SEGMENT_STATEMENTS__H__
#define MQL_SEGMENT_STATEMENTS__H__

#include "mql_types.h"
#include "mql_execution_environment.h"


// Base class for the segment manipulation statements
class SegmentStatement : public Statement {
 protected:
	std::string* m_segment_name;
 public:
	SegmentStatement(MQLExecEnv *pEE, std::string* segment_name) : Statement(pEE), m_segment_name(segment_name) {};
		virtual ~SegmentStatement();
};

// CREATE SEGMENT
class CreateSegmentStatement : public SegmentStatement {
 private:
	MQLMonadSetElement* m_segment_range;
 public:
	CreateSegmentStatement(MQLExecEnv *pEE, 
			       std::string* segment_name, 
			       MQLMonadSetElement* segment_range);
	virtual ~CreateSegmentStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool exec();
};


#endif /* MQL_SEGMENT_STATEMENTS__H__ */
