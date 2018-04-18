/*
 * agxml.cpp
 *
 * Support for exporting to Annotation Graph XML.
 *
 * Ulrik Petersen
 * Created: 12/15-2005
 * Last update: 3/9-2006
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

/**@file agxml.cpp
 *@brief Implementation of Annotation Graph XML support (util library)
 */


#include <agxml.h>
#include <string_func.h>
#include <sstream>
#include <emdf_value.h>

/** Constructor.
 *
 * @param pEE The EmdrosEnv from which to get the connection to the
 * database to be dumped.
 * 
 * @param pOut the EMdFOutput on which to dump the XML.
 *
 * @param start The first monad to dump.
 *
 * @param end The last monad to dump.
 */
AGXMLExporter::AGXMLExporter(EmdrosEnv *pEE, EMdFOutput *pOut, monad_m start, monad_m end)
	: m_pEE(pEE),
	  m_pOut(pOut),
	  m_start(start),
	  m_end(end),
	  m_pSchema(0)
{
}

/** Destructor
 */
AGXMLExporter::~AGXMLExporter()
{
	delete m_pSchema;
	// Don't delete anything else; you don't own it...
}

/** Emit Annotation Graph DTD on m_pOut.
 */
void AGXMLExporter::emitAGDTD()
{
	m_pOut->out("\n");
	m_pOut->printDTDstart("AGSet");
	m_pOut->out("<!ELEMENT Metadata ( MetadataElement* )>\n");
	m_pOut->out("\n");
	m_pOut->out("<!ELEMENT MetadataElement ANY>\n");
	m_pOut->out("<!ATTLIST MetadataElement\n");
	m_pOut->out("    name        CDATA  #REQUIRED\n");
	m_pOut->out(">\n");
	m_pOut->out("\n");
	m_pOut->out("<!ELEMENT AGSet\n");
	m_pOut->out("  ( Metadata, Timeline*, AG* )>\n");
	m_pOut->out("<!ATTLIST AGSet\n");
	m_pOut->out("    id          ID     #REQUIRED\n");
	m_pOut->out("    version     CDATA  #FIXED    \"1.0\"\n");
	m_pOut->out("    xmlns       CDATA  #FIXED    \"http://www.ldc.upenn.edu/atlas/ag/\"\n");
	m_pOut->out("    xmlns:xlink CDATA  #FIXED    \"http://www.w3.org/1999/xlink\"\n");
	m_pOut->out("    xmlns:dc    CDATA  #FIXED    \"http://purl.org/DC/documents/rec-dces-19990702.htm\"\n");
	m_pOut->out(">\n");
	m_pOut->out("\n");
	m_pOut->out("<!ELEMENT AG\n");
	m_pOut->out("  ( Metadata?, Anchor*, Annotation* )>\n");
	m_pOut->out("<!ATTLIST AG\n");
	m_pOut->out("    id          ID         #REQUIRED\n");
	m_pOut->out("    type        NMTOKEN    #IMPLIED\n");
	m_pOut->out("    timeline    IDREF      #IMPLIED\n");
	m_pOut->out(">\n");
	m_pOut->out("\n");
	m_pOut->out("<!ELEMENT Timeline\n");
	m_pOut->out("  ( Metadata?, Signal* )>\n");
	m_pOut->out("<!ATTLIST Timeline\n");
	m_pOut->out("    id          ID         #REQUIRED\n");
	m_pOut->out(">\n");
	m_pOut->out("\n");
	m_pOut->out("<!ELEMENT Signal ( Metadata? )>\n");
	m_pOut->out("<!ATTLIST Signal\n");
	m_pOut->out("    id          ID         #REQUIRED\n");
	m_pOut->out("    mimeClass   CDATA      #REQUIRED\n");
	m_pOut->out("    mimeType    CDATA      #REQUIRED\n");
	m_pOut->out("    encoding    CDATA      #REQUIRED\n");
	m_pOut->out("    unit        CDATA      #REQUIRED\n");
	m_pOut->out("    xlink:type  CDATA      #FIXED        \"simple\"\n");
	m_pOut->out("    xlink:href  CDATA      #REQUIRED\n");
	m_pOut->out("    track       NMTOKEN    #IMPLIED\n");
	m_pOut->out(">\n");
	m_pOut->out("\n");
	m_pOut->out("<!ELEMENT Annotation ( Feature* )>\n");
	m_pOut->out("<!ATTLIST Annotation\n");
	m_pOut->out("    id         ID          #REQUIRED\n");
	m_pOut->out("    type       NMTOKEN     #REQUIRED\n");
	m_pOut->out("    start      IDREF       #REQUIRED\n");
	m_pOut->out("    end        IDREF       #REQUIRED\n");
	m_pOut->out(">\n");
	m_pOut->out("\n");
	m_pOut->out("<!ELEMENT Feature (#PCDATA)>\n");
	m_pOut->out("<!ATTLIST Feature\n");
	m_pOut->out("    name        NMTOKEN    #REQUIRED\n");
	m_pOut->out(">\n");
	m_pOut->out("\n");
	m_pOut->out("<!--\n");
	m_pOut->out("  Note that feature content may be structured, and the recommended\n");
	m_pOut->out("  way is to use Dublin Core Structured Values DCSV (Cox & Iannella)\n");
	m_pOut->out("  http://purl.org/dc/documents/rec/dcmi-dcsv-20000728.htm\n");
	m_pOut->out("  e.g. \"name.given=Eric; name.family=Van Sandt; height=170 cm\"\n");
	m_pOut->out("-->\n");
	m_pOut->out("\n");
	m_pOut->out("<!ELEMENT Anchor EMPTY>\n");
	m_pOut->out("<!ATTLIST Anchor\n");
	m_pOut->out("    id          ID         #REQUIRED\n");
	m_pOut->out("    offset      CDATA      #IMPLIED\n");
	m_pOut->out("    unit        CDATA      #IMPLIED\n");
	m_pOut->out("    signals     IDREFS     #IMPLIED\n");
	m_pOut->out(">\n");
	m_pOut->out("\n");
	m_pOut->printDTDend();
}


/** Emit the AGXML for a whole ObjectType.
 *
 * @param pStraw the FlatStraw holding the objects to dump.
 */
void AGXMLExporter::emitObjectTypeAGXML(const FlatStraw *pStraw)
{
	FlatStrawConstIterator ci = pStraw->const_iterator();

	if (!ci.hasNext()) {
		// Nothing to do
		return;
	} else {
		// Get object type name
		std::string OT_name = pStraw->getObjectTypeName();
		std::cerr << "Now doing: " << OT_name << std::endl;

		// Emit <AG id="{OT_name}" type="object_type" timeline="E1">
		AttributePairList attlist;
		attlist.push_back(std::pair<std::string, std::string>("id", OT_name));
		attlist.push_back(std::pair<std::string, std::string>("type", "object_type"));
		attlist.push_back(std::pair<std::string, std::string>("timeline", "E1"));
		m_pOut->startTag("AG", attlist, true);
		attlist.clear();	
	
		// Get set of monads for this OT, as the big union of
		// all the first and last monads of the monad sets 
		// of all objects
		std::set<monad_m> som;
		while (ci.hasNext()) {
			MatchedObject *pMO = ci.next();
			SetOfMonads MOSOM = pMO->getMonads();
			SOMConstIterator somci = MOSOM.const_iterator();
			while (somci.hasNext()) {
				MonadSetElement mse = somci.next();
				som.insert(mse.first());
				som.insert(mse.last());
			}
		}

		// Emit all monads in som as an anchor
		for (std::set<monad_m>::const_iterator somci = som.begin();
		     somci != som.end();
		     ++somci) {
			monad_m m = *somci;
			std::ostringstream ostr;
			ostr << OT_name << "_" << m;
			attlist.push_back(std::pair<std::string, std::string>("id", ostr.str()));
			attlist.push_back(std::pair<std::string, std::string>("offset", monad_m2string(m)));
			attlist.push_back(std::pair<std::string, std::string>("unit", "monad"));
			m_pOut->startSingleTag("Anchor", attlist, true);
			attlist.clear();
		}

		// Get list of features, without "self"
		const EmdrosObjectTypes& object_types = m_pSchema->getObjectTypes();
		std::list<FeatureNameAndType> features 
			= object_types.getFeaturesOfObjectType(OT_name);
		for (std::list<FeatureNameAndType>::iterator it 
			     = features.begin();
		     it != features.end();
		     ++it) {
			if (strcmp_nocase(it->name, "self") == 0) {
				features.erase(it);
				break;
			}
		}

		// Emit all objects
		ci = pStraw->const_iterator();
		while (ci.hasNext()) {
			MatchedObject *pMO = ci.next();
			emitObject(pMO, OT_name, features);
		}


		// End tag
		m_pOut->endTag("AG", true);

	}
}

/** Construct a GET OBJECTS HAVING MONADS IN query from an Object Type name.
 *
 * @param OT_name the name of the Object Type.
 *
 * @return The string containing the query.
 */
std::string AGXMLExporter::getQuery(const std::string& OT_name)
{
	std::string result = "GET OBJECTS HAVING MONADS IN { "
		+ monad_m2string(m_start)
		+ '-'
		+ monad_m2string(m_end)
		+ "}\n[" + OT_name;
	
	// Get list of features, without "self"
	const EmdrosObjectTypes& object_types = m_pSchema->getObjectTypes();
	std::list<FeatureNameAndType> features
		= object_types.getFeaturesOfObjectType(OT_name);
	std::list<FeatureNameAndType>::iterator it;
	for (it = features.begin();
	     it != features.end();
	     ++it) {
		if (strcmp_nocase(it->name, "self") == 0) {
			features.erase(it);
			break;
		}
	}

	// Add them to query
	it = features.begin();
	if (it != features.end()) {
		result += " GET " + it->name;
		++it;
	}
	while (it != features.end()) {
		result += ", " + it->name;
		++it;
	}
	
	result += "\n]\nGO";

	return result;	
}


/** Perform the export.
 *
 * @return True on everything went OK. False on something went wrong.
 */
bool AGXMLExporter::exportDB()
{
	// Emit XML declaration
	m_pOut->printXMLDecl();

	// Emit the DTD...
	emitAGDTD();

	// Emit AGSet
	AttributePairList attlist;
	attlist.push_back(std::pair<std::string, std::string>("id", "EmdrosAGXML_Version_10"));
	attlist.push_back(std::pair<std::string, std::string>("version", "1.0"));
	attlist.push_back(std::pair<std::string, std::string>("xmlns", "http://www.ldc.upenn.edu/atlas/ag/"));
	attlist.push_back(std::pair<std::string, std::string>("xmlns:xlink", "http://www.w3.org/1999/xlink"));
	attlist.push_back(std::pair<std::string, std::string>("xmlns:dc", "http://purl.org/DC/documents/rec-dces-19990702.htm"));
	m_pOut->startTag("AGSet", attlist, false);
	attlist.clear();

	// Emit MetaData for AGSet
	m_pOut->startTag("Metadata", true);
	attlist.push_back(std::pair<std::string, std::string>("name", "format_version"));
	m_pOut->startTag("MetadataElement", attlist, true);
	attlist.clear();
	m_pOut->outCharData("1.0");
	m_pOut->endTag("MetadataElement");
	m_pOut->endTag("Metadata", true);

	// Emit Timeline with id="E1"
	attlist.push_back(std::pair<std::string, std::string>("id", "E1"));
	m_pOut->startSingleTag("Timeline", attlist, true);
	attlist.clear();

	bool bResult = true;

	const EmdrosObjectTypes& object_types = m_pSchema->getObjectTypes();
	if (!object_types.isEmpty()) {
		StringVec object_type_names = object_types.getObjectTypeNames();
		for (unsigned int index = 0;
		     index < object_type_names.size();
		     ++index) {
			std::string OT_name = object_type_names[index];

			std::string query = getQuery(OT_name);

			bool bCompilerResult;
			if (!m_pEE->executeString(query, bCompilerResult, false, false)) {
				bResult = false;
				break;
			} else {
				if (!bCompilerResult) {
					bResult = false;
					break;
				}
			}
			
			// Get flat sheaf
			const FlatSheaf* pFlatSheaf = m_pEE->takeOverFlatSheaf();
			// Emit object 
			FlatSheafConstIterator ci = pFlatSheaf->const_iterator();
			const FlatStraw* pFlatStraw = ci.next();
			emitObjectTypeAGXML(pFlatStraw);

			delete pFlatSheaf;
		}
	}

	// End </AGSet>
	m_pOut->endTag("AGSet", true);

	// Flush
	m_pOut->flush();

	return bResult;
}


/** Perform the export for one object.
 *
 * @param pMO The MatchedObject to export.
 *
 * @param object_type_name the name of the object type to which the
 * matched object belongs.
 *
 * @param features A std::list of FeatureNameAndType pairs, containing
 * the features to be dumped.
 */
void AGXMLExporter::emitObject(const MatchedObject *pMO, const std::string& object_type_name, const std::list<FeatureNameAndType>& features)
{
	SetOfMonads SOM = pMO->getMonads();
	
	SOMConstIterator ci = SOM.const_iterator();
	bool bIsFirst = true;
	while (ci.hasNext()) {
		MonadSetElement mse = ci.next();
		
		// Get ID as {OT_name}_{id_d}_{first_monad}
		std::string myid;
		{
			std::ostringstream ostr;
			ostr << object_type_name << "_" << pMO->getID_D() << "_" << mse.first();
			myid = ostr.str();
		}
	
		// Get start as {OT_name}_{first_monad}
		std::string mystart;
		{
			std::ostringstream ostr;
			ostr << object_type_name << "_" << mse.first();
			mystart = ostr.str();
		}
	
		// Get end as {OT_name}_{end_monad}
		std::string myend;
		{
			std::ostringstream ostr;
			ostr << object_type_name << "_" << mse.last();
			myend = ostr.str();
		}
	
		AttributePairList attlist;
		attlist.push_back(std::pair<std::string, std::string>("id", myid));
		attlist.push_back(std::pair<std::string, std::string>("type", "Object"));
		attlist.push_back(std::pair<std::string, std::string>("start", mystart));
		attlist.push_back(std::pair<std::string, std::string>("end", myend));
		m_pOut->startTag("Annotation", attlist, true);

		if (bIsFirst) {
			emitFeaturesAGXML(pMO, features, false);
		} else {
			emitFeaturesAGXML(pMO, features, true);		
		}
		
		bIsFirst = false;
	
		m_pOut->endTag("Annotation", true);
	}
}

/** Emit the Feature tag for a feature name / feature value pair.
 *
 * @param feature_name The name of the feature to emit.
 *
 * @param feature_value The value of the feature to emit.
 */
void AGXMLExporter::emitFeatureAGXML(const std::string& feature_name, 
				     const std::string& feature_value)
{
	std::string fname = feature_name;
	std::string::size_type i = fname.find_last_of(":");
	if (i != std::string::npos) {
		fname.replace(i, 1, "");
	}
	AttributePairList attlist;
	attlist.push_back(std::pair<std::string, std::string>("name", fname));
	m_pOut->startTag("Feature", attlist, true);
	m_pOut->outCharData(feature_value);
	m_pOut->endTag("Feature", false);
}

/** Emit all the features of a MatchedObject.
 *
 * @param pMO The MatchedObject to emit features from.
 *
 * @param features the name and type of the features to emit.
 *
 * @param bSelfOnly Must be true if we only wish to emit "self".
 */
void AGXMLExporter::emitFeaturesAGXML(const MatchedObject *pMO, const std::list<FeatureNameAndType>& features, bool bSelfOnly)
{
	emitFeatureAGXML("self", id_d2number_string(pMO->getID_D()));
	if (!bSelfOnly) {
		std::list<FeatureNameAndType>::const_iterator ci;
		int index = 0;
		for (ci = features.begin();
		     ci != features.end();
		     ++ci) {
			std::string value;
			const EMdFValue *pValue = pMO->getEMdFValue(index);
			if (pValue->getKind() == kEVEnum) {
				value = pMO->getEnumLabel(index);
			} else {
				value = pValue->toString();
			}

			emitFeatureAGXML(ci->name, value);

			++index;
		}
	}
}


/** Perform the export.
 *
 * @param message a string in which to return any error or warning message.
 *
 * @return True if everything went OK.  False if there was some
 * error. Consult the message parameter in that case.
 */
bool AGXMLExporter::doIt(std::string& message)
{
	// Make an EmdrosSchema
	std::ostringstream strout;
	m_pSchema = new EmdrosSchema(m_pEE);
	if (!m_pSchema->populate(&strout)) {
		message = strout.str();
		delete m_pSchema;
		m_pSchema = 0;
		return false;
	}

	if (!exportDB()) {
		return false;
	} else {
		return true;
	}
}
