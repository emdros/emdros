/*
 * bptdumper.cpp
 *
 * Classes and methods to dump another Emdros database into a Bit
 * Packed Table
 *
 * Ulrik Petersen
 * Created: 3/2-2010
 * Last update: 4/3-2018
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


#include <bptdumper.h>
#include <environment_emdros.h>


bool BPTdumpAsBPT(EmdrosEnv *pSourceEE, const std::string& bptfilename, const std::string& bptkey, std::string& /* out */ error_message, long schema_version, const std::string& payload_filename, const std::string& payload_inkey, const std::string& payload_codec)
{
	UNUSED(pSourceEE);
	UNUSED(bptfilename);
	UNUSED(bptkey);
	UNUSED(error_message);
	UNUSED(schema_version);
	UNUSED(payload_filename);
	UNUSED(payload_inkey);
	UNUSED(payload_codec);
	
	error_message = "Sorry, the BPT engine is not available as Open Source.\n"
		"Please contact Ulrik Sandborg-Petersen, <ulrikp{at}emergence[dot]dk>,\n"
		"for inquiries concerning licensing.\nThank you.\n\n";
	return false;
}


