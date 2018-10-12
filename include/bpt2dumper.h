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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
