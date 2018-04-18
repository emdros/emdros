/*
 * mql_execution_environment.h
 *
 * MQL execution environment
 *
 * Ulrik Petersen
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 1/22-2014
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

/**@file mql_execution_environment.h
 *@brief Header file for MQLExecEnv (MQL layer)
 */


#ifndef MQL_EXECUTION_ENVIRONMENT__H__
#define MQL_EXECUTION_ENVIRONMENT__H__

#include "emdf_output.h"
#include "mql_types.h"
#include "mql_error.h"
#include "mql_query.h"
#include "mql_sheaf.h"
#include "monads.h"
#include <map>
#ifndef SWIG
#include "smart_vector.h"
#include "mql_lexer.h"
#include "llist.h"
#endif // !defined SWIG

/**
 * \defgroup CompilerStages Stages of the MQL compiler
 *\ingroup MQL
 *
 * These compiler stages are stored in MQLExecEnv::nCompilerStage, and
 * can be accessed via EmdrosEnv::getLastCompilerStage().
 */
/*@{*/
#define COMPILER_STAGE_NONE   (0) /**< No stage: Compile not started. */
#define COMPILER_STAGE_PARSE  (1) /**< Parser stage. */
#define COMPILER_STAGE_WEED   (2) /**< Weeder stage. */
#define COMPILER_STAGE_SYMBOL (3) /**< Symbol checker stage. */
#define COMPILER_STAGE_TYPE   (4) /**< Type checker stage. */
#define COMPILER_STAGE_MONADS (5) /**< Monads checker/builder stage. */
#define COMPILER_STAGE_EXEC   (6) /**< Execution stage. */
/*@}*/

class xxFlexLexer; // forward declaration
class QueryNode; // forward declaration

#ifndef SWIG

/** A binary predicate on std::string pointers, returning true iff the
 * two strings compare equal after pointer dereferencing.
 */
struct strptr_equal : public std::equal_to<std::string*> {
	/** Comparison operator.
	 *
	 * @param x The first string.
	 *
	 * @param y The second string.
	 *
	 * @return True iff the strings compare equal after pointer
	 * dereferencing.
	 */
	bool operator() (const std::string* x, const std::string* y) const { return *x == *y; };
};

/** A SmartVector of std::string using strptr_equal to compare strings.
 *
 * @see strptr_equal.
 */
typedef SmartVector<std::string, strptr_equal > StrVec;

#endif // !defined SWIG 

class EMdFDB; // Forward declaration

typedef std::map<std::string, node_number_t> String2NodeNumberMap;

/**
 * MQL Result Callback.
 *
 * Subclass this class in order to decide yourself what to do with MQL
 * Results.
 *
 * The subclassed object can then be passed as a parameter to
 * EmdrosEnv::executeString(), for example, in order to decide what to
 * do with each result-part.
 *
 * This currently only works for topographic queries (SELECT ALL|FOCUS
 * OBJECTS).
 *
 * One application would be to have the ability to not store the whole
 * sheaf in-memory, but to consume the sheaf one top-level "blocks"
 * ListOfStraws at a time.
 *
 * An example of how to use it is present in the Emdros sources in
 * include/mql_execution_environment.h and
 * MQL/mql_execution_environment.cpp, in the form of the
 * MQLResultCallbackOutput class. This, in turn, is used in
 * src/mql.cpp.
 *
 * The idea is that the virtual functions should be overridden in
 * order to decide what to do with each top-level part of the Sheaf,
 * that is, to be precise, each ListOfStraws that comes from one
 * complete matching of the "blocks" part of the topograph.
 *
 *
 * For example, in the following query:
 *
 * [chapter
 *    [word surface = "the"]
 *    ..
 *    [Word surface = "decision"]
 * ]
 *
 * this function would be called once for each complete matching of
 * the outer "chapter" block, not once for each pair of words inside
 * the chapter.
 *
 */
class MQLResultCallback {
 public:
	MQLResultCallback() {};
	virtual ~MQLResultCallback() {};

	/**
	 * Is called just before the processing of a topographic query
	 * starts, i.e., after all objects have been pre-queried, but
	 * before processing of the Inst functions starts.
	 *
	 * @param bStoreSheafInMemory The method must set this to true
	 * iff the method wants the MQL engine to store the sheaf
	 * in-memory.  Set this to false, and the sheaf will not be
	 * stored.  This is useful if you actually consume the sheaf
	 * on-the-fly.
	 *
	 * @return: true if the user wants processing to continue,
	 * false if the user wants the processing to stop.
	 */
	virtual bool startSheaf(bool &bStoreSheafInMemory)  { bStoreSheafInMemory = true; return true; }

	/** For topographic queries: Is called at the end of the
	 * topographic query.
	 *
	 * The boolean parameter indicates whether the sheaf was a
	 * failed sheaf or not. The sheaf may be failed if: a) There
	 * were no hits at all, or b) the user set
	 * MQLExecEnv::m_bContinueExecution to false (or returned
	 * false from either startSheaf() or nextLOS().
	 *
	 * @param bSheafWasFailed indicates whether the sheaf was a
	 * failed sheaf (true) or not (false).  
	 */
	virtual void endSheaf(bool bSheafWasFailed) { UNUSED(bSheafWasFailed); };

	/** For topographic queries, for consuming the next list of
	 * straws from the top-level topograph.  
	 *
	 * If this is called at all, it means the sheaf will not be a
	 * failed sheaf.
	 *
	 * @param pLOS the next top-level ListOfStraws.  You should
	 * not delete this or otherwise manipulate it in a non-const
	 * manner.  It will be deleted by someone else later.
	 *
	 * This ListOfStraws may contain straws which contain
	 * MatchedObjects pObj for which pObj->getRetrieve() returns
	 * false.  Normally, these are sifted out by the MQL engine
	 * before returning the whole result to the user.  Thus, if
	 * you use this callback, you should be aware of the presence
	 * of these MatchedObject objects.
	 * 
	 * @return: true if the user wants processing to continue,
	 * false if the user wants the processing to stop.
	 */
	virtual bool nextLOS(const ListOfStraws* pLOS) {  UNUSED(pLOS); return true; }


	/** Used to report whether to do anything with the results.
	 *
	 * Note that we cannot be sure that we have been dealing with
	 * a sheaf result.
	 *
	 * @return true iff the MQLResult should be used afterwards.
	 */
	virtual bool useResultAfterwards() const { return true; }

	/** Is called from MQLExecEnv::clean(), which in turn is
	 * called by Emdros just before each query is issued.  
	 *
	 * Use this to return the object to whatever state is
	 * necessary for the beginning.
	 */
	virtual void clean() {};
};


/**
 * Example implementation of an MQLResultCallback implementation.
 *
 * Used in src/mql.cpp.
 */
class MQLResultCallbackOutput : public MQLResultCallback {
private:
	MQLExecEnv *m_pEE;
	bool m_bSuccess;
	bool m_bIsTopograph;
	bool m_bFirstLOSHasBeenSeen;
	bool m_bFlushAfterEveryResult;
public:
	MQLResultCallbackOutput(MQLExecEnv *pEE, bool bFlushAfterEveryResult);
	virtual ~MQLResultCallbackOutput();
	virtual bool startSheaf(bool &bStoreSheafInMemory);
	virtual void endSheaf(bool bSheafWasFailed);
	virtual bool nextLOS(const ListOfStraws* pLOS);
	virtual bool useResultAfterwards() const;
	/** Is called from MQLExecEnv::clean(), which in turn is
	 * called by Emdros just before each query is issued.  
	 *
	 * Use this to return the object to whatever state is
	 * necessary for the beginning.
	 */
	virtual void clean();

private:
	void printSheafStart(bool hasLOS);
};




/** MQL execution environment.
 *
 * A class containing all that is necessary to run an MQL session,
 * including an EMdFDB, a Statement, and other objects and data.
 *
 * You should probably not create objects of this type, but rather go
 * through the EmdrosEnv class.
 *
 * See \ref CompilerStages "Stages of the MQL compiler" for
 * information on the MQLExecEnv::nCompilerStage member.
 * 
 */
class MQLExecEnv {
 private:
	short m_nProgressCount;
 public:
	/** Continue execution?
	 *
	 * Is automatically set to true by the constructor (and
	 * clean()).
	 *
	 * If set to false, execution will stop, and a failed sheaf
	 * will be returned.
	 *
	 * Only works with topographic queries, for now.
	 *
	 */
	volatile bool m_bContinueExecution; 

	/**
	 * MQL Result callback object.
	 *
	 * It is used for controlling whether to consume results
	 * on-the-fly, or store them in-memory.
	 *
	 * The MQLExecEnv does not own the pCallback, hence it is not
	 * deleted.
	 *
	 * Don't set this yourself -- it is set by the functions in
	 * MQL/mql_execute.cpp. Instead, pass it to, e.g.,
	 * EmdrosEnv::executeString().
	 */
	MQLResultCallback *pCallback;

	/** Current Arena.
	 *
	 * Is automatically cleared by clean().
	 *
	 */
#ifndef SWIG
	Arena m_arena;
#endif

	/** Current statement
	 */
	Statement *pStatement;     
	/** An empty sheaf
	 * \internal
	 * Used while processing MQL queries and printing results.
	 */
	static Sheaf  m_EmptySheaf; 
	/** Current database The current database.  
	 *
	 * Will, in practice, be a PgEMdFDB, a MySQLEMdFDB, or an
	 * SQLiteEMdFDB.
	 */
	EMdFDB* pDB;
	/** Current output object.
	 */
	EMdFOutput *pOut;
	/** Current error object.
	 */
	MQLError *pError;
	/** Current vector of ObjectBlockBase's
	 */
#ifndef SWIG
	OBBVec *pOBBVec;
#endif
	MQLExecEnv(EMdFDB* pMyDB, EMdFOutput *pMyOut);
	~MQLExecEnv();
	/** Last compiler stage executed.
	 *
	 * \see \ref CompilerStages
	 */
	int nCompilerStage;
	void clean(void);
#ifndef SWIG
	/** lexer.
	 *\internal
	 * The current MQL lexer.  Should NEVER be tampered with by any
	 * user.
	 */
	MQLScanner *pLexer;
	/** all_m-1 of the current database.
	 * \internal
	 * Used while processing an MQL query. Set by QueryBase::myMonads().
	 */
	SetOfMonads m_all_m_1;
	bool updateAll_m_1();
	/** Quit?
	 *\internal
	 * Used to know whether we should quit at the next given opportunity.
	 * Should NEVER be tampered with by the user.  Is set to true by the
	 * MQL QUIT meta-statement.
	 */
	bool bQuit;  // Should we quit at next given opportunity?
	/** Syntax error?
	 *\internal
	 * Used to let yyparse know whether the parser detected a syntax error.
	 */ 
	bool bSyntaxError;
	/** Object Reference Declaration to Node Number map.
	 *\internal
	 *
	 * Maps the lower-case version of an ORD to the node number of
	 * the ObjectBlock which contains the ORD.
	 */
	String2NodeNumberMap m_ORD2NodeNumberMap;


	/** Vector of QueryNode* pointers.
	 *\internal
	 *
	 * Some, but not all, QueryNodes give rise to an entry in this
	 * vector.  The ones that do get in are:
	 *
	 * - BlockString2
	 * - ObjectBlock
	 * - FFeatures
	 */
	QueryNodeVector m_node_vector;
	node_number_t getNextNodeNumber(QueryNode* pNode);
	QueryNode* getNodeFromNodeNumber(node_number_t node_number);
	node_number_t getHighestNodeNumber(void) const { return m_node_vector.size() - 1; };
	
	void progress(void); // Called to show progress
#endif // !defined SWIG
 private:
	static short int addStringToStrVec(const std::string& str, StrVec& vec);
};

#ifndef SWIG
/** Advance progress counter
 * \internal
 *
 * Called from time to time while executing a query.  Increments a 
 * progress counter.  At certain progress-intervals, emit something
 * if we are doing XML output.
 *
 * Should NEVER be called by any user.
 */
inline void MQLExecEnv::progress(void)
{
	m_nProgressCount++;
	if ((m_nProgressCount % 100) == 99) {
		if (pOut->isXML()) {
			m_nProgressCount = 0;
			pOut->out(".");
			pOut->flush();
		}
	}
}

#endif // !defined SWIG

#endif // MQL_EXECUTION_ENVIRONMENT__H__
