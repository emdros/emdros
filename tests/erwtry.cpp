/*
 * erwtry.cpp
 *
 * Testing the Erw class of Emdros
 *
 * Claus Tøndering
 * Created: 4/18-2018
 * Last update: 1/19-2023
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
#include <cstdio>
#include <map>
#include <erw.h>
#include <arena.h>
#include <emdros_environment.h>
#include <opt.h>
#include <unistd.h>

using std::ostream;
using std::ofstream;
using std::endl;
using std::cout;
using std::cerr;
using std::string;
using std::list;
using std::tmpnam;
using std::map;

void show_version(ostream& ostr)
{
	ostr << "erwtry from Emdros version " << EMDROS_VERSION << endl;
}


void print_usage(ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:\n";
	ostr << "   erwtry [options] \n";
	ostr << "   --help               Show this help" << endl;
	ostr << "   -V , --version       Show version" << endl;
}

void show_DB_error(EmdrosEnv& env) {
	cerr << "DB Error:" << endl;
	string strDBError;
	strDBError = env.getDBError();
	cerr << strDBError << endl;
}


bool execute_query(EmdrosEnv& env, 
                   const string& query, 
                   bool showResult)
{
	// Show query
	if (showResult) {
		cout << "Executing query: '" + query + "'" << endl;
	}

  
	// Execute query
	bool bCompilerResult;
	bool bDBResult;
	try {
		bDBResult = env.executeString(query, bCompilerResult, showResult, false);
	} catch (EMdFDBDBError& e) {
		cerr << e.what() << endl;
		show_DB_error(env);
		return false;
	} catch (EMdFDBException& e) {
		cerr << e.what() << endl;
		show_DB_error(env);
		return false;
	} catch (TableColumnException& e) {
		cerr << e.what() << endl;
		show_DB_error(env);
		return false;
	} catch (TableRowException& e) {
		cerr << e.what() << endl;
		show_DB_error(env);
		return false;
	} catch (TableException& e) {
		cerr << e.what() << endl;
		show_DB_error(env);
		return false;
	} catch (WrongCharacterSetException& e) {
		cerr << e.what() << endl;
		show_DB_error(env);
		return false;
	} catch (EMdFOutputException& e) {
		cerr << e.what() << endl;
		show_DB_error(env);
		return false;
	} catch (BadMonadsException& e) {
		cerr << e.what() << endl;
		show_DB_error(env);
		return false;
	}
  
	// Show success-info
	if (showResult) {
		cout << "DB sucess: " << bDBResult << ", Compiler success: " << bCompilerResult << endl;
	}
  
	// Show DB error, if any
	if (!bDBResult) {
		show_DB_error(env);
	}

	// Show compiler error, if any
	if (!bCompilerResult) {
		cerr << "Compiler error:" << endl;
		string strCompilerError;
		strCompilerError = env.getCompilerError();
		cerr << strCompilerError << endl;
		cerr << "Query was:'" << endl;
		cerr << query + "'" << endl;
	}
  
	// Return success-info
	return !(!bCompilerResult || !bDBResult);
}

bool execute_query(EmdrosEnv& env, const string& query, string success, bool showResult)
{
	bool bResult = execute_query(env, query, showResult);

	if (bResult)
		cout << "SUCCESS in " + success << endl;
	else
		cerr << "FAILURE in " + success << endl;

	return bResult;
}



bool create_database(EmdrosEnv& env)
{
	return
        execute_query(env, "CREATE DATABASE erwtry_emdros GO", "creating database", false)
        && execute_query(env, "CREATE OBJECT TYPE [word surface : ASCII; ] GO", "creating object type", false)
        && execute_query(env, "CREATE OBJECT FROM MONADS={ 1 } [word surface:=\"xyzzy\"; ] GO", "creating object", false);
}


bool drop_database(EmdrosEnv& env)
{
	return execute_query(env, "DROP DATABASE erwtry_emdros GO", "dropping database", false);
}

string config_content_text = "[/enum]\n"
                             "	  verb = GET-POST\n"
							 "	  description = Enumerates the available databases\n"
							 "	  is_enumerator = true\n"
							 "[/tester]\n"
							 "	  verb = GET\n"
							 "	  access = read\n"
							 "	  description = The test database\n"
							 "	  db_host = localhost\n"
							 "	  db_name = erwtry_emdros\n"
							 "	  db_backend = sqlite3\n";

string config_content_json = "{ \"erw_databases\":\n"
                             "   [ { \"pathname\": \"\\/enum\",\n"
                             "       \"description\": \"Enumerates the available databases\",\n"
                             "       \"is_enumerator\": \"true\",\n"
                             "       \"verb\": \"GET-POST\" },\n"
                             "     { \"pathname\": \"\\/tester\",\n"
                             "       \"access\": \"read\",\n"
                             "       \"db_backend\": \"sqlite3\",\n"
                             "       \"db_host\": \"localhost\",\n"
                             "       \"db_name\": \"erwtry_emdros\",\n"
                             "       \"description\": \"The test database\",\n"
                             "       \"verb\": \"GET\" } ] }";

string select_result_json = "{\n"
                            "   \"mql_results\": [\n"
                            "      {\n"
                            "         \"status_success\": true,\n"
                            "         \"sheaf\": {\n"
                            "            \"straws\": [\n"
                            "               {\n"
                            "                  \"matched_objects\": [\n"
                            "                     {\n"
                            "                        \"object_type_name\": \"word\",\n"
                            "                        \"focus\": false,\n"
                            "                        \"id_d\": 1,\n"
                            "                        \"monad_set\": [\n"
                            "                           {\n"
                            "                              \"first\": 1,\n"
                            "                              \"last\": 1\n"
                            "                           }\n"
                            "                        ],\n"
                            "                        \"features\": [\n"
                            "                           {\n"
                            "                              \"feature_name\": \"surface\",\n"
                            "                              \"feature_type\": \"string\",\n"
                            "                              \"feature_value\": \"xyzzy\"\n"
                            "                           }\n"
                            "                        ]\n"
                            "                     }\n"
                            "                  ]\n"
                            "               }\n"
                            "            ]\n"
                            "         }\n"
                            "      }\n"
                            "   ]\n"
                            "}\n";



bool create_config_file(string &name)
{
    name = tmpnam(0);

    ofstream ofile(name.c_str());

    if (!ofile) {
		cerr << "FAILURE creating configuration file." << endl;
        return false;
    }

    ofile << config_content_text;

    if (!ofile) {
		cerr << "FAILURE writing to configuration file." << endl;
        return false;
    }

    return true;
}

bool test_erw_class()
{
	bool bContinue = true;

	// Make execution environment
	EmdrosEnv env(kOKConsole,
                  kCSUTF8,
                  "localhost",
                  "", "",
                  "",
                  kSQLite3);
  
	bool bConnectionOk = env.connectionOk();
	if (!bConnectionOk) {
		show_DB_error(env);
		cerr << "Connection could not be established." << endl;
		return 1;
	}
	cerr << "Connection established." << endl;

    if (!create_database(env))
        return false;

    string config_file;
    bContinue = create_config_file(config_file);

    if (bContinue) {
        Erw erw(config_file);

        bContinue = erw;

        if (!bContinue)
            cerr << "FAILURE creating Erw object." << endl;

        string reply;
        if (bContinue) {
            map<string, string> m;

            m["Accept"] = "application/json";

            bContinue = erw.request("/enum", "GET", m, reply)==ERW_OK;

            if (!bContinue)
                cerr << "FAILURE executing Erw enumeration request." << endl;
        }
            
        if (bContinue) {
            bContinue = reply == config_content_json;

            if (!bContinue)
                cerr << "FAILURE in enumeration reply." << endl;
        }

        if (bContinue) {
            map<string, string> m;

            m["Accept"] = "application/json";
            m["mql"] = "SELECT ALL OBJECTS WHERE [word GET surface]";

            bContinue = erw.request("/tester","GET", m, reply)==ERW_OK;
            if (!bContinue)
                cerr << "FAILURE executing SELECT request." << endl;
        }


        if (bContinue) {
            bContinue = reply == select_result_json;

            if (!bContinue)
                cerr << "FAILURE in SELECT reply." << endl;
        }


    }

    unlink(config_file.c_str());
    drop_database(env);

    return bContinue;
}


int test_erw_library()
{
	cout << "----------------------------------\n"
		  << "Testing erw library\n"
		  << "----------------------------------" << endl;

	int result = 0;
	bool bContinue = true;
	bool bAllOK = true;

	// Test ConfigurationINI.
	if (bContinue) {
		bContinue = test_erw_class();
		

		if (bContinue) {
			cout << "SUCCESS: All tested Erw methods passed.\n";
		} else {
			bAllOK = false;
		}
	}
	

	if (bAllOK) {
		cout << "SUCCESS: All tests completed successfully on the Emdros erw library." << endl;
	} else {
		cout << "FAILURE: At least one test failed on the Emdros erw library." << endl;
		result = 1;
	}

	return result;
}


int main(int argc, char* argv[])
{
	int result = 0;

	addOption("--help", "--help");
	addOption("-V", "--version");

	// Parse arguments
	list<string> surplus_arguments;

	string error_message;
	if (!parseArguments(argc, argv, error_message, surplus_arguments)) {
		print_usage(cerr);
		cerr << error_message << endl;
		return 1;
	} else {
		bool bShowVersion;
		bool bShowHelp;
		bShowVersion = getArgumentPresent("-V");
		bShowHelp = getArgumentPresent("--help");
	
		// -V | --version
		if (bShowVersion) {
			show_version(cout);
			return 0;
		}

		// --help
		if (bShowHelp) {
			print_usage(cout);
			return 0;
		}

		if (surplus_arguments.size() != 0) {
			print_usage(cout);
			return 0;
		}
	}


	result = test_erw_library();
	

	if (result == 0) {
		cout << "SUCCESS: All tests completed successfully.\n";
	} else {
		cerr << "FAILURE: At least one test failed." << endl;
	}

	return result;
}


