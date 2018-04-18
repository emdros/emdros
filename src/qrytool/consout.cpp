/*
 * consout.cpp
 *
 * Classes to do output from the query tool.
 *
 * Ulrik Petersen
 * Created: 4/16-2005
 * Last update: 3/1-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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

#include "consout.h"

ConsoleQTOutput::ConsoleQTOutput(std::ostream* pOut, bool bDeleteOut) 
  : m_pOut(pOut), m_bDeleteOut(bDeleteOut) 
{
}

ConsoleQTOutput::~ConsoleQTOutput() 
{ 
  if (m_bDeleteOut) { 
    delete m_pOut; 
  }
}

void ConsoleQTOutput::print(const std::string& toPrint)
{
  (*m_pOut) << toPrint;
}


void ConsoleQTOutput::startUnit(const std::string& object_type_name)
{
  StringStringMap::const_iterator ci = m_data_left_boundary_map.find(object_type_name);
  if (ci != m_data_left_boundary_map.end()) {
    print(ci->second);
  }
}

void ConsoleQTOutput::startUnitFeatures(const std::string& object_type_name)
{
	if (m_data_names.find(object_type_name) != m_data_names.end()) {
		std::list<std::string>::const_iterator ci = m_data_names[object_type_name].begin();
		std::list<std::string>::const_iterator cend = m_data_names[object_type_name].end();
		while (ci != cend) {
			print(*ci);
			print(" ");
			++ci;
		}
	}
}




void ConsoleQTOutput::endUnit(const std::string& object_type_name)
{
  StringStringMap::const_iterator ci = m_data_right_boundary_map.find(object_type_name);
  if (ci != m_data_right_boundary_map.end()) {
    print(ci->second);
  }
}


void ConsoleQTOutput::printFeature(const std::string& object_type_name,
				   unsigned int feature_index,
				   const std::string& feature_name,
				   const std::string& feature_value)
{
  // The naive implementation just prints the feature value
  // A more sophisticated version will change the font...
  std::string new_feature_value = applyTECkit(object_type_name, feature_index, feature_name, feature_value);
  print(new_feature_value + ' ');
}


void ConsoleQTOutput::newline(void)
{
  print("\n");
}

void ConsoleQTOutput::endl(void)
{
  (*m_pOut) << std::endl;
}

void ConsoleQTOutput::startFocus(void)
{
  (*m_pOut) << '{';
}

void ConsoleQTOutput::endFocus(void)
{
  (*m_pOut) << '}';
}

void ConsoleQTOutput::MyYield(void)
{
  // Does nothing: This is for the GUI stuff...
}

void ConsoleQTOutput::flush(void)
{
  (*m_pOut) << std::flush;
}

void ConsoleQTOutput::startTable(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	newline();
}


void ConsoleQTOutput::endTable(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	newline();
	flush();
}


void ConsoleQTOutput::startTableRow(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	// Nothing to do
}


void ConsoleQTOutput::endTableRow(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	newline();
}


void ConsoleQTOutput::startTableCell(const std::string& CSS_class)
{
	if (CSS_class == "keyword") {
		print("<<<");
	}
}


void ConsoleQTOutput::endTableCell(const std::string& CSS_class)
{
	if (CSS_class == "keyword") {
		print(">>> ");
	} else {
		print(" ");
	}
}
