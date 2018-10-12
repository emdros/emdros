/*
 * erw.h
 *
 * A class providing a RESTful interface to Emdros.
 *
 * Claus Tøndering and Ulrik Sandborg-Petersen
 * Created: 1/14-2016
 * Last update: 4/10-2017
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


#ifndef _ERW_H_
#define _ERW_H_

#include <string>
#include <map>
#include <memory>
#include <conf_ini.h>


#define ERW_OK                  0
#define ERW_NOT_INITIALIZED     1
#define ERW_MQL_ERROR           2
#define ERW_DB_ERROR            3
#define ERW_UNKNOWN_PATHNAME    4
#define ERW_ILLEGAL_VERB        5
#define ERW_BAD_PARAM           6
#define ERW_BAD_CONFIG          7

class Erw {
  public:
    // Constructor. Parameter is configuration file.
    Erw(const std::string& pathname = "/etc/emdros/erw.conf");

    // Destructor
    ~Erw() {
	    delete m_pConf;
    }

    // Executes MQL request
    int request(const std::string& pathname,
                const std::string& verb, 
                const std::map<std::string, std::string>& params,
                std::string& reply) const;

    // Returns true if this Erw object has been properly initialized
    operator bool() const;

  private:
    ConfigurationINI *m_pConf;  // The associated configuration data

    std::string escapeForJson(const std::string& s) const;
    std::string enumJson() const;
    std::string getValueDefault(const std::string& section, const std::string& key, const std::string& defaultValue) const;
};



#endif // _ERW_H_
