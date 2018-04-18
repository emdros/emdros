/*
 * emdfxml.cpp
 *
 * Utilities to read / write emdfxml.
 *
 * Ulrik Sandborg-Petersen
 * Created: 10/3-2017 (October 3, 2017)
 * Last update: 10/3-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2017  Ulrik Sandborg-Petersen
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


#ifndef EMDXML_H_
#define EMDXML_H_

#include <string>
#include <list>

#include <emdf_enums.h>
#include <infos.h>
#include <monads.h>
#include <mql_sheaf.h>

class EMdFXMLFeatureInfo {
 public:
	std::string m_feature_name;
	std::string m_feature_type;
	bool m_is_from_set;
	bool m_is_with_index;
	std::string m_default_value;
 public:
	EMdFXMLFeatureInfo(const std::string& feature_name,
			   const std::string& feature_type,
			   bool is_from_set,
			   bool is_with_index,
			   const std::string& default_value);
};

class EMdFXMLEventHandler {
 public:
	EMdFXMLEventHandler() = 0;
	virtual ~EMdFXMLEventHandler() = 0;

	virtual void onStartDatabase(const std::string& name) = 0;
	virtual void onEndDatabase(const std::string& name) = 0;

	virtual void onStartSchema() = 0;
	virtual void onEndSchema() = 0;

	virtual void onStartMonadSets() = 0;
	virtual void onDeclareMonadSet(const std::string& name, const SetOfMonads& monads)
	virtual void onEndMonadSets() = 0;

	virtual void onStartEnumerations() = 0;
	virtual void onDeclareEnumeration(const std::string& name, const std::list<EnumConstInfo>& enum_consts) = 0;
	virtual void onEndEnumerations() = 0;

	virtual void onStartObjectTypeDeclarations() = 0;
	virtual void onDeclareObjectType(const std::string& object_type_name,
					 eObjectRangeType range_type,
					 eMonadUniquenessType uniqueness_type,
					 const std::list<EMdFXMLFeatureInfo>& feature_declarations) = 0;
	virtual void onEndObjectTypeDeclarations() = 0;

	virtual void onStartData() = 0;
	virtual void onObjectTypeData(const std::string& object_type_name, FlatSheaf *pObjects) = 0;
	virtual void onEndData() = 0;
};

#ifndef SWIG
bool read_EMdFXML_fromStream(std::istream *pIn, EMdFXMLEventHandler *pEventHandler, std::string& /* out */ error_message);
#endif

bool read_EMdFXML_fromFile(const std::string& file_name, EMdFXMLEventHandler *pEventHandler, std::string& /* out */ error_message);


#endif /* !defined(EMDXML_H_) */
