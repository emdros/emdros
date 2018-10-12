/*
 * HALSpace.h
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



#ifndef HALSPACE__H__
#define HALSPACE__H__

#include <environment_emdros.h>
#include <monads.h>
#include <vector>
#include <map>
#include <string>
#include <mql_sheaf.h>
#include <arena.h>


typedef std::vector<double> DoubleVec;

typedef std::vector<double*> PDoubleVec;

typedef std::vector<std::vector<int> > IntVecVec;

typedef std::map<int, int> Int2IntMap;
typedef std::map<int, Int2IntMap *> Int2PInt2IntMapMap;

typedef std::map<int, double> Int2DoubleMap;
typedef std::map<int, Int2DoubleMap *> Int2PInt2DoubleMapMap;

typedef std::vector<std::pair<std::string, bool> > StringBoolVec;
typedef std::vector<std::string> StringVec;

typedef std::map<int, std::string> IntStringMap;
typedef std::map<std::string, int> StringIntMap;


typedef struct IntIntDoubleTriple {
	int m_row;
	int m_column;
	double m_real_value;
        IntIntDoubleTriple(int row, int column, double rv) : m_row(row), m_column(column), m_real_value(rv) {};
	bool operator<(const IntIntDoubleTriple& other) const {
		if (m_real_value < other.m_real_value) {
			return true;
		} else if (m_real_value > other.m_real_value) {
			return false;
		} else if (m_row < other.m_row) {
			return true;
		} else if (m_row > other.m_row) {
			return false;
		} else if (m_column < other.m_column) {
			return true;
		} else {
			return false;
		}
	}
} IntIntDoubleTriple;

typedef std::vector<IntIntDoubleTriple> IIDTripleVec;

typedef struct MyTriple {
	int value;
	double real_value;
	std::string other_word;
	MyTriple(const std::string& ow, double rv, int v) : value(v), real_value(rv), other_word(ow) {};
	bool operator<(const MyTriple& other) const { return real_value > other.real_value; };
} MyTriple;

typedef std::vector<MyTriple> MyTripleVec;

class MyMatrix {
 private:
	Int2PInt2IntMapMap m_matrix; // row --> column --> number
	int m_cur_row;
	Int2IntMap *m_pCurRow;
 public:
        MyMatrix(int rows, int columns);
	~MyMatrix();
	int get(int row, int column);
	void set(int row, int column, int value);
	void add(int row, int column, int value_to_add);
};

class DoubleMatrix {
 private:
	Int2PInt2DoubleMapMap m_matrix; // row --> column --> number
	int m_cur_row;
	Int2DoubleMap *m_pCurRow;
 public:
        DoubleMatrix(int rows, int columns);
	~DoubleMatrix();
	double get(int row, int column);
	void set(int row, int column, double value);
};

class MyDoubleMatrix {
 private:
	PDoubleVec m_matrix;
	int m_rows;
	int m_columns;
 public:
        MyDoubleMatrix(int rows, int columns);
	~MyDoubleMatrix();
	double get(int row, int column);
	void set(int row, int column, double value);
};

class HALSpace {
	int m_n;
	MyMatrix *m_pMatrix;
	IntStringMap m_hal_words;
	StringIntMap m_hal_word_indexes_by_word;
	monad_m m_first_HAL_monad;
	int m_max_hal_word;
	bool m_bAddNumbers;
	EmdrosEnv *m_pEE;
 public:
	HALSpace(int n, monad_m first_HAL_monad, EmdrosEnv *pEE);

	~HALSpace();
  
	void EmptyMatrix();

	void load();
 private:

	void testSheafNotEmpty(const Sheaf *pSheaf);            
	void load_space();
	void process_hal_space_mo(const MatchedObject *hal_space_mo, int which);
	void process_hal_space_sheaf(const Sheaf *pSheaf, int which);        
	void harvest_words(const Sheaf *pSheaf);
	std::string get_mo_feature(const MatchedObject *pMo, unsigned int feature_index);
	void process_hal_word_mo(const MatchedObject *hal_word_mo);            
	void harvest_cells(const Sheaf *pSheaf);            
	bool db_has_space();
	void save_space();
	bool maybeExecQuery(Bigstring **ppQuery,
			    const std::string& object_type_name, 
			    int& count,
			    bool bDoItRegardless);  
	void save_cells();
	void save_row(int row_index, Bigstring *pQuery, int &count);
	void save_cell(int row_index, int column_index, int value, Bigstring *pQuery);
	void save_hal_space();
	Sheaf *executeQueryGetSheaf(const std::string& query);
	bool executeString(const std::string& query);
	void generate_space();
	void generate_reverse_lookup_dictionary();
	void load_hal_words();
	SetOfMonads get_text_object();
	int get_text_length();
	void load_text_words(const SetOfMonads& som, StringBoolVec& l);
	void process_text_mo(const MatchedObject *pText_mo, StringBoolVec& l);
	void process_text_word_mo(const MatchedObject *text_word_mo, StringBoolVec& l);
	void traverse_words();
	void advance_indices(int &index_tail, int &index_head, monad_m last_monad);
	void traverse_word_range(StringBoolVec& text_word_list, 
				 int index_tail, int index_head);
	void add_word_pair(const std::pair<std::string, bool>& first_word, 
			   const std::pair<std::string, bool>& second_word, 
			   int addition);	      
	void write_row_to_csv(std::ostream *pOut, int row_index);
	void write_similarity_row_to_csv(std::ostream *pOut, int row_index, MyDoubleMatrix *pMatrix);
	void write_word_vector(const std::string& word, 
			       int factor, 
			       int text_length, 
			       unsigned int max_values, 
			       std::ostream *pOut);
	void get_word_vector(const std::string& word, 
			     int row_index, 
			     int factor, 
			     int text_length, 
			     int max_values,
			     MyTripleVec& mytriplevec);
	void compute_normalized_matrix_etc(MyDoubleMatrix& normalized_matrix, MyDoubleMatrix& nm_squared, DoubleVec& Ai2);
	double compute_similarity_measure(int i, int j, MyDoubleMatrix& nm_squared, const DoubleVec& SQRT_Ai2);
 public:
	void writeSQL(std::ostream *pOut);
	void writeCXT(std::ostream *pOut, const StringVec& words, int threshold);
	void write_to_csv_file(std::ostream *pOut, const StringVec& interesting_words);
	void write_word_vectors(const StringVec& words, 
				int factor, 
				int max_values, 
				std::ostream *pOut);
	void write_similarity_measures(std::ostream *pOut);
};
                
        


#endif
