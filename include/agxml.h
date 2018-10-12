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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
