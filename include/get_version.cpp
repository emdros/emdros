#include <iostream>
#include <fstream>
#include <string>

void usage(std::ostream& ostr)
{
	ostr << "USAGE:\n"
	     << "    get_version.exe path\\to\\configure.ac\n"
	     << std::endl;
}

int getInt(const std::string& instring, std::string::size_type start_index, std::string::size_type end_index)
{
	std::string intstring = instring.substr(start_index, end_index - start_index + 1);
	std::string::size_type index = 0;
	std::string::size_type length = intstring.length();
	int nResult = 0;
	while (index != length) {
		char c = intstring[index];
		int v = c - '0';
		if (v < 0
		    || v > 9) {
			break;
		} else {
			nResult *= 10;
			nResult += v;
		}
		++index;
	}
	return nResult;
}

void split_emdros_version(const std::string& EMDROS_VERSION,
			  int& EMDROS_VERSION_MAJOR,
			  int& EMDROS_VERSION_MINOR,
			  int& EMDROS_VERSION_RELEASE)
{
	std::string::size_type index = 0;
	std::string::size_type index_end = EMDROS_VERSION.find(".", index);
	EMDROS_VERSION_MAJOR = getInt(EMDROS_VERSION, index, index_end-1);

	index = index_end + 1;
	index_end = EMDROS_VERSION.find(".", index);
	EMDROS_VERSION_MINOR = getInt(EMDROS_VERSION, index, index_end-1);
	
	index = index_end + 1;
	index_end = EMDROS_VERSION.length();
	EMDROS_VERSION_RELEASE = getInt(EMDROS_VERSION, index, index_end-1);
}

void handle_output(const std::string& strLine, std::string::size_type index_version, std::string::size_type index_end, std::ostream& fout)
{
	std::string EMDROS_VERSION = strLine.substr(index_version, index_end - index_version + 1);
	
	int EMDROS_VERSION_MAJOR = 0;
	int EMDROS_VERSION_MINOR = 0;
	int EMDROS_VERSION_RELEASE = 0;
	split_emdros_version(EMDROS_VERSION,
			     EMDROS_VERSION_MAJOR,
			     EMDROS_VERSION_MINOR,
			     EMDROS_VERSION_RELEASE);
	
	fout << "#ifndef EMDROS_VERSION" << std::endl;
	fout << "#define EMDROS_VERSION \"" << EMDROS_VERSION << "\"" << std::endl;
	fout << "#endif" << std::endl;
	fout << std::endl;
	
	fout << "#ifndef EMDROS_VERSION_MAJOR" << std::endl;
	fout << "#define EMDROS_VERSION_MAJOR (" << EMDROS_VERSION_MAJOR << ")" << std::endl;
	fout << "#endif" << std::endl;
	fout << std::endl;
	
	fout << "#ifndef EMDROS_VERSION_MINOR" << std::endl;
	
	fout << "#define EMDROS_VERSION_MINOR (" << EMDROS_VERSION_MINOR << ")" << std::endl;
	fout << "#endif" << std::endl;
	fout << std::endl;
	
	fout << "#ifndef EMDROS_VERSION_RELEASE" << std::endl;
	fout << "#define EMDROS_VERSION_RELEASE (" << EMDROS_VERSION_RELEASE << ")" << std::endl;
	fout << "#endif" << std::endl;
	fout << std::endl;
}


int doIt(const std::string& configure_infilename, std::ostream& fout)
{
	int nResult = 2;
	std::ifstream fin(configure_infilename.c_str(), std::fstream::in);
	if (!fin.good()) {
		std::cerr << "ERROR: Could not open file '" << configure_infilename << "' for reading\n";
		nResult = 3;
	} else {
		std::streamsize nLineLength = 1024;
		char szLine[nLineLength];
		szLine[0] = '\0';
		while (fin.good()) {
			std::string strLine = szLine;
			std::string::size_type index = strLine.find("AC_INIT");
			if (index != std::string::npos) {
				nResult = 0;
				index = strLine.find(",");
				std::string::size_type index_version = strLine.find("[", index+1) + 1;
				std::string::size_type index_end = strLine.find("]", index_version) - 1;
				handle_output(strLine, index_version, index_end, fout);
				
				break;
			}
			
			fin.getline(szLine, nLineLength);
		}
	}
	
	fin.close();

	return nResult;
}

int main(int argc, char *argv[])
{
	int nResult = 0;
	if (argc != 2) {
		usage(std::cerr);
		nResult = 1;
	} else {
		std::string configure_infilename = argv[1];
		nResult = doIt(configure_infilename, std::cout);
	}
	
	switch (nResult) {
	case 0:
		// Success. Nothing to report
		break;
	case 1:
		std::cerr << "Wrong usage." << std::endl;
		break;
	case 2:
		std::cerr << "Invalid input - file does not contain AC_INIT.\n(Maybe it isn't the configure.ac script?)" << std::endl;
		break;
	case 3:
		std::cerr << "Invalid input - could not open file." << std::endl;
		break;
	}
	return nResult;
}
