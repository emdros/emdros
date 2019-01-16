/*
 * qthtmloutput.cpp
 *
 * Classes to do output from the query tool in HTML.
 *
 * Ulrik Petersen
 * Created: 5/15-2005
 * Last update: 1/16-2019
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


#include "qthtmloutput.h"

QTHTMLOutput::QTHTMLOutput(std::ostream* pOut, bool bDeleteOut,
			   bool bDoHTMLPrePostAmble, bool bEmitStylesheet)
	: m_pOut(pOut),
	  m_bDeleteOut(pOut),
	  m_bDoHTMLPrePostAmble(bDoHTMLPrePostAmble),
	  m_bEmitStylesheet(bEmitStylesheet),
	  m_bInTable(false)
{
}


QTHTMLOutput::~QTHTMLOutput()
{
}


void QTHTMLOutput::print(const std::string& toPrint)
{
	(*m_pOut) << replace_substring(toPrint, "\n", "<br>\n");
}


void QTHTMLOutput::startUnit(const std::string& object_type_name)
{
	// TODO: Do something intelligent...
	/*
	StringStringMap::const_iterator ci = m_data_left_boundary_map.find(object_type_name);
	if (ci != m_data_left_boundary_map.end()) {
		print(ci->second);
	}
	*/
}



void QTHTMLOutput::endUnit(const std::string& object_type_name)
{
	// TODO: Do something intelligent
	/*
	StringStringMap::const_iterator ci = m_data_right_boundary_map.find(object_type_name);
	if (ci != m_data_right_boundary_map.end()) {
		print(ci->second);
	}
	*/
}


void QTHTMLOutput::endSolution(void)
{
	newline();
}


void QTHTMLOutput::startRaster(void)
{
	if (m_bInTable) {
		; // Do nothing
	} else {
		if (m_state != ST_RASTER) {
			m_state = ST_RASTER;
			if (m_current_paragraph_state != ST_RASTER) {
				(*m_pOut) << "</p><p>";
				m_current_paragraph_state = ST_RASTER;
			}
		}
	}
}


void QTHTMLOutput::startLatin(void)
{
	m_state = ST_LATIN;
}

void QTHTMLOutput::startLatinParagraph(void)
{ 
	if (m_state != ST_LATIN) {
		m_state = ST_LATIN; 
		if (m_current_paragraph_state != ST_LATIN) {
			newline();
			(*m_pOut) << "<p class=\"latin\">";
			m_current_paragraph_state = ST_LATIN;
		}
	} 
}


void QTHTMLOutput::printFeature(const std::string& object_type_name,
				unsigned int feature_index,
				const std::string& feature_name,
				const std::string& feature_value)
{
	UNUSED(feature_index); // Used when we had applyTECkit()
	
	std::string new_feature_value = feature_value; // applyTECkit(object_type_name, feature_index, feature_name, feature_value);
	int index = getIndexOfFeature(object_type_name, feature_name);
	if (m_current_unit_strings[index].empty()) {
		m_current_unit_strings[index] = new_feature_value;
	
	} else {
		m_current_unit_strings[index] += ' ' + new_feature_value;
	}
}


void QTHTMLOutput::startFocus(void)
{
	// TODO: Implement
}


void QTHTMLOutput::endFocus(void)
{
	// TODO: Implement
}


void QTHTMLOutput::newline(void)
{
	if (m_state == ST_LATIN) {
		closeParagraph();
		openParagraph("latin");
		m_current_paragraph_state = ST_LATIN;
	} else {
		m_pCanvas->AddParagraph(m_bIsRightToLeft,
					false, // bIsCentered
					0,     // indent
					false); // AddVerticalSpaceAfter
		m_current_paragraph_state = ST_RASTER;
	}
}


void QTHTMLOutput::endl(void)
{
	// TODO: Implement
}


void QTHTMLOutput::flush(void)
{
	(*m_pOut) << std::flush;
}



void QTHTMLOutput::startTable(const std::string& CSS_class)
{
	(*m_pOut) << ("<table class=\"" + CSS_class + "\">\n");
	m_bInTable = true;
}


void QTHTMLOutput::endTable(const std::string& CSS_class)
{
	(*m_pOut) << ("</table>\n");
	m_bInTable = false;
}


void QTHTMLOutput::startTableRow(const std::string& CSS_class)
{
	(*m_pOut) << ("<tr class=\"" + CSS_class + "\">");
}


void QTHTMLOutput::endTableRow(const std::string& CSS_class)
{
	(*m_pOut) << ("</tr>\n");
}


void QTHTMLOutput::startTableCell(const std::string& CSS_class)
{
	(*m_pOut) << ("<td class=\"" + CSS_class + "\">");
}


void QTHTMLOutput::endTableCell(const std::string& CSS_class)
{
	(*m_pOut) << ("</td>");
}



void QTHTMLOutput::MyYield(void)
{
	// Does nothing. This is for the GUI stuff.
}


