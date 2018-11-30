/*
 * HALSpace.cpp
 *
 * Implementation of a HAL space.
 *
 * Created: 4/12-2005
 * Last update: 7/14-2017
 *
 *******************************************************************/

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



enum {WORDS = 0, CELLS=1 };

#include <cmath>

#include "HALSpace.h"
#include <emdf_value.h>
#include <emdf_exception.h>
#include <string_func.h>
#include <algorithm>
#include <iostream>

///////////////////////////////////////////////////////////
//
// MyDoubleMatrix
//
///////////////////////////////////////////////////////////
MyDoubleMatrix::MyDoubleMatrix(int rows, int columns)
	: m_rows(rows),
	  m_columns(columns)
{
	m_matrix.reserve(rows+1);
	int row;
	for (row = 0;
	     row <= rows;
	     ++row) {
		double *pRow = new double[columns+1];
		int col;
		for (col = 0;
		     col <= columns;
		     ++col) {
			(pRow)[col] = 0.0;
		}
		m_matrix[row] = pRow;
	}
}

MyDoubleMatrix::~MyDoubleMatrix()
{
	for (int row = 0;
	     row < m_rows;
	     ++row) {
		delete[] m_matrix[row];
	}
	// delete[] m_matrix;
}

double MyDoubleMatrix::get(int row, int column)
{
	return m_matrix[row][column];
}

void MyDoubleMatrix::set(int row, int column, double value)
{
	m_matrix[row][column] = value;
}




///////////////////////////////////////////////////////////
//
// DoubleMatrix
//
///////////////////////////////////////////////////////////
DoubleMatrix::DoubleMatrix(int rows, int columns)
	: m_cur_row(-1),
	  m_pCurRow(0)
{
	UNUSED(rows);
	UNUSED(columns);
}

DoubleMatrix::~DoubleMatrix()
{
	Int2PInt2DoubleMapMap::iterator it = m_matrix.begin();
	while (it != m_matrix.end()) {
		delete it->second;
		++it;
	}
	m_matrix.clear();
}

double DoubleMatrix::get(int row, int column)
{
	const Int2DoubleMap *pColumnMap;
	if (row == m_cur_row) {
		if (m_pCurRow == 0) {
			return 0.0;
		} else {
			pColumnMap = m_pCurRow;
		}
	} else {
		m_cur_row = row;
		Int2PInt2DoubleMapMap::const_iterator ci = m_matrix.find(row);
		if (ci == m_matrix.end()) {
			m_pCurRow = 0;
			return 0.0;
		} else {
			m_pCurRow = ci->second;
			pColumnMap = m_pCurRow;
		}
	}
	Int2DoubleMap::const_iterator ci2 = pColumnMap->find(column);
	if (ci2 == pColumnMap->end()) {
		return 0.0; 
	} else {
		return ci2->second;
	}
	
}

void DoubleMatrix::set(int row, int column, double value)
{
	if (value == 0.0) {
		return;
	} else {
		Int2DoubleMap *pColumnMap = 0;
		Int2PInt2DoubleMapMap::iterator it = m_matrix.find(row);
		if (it == m_matrix.end()) {
			pColumnMap = new Int2DoubleMap();
			m_matrix.insert(std::make_pair(row, pColumnMap));
		} else {
			pColumnMap = it->second;
		}
		(*pColumnMap)[column] = value;
	}
}



///////////////////////////////////////////////////////////
//
// MyMatrix
//
///////////////////////////////////////////////////////////

MyMatrix::MyMatrix(int rows, int columns)
	: m_cur_row(-1),
	  m_pCurRow(0)
{
	UNUSED(rows);
	UNUSED(columns);
}

MyMatrix::~MyMatrix()
{
	Int2PInt2IntMapMap::iterator it = m_matrix.begin();
	while (it != m_matrix.end()) {
		delete it->second;
		++it;
	}
	m_matrix.clear();
}


int MyMatrix::get(int row, int column)
{ 
	const Int2IntMap *pColumnMap;
	if (row == m_cur_row) {
		if (m_pCurRow == 0) {
			return 0;
		} else {
			pColumnMap = m_pCurRow;
		}
	} else {
		m_cur_row = row;
		Int2PInt2IntMapMap::const_iterator ci = m_matrix.find(row);
		if (ci == m_matrix.end()) {
			m_pCurRow = 0;
			return 0;
		} else {
			m_pCurRow = ci->second;
			pColumnMap = m_pCurRow;
		}
	}
	Int2IntMap::const_iterator ci2 = pColumnMap->find(column);
	if (ci2 == pColumnMap->end()) {
		return 0; 
	} else {
		return ci2->second;
	}
}

void MyMatrix::set(int row, int column, int value)
{
	if (value == 0) {
		return;
	} else {
		Int2IntMap *pColumnMap = 0;
		Int2PInt2IntMapMap::iterator it = m_matrix.find(row);
		if (it == m_matrix.end()) {
			pColumnMap = new Int2IntMap();
			m_matrix.insert(std::make_pair(row, pColumnMap));
		} else {
			pColumnMap = it->second;
		}
		(*pColumnMap)[column] = value;
	}
}


void MyMatrix::add(int row, int column, int value_to_add)
{
	Int2IntMap *pColumnMap = 0;
	Int2PInt2IntMapMap::iterator it = m_matrix.find(row);
	if (it == m_matrix.end()) {
		pColumnMap = new Int2IntMap();
		m_matrix.insert(std::make_pair(row, pColumnMap));
	} else {
		pColumnMap = it->second;
	}
	Int2IntMap::iterator it2 = pColumnMap->find(column);
	if (it2 == pColumnMap->end()) {
		pColumnMap->insert(std::make_pair(column, value_to_add));
	} else {
		(*pColumnMap)[column] += value_to_add;
	}
}


///////////////////////////////////////////////////////////
//
// HALSpace
//
///////////////////////////////////////////////////////////


HALSpace::HALSpace(int n, monad_m first_HAL_monad, EmdrosEnv *pEE) 
{
	m_n = n;
	m_pMatrix = 0;
	m_max_hal_word = 0;
	m_first_HAL_monad = first_HAL_monad;
	m_bAddNumbers = false;
	m_pEE = pEE;
}

HALSpace::~HALSpace() 
{
	delete m_pMatrix;
}

void HALSpace::EmptyMatrix() 
{
	// Create an empty matrix
	m_pMatrix = new MyMatrix(m_max_hal_word+1, m_max_hal_word+1);
}

void HALSpace::load() 
{
	/* Load a HAL space.  Generate it if not there.*/
	std::cout << "Attempting to load HAL space..." << std::endl;
	if (!db_has_space()) {
		std::cout << "HAL space of window size " + long2string(m_n) + " was not found in database." << std::endl;
		generate_space();
		save_space();
	} else {
		std::cout << "HAL space of window size " + long2string(m_n) + " was found." << std::endl;
		load_space();
	}
}

void HALSpace::testSheafNotEmpty(const Sheaf *pSheaf) 
{
	// Test that sheaf is not empty, and that it exists
	if (pSheaf == 0) {
		std::cout << "Sheaf was nil" << std::endl;
		throw EmdrosException("Sheaf was nil.");
	}
	if (pSheaf->isFail()) {
		std::cout << "Sheaf failed." << std::endl;
		throw EmdrosException("Sheaf failed.");
	}
	if (!pSheaf->const_iterator().hasNext()) {
		std::cout << "Sheaf was empty." << std::endl;
		throw EmdrosException("Sheaf was empty.");
	}
}
          
void HALSpace::load_space() {
	// Load a HAL space.
	std::cout << "Loading HAL space" << std::endl;

	//
	// Get words
	//

	// Query backend
	std::string query = "SELECT ALL OBJECTS\n"
		"WHERE\n"
		"[hal_space n =" + monad_m2string(m_n) + "\n"
		"   [hal_word get stripped_surface]\n"
		"]GO\n";
	std::cout << "... now querying backend to get all relevant HAL word data..." << std::endl;
	Sheaf *pSheaf = executeQueryGetSheaf(query);
	testSheafNotEmpty(pSheaf);
	std::cout << "... done querying backend!" << std::endl;

	//
	// Set self.max_hal_word and Empty matrix
	//
  
	// Get straw
	const Straw *pStraw = pSheaf->const_iterator().current();
  
	// Get hal_space mo
	const MatchedObject *hal_space_mo = pStraw->const_iterator().current();
  
	// Get monads
	SetOfMonads hal_space_monads = hal_space_mo->getMonads();

	// Set max_hal_word
	m_max_hal_word = hal_space_monads.last() - hal_space_monads.first();

	// Initialize in-memory word lists and make an emptry matrix
	std::cout << "Now initializing the HAL matrix..." << std::endl;
	//self.make_word_lists()
	EmptyMatrix();
	std::cout << "... done!" << std::endl;

	// Process hal_space mo
	process_hal_space_mo(hal_space_mo, WORDS);

	//
	// Get cells
	//

	// Query backend
	query = "SELECT ALL OBJECTS\n"
		"WHERE\n"
		"[hal_space n = " + long2string(m_n) + "\n"
		"   [hal_cell n = " + long2string(m_n) + " get column, value]\n"
		"]GO";
	std::cout << "... now querying backend to get all relevant HAL cell data..." << std::endl;
	pSheaf = executeQueryGetSheaf(query);
	testSheafNotEmpty(pSheaf);
	std::cout << "... done querying backend!" << std::endl;

	// Get straw
	pStraw= pSheaf->const_iterator().current();

	// Get hal_space mo
	hal_space_mo = pStraw->const_iterator().current();

	// Process hal_space mo
	process_hal_space_mo(hal_space_mo, CELLS);

	// Generate self.hal_word_indexes_by_word
	generate_reverse_lookup_dictionary();

	// Done!
	std::cout << "... done loading HAL space!" << std::endl;
}

void HALSpace::process_hal_space_mo(const MatchedObject *hal_space_mo, int which) 
{
	// Process a hal_space matched_object.
	if (which == WORDS) {
		std::cout << "... now harvesting HAL words..." << std::endl;
	} else {
		std::cout << "... now harvesting cells..." << std::endl;
	}

	// Get sheaf
	const Sheaf *pSheaf = hal_space_mo->getSheaf();
    
	// Process sheaf
	process_hal_space_sheaf(pSheaf, which);
}
          
void HALSpace::process_hal_space_sheaf(const Sheaf *pSheaf, int which) 
{
	// Process a sheaf which holds hal_word mos in each straw
	if (which == WORDS) {
		harvest_words(pSheaf);
	} else {
		harvest_cells(pSheaf);
	}
}
      
void HALSpace::harvest_words(const Sheaf *pSheaf) {
	// Get sheaf const iterator
	SheafConstIterator sheaf_ci = pSheaf->const_iterator();

	// Set counter to 0
	int counter = 0;
  
	// iterate over sheaf
	while (sheaf_ci.hasNext()) {
		// Get current straw
		const Straw *pStraw= sheaf_ci.current();

		// Get hal_word mo
		const MatchedObject *hal_word_mo = pStraw->const_iterator().current();

		// Process hal_word mo
		process_hal_word_mo(hal_word_mo);

		// out message if
		if ((counter % 250) == 0) {
			std::cout << "... came to HAL word no. " << counter+1 << " out of " << m_max_hal_word + 1 << ", harvesting HAL words." << std::endl;
		}

		// Advance iterators
		sheaf_ci.next();
		++counter;
	}
	std::cout << "... done harvesting HAL words!" << std::endl;
}

/*
  def get_feature_dict(mo, l, dict_feature_name_to_index):
  """Get features in the list l from the matched_object mo.
  The feature names must all be in dict_feature_name_to_index."""
  for feature_name in l:
  get_feature_dict_feature(mo, feature_name, dict_feature_name_to_index)

  def get_feature_dict_feature(mo, feature_name, dict_feature_name_to_index):
  """Get a single feature from a matched_object mo.  The feature name
  must be in dict_feature_name_to_index."""
  index = mo.getEMdFValueIndex(feature_name)
  dict_feature_name_to_index[feature_name] = index

*/
std::string HALSpace::get_mo_feature(const MatchedObject *pMo, unsigned int feature_index) 
{
	// Get feature from matched_object.  The feature_name must be in
	// dict_feature_name_to_index.
	std::string feature_value = "";
	const EMdFValue *pEmdf_value = pMo->getEMdFValue(feature_index);
	feature_value = pEmdf_value->toString();
	return feature_value;
}

void HALSpace::process_hal_word_mo(const MatchedObject *hal_word_mo) {
	// Process a hal_word matched_object.  This is in the context of a 
	// hal_space mo.

	// Get monad
	monad_m monad = hal_word_mo->getMonads().first();

	// Get index from monad
	int index = monad - m_first_HAL_monad;

	// Get features
	std::string stripped_surface = get_mo_feature(hal_word_mo, 0);

	// Set features
	m_hal_words[index] = stripped_surface;

	// If index is greater than current max, set
	// current max to index.
	if (index > m_max_hal_word) {
		m_max_hal_word = index;
	}
}
	      
          
void HALSpace::harvest_cells(const Sheaf *pSheaf) 
{
	// Process a hal_space inner sheaf with cells.

	// Get sheaf const iterator
	SheafConstIterator sheaf_ci = pSheaf->const_iterator();
      
	// Initialize last_index_seen
	int last_index_seen = -1;

	// Iterate over sheaf
	while (sheaf_ci.hasNext()) { 
		// Get straw
		const Straw *pStraw= sheaf_ci.current();
          
		// Get hal_cell mo
		const MatchedObject *pHal_cell_mo = pStraw->const_iterator().current();

		// Get monad
		monad_m monad = pHal_cell_mo->getMonads().first();
          
		// Get index from monad
		int index = monad - m_first_HAL_monad;
          
		// Get features
		int column = string2long(get_mo_feature(pHal_cell_mo, 0));
		int value = string2long(get_mo_feature(pHal_cell_mo, 1));

		// out message if
		if (index != last_index_seen && (index % 100) == 0) {
			std::cout << "... came to HAL word no. " << index+1 << " out of " << m_max_hal_word+1 << ", harvesting HAL cells." << std::endl;
			last_index_seen = index;
		}

		// Set matrix-entry
		m_pMatrix->set(index, column, value);

		// Advance iterator
		sheaf_ci.next();
	}

	std::cout << "Done harvesting cells!" << std::endl;
}
          
bool HALSpace::db_has_space() 
{
	//Check whether DB has hal_space of window width n.
	std::string query = "SELECT ALL OBJECTS\n"
		"WHERE [hal_space n = " + long2string(m_n) + "]GO";
	Sheaf *pSheaf = executeQueryGetSheaf(query);
	if (pSheaf == 0)
		return false;
	if (pSheaf->isFail())
		return false;
	if (!pSheaf->const_iterator().hasNext()) {
		return false;
	} else {
		return true;
	}
}

void HALSpace::save_space() 
{
	// Save a HAL space
	std::cout << "Saving HAL space" << std::endl;
	save_cells();
	save_hal_space();
	std::cout << "All done." << std::endl;
}

bool HALSpace::maybeExecQuery(Bigstring **ppQuery, 
			      const std::string& object_type_name, 
			      int& count,
			      bool bDoItRegardless) 
{
	if ((bDoItRegardless && ! (*ppQuery)->empty())
	    || count >= 10000) {
		std::cout << "Committing to DB..." << std::endl;
		std::string query;
		(*ppQuery)->toString(query);
		if (!executeString("CREATE OBJECTS WITH OBJECT TYPE [" + object_type_name + "]\n" + query + "GO")) {
			std::cout << "Could not execute query for object type:" <<  object_type_name << std::endl;
			throw EmdrosException();
		}
		std::cout << "...Done!" << std::endl;
		count = 0;
		delete (*ppQuery);
		(*ppQuery) = new Bigstring();
		return true;
	} else {
		return false;
	}
}
      

void HALSpace::save_cells() 
{
	// Save the cells of a HAL space
	std::cout << "Saving cells..." << std::endl;
	int row_index = 0;
	Bigstring * pQuery = new Bigstring;
	int count = 0;
	for (row_index = 0;
	     row_index <= m_max_hal_word;
	     ++row_index) {
		if ((row_index % 50 == 0)) {
			std::cout << "... came to HAL word no. " << row_index + 1 << " out of " << m_max_hal_word + 1 << ", saving HAL matrix cells."  << std::endl;
		}
		save_row(row_index, pQuery, count);
		maybeExecQuery(&pQuery, "hal_cell", count, false);
	}
	maybeExecQuery(&pQuery, "hal_cell", count, true);
	std::cout << "... came to HAL word no. " << row_index + 1 << " out of " << m_max_hal_word + 1 << ", saving HAL matrix cells."  << std::endl;
	delete pQuery;
}

void HALSpace::save_row(int row_index, Bigstring *pQuery, int &count) 
{
	// Save a whole row
	int column_index;
	for (column_index = 0;
	     column_index <= m_max_hal_word;
	     ++column_index) {
		int value = m_pMatrix->get(row_index, column_index);
		if (value > 0) {
			save_cell(row_index, column_index, value, pQuery);
			++count;
		}
	}
}

void HALSpace::save_cell(int row_index, int column_index, int value, Bigstring *pQuery) 
{

	// Save a single cell
	SetOfMonads som = SetOfMonads(m_first_HAL_monad + row_index);
	std::string mql = "CREATE OBJECT\n"
		"FROM MONADS = " + som.toString() + "\n"
		"[\n"
		"   n := " + long2string(m_n) + ";\n"
		"   column := " + long2string(column_index) + ";\n"
		"   value := " + long2string(value) + ";\n"
		"]\n";
	pQuery->addsz(mql.c_str());
}

void HALSpace::save_hal_space() 
{
	// Save the HAL_space object of a HAL space.
	std::cout << "Saving HAL space object..." << std::endl;
	SetOfMonads som = SetOfMonads(m_first_HAL_monad, m_first_HAL_monad + m_max_hal_word);
	std::string query = "CREATE OBJECT\n"
		"FROM MONADS = " + som.toString() + "\n"
		"[hal_space\n"
		"  n :=" + long2string(m_n) + ";\n"
		"]GO";
	if (!executeString(query)) {
		std::cout << "Could not execute query " + query << std::endl;
		throw EmdrosException();
	}
}

bool HALSpace::executeString(const std::string& query) 
{
	bool bNoCompilerError = true;
	bool bNoDBError = m_pEE->executeString(query, bNoCompilerError, false, true);
	return bNoCompilerError && bNoDBError;
}



Sheaf *HALSpace::executeQueryGetSheaf(const std::string& query) 

{
	// std::cout << "QUERY = '" + query + "'" << std::endl;
	if (!executeString(query)) {
		std::cout << "Could not execute query " + query << std::endl;
		throw EmdrosException();
	} else {
		if (!m_pEE->isSheaf()) {
			std::cout << "Result was not a pSheaf->" << std::endl;
			throw EmdrosException();
		} else {
			Sheaf *pSheaf = m_pEE->takeOverSheaf();
			return pSheaf;
		}
	}
}

void HALSpace::generate_space() 
{
	/// Generate a HAL space of window-width n
	std::cout << "Now generating HAL space..." << std::endl;
	load_hal_words();
	std::cout << "...Now initializing the HAL matrix..." << std::endl;
	EmptyMatrix();
	std::cout << "...Done!" << std::endl;
	generate_reverse_lookup_dictionary();
	traverse_words();
	std::cout << "Done generating HAL space" << std::endl;
}

void HALSpace::generate_reverse_lookup_dictionary() 
{
	std::cout << "Now making reverse lookup dictionary..:" << std::endl;
	for (IntStringMap::const_iterator ci = m_hal_words.begin();
	     ci != m_hal_words.end();
	     ++ci) {
		std::string word = ci->second;
		m_hal_word_indexes_by_word[word] = ci->first;
	}
	std::cout << "...Done!" << std::endl;
}

void HALSpace::load_hal_words() 
{
	std::cout << "Now loading HAL words..." << std::endl;
      
	// Query backend
	std::string query = "SELECT ALL OBJECTS\n"
		"WHERE [hal_word get stripped_surface]GO";
	std::cout << "... now querying backend to get all relevant data..." << std::endl;
	Sheaf *pSheaf = executeQueryGetSheaf(query);
	testSheafNotEmpty(pSheaf);
	std::cout << "... done querying backend!" << std::endl;

	// Process sheaf
	process_hal_space_sheaf(pSheaf, false);
      
	std::cout << "... done loading HAL words!" << std::endl;
}

SetOfMonads HALSpace::get_text_object() 
{
	std::string query = "SELECT ALL OBJECTS\n"
		"WHERE [text GET add_numbers]GO";
	Sheaf *pSheaf = executeQueryGetSheaf(query);
	testSheafNotEmpty(pSheaf);
	const Straw *pStraw= pSheaf->const_iterator().current();
	const MatchedObject *pText_mo = pStraw->const_iterator().current();
	SetOfMonads result_SOM = pText_mo->getMonads();
	if (pText_mo->getEMdFValue(0)->toString() == "true") {
		m_bAddNumbers = true;
	} else {
		m_bAddNumbers = false;
	}
	return result_SOM;
}

int HALSpace::get_text_length() 
{
	SetOfMonads som = get_text_object();
	return som.last() - som.first() + 1;
}

void HALSpace::load_text_words(const SetOfMonads& som, StringBoolVec& l) 
{
	std::cout << "Loading text words..." << std::endl;
	std::cout << "Now querying backend..." << std::endl;
	std::string query = "SELECT ALL OBJECTS\n"
		"IN " + som.toString() + "\n"
		"WHERE\n"
		"[text\n"
		"    [word get stripped_surface,is_number]\n"
		"]GO";
	Sheaf *pSheaf = executeQueryGetSheaf(query);
	std::cout << "...Querying done." << std::endl;
	testSheafNotEmpty(pSheaf);
	const Straw *pStraw= pSheaf->const_iterator().current();
	const MatchedObject *pText_mo = pStraw->const_iterator().current();
	process_text_mo(pText_mo, l);
	std::cout << "Loaded all text words!" << std::endl;
}

void HALSpace::process_text_mo(const MatchedObject *pText_mo, StringBoolVec& l) 
{
	std::cout << "Getting text words..." << std::endl;

	// Get sheaf
	const Sheaf *pSheaf = pText_mo->getSheaf();

	// Get sheaf const iterator
	SheafConstIterator sheaf_ci = pSheaf->const_iterator();

	// Set counter
	int counter = 0;

	// Iterate over sheaf
	while (sheaf_ci.hasNext()) {
		const Straw *pStraw= sheaf_ci.current();
		const MatchedObject *pText_word_mo = pStraw->const_iterator().current();
		process_text_word_mo(pText_word_mo, l);
		// out message if
		if ((counter % 250) == 1) {
			std::cout << "... came to word no. " + long2string(counter) + ", harvesting text words." << std::endl;
		}

		// Advance iterators
		sheaf_ci.next();
		++counter;
	}
	std::cout << "Done getting text words..." << std::endl;
}

void HALSpace::process_text_word_mo(const MatchedObject *text_word_mo, 
				    StringBoolVec& l) 
{
	// Get feature
	std::string stripped_surface = get_mo_feature(text_word_mo, 0);
	std::string is_number = get_mo_feature(text_word_mo, 1);

	// Get bIsNumber
	bool bIsNumber = is_number == "true";

	// Append
	l.push_back(std::pair<std::string, bool>(stripped_surface, bIsNumber));
}


void HALSpace::traverse_words() 
{
	SetOfMonads som = get_text_object();
	StringBoolVec text_word_list;
	load_text_words(som, text_word_list);
	monad_m last_monad = som.last();
	int index_head = 2;
	int index_tail = 1;
	// iterator_list = [index_tail, index_head, last_monad];
	std::cout << "Now traversing words, building HAL space..." << std::endl;
	while (index_head < last_monad) {
		traverse_word_range(text_word_list, index_tail, index_head);
		advance_indices(index_tail, index_head, last_monad);
		//std::cout << "iterator_list = " + str(iterator_list) << std::endl;
		if ((index_head % 250) == 1) {
			std::cout << "... came to text word number " + long2string(index_head) + ", building HAL space..." << std::endl;
		}
	}
	std::cout << "...all done!" << std::endl;
}

void HALSpace::advance_indices(int &index_tail, int &index_head, monad_m last_monad) 
{
	if (index_head != last_monad) {
		int diff = index_head - index_tail;
		if (diff == m_n) {
			++index_tail;
		} 
		++index_head;
	}
}

void HALSpace::traverse_word_range(StringBoolVec& text_word_list, 
				   int index_tail, int index_head) 
{
	int index = index_tail;
	while (index != index_head) {
		int addition = m_n - (index_head - index) + 1;
		add_word_pair(text_word_list[index], text_word_list[index_head], addition);
		++index;
	}
}

void HALSpace::add_word_pair(const std::pair<std::string, bool>& first_word, 
			     const std::pair<std::string, bool>& second_word, 
			     int addition) 
{
	std::string w1surf = first_word.first;
	bool w1isnum = first_word.second;
	std::string w2surf = second_word.first;
	bool w2isnum = second_word.second;
	if (!m_bAddNumbers
	    && (w1isnum || w2isnum)) {
		return;
	} else {
		int w1index = m_hal_word_indexes_by_word[w1surf];
		int w2index = m_hal_word_indexes_by_word[w2surf];
		m_pMatrix->add(w2index,w1index,addition);
	}
}

double HALSpace::compute_similarity_measure(int i, int j, MyDoubleMatrix& nm_squared, const DoubleVec& SQRT_Ai2)
{
	double dResult;
	
	if (i == j) {
		dResult = 1.0;
	} else {
		double dSUM_Ai_Bi = nm_squared.get(i, j);

		double dSQR_SUM_Ai2 = SQRT_Ai2[i];
		double dSQR_SUM_Bi2 = SQRT_Ai2[j];

		double dDenominator = dSQR_SUM_Ai2 * dSQR_SUM_Bi2;
		if (dDenominator == 0.0) {
			dResult = 0.0;
		} else {
			dResult = dSUM_Ai_Bi / dDenominator;
		}
	}

	return dResult;
}

void HALSpace::compute_normalized_matrix_etc(MyDoubleMatrix& normalized_matrix, MyDoubleMatrix& nm_squared, DoubleVec& Ai2)
{
	double dHALWordCount = (double) m_max_hal_word;

	std::cerr << "UP200: Now calculating normalized matrix." << std::endl;

	Ai2.reserve(m_max_hal_word+1);
	int row;
	for (row = 0;
	     row <= m_max_hal_word;
	     ++row) {
		int col;
		double dSUM_Ai2 = 0.0;
		for (col = 0;
		     col <= m_max_hal_word;
		     ++col) {
			int A_value = m_pMatrix->get(row, col);
			double A_dValue = A_value / dHALWordCount;
			normalized_matrix.set(row, col, A_dValue);

			dSUM_Ai2 += A_dValue * A_dValue;
		}

		Ai2[row] = sqrt(dSUM_Ai2);
	}

	std::cerr << "UP200: Now calculating matrix product..." << std::endl;

	
	// Now calculate the matrix product of normalized_matrix
	// and its transpose
	int i = 0;
	for (i = 0;
	     i <= m_max_hal_word;
	     ++i) {
		int j;
		for (j = 0;
		     j <= i;
		     ++j) {
			double dSUM = 0.0;
			int col;
			for (col = 0;
			     col <= m_max_hal_word;
			     ++col) {
				double dA = normalized_matrix.get(i, col);
				double dB = normalized_matrix.get(j, col);
				dSUM += dA * dB;
			}

			nm_squared.set(i, j, dSUM);
		}
	}

	std::cerr << "UP200: ...Done!" << std::endl;
}


void HALSpace::write_similarity_measures(std::ostream *pOut)
{
	MyDoubleMatrix normalized_matrix(m_max_hal_word, m_max_hal_word);
	MyDoubleMatrix nm_squared(m_max_hal_word, m_max_hal_word);
	MyDoubleMatrix result(m_max_hal_word, m_max_hal_word);
	IIDTripleVec result_vec;
	
	DoubleVec SQRT_Ai2;
	
	compute_normalized_matrix_etc(normalized_matrix, nm_squared, SQRT_Ai2);
	
	// Computing similarity measures
	
	std::cout << "Computing similarity measures..." << std::endl;
	
	int i;
	for (i = 0;
	     i <= m_max_hal_word;
	     ++i) {
		if ((i % 100) == 0) {
			std::cout << "\nUP200: Came to row = " << i << " out of " << m_max_hal_word;
		}
		int j;
		for (j = 0;
		     j <= i;
		     ++j) {
			if ((j % 1000) == 0) {
				std::cout << "." << std::flush;
			}
			
			double dResult = compute_similarity_measure(i, j, nm_squared, SQRT_Ai2);
			result.set(i, j, dResult);

			result_vec.push_back(IntIntDoubleTriple(i, j, dResult));
		}
	}
	
	std::cout << "\n...Done!" << std::endl;

	std::cout << "Computing result vector..." << std::endl;

	std::sort(result_vec.begin(), result_vec.end());

	std::cout << "\n...Done!" << std::endl;
	
	std::cout << "Writing similarity measures to CSV file..." << std::endl;

	*pOut << "Rank\tWord1\tWord2\tCosineSimilarity\n";

	IIDTripleVec::size_type index = result_vec.size()-1;	
	while (true) {
		const IntIntDoubleTriple& iid = result_vec[index];
		double dResult = iid.m_real_value;
		std::string word1 = m_hal_words[iid.m_row];
		std::string word2 = m_hal_words[iid.m_column];

		*pOut << (result_vec.size() - index + 1) << '\t'
		      << word1 << '\t'
		      << word2 << '\t'
		      << dResult << '\n';

		--index;
		if (index == 0) {
			break;
		}
	}

	/*
	for (i = 0;
	     i <= m_max_hal_word;
	     ++i) {
		write_similarity_row_to_csv(pOut, i, &result);
	}
	*/
	
	std::cout << "...Done!" << std::endl;
}

/*

void HALSpace::write_similarity_measures(std::ostream *pOut)
{
	Int2IntMap Ai2;
	MyMatrix nm_squared;
	MyMatrix result;

	

}
*/

void HALSpace::write_to_csv_file(std::ostream *pOut, const StringVec& interesting_words) 
{
	std::set<std::string> interesting_set;
	StringVec::const_iterator ci = interesting_words.begin();
	while (ci != interesting_words.end()) {
		interesting_set.insert(*ci);
		++ci;
	}
	bool bDontUseInterestingSet = interesting_set.empty();
	std::cout << "Writing to CSV file..." << std::endl;
	int row_index = 0;
	int last_index_seen = -1;
	for (row_index = 0;
	     row_index <= m_max_hal_word;
	     ++row_index) {
		if (row_index != last_index_seen
		    && (row_index % 100) == 0) {
			std::cout << "... came to HAL word no. " << row_index + 1 << " out of " << m_max_hal_word + 1 << ", writing to CSV file." << std::endl;
			last_index_seen = row_index;
		}

		if (bDontUseInterestingSet) {
			write_row_to_csv(pOut, row_index);
		} else {
			std::string hal_word = m_hal_words[row_index];
			if (interesting_set.find(hal_word) != interesting_set.end()) {
				write_row_to_csv(pOut, row_index);
			}
		}
	}
	std::cout << "... done writing to CSV file." << std::endl;
}

void HALSpace::write_row_to_csv(std::ostream *pOut, int row_index) 
{

	char szNumber[20];
	sprintf(szNumber, "%05d", row_index);

	std::string S = std::string(szNumber) + "," + m_hal_words[row_index];

	for (int column = 0;
	     column <= m_max_hal_word;
	     ++column) {
		S += "," + long2string(m_pMatrix->get(row_index, column));
	}
	S += "\n";
	(*pOut) << S;
	/*
	(*pOut) << std::string(szNumber) << ',' << m_hal_words[row_index];

	for (int column = 0;
	     column <= m_max_hal_word;
	     ++column) {
		(*pOut) << ',' << m_pMatrix->get(row_index, column);
	}
	(*pOut) << '\n';

	*/
}

void HALSpace::write_similarity_row_to_csv(std::ostream *pOut, int row_index, MyDoubleMatrix *pMatrix) 
{

	char szNumber[20];
	sprintf(szNumber, "%05d", row_index);

	std::string S = std::string(szNumber) + "," + m_hal_words[row_index];

	*pOut << S;

	for (int column = 0;
	     column <= row_index;
	     ++column) {
		double dResult = pMatrix->get(row_index, column);
		if (dResult == 1.0) {
			*pOut << ",1.0";
		} else {
			*pOut << ',' << dResult;
		}
	}
	*pOut << '\n';
}

void HALSpace::write_word_vectors(const StringVec& words, 
				  int factor, 
				  int max_values, 
				  std::ostream *pOut) 
{
	std::cout << "Now printing word vectors..." << std::endl;
	int text_length = get_text_length();
	(*pOut) << "n = " << long2string(m_n) << std::endl;
	(*pOut) << "text-length = " << long2string(text_length) << std::endl;
	(*pOut) << "factor = " << long2string(factor) << std::endl;
	(*pOut) << "max_values= " << long2string(max_values) << std::endl;
	(*pOut) << "words = " << std::endl;
	StringVec::size_type i;
	for (i = 0;
	     i < words.size();
	     ++i) {
		(*pOut) << "  " << words[i] << std::endl;
	}
	for (i = 0;
	     i < words.size();
	     ++i) {
		write_word_vector(words[i], factor, text_length, max_values, pOut);
	}
	std::cout << "... done printing word vectors." << std::endl;
}

void HALSpace::write_word_vector(const std::string& word, 
				 int factor, 
				 int text_length, 
				 unsigned int max_values, 
				 std::ostream *pOut) 
{
	if (m_hal_word_indexes_by_word.find(word) == m_hal_word_indexes_by_word.end()) {
		(*pOut) << "The word '" << word << "' was not found in the HAL space." << std::endl;
	} else {
		int row_index = m_hal_word_indexes_by_word.find(word)->second;
		(*pOut) << word << " = [\n";
		MyTripleVec mytriplevec;
		get_word_vector(word, row_index, factor, text_length, max_values, mytriplevec);
		MyTripleVec::size_type vecsize = mytriplevec.size();
		MyTripleVec::size_type mymin = (vecsize < max_values) ? vecsize : max_values;
		for (MyTripleVec::size_type word_index = 0;
		     word_index < mymin;
		     ++word_index) {
			char szBuf[1024];
			sprintf(szBuf, "%-20s", mytriplevec[word_index].other_word.c_str());
			(*pOut) << "    " << szBuf << " : ";
			sprintf(szBuf, "%10f", mytriplevec[word_index].real_value);
			(*pOut) << szBuf << "    " << mytriplevec[word_index].value << std::endl;
		}
		(*pOut) << "] " << word << std::endl;
	}
}

void HALSpace::get_word_vector(const std::string& word, 
			       int row_index, 
			       int factor, 
			       int text_length, 
			       int max_values,
			       MyTripleVec& mytriplevec) 

{
	UNUSED(word);
	UNUSED(max_values);
	
	
	for (int col_index = 0;
	     col_index <= m_max_hal_word;
	     ++col_index) {
		int value = m_pMatrix->get(row_index, col_index) + m_pMatrix->get(col_index, row_index);
		double real_value = (value * 1.0) * factor / text_length;
		if (value > 0) {
			std::string other_word = m_hal_words[col_index];
			mytriplevec.push_back(MyTriple(other_word, real_value, value));
		}
	}
	std::sort(mytriplevec.begin(), mytriplevec.end());
}

void HALSpace::writeSQL(std::ostream *pOut)
{
	int max_row = 100;
	//int max_row = m_max_hal_word;
	//(*pOut) << "BEGIN;\n";
	(*pOut) << "CREATE TABLE hal (source INT NOT NULL";
	for (int i = 0; i <= max_row; ++i) {
		(*pOut) << ", dest" << i << " INT NOT NULL\n";
	}
	(*pOut) << ");\n";
	for (int row_index = 0; row_index <= max_row; ++row_index) {
		(*pOut) << "INSERT INTO hal VALUES (" << row_index;
		for (int col_index = 0; col_index <= max_row; ++col_index) {
			(*pOut) << ',' << m_pMatrix->get(row_index,col_index);
		}
		(*pOut) << ");\n";
	}
	//(*pOut) << "COMMIT;\n";
}


void HALSpace::writeCXT(std::ostream *pOut, const StringVec& words, int threshold)
{
	std::list<int> rows_to_print;
	std::set<std::string> word_set;

	for (StringVec::size_type index = 0; index < words.size(); index++) {
		word_set.insert(words[index]);
	}
	
	for (int row_index = 0; row_index <= m_max_hal_word; ++row_index) {
		if (word_set.find(m_hal_words[row_index]) != word_set.end()) {
			for (int col_index = 0; col_index <= m_max_hal_word; ++col_index) {
				if (word_set.find(m_hal_words[col_index]) != word_set.end()) {
					if (m_pMatrix->get(row_index, col_index) > threshold) {
						rows_to_print.push_back(row_index);
						
						// We don't need to search further 
						// in this row, so we break out 
						// of the col_index loop.
						break;
					}
				}
			}
		}
	}
	
	
	(*pOut) << "B\n"; // file magic
	(*pOut) << "HALSPACE\n"; // Name for the context
	(*pOut) << rows_to_print.size() << '\n'; // Number of objects
	(*pOut) << rows_to_print.size() << "\n\n"; // Number of attributes

	// Objects names
	std::list<int>::const_iterator ci;
	ci = rows_to_print.begin();
	while (ci != rows_to_print.end()) {
		(*pOut) << m_hal_words[*ci] << '\n';
		//(*pOut) << "OBJ_" << *ci << '\n';

		++ci;
	}

	// Attribute ames
	ci = rows_to_print.begin();
	while (ci != rows_to_print.end()) {
		(*pOut) << m_hal_words[*ci] << '\n';
		//(*pOut) << "OBJ_" << *ci << '\n';

		++ci;
	}

	// incidence relation matrix
	ci = rows_to_print.begin();
	while (ci != rows_to_print.end()) {
		int row_index = *ci;
		std::list<int>::const_iterator ci2 = rows_to_print.begin();
		while (ci2 != rows_to_print.end()) {
			int col_index = *ci2;
			(*pOut) << 
				((m_pMatrix->get(row_index,col_index) > threshold) 
				 ? 'x' : '.');
			
			++ci2;
		}

		(*pOut) << '\n';
		++ci;
	}
}
