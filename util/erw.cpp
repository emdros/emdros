/*
 * erw.cpp
 *
 * A class providing a RESTful interface to Emdros.
 *
 * Claus Tøndering and Ulrik Sandborg-Petersen
 * Created: 1/14-2016
 * Last update: 11/4-2023
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


#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <erw.h>
#include <arena.h>
#include <emdros_environment.h>



using std::ifstream;
using std::cerr;
using std::cout;
using std::string;
using std::endl;
using std::map;
using std::ostringstream;


/// Tests if the Erw object has been properly initialized.
///
/// @return True if the object has been properly initalized.
Erw::operator bool() const
{
    return m_pConf!=0;
}


/// Constructor.
///
/// @param pathname Pathname of configuration file.
Erw::Erw(const std::string& pathname)
	: m_pConf(0)
{
    m_pConf = parse_configINI_file(pathname, 0);
}

/// Escapes slases and quotation marks in a JSON string.
/// Although not required by JSON, it is common to replace / by \/ in JSON strings.
///
/// @param s Source string.
///
/// @return String with escaped characters.
string Erw::escapeForJson(const string& s) const
{
    string s2;
    for (string::const_iterator it=s.begin(); it!=s.end(); ++it) {
        if (*it=='/' || *it=='"')
            s2.push_back('\\');
        s2.push_back(*it);
    }
    return s2;
}


/// Fetches a configuration value or a default value.
///
/// @param section Configuration section name.
/// @param key Configuration key.
/// @param defaultValue Value to use in the absense of a configuration value
///
/// @return Configuration value if one exists, otherwise defaultValue.
string Erw::getValueDefault(const std::string& section, const std::string& key, const std::string& defaultValue) const
{
    string res;
    return m_pConf->getValue(section, key, res) ? res : defaultValue;
}
    

/// Enumerates configured databases as a JSON string.
///
/// @return Database enumeration as a JSON string.
string Erw::enumJson() const
{
    StringVec dbs;

    StringVec sv = m_pConf->getSections();
 
    for (StringVec::iterator si = sv.begin(); si!=sv.end(); ++si) {
        StringVec parms;

        parms.push_back("\"pathname\": \"" + escapeForJson(*si) + "\"");
        

        StringVec sk = m_pConf->getKeysInSection(*si);
 
        for (StringVec::iterator si2 = sk.begin(); si2!=sk.end(); ++si2) {
            string value;
            m_pConf->getValue(*si,*si2,value);
            parms.push_back("\"" + *si2 + "\": \"" + escapeForJson(value) + "\"");
        }
        
        dbs.push_back(joinVector(",\n       ", parms));
    }
    return "{ \"erw_databases\":\n   [ { " + joinVector(" },\n     { ",dbs) + " } ] }";
}    


/// Executes an MQL request or a database enumeration request.
///
/// @param pathname Identifies a database in the configuration file.
/// @param verb The HTTP verb, "GET" or "POST".
/// @param params HTTP GET or POST parameters.
/// @param reply The resulting string will be stored here.
///
/// @return Error code.
int Erw::request(const string& pathname,
                 const string& verb, 
                 const ERWParamsString2StringMap& params,
                 string& reply) const
{
    reply = "";

    if (!*this)   // Uses operator bool()
        return ERW_NOT_INITIALIZED;

    if (!m_pConf->hasSection(pathname))
        return ERW_UNKNOWN_PATHNAME;

    string verb_config = getValueDefault(pathname, "verb", "GET-POST");

    // Is this an enumeration request?
    if (m_pConf->hasValueInSection(pathname, "is_enumerator", "true")) {
        // Yes, it is

        // Sanity check:
        if (verb_config != "GET-POST")
            return ERW_BAD_CONFIG;

        if (verb!="GET" && verb!="POST")
            return ERW_ILLEGAL_VERB;

        map<string,string>::const_iterator it = params.find("Accept");
        if (it==params.end() || it->second!="application/json")
            return ERW_BAD_PARAM;

        reply = enumJson();

        return ERW_OK;
    }


    // Handle ordinary MQL request

    // Verify "Accept" parameter

    map<string,string>::const_iterator accept_iter = params.find("Accept");
    if (accept_iter==params.end())
        return ERW_BAD_PARAM;

    bool use_json;

    if (accept_iter->second=="application/json")
        use_json = true;
    else if (accept_iter->second=="application/xml")
        use_json = false;
    else
        return ERW_BAD_PARAM;
    

    // Verify "mql" parameter

    map<string,string>::const_iterator mql_iter = params.find("mql");
    
    if (mql_iter==params.end())
        return ERW_BAD_PARAM;


    // Verify "verb" parameter

    if (verb_config!="GET" && verb_config!="POST" && verb_config!="GET-POST")
        return ERW_BAD_CONFIG;

    if (verb=="GET") {
        if (verb_config!="GET" && verb_config!="GET-POST")
            return ERW_ILLEGAL_VERB;
    }
    else if (verb=="POST") {
        if (verb_config!="GET" && verb_config!="GET-POST")
            return ERW_ILLEGAL_VERB;
    }
    else
        return ERW_ILLEGAL_VERB;
        

    // Extract Emdros parameters

    bool read_only = getValueDefault(pathname, "access", "read") != "modify";
    string db_host = getValueDefault(pathname, "db_host", "localhost");
    string db_user = getValueDefault(pathname, "db_user", "");
    string db_pass = getValueDefault(pathname, "db_pass", "");
    string db_name, db_backend;

    if (!m_pConf->getValue(pathname, "db_name", db_name)
        || !m_pConf->getValue(pathname, "db_backend", db_backend))
        return ERW_BAD_CONFIG;

    eBackendKind backend;
    if (!string2backend_kind(db_backend, backend))
        return ERW_BAD_CONFIG;

    UNUSED(read_only); // FIXME: Use it!
    

    ostringstream os; // Captures MQL result

    // TODO: Use read_only

    EmdrosEnv EE(&os,
                 use_json ? kOKJSON : kOKXML,
                 kCSUTF8,
                 db_host,
                 db_user,
                 db_pass,
                 db_name,
                 backend);

    bool bResult = false;
    int err;

    if (!EE.executeString(mql_iter->second, bResult, true, true))
        err = ERW_DB_ERROR;
    else if (!bResult)
        err = ERW_MQL_ERROR;
    else
        err = ERW_OK;

    reply = os.str();

    return err;
}
