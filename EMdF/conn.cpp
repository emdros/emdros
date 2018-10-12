/*
 * conn.cpp
 *
 * Database connection abstract base class.
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 3/1-2017
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


/**@file conn.cpp
 *@brief Implementation of EMdFConnection (EMdF layer)
 */


#include <conn.h>
#include <utils.h>
#include <sstream>
#include <debug.h>


EMdFConnection::EMdFConnection()
{
}

EMdFConnection::~EMdFConnection()
{
}


void EMdFConnection::appendLocalError(std::string error)
{
  m_local_errormessage += error;
}

void EMdFConnection::clearLocalError(void)
{
  m_local_errormessage = "";
}

std::string EMdFConnection::getLocalError(void) 
{ 
  return m_local_errormessage; 
}



// Must be overridden in derived class if used.
bool EMdFConnection::useDatabase(const std::string& database_name, const std::string& key)
{
	UNUSED(database_name);
	UNUSED(key);
	return false;
}

