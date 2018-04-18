/*
 * hal_build_database.h
 *
 * Functions for building a HAL db
 *
 * Created: Sometime in the first half of 2003
 * Last update: 11/3-2015
 *
 *******************************************************************/
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2015  Ulrik Sandborg-Petersen
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, license version 2.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 **************************************************************************/


#ifndef __HAL_BUILD_DB__H__
#define __HAL_BUILD_DB__H__

#include <string>

class EmdrosEnv; // Forward declaration

extern bool CreateDB(EmdrosEnv *pEEE, std::string dbname);
extern bool build_db(std::string inputfilename, std::string dbname, std::string wordlist_filename, bool bAddNumbers);

#endif // __HAL_BUILD_DB__H__
