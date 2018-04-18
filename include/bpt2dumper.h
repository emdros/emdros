/*
 * bpt2dumper.h
 *
 * Classes and methods to dump another Emdros database into a Bit
 * Packed Table version 2.
 *
 * Ulrik Petersen
 * Created: 9/19-2014
 * Last update: 9/19-2014
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2010-2014  Ulrik Sandborg-Petersen
 *
 *   This file is NOT under the GNU General Public License.  You must
 *   obtain a license from the author, Ulrik Sandborg-Petersen, in
 *   order to use and distribute this sourcecode, or binaries based on
 *   it.  Questions, including licensing inquiries, should be directed
 *   to:  ulrikp@emergence.dk.
 *
 *
 **************************************************************************/

/**@file bpt2dumper.h
 *@brief Header file for BPT2Dumper (EMdF layer)
 */

#ifndef BPT2DUMPER__H__
#define BPT2DUMPER__H__

#include <string>
#include "emdf_enums.h"

class EmdrosEnv;

#define BPT2_SCHEMA_1      (1L)

bool BPT2dumpAsBPT2(EmdrosEnv *pSourceEE, const std::string& bptfilename, const std::string& bptkey, std::string& /* out */ error_message, long schema_version, const std::string& payload_filename, const std::string& payload_inkey);

#endif // BPT2DUMPER__H__
