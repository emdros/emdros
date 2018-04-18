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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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
