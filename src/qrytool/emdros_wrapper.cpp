/*
 * emdros_wrapper.cpp
 *
 * A wrapper around EmdrosEnv.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 5/1-2010
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


#include "emdros_wrapper.h"
#include "harvest.h"
#include "output_style.h"
#include "schema.h"
#include <emdf_output.h>
#include <sstream>

EmdrosWrapper::EmdrosWrapper(EmdrosEnv *pEE, 
			     Configuration *pConf,
			     QTOutputBase *pOut) 
{
	m_pEE = pEE;
	m_pConf = pConf;
	m_pOut = pOut;
	m_pSchema = 0;
}

EmdrosWrapper::~EmdrosWrapper()
{ 
	// You don't own the other members!
	delete m_pSchema; 
}

bool EmdrosWrapper::configurePOut(std::string& message)
{
	if (m_pEE != 0
	    && m_pConf != 0) {
		std::ostringstream ostrStream;
		if (!retrieveSchema(&ostrStream)) {
			message = ostrStream.str();
			return false;
		} else {
			return m_pOut->getConfigurationValues(m_pConf, m_pSchema, message);
		}
	} else {
		message = "EmdrosWrapper not configured. This is an internal error.";
		return false;
	}
}


bool EmdrosWrapper::executeString(const std::string& query) 
{
	bool bNoCompilerError = true;
	bool bNoDBError = m_pEE->executeString(query, bNoCompilerError, false, true);
	return bNoCompilerError && bNoDBError;
}


void EmdrosWrapper::show_result() 
{
	if (m_pEE->isTable()) {
		show_table(m_pEE->takeOverTable());
	} else if (m_pEE->isSheaf()) {
		show_sheaf(m_pEE->takeOverSheaf());
	} else {
		(*m_pOut) << "Query did not yield any results.";
		m_pOut->endl();
	}

	// Make sure that the results are displayed,
	// even if the "quiet" option is on.
	m_pOut->flush();
}

void EmdrosWrapper::show_table(Table *pTable) 
{
	std::ostringstream ostr;
	EMdFOutput emdf_output(kCSISO_8859_1, &ostr, kOKConsole);
	pTable->printConsole(&emdf_output);
	delete pTable;
	(*m_pOut) << ostr.str();
	printIfNotQuiet("\nDone printing table.");
}

void EmdrosWrapper::show_sheaf(Sheaf *pSheaf) 
{
	std::string output_style = *(m_pConf->getValues("output_style").begin());
	if (output_style == "xml") {
		(*m_pOut) << "<?xml version='1.0' encoding='utf-8' standalone='yes'?>\n";
		(*m_pOut) << "<!DOCTYPE solutions [\n"
			  << "<!-- solutions is the root element, containing all solutions, and having\n     an attribute 'count' showing number of solutions. -->\n"
			  << "<!ELEMENT solutions (solution)* >\n"
			  << "<!ATTLIST solutions\n"
			  << "          count CDATA #REQUIRED >\n"
			  << "<!-- A solution contains zero or more raster_unit elements. Please ignore any #PCDATA here. The number of the solution in the count of solutions is given in the 'number' attribute. -->\n"
			  << "<!ELEMENT solution (#PCDATA|raster_unit)* >\n"
			  << "<!ATTLIST solution\n"
			  << "          number CDATA #REQUIRED >\n"
			  << "<!-- A raster_unit element contains the units contained in a single raster unit.\n      Please ignore the #PCDATA here. -->\n"
			  << "<!ELEMENT raster_unit (#PCDATA|raster_unit_row)* >\n"
			  << "<!-- A raster_unit_row element indicates the beginning and end of a raster unit row.\n      More than these may occur in a raster_unit when two or more raster units\n      are necessary for a single solution. -->\n"
			  << "<!ELEMENT raster_unit_row (#PCDATA|focus|unit_start|unit_end|f)* >\n"
			  << "<!-- The reference attribute contains the reference. This may, in some cases, be empty. -->\n"
			  << "<!ATTLIST raster_unit_row\n"
			  << "          reference CDATA #REQUIRED >\n"
			  << "<!-- A focus element contains one or more units that need to be in focus.\n      Please ignore the #PCDATA here. -->\n"
			  << "<!ELEMENT focus (#PCDATA|unit_start|unit_end|f)* >\n"
			  << "<!-- The unit_start element tells in the 'object_type_name' which unit starts here. Only the \"left\" boundary is given, not any inside boundaries.\n      Note that units may not nest, but overlap, which is why we use\n      unit_start and unit_end rather than a single unit element. -->\n"
			  << "<!ELEMENT unit_start EMPTY >\n"
			  << "<!ATTLIST unit_start\n"
			  << "          object_type_name CDATA #REQUIRED >\n"
			  << "<!-- The unit_end element tells in the 'object_type_name' which unit ends here. Only the \"right\" boundary is given, not any inside boundaries. -->\n"
			  << "<!ELEMENT unit_end EMPTY >\n"
			  << "<!ATTLIST unit_end\n"
			  << "          object_type_name CDATA #REQUIRED >\n"
			  << "<!-- The f (feature) element contains (in the #PCDATA) the value of the feature.\nThe attribute 'feature_name' gives the name of the feature, and the attribute\n'unit_name gives the name of the unit to which the feature belongs.\nThis belongs to the object that was most recently started with unit_start\nand having the same unit_name (object_type_name). -->\n"
			  << "<!ELEMENT f (#PCDATA)* >\n"
			  << "<!ATTLIST f\n"
			  << "          feature_name CDATA #REQUIRED\n"
			  << "          unit_name CDATA #REQUIRED >\n"
			  << "]>\n";
	}
	if (pSheaf->isFail()) {
		if (output_style == "xml") {
			(*m_pOut) << "<solutions count=\"0\"/>";
		} else {
			(*m_pOut) << "No solutions found.";
			m_pOut->endl();
		}
	} else {
		printIfNotQuiet("Now harvesting results...");
		Harvest h(this, pSheaf);
		printIfNotQuiet("... done harvesting!");
		if (!m_pOut->ShouldStop()) {
			m_pEE->clean();
			if (output_style == "output") {
				Output_style outstyle(m_pConf, m_pOut);
				printIfNotQuiet("Number of solutions: " + long2string(h.size()));
				unsigned int i = 0;
				SolutionListConstIterator ci = h.const_iterator();
				while (ci.hasNext() && !m_pOut->ShouldStop()) {
					const Solution *pSolution = ci.next();
					outstyle.show(pSolution, i+1);
					++i;
				}
			} else if (output_style == "kwic") {
				KWIC_Output_style outstyle(m_pConf, m_pOut);
				printIfNotQuiet("Number of solutions: " + long2string(h.size()));
				m_pOut->startTable("kwic");
				unsigned int i = 0;
				SolutionListConstIterator ci = h.const_iterator();
				while (ci.hasNext() && !m_pOut->ShouldStop()) {
					const Solution *pSolution = ci.next();
					outstyle.show(pSolution, i+1);
					++i;
				}
				m_pOut->endTable("kwic");
			} else if (output_style == "xml") {
				XML_Output_style outstyle(m_pConf, m_pOut);
				m_pOut->print("<solutions count=\"" + long2string(h.size()) + "\">\n");
				unsigned int i = 0;
				SolutionListConstIterator ci = h.const_iterator();
				while (ci.hasNext() && !m_pOut->ShouldStop()) {
					const Solution *pSolution = ci.next();
					outstyle.show(pSolution, i+1);
					++i;
				}
				m_pOut->print("\n</solutions>");
			} else if (output_style == "tree") {
				Tree_Output_style outstyle(m_pConf, m_pOut);
				printIfNotQuiet("Number of solutions: " + long2string(h.size()));
				unsigned int i = 0;
				SolutionListConstIterator ci = h.const_iterator();
				while (ci.hasNext() && !m_pOut->ShouldStop()) {
					const Solution *pSolution = ci.next();
					outstyle.show(pSolution, i+1);
					++i;
				}
			} else {
				(*m_pOut) << "Unknown output_style: '" + output_style + "' in configuration file.";
				m_pOut->newline();
				(*m_pOut) << "The only valid option at the moment is \"output\" (without the quotes).";
				m_pOut->endl();
			}
			printIfNotQuiet("\nDone printing sheaf.");
		}
	}
	delete pSheaf;
}


void EmdrosWrapper::printIfNotQuiet(const std::string& toPrint)
{
	std::string output_style = *(m_pConf->getValues("output_style").begin());
	if (!m_pConf->has("option", "quiet")
	    && output_style != "xml") {
		if (m_pOut != 0) {
			m_pOut->startLatinParagraph();
			(*m_pOut) << toPrint;
			m_pOut->endl();
		}
	}
}


bool EmdrosWrapper::retrieveSchema(std::ostream *pOut)
{
	if (!m_pEE->connectionOk()) {
		(*pOut) << "Connection to backend not OK.";
		return false;
	} else {
		delete m_pSchema;
		m_pSchema = new EmdrosSchema(this->m_pEE);
		if (!m_pSchema->populate(pOut)) {
			delete m_pSchema;
			m_pSchema = 0;
			return false; 
		} else {
			return true;
		}
	}
}
