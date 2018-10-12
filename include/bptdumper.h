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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
