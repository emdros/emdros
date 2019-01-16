/*
 * environment_emdros.h
 *
 * Emdros environment
 *
 * Ulrik Petersen
 * Created: 3/14-2003
 * Last update: 1/16-2019
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


/**@file environment_emdros.h
 *@brief Header file for EmdrosEnv (MQL layer)
 */


#ifndef ENVIRONMENT_EMDROS__H__
#define ENVIRONMENT_EMDROS__H__

#ifndef SWIG
#include "emdf_output.h"
#include "emdros-lconfig.h"
#include "emdf_enums.h"

#include <istream>
#include <ostream>

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
