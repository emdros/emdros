/*
 * hal_execution_environment.h
 *
 * Execution environment
 *
 * Ulrik Petersen
 * Created: 9/12-2002 (December 9, 2002)
 * Last update: 5/11-2005
 *
 **********************************************************************/

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



#ifndef _HAL_EXECUTION_ENVIRONMENT__H__
#define _HAL_EXECUTION_ENVIRONMENT__H__

#include <string>
#include <map>
#include <iostream>
#include <hallexer.h>

typedef std::map<std::string, int> WordSet_t;

class HAL_Execution_environment {
private:
  HALScanner *pYYLexer;
  WordSet_t m_word_set;
  std::string m_current_word;
  std::string m_current_punctuation;
  std::string m_current_stripped_surface;
public:
  typedef WordSet_t::const_iterator const_iterator;
  // Methods
  HAL_Execution_environment(std::istream *pIStream, std::ostream *pOStream);
  ~HAL_Execution_environment();

  // Word set
  void add_word(std::string word);
  const_iterator begin(void) const;
  const_iterator end(void) const;
  const_iterator find(std::string word);
  bool is_number(const HAL_Execution_environment::const_iterator& ci) const;

  // Lexer
  int yylex(void);

  // Current word
  void set_current_word(std::string cw);
  std::string get_current_word(void) { return m_current_word; };
  std::string get_current_stripped_surface(void) { return m_current_stripped_surface; };

  // Current punctuation
  void set_current_punctuation(std::string cp) { m_current_punctuation = cp; };
  std::string get_current_punctuation(void) { return m_current_punctuation; };

  // Printing
  void Print(std::ostream *pOut) const;

};

extern HAL_Execution_environment *pHEE;

#endif // _HAL_EXECUTION_ENVIRONMENT__H__

