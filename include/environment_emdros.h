/*
 * environment_emdros.h
 *
 * Emdros environment
 *
 * Ulrik Petersen
 * Created: 3/14-2003
 * Last update: 8/17-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2013  Ulrik Sandborg-Petersen
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

/**@file environment_emdros.h
 *@brief Header file for EmdrosEnv (MQL layer)
 */


#ifndef ENVIRONMENT_EMDROS__H__
#define ENVIRONMENT_EMDROS__H__

#ifndef SWIG
#include "emdf_output.h"
#include "emdros-lconfig.h"
#include "emdf_enums.h"

#if HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif

#if HAVE_OSTREAM
#include <ostream>
#else
#include <ostream.h>
#endif

#include "emdfdb.h"
#include "table.h"
#include "monads.h"



#endif // !defined SWIG

class SetOfMonads; // Forward declaration
class MQLExecEnv; // Forward declaration
class MQLResultCallback; // Forward declaration
class Table; // Forward declaration
class Sheaf; // Forward declaration
class FlatSheaf; // Forward delcaration
class MQLResult; // Forward declaration
class Statement; // Forward declaration

/**The main entry point into Emdros.
 *@ingroup MQL
 *
 * EmdrosEnv serves as the main class with which to interact with
 * Emdros.
 *
 * It is basically an easy-to-use wrapper around MQLExecEnv, EMdFDB,
 * EMdFOutput, and other EMdF and MQL functions and classes.
 *
 */
class EmdrosEnv {
 protected:
	MQLExecEnv *m_pEE;
 public:
	EmdrosEnv(std::ostream* output_stream, 
		  eOutputKind output_kind, eCharsets charset, 
		  std::string hostname, 
		  std::string user, std::string password, 
		  std::string initial_db,
		  eBackendKind backend = DEFAULT_BACKEND_ENUM);
	// The following constructor uses std::cout (Standard Out)
	EmdrosEnv(eOutputKind output_kind, eCharsets charset, 
		  std::string hostname, 
		  std::string user, std::string password, 
		  std::string initial_db,
		  eBackendKind backend = DEFAULT_BACKEND_ENUM);
	virtual ~EmdrosEnv();

	// Gets backend name as a string
	std::string getBackendName(void);

	// Executing MQL queries
	bool executeString(const std::string& input, 
			   bool &bResult, 
			   bool bPrintResult, 
			   bool bReportError);
	bool executeString(const std::string& input, 
			   bool& bResult, 
			   bool bPrintResult, 
			   bool bReportError,
			   MQLResultCallback *pCallback);
	bool executeFile(std::string filename, 
			 bool& bResult, 
			 bool bPrintResult, 
			 bool bReportError,
			 MQLResultCallback *pCallback = 0);
	bool executeStream(std::istream& strin, 
			   bool& bResult, 
			   bool bPrintResult, 
			   bool bReportError,
			   MQLResultCallback *pCallback = 0);

	// calls MQLExecEnv::clean().
	void clean();

	// Database support
	// Call the corresponding EMdFDB method
	bool connectionOk(void);
	bool vacuum(bool bAnalyze);
	bool getMin_m(monad_m& /* out */ min_m);
	bool getMax_m(monad_m& /* out */ max_m);
	bool getAll_m_1(SetOfMonads& /* out */ all_m_1);


	// Returns the string-representation of an enumeration constant in 
	// enum enum_name with the value value.
	// Just calls the corresponding EMdFDB method.
	virtual std::string getEnumConstNameFromValue(long value,
						      const std::string& enum_name, 
						      /* out */ bool &bDBOK);



	// Transactions
	bool beginTransaction(void);
	bool commitTransaction(void);
	bool abortTransaction(void);

	//
	// Check for results
	//

	// Check for sheaf
	// Returns true on result is sheaf.  
	// Returns false on no result or result not sheaf 
	bool isSheaf(void); 

	// Check for flat sheaf
	// Returns true on result is flat sheaf.  
	// Returns false on no result or result not flat sheaf 
	bool isFlatSheaf(void); 

	// Check for table
	// Returns true on result is table.
	// Returns false on no result or result not table.
	bool isTable(void); 
  

	// NOTE on statements:
	// If only one MQL query was executed by the last executeXXX
	// invocation, then there will be only one statement to get.  
	// If more than one MQL query was given to one of these methods,
	// only the results from the last statement executed will be available.

	// Getting results.  Next invocation of executeXXX deletes the object,
	// so you cannot execute a query until you are done processing
	// the result
	MQLResult* getResult(void); // Return nil on no result
	Sheaf* getSheaf(void); // Returns nil on no result or result not a sheaf 
	FlatSheaf* getFlatSheaf(void); // Returns nil on no result or result not a flat sheaf
	Table* getTable(void); // Returns nil on no result or result not a table
	Statement *getStatement(void); // Returns nil on no statement

#ifdef SWIG
	// Tell SWIG that these methods return something which its shadow
	// classes must own.
	%newobject takeOverResult;
	%newobject takeOverSheaf;
	%newobject takeOverFlatSheaf;
	%newobject takeOverTable;
	%newobject takeOverStatement;
#endif       
	// Take over object.  You now have responsibility for deleting it, 
	// and it will not be deleted by the next invocation of executeXXX.
#ifdef SWIG
	%newobject takeOverResult;
	%newobject taeOverSheaf;
	%newobject takeOverFlatSheaf;
	%newobject takeOverTable;
        %newobject takeOverStatement;
#endif
	MQLResult* takeOverResult(void); // Return nil on no result
	Sheaf* takeOverSheaf(void); // Returns nil on no result or result not a sheaf
	FlatSheaf* takeOverFlatSheaf(void);  // Returns nil on no result or result not a flat sheaf
	Table* takeOverTable(void); // Returns nil on no result or result not a table
	Statement *takeOverStatement(void); // Returns nil on no statement

	// Getting error-messages and info
	std::string getDBError(void);
	std::string getCompilerError(void);
	// Gets the compiler stage that was executed last
	int getLastCompilerStage(void); 
	// clears local DB error message in EMdFDB and local error in MQLError
	void clearErrors(void); 

	// Outputting
	// These all output on the stream in local EMdFOutput,
	// i.e., the stream you passed to the constructor of EmdrosEnv,
	// or stdout if the other constructor was used
	void out(std::string str);  // out your own string
	void out(MQLResult *pResult);
	void out(Sheaf *pSheaf);
	void out(FlatSheaf *pFlatSheaf);
	void out(Table *pTable);
	void out(SetOfMonads *pMonads);

	//
	// XML outputting
	//

	// XML declaration.
	// Calls EMdFOutput::printXMLDecl on the local EMdFOutput
	void printXMLDecl(); 

	// DTDs
	void printDTDStart(std::string root_element);
	void printDTDEnd();
	void printDTDMQLResult(void);
	void printDTDTable(void);
	void printDTDSheaf(void);
	void printDTDFlatSheaf(void);

	// Accessing members
	/** Direct access to the local MQLExecEnv.
	 *
	 * Use this with care.
	 *
	 * @return A pointer to the local MQLExecEnv.
	 */
	MQLExecEnv* getMQLEE(void) { return m_pEE; };
 private:
	// Only used by constructors
	void init(std::ostream* output_stream, 
		  eOutputKind output_kind, eCharsets charset, 
		  std::string hostname, 
		  std::string user, std::string password, 
		  std::string initial_db,
		  eBackendKind backend_kind);
};


#endif // ENVIRONMENT_EMDROS__H__
