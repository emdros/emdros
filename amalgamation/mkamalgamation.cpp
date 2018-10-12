/*
 * mkamalgamation.cpp
 *
 * A program to create the Emdros amalgamation.
 *
 * Created: 4/12-2016 
 * Last update: 5/11-2018
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Something went wrong.
 * 3 : An exception occurred
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




#include <emdros-config.h>
// Do this because emdros-config might #define malloc,
// in which case some systems will fail to use the real 
// malloc, thus causing build errors.
#undef malloc


#include <json_classes.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string_func.h>
#include <emdros-lconfig.h>

std::string get_boilerplate()
{
	return std::string("\n")
		+ "/************************************************************************\n"
		+ " *\n"
		+ " *   Emdros - the database engine for analyzed or annotated text\n"
		+ " *   Copyright (C) 1999-2016  Ulrik Sandborg-Petersen\n"
		+ " *\n"
		+ " ***********************************************************************/\n"
		+ "\n"
		+ "/****\n"
		+ " * _GNU_SOURCE MUST be defined before _any_ standard headers are included. \n"
		+ " * Hence, we define it at the very top.\n"
		+ " ****/\n"
		+ "#if defined(__GNUC__) && !defined(_GNU_SOURCE)\n"
		+ "# define _GNU_SOURCE\n"
		+ "#endif\n"
		+ "\n"
		+ "#ifndef HAVE_CONFIG_H\n"
		+ "#define HAVE_CONFIG_H  (1)\n"
		+ "#endif\n"
		+ "\n"
		+ "#include \"emdros-lconfig.h\"\n"
		+ "#include \"emdros-config.h\"\n"
		+ "\n";
}

bool get_lines_from_file(const std::string& filename, std::list<std::string>& lines_list)
{
	std::ifstream fin;
	fin.open(filename.c_str());
	if (!fin) {
		std::cerr << "FAILURE: Could not open file " << filename << " for reading.\nAction aborted.\n";

		return false;
	}
	std::string line;
	while (!fin.eof() && !fin.fail() && !fin.bad()) {
		std::getline(fin, line);
		lines_list.push_back(line + "\n");
	}
	
	if (!fin.eof()) {
		std::cerr << "FAILURE: Could not read contents of file " << filename << ".\n"
			  << "Action aborted.\n";

		return false;
	}

	return true;
}

std::list<std::string> get_stringlist_from_JSONValue_list(const JSONValue *pList)
{
	std::list<std::string> result;

	if (pList != 0) {
		const std::list<JSONValue*>& inner_list = pList->getList();
		std::list<JSONValue*>::const_iterator ci = inner_list.begin();
		std::list<JSONValue*>::const_iterator cend = inner_list.end();
		while (ci != cend) {
			const JSONValue *pInnerValue = *ci++;
			std::string inner = pInnerValue->getString();
			result.push_back(inner);
		}
	}		
	

	return result;
}

std::string get_header_filename_from_path(const std::string& header_path)
{
	std::list<std::string> path_arr;
			
	split_string(header_path, "/", path_arr);

	if (path_arr.empty()) {
		return "";
	} else {
		std::string header_filename = *path_arr.rbegin();
		return header_filename;
	}
}

bool get_include_path(const std::string& line, std::string& header_filename)
{
#define ST_NONE (0)
#define ST_HASH (1)
#define ST_INCLUDE (2)
#define ST_LT_OR_QUOTE (3)
#define ST_FILENAME (4)
#define ST_GT_OR_QUOTE (5)
	
	size_t nIndex = 0;
	size_t nLength = line.length();
	int state = ST_NONE;
	header_filename = "";
	while (nIndex < nLength) {
		char c = line[nIndex++];

		switch (state) {
		case ST_NONE:
			if (c == '#') {
				state = ST_HASH;
			} else if (c == ' ' || c == '\t') {
				; // Nothing to do
			} else {
				// Finish with failure
				nIndex = nLength;
			}
			break;
		case ST_HASH:
			if (c == ' ') {
				; // Nothing to do
			} else if (c == 'i') {
				if (line.substr(nIndex-1,7) == "include") {
					state = ST_INCLUDE;
					nIndex += 7-1;
				} else {
					// Finish with failure
					nIndex = nLength;
				}
			} else {
				// Finish with failure
				nIndex = nLength;
			}
			break;
		case ST_INCLUDE:
			if (c == '"' || c == '<') {
				state = ST_LT_OR_QUOTE;
			} else if (c == ' ') {
				; // Nothing to do
			} else {
				// Finish with failure
				nIndex = nLength;
			}
			break;
		case ST_LT_OR_QUOTE:
			header_filename += c;
			state = ST_FILENAME;
			break;
		case ST_FILENAME:
			if (c == '"' || c == '>') {
				state = ST_GT_OR_QUOTE;
			} else {
				header_filename += c;
			}
			break;
		case ST_GT_OR_QUOTE:
			// Finish with success
			nIndex = nLength;
			break;
		}
	}

	bool bResult = state == ST_GT_OR_QUOTE;

	return bResult;
}

void make_headers_available_from_list(const JSONValue *json_dict, const JSONValue *pSourceList, std::map<std::string, std::string>& result)
{
	const std::list<JSONValue*>& obj_list = pSourceList->getList();
	std::list<JSONValue*>::const_iterator obj_ci = obj_list.begin();
	std::list<JSONValue*>::const_iterator obj_cend = obj_list.end();
	while (obj_ci != obj_cend) {
		const JSONValue* pObj = *obj_ci++;

		const JSONValue *pHeaderFileList = pObj->getObjectValue("header_files");
		std::list<std::string> header_path_list = get_stringlist_from_JSONValue_list(pHeaderFileList);
		std::list<std::string>::const_iterator header_ci = header_path_list.begin();
		std::list<std::string>::const_iterator header_cend = header_path_list.end();
		while (header_ci != header_cend) {
			std::string header_path = *header_ci++;

			std::string header_filename = get_header_filename_from_path(header_path);

			if (!header_filename.empty()) {
				result.insert(std::make_pair(header_filename, header_path));
			}
		}
	}
	
	// Ignore these, and elide their inclusion.

	const JSONValue *pIgnoredHeaderList = json_dict->getObjectValue("ignored_header_files");
	std::list<std::string> ignored_header_list = get_stringlist_from_JSONValue_list(pIgnoredHeaderList);
	std::list<std::string>::const_iterator header_ci = ignored_header_list.begin();
	std::list<std::string>::const_iterator header_cend = ignored_header_list.end();
	while (header_ci != header_cend) {
		std::string header_filename = *header_ci++;

		result[header_filename] = "";
	}
}


std::map<std::string, std::string> make_headers_available(const JSONValue *json_dict)
{
	std::map<std::string, std::string> result;

	const JSONValue *pSourceList = json_dict->getObjectValue("source_files_list");
	if (pSourceList != 0) {
		make_headers_available_from_list(json_dict, pSourceList, result);
	}

	return result;
}

std::string get_JSON_string(const JSONValue *pDict, const std::string& key)
{
	const JSONValue *pValue = pDict->getObjectValue(key);
	if (pValue == 0) {
		return "";
	} else {
		return pValue->getString();
	}
}

std::string get_OS_dependent_path(const std::string input_path)
{
#ifdef WIN32
	std::string result = input_path;
	replace_chars(result, '/', '\\');
	return result;
#else
	// Linux, OSX, Android, etc. leave the path as-is.
	return input_path;
#endif
}

bool copy_file(std::ofstream& fout, const std::string& current_language, std::map<std::string, std::string>& headers_available, const std::string& source_filename)
{
	fout << "/**************** A copy of ";
	fout << source_filename;
	fout << " ****************/\n"; 

	if (current_language == "C") {
		fout << "\n#ifdef __cplusplus\n"
		     << "extern \"C\" {\n"
		     << "#endif\n\n";
	}

	fout << "#line 1 \"" << source_filename << "\"\n";

	std::string path = get_OS_dependent_path("../" + source_filename);

	std::list<std::string> lines_list;
	if (!get_lines_from_file(path, lines_list)) {
		fout.close();
		return false;
	}

	bool bResult = true;
	
	std::list<std::string>::const_iterator line_ci = lines_list.begin();
	std::list<std::string>::const_iterator line_cend = lines_list.end();
	size_t line_count = 0;
	while (bResult && line_ci != line_cend) {
		std::string line = *line_ci++;

		++line_count;

		std::string header_filename;
		if (get_include_path(line, header_filename)) {
			std::map<std::string, std::string>::iterator it = headers_available.find(header_filename);
			if (it == headers_available.end()) {
				fout << line;
			} else if (it->second != "") {
				fout << "/**************** leaving " << source_filename << " temporarily *****************/\n"; 
				std::string header_path = it->second;

				// Make sure we don't emit header_path again.
				headers_available[header_filename] = "";

				bResult = copy_file(fout, current_language, headers_available, header_path);

				fout << "/**************** continuing " << source_filename << " where we left off *****************/\n"; 
				fout << "\n\n#line " << line_count << " \"" << source_filename << "\"\n";
			} else {
				fout << "/**************** already included " << header_filename << " -- not including again *****************/\n";
				fout << "\n\n#line " << line_count << " \"" << source_filename << "\"\n";
			}
		} else {
			fout << line;
		}
	}

	if (current_language == "C") {
		fout << "\n#ifdef __cplusplus\n"
		     << "} /* extern \"C\" ends here. */\n"
		     << "#endif\n\n";
	}
	
	return bResult;
}

bool make_sourcecode_amalgamation_files(const JSONValue *json_dict)
{
	const JSONValue *source_files_list = (*json_dict)["source_files_list"];

	if (source_files_list == 0) {
		std::cerr << "ERROR: Could not determine source files list.\nPlease amend the input JSON file.\n" << std::endl;
		return false;
	}

	bool bResult = true;

	const std::list<JSONValue*> sf_list = source_files_list->getList();
	std::list<JSONValue*>::const_iterator sf_ci = sf_list.begin();
	std::list<JSONValue*>::const_iterator sf_cend = sf_list.end();
	while (bResult && sf_ci != sf_cend) {
		const JSONValue *pObj = *sf_ci++;

		if (pObj->getKind() == kJSONObject) {
			std::string current_language = get_JSON_string(pObj, "language");

			std::string current_output_filename = get_JSON_string(pObj, "output_filename");

			// This will set pCode_prefix_dict to 0 if there is no key 'code_prefix'.
			const JSONValue *pCode_prefix_dict = pObj->getObjectValue("code_prefix");

			std::ofstream fout;
			fout.open(current_output_filename.c_str());
			if (!fout) {
				std::cerr << "FAILURE: Could not open file " << current_output_filename << " for writing." << std::endl;
				std::cerr << "Action aborted." << std::endl;
				return false;
			}
			
			
			fout << get_boilerplate();
			
			std::map<std::string, std::string> headers_available = make_headers_available(json_dict);
			
			
			fout << get_JSON_string(pObj, "prefix");
			
			const std::list<JSONValue*> source_filename_list = pObj->getObjectValue("source_files")->getList();
			std::list<JSONValue*>::const_iterator sf_ci2 = source_filename_list.begin();
			std::list<JSONValue*>::const_iterator sf_cend2 = source_filename_list.end();
			
			while (bResult && sf_ci2 != sf_cend2) {
				std::string source_filename = (*sf_ci2)->getString();
				
				if (pCode_prefix_dict != 0
				    && pCode_prefix_dict->hasObjectKey(source_filename)) {
					std::string code_prefix_string = get_JSON_string(pCode_prefix_dict, source_filename);
					fout << code_prefix_string;
				}
				
				bResult = copy_file(fout, current_language, headers_available, source_filename);
				
				++sf_ci2;
			}
			
			fout << get_JSON_string(pObj, "suffix");

			fout.close();
		}
	}

	return bResult;
}
				    
bool make_h_amalgamation(const JSONValue *json_dict, const std::string& output_filename)
{
	std::ofstream fout;
	fout.open(output_filename.c_str());
	if (!fout) {
		std::cerr << "FAILURE: Could not open file " << output_filename << " for writing." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		return false;
	}

	const JSONValue *source_files_list
		= (*json_dict)["source_files_list"];

	if (source_files_list == 0) {
		std::cerr << "ERROR: Could not determine source files list.\nPlease amend the input JSON file.\n" << std::endl;
		fout.close();
		return false;
	}

	fout << "#ifndef EMDROS_H_\n#define EMDROS_H_\n\n";
	
	fout << get_boilerplate();

	bool bResult = true;

	std::map<std::string, std::string> headers_available = make_headers_available(json_dict);

	const std::list<JSONValue*> sf_list = source_files_list->getList();
	std::list<JSONValue*>::const_iterator sf_ci = sf_list.begin();
	std::list<JSONValue*>::const_iterator sf_cend = sf_list.end();
	while (bResult && sf_ci != sf_cend) {
		const JSONValue *pObj = *sf_ci++;

		if (pObj->getKind() == kJSONObject) {
			std::string current_language = get_JSON_string(pObj, "language");

			if (current_language == "C++") {
				// This will set pCode_prefix_dict to 0 if there is no key 'code_prefix'.
				const JSONValue *pCode_prefix_dict = pObj->getObjectValue("code_prefix");
				
				fout << get_JSON_string(pObj, "prefix");
				
				const std::list<JSONValue*> source_filename_list = pObj->getObjectValue("header_files")->getList();
				std::list<JSONValue*>::const_iterator sf_ci2 = source_filename_list.begin();
				std::list<JSONValue*>::const_iterator sf_cend2 = source_filename_list.end();
				
				while (bResult && sf_ci2 != sf_cend2) {
					std::string source_filename = (*sf_ci2)->getString();
					std::string header_filename = get_header_filename_from_path(source_filename);
					
					bool bDoIt = false;
					std::map<std::string, std::string>::iterator it = headers_available.find(header_filename);
					if (it == headers_available.end()) {
						bDoIt = false;
					} else if (it->second != "") {
						bDoIt = true;
						
						// Make sure we don't emit it again.
						headers_available[header_filename] = "";
					} else {
						bDoIt = false;
					}
					
					if (bDoIt) {
						if (pCode_prefix_dict != 0
						    && pCode_prefix_dict->hasObjectKey(source_filename)) {
							std::string code_prefix_string = get_JSON_string(pCode_prefix_dict, source_filename);
							fout << code_prefix_string;
						}
						
						bResult = copy_file(fout, current_language, headers_available, source_filename);
					}
					
					
					++sf_ci2;
				}
			
				fout << get_JSON_string(pObj, "suffix");
			}
		}
	}

	fout << "\n#endif /* !defined(EMDROS_H_) */\n";
	
	fout.close();

	return bResult;
}
				    
bool doIt(const std::string json_filename)
{
	std::string error_message;
	const JSONValue *json_dict = readAndParseJSONFromFile(json_filename, error_message);
	if (json_dict == 0) {
		std::cerr << "ERROR parsing JSON file with filename '" << json_filename << "'.\nError message = " << error_message << std::endl;
		return false;
	}

	bool bDoneAtLeastOne = false;
	bool bResult = true;


	if (bResult) {
		bResult = make_sourcecode_amalgamation_files(json_dict);
		bDoneAtLeastOne = true;
	}

	if (bResult && json_dict->hasObjectKey("output_filename_h")) {
		std::string h_filename = json_dict->getObjectValue("output_filename_h")->getString();
		bResult = make_h_amalgamation(json_dict, h_filename);
		bDoneAtLeastOne = true;
	}

	if (!bDoneAtLeastOne) {
		std::cerr << "ERROR: Nothing was done. Why? Please check file '" << json_filename << "'" << std::endl;
		bResult = false;
	}
	

	return bResult;
}


void show_version(std::ostream& ostr)
{
	ostr << "mkamalgamation from Emdros " << EMDROS_VERSION << "\n" << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   mkamalgamation";
#ifdef WIN32
	ostr << ".exe";
#endif
	ostr << " json_filename" << std::endl;
}

int main(int argc, char* argv[])
{
	// Set defaults
	std::string filename;

	if (argc == 2) {
		filename = argv[1];
	} else {
		print_usage(std::cerr);
		return 1;
	}

	int nResult = 3;
	try {
		bool bResult = doIt(filename);
		if (!bResult) {
			nResult = 2;
		} else {
			nResult = 0;
		}
	} catch (EmdrosException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception occurred.  Program aborted." << std::endl;
	} 

	return nResult;
}


