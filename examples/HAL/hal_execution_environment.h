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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2005  Ulrik Petersen
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, license version 2.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 **************************************************************************/

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

