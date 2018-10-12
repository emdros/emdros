/*
 * emdf.h
 *
 * Basic definitions for EMdF
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



/**@file emdf.h
 *@brief Header file for basic EMdF definitions (EMdF layer)
 */


#ifndef EMDF__H__
#define EMDF__H__

/**
 *\defgroup EMdF EMdF layer
 */

#include "emdros-lconfig.h"
#include "version-emdros.h"


#if defined(_MSC_VER)
#pragma warning( disable : 4290 ) 
#endif

typedef long id_d_t;
typedef long monad_m;

#define SEQUENCE_OBJECT_ID_DS   (0)
#define SEQUENCE_TYPE_IDS       (1)
#define SEQUENCE_OTHER_IDS      (2)

/* This hack is necessary because of a bug in the SWIG Ruby backend
   which would generate garbage for MIN_ENUM_TYPE_ID if we did not use
   unparenthesized versions of SEQUENCE_START and
   SEQUENCE_TYPE_IDS_FREE_LOWER_BITS. */
#define SEQUENCE_START_UNPARENTHESIZED 1
#define SEQUENCE_START          (SEQUENCE_START_UNPARENTHESIZED)

/* This hack is necessary because of a bug in the SWIG Ruby backend
   which would generate garbage for MIN_ENUM_TYPE_ID if we did not use
   unparenthesized versions of SEQUENCE_START and
   SEQUENCE_TYPE_IDS_FREE_LOWER_BITS. */
#define SEQUENCE_TYPE_IDS_FREE_LOWER_BITS_UNPARENTHESIZED  16
#define SEQUENCE_TYPE_IDS_FREE_LOWER_BITS  (SEQUENCE_TYPE_IDS_FREE_LOWER_BITS_UNPARENTHESIZED)

#define MIN_ENUM_TYPE_ID        (SEQUENCE_START_UNPARENTHESIZED << SEQUENCE_TYPE_IDS_FREE_LOWER_BITS_UNPARENTHESIZED)


#define MAX_DBNAME_CHARS         (32)
#define MAX_IDENTIFIER_CHARS     (512)

#define MAX_MONAD               (2100000000L)


/* The maximum chars to put in an index on MySQL for 
 * EMdFDB::getSQL_TEXT_TYPE() columns. */
#define MAX_TEXT_INDEX_CHARS     (20)


/* FEATURE_TYPE_TYPE_MASK doesn't catch enums: They need to be caught
 * by checking for FEATURE_TYPE_ENUM.
 */
#define FEATURE_TYPE_TYPE_MASK  (0x000000FFL)
#define FEATURE_TYPE_INTEGER              (0)
#define FEATURE_TYPE_STRING               (1)
#define FEATURE_TYPE_ASCII                (2)
#define FEATURE_TYPE_ID_D                 (3)
#define FEATURE_TYPE_ENUM                 (4)
#define FEATURE_TYPE_SET_OF_MONADS        (5)
#define FEATURE_TYPE_LIST_OF_INTEGER      (8)
#define FEATURE_TYPE_LIST_OF_ID_D         (11)
#define FEATURE_TYPE_LIST_OF_ENUM         (12)


/* We need to stay clear of the bits above bit 16, since from bit 16
 * (65536) onwards, the feature type is used for enumerations.
 *
 * Note to self: Here, 16 really means
 * SEQUENCE_TYPE_IDS_FREE_LOWER_BITS, so if you change that, please
 * change this comment.
 */
#define FEATURE_TYPE_FLAGS_MASK (0x0000FF00L)
#define FEATURE_TYPE_WITH_INDEX (0x00000100L)
#define FEATURE_TYPE_FROM_SET   (0x00000200L)

#undef NIL
#define NIL                     (0)
#define NIL_AS_STRING           "0"
#define NIL_AS_VISIBLE_STRING   "nil"

#define OBJECT_TYPE_POW_M       (-1)

#define DEFAULT_LIST_DELIMITER  " "


/* Note: This is not a bit-field, but a three-bit integer! */
#define OT_RANGE_MASK                  (0x00000007)
#define OT_WITH_MULTIPLE_RANGE_OBJECTS (0x00000000)
#define OT_WITH_SINGLE_RANGE_OBJECTS   (0x00000001)
#define OT_WITH_SINGLE_MONAD_OBJECTS   (0x00000002)


/* Note: This is not a bit-field, but a three-bit integer! */
#define OT_MONAD_UNIQUENESS_MASK       	      (0x00000078)
#define OT_WITHOUT_UNIQUE_MONADS       	      (0x00000000)
#define OT_HAVING_UNIQUE_FIRST_MONADS           (0x00000008)
#define OT_HAVING_UNIQUE_FIRST_AND_LAST_MONADS  (0x00000010)


/* Silence compiler warnings... */
#define UNUSED(V) ((void) (V))

/*
 * Emdros versions 
 */

#define EMDROS_VERSION_PRE_1_1_0                        (0)
#define EMDROS_VERSION_1_1_0_TO_1_1_4                   (1)
#define EMDROS_VERSION_1_1_5_TO_1_1_7                   (2)
#define EMDROS_VERSION_1_1_8_TO_1_1_20                  (3)
#define EMDROS_VERSION_1_2_0_PRE_01_TO_1_2_0_PRE_58     (4)
#define EMDROS_VERSION_1_2_0_PRE_59_TO_1_2_0_PRE_79     (5)
#define EMDROS_VERSION_1_2_0_PRE_80_TO_1_2_0_PRE_85     (6)
#define EMDROS_VERSION_1_2_0_PRE_86_TO_1_2_0_PRE_96     (7)
#define EMDROS_VERSION_1_2_0_PRE_97_TO_1_2_0_PRE_151    (8)
#define EMDROS_VERSION_1_2_0_PRE_152_TO_1_2_0_PRE_153   (9)
#define EMDROS_VERSION_1_2_0_PRE_154_TO_1_2_0_PRE198    (10)
#define EMDROS_VERSION_1_2_0_PRE_199_TO_1_2_0_PRE207    (11)
#define EMDROS_VERSION_1_2_0_PRE_208                    (12)
#define EMDROS_VERSION_CURRENT     (EMDROS_VERSION_1_2_0_PRE_208)


#endif /* EMDF__H__ */
