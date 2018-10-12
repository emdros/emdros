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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
