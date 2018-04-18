/*
 * table.h
 *
 * Tables
 *
 * Ulrik Petersen
 * Created: 5/5-2001 
 * Last update: 3/1-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2017  Ulrik Sandborg-Petersen
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

/**@file table.h
 *@brief Header file for Table (EMdF layer)
 */



#ifndef TABLE__H__
#define TABLE__H__


#include "emdf.h"
#ifndef SWIG
#include "emdf_output.h"
#include "emdf_enums.h"
#endif // !defined SWIG

#include <string>

#undef list
#include <list>

#ifdef SWIG
#pragma SWIG nowarn=401
#endif

/**
 *\defgroup Table Tables (EMdF layer)
 *\ingroup EMdF
 */


#ifndef SWIG
/** list of TableColumnType.
 *@ingroup Table
 */
typedef std::list<TableColumnType> TCTList;

#endif



class Table; // Forward declaration
class TableRow; // Forward declaration

/** An iterator on a table row (getting the columns)
 *@ingroup Table
 *
 * The columns are numbered beginning with 1, not 0.
 *
 */
class TableRowIterator {
 private:
	friend class Table;
	TableRow *m_pMotherTableRow;
	std::list<std::string>::iterator m_iter;
 public:
	TableRowIterator(TableRow *pMotherTableRow);
	TableRowIterator();
	TableRowIterator(const TableRowIterator& other);
 public:
	~TableRowIterator();
#ifndef SWIG
	TableRowIterator& operator=(const TableRowIterator& other);
#endif 
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	std::string next(); // Gets current and advances iterator afterwards
	std::string previous(); // Regresses iterator and then gets current
	std::string current(); // Gets current without altering iterator
	std::string getColumn(int column); // Gets column in current without touching iterator
 private:
	void assign(const TableRowIterator& other);
};

/** A Table row.
 *@ingroup Table
 *
 * Is basically a std::list of std::string.
 *
 * Use TableRowIterator to iterate over a TableRow.
 *
 * The columns are numbered beginning with 1, not 0.
 *
 */
class TableRow : public std::list<std::string> {
 public:
	TableRow();
#ifndef SWIG
	TableRow(std::list<std::string> other);
 TableRow(const TableRow& other) : std::list<std::string>() {
		assign(other);
	};
	TableRow& operator=(const TableRow& other)  {
		assign(other);
		return *this;
	};
 private:
	void assign(const TableRow& other) {
		*((std::list<std::string>*) this) = other;
	}
 public:
#endif
	virtual ~TableRow();
	TableRowIterator iterator();  // Gets iterator pointing to beginning
	std::string getColumn(int column); // Gets column
	unsigned int size() const;  // Gets number of columns
	bool equals(TableRow& other);
};

/** Iterator on a Table (getting TableRow objects)
 *@ingroup Table
 *
 */
class TableIterator {
 private:
	friend class Table;
	Table *m_pMotherTable;
	std::list<TableRow>::iterator m_iter;
 public:
	TableIterator();
	TableIterator(Table *pMotherTable);
	TableIterator(const TableIterator& other);
 public:
	~TableIterator();
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	TableRow& next(); // Gets current and advances iterator afterwards
	TableRow& previous();  // Regresses iterator and then gets current
	TableRow& current(); // Gets current without altering iterator
	std::string next(int column);  // Gets column in current and then advances iterator
	std::string getColumn(int column); // Gets column in current without altering iterator
#ifndef SWIG
	TableIterator& operator=(const TableIterator& other);
#endif
 private:
	void assign(const TableIterator& other);
};





/** A table (rows are lists of strings, so columns are strings)
 *@ingroup Table
 *
 * The table is the kind of output returned from all MQL statements
 * which return something, except for "SELECT (ALL|FOCUS) OBJECTS",
 * which returns a Sheaf or a FlatSheaf.
 *
 * A table is a list of TableRows. A TableRow, in turn, is a list of
 * strings.
 *
 * The columns are numbered beginning with 1, not 0.
 *
 * Java-style iterators are provided for accessing the rows and their
 * contents.
 *
 * \section Examples
 *
 * \subsection Iterators (reading from a table)
 *
 * Here is an example of how to use the iterators.
 *
 * \code
 * Table *pTable; // Assumed to be initialized from somewhere
 *
 * // Get first string of first row
 * TableIterator ti = pTable->iterator();
 * // Note how column 1 is the first.
 * std::string firstRowCol = pTable->getColumn(ti, 1);
 *
 * // out table
 * ti = pTable->iterator();
 * while (ti.hasNext()) {
 *   // Get iterator for current row
 *   TableRowIterator tri = ti.current().iterator();
 *
 *   // out row
 *   while (tri.hasNext()) {
 *     // out string plus horizontal tab, and advance iterator
 *     printf("%s\t", tri.next());
 *   }
 *
 *   // out newline
 *   printf("\n");
 *
 *   // Advance iterator
 *   ti.next();
 * }
 *
 * // Find something in table
 * int column_to_search = 2;
 * TableIterator ti2 = pTable->find("StringToFind", column_to_search);
 * if (ti2.hasNext()) {
 *   printf("String found!\n");
 *   TableRow& tr = ti.current();
 *   printf("Column 1 = '%s'\n", tr.getColumn(1));
 * } else {
 *   printf("String not found.\n");
 * }
 * \endcode
 *
 * \subsection Adding to a table
 *
 * Here's an example of how to add to a table:
 *
 * \code
 * // Create header (not really necessary if you know what you've
 * // placed into the table; mostly for aesthetics when printing
 * // and for communicating with other parts of the program)
 * pTable->appendHeader("first_monad", kTCMonad_m);
 * pTable->appendHeader("last_monad", kTCMonad_m);
 * 
 * // Start a new row (call this method before starting a new row)
 * pTable->startNewRow();
 *
 * // Add row 1
 * pTable->append(1); // Monad 1 in first_monad
 * pTable->append(4); // Monad 4 in last_monad
 *
 *
 * // Add row 2
 * pTable->startNewRow();
 * pTable->append(10); // Monad 10 in first_monad
 * pTable->append(13); // Monad 13 in last_monad
 * \endcode

*/
class Table {
 private:
	typedef std::list<TableRow> TableRowList;
	friend class TableIterator;
	TableRowList *m_list;
	TableRow *m_header_names;
	TCTList *m_header_types;
	TableRow *m_header_enum_types;
 public:
	Table();
 private:
	Table(const Table& other);
	Table& operator=(const Table& other);
 public:
	~Table();
	void append(const std::string& str);
#ifndef SWIG
	void append(const std::list<std::string>& l);
#endif // !defined SWIG
	void newline();
	void appendHeader(std::string header_name, TableColumnType tc_type, std::string enum_type = "");
	void updateRow(TableIterator& i, const std::string new_value, int column_index);
	void startNewRow();
	TableIterator iterator();
	TableIterator find(const std::string& str, int column_index, bool bCompareCaseInsensitively = true);
	TableIterator find(const std::string& str1, int column_index1, const std::string& str2, int column_index2, bool bCompareCaseInsensitively = true);
	void erase(TableIterator& d);
	void clear();
	std::string getColumn(TableIterator& i, int column_index);
	void printConsole(EMdFOutput *pOut) const;
	void printXML(EMdFOutput *pOut) const;
	void printJSON(EMdFOutput *pOut) const;
	static void printDTD(EMdFOutput *pOut);
	long size() const; // Get number of rows
	bool equals(Table& other); // Equals except for header
 private:
	void createHeaders(TableRow& headers) const;
	void printXMLHeader(EMdFOutput *pOut) const;
	void printXMLRows(EMdFOutput *pOut) const;
	void printXMLRow(EMdFOutput *pOut, const TableRow& sl) const;
	void printJSONHeader(EMdFOutput *pOut) const;
	void printJSONRows(EMdFOutput *pOut) const;
	void printJSONRow(EMdFOutput *pOut, const TableRow& sl) const;
	void getMaxLengthsAll(std::list<std::string::size_type>& lengths, const TableRow& headers) const;
	void getMaxLengths(const TableRow& strlist, std::list<std::string::size_type>& lengths) const;
	void dumpResultsAll(EMdFOutput* pOut, const std::list<std::string::size_type>& max_lengths, const TableRow& headers) const;
	void dumpResults(EMdFOutput* pOut, const TableRow& strlist, const std::list<std::string::size_type>& max_lengths) const;
	void emitHorizontalLine(EMdFOutput* pOut, const std::list<std::string::size_type>& max_lengths) const;
};


#endif /* TABLE__H__ */
