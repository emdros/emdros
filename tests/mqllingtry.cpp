/*
 * mqllingtry.cpp
 *
 * Linguistic trial of MQL and EMdF framework
 * Created: 3/30-2005
 * Last update: 5/11-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2018  Ulrik Sandborg-Petersen
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

#include <emdros_environment.h>
#include <encryption.h>
#include <iostream>
#include <string>
#include <monads.h>
#include <emdf.h>
#include <map>
#include <deque>
#include <list>
#include <string_func.h>
#include <string_list.h>
#include <mql_result.h>
#include <table.h>
#include <mql_sheaf.h>

#include <opt.h>


static std::string NEWLINE_STRING("\n");

void show_version(std::ostream& ostr)
{
	ostr << "mqllingtry from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	ostr << "Usage:" << std::endl;
	ostr << "   mqllingtry [options] " << std::endl;
	ostr << "OPTIONS:" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "DEFAULTS:" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
}

void show_DB_error(EmdrosEnv *pEnv) {
	std::cerr << "DB Error:" << std::endl;
	std::string strDBError;
	strDBError = pEnv->getDBError();
	std::cerr << strDBError << std::endl;
}

bool execute_query(EmdrosEnv *pEnv, 
		   const std::string& query, 
		   bool showResult) {
	// Show query
	if (showResult) {
		std::cout << "Executing query: '" + query + "'" << std::endl;
	}

  
	// Execute query
	bool bCompilerResult;
	bool bDBResult;
	try {
		bDBResult = pEnv->executeString(query, bCompilerResult, showResult, false);
	} catch (EMdFDBDBError& e){
		std::cerr << e.what() << std::endl;
		show_DB_error(pEnv);
		return false;
	} catch (EMdFDBException& e){
		std::cerr << e.what() << std::endl;
		show_DB_error(pEnv);
		return false;
	} catch (TableColumnException& e){
		std::cerr << e.what() << std::endl;
		show_DB_error(pEnv);
		return false;
	} catch (TableRowException& e){
		std::cerr << e.what() << std::endl;
		show_DB_error(pEnv);
		return false;
	} catch (TableException& e){
		std::cerr << e.what() << std::endl;
		show_DB_error(pEnv);
		return false;
	} catch (WrongCharacterSetException& e){
		std::cerr << e.what() << std::endl;
		show_DB_error(pEnv);
		return false;
	} catch (EMdFOutputException& e){
		std::cerr << e.what() << std::endl;
		show_DB_error(pEnv);
		return false;
	} catch (BadMonadsException& e){
		std::cerr << e.what() << std::endl;
		show_DB_error(pEnv);
		return false;
	}
  
	// Show success-info
	if (showResult) {
		std::cout << "DB sucess: " << bDBResult << ", Compiler success: " << bCompilerResult << std::endl;
	}
  
	// Show DB error, if any
	if (!bDBResult) {
		show_DB_error(pEnv);
	}

	// Show compiler error, if any
	if (!bCompilerResult) {
		std::cerr << "Compiler error:" << std::endl;
		std::string strCompilerError;
		strCompilerError = pEnv->getCompilerError();
		std::cerr << strCompilerError << std::endl;
		std::cerr << "Query was:'" << std::endl;
		std::cerr << query + "'" << std::endl;
	}
  
	// Return success-info
	return !(!bCompilerResult || !bDBResult);
}

bool execute_query(EmdrosEnv *pEnv, 
		   const std::string& query, 
		   std::string success, 
		   bool showResult) {
	bool bResult = execute_query(pEnv, query, showResult);
	if (bResult) {
		std::cout << "SUCCESS in " + success << std::endl;
	} else {
		std::cerr << "FAILURE in " + success << std::endl;
	}
	return bResult;
}


class EmdrosObject {
protected:
	SetOfMonads monads;
	int object_id_d;

public:
	EmdrosObject() {
		object_id_d = NIL;
	};

	EmdrosObject(int id_d) {
		object_id_d = id_d;
	};
	virtual ~EmdrosObject() {};
  
	void addMonad(monad_m monad) {
		monads.add(monad);
	};

	SetOfMonads getMonads() { return monads; };

	int getID_D() { return object_id_d; };

	void setID_D(int oid) { object_id_d = oid; };

	bool createObject(EmdrosEnv *pEnv) {	
		// Build query
		std::string query;
		query = std::string("CREATE OBJECT") + NEWLINE_STRING
			+ "FROM MONADS = " + monads.toString() + NEWLINE_STRING
			+ "[" + objectType() + NEWLINE_STRING
			+ featureString()
			+ "]" + NEWLINE_STRING
			+ "GO";
    
		// Execute query
		if (!execute_query(pEnv, query, false)) 
			return false;
    
		// Get result
		MQLResult *pResult = pEnv->getResult();
		if (!pResult->isTable()) {
			std::cerr << "Error: Was expecting table as result." << std::endl;
			return false;
		}
   
		Table *pTable = pResult->getTable();
    
		// Get first column of first row
		TableIterator it = pTable->iterator();
		std::string object_id_d_str = ((Table*)pTable)->getColumn(it, 1);
	    
		// Set object_id_d
		object_id_d = string2id_d(object_id_d_str);
    
		// If we got this far, there were no errors
		return true;
	};
	std::string virtual objectType() = 0;
	std::string virtual featureString() = 0;
};



class Phrase : public EmdrosObject {
protected:
	std::string phraseType;

public:
	Phrase() {
		phraseType = "Not yet set";
	}

	Phrase(int oid) : EmdrosObject(oid) {
		phraseType = "Not yet set";
	}

	Phrase(const std::string& phrType) {
		phraseType = phrType;
	}

	virtual ~Phrase() {};

	std::string getPhraseType() { return phraseType; };

	void setPhraseType(const std::string& phrasetype) { phraseType = phrasetype; };
	std::string objectType() { return "Phrase"; }

	std::string featureString() { 
		std::string result;
		result = "  phrase_type := " + phraseType + ";" + NEWLINE_STRING;
		return result;
	}
};

class Word : public EmdrosObject {
protected:
	std::string surface;
	std::string psp;
public:
	Word() {
		surface = "Not yet set";
		psp = "Not yet set";
	};
	
	Word(int oid) : EmdrosObject(oid) {
		surface = "Not yet set";
		psp = "Not yet set";
	};

	Word(const std::string& surf, const std::string& p) {
		surface = surf;
		psp = p;
	};

	virtual ~Word() {};

	std::string getSurface() { return surface; };

	void setSurface(const std::string& surf) { surface = surf; };

	std::string getPsp() { return psp; };

	void setPsp(const std::string& p) { psp = p; };

	std::string objectType() { return "Word"; }
	
	std::string featureString() { 
		std::string result;
		result = "  surface := \"" + surface + "\";" + NEWLINE_STRING
			+ "  psp := " + psp + ";" + NEWLINE_STRING;
		return result;
	}
};

typedef std::map<id_d_t, EmdrosObject*> MyMap;

void clear_MyMap(MyMap& mymap)
{
	MyMap::iterator it = mymap.begin();
	while (it != mymap.end()) {
		delete it->second;
		++it;
	}
}


bool create_database(EmdrosEnv *pEnv) {
	// Create database
	if (!execute_query(pEnv, "CREATE DATABASE EMDF6 GO", "creating database", false)) 
		return false;
  
	// Create enumeration psp_t
	std::string query;
	query = "CREATE ENUMERATION psp_t = {" + NEWLINE_STRING
		+ "  psp_None, Noun, Verb, ProperNoun, Adjective, Adverb, Preposition," + NEWLINE_STRING
		+ "  Demonstrative, Pronoun, Article" + NEWLINE_STRING
		+ "}" + NEWLINE_STRING
		+ "GO";
	if (!execute_query(pEnv, query, "creating psp_t enumeration", false))
		return false;
  
	// Create enumeration phrase_type_t
	query = "CREATE ENUMERATION phrase_type_t = {" + NEWLINE_STRING
		+ "  pt_None, NP, VP, AP, AdvP, PP" + NEWLINE_STRING
		+ "}" + NEWLINE_STRING
		+ "GO";
	if (!execute_query(pEnv, query, "creating phrase_type_t enumeration", false))
		return false;
  
	// Create object type Word
	query = "CREATE OBJECT TYPE" + NEWLINE_STRING
		+ "[Word" + NEWLINE_STRING
		+ "  surface : ASCII;" + NEWLINE_STRING
		+ "  psp : psp_t;" + NEWLINE_STRING
		+ "]" + NEWLINE_STRING
		+ "GO";
	if (!execute_query(pEnv, query, "creating object type Word", false))
		return false;
  
	// Create object type Clause
	query = "CREATE OBJECT TYPE" + NEWLINE_STRING
		+ "[Phrase" + NEWLINE_STRING
		+ "   phrase_type : phrase_type_t;" + NEWLINE_STRING
		+ "]" + NEWLINE_STRING
		+ "GO";
	if (!execute_query(pEnv, query, "creating object type Phrase", false))
		return false;
  
	// If we got this far, there were no errors
	return true;
}

bool printSheaf(const Sheaf *pSheaf, MyMap& phraseHM, MyMap& wordHM); // Forward declaration


bool printPhrase(const MatchedObject *pMo, MyMap& phraseHM, MyMap& wordHM) {
	// Get object id_d
	id_d_t object_id_d = pMo->getID_D();
  
	// Get phrase and check that it isn't null
	MyMap::iterator it = phraseHM.find(object_id_d);
	if (it == phraseHM.end()) {
		std::cerr << "Error: phrase with id_d " << object_id_d << "was not found in map." << std::endl;
		return false;
	}

	Phrase *pPhr = (Phrase*) phraseHM[object_id_d];

	std::string phrase_type = pPhr->getPhraseType();
	std::cout << "[" + phrase_type + " ";
	if (!printSheaf(pMo->getSheaf(), phraseHM, wordHM))
		return false;
	std::cout << phrase_type + "]" << std::endl;
	return true;
}

bool printWord(const MatchedObject *pMo, MyMap& wordHM) {
	// Get object id_d
	id_d_t object_id_d = pMo->getID_D();
  
	// Get phrase and check that it isn't null
	MyMap::iterator it = wordHM.find(object_id_d);
	if (it == wordHM.end()) {
		std::cerr << "Error: word with id_d " << object_id_d << "was not found in map." << std::endl;
		return false;
	}

	Word *pWord = (Word*) wordHM[object_id_d];

	// Get surface
	std::string surface = pWord->getSurface();
	std::cout << surface + " ";
	return true;
}

bool printMatchedObject(const MatchedObject *pMo, MyMap& phraseHM, MyMap& wordHM) {
	if (pMo->getKind() == kMOKID_D) {
		// So the matched object matched an object block.
		std::string object_type_name = pMo->getObjectTypeName();
		if (object_type_name == "phrase") {
			return printPhrase(pMo, phraseHM, wordHM);
		} else if (object_type_name == "word") {
			return printWord(pMo, wordHM);
		} else {
			std::cerr << "Error: object_type_name was '" + object_type_name + "', which wasn't either 'phrase' or 'word'." << std::endl;
			return false;
		}
	} else {
		// We shouldn't get here: All the pMo's in this
		// example should be of kind ID_D
		std::cerr << "ERROR: pMo->getKind() wasn't kMOKID_D." << std::endl;
		return false;
	}
}

bool printStraw(const Straw *pStraw, MyMap& phraseHM, MyMap& wordHM) {
	StrawConstIterator sci = pStraw->const_iterator();
	while (sci.hasNext()) {
		if (!printMatchedObject(sci.next(), phraseHM, wordHM)) 
			return false;
	}
	return true;
}

bool printSheaf(const Sheaf *pSheaf, MyMap& phraseHM, MyMap& wordHM) {
	SheafConstIterator sci = pSheaf->const_iterator();
	while (sci.hasNext()) {
		const Straw *pStraw = sci.next();
		if (!printStraw(pStraw, phraseHM, wordHM)) 
			return false;
	}

	// If we got this far, there were no errors
	return true;
}

bool gatherSheaf(const Sheaf *pSheaf, MyMap& phraseHM, MyMap& wordHM); // Forward declaration


bool gatherPhrase(const MatchedObject *pMo, MyMap& phraseHM, MyMap& wordHM) {
	id_d_t object_id_d = pMo->getID_D();
	Phrase *pPhr = new Phrase(object_id_d);
	if (phraseHM.find(object_id_d) == phraseHM.end()) {
		phraseHM[object_id_d] = pPhr;
	} else {
		delete pPhr;
	}
	if (!gatherSheaf(pMo->getSheaf(), phraseHM, wordHM))
		return false;
	return true;
}

bool gatherWord(const MatchedObject *pMo, MyMap& wordHM) {
	id_d_t object_id_d = pMo->getID_D();
	Word *pWord = new Word(object_id_d);
	if (wordHM.find(object_id_d) == wordHM.end()) {
		wordHM[object_id_d] = pWord;
	} else {
		delete pWord;
	}
	return true;
}

bool gatherMatchedObject(const MatchedObject *pMo, MyMap& phraseHM, MyMap& wordHM) {
	if (pMo->getKind() == kMOKID_D) {
		// So the matched object matched an object block.
		std::string object_type_name = pMo->getObjectTypeName();
		if (object_type_name == "phrase") {
			return gatherPhrase(pMo, phraseHM, wordHM);
		} else if (object_type_name == "word") {
			return gatherWord(pMo, wordHM);
		} else {
			std::cerr << "Error: object_type_name was '" + object_type_name + "', which wasn't either 'phrase' or 'word'." << std::endl;
			return false;
		}
	} else {
		// We shouldn't get here: All the pMo's in this
		// example should be of kind ID_D
		std::cerr << "ERROR: pMo->getKind() wasn't kMOKID_D." << std::endl;
		return false;
	}
}

bool gatherStraw(const Straw *pStraw, MyMap& phraseHM, MyMap& wordHM) {
	StrawConstIterator sci = pStraw->const_iterator();
	while (sci.hasNext()) {
		if (!gatherMatchedObject(sci.next(), phraseHM, wordHM)) 
			return false;
	}
	return true;
}

bool gatherSheaf(const Sheaf *pSheaf, MyMap& phraseHM, MyMap& wordHM) {
	SheafConstIterator sci = pSheaf->const_iterator();
	while (sci.hasNext()) {
		const Straw *pStraw = sci.next();
		if (!gatherStraw(pStraw, phraseHM, wordHM)) 
			return false;
	}
  
	// If we got this far, there were no errors
	return true;
}

std::string getID_DString(MyMap& HM) {
	// Declare string result
	std::string result;

	int i = 0;
	for (MyMap::const_iterator ci = HM.begin(); ci != HM.end(); ) {
		// Get id_d from iterator
		id_d_t id_d = ci->first;

		// Add it to the query
		result += id_d2string(id_d);

		++ci;
		++i;
    
		// See if we should output comma
		if (ci != HM.end()) {
			result += ", ";
		}
    
		// See if we should output newline
		if ((i % 5) == 4) {
			result += NEWLINE_STRING;
		}
	}
  
	return result;
  
}

bool fillPhrases(EmdrosEnv *pEnv, MyMap& phraseHM) {
	// get and checkID_D string
	std::string id_dString = getID_DString(phraseHM);
	if (id_dString == "") {
		std::cerr << "Error: there were no id_ds." << std::endl;
		return false;
	}

	// Build query
	std::string query = "GET FEATURES phrase_type" + NEWLINE_STRING
		+ "FROM OBJECTS" + NEWLINE_STRING
		+ "WITH ID_DS = " + id_dString + NEWLINE_STRING
		+ "[Phrase]" + NEWLINE_STRING
		+ "GO";
  
	// execute query
	if (!execute_query(pEnv, query, false)) {
		return false;
	}
  
	// Take over result
	MQLResult *pResult = pEnv->takeOverResult();
  
	// Check that result is table
	if (!pResult->isTable()) {
		std::cerr << "Error: result wasn't a table." << std::endl;
		delete pResult;
		return false;
	}

	// Iterate over table
	Table *pTable = pResult->getTable();
	TableIterator it = pTable->iterator();
	try {
		while (it.hasNext()) {
			// Get current table row
			TableRow trow = it.current();
		
			// Get object_id_d as id_d
			std::string strOid = trow.getColumn(1);
			id_d_t id_d = string2id_d(strOid);
      
			// Get phrase type
			std::string phrase_type = trow.getColumn(2);
      
			// Get phrase object
			Phrase *pPhr = (Phrase*) phraseHM[id_d];
      
			// Update phrase
			pPhr->setPhraseType(phrase_type);
      
			// Advance iterator
			it.next();
		}
	} catch (TableException& e) {
		std::cerr << e.what() << std::endl;
		delete pResult;
		return false;
	}

	delete pResult;
	
	// If we got this far, there were no errors
	return true;
}
    
bool fillWords(EmdrosEnv *pEnv, MyMap& wordHM) {
	// Build query
	std::string query = "GET FEATURES surface, psp" + NEWLINE_STRING
		+ "FROM OBJECTS" + NEWLINE_STRING
		+ "WITH ID_DS = " + getID_DString(wordHM) + NEWLINE_STRING
		+ "[Word]" + NEWLINE_STRING
		+ "GO";
  
	// execute query
	if (!execute_query(pEnv, query, true)) {
		return false;
	}
  
	// Get statement and result
	MQLResult *pResult = pEnv->takeOverResult();
	
	// Check that result is table
	if (!pResult->isTable()) {
		std::cerr << "Error: result wasn't a table" << std::endl;
		delete pResult;
		return false;
	}
  
	// Iterate over table
	Table *pTable = pResult->getTable();
	TableIterator it = pTable->iterator();
	try {
		while (it.hasNext()) {
			// Get current table row
			TableRow trow = it.current();

			// Get object_id_d as myInt
			// Get object_id_d as id_d
			std::string strOid = trow.getColumn(1);
			id_d_t id_d = string2id_d(strOid);
      
			// Get surface
			std::string surface = trow.getColumn(2);
      
			// Get psp
			std::string psp = trow.getColumn(3);
      
			// Get word object
			Word *pW = (Word*) wordHM[id_d];
      
			// Update word
			pW->setSurface(surface);
			pW->setPsp(psp);
      
			// Advance iterator
			it.next();
		}
	} catch (TableException& e) {
		std::cerr << e.what() << std::endl;
		delete pResult;
		return false;
	}

	delete pResult;
  
	// If we got this far, there were no errors
	return true;
}

bool harvestSheaf(EmdrosEnv *pEnv, Sheaf *pSheaf, MyMap& phraseHM, MyMap& wordHM) 
{
	// Gather object id_ds and empty objects
	if (!gatherSheaf(pSheaf, phraseHM, wordHM))
		return false;

	// Test phraseHM
	if (phraseHM.size() == 0) {
		std::cerr << "Error: there were no phrases." << std::endl;
		return false;
	}
  
	// Test wordHM
	if (wordHM.size() == 0) {
		std::cerr << "Error: there were no words." << std::endl;
		return false;
	}
  
	// Fill phase objects with values
	if (!fillPhrases(pEnv, phraseHM))
		return false;

	// Fill word objects with values
	if (!fillWords(pEnv, wordHM)) 
		return false;

	return true;
}

bool query_phrases(EmdrosEnv *pEnv, const std::string& phrase_type) {
	// out opening message
	std::cout << "Attempting to get all phrases of type " + phrase_type << std::endl;

	// Create query-string
	std::string query;
	query = "SELECT ALL OBJECTS" + NEWLINE_STRING
		+ "WHERE" + NEWLINE_STRING
		+ "[Phrase" + NEWLINE_STRING
		+ "  phrase_type = " + phrase_type + NEWLINE_STRING
		+ "  [Word]" + NEWLINE_STRING
		+ "]" + NEWLINE_STRING
		+ "GO";

	// Execute query
	if (!execute_query(pEnv, query, true)) {
		std::cerr << "Query failed!" << std::endl;
		return false;
	}

	std::cout << "Query executed. Now taking over sheaf" << std::endl;

	// Take over sheaf
	Sheaf *pSheaf = pEnv->takeOverSheaf();

	std::cout << "Sheaf taken over. Now harvesting sheaf" << std::endl;

	// Harvest sheaf
	MyMap phraseHM = MyMap();
	MyMap wordHM = MyMap();
	if (!harvestSheaf(pEnv, pSheaf, phraseHM, wordHM)) {
		std::cerr << "Could not harvest!" << std::endl;
		clear_MyMap(phraseHM);
		clear_MyMap(wordHM);
		delete pSheaf;
		return false;
	}
  
	std::cout << "Sheaf harvested. Now printing sheaf" << std::endl;
  
	// out sheaf
	std::cout << "================================================================" << std::endl;
	printSheaf(pSheaf, phraseHM, wordHM);
	std::cout << "================================================================" << std::endl;

	// If we got this far, there were no errors
	std::cout << "SUCCESS in querying phrase-type " + phrase_type << std::endl;

	clear_MyMap(phraseHM);
	clear_MyMap(wordHM);
	delete pSheaf;

	return true;
}

bool query_enumeration(EmdrosEnv *pEnv, const std::string& myenum)
{
	UNUSED(pEnv);
	
	// out opening message
	std::cout << "Attempting to get table of enumeration constants in " + myenum << std::endl;
	// If we got this far, there were no errors
	return true;
}

bool query_database(EmdrosEnv *pEnv)
{
	if (!query_phrases(pEnv, "NP")) 
		return false;

	if (!query_phrases(pEnv, "AP")) 
		return false;

	if (!query_enumeration(pEnv, "psp_t")) {
		return false;
	}

	// If we got this far, there were no errors
	return true;
}

bool drop_database(EmdrosEnv *pEnv) {
	return execute_query(pEnv, "DROP DATABASE EMDF6 GO", "dropping database", false);
}

bool fill_database_single_word(EmdrosEnv *pEnv,
			       const std::string& surface, 
			       const std::string& psp, 
			       monad_m monad) {
	Word *pWd = new Word(surface, psp);
	pWd->addMonad(monad);
	pWd->createObject(pEnv);
	delete pWd;
	
	// If we got this far, there were no errors
	return true;
}

typedef std::deque<Phrase*> PhraseStack;

void clear_PhraseStack(PhraseStack& ps)
{
	PhraseStack::iterator it = ps.begin();
	while (it != ps.end()) {
		delete *it;
		++it;
	}
}

bool fill_database(EmdrosEnv *pEnv, const StringList& words) {
	monad_m monad = 1;
	PhraseStack phrase_stack;
	StringListConstIterator ci = words.const_iterator();
	while (ci.hasNext()) {
		std::string w1 = ci.next();
		std::string w2 = ci.next();
		if (w1 == "[") {
			// Open phrase
			Phrase *pPhrase = new Phrase(w2);
			phrase_stack.push_back(pPhrase);
		} else if (w1 == "]") {
			Phrase *pPhrase = phrase_stack.back();
			phrase_stack.pop_back();
			if (pPhrase->getPhraseType() != w2) {
				std::cerr << "Error in word string: Closing phrase type was '" 
					+ w2 + "', expected '"
					+ pPhrase->getPhraseType() + "'" << std::endl;
				delete pPhrase;
				clear_PhraseStack(phrase_stack);
				return false;
			}
			if (!pPhrase->createObject(pEnv)) {
				delete pPhrase;
				clear_PhraseStack(phrase_stack);
				return false;
			}
			delete pPhrase;
		} else {
			// Make word
			if (!fill_database_single_word(pEnv, w1, w2, monad)) {
				clear_PhraseStack(phrase_stack);
				return false;
			}
      
			// Add monad to all phrases
			for (PhraseStack::const_iterator ci = phrase_stack.begin();
			     ci != phrase_stack.end();
			     ++ci) {
				Phrase *pPhrase = (Phrase*) *ci;
				pPhrase->addMonad(monad);
			}
      
			// Increment monad
			++monad;
		}
	}
	clear_PhraseStack(phrase_stack);
	return true;
}

int test_backend(eBackendKind backend_kind, 
		 const std::string& hostname, 
		 const std::string& user, 
		 const std::string& password,
		 const std::string& initial_db)
{
	std::cout << "----------------------------------\n"
		  << "Testing backend " << backend_kind2string(backend_kind) << '\n'
		  << "----------------------------------" << std::endl;


	// Make execution environment
	EmdrosEnv *pEnv;
	pEnv = new EmdrosEnv(kOKConsole, kCSISO_8859_1,
			     hostname,
			     user, password,
			     initial_db,
			     backend_kind);
  
	bool bConnectionOk = pEnv->connectionOk();
	if (!bConnectionOk) {
		show_DB_error(pEnv);
		std::cerr << "Connection could not be established." << std::endl;
		delete pEnv;
		return 1;
	}
	std::cerr << "Connection established." << std::endl;
  
	// Create database
	if (!create_database(pEnv)) {
		drop_database(pEnv);
		if (!create_database(pEnv)) {
			delete pEnv;
			return 1;
		}
	}
  
	// Fill database
	StringList words;
	words.addStringBack("[");
	words.addStringBack("NP");
	words.addStringBack("[");
	words.addStringBack("NP");
	words.addStringBack("The");
	words.addStringBack("Article");
	words.addStringBack("door");
	words.addStringBack("Noun");
	words.addStringBack("]");
	words.addStringBack("NP");
	words.addStringBack("[");
	words.addStringBack("PP");
	words.addStringBack("on");
	words.addStringBack("Preposition");
	words.addStringBack("[");
	words.addStringBack("NP");
	words.addStringBack("the");
	words.addStringBack("Article");
	words.addStringBack("East");
	words.addStringBack("ProperNoun");
	words.addStringBack("]");
	words.addStringBack("NP");
	words.addStringBack("]");
	words.addStringBack("PP");
	words.addStringBack("]");
	words.addStringBack("NP");
	words.addStringBack("[");
	words.addStringBack("VP");
	words.addStringBack("was");
	words.addStringBack("Verb");
	words.addStringBack("[");
	words.addStringBack("AP");
	words.addStringBack("blue.");
	words.addStringBack("Adjective");
	words.addStringBack("]");
	words.addStringBack("AP");
	words.addStringBack("]");
	words.addStringBack("VP");
  
	if (!fill_database(pEnv, words)) {
		std::cerr << "FAILURE in creating objects" << std::endl;
		drop_database(pEnv);
		delete pEnv;
		return 1;
	} else {
		std::cout << "SUCCESS in creating objects" << std::endl;
	}
  
	// Attempt querying
	if (!query_database(pEnv)) {
		std::cerr << "FAILURE in querying database" << std::endl;
		drop_database(pEnv);
		delete pEnv;
		return 1;
	} else {
		std::cout << "SUCCESS in querying database" << std::endl;
	}
  
  
	// Drop database
	drop_database(pEnv);

	delete pEnv;
  
	// out success message
	std::cout << "SUCCESS: All tests completed successfully on backend " << backend_kind2string(backend_kind) << '.' << std::endl;
	return 0;
}

int main(int argc, char *argv[]) 
{
	int result = 0;
	std::string hostname;
	std::string password;
	std::string user;
	std::string initial_db("emdf");
	eBackendKind backend_kind = kBackendNone;
	eCharsets dummy_charset;
	bool bTestedSQLite3 = false;
	bool bTestedPostgreSQL = false;
	bool bTestedMySQL = false;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	// Parse arguments
	std::list<std::string> surplus_arguments;

	std::string error_message;
	if (!parseArguments(argc, argv, error_message, surplus_arguments)) {
		print_usage(std::cerr);
		std::cerr << error_message << std::endl;
		return 1;
	} else {
		bool bShowVersion;
		bool bShowHelp;
		std::string error_message;
		if (!getStandardArguments(bShowVersion, bShowHelp,
					  hostname,
					  user,
					  password,
					  backend_kind,
					  dummy_charset,
					  error_message)) {
			print_usage(std::cerr);
			std::cerr << error_message << std::endl;
			return 1;
		}

		// -V | --version
		if (bShowVersion) {
			show_version(std::cout);
			return 0;
		}

		// --help
		if (bShowHelp) {
			print_usage(std::cout);
			return 0;
		}
		
		// Did the user specify a backend?
		if (!getArgumentPresent("-b")) {
			// No, so we say it's "kBackenNone" to indicate
			// further down that we want them all...
			backend_kind = kBackendNone;
		}

		if (surplus_arguments.size() != 0) {
			print_usage(std::cerr);
			std::cerr << "ERROR: Extraneous argument(s) not parsed: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}

#if USE_SQLITE3
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kSQLite3)) {
		bTestedSQLite3 = true;
		result = test_backend(kSQLite3, hostname, user, password, initial_db);
		if (result != 0) {
			goto end_backends;
		}
	}
#else
	if (backend_kind == kSQLite3) {
		std::cerr << "ERROR: SQLite3 backend selected, but SQLite3 support not compiled in." << std::endl;
		return 1;
	}
#endif
#if USE_POSTGRESQL
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kPostgreSQL)) {
		bTestedPostgreSQL = true;
		result = test_backend(kPostgreSQL, hostname, user, password, initial_db);
		if (result != 0) {
			goto end_backends;
		}
	}
#else
	if (backend_kind == kPostgreSQL) {
		std::cerr << "ERROR: PostgreSQL backend selected, but PostgreSQL support not compiled in." << std::endl;
		return 1;
	}
#endif
#if USE_MYSQL
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kMySQL)) {
		bTestedMySQL = true;
		result = test_backend(kMySQL, hostname, user, password, initial_db);
		if (result != 0) {
			goto end_backends;
		}
	}
#else
	if (backend_kind == kMySQL) {
		std::cerr << "ERROR: MySQL backend selected, but MySQL support not compiled in." << std::endl;
		return 1;
	}
#endif

end_backends:
	std::cout << "Backends tested:" << std::endl;
	if (bTestedSQLite3) {
		std::cout << "                 - SQLite 3.X.X" << std::endl;
	}
	if (bTestedPostgreSQL) {
		std::cout << "                 - PostgreSQL" << std::endl;
	}
	if (bTestedMySQL) {
		std::cout << "                 - MySQL" << std::endl;
	}

	if (result == 0) {
		std::cout << "SUCCESS: All tests completed successfully." << std::endl;
	} else {
		std::cerr << "FAILURE: At least one test failed." << std::endl;
	}

	return result;
}
