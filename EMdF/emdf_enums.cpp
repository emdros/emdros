/*
 * emdf_enums.cpp
 *
 * EMdF enumerations
 *
 * Ulrik Sandborg-Petersen
 * Created: 11/14-2018
 * Last update: 11/14-2018
 *
 */

/*
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


/**@file emdf_enums.cpp
 *@brief Implementation file for EMdF enumerations (EMdF layer)
 *
 */
#include "emdf_enums.h"
#include "string_func.h"


eComputedFeatureKind getComputedFeatureKindFromComputedFeatureName(const std::string& computed_feature_name)
{
	eComputedFeatureKind kind = kCFKNone;
	
	if (strcmp_nocase(computed_feature_name, "first_monad") == 0) {
		kind = kCFKFirstMonad;
	} else if (strcmp_nocase(computed_feature_name, "last_monad") == 0) {
		kind = kCFKLastMonad;
	} else if (strcmp_nocase(computed_feature_name, "monad_count") == 0) {
		kind = kCFKMonadCount;
	} else if (strcmp_nocase(computed_feature_name, "monad_set_length") == 0) {
		kind = kCFKMonadSetLength;
	} else {
		kind = kCFKNone;
	}
	
	return kind;
}
