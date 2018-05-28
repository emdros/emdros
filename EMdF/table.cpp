/*
 * table.cpp
 *
 * Tables
 *
 * Ulrik Petersen
 * Created: 5/5-2001
 * Last update: 5/28-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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

/**@file table.cpp
 *@brief Implementation of Table (EMdF layer)
 */


#include <string_func.h>
#include <debug.h>
#include <table.h>
#include <iostream>


/////////////////////////////////////////////////////////////////
//
// TableRowIterator
//
/////////////////////////////////////////////////////////////////

/** Constructor
 *
 * @param pMotherTableRow A pointer to the TableRow that this iterator
 * iterates over.
 */
TableRowIterator::TableRowIterator(TableRow *pMotherTableRow)
	: m_pMotherTableRow(pMotherTableRow),
	  m_iter(pMotherTableRow->begin())
{
}

/** Constructor
 *
 * Use this if you plan to use the assignment operator
 * (TableRowIterator::operator=) or TableRowIterator::assign() to
 * initialize it.
 */
TableRowIterator::TableRowIterator()
	:  m_pMotherTableRow(0),
	   m_iter()
{
}

/** Copy constructor
 *
 * @param other The other TableRowIterator from which to initialize.
 */
TableRowIterator::TableRowIterator(const TableRowIterator& other)
	:  m_pMotherTableRow(0),
	   m_iter()
{
	assign(other);
}


/**Destructor
 */
TableRowIterator::~TableRowIterator()
{
}

/** Assignment operator
 *
 * This method is not SWIG-wrapped, but may be used from C++.
 * Programs using the SWIG wrappers may use TableRowIterator::assign()
 * in lieu of this operator.
 *
 * @see TableRowIterator::assign().
 *
 * @param other The other TableRowIterator from which to get our
 * contents.
 */
TableRowIterator& TableRowIterator::operator=(const TableRowIterator& other)
{
	assign(other);
	return *this;
}


/** Assignment
 *
 * Mimics the assignment operator (TableRowIterator::operator=), but
 * does not return ourself.
 *
 * @see TableRowIterator::operator=().
 *
 * @param other The other TableRowIterator from which to get our contents.
 */
void TableRowIterator::assign(const TableRowIterator& other)
{
	m_pMotherTableRow = other.m_pMotherTableRow;
	m_iter = other.m_iter;
}


/** Does the TableRow have more columns?
 *
 * @return true if there are more elements, false if we have reached
 * the end.  <b>DO NOT</b> call next() if hasNext() returns false!
 */
bool TableRowIterator::hasNext() const
{
	ASSERT_THROW_X(m_pMotherTableRow != 0, TableRowException, "No mother table row.");
	return m_iter != m_pMotherTableRow->end();
}


/** Get the current column and advance iterator.
 *
 * NOTE: Only call if hasNext() returns true.
 *
 * @return The string contents of the current column.
 */
std::string TableRowIterator::next()
{
	ASSERT_THROW_X(m_pMotherTableRow != 0, TableRowException, "No mother table row.");
	ASSERT_THROW_X(hasNext(), TableRowException, "No more elements left.");
	std::string strResult = *m_iter;
	++m_iter;
	return strResult;
}

/** make iterator recede to the previous and get that column.
 *
 * @return The string contents of the previous column.
 */
std::string TableRowIterator::previous()
{
	ASSERT_THROW_X(m_pMotherTableRow != 0, TableRowException, "No mother table row.");
	if (m_iter == m_pMotherTableRow->begin()) {
		throw TableRowException();
	} else {
		--m_iter;
		return *m_iter;
	}
}

/** Get current column without touching iterator.
 *
 * @return The string contents of the current column.
 */
std::string TableRowIterator::current()
{
	ASSERT_THROW_X(m_pMotherTableRow != 0, TableRowException, "No mother table row.");
	return *m_iter;
}

/** Get column by index and set iterator to point to that column.
 *
 * @param column The index of the column. The first is number 1, not
 * 0.
 *
 * @return The string contents of the indexed column.
 */
std::string TableRowIterator::getColumn(int column)
{
	ASSERT_THROW_X(m_pMotherTableRow != 0, TableRowException, "No mother table row.");
	if (column < 1) {
		throw TableColumnException();
	}
	std::list<std::string>::iterator iter, iterend;
	iter = m_pMotherTableRow->begin();
	iterend = m_pMotherTableRow->end();
	int nCol = 1;
	while (nCol != column && iter != iterend) {
		++nCol;
		++iter;
	}
	if (iter == iterend) {
		throw TableColumnException();
	} else {
		return *iter;
	}
}






/////////////////////////////////////////////////////////////////
//
// TableRow
//
/////////////////////////////////////////////////////////////////


/** Constructor
 */
TableRow::TableRow()
{
}

/** Constructor which copies its contents from a std::list of
 *  std::string.
 */
TableRow::TableRow(std::list<std::string> other)
	: std::list<std::string>(other)
{
}


/** Destructor
 */
TableRow::~TableRow()
{
}


/** Get a TableRowIterator pointing to the first column.
 */
TableRowIterator TableRow::iterator()
{
	return TableRowIterator(this);
}


/** Get column by index.
 *
 * @param column The index of the column. The first is number 1, not
 * 0.
 *
 * @return The string contents of the indexed column.
 */
std::string TableRow::getColumn(int column)
{
	return TableRowIterator(this).getColumn(column);
}


/** Get the number of columns in the row.
 *
 * @return The number of columns in the row.
 */
unsigned int TableRow::size() const
{
	return ((std::list<std::string>*) this)->size();
}

/** Tests for equality with another TableRow.
 *
 * To be equal, the two TableRow objects must have the same number of
 * columns and the contents of each indexed column must be equal in
 * both.
 *
 * @param other The other TableRow to test against.
 *
 * @return true on identical, false otherwise.
 */
bool TableRow::equals(TableRow& other)
{
	TableRowIterator myiter = this->iterator();
	TableRowIterator youriter = other.iterator();

	// If one but not both iterators are done,
	// then the rows were not identical
	if ((myiter.hasNext() && !youriter.hasNext())
	    ||
	    (!myiter.hasNext() && youriter.hasNext())) {
		return false;
	}
  
      
	// Only enter loop if there is something in both iterators
	bool bGoOn = myiter.hasNext() && youriter.hasNext();

	while (bGoOn) {
		std::string mystring = myiter.next();
		std::string yourstring = youriter.next();

		// Check equality
		if (mystring != yourstring)
			return false;
    
		// If one but not both iterators are done,
		// then the tables were not identical
		if ((myiter.hasNext() && !youriter.hasNext())
		    ||
		    (!myiter.hasNext() && youriter.hasNext())) {
			return false;
		}
    
		bGoOn = bGoOn && myiter.hasNext() && youriter.hasNext();
	}

	// If we came this far, the rows were identical
	return true;
}



/////////////////////////////////////////////////////////////////
//
// TableIterator
//
/////////////////////////////////////////////////////////////////

/** Constructor
 *
 * Use this if you plan to use the assignment operator
 * (TableIterator::operator=) or TableIterator::assign() to
 * initialize it.
 */
TableIterator::TableIterator()
	: m_pMotherTable(0),
	  m_iter()
{
}

/** Constructor
 *
 * @param pMotherTable A pointer to the Table that this iterator
 * iterates over.
 */
TableIterator::TableIterator(Table *pMotherTable)
	: m_pMotherTable(pMotherTable),
	  m_iter(pMotherTable->m_list->begin())
{
}


/** Copy constructor
 *
 * @param other The other TableIterator from which to initialize.
 */
TableIterator::TableIterator(const TableIterator& other)
	: m_pMotherTable(0),
	  m_iter()
{
	assign(other);
}


/** Destructor
 */
TableIterator::~TableIterator()
{
}

/** Assignment operator
 *
 * This method is not SWIG-wrapped, but may be used from C++.
 * Programs using the SWIG wrappers may use TableIterator::assign() in lieu
 * of this operator.
 *
 * @see TableIterator::assign().
 *
 * @param other The other TableIterator from which to get our
 * contents.
 */
TableIterator& TableIterator::operator=(const TableIterator& other)
{
	assign(other);
	return *this;
}

/** Assignment
 *
 * Mimics the assignment operator (TableIterator::operator=), but does
 * not return ourself.
 *
 * @see TableIterator::operator=().
 *
 * @param other The other TableIterator from which to get our
 * contents.
 */
void TableIterator::assign(const TableIterator& other)
{
	m_pMotherTable = other.m_pMotherTable;
	m_iter = other.m_iter;
}


/** Does the Table have more rows?
 *
 * @return true if there are more rows, false if we have reached the
 * end.  <b>DO NOT</b> call next() if hasNext() returns false!
 */
bool TableIterator::hasNext() const
{
	ASSERT_THROW_X(m_pMotherTable != 0, TableException, "No mother table.");
	return m_iter != m_pMotherTable->m_list->end();
}


/** Get the current row and advance iterator.
 *
 * Only call if hasNext() returns true.
 *
 * @return A reference to the TableRow which is the current row.
 */
TableRow& TableIterator::next()
{
	ASSERT_THROW_X(m_pMotherTable != 0, TableException, "No mother table.");
	TableRow& trResult = *m_iter;
	++m_iter;
	return trResult;
}


/** Make iterator recede to the previous table row and get that row.
 *
 * @return A reference to the TableRow which is the previous row.
 *
 */
TableRow& TableIterator::previous()
{
	ASSERT_THROW_X(m_pMotherTable != 0, TableException, "No mother table.");
	--m_iter;
	return *m_iter;
}

/** Get current row without touching iterator.
 *
 * @return The TableRow which is the current row.
 */
TableRow& TableIterator::current()
{
	ASSERT_THROW_X(m_pMotherTable != 0, TableException, "No mother table.");
	return *m_iter;
}


/** Get a column of the current row and advance the iterator
 *
 * Returns the std::string which is at the column index \p column in
 * the current row (the first is 1, not 0), and advances iterator to
 * next row with TableIterator::next().
 *
 * @param column The index of the column of the current row to get
 * (the first is 1, not 0).
 *
 * @return The std::string which is the contents of the indexed column
 * of the current row.
 */
std::string TableIterator::next(int column)
{
	ASSERT_THROW_X(m_pMotherTable != 0, TableException, "No mother table.");
	ASSERT_THROW_X(hasNext(), TableException, "No more elements left.");
	std::string strResult = current().getColumn(column);
	next();
	return strResult;
}


/** Get a column of the current row and leave iterator alone
 *
 * Returns the std::string which is at the column index \p column in
 * the current row (the first is 1, not 0).  In contrast to
 * TableIterator::next(int), this method does not advance the iterator
 * to the next row.
 *
 * @see TableIterator::next(int).
 *
 * @param column The index of the column of the current row to get
 * (the first is 1, not 0).
 *
 * @return The std::string which is the contents of the indexed column
 * of the current row.
 */
std::string TableIterator::getColumn(int column)
{
	ASSERT_THROW_X(m_pMotherTable != 0, TableException, "No mother table.");
	ASSERT_THROW_X(hasNext(), TableException, "No more elements left.");
	return current().getColumn(column);
}






/////////////////////////////////////////////////////////////////
//
// Table
//
/////////////////////////////////////////////////////////////////


/** Convert a table column type to its string equivalent.
 *
 * @internal
 *
 * @param tc_type The TableColumnType to convert.
 *
 * @return The string corresponding to the table column type.
 *
 */
std::string TCT2String(TableColumnType tc_type)
{
	std::string strResult;
	switch (tc_type) {
	case kTCString:
		strResult = "string";
		break;
	case kTCInteger:
		strResult = "integer";
		break;
	case kTCID_D:
		strResult = "id_d";
		break;
	case kTCMonad_m:
		strResult = "monad_m";
		break;
	case kTCBool:
		strResult = "bool";
		break;
	case kTCEnum:
		strResult = "enum";
		break;
	case kTCListOfInteger:
		strResult = "list_of_integer";
		break;
	case kTCListOfID_D:
		strResult = "list_of_id_d";
		break;
	case kTCListOfEnum:
		strResult = "list_of_enum";
		break;
	case kTCSetOfMonads:
		strResult = "set_of_monads";
		break;
	default:
		std::cerr << "tc_type = " << tc_type << std::endl;
		ASSERT_THROW_X(false, TableException, "table.cpp: TCT2String(tc_type): tc_type unknown. Please contact the author of Emdros.");
		strResult = "";
	}
	return strResult;
}

/** Constructor
 */
Table::Table()
	: m_list(0),
	  m_header_names(0),
	  m_header_types(0),
	  m_header_enum_types(0)
{
	m_header_enum_types = new TableRow;
	m_header_types = new TCTList;
	m_list = new TableRowList;
	m_header_names = new TableRow;
}

/** Destructor
 */
Table::~Table()
{
	delete m_list;
	delete m_header_names;
	delete m_header_enum_types;
	delete m_header_types;
}

/** Construct a TableIterator pointing to the first TableRow of the table.
 */
TableIterator Table::iterator()
{
	return TableIterator(this);
}


/** Append a string to the last TableRow.
 *
 * If the list of lists is empty, first make a new list of strings.
 *
 * To use this function, there are two regimes:
 *
 * 1) Table::newline() followed by all Table::append()s for all
 * columns in the row, then loop.
 *
 * 2) Table::startNewRow(), followed by all Table::append()s for all
 *     columns in the row, then loop.
 *
 * @param str The string to append.
 */
void Table::append(const std::string& str)
{
	if (m_list->empty())
		newline();
	m_list->rbegin()->push_back(str);
}

/** Append a std::list of std::string as the new last TableRow.
 *
 * @param l The std::list of std::string to convert to a TableRow and
 * append as such.
 */
void Table::append(const std::list<std::string>& l)
{
	m_list->push_back((TableRow) l);
}

/** Append a header column
 *
 * This is analogous to Table::append(std::string), but appends to the
 * header.  There is only one header row, and it should have as many
 * columns as the rest of the TableRow's.
 *
 * You needn't call Table::newline() before calling this method.
 *
 * @param header_name The name of the header.
 *
 * @param tc_type a TableColumnType describing the type of the column.
 *

 * @param enum_type if \p tc_type is kTCEnum or kTCListOfEnum, this
 * parameter will be used as the enumeration name.

*/
void Table::appendHeader(std::string header_name, TableColumnType tc_type, std::string enum_type)
{
	m_header_names->push_back(header_name);
	m_header_types->push_back(tc_type);
	m_header_enum_types->push_back(enum_type);
}


/** Make an empty table row and append it to the table.
 *
 * This should be called before calling Table::append() for the first
 * column in a table.
 *
 * You can also use Table::starNewRow() in place of this.
 */
void Table::newline()
{
	TableRow l;
	m_list->push_back(l);
}

//
// First column is 1.
// case sensitive according to whether bCompareCaseInsensitively is true or not
/** Find the first TableRow containing \p str at column \p column_index.
 *
 * @param str The string to find.
 *
 * @param column_index The index of the column to search.  The first
 * column is number 1, not 0.
 *
 * @param bCompareCaseInsensitively If true, the string comparison will
 * be done case-insensitively, otherwise case matters.
 *
 * @return The TableIterator pointing to the row, if found.  If this
 * returns false on hasNext(), the string was not found.  Otherwise,
 * if hasNext() returns true, the string was found.
 */
TableIterator Table::find(const std::string& str, 
			  int column_index, 
			  bool bCompareCaseInsensitively)
{
	TableIterator ti = iterator();
	while (ti.hasNext()) {
		TableRowIterator tri = ti.current().iterator();
		bool found = false;
		int col_index = 1;
		while (tri.hasNext() && !found) {
			if (col_index > column_index)
				break;
			if (col_index == column_index) {
				if (bCompareCaseInsensitively) {
					if (strcmp_nocase(tri.current(), str) == 0) {
						found = true;
					}
				} else {
					if (strcmp_case(tri.current(), str) == 0) { 
						found = true;
					}
				}
			}
			++col_index;
			tri.next();
		}
		if (found)
			break;
		ti.next();
	}

	return ti;
}

/** Find the first row containing two specific strings in two specific columns.
 *
 * This is logical AND, not logical OR.
 *
 * The column indexes start at 1 for the first column.
 *
 * The column indexes need not be in any order (i.e., you can have \p column_index1 be greater than \p column_index2, and vice versa.
 *
 * @param str1 The first string to find.
 *
 * @param column_index1 The index of the column to search for \p str1.  The
 * first column is number 1, not 0.
 *
 * @param str2 The second string to find.
 *
 * @param column_index2 The index of the column to search for \p str2.
 * The first column is number 1, not 0.
 *
 * @param bCompareCaseInsensitively If true, the string comparisons will
 * be done case-insensitively, otherwise case matters.
 *
 * @return The TableIterator pointing to the row, if found.  If this
 * returns false on hasNext(), the string-combination was not found.
 * Otherwise, if hasNext() returns true, the string combination was
 * found.
 */
TableIterator Table::find(const std::string& str1, int column_index1, const std::string& str2, int column_index2, bool bCompareCaseInsensitively)
{
	TableIterator ti = iterator();
	while (ti.hasNext()) {
		TableRowIterator tri = ti.current().iterator();
		bool found = false;
		bool found1 = false;
		bool found2 = false;
		int col_index = 1;
		while (tri.hasNext() && !found) {
			if (col_index > column_index1 && col_index > column_index2)
				break;
			const std::string& strString = tri.current();

			if (col_index == column_index1) {
				if (bCompareCaseInsensitively) {
					if (strcmp_nocase(strString, str1) == 0)
						found1 = true;
				} else {
					if (strcmp_case(strString, str1) == 0)
						found1 = true;
				}
			}

			if (col_index == column_index2) {
				if (bCompareCaseInsensitively) {
					if (strcmp_nocase(strString, str2) == 0)
						found2 = true;
				} else {
					if (strcmp_case(strString, str2) == 0)
						found2 = true;
				}
			}

			if (found1 && found2)
				found = true;

			++col_index;
			tri.next();
		}
		if (found)
			break;
		ti.next();
	}

	return ti;
}


/** Erase a TableRow based on a TableIterator pointing to it.
 *
 * The TableIterator must point to a valid row (i.e., hasNext() must
 * not return false).
 *
 * @param d A TableIterator pointing to the row to be erased.
 */
void Table::erase(TableIterator& d)
{
	m_list->erase(d.m_iter);
}

/** Update a column.
 *
 * @param i A TableIterator pointing to the row containing the column
 * to be updated.
 *
 * @param new_value The new string value for the column.
 *
 * @param column_index The index of the column to be updated.  The
 * first column is number 1, not number 0.
 */
void Table::updateRow(TableIterator& i, const std::string new_value, int column_index)
{
	TableRowIterator tri = i.current().iterator();
	int col_index = 1;
	while (tri.hasNext()) {
		if (col_index == column_index) {
			(*(tri.m_iter)) = new_value;
			return;
		}
		++col_index;
		tri.next();
	}
	std::cerr << "Error: table.cpp:Table::updateRow(): column_index '" << column_index << "' out of range. col_index = '" << col_index << "'." << std::endl;
	ASSERT_THROW_X(false, TableColumnException, "Table::updateRow(): column index out of range.");
	return;
}

/** Get a column value
 *
 * @param i A TableIterator pointing to the row containign the column
 * to get.
 *
 * @param column_index The index of the column to get.  Column indexes
 * start at 1 for the first column.
 *
 * @return The string value from the column.
 */
std::string Table::getColumn(TableIterator& i, int column_index)
{
	return i.current().getColumn(column_index);
}

/** Return the number of TableRows (not including the header)
 *
 * For example, if you have two rows and a header, this will return 2.
 *
 * @return The number of TableRows in the table.
 */
long Table::size() const
{
	return m_list->size();
}


/** Start a new row
 *
 * You can use this in place of Table::newline() AFTER you've
 * Table::append()'ed all columns in a row.
 *
 * Thus there are two regimes:
 *
 * 1) Table::newline() followed by all Table::append()s for all
 * columns in the row, then loop.
 *
 * 2) Table::startNewRow(), followed by all Table::append()s for all
 *     columns in the row, then loop.
 */
void Table::startNewRow()
{
	// Only add a new line if the list is not empty
	if (m_list->begin() != m_list->end()) {
		newline();
	}
}

/** Print on an EMdFOutput with console style
 *
 * @param pOut A pointer to the EMdFOutput to use.
 */
void Table::printConsole(EMdFOutput* pOut) const
{
	std::list<std::string::size_type> max_lengths;
	TableRow headers;
	createHeaders(headers);
	getMaxLengthsAll(max_lengths, headers);
	dumpResultsAll(pOut, max_lengths, headers);
}

/** Create readable (console) headers.
 *@internal
 *
 * Used by Table::printConsole().
 *
 * @param headers Used to return the readable (console) headers as a
 * TableRow.
 */
void Table::createHeaders(TableRow& headers) const
{
	// Make sure we are pristine
	headers.clear();

	//
	// Build headers
	//
	TableRow::const_iterator ci_header_names(m_header_names->begin());
	TableRow::const_iterator ci_header_names_end(m_header_names->end());
	TCTList::const_iterator ci_header_types(m_header_types->begin());
	TableRow::const_iterator ci_header_enum_types(m_header_enum_types->begin());
	while (ci_header_names != ci_header_names_end) {
		std::string header;

		// header-name
		header = *ci_header_names + " : ";

		// header-type
		header += TCT2String(*ci_header_types);

		// Possible enum-type
		if (*ci_header_types == kTCEnum
		    || *ci_header_types == kTCListOfEnum) {
			header += "(" + *ci_header_enum_types +")";
		}

		// Push it back
		headers.push_back(header);

		// Advance iterators
		++ci_header_names;
		++ci_header_types;
		++ci_header_enum_types;
	}
}

/** Emit a number of minuses (used to draw console tables).
 *
 * @internal
 *
 * @param pOut The EMdFOutput to use.
 *
 * @param no_of_minuses The number of minuses to emit.
 *
 */
void emit_minuses(EMdFOutput* pOut, std::string::size_type no_of_minuses)
{
	std::string minuses(no_of_minuses, '-');
	pOut->out(minuses);
}

/** Emit (console) horizontal line
 *@internal
 *
 * @param pOut The EMdFOutput to emit on.
 *
 * @param max_lengths The maximum lengths of each column.
 */
void Table::emitHorizontalLine(EMdFOutput* pOut, const std::list<std::string::size_type>& max_lengths) const
{
	// Output leading "+"
	pOut->out("+");

	// Output each of the max_lengths
	std::list<std::string::size_type>::const_iterator ci(max_lengths.begin());
	std::list<std::string::size_type>::const_iterator cend(max_lengths.end());
	while (ci != cend) {
		// Emit minuses
		emit_minuses(pOut, *ci + 2);

		// Emit "+"
		pOut->out("+");

		// Advance iterator
		++ci;
	}

	// Emit trailing newline
	pOut->newline();
}

/** Dump all (console) results.
 *@internal
 *
 * @param pOut The EMdFOUtput to use.
 *
 * @param max_lengths The maximum lengths of each column.
 *
 * @param headers Readable headers as a TableRow.
 */
void Table::dumpResultsAll(EMdFOutput* pOut, 
			   const std::list<std::string::size_type>& max_lengths, 
			   const TableRow& headers) const 
{
	// Emit horizontal line
	emitHorizontalLine(pOut, max_lengths);

	// Dump header
	dumpResults(pOut, headers, max_lengths);

	// Emit horizontal line
	emitHorizontalLine(pOut, max_lengths);

	// Dump results
	TableRowList::const_iterator ci(m_list->begin());
	TableRowList::const_iterator cend(m_list->end());
	while (ci != cend) {
		dumpResults(pOut, *ci, max_lengths);
		++ci;
	}

	// Emit horizontal line
	emitHorizontalLine(pOut, max_lengths);
}

/** Dump results for a single table row (console).
 *@internal
 *
 * @param pOut The EMdFOutput to use.
 *
 * @param strlist The TableRow to dump.
 *
 * @param max_lengths The maximum lengths of each column.
 */
void Table::dumpResults(EMdFOutput* pOut, 
			const TableRow& strlist, 
			const std::list<std::string::size_type>& max_lengths) const 
{
	// Output initial "|".
	pOut->out("|");

	// Declare and initialize iterators
	std::list<std::string::size_type>::const_iterator mli;
	TableRow::const_iterator sli;
	TableRow::const_iterator sliend;
	mli = max_lengths.begin();
	sli = strlist.begin();
	sliend = strlist.end();

	// Iterate through lists
	while (sli != sliend) {
		// Output leading " " and string
		pOut->out(" ");
		pOut->out(*sli);

		// Output padding
		std::string spaces(*mli - sli->length() + 1, ' ');
		pOut->out(spaces);

		// Output trailing "|"
		pOut->out("|");

		// Advance iterators
		++mli;
		++sli;
	}

	// Output trailing endl
	pOut->newline();
}


/** Compute maximum lengths from a single TableRow.
 *@internal
 *
 * Used when computing the maximum lengths of all columns in all rows.
 * Only updates \p max_lengths if the value for the given column is
 * higher than what is in \p max_lengths.
 *
 * @param strlist The TableRow to process.
 *
 * @param max_lengths The current maximum lengths of all columns.  May
 * be the empty list to start with, since this method resizes \p
 * max_lengths when necessary (which should only be for the first row,
 * since all rows should have the same number of columns).
 */
void Table::getMaxLengths(const TableRow& strlist, 
			  std::list<std::string::size_type>& max_lengths) const
{
	// Resize max_lengths if necessary to add members
	if (max_lengths.size() < strlist.size()) {
		max_lengths.resize(strlist.size(), 0);
	}

	// Iterate over strlist
	std::list<std::string::size_type>::iterator mli;
	TableRow::const_iterator sli;
	TableRow::const_iterator sliend;
	mli = max_lengths.begin();
	sli = strlist.begin();
	sliend = strlist.end();
	while (sli != sliend) {
		if (sli->length() > *mli)
			*mli = sli->length();

		++mli;
		++sli;
	}
}


/** Get maximum lengths for all of the table.
 *
 * @param max_lengths used to return the maximum lengths.  May be
 * empty when calling this method, as it is automatically resized.
 * Indeed, it is completely erased before anything happens to it.
 *
 * @param headers The TableRow containing the readable console headers.
 */
void Table::getMaxLengthsAll(std::list<std::string::size_type>& max_lengths, 
			     const TableRow& headers) const
{
	// Erase so we are sure we begin fresh
	max_lengths.erase(max_lengths.begin(), max_lengths.end());

	// Do headers
	getMaxLengths(headers, max_lengths);

	// Do results
	TableRowList::const_iterator ci(m_list->begin());
	TableRowList::const_iterator cend(m_list->end());
	while (ci != cend) {
		getMaxLengths(*ci, max_lengths);
		++ci;
	}
}


/** Erase all table rows in the table, but not the headers.
 */
void Table::clear()
{
	m_list->clear();
}

/** Print table as XML.
 *
 * @param pOut the EMdFOutput to use.  This must have been initialized
 * to use XML.
 */
void Table::printXML(EMdFOutput *pOut) const
{
	pOut->startTag("table", true);
	printXMLHeader(pOut);
	printXMLRows(pOut);
	pOut->endTag("table", true);
}

/** Print XML header
 *
 * @param pOut the EMdFOutput to use.  This must have been initialized
 * to use XML.
 */
void Table::printXMLHeader(EMdFOutput *pOut) const
{
	pOut->startTag("theader", true);
	TableRow::const_iterator ci_header_names(m_header_names->begin());
	TableRow::const_iterator ci_header_names_end(m_header_names->end());
	TCTList::const_iterator ci_header_types(m_header_types->begin());
	TableRow::const_iterator ci_header_enum_types(m_header_enum_types->begin());
	while (ci_header_names != ci_header_names_end) {
		// Start theadercolumn
		pOut->startTag("theadercolumn", true);

		// tcaption
		pOut->startTag("tcaption", false); 
		pOut->outCharData(*ci_header_names);
		pOut->endTag("tcaption", false);

		// ttype
		AttributePairList attribute_list;
		attribute_list.push_back(AttributePair(std::string("type"), TCT2String(*ci_header_types)));
		if (*ci_header_types == kTCEnum
		    || *ci_header_types == kTCListOfEnum) {
			attribute_list.push_back(AttributePair(std::string("enum_type"), *ci_header_enum_types));
		}
		pOut->startSingleTag("ttype", attribute_list, false);

		// End theadercolumn
		pOut->endTag("theadercolumn", false);

		// Advance iterators
		++ci_header_names;
		++ci_header_types;
		++ci_header_enum_types;
	}

	// End theader tag
	pOut->endTag("theader", true);
}

/** Print all TableRow's as XML.
 *
 * @param pOut the EmdFOutput to use.
 */
void Table::printXMLRows(EMdFOutput *pOut) const
{
	TableRowList::const_iterator ci(m_list->begin());
	TableRowList::const_iterator cend(m_list->end());
	while (ci != cend) {
		printXMLRow(pOut, *ci);
		++ci;
	}
}

/** Print a TableRow as XML.
 *
 * @param pOut The EmdFOutput to use.
 *
 * @param sl The TableRow to print.
 */
void Table::printXMLRow(EMdFOutput *pOut, const TableRow& sl) const
{
	// Start trow tag
	pOut->startTag("trow", true);
	TableRow::const_iterator ci(sl.begin());
	TableRow::const_iterator cend(sl.end());
	while (ci != cend) {
		// Output tcolumn
		pOut->startTag("tcolumn", true);
		pOut->outCharData(*ci);
		pOut->endTag("tcolumn", false);

		// Advance iterator
		++ci;
	}

	// End trow tag
	pOut->endTag("trow", true);
}

/** Print DTD for Table.  The root element of the Table DTD is \p "table".
 *
 * @param pOut the EMdFOutput to print it on.
 */
void Table::printDTD(EMdFOutput *pOut)
{
	pOut->out("  <!ELEMENT table (theader, trow*) >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT theader (theadercolumn)* >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT theadercolumn (tcaption, ttype) >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT tcaption (#PCDATA)* >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT ttype EMPTY >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT trow (tcolumn)* >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT tcolumn (#PCDATA)* >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST table >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST theader >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST theadercolumn >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST tcaption >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST ttype "); 
	pOut->newline();
	pOut->out("     type (bool|string|integer|id_d|enum|list_of_integer|list_of_id_d|list_of_enum|set_of_monads) #REQUIRED"); 
	pOut->newline();
	pOut->out("     enum_type CDATA #IMPLIED >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST trow >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST tcolumn >"); 
	pOut->newline();
}


/** Print table as JSON.
 *
 * @param pOut the EMdFOutput to use.  This must have been initialized
 * to use JSON.
 */
void Table::printJSON(EMdFOutput *pOut) const
{
    pOut->jsonLabel("table");

    pOut->jsonStartStruct();

    printJSONHeader(pOut);
    printJSONRows(pOut);

    pOut->jsonEndStruct();
}

/** Print JSON header
 *
 * @param pOut the EMdFOutput to use.  This must have been initialized
 * to use JSON.
 */
void Table::printJSONHeader(EMdFOutput *pOut) const
{
    pOut->jsonLabel("theader");

    pOut->jsonStartArray();
    TableRow::const_iterator ci_header_names(m_header_names->begin());
    TableRow::const_iterator ci_header_names_end(m_header_names->end());
    TCTList::const_iterator ci_header_types(m_header_types->begin());
    TableRow::const_iterator ci_header_enum_types(m_header_enum_types->begin());
    while (ci_header_names != ci_header_names_end) {
        // Start theadercolumn
        pOut->jsonStartStruct();

        // tcaption
        pOut->jsonLabel("tcaption");
        pOut->jsonOutValue(*ci_header_names);

        // ttype
        pOut->jsonLabel("ttype");
        pOut->jsonOutValue(TCT2String(*ci_header_types));

        if (*ci_header_types == kTCEnum
            || *ci_header_types == kTCListOfEnum) {
            pOut->jsonLabel("enum_type");
            pOut->jsonOutValue(*ci_header_enum_types);
        }
        
        // End theadercolumn
        pOut->jsonEndStruct();

        // Advance iterators
        ++ci_header_names;
        ++ci_header_types;
        ++ci_header_enum_types;
    }

    // End theader tag
    pOut->jsonEndArray();
}

/** Print all TableRow's as JSON.
 *
 * @param pOut the EmdFOutput to use.
 */
void Table::printJSONRows(EMdFOutput *pOut) const
{
    pOut->jsonLabel("trows");
    pOut->jsonStartArray();

    TableRowList::const_iterator ci(m_list->begin());
    TableRowList::const_iterator cend(m_list->end());
    while (ci != cend) {
        printJSONRow(pOut, *ci);
        ++ci;
    }

    pOut->jsonEndArray();
}

/** Print a TableRow as JSON.
 *
 * @param pOut The EmdFOutput to use.
 *
 * @param sl The TableRow to print.
 */
void Table::printJSONRow(EMdFOutput *pOut, const TableRow& sl) const
{
    // Start trow tag
    pOut->jsonStartStruct();
    pOut->jsonLabel("tcolumns");
    pOut->jsonStartArray();

    TableRow::const_iterator ci(sl.begin());
    TableRow::const_iterator cend(sl.end());
    while (ci != cend) {
        // Output tcolumn
        pOut->optComma();
        pOut->jsonOutValue(*ci);

        // Advance iterator
        ++ci;
    }

    // End trow
    pOut->jsonEndArray();
    pOut->jsonEndStruct();
}


// Equals except for header
/** Tests all TableRows (excluding the header) for equality with
 *  another table.
 *
 * @param other The other Table to compare with
 *
 * @return True on all TableRows compared equal in all columns.
 */
bool Table::equals(Table& other)
{
	TableIterator myiter = this->iterator();
	TableIterator youriter = other.iterator();

	// If one but not both iterators are done,
	// then the tables were not identical
	if ((myiter.hasNext() && !youriter.hasNext())
	    ||
	    (!myiter.hasNext() && youriter.hasNext())) {
		return false;
	}
  
      
	// Only enter loop if there is something in both iterators
	bool bGoOn = myiter.hasNext() && youriter.hasNext();

	while (bGoOn) {
		TableRow& myrow = myiter.next();
		TableRow& yourrow = youriter.next();

		// Check equality
		if (!myrow.equals(yourrow))
			return false;
    
		// If one but not both iterators are done,
		// then the tables were not identical
		if ((myiter.hasNext() && !youriter.hasNext())
		    ||
		    (!myiter.hasNext() && youriter.hasNext())) {
			return false;
		}
    
		bGoOn = bGoOn && myiter.hasNext() && youriter.hasNext();
	}

	// If we came this far, the tables were identical
	return true;
}
