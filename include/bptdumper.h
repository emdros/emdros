/*
 * bptdumper.h
 *
 * Classes and methods to dump another Emdros database into a Bit
 * Packed Table
 *
 * Ulrik Petersen
 * Created: 3/2-2010
 * Last update: 4/3-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2010-2018  Ulrik Sandborg-Petersen
 *
 *   This file is NOT under the GNU General Public License.  You must
 *   obtain a license from the author, Ulrik Sandborg-Petersen, in
 *   order to use and distribute this sourcecode, or binaries based on
 *   it.  Questions, including licensing inquiries, should be directed
 *   to:  ulrikp@emergence.dk.
 *
 *
 **************************************************************************/

/**@file bptdumper.h
 *@brief Header file for BPTDumper (EMdF layer)
 */

#ifndef BPTDUMPER__H__
#define BPTDUMPER__H__

#include <string>
#include "emdf_enums.h"

class EmdrosEnv;

#define BPT_SCHEMA_1      (1L)
#define BPT_SCHEMA_2      (3L)
#define BPT_SCHEMA_3      (3L)
#define BPT_SCHEMA_5      (5L)
#define BPT_SCHEMA_7      (7L)

bool BPTdumpAsBPT(EmdrosEnv *pSourceEE, const std::string& bptfilename, const std::string& bptkey, std::string& /* out */ error_message, long schema_version, const std::string& payload_filename, const std::string& payload_inkey, const std::string& payload_codec);

#endif // BPTDUMPER__H__
