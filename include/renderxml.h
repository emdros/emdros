/*
 * renderxml.h
 *
 * Methods and classes to render XML into something else,
 * based on a JSON template.
 *
 * Ulrik Sandborg-Petersen
 * Created: 22/4-2007
 * Last update: 4/11-2017
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


#ifndef XML2HTML__H__
#define XML2HTML__H__

#include <string>
#include "json_classes.h"
#include "templatelang_classes.h"

extern std::string RenderXML(const std::string& xml, const JSONValue *pTopStyleSheet, const std::string& render_stylesheet, bool& bProcessOK, std::string& strError);

#ifndef SWIG
extern std::string RenderXML(const std::string& xml, const JSONValue *pTopStyleSheet, const std::string& render_stylesheet, const TemplateLangVariableMap& variables, bool& bProcessOK, std::string& strError);
#endif

extern std::string utf8ToEntity(const std::string& inHTML);

#endif
