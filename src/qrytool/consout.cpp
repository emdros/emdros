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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
