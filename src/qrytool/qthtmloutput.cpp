/*
 * qthtmloutput.cpp
 *
 * Classes to do output from the query tool in HTML.
 *
 * Ulrik Petersen
 * Created: 5/15-2005
 * Last update: 3/24-2008
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2008  Ulrik Sandborg-Petersen
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
	std::string new_feature_value = applyTECkit(object_type_name, feature_index, feature_name, feature_value);
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


