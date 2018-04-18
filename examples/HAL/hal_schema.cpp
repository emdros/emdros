/*
 * hal_schema.cpp
 *
 * HAL schema string
 *
 * Created: Sometime in the first half of 2003
 * Last update: 11/3-2015
 *
 ************************************************************************/
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2015  Ulrik Petersen
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

#include <hal_schema.h>

std::string strHALschema = 
  "//\n"
  "// schema.mql - schema for stem cell research project\n"
  "//\n"
  "// Created: 29/5-2003\n"
  "// Last update: 5/7-2003\n"
  "//\n"
  "\n"
  "CREATE ENUMERATION boolean = {\n"
  "  false = 0, \n"
  "  true = 1\n"
  "} GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE MONAD OBJECTS\n"
  "[Word\n"
  "   surface : string FROM SET WITH INDEX;\n"
  "   stripped_surface : string FROM SET WITH INDEX;\n"
  "   is_number : boolean DEFAULT false;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "[Sentence\n"
  "  end_of_sentence_punctuation : STRING(1);\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "[Text\n"
  "   add_numbers : boolean DEFAULT false;\n"	
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE MONAD OBJECTS\n"
  "[HAL_word\n"
  "   stripped_surface : string FROM SET WITH INDEX;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE MONAD OBJECTS\n"
  "[HAL_cell\n"
  "   n : integer;\n"
  "   column : integer;\n"
  "   value : integer;\n"
  "]\n"
  "GO\n"
  "\n"
  "CREATE OBJECT TYPE\n"
  "WITH SINGLE RANGE OBJECTS\n"
  "[HAL_space\n"
  "   n : integer;\n"
  "]\n"
  "GO\n"
  "\n";
