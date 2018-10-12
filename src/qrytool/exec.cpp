/*
 * exec.cpp
 *
 * Functions to execute stuff
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 5/11-2018
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

#include "exec.h"
#ifdef __DJGPP__
#include <mql_ex~1.h>
#include <mql_ex~2.h>
#include "emdros~1.h"
#else
#include <mql_execute.h>
#include "emdros_wrapper.h"
#endif
#include <fstream>


int exec_stream(std::istream *pStrin, EmdrosWrapper *pEW)
{
	int nResult = 0;
	EmdrosEnv *pEE = pEW->getPEE();
	QTOutputBase *pOut = pEW->getPOut();
	try {
		std::string query;
		bool bQuit = false;
		while (!bQuit &&
		       mqlGetNextQuery(pStrin, query)) {
			pEW->printIfNotQuiet("Now executing query...");
			if (pEW->executeString(query)) {
				if (!pEE->getMQLEE()->bQuit) {
					pEW->printIfNotQuiet("... done querying!");
					pEW->show_result();
				} else {
					pEW->printIfNotQuiet("... OK, quitting...");
					bQuit = true;
				}
			} else {
				pEW->printIfNotQuiet("ERROR: query could not be executed.");
				pEW->printIfNotQuiet(pEE->getDBError());
				pEW->printIfNotQuiet("");
				pEW->printIfNotQuiet(pEE->getCompilerError());
				pEW->printIfNotQuiet("");
			}
			pEE->clean();
		}
	} catch (EMdFDBException& e) {
		nResult = 3;
		(*pOut) << "ERROR: EMdFDBException (Database error)...";
		pOut->newline();
		(*pOut) << pEE->getDBError();
		pOut->newline();
		(*pOut) << pEE->getCompilerError();
		pOut->newline();
		(*pOut) << e.what();
		pOut->newline();
		(*pOut) << "Program aborted.";
		pOut->endl();
	} catch (BadMonadsException& e) {
		nResult = 3;
		(*pOut) << e.what();
		pOut->newline();
		(*pOut) << "Program aborted.";
		pOut->endl();
	} catch (WrongCharacterSetException& e) {
		nResult = 3;
		(*pOut) << e.what();
		pOut->newline();
		(*pOut) << "Program aborted.";
		pOut->endl();
	} catch (EMdFOutputException& e) {
		nResult = 3;
		(*pOut) << e.what();
		pOut->newline();
		(*pOut) << "Program aborted.";
		pOut->endl();
	} catch (EmdrosException& e) {
		nResult = 3;
		(*pOut) << e.what();
		pOut->newline();
		(*pOut) << "Program aborted.";
		pOut->endl();
	} catch (...) {
		(*pOut) << "Unknown exception caught. Program aborted.";
		pOut->endl();
	} 
	return nResult;
}

int exec_file(std::string filename, EmdrosWrapper *pEW)
{
	std::string strError;
	std::ifstream fin;
	int nResult;
	fin.open(filename.c_str());
	QTOutputBase *pOut = pEW->getPOut();
	if (!fin) {
		(*pOut) << "FAILURE: Could not open file " << filename;
		pOut->newline();
		(*pOut) << "for reading.  Action aborted.";
		pOut->endl();
		nResult = 4;
	}

	nResult = exec_stream(&fin, pEW);

	fin.close();

	return nResult;
}




int exec_cin(EmdrosWrapper *pEW)
{
	return exec_stream(&std::cin, pEW);
}


bool app_checkConfiguration(Configuration *pConf, std::string& error_msg)
{
	// Check for consistency
	StringVec keysToCheck;
	keysToCheck.push_back("data_unit");
	keysToCheck.push_back("data_feature");
	keysToCheck.push_back("output_style");
	std::ostringstream ssout;
	if (!pConf->check(keysToCheck, &ssout)) {
		error_msg = ssout.str()
			+ "Error: at least one key which needed to be defined\n"
			+ "was not defined in the configuration file.";
		return false;
	} else {
		if (pConf->has_key("reference_unit")) {
			if (!pConf->has_key("reference_feature")) {
				error_msg =  "Error: if you define reference_unit, you must also define at least one reference_feature.\n";
				return false;
			}
		}
		if (!pConf->has_key("raster_unit")) {
			if(!pConf->has_key("raster_context_before")
			   || !pConf->has_key("raster_context_after")) {
				error_msg +=  "Error: if you do not define raster_unit, you must define both\nraster_context_before and raster_context_after.\n";
				return false;
			} else {
				std::string raster_context_before = pConf->getValues("raster_context_before")[0];
				std::string raster_context_after  =  pConf->getValues("raster_context_after")[0];

				if (!string_is_number(raster_context_before)) {
					error_msg +=  "Error: raster_context_before must be an integer.";
					return false;
				}
				if (!string_is_number(raster_context_after)) {
					error_msg +=  "Error: raster_context_after must be an integer.";
					return false;
				}
			}
		}
	}
	return true;
}

