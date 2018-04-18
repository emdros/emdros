/*
 * agxml.h
 *
 * Support for exporting to Annotation Graph XML.
 *
 * Ulrik Petersen
 * Created: 12/15-2005
 * Last update: 3/27-2006
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2006  Ulrik Petersen
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

#ifndef __AGXML__H__
#define __AGXML__H__

#include "mql_sheaf.h"
#include "schema.h"
#include "emdf_output.h"

/**@file agxml.h
 *@brief Header file for Annotation Graph XML support (util library).
 */


/** An exporter to Annotation Graph XML format.
 *\internal
 *
 * \section Overview
 *
 * This class is for exporting an Emdros database to Annotation Graph (AG)
 * XML format.  AG format is a standard XML format.
 *
 * See the manual (man) page for agexport(1) for more information on
 * the way the Emdros database is exported.
 *
 * You can get more information about the Annotation Graph format here:
 *
 * http://agtk.sourceforge.net/
 */
class AGXMLExporter {
protected:
	EmdrosEnv *m_pEE; /**< The EmdrosEnv holding the connection to the DB to dump. */
	EMdFOutput *m_pOut; /**< The EMdFOuptut on which to dump. */
	monad_m m_start; /**< The start monad from which to dump. */
	monad_m m_end; /**< The end monad up to which to dump. */
	EmdrosSchema *m_pSchema; /**< Internal schema variable to help in the process. */
public:
	AGXMLExporter(EmdrosEnv *pEE, EMdFOutput *pOut, monad_m start, monad_m end);
	~AGXMLExporter();
	bool doIt(std::string& message);
protected:
	void emitAGDTD();
	void emitObjectTypeAGXML(const FlatStraw *pStraw);
	bool exportDB();
	void emitObject(const MatchedObject *pMO, const std::string& object_type_name, const std::list<FeatureNameAndType>& features);
	void emitFeatureAGXML(const std::string& feature_name, 
					     const std::string& feature_value);
	void emitFeaturesAGXML(const MatchedObject *pMO, const std::list<FeatureNameAndType>& features, bool bSelfOnly);
	std::string getQuery(const std::string& OT_name);
};


#endif
