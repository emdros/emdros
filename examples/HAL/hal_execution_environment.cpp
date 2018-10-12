/*
 * hal_execution_environment.cpp
 *
 * HAL Execution environment
 *
 * Ulrik Petersen
 * Created: 9/12-2002 (December 9, 2002)
 * Last update: 3/1-2017
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


#include <hal_execution_environment.h>
#include <string_func.h>
#include <hal_string_func.h>
#include <cstring>

HAL_Execution_environment::HAL_Execution_environment(std::istream *pIStream, std::ostream *pOStream) 
{
	UNUSED(pOStream); // FIXME: Remove from parameters, then!
	
	pYYLexer = new HALScanner(pIStream);
}

HAL_Execution_environment::~HAL_Execution_environment() 
{ 
	delete pYYLexer;
}



// Word set
void HAL_Execution_environment::add_word(std::string word)
{
  std::string word_to_find;
  str_tolower(word, word_to_find);
  WordSet_t::iterator i = m_word_set.find(word_to_find);
  if (i == m_word_set.end()) {
    // Add it
    m_word_set[word_to_find] = 1;
  } else {
    // Increment usage number
    (i->second)++;
  }
}

HAL_Execution_environment::const_iterator HAL_Execution_environment::begin(void) const
{
  return m_word_set.begin();
}

HAL_Execution_environment::const_iterator HAL_Execution_environment::end(void) const
{
  return m_word_set.end();
}

HAL_Execution_environment::const_iterator HAL_Execution_environment::find(std::string word)
{
  std::string word_to_find;
  str_tolower(word, word_to_find);
  return m_word_set.find(word_to_find);
}


// Lexer
int HAL_Execution_environment::yylex(void)
{
  return pYYLexer->scan(this);
}


void HAL_Execution_environment::Print(std::ostream *pOut) const
{
  int count = 0;
  const_iterator ci;
  ci = begin();
  char szOut[512];
  while (ci != end()) {
    sprintf(szOut, "%05d %s", count, ci->first.c_str());
    (*pOut) << szOut << std::endl;
    ci++;
    count++;
  }
}

void HAL_Execution_environment::set_current_word(std::string cw)
{
  m_current_word = cw;
  std::string tmp;
  str_tolower(cw, tmp);
  m_current_stripped_surface = hal_strip_string(tmp, "()\"\'");
}


bool HAL_Execution_environment::is_number(const HAL_Execution_environment::const_iterator& ci) const
{
  std::string word = ci->first;
  const char *pString = word.c_str();
  if (strspn(pString, "0123456789.,") == strlen(pString) 
      && strpbrk(pString, "0123456789") != NULL)
    return true;
  else
    return false;
}
