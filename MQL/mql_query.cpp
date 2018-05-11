/*
 * mql_query.cpp
 *
 * MQL query classes
 *
 * Ulrik Petersen
 * Created: 2/27-2001
 * Last update: 5/11-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, license version 2.  
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307 USA
 *
 *
 *   Special exception
 *   =================
 * 
 *   In addition, as a special exception, Ulrik Petersen, the
 *   copyright holder of Emdros, gives permission to link Emdros, in
 *   whole or in part, with the libraries which are normally
 *   distributed with:
 *   
 *   a) Sun's Java platform,
 *   b) Python, 
 *   c) Jython,
 *   d) Ruby, and/or 
 *   e) Perl 
 *   f) PostgreSQL
 *   g) OpenSSL
 *
 *   (or with modified versions of these), and to distribute linked
 *   combinations including both Emdros, in whole or in part, and one
 *   or more of the libraries normally distributed with (a)-(g) above.
 *
 *   Please note: This gives you special rights concerning the
 *   libraries which normally accompany the above pieces of software.
 *   It gives you no special rights concerning software that you write
 *   yourself.  You must obey the GNU General Public License in all
 *   respects for all of the code used other than the libraries
 *   normally distributed with (a)-(g) above.
 *
 *   If you modify this file, you may extend this exception to your
 *   version of the file, but you are not obligated to do so. If you
 *   do not wish to do so, delete this exception statement from your
 *   version.
 *
 *
 *   Other licensing forms
 *   =====================
 *
 *   If you wish to negotiate commercial licensing, please contact
 *   Ulrik Petersen at ulrikp[at]users.sourceforge.net.
 *
 *   Licensing can also be negotiated if your organization is an
 *   educational, non-profit, charity, missionary or similar
 *   organization.
 *
 *
 *   Website
 *   =======
 *
 *   Emdros has a website here:
 *
 *   http://emdros.org
 *
 *
 *
 **************************************************************************/

/**@file mql_query.cpp
 *@brief Implementation of classes related to topographic queries (MQL layer)
 */


#include <string>
#include <emdfdb.h>
#include <emdf_ffeatures.h>
#include <enum_const_cache.h>
#include <mql_query.h>
#include <mql_error.h>
#include <mql_extern.h>
#include <mql_R.h>
#include <mql_types.h>
#include <emdf_value.h>
#include <mql_object.h>
#include <string_func.h>
#include <mql_execution_environment.h>
#include <cstdlib>
#include <logging.h>
#include <monads.h>
#include <inst.h>
#include <debug.h>

#undef MYMAX
#undef MYMIN

#define MYMAX(A, B) (((A) > (B)) ? (A) : (B))
#define MYMIN(A, B) (((A) < (B)) ? (A) : (B))

////////////////////////////////////////////////////////////
//
// MonadSetRelationClause
//
////////////////////////////////////////////////////////////


MonadSetRelationClause::MonadSetRelationClause(eMonadSetRelationOperation operation,
					       std::string* ourMonadSet,
					       eUniverseOrSubstrate universeOrSubstrate)
{
	m_feature_type_id = 0;

	m_operation = operation;

	str_tolower(*ourMonadSet, m_our_monad_set);
	delete ourMonadSet;

	m_universe_or_substrate = universeOrSubstrate;
}

MonadSetRelationClause::MonadSetRelationClause(const MonadSetRelationClause& other)
{
	m_feature_type_id = other.m_feature_type_id;

	m_operation = other.m_operation;

	m_our_monad_set = other.m_our_monad_set;

	m_universe_or_substrate = other.m_universe_or_substrate;
}

MonadSetRelationClause::~MonadSetRelationClause()
{
}


void MonadSetRelationClause::weed(MQLExecEnv *pEE, bool& bResult)
{
	UNUSED(pEE);
	
	bResult = true;
}

bool MonadSetRelationClause::symbol(MQLExecEnv *pEE, id_d_t object_type_id, bool& bResult)
{
	if (strcmp_nocase(m_our_monad_set, "monads") == 0) {
		bResult = true;
		return true;
	} else {
		bool bFeatureExists;
		std::string dummy_default_value;
		bool dummy_is_computed;
		if (!pEE->pDB->featureExists(m_our_monad_set,
					     object_type_id,
					     bFeatureExists,
					     m_feature_type_id,
					     dummy_default_value,
					     dummy_is_computed))
			return false;
		if (!bFeatureExists) {
			pEE->pError->appendError("The feature " + m_our_monad_set + " does not exist\non the object type of the object block with a monad set relation clause.\n");
			bResult = false;
			return true;
		} else {
			bResult = true;
			return true;
		}
	}
}

bool MonadSetRelationClause::type(MQLExecEnv *pEE, bool& bResult)
{
	if (strcmp_nocase(m_our_monad_set, "monads") == 0) {
		bResult = true;
		return true;
	} else {
		if (!featureTypeIdIsSOM(m_feature_type_id)) {
			pEE->pError->appendError("Although the feature " + m_our_monad_set + " exists\non the object type of the object block with a monad set relation clause,\nit is not of the type SET OF MONADS and therefore cannot be used in the\nmonad set relation clause.\n");
			bResult = false;
			return true;
		} else {
			bResult = true;
			return true;
		}
	}
}


bool MonadSetRelationClause::exec(const SetOfMonads& object_som,
				  const SetOfMonads& universe, 
				  const SetOfMonads& substrate) const
{
    // Select universe or substrate
    const SetOfMonads *somp;
    switch (m_universe_or_substrate) {
      case kMSNSubstrate:
            somp = &substrate;
            break;

      case kMSNUniverse:
            somp = &universe;
            break;

      default:
            ASSERT_THROW(false,
                         "Unknown monad set name in MonadSetRelationClause::operation()");
            break;
    }

    // Perform operation
    switch (m_operation) {
      case kMSROPartOf:
            return object_som.part_of(*somp);

      case kMSROOverlap:
            return SetOfMonads::overlap(object_som, *somp);

      case kMSROStartsIn:
            return object_som.starts_in(*somp);

      default:
            ASSERT_THROW(false,
                         "Unknown operation in MonadSetRelationClause::operation()");
            break;
	}
}

std::string MonadSetRelationClause::calculateCharacteristicString() const
{
	std::string characteristic_string;

	switch (m_operation) {
	case kMSROPartOf:
		characteristic_string += "PO";
		break;
	case kMSROOverlap:
		characteristic_string += "OL";
		break;
	case kMSROStartsIn:
		characteristic_string += "SI";
		break;
	default:
		ASSERT_THROW(false, "Unknown m_operation");
		break;
	}

	characteristic_string += "(" + m_our_monad_set + ",";
	switch (m_universe_or_substrate) {
	case kMSNSubstrate:
		characteristic_string += "Su)";
		break;
	case kMSNUniverse:
		characteristic_string += "U)";
		break;
	default:
		ASSERT_THROW(false, "Unknown m_universe_or_substrate");
		break;
	}

	return characteristic_string;
}


////////////////////////////////////////////////////////////
//
// UsingRange
//
////////////////////////////////////////////////////////////

UsingRange::UsingRange(long start, long end)
	: m_start(start),
	  m_end(end)
{
}

void UsingRange::weed(MQLExecEnv *pEE, bool& bResult)
{
	if (m_start < 0 
	    || m_end < 0) {
		pEE->pError->appendError("The RANGE(" + long2string(m_start) + "," + long2string(m_end) + ") cannot use negative numbers.\n");
		bResult = false;
	}
	if (m_start > m_end) {
		pEE->pError->appendError("The RANGE(" + long2string(m_start) + "," + long2string(m_end) + ") cannot have start > end.\n");
		bResult = false;
	}
}




////////////////////////////////////////////////////////////
//
// StartMonadIterator 
//
////////////////////////////////////////////////////////////

class InstStartMonadIterator : public StartMonadIterator {
protected:
	Inst *m_pInst;
	Inst::const_iterator m_instci;
public:
	InstStartMonadIterator(Inst *pInst, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm);
	virtual ~InstStartMonadIterator();
	virtual bool hasNext();
	virtual monad_m next();
	virtual std::string getSMIKind() { return "InstStartMonadIterator"; }

 private:
	// Disallow assignment, copy construction, and default construction
	InstStartMonadIterator() {};
	InstStartMonadIterator& operator=(const InstStartMonadIterator& other) { UNUSED(other); return *this;};
	InstStartMonadIterator(const InstStartMonadIterator& other) : StartMonadIterator(other) { };
};

class MonadSetStartMonadIterator : public StartMonadIterator {
protected:
	SOMConstMonadIterator m_somci;
public:
	MonadSetStartMonadIterator(const SetOfMonads& Su, monad_m Sm, bool isGaps);
	virtual ~MonadSetStartMonadIterator() {};
	virtual bool hasNext();
	virtual monad_m next();
	virtual std::string getSMIKind() { return "MonadSetStartMonadIterator"; }

 private:
	// Disallow assignment and copy construction
	MonadSetStartMonadIterator() {};
	MonadSetStartMonadIterator& operator=(const MonadSetStartMonadIterator& other) { UNUSED(other); return *this;};
	MonadSetStartMonadIterator(const MonadSetStartMonadIterator& other) : StartMonadIterator(other) { };
};

InstStartMonadIterator::InstStartMonadIterator(Inst *pInst, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm)
	: StartMonadIterator(Su),
	  m_pInst(pInst)
{
	m_pInst->set_current_universe(U);
	m_instci = m_pInst->begin();

	while (m_instci.hasNext()) {
		const InstObject *pInstObj = m_instci.current();
		monad_m current = pInstObj->first();
		if (current >= Sm) {
			break;
		} else {
			m_pInst->nextAtHigherKey(m_instci);
		}
	}
}



MonadSetStartMonadIterator::MonadSetStartMonadIterator(const SetOfMonads& Su, monad_m Sm, bool isGaps)
	: StartMonadIterator((isGaps) ? (Su.getGaps()) : (Su)),
	  m_somci(m_Su)
{
	while (m_somci.hasNext()) {
		monad_m current = m_somci.current();
		if (current >= Sm) {
			break;
		} else {
			m_somci.next();
		}
	}
}


InstStartMonadIterator::~InstStartMonadIterator()
{
	if (!m_pInst->isAggregate()) {
		delete m_pInst;
	}
}


bool InstStartMonadIterator::hasNext()
{
	return m_instci.hasNext();
}

bool MonadSetStartMonadIterator::hasNext() 
{
	return m_somci.hasNext();
}


monad_m InstStartMonadIterator::next()
{
	const InstObject *pInstObj = m_instci.current();
	m_pInst->nextAtHigherKey(m_instci);
	return pInstObj->first();
}

monad_m MonadSetStartMonadIterator::next()
{
	return m_somci.next();
}




////////////////////////////////////////////////////////////
//
// NonParentORDSolution
//
////////////////////////////////////////////////////////////


NonParentORDSolution::NonParentORDSolution()
{
}

NonParentORDSolution::NonParentORDSolution(NonParentORDSolution& other)
{
	unsigned int other_size = other.m_vec.size();
	for (unsigned int index = 0;
	     index < other_size;
	     ++index) {
		m_vec.push_back(other[index]);
	}
}

// Extends pOther with myself, i.e., appends all in me to pOther
void NonParentORDSolution::extendWithMe(NonParentORDSolution *pOther)
{
	unsigned int my_size = m_vec.size();
	for (unsigned int index = 0;
	     index < my_size;
	     ++index) {
		pOther->m_vec.push_back(m_vec[index]);
	}
}

const MatchedObject* NonParentORDSolution::operator[](int index)
{
	return m_vec[index];
}


NonParentORDSolution::~NonParentORDSolution()
{
	m_vec.clear();
}


////////////////////////////////////////////////////////////
//
// GapBlock 
//
////////////////////////////////////////////////////////////
GapBlock::GapBlock(eRetrieval retrieval, std::string *mark_declaration, Blocks *opt_blocks)
	: m_retrieval(retrieval), m_opt_blocks(opt_blocks), m_pOBB(0), m_marks(mark_declaration)
{
}

GapBlock::~GapBlock()
{
	delete m_opt_blocks;
	delete m_marks;
	// Do not delete m_pOBB -- it has been added to OBBVec.
}

void GapBlock::weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, node_number_t block_string2_parent)
{
	UNUSED(is_first);
	UNUSED(is_last);
	
	if (m_opt_blocks) {
		m_opt_blocks->weed(pEE, bResult, block_string2_parent);
	}
}

bool GapBlock::symbol(MQLExecEnv *pEE, bool& bResult)
{
	//
	// - Check the blocks
	//
	if (m_opt_blocks != 0) {
		if (!m_opt_blocks->symbol(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

  
	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

bool GapBlock::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	//
	// - Check the blocks
	//
	if (m_opt_blocks != 0) {
		if (!m_opt_blocks->symbolObjectReferences(pEE, bResult, ORD_set))
			return false;
		if (!bResult)
			return true;
	}

  
	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}


bool GapBlock::symbolObjectReferences2(MQLExecEnv *pEE)
{
	//
	// - Check the blocks
	//
	if (m_opt_blocks != 0) {
		if (!m_opt_blocks->symbolObjectReferences2(pEE))
			return false;
	}

  
	// If we got this far, there were no DB errors
	return true;
}


bool GapBlock::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	UNUSED(contextRangeType);
	
	// Check opt_blocks
	if (m_opt_blocks != 0) {
		if (!m_opt_blocks->type(pEE, kORTSingleRange, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// If we got this far, there were no DB errors and no compiler-errors
	bResult = true;
	return true;
}

bool GapBlock::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	// Write to log
	// LOG_WRITE_TIME("GapBlock::aggregateQuery", *m_object_type_name + " Making inst.");

	// Recurse down through the opt_blocks
	if (m_opt_blocks != 0) {

		switch (strategy) {
		case kAQSOutermostFirst:
			// Recurse downwards, but only if characteristic_set is not empty!
			// If it is empty, there are no monads within which to look,
			// and we will get a NoMonadsException.
			// Also, it means that the query from this object_block_common
			// is going to return no results, since an empty bigUnion means
			// there are no objects in the inst.  This means that there will be
			// no matches inside it either, so there is no point in 
			// aggregately querying inside.
			if (!characteristic_set.isEmpty()) {
				// LOG_WRITE_TIME("GapBlock::aggregateQuery ", *m_object_type_name + " Recursing down through opt_blocks.");
				if (!m_opt_blocks->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap))
					return false;
				// LOG_WRITE_TIME("GapBlock::aggregateQuery ", *m_object_type_name + " Recursion finished.");
			} 
			break;
		case kAQSInnermostFirst: {
			ASSERT_THROW(false,
				     "We should not get here, since we don't know how to d Innermost First strategy with this Gap Block");

		}
			break;
		default:
			ASSERT_THROW(false,
				     "Unknown aggregate query strategy");
			break;
			
		}
	}

	// If we came this far, there were no DB errors
	return true;
}

void GapBlock::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	if (m_opt_blocks != 0) {
		// We currently don't know what to do with gap blocks
		// and opt gap blocks that have inner blocks when
		// doing innermost first strategy, so we ban it.
		bResult = false;
	}
}

void GapBlock::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB)
{
	// Get characteristic string
	std::string characteristic_string = prefix + "gap,";

	// Don't add this to the mmap; it is not necessary.
	// mmap.insert(String2COBPtrMMap::value_type(characteristic_string, this));
 
	// Recurse down through inner blocks
	if (m_opt_blocks != 0) {
		std::string prefix_opt_blocks = characteristic_string + ";";
		m_opt_blocks->calculateMMapOutermost(mmap, 
					    prefix_opt_blocks,
					    pDB);
	}
}

void GapBlock::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	// Recurse down through inner blocks
	std::string opt_blocks_charstring;
	if (m_opt_blocks != 0) {
		m_opt_blocks->calculateMMapInnermost(mmap, 
						     opt_blocks_charstring,
						     pDB);
	}
	// Get characteristic string
	inner_charstring = "gap," + opt_blocks_charstring;

	// Don't add this to the mmap; it is not necessary.
	// mmap.insert(String2COBPtrMMap::value_type(characteristic_string, this));
 
}
      


void GapBlock::addOBBToVec(OBBVec *pOBBVec)
{
	m_pOBB = new ObjectBlockBase(new std::string("pow_m"),
				     new std::string(*m_marks),
				     m_retrieval,
				     (Feature*) 0,
				     false // Is (not, in this case) NOT EXIST
				     );
	pOBBVec->push_back(m_pOBB);
	if (m_opt_blocks) {
		m_opt_blocks->addOBBToVec(pOBBVec);
	}
}






////////////////////////////////////////////////////////////
//
// Block 
//
////////////////////////////////////////////////////////////
Block::Block(OptGapBlock* opt_gap_block)
	: ByMonads(0),
	  m_kind(kOptGapBlock), 
	  m_gap_block(0),
	  m_opt_gap_block(opt_gap_block),
	  m_object_block(0),
	  m_power_block(0)
    
{
}

Block::Block(GapBlock* gap_block)
	: ByMonads(0),
	  m_kind(kGapBlock), 
	  m_gap_block(gap_block),
	  m_opt_gap_block(0),
	  m_object_block(0),
	  m_power_block(0)
{
}

Block::Block(Power *power)
	: ByMonads(0),
	  m_kind(kPowerBlock), 
	  m_gap_block(0),
	  m_opt_gap_block(0),
	  m_object_block(0),
	  m_power_block(power)
{
}

Block::Block(ObjectBlock* object_block)
	: ByMonads(0),
	  m_kind((object_block->isNOTEXIST()) ? kObjectBlockNOTEXIST : kObjectBlock), 
	  m_gap_block(0),
	  m_opt_gap_block(0),
	  m_object_block(object_block),
	  m_power_block(0)
{
}

Block::~Block()
{
	delete m_gap_block;
	delete m_opt_gap_block;
	delete m_object_block;
	delete m_power_block;
}

void Block::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	std::string som_name;
	switch(m_kind) {
	case kOptGapBlock:
		break;
	case kGapBlock:
		break;
	case kPowerBlock:
		break;
	case kObjectBlock:
	case kObjectBlockNOTEXIST:
		str_tolower(m_object_block->getMSRC()->getOurMonadSet(), som_name);
		som_name_set.insert(som_name);
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
}

void Block::weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, node_number_t block_string2_parent)
{
	switch(m_kind) {
	case kOptGapBlock:
	        m_opt_gap_block->weed(pEE, bResult, is_first, is_last, block_string2_parent);
		break;
	case kGapBlock:
	        m_gap_block->weed(pEE, bResult, is_first, is_last, block_string2_parent);
		break;
	case kPowerBlock:
	        m_power_block->weed(bResult);
		if (!bResult) {
			pEE->pError->appendError("Error: Something wrong with a restrictor on a power block (\"..\").\n"
						 "If the restrictor is \"< integer\", the integer must not be 0.\n"
						 "If the restrictor is \"between X and Y\", then X must be less than or\n"
						 "equal to Y.\n");
		} else {
			// A power block must NEITHER be first, NOR be last!
			bResult = (!is_first && !is_last);
			if (!bResult) {
				pEE->pError->appendError("Error: The power block cannot appear at the beginning or end.\n");
			}
		}
		
		break;
	case kObjectBlock:
	case kObjectBlockNOTEXIST:
		m_object_block->weed(pEE, bResult, is_first, is_last, block_string2_parent);
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
}

bool Block::symbol(MQLExecEnv *pEE, bool& bResult)
{
	switch(m_kind) {
	case kOptGapBlock:
		if (!m_opt_gap_block->symbol(pEE, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	case kPowerBlock:
		bResult = true;
		return true;
	case kGapBlock:
		if (!m_gap_block->symbol(pEE, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	case kObjectBlock:
		if (!m_object_block->symbol(pEE, true, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	case kObjectBlockNOTEXIST:
		if (!m_object_block->symbol(pEE, true, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
	bResult = true;
	return true;
}

bool Block::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	std::set<std::string> ObjectBlockInnerORD_set;

	switch(m_kind) {
	case kOptGapBlock:
		if (!m_opt_gap_block->symbolObjectReferences(pEE, bResult, ORD_set))
			return false;
		if (!bResult)
			return true;
		break;
	case kPowerBlock:
		bResult = true;
		return true;
	case kGapBlock:
		if (!m_gap_block->symbolObjectReferences(pEE, bResult, ORD_set))
			return false;
		if (!bResult)
			return true;
		break;
	case kObjectBlock:
		if (!m_object_block->symbolObjectReferences(pEE, bResult, ORD_set))
			return false;
		if (!bResult)
			return true;
		break;
	case kObjectBlockNOTEXIST:
		ObjectBlockInnerORD_set = ORD_set;
		if (!m_object_block->symbolObjectReferences(pEE, bResult, ObjectBlockInnerORD_set))
			return false;
		if (!bResult)
			return true;
		// If any ORD is in ObjectBlockInnerORD_set but not in ORD_set, 
		// it will not get passed back up, since we have not touched
		// ORD_set.  This is as it should be: We cannot use it 
		// above this ObjectBlock* or NOTEXIST ObjectBlock.
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
	bResult = true;
	return true;
}




 bool Block::symbolObjectReferences2(MQLExecEnv *pEE)
{
	std::set<std::string> ObjectBlockInnerORD_set;
	switch(m_kind) {
	case kOptGapBlock:
		if (!m_opt_gap_block->symbolObjectReferences2(pEE))
			return false;
		break;
	case kPowerBlock:
		return true;
	case kGapBlock:
		if (!m_gap_block->symbolObjectReferences2(pEE))
			return false;
		break;
	case kObjectBlock:
		if (!m_object_block->symbolObjectReferences2(pEE))
			return false;
		break;
	case kObjectBlockNOTEXIST:
		if (!m_object_block->symbolObjectReferences2(pEE))
			return false;
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
	return true;
}




bool Block::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	switch(m_kind) {
	case kOptGapBlock:
		if (!m_opt_gap_block->type(pEE, contextRangeType, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	case kGapBlock:
		if (!m_gap_block->type(pEE, contextRangeType, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	case kPowerBlock:
		bResult = true;
		return true;
	case kObjectBlock:
	case kObjectBlockNOTEXIST:
		if (!m_object_block->type(pEE, contextRangeType, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown object block type"); // In case we ever invent more labels...
		break;
	}
	bResult = true;
	return true;
}

// Is it an opt_gap_block or a gap_block?
bool Block::isGapOrOptGapBlock(void)
{
	return m_kind == kOptGapBlock || m_kind == kGapBlock;
}

void Block::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB)
{
	switch(m_kind) {
	case kOptGapBlock:
		m_opt_gap_block->calculateMMapOutermost(mmap, prefix, pDB);
		break;
	case kGapBlock:
		m_gap_block->calculateMMapOutermost(mmap, prefix, pDB);
		break;
	case kPowerBlock:
		break;
	case kObjectBlock:
	case kObjectBlockNOTEXIST:
		m_object_block->calculateMMapOutermost(mmap, prefix, pDB);
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
}

void Block::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	switch(m_kind) {
	case kOptGapBlock:
		m_opt_gap_block->calculateMMapInnermost(mmap, inner_charstring, pDB);
		break;
	case kGapBlock:
		m_gap_block->calculateMMapInnermost(mmap, inner_charstring, pDB);
		break;
	case kPowerBlock:
		break;
	case kObjectBlock:
	case kObjectBlockNOTEXIST:
		m_object_block->calculateMMapInnermost(mmap, inner_charstring, pDB);
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
}
      

bool Block::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	switch(m_kind) {
	case kOptGapBlock:
		m_opt_gap_block->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap);
		break;
	case kGapBlock:
		m_gap_block->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap);
		break;
	case kPowerBlock:
		break;
	case kObjectBlock:
	case kObjectBlockNOTEXIST:
		// Do it if there is more than one with this characteristic string
		return m_object_block->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap);
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
  
	// If we came this far, there were no DB errors
	return true;
}

void Block::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	switch(m_kind) {
	case kOptGapBlock:
		m_opt_gap_block->canChooseAQStrategyInnermostFirst(bResult);
		break;
	case kGapBlock:
		m_gap_block->canChooseAQStrategyInnermostFirst(bResult);
		break;
	case kPowerBlock:
		break;
	case kObjectBlock:
		m_object_block->canChooseAQStrategyInnermostFirst(bResult);
		break;
	case kObjectBlockNOTEXIST:
		// We currently don't know how to do a NOTEXIST with the InnermostFirst strategy...
		bResult = false;
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
}

bool Block::hasObjectBlockInBlockString()
{
	bool bResult = false;
	switch(m_kind) {
	case kOptGapBlock:
		bResult = false;
		break;
	case kGapBlock:
		bResult = false;
		break;
	case kPowerBlock:
		bResult = false;
		break;
	case kObjectBlock:
		bResult = true;
		break;
	case kObjectBlockNOTEXIST:
		bResult = false;
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown block type"); // In case we ever invent more labels...
		break;
	}
	
	return bResult;
}

void Block::addOBBToVec(OBBVec *pOBBVec)
{
	switch(m_kind) {
	case kOptGapBlock:
		m_opt_gap_block->addOBBToVec(pOBBVec);
		break;
	case kGapBlock:
		m_gap_block->addOBBToVec(pOBBVec);
		break;
	case kPowerBlock:
		break;
	case kObjectBlock:
	case kObjectBlockNOTEXIST:
		m_object_block->addOBBToVec(pOBBVec);
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown object block type"); // In case we ever invent more labels...
		break;
	}
}

StartMonadIterator* Block::getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm)
{
	switch(m_kind) {
	case kOptGapBlock:
		// We need to look within the whole U for opt gap blocks!
		return new MonadSetStartMonadIterator(U, Sm, false);
	case kGapBlock:
		return new MonadSetStartMonadIterator(Su, Sm, true);
	case kPowerBlock:
		ASSERT_THROW(false,
			     "We should have handled Block::m_power_block higher up in the hierarchy!");
		break;
	case kObjectBlockNOTEXIST:
	case kObjectBlock:
		return m_object_block->getSMI(pEE, U, Su, Sm);
	default:
		ASSERT_THROW(false,
			     "Unknown object block type"); // In case we ever invent more labels...
		break;
	}
}



////////////////////////////////////////////////////////////
//
// ObjectReferenceUsage 
//
////////////////////////////////////////////////////////////
ObjectReferenceUsage::ObjectReferenceUsage(std::string* object_reference, 
					   std::string* feature_name)
	: m_object_reference(object_reference),
	  m_feature_name(feature_name),
	  m_controlling_object_block_node_number(0),
	  m_controlling_object_block_object(0),
	  m_ffeatures_parent(-1),
	  m_block_string2_parent(-1),
	  m_index_of_feature_in_MQLObject(0),
	  m_index_of_feature_in_MatchedObject(0),
	  m_matched_object_index(0),
	  m_bORDIsParent(false)
{
	str_tolower(*m_object_reference, m_object_reference_lower);
}

ObjectReferenceUsage::~ObjectReferenceUsage()
{
	delete m_object_reference;
	delete m_feature_name;
}

//
// - Check that object references have been declared 
//   before being used.
//
// - Check that feature on object reference usage exists 
//   for the given object type.  At the same time, store
//   the information in the symbol table that this feature
//   will be used for this object reference.
//
bool ObjectReferenceUsage::symbol(MQLExecEnv *pEE, node_number_t ffeatures_parent, bool& bResult)
{
	m_ffeatures_parent = ffeatures_parent;

	// Set m_block_string2_parent
	FFeatures *pFFeaturesParent = (FFeatures*) pEE->getNodeFromNodeNumber(m_ffeatures_parent);
	m_block_string2_parent = pFFeaturesParent->getBlockString2Parent();


	//
	// - Check that object references have been declared 
	//   before being used.
	//
	if (pEE->m_ORD2NodeNumberMap.find(m_object_reference_lower) == pEE->m_ORD2NodeNumberMap.end()) {
		pEE->pError->appendError("The object reference has not been declared in the object reference usage '" + *m_object_reference + "." + *m_feature_name + "'.\n");
		bResult = false;
		return true;
	}

	//
	// - Check that feature on object reference usage exists 
	//   for the given object type.
	node_number_t ObjectBlockNodeNumber =
		pEE->m_ORD2NodeNumberMap[m_object_reference_lower];
	ObjectBlock *pObjectBlock = (ObjectBlock*) pEE->getNodeFromNodeNumber(ObjectBlockNodeNumber);
	

	bool bFeatureExists;
	id_d_t feature_type_id;
	std::string default_value;
	bool is_computed;
	if (!pEE->pDB->featureExists(*m_feature_name,
				     pObjectBlock->getObjectTypeId(),
				     bFeatureExists,
				     feature_type_id,
				     default_value,
				     is_computed))
		return false;
	if (!bFeatureExists) {
		pEE->pError->appendError("The feature does not exist on the object type of the object reference\nin the object reference usage '" + *m_object_reference + "." + *m_feature_name + "'.\n");
		bResult = false;
		return true;
	}

	// Create feature-info
	m_feature_info = FeatureInfo(*m_feature_name, feature_type_id, default_value, is_computed);

	// If we got this far, there were no DB errors and no compiler-errors
	bResult = true;
	return true;
}


bool ObjectReferenceUsage::symbolObjectReferences2(MQLExecEnv *pEE)
{
	node_number_t ObjectBlockNodeNumber = pEE->m_ORD2NodeNumberMap[m_object_reference_lower];
	ObjectBlock *pObjectBlock = (ObjectBlock*) pEE->getNodeFromNodeNumber(ObjectBlockNodeNumber);

	if (m_bORDIsParent) {
		// Add FeatureInfo to ObjectBlock's MQLObject, 
		// if not already there
		MQLObject *pMQLObject = pObjectBlock->getObject();
		pMQLObject->addFeature(m_feature_info);
		m_index_of_feature_in_MQLObject = pMQLObject->getFeatureIndex(m_feature_info.getName());
		pObjectBlock->addFeatureToBeRetrieved(pEE, m_feature_info, m_index_of_feature_in_MatchedObject);
	} else {
		// Add Feature to the controlling ObjectBlock's
		// m_Feature_retrieval_vec, and add it to the
		// MQLObject of the ObjectBlock.  Set
		// m_index_of_feature_in_MatchedObject to be the index
		// of the feature in the ObjectBlock's
		// m_Feature_retrieval_vec.
		pObjectBlock->addFeatureToBeRetrieved(pEE, m_feature_info, m_index_of_feature_in_MatchedObject);
	}

	return true;
}


void ObjectReferenceUsage::setControllingObjectBlockNodeNumber(MQLExecEnv *pEE, node_number_t OBB_node_number)
{
	m_controlling_object_block_node_number = OBB_node_number;
	m_controlling_object_block_object = (ObjectBlock*) pEE->getNodeFromNodeNumber(OBB_node_number);
}


void ObjectReferenceUsage::determineSolutionType(MQLExecEnv *pEE)
{

	// Determines which solution to use for resolving the
        // EMdFValue of this ORU:
        //
        // a) ORDIsNotParent: Whether the ORD of this ORU is NOT on an
        //   ObjectBlock which is in the parent chain of this ORU.
        // 
        // b) ORDIsParent: Whether the ORD of this ORU IS on an
        //    ObjectBlock which is in the parent chain of this ORU.
        //   
        // The reason this is significant is that this determines how to
        // find the EMdFValue(s) corresponding to this ORU:
        // 
        // If it is (a), ORDIsNotParent, we need to traverse the Sheaf.
        // 
        // If it is (b); ORDIsParent, we can store the EMdFValue directly
        // in the MQLObject belonging to the ObjectBlock on which the ORD
        // is located.

	bool bORDIsParent = false;
	node_number_t bs2_parent = m_block_string2_parent;
	std::string ORD = m_object_reference_lower;
	
	while (!bORDIsParent && bs2_parent != -1) {
		BlockString2 *pBlockString2Node = 
			(BlockString2*) pEE->getNodeFromNodeNumber(bs2_parent);
		if (pBlockString2Node->hasImmediateORDChild(ORD)) {
			bORDIsParent = true;
		}
		bs2_parent = pBlockString2Node->getBlockString2Parent();
	}
	
	m_bORDIsParent = bORDIsParent;
}

////////////////////////////////////////////////////////////
//
// Value 
//
////////////////////////////////////////////////////////////
Value::Value(std::string* str, eValueKind kind) // for enum_const and string
	: m_emdf_value(0),
	  m_object_reference_usage(0),
	  m_enum_const(0),
	  m_integer(0),
	  m_string(0)
{
	ASSERT_THROW(kind == kValEnumConst || kind == kValString,
		     "value kind was neither enumconst nor string");
	m_kind = kind;
	switch (kind) {
	case kValEnumConst:
		m_enum_const = str;
		break;
	case kValString:
		m_string = str;
		break;
	default:
		ASSERT_THROW(false, "Unknown eValueKind");
		break;
	}
}

Value::Value(long integer)
	: m_emdf_value(0),
	  m_object_reference_usage(0),
	  m_enum_const(0),
	  m_integer(integer),
	  m_string(0),
	  m_kind(kValInteger)
{
}

Value::Value(ObjectReferenceUsage* object_reference_usage)
	: m_emdf_value(0),
	  m_object_reference_usage(object_reference_usage),
	  m_enum_const(0),
	  m_integer(0),
	  m_string(0),
	  m_kind(kValObjectReferenceUsage)
{
}

Value::~Value()
{
	delete m_enum_const;
	delete m_string;
	delete m_object_reference_usage;
	delete m_emdf_value;
}

//
// - Check that object references have been declared 
//   before being used.
//
// - Check that feature on object reference usage exists 
//   for the given object type.  At the same time, store
//   the information in the symbol table that this feature
//   will be used for this object reference.
//
bool Value::symbol(MQLExecEnv *pEE, id_d_t feature_type_id, node_number_t ffeatures_parent, bool& bResult)
{
	bool bEnumConstExists;
	bool dummy_is_default;
	switch (m_kind) {
	case kValEnumConst:
		if (!pEE->pDB->enumConstExists(*m_enum_const,
					       feature_type_id,
					       bEnumConstExists,
					       m_enum_const_value,
					       dummy_is_default))
			return false;
		if (!bEnumConstExists) {
			bResult = false;
			pEE->pError->appendError("The enumeration constant " + *m_enum_const + " does not exist\nin the enumeration of the feature.\n");
			return true;
		}

		// Now we know the enum id
		m_enum_id = GET_ENUM_FEATURE_ID_FROM_LIST_OF_ENUM_FEATURE_ID(feature_type_id);
		break;
	case kValInteger:
		break;
	case kValString:
		break;
	case kValObjectReferenceUsage:
		if (!m_object_reference_usage->symbol(pEE, ffeatures_parent, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	}

	// If we got this far, there were no DB errors and no compiler-errors
	bResult = true;
	return true;
}

const EMdFValue* Value::getEMdFValue(void)
{
	if (m_emdf_value == 0) {
		switch (m_kind) {
		case kValEnumConst:
			m_emdf_value = new EMdFValue(kEVEnum, m_enum_const_value);
			break;
		case kValInteger:
			m_emdf_value = new EMdFValue(kEVInt, m_integer);
			break;
		case kValString:
			m_emdf_value = new EMdFValue(*m_string);
			break;
		case kValObjectReferenceUsage:
			ASSERT_THROW(false,
				     "Unknown value string");
			break;
		}
	}
	return m_emdf_value;
}

EMdFValue *Value::getAsNewedEMdFValue() const
{
	// Declare result
	EMdFValue* pResult;

	// Act on m_kind
	switch (m_kind) {
	case kValEnumConst:
		pResult = new EMdFValue(kEVEnum, m_enum_const_value);
		break;
	case kValInteger:
		pResult = new EMdFValue(kEVInt, m_integer);
		break;
	case kValString:
		pResult = new EMdFValue(*m_string);
		break;
	case kValObjectReferenceUsage:
		ASSERT_THROW(false, "value kind was kValObjectReferenceUsage, which it shouldn't be");
		return 0; // Just to fool the compiler into giving no warning with -Wall
		break;
	default:
		ASSERT_THROW(false, "Unknown value kind");
		return 0; // Just to fool the compiler into giving no warning with -Wall
		break;
	}

	// Return result;
	return pResult;
}

const EMdFValue *getEMdFValueFromObjectReferenceUsage(const Value *pValueObject, NonParentORDSolution *pNonParentSolution)
{
	const ObjectReferenceUsage *pORU = pValueObject->getObjectReferenceUsageConst();	
	const EMdFValue *pValue = 0;
	if (pORU->getORDIsParent()) {
		const ObjectBlock* pObjectBlock =
			pORU->getControllingObjectBlock();
		const MQLObject* pMQLObject =
			pObjectBlock->getObject();
		pValue = pMQLObject->getFeatureValue(pORU->getFeatureIndexInMQLObject());
	} else {
		const MatchedObject *pMatchedObject =
			(*pNonParentSolution)[pORU->getMatchedObjectIndex()];
		pValue = pMatchedObject->getEMdFValue(pORU->getFeatureIndexInMatchedObject());
	}
	return pValue;
}



std::string Value::getAsString(MQLExecEnv *pEE, NonParentORDSolution *pNonParentORDSolution) const
{
	UNUSED(pEE);

	std::string result;
	const EMdFValue* pValue;
	switch (m_kind) {
	case kValEnumConst:
		result = long2string(m_enum_const_value);
		break;
	case kValInteger:
		result = long2string(m_integer);
		break;
	case kValString:
		result = *m_string;
		break;
	case kValObjectReferenceUsage:
		pValue = getEMdFValueFromObjectReferenceUsage(this, pNonParentORDSolution);
		result = pValue->toString();
		break;
	}

	// Return result;
	return result;
}

const std::string& Value::getEnumConst()
{
	ASSERT_THROW(m_kind == kValEnumConst,
		     "value kind was not enumconst");
	return *m_enum_const;
}

long Value::getInteger()
{
	ASSERT_THROW(m_kind == kValInteger,
		     "value kind was not integer");
	return m_integer;
}

const std::string& Value::getString()
{
	ASSERT_THROW(m_kind == kValString,
		     "value kind was not string");
	return *m_string;
}

ObjectReferenceUsage* Value::getObjectReferenceUsage()
{
	ASSERT_THROW(m_kind == kValObjectReferenceUsage,
		     "value kind was not object reference usage");
	return m_object_reference_usage;
}

const ObjectReferenceUsage* Value::getObjectReferenceUsageConst() const
{
	ASSERT_THROW(m_kind == kValObjectReferenceUsage,
		     "value kind was not object reference usage");
	return m_object_reference_usage;
}



////////////////////////////////////////////////////////////
//
// FeatureComparison 
//
////////////////////////////////////////////////////////////
FeatureComparison::FeatureComparison(std::string* feature_name,
				     eComparisonOp comparison_op,
				     Value* value)
	: m_feature_name(feature_name),
	  m_object_type_name(""),
	  m_object_type_id(NIL),
	  m_comparison_op(comparison_op),
	  m_value(value),
	  m_in_enum_list(0),
	  m_in_integer_list(0),
	  m_in_enum_const_cache(0),
	  m_pcre(NULL), m_pcre_extra(NULL), 
	  m_ovector(0), m_ovectorsize(0),
	  m_feature_index(-1), 
	  m_bCanBePreQueried(false),
	  m_bContextHasBeenNegative(false),
	  m_ffeatures_parent(-1)
{
}

FeatureComparison::FeatureComparison(std::string* feature_name,
				     StringListNode *in_enum_list)
	: m_feature_name(feature_name),
	  m_object_type_name(""),
	  m_object_type_id(NIL),
	  m_comparison_op(kIn),
	  m_value(0),
	  m_in_enum_list(0),
	  m_in_integer_list(0),
	  m_in_enum_const_cache(0),
	  m_pcre(NULL), m_pcre_extra(NULL), 
	  m_ovector(0), m_ovectorsize(0),
	  m_feature_index(-1), 
	  m_bCanBePreQueried(false),
	  m_bContextHasBeenNegative(false),
	  m_ffeatures_parent(-1)
{
	// Copy in_enum_list to a StringList. This is in the right order,
	// whereas in_enum_list is in reverse order (LALR-order).
	m_in_enum_list = new StringList(in_enum_list);

	// Now we can dispense with the input StringListNode "reverse" list.
	delete in_enum_list;

	// Make a new EnumConstCache
	m_in_enum_const_cache = new EnumConstCache();
}

// For feature_name IN (integer, integer, ..., integer)
FeatureComparison::FeatureComparison(std::string* feature_name,
				     IntegerListNode *in_integer_list)
	: m_feature_name(feature_name),
	  m_object_type_name(""),
	  m_object_type_id(NIL),
	  m_comparison_op(kIn),
	  m_value(0),
	  m_in_enum_list(0),
	  m_in_integer_list(0),
	  m_in_enum_const_cache(0),
	  m_pcre(NULL), m_pcre_extra(NULL), 
	  m_ovector(0), m_ovectorsize(0),
	  m_feature_index(-1), 
	  m_bCanBePreQueried(false),
	  m_bContextHasBeenNegative(false),
	  m_ffeatures_parent(-1)
{
	// Copy in_integer_list to an IntegerList. This is in the right order,
	// whereas in_integer_list is in reverse order (LALR-order).
	m_in_integer_list = new IntegerList(in_integer_list);

	m_in_integer_list_as_string = m_in_integer_list->getDelimitedString(DEFAULT_LIST_DELIMITER);

	// Now we can dispense with the input IntegerListNode "reverse" list.
	delete in_integer_list;
}



FeatureComparison::~FeatureComparison()
{
	delete m_feature_name;
	delete m_value;
	delete m_in_enum_list;
	delete m_in_integer_list;
	delete m_in_enum_const_cache;

	// Regular expressions
	if (m_pcre != NULL)
		pcre_free(m_pcre);
	if (m_pcre_extra != NULL)
		pcre_free(m_pcre_extra);
	delete[] m_ovector;
}

//
// - If this is the tilde operator, check that the value is a string.
//
// - Also check/compile regular expression
//
void FeatureComparison::weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult, node_number_t ffeatures_parent)
{
	m_ffeatures_parent = ffeatures_parent;
	if (m_comparison_op == kTilde || m_comparison_op == kNotTilde) {
		// kTilde and kNotTilde only occur with m_value, not with
		// m_in_enum_list. Hence it is safe to dereference the m_value
		// pointer here.
		if (m_value->getKind() == kValString) {
			//
			// Compile
			//
			const char *pcre_error;
			int error_offset;
			int options = 0;
			m_pcre = pcre_compile(m_value->getString().c_str(), options, &pcre_error, &error_offset, NULL);
			if (m_pcre == NULL) {
				pEE->pError->appendError(std::string("Error compiling regular expression.  PCRE reported:\n") + pcre_error + "\n");
				pEE->pError->appendError(std::string("Error occured at characterposition ") + int2string(error_offset) + " in the pattern.\n");
				bResult = false;
				return;
			}
			//
			// Study
			//
			options = 0;
			m_pcre_extra = pcre_study(m_pcre, options, &pcre_error);
			if (pcre_error != NULL) {
				pEE->pError->appendError(std::string("Error studying regular expression.  PCRE reported:\n") + pcre_error + "\n");
				bResult = false;
				return;
			}

			// 
			// Make ovector
			//
			int capture_count;
			int fullinfo_return_value;
			if ((fullinfo_return_value = pcre_fullinfo(m_pcre, m_pcre_extra, PCRE_INFO_CAPTURECOUNT, &capture_count)) != 0) {
				pEE->pError->appendError(std::string("Error getting info from regular expression.\nmql_query.cpp:FeatureComparison::weedFeatureConstraints: pcre_fullinfo returned ") + int2string(fullinfo_return_value) + "\n");
				m_ovector = 0;
				m_ovectorsize = 0;
				bResult = false;
				return;
			} else {
				m_ovectorsize = (capture_count + 1)*3;
				m_ovector = new int[m_ovectorsize];
			}

			// If we got this far, there were no compiler errors
			bResult = true;
		}
		else {
			pEE->pError->appendError("The '~' and '!~' tilde operators (regular expression matching) can only be used with strings on the right-hand side.\n");
			bResult = false;
		}
	} else {
		bResult = true;
	}

	// Check that enclosing_object_reference is not in ourselves.
	/*
        // This requirement has been lifted by the new object reference
        // machinery.
	if (bResult && m_value != 0 && m_value->getKind() == kValObjectReferenceUsage) {
		// If they compare equal, bResult is false.
		if (strcmp_nocase(enclosing_object_reference_usage, m_value->getObjectReferenceUsage()->getObjectReference()) == 0) {
			bResult = false;
			pEE->pError->appendError("The object reference usage " + enclosing_object_reference_usage + " cannot be used within the same object block as that on which it was declared.\n");
		} 
	} 
	*/
}

//
// - Check that feature_name is part of the enclosing object type.
//   In doing so, store its feature type_id in the local object.
//
// - Symbol-checking of value
//
bool FeatureComparison::symbol(MQLExecEnv *pEE, 
			       const std::string& object_type_name,
			       id_d_t enclosing_object_type_id, 
			       bool& bResult)
{
	m_object_type_name = object_type_name;
	m_object_type_id = enclosing_object_type_id;

	//
	// - Check that feature_name is part of the enclosing object type.
	//   In doing so, store its feature info in the local object.
	//
	bool bFeatureExists;
	id_d_t feature_type_id;
	std::string default_value;
	bool is_computed;
	if (!pEE->pDB->featureExists(*m_feature_name,
				     enclosing_object_type_id,
				     bFeatureExists,
				     feature_type_id,
				     default_value,
				     is_computed))
		return false;
	if (!bFeatureExists) {
		pEE->pError->appendError("Feature " + *m_feature_name + " does not exist on its enclosing object type.\n");
		bResult = false;
		return true;
	}
	m_feature_info = FeatureInfo(*m_feature_name, feature_type_id, default_value, is_computed);

	// Check that it is in the enclosing object
  
	if (m_value != 0) {
		//
		// - Symbol-checking of value
		//
		if (!m_value->symbol(pEE, feature_type_id, m_ffeatures_parent, bResult)) 
			return false; 
		if (!bResult)
			return true;
	} else if (m_in_enum_list != 0) {
		//
		// - Symbol-checking of m_in_enum_list
		//

		// Get enum name
		bool bEnumExists;
		std::string enum_name;
		if (!pEE->pDB->enumExists(feature_type_id, bEnumExists, enum_name)) {
			pEE->pError->appendError("DB error getting existence of enumeration on feature " + *m_feature_name + ".\n");
			return false;
		}
		if (!bEnumExists) {
			bResult = false;
			pEE->pError->appendError("The feature '" + *m_feature_name + "' is not an enumeration, yet the left-hand-side is a list of enumeration constants.\n");
			return true;
		}


		StringListConstIterator ci = m_in_enum_list->const_iterator();
		while (ci.hasNext()) {
			// Get next enum constant
			const std::string& cur_enum_const = ci.next();

			// Check that it exists
			bool bEnumConstExists;
			long enum_const_value;
			bool bIsDefault;
			if (!pEE->pDB->enumConstExists(cur_enum_const,
						       feature_type_id,
						       bEnumConstExists,
						       enum_const_value,
						       bIsDefault)) {
				pEE->pError->appendError("DB error verifying existence of enum const '" + cur_enum_const + "' in enum '" + enum_name + "' on feature '" + *m_feature_name + "'.\n");
				return false;
			}

			// If it did not exist, complain with compiler error.
			if (!bEnumConstExists) {
				bResult = false;
				pEE->pError->appendError("The enumeration constant " + cur_enum_const + " does not exist\nin the enumeration of the feature.\n");
				return true;
			}

			// Add the info to the enum const cache
			EnumConstInfo eci(cur_enum_const, enum_const_value, bIsDefault);
			m_in_enum_const_cache->addEci(feature_type_id, enum_name, eci);

			// Add the info to the list of enum const infos
			m_in_enum_const_info_list.push_back(eci);
		}
	} else if (m_in_integer_list != 0) {
		// That's OK...
	} else {
		// It should be either m_value or m_in_enum_list or m_in_integer_list
		ASSERT_THROW(false,
			     "Kind unknown");
	}

	// If we got this far, we had no DB errors and no compiler-errors
	bResult = true;
	return true;
}

bool FeatureComparison::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (m_value != 0 && m_value->getKind() == kValObjectReferenceUsage) {
		ObjectReferenceUsage *pORU = m_value->getObjectReferenceUsage();
		std::string ORD = pORU->getObjectReference(); // Also lower-cases it
		if (ORD_set.find(ORD) == ORD_set.end()) {
			bResult = false;
			pEE->pError->appendError("You have used the object reference '" + pORU->getObjectReference() + "' in such as way as to cross\n"
						 "an \"export barrier\". An export barrier is either an OR construct,\n"
						 "a STAR construct, or an UnorderedGroup construct.\n");
			return true;
                }

		pORU->determineSolutionType(pEE);

		node_number_t ORDObjectBlockNodeNumber = pEE->m_ORD2NodeNumberMap[ORD];

		pORU->setControllingObjectBlockNodeNumber(pEE, ORDObjectBlockNodeNumber);

		FFeatures *pFFeaturesNode = (FFeatures*) pEE->getNodeFromNodeNumber(m_ffeatures_parent);
		if (!pORU->getORDIsParent()) {
			pFFeaturesNode->addNonParentORU(pORU);
		} 

		pORU->setBlockString2Parent(pFFeaturesNode->getBlockString2Parent());
		
	}
	return true;
}


bool FeatureComparison::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (m_value != 0 && m_value->getKind() == kValObjectReferenceUsage) {
		return m_value->getObjectReferenceUsage()->symbolObjectReferences2(pEE);
	}

	return true;
}



void FeatureComparison::symbolAddFeaturesToSet(std::set<std::string>& myset) const
{
	std::string lowercase_feature_name;
	str_tolower(*m_feature_name, lowercase_feature_name);
	myset.insert(lowercase_feature_name);
}

void FeatureComparison::symbolAddFeatures(MQLObject* pObj, EMdFDB *pDB)
{
	UNUSED(pDB);

	// It used to be that we did not add to inst if certain
	// conditions were met, namely that we had not encountered a
	// negative context, and bCanBePreQueried was true.
	//
	// This was used as an optimization in R_feature_comparison,
	// where we assumed that the result was true, since it had
	// already been checked in the backend.
	//
	// However, this caused erronerous results with this query on
	// the bhs4 database:
	//
	// /* false OR false = true?? */
	//
	// select all objects where
	// 
	// [verse label = " ICHR05,23"
	//    [clause
	//       [phrase function = Subj
	// 	 [word as w1 get nu, gn]
	//       ]
	//       [phrase function = Pred
	// 	 [word
	// 	    (nu <> unknown AND nu <> w1.nu) OR
	// 	    (gn <> unknown AND gn <> w1.gn)
	// 	    get nu, gn
	// 	 ]
	//       ]
	//    ]
	// ]
	//
	// The issue here is that the second part of each AND could
	// not be checked in the backend, reducing the pre-querying
	// to:
	//
	// (nu <> unknown) OR (gn <> unknown).
	// 
	// This, of course, could be true even though either nu ==
	// unknown or gn == unknown, in the case in which the other
	// was <> unknown.  So, we had not checked it in the backend,
	// and we needed to check it in the R_feature_comparison
	// anyway.
	//
	// So, the two conditions were not sufficient to establish
	// that we could skip the comparison in R_feature_comparison.
	//
	// Until we find a better criterion, we will therefore always
	// check in R_feature_comparison.
	//

	pObj->addFeature(m_feature_info);
	m_feature_index = pObj->getFeatureIndex(*m_feature_name);
}

bool FeatureComparison::symbolSetFeatureIndex(const std::vector<std::string>& feature_name_vec, bool& bResult)
{
	for (unsigned int i = 0; 
	     i < feature_name_vec.size(); ++i) {
		std::string feature_name = feature_name_vec[i];

		if (strcmp_nocase(feature_name, m_feature_info.getName()) == 0) {
			m_feature_index = i;
			bResult = true;
			return true;
		}
	}
	// This should not happen....
	m_feature_index = -1;
	bResult = false;
	return true;
}


bool FeatureComparison::type(MQLExecEnv *pEE, bool& bResult)
{
	// Assume everything went well
	bResult = true;

	// Get feature type
	id_d_t ft = m_feature_info.getType();

	// Only do stuff to m_value if we are doing a non-IN comparison
	if (m_value != 0) {
		const ObjectReferenceUsage *pORU = 0;
		// What is value's kind?
		switch (m_value->getKind()) {
		case kValEnumConst:
			if (featureTypeIdIsListOf(ft)) {
				if (m_comparison_op != kHas) {
					bResult = false;
					pEE->pError->appendError("The list feature " + *m_feature_name + " is being compared with an enumeration constant, but the comparison operator is not HAS, as it should be.\n");
				} else {
					bResult = featureTypeIdIsListOfENUM(ft);					
					if (!bResult) {
						pEE->pError->appendError("The list feature " + *m_feature_name + " is being compared with a enumeration constant using the HAS operator, but the list is not a list of enum constants.\n");
					}
				}
			} 
			if (bResult) {
				bResult = STRIP_ENUM_ID_OF_LOWER_BITS(ft) == STRIP_ENUM_ID_OF_LOWER_BITS(m_value->getEnumId());
				if (!bResult) {
					pEE->pError->appendError("The feature " + *m_feature_name + " is being compared with an enumeration constant.  The enumeration constant does not belong to the enumeration which is the type of the feature.\n");
				}
			}
			break;
		case kValInteger:
			if (featureTypeIdIsListOf(ft)) {
				if (m_comparison_op != kHas) {
					bResult = false;
					pEE->pError->appendError("The list feature " + *m_feature_name + " is being compared with an integer, but the comparison operator is not HAS, as it should be.\n");
				} else {
					bResult = featureTypeIdIsListOfINTEGER(ft) 
						|| featureTypeIdIsListOfID_D(ft);
					if (!bResult) {
						pEE->pError->appendError("The list feature " + *m_feature_name + " is being compared with a integer using the HAS operator, but the list is neither a list of integers, nor a list of id_ds.\n");
					}
				}
			} else {
				// !Listof(ft)
				bResult = featureTypeIdIsINTEGER(ft) || featureTypeIdIsID_D(ft);
				if (!bResult) {
					pEE->pError->appendError("The feature " + *m_feature_name + " is being compared with an integer.  The feature is neither an integer nor an id_d.\n");
				}
			}
			break;
		case kValString:
			bResult = featureTypeIdIsSTRING(ft)
				|| featureTypeIdIsASCII(ft);
			if (!bResult) {
				pEE->pError->appendError("The feature " + *m_feature_name + " is being compared with a string.  The feature is not a string.\n");
			}
			break;
		case kValObjectReferenceUsage:
			pORU = m_value->getObjectReferenceUsage();
			id_d_t oru_ft = pORU->getFeatureTypeId();
			if (m_comparison_op == kHas) {
				if (featureTypeIdIsListOf(ft)
				    && !featureTypeIdIsListOf(oru_ft)) {
					if (featureTypeIdIsListOfINTEGER(ft)) {
						bResult = featureTypeIdIsINTEGER(oru_ft);
						if (!bResult) {
							pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing HAS and an object reference usage. The feature is a list of integer, but\nthe object reference usage is not an integer.\n");
						}
					} else if (featureTypeIdIsListOfID_D(ft)) {
						bResult = featureTypeIdIsID_D(oru_ft);
						if (!bResult) {
							pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing HAS and an object reference usage. The feature is a list of id_d, but\nthe object reference usage is not an id_d.\n");
						}
					} else if (featureTypeIdIsListOfENUM(ft)) {
						if (featureTypeIdIsENUM(oru_ft)) {
							if (STRIP_ENUM_ID_OF_LOWER_BITS(ft) == STRIP_ENUM_ID_OF_LOWER_BITS(oru_ft)) {
								bResult = true;
							} else {
								bResult = false;
								pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing HAS and an object reference usage. The feature is a list of enum\nconstants, but the object reference usage is not the same enum.\n");
							}
						} else {
							bResult = false;
							pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing HAS and an object reference usage. The feature is a list of enum\nconstants, but the object reference usage is not an enum.\n");
	      
						}
					}
				} else {
					bResult = false;
					pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing HAS and an object reference usage.\n");
					if (featureTypeIdIsListOf(ft)) {
						pEE->pError->appendError("The feature is a list, but so is the object reference usage.\nThe HAS operator is only for comparing lists with atomic values.\n");
					} else {
						pEE->pError->appendError("The feature is not a list.\nThe HAS operator is only for comparing lists with atomic values.\n");
					}
				}
			} else if (m_comparison_op == kIn) {
				if (!featureTypeIdIsListOf(ft)
				    && featureTypeIdIsListOf(oru_ft)) {
					if (featureTypeIdIsINTEGER(ft)) {
						bResult = featureTypeIdIsListOfINTEGER(oru_ft);
						if (!bResult) {
							pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing IN and an object reference usage. The feature is an integer, but\nthe object reference usage is not a list of integer.\n");
						}
					} else if (featureTypeIdIsID_D(ft)) {
						bResult = featureTypeIdIsListOfID_D(oru_ft);
						if (!bResult) {
							pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing IN and an object reference usage. The feature is an id_d, but\nthe object reference usage is not a list of id_d.\n");
						}
					} else if (featureTypeIdIsENUM(ft)) {
						if (featureTypeIdIsListOfENUM(oru_ft)) {
							if (STRIP_ENUM_ID_OF_LOWER_BITS(ft) == STRIP_ENUM_ID_OF_LOWER_BITS(oru_ft)) {
								bResult = true;
							} else {
								bResult = false;
								pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing IN and an object reference usage. The feature is an enum,\n but the object reference usage is not the same enum.\n");
							}
						} else {
							bResult = false;
							pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing IN and an object reference usage. The feature is an enum,\nbut the object reference usage is not a list of enum.\n");
						}
					}
				} else {
					bResult = false;
					pEE->pError->appendError("The feature " + *m_feature_name + " is being compared\nusing IN and an object reference usage.\n");
					if (featureTypeIdIsListOf(oru_ft)) {
						pEE->pError->appendError("The object reference usage is a list, but so is the feature.\nThe IN operator is only for comparing atomic values with lists.\n");
					} else {
						pEE->pError->appendError("The object reference usage is not a list.\nThe IN operator is only for comparing atomic values with lists.\n");
					}
				}
			} else {
				if (featureTypeIdIsINTEGER(ft) && featureTypeIdIsINTEGER(oru_ft)) {
					bResult = true;
				} else if (featureTypeIdIsID_D(ft) && featureTypeIdIsID_D(oru_ft)) {
					bResult = true;
				} else if (featureTypeIdIsASCIIorSTRING(ft) && featureTypeIdIsASCIIorSTRING(oru_ft)) {
					bResult = true;
				} else if (featureTypeIdIsENUM(ft) && featureTypeIdIsENUM(oru_ft)) {
					if (STRIP_ENUM_ID_OF_LOWER_BITS(ft) == STRIP_ENUM_ID_OF_LOWER_BITS(oru_ft)) {
						bResult = true;
					} else {
						bResult = false;
					}
				} else if (featureTypeIdIsListOfINTEGER(ft) && featureTypeIdIsListOfINTEGER(oru_ft)) {
					bResult = true;
				} else if (featureTypeIdIsListOfID_D(ft) && featureTypeIdIsListOfID_D(oru_ft)) {
					bResult = true;
				} else if (featureTypeIdIsListOfENUM(ft) && featureTypeIdIsListOfENUM(oru_ft)) {
					if (GET_ENUM_FEATURE_ID_FROM_LIST_OF_ENUM_FEATURE_ID(ft) == GET_ENUM_FEATURE_ID_FROM_LIST_OF_ENUM_FEATURE_ID(oru_ft)) {
						bResult = true;
					} else {
						bResult = false;
					}
				} else if (featureTypeIdIsSOM(ft) && featureTypeIdIsSOM(oru_ft)) {
					bResult = m_comparison_op == kEqual;
					if (!bResult) {
						pEE->pError->appendError("The feature " + *m_feature_name + " is being compared with an object reference usage. Both are sets of monads, so you must use = as the comparison operator.\n");
						
					}
				} else {
					bResult = false;
				}
				if (!bResult) {
					pEE->pError->appendError("The feature " + *m_feature_name + " is being compared with an object reference usage whose feature is of a different type.\n");
				} else {
					if (featureTypeIdIsListOf(oru_ft)
					    && featureTypeIdIsListOf(ft)) {
						bResult = m_comparison_op == kEqual;
						if (!bResult) {
							pEE->pError->appendError("The feature " + *m_feature_name + " is being compared with an object reference usage. Both are lists, so you must use either IN, HAS, or = as the comparison operator.\n");

						}
					}
				}
			}
			break;
		}
	} else if (m_in_enum_list != 0) {
		// Nothing to do: symbol has already checked that the enum exists.    
	} else if (m_in_integer_list != 0) {
		if (featureTypeIdIsINTEGER(ft)
		    || featureTypeIdIsID_D(ft)) {
			bResult = true;
		} else {
			bResult = false;
		}
		if (!bResult) {
			pEE->pError->appendError("The feature " + *m_feature_name + " is being compared with a list of integers.  The feature is neither of type INTEGER nor of type ID_D.\n");
		}
	} else {
		// It should be either m_in_enum_list or m_in_integer_list or m_value.
		ASSERT_THROW(false,
			     "Kind unknown");
	}

	// If we got this far, there were no DB errors
	return true;
}

bool FeatureComparison::exec(MQLExecEnv *pEE, const InstObject *pInstObj)
{
	NonParentORDSolution *pNonParentORDSolution = 0;
	const EMdFValue *left_value = pInstObj->getFeature(m_feature_index);
	return this->compare(pEE, left_value, pNonParentORDSolution);
}

void FeatureComparison::pretty(std::ostream *pOut, int indent, NonParentORDSolution *pNonParentORDSolution)
{
	print_indent(pOut, indent);
	(*pOut) << "FeatureComparison {\n";
	print_indent(pOut, indent+1);
	(*pOut) << "feature_name = '" << *m_feature_name << "' ";
	(*pOut) << getStringFromeComparisonOp(m_comparison_op) << " ";
	if (m_value != 0) {
		(*pOut) << m_value->getAsString(0, pNonParentORDSolution) << '\n';
	} else if (m_in_enum_list != 0) {
		(*pOut) << "(";
		std::list<EnumConstInfo>::const_iterator ci = m_in_enum_const_info_list.begin();
		while (ci != m_in_enum_const_info_list.end()) {
			EnumConstInfo eci = *ci;

			std::string enum_const_name = eci.getName();

			(*pOut) << enum_const_name;

			++ci;

			if (ci != m_in_enum_const_info_list.end()) {
				(*pOut) << ", ";
			}
		}
		(*pOut) << ")\n";
	} else if (m_in_integer_list != 0) {
		(*pOut) << m_in_integer_list_as_string << "\n";
		// That's OK...
	} else {
		// It should be either m_value or m_in_enum_list or m_in_integer_list
		ASSERT_THROW(false,
			     "Kind unknown");
	}
	print_indent(pOut, indent);
	(*pOut) << "} FeatureComparison\n";
}

bool FeatureComparison::hasORU()
{
	bool bResult = false;
	if (m_value != 0) {
		switch (m_value->getKind()) {
		case kValObjectReferenceUsage:
			bResult = true;
			break;
		default:
			bResult = false;
			break;
		}
	}
	return bResult;
}

bool FeatureComparison::compareObjectReferenceUsage(const EMdFValue* left_value, NonParentORDSolution *pNonParentORDSolution)
{	
	const EMdFValue *pRightValue = getEMdFValueFromObjectReferenceUsage(m_value, pNonParentORDSolution);
	ASSERT_THROW(pRightValue != 0,
		     "pRightValue was 0!");
	return left_value->compare(*pRightValue, m_comparison_op);
}





bool FeatureComparison::compare(MQLExecEnv *pEE, const EMdFValue *left_value, NonParentORDSolution *pNonParentORDSolution)
{
        // left_value is an EMdFValue 

        // pNonParentSolution is for resolving those Object Reference
        // Usages whose Object Reference Declarations are not parents
        // of themselves, but siblings, cousins, uncles, nephews, etc.
        
        // pNonParentSolution is a list of EMdFValue.  We have been told
        // which index to use in the symbol stage.

	// Compare
	bool bResult;
	int pcre_result;
	int start_offset = 0;
	std::string right_value;
	std::string left_value_str;
	const EMdFValue *pRightValue = 0;
	switch (m_comparison_op) {
	case kIn:
		if (m_in_enum_list != 0) {
			// It was an explicit enum-list
			// symbol has already checked that left-hand-side is an enum
			// with the correct type id.
			if (m_in_enum_const_cache->find(m_feature_info.getType(), 
							left_value->getEnum()) 
			    != 0) {
				bResult = true;
			} else {
				bResult = false;
			}
		} else if (m_in_integer_list != 0) {
			std::string string_to_look_for = DEFAULT_LIST_DELIMITER + left_value->toString() + DEFAULT_LIST_DELIMITER;
			if (m_in_integer_list_as_string.find(string_to_look_for) == std::string::npos) {
				bResult = false;
			} else {
				bResult = true;
			}
		} else if (m_value->getKind() == kValObjectReferenceUsage) {
			bResult = compareObjectReferenceUsage(left_value, pNonParentORDSolution);
		} else {
			// It was neither an explicit enum-list
			// nor an explicit integer-list: So
			// we don't know how to solve it...
			ASSERT_THROW(false,
				     "It was neither an explicit enum-list, nor an explicit integer-list, so we don't know how to solve it");
		}
		break;
	case kHas:
	case kEqual:
	case kLessThan:
	case kGreaterThan:
	case kNotEqual:
	case kLessThanOrEqual:
	case kGreaterThanOrEqual:
		switch (m_value->getKind()) {
		case kValEnumConst:
			pRightValue = m_value->getEMdFValue();
			bResult = left_value->compare(*pRightValue, m_comparison_op);
			break;
		case kValInteger:
		case kValString:
			pRightValue = m_value->getEMdFValue();
			bResult = left_value->compare(*pRightValue, m_comparison_op);
			break;
		case kValObjectReferenceUsage:

			bResult = compareObjectReferenceUsage(left_value, pNonParentORDSolution);
			break;
		default:
			ASSERT_THROW(false,
				     "Unknown eValueKind");
			break;
		}
		break;
	case kTilde:
		right_value = m_value->getAsString(pEE, pNonParentORDSolution);
		left_value_str = left_value->getString();
		pcre_result = pcre_exec(m_pcre, m_pcre_extra, left_value_str.c_str(), left_value_str.length(), start_offset, 0, m_ovector, m_ovectorsize);
		if (pcre_result <= 0) {
			bResult = false;
		} else {
			bResult = true;
		}
		break;
	case kNotTilde:
		right_value = m_value->getAsString(pEE, pNonParentORDSolution);
		left_value_str = left_value->getString();
		pcre_result = pcre_exec(m_pcre, m_pcre_extra, left_value_str.c_str(), left_value_str.length(), start_offset, 0, m_ovector, m_ovectorsize);
		if (pcre_result <= 0) {
			bResult = true;
		} else {
			bResult = false;
		}
		break;
	default:
		bResult = false; // Just to fool the compiler into giving no warning with -W
		ASSERT_THROW(false,
			     "Unknown operator kind");
	}

	// Return result
	return bResult;
}


EMdFComparison *FeatureComparison::makeConstraints(EMdFDB *pDB, bool bContextHasBeenNegative)
{
	// Declare the thing we'll return.
	EMdFComparison *pComparison = 0;

	// Assume the worst...
	m_bCanBePreQueried = false;

	m_bContextHasBeenNegative = bContextHasBeenNegative;


	// Check for self feature
	bool bIsSelf = is_self(*m_feature_name);

	// Feature must be stored, but self is OK, even though it is computed
	if (!bIsSelf && m_feature_info.getIsComputed()) {
		return 0;
	}
  
	// Get eEVkind for left-hand side
	id_d_t type_id_d = m_feature_info.getType();
	eEVkind EVkind = pDB->getEVkindFromTypeID_D(type_id_d);

	// Comparison operator must have a counterpart in SQL
	if (!pDB->canDoComparisonOp(m_comparison_op, EVkind)) {
		return 0;
	}

	if (m_value != 0) {
		// Value must be atomic, not an object reference
		if (m_value->getKind() == kValObjectReferenceUsage) {
			return 0;
		}

		// Calculate right-hand-side
		// We aren't doing object references, therefore this is 
		// allowed to ASSERT_THROW(false, "...") if it is an 
		// object reference.
		EMdFValue *pRight_hand_side = m_value->getAsNewedEMdFValue();


		// If we got this far, we can return the feature comparison
		pComparison = pDB->getEMdFComparison(*m_feature_name, 
						     type_id_d, 
						     m_object_type_name,
						     m_object_type_id,
						     m_comparison_op, 
						     pRight_hand_side);
	} else if (m_in_enum_list != 0) {
		// If we got this far, we can return the feature comparison
		pComparison = pDB->getEMdFComparison(*m_feature_name, type_id_d, 
						     m_object_type_name, m_object_type_id,
						     m_in_enum_const_info_list);
	} else if (m_in_integer_list != 0) {
		// If we got this far, we can return the feature comparison
		pComparison = pDB->getEMdFComparison(*m_feature_name, type_id_d, 
						     m_object_type_name, m_object_type_id,
						     m_in_integer_list);
	} else {
		// It should be either m_value or m_in_enum_list
		ASSERT_THROW(false,
			     "Unknown kind"); 
	}

	// If we got this far, pComparison has an acceptable value
	m_bCanBePreQueried = true;

	return pComparison;
}




////////////////////////////////////////////////////////////
//
// FFactor 
//
////////////////////////////////////////////////////////////
FFactor::FFactor(FFactor* ffactor)
	: m_ffactor(ffactor),
	  m_ffeatures(0),
	  m_feature_comparison(0)
{
}

FFactor::FFactor(FFeatures* ffeatures)
	: m_ffactor(0),
	  m_ffeatures(ffeatures),
	  m_feature_comparison(0)
{
}

FFactor::FFactor(FeatureComparison* feature_comparison)
	: m_ffactor(0),
	  m_ffeatures(0),
	  m_feature_comparison(feature_comparison)
{
}

FFactor::~FFactor()
{
	delete m_ffactor;
	delete m_ffeatures;
	delete m_feature_comparison;
}

void FFactor::weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult, node_number_t ffeatures_parent)
{
	if (isNOT()) {
		m_ffactor->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
	} else if (isParenthesis()) {
		m_ffeatures->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
	} else { // is_feature_comparison
		m_feature_comparison->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
	}
}

void FFactor::resetCanBePreQueried(void)
{
	if (isNOT()) {
		m_ffactor->resetCanBePreQueried();
	} else if (isParenthesis()) {
		m_ffeatures->resetCanBePreQueried();
	} else { // is_feature_comparison
		m_feature_comparison->resetCanBePreQueried();
	}
}

bool FFactor::hasORU() const
{
	if (isNOT()) {
		return m_ffactor->hasORU();
	} else if (isParenthesis()) {
		return m_ffeatures->hasORU();
	} else { // is_feature_comparison
		return m_feature_comparison->hasORU();
	}
}



bool FFactor::symbol(MQLExecEnv *pEE, 
		     const std::string& object_type_name,
		     id_d_t enclosing_object_type_id, 
		     bool& bResult)
{
	if (isNOT()) {
		return m_ffactor->symbol(pEE, 
					 object_type_name,
					 enclosing_object_type_id, 
					 bResult);
	} else if (isParenthesis()) {
		return m_ffeatures->symbol(pEE, 
					   object_type_name,
					   enclosing_object_type_id, 
					   bResult);
	} else { // is_feature_comparison
		return m_feature_comparison->symbol(pEE, 
						    object_type_name,
						    enclosing_object_type_id, 
						    bResult);
	}
}

bool FFactor::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (isNOT()) {
		return m_ffactor->symbolObjectReferences(pEE, bResult, ORD_set);
	} else if (isParenthesis()) {
		return m_ffeatures->symbolObjectReferences(pEE, bResult, ORD_set);
	} else { // is_feature_comparison
		return m_feature_comparison->symbolObjectReferences(pEE, bResult, ORD_set);
	}
}



bool FFactor::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (isNOT()) {
		return m_ffactor->symbolObjectReferences2(pEE);
	} else if (isParenthesis()) {
		return m_ffeatures->symbolObjectReferences2(pEE);
	} else { // is_feature_comparison
		return m_feature_comparison->symbolObjectReferences2(pEE);
	}
}



bool FFactor::symbolSetFeatureIndex(const std::vector<std::string>& feature_name_vec, bool& bResult)
{
	if (isNOT()) {
		return m_ffactor->symbolSetFeatureIndex(feature_name_vec, bResult);
	} else if (isParenthesis()) {
		return m_ffeatures->symbolSetFeatureIndex(feature_name_vec, bResult);
	} else { // is_feature_comparison
		return m_feature_comparison->symbolSetFeatureIndex(feature_name_vec, bResult);
	}
}
	      
void FFactor::symbolAddFeatures(MQLObject* pObj, EMdFDB *pDB)
{
	if (isNOT()) {
		m_ffactor->symbolAddFeatures(pObj, pDB);
	} else if (isParenthesis()) {
		m_ffeatures->symbolAddFeatures(pObj, pDB);
	} else { // is_feature_comparison
		m_feature_comparison->symbolAddFeatures(pObj, pDB);
	}
}

void FFactor::symbolAddFeaturesToSet(std::set<std::string>& myset) const
{
	if (isNOT()) {
		m_ffactor->symbolAddFeaturesToSet(myset);
	} else if (isParenthesis()) {
		m_ffeatures->symbolAddFeaturesToSet(myset);
	} else { // is_feature_comparison
		m_feature_comparison->symbolAddFeaturesToSet(myset);
	}
}

bool FFactor::type(MQLExecEnv *pEE, bool& bResult)
{
	if (isNOT()) {
		return m_ffactor->type(pEE, bResult);
	} else if (isParenthesis()) {
		return m_ffeatures->type(pEE, bResult);
	} else { // is_feature_comparison
		return m_feature_comparison->type(pEE, bResult);
	}
}

bool FFactor::exec(MQLExecEnv *pEE, const InstObject *pInstObj)
{
	if (isNOT()) {
		return !m_ffactor->exec(pEE, pInstObj);
	} else if (isParenthesis()) {
		return m_ffeatures->exec(pEE, pInstObj);
	} else { // is_feature_comparison
		return m_feature_comparison->exec(pEE, pInstObj);
	}
}

void FFactor::pretty(std::ostream *pOut, int indent, NonParentORDSolution *pNonParentORDSolution)
{
	print_indent(pOut, indent);
	(*pOut) << "FFactor {\n";
	if (isNOT()) {
		print_indent(pOut, indent);
		(*pOut) << "NOT {\n";
		m_ffactor->pretty(pOut, indent+1, pNonParentORDSolution);
		print_indent(pOut, indent);
		(*pOut) << "} NOT\n";
	} else if (isParenthesis()) {
		print_indent(pOut, indent);
		(*pOut) << "(PARENS {\n";
		m_ffeatures->pretty(pOut, indent+1, pNonParentORDSolution);
		print_indent(pOut, indent);
		(*pOut) << "}PARENS)\n";
	} else { // is_feature_comparison
		m_feature_comparison->pretty(pOut, indent+1, pNonParentORDSolution);
	}
	print_indent(pOut, indent);
	(*pOut) << "} FFactor\n";
}


EMdFFFactor *FFactor::makeConstraints(EMdFDB *pDB, bool bContextIsPositive, bool bContextHasBeenNegative) const
{
	if (isNOT()) {
		EMdFFFactor *pFFactor = m_ffactor->makeConstraints(pDB, !bContextIsPositive, true);
		if (pFFactor == 0) {
			return 0;
		} else {
			return pDB->getEMdFFFactor(pFFactor);
		}
	} else if (isParenthesis()) {
		EMdFFFeatures *ffeatures = m_ffeatures->makeConstraints(pDB, bContextIsPositive, bContextHasBeenNegative);
		if (ffeatures == 0) {
			return 0;
		} else {
			return pDB->getEMdFFFactor(ffeatures);
		}
	} else { // is_feature_comparison
		EMdFComparison *pComparison = m_feature_comparison->makeConstraints(pDB, bContextHasBeenNegative);
		if (pComparison == 0) {
			return 0;
		} else {
			return pDB->getEMdFFFactor(pComparison);
		}
	}
}


////////////////////////////////////////////////////////////
//
// FTerm 
//
////////////////////////////////////////////////////////////
FTerm::FTerm(FFactor* ffactor)
	: m_ffactor(ffactor),
	  m_fterm(0)
{
}

FTerm::FTerm(FTerm* fterm, FFactor* ffactor)
	: m_ffactor(ffactor),
	  m_fterm(fterm)
{
}

FTerm::~FTerm()
{
	delete m_ffactor;
	delete m_fterm;
}

void FTerm::weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult, node_number_t ffeatures_parent)
{
	if (isFFactor()) {
		m_ffactor->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
	} else {
		m_fterm->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
		if (!bResult)
			return;
		m_ffactor->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
	}
}

void FTerm::resetCanBePreQueried()
{
	if (isFFactor()) {
		m_ffactor->resetCanBePreQueried();
	} else {
		m_fterm->resetCanBePreQueried();
		m_ffactor->resetCanBePreQueried();
	}
}

bool FTerm::hasORU() const
{
	if (isFFactor()) {
		return m_ffactor->hasORU();
	} else {
		return m_fterm->hasORU()
			|| m_ffactor->hasORU();
	}
}

bool FTerm::symbol(MQLExecEnv *pEE, 
		   const std::string& object_type_name,
		   id_d_t enclosing_object_type_id, 
		   bool& bResult)
{
	if (isFFactor()) {
		return m_ffactor->symbol(pEE, 
					 object_type_name,
					 enclosing_object_type_id, 
					 bResult);
	} else {
		if (!m_fterm->symbol(pEE, 
				     object_type_name,
				     enclosing_object_type_id, 
				     bResult))
			return false;
		if (!bResult)
			return true;
		return m_ffactor->symbol(pEE, 
					 object_type_name,
					 enclosing_object_type_id, 
					 bResult);
	}
}

bool FTerm::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (isFFactor()) {
		return m_ffactor->symbolObjectReferences(pEE, bResult, ORD_set);
	} else {
		if (!m_fterm->symbolObjectReferences(pEE, bResult, ORD_set)) 
			return false;
		if (!bResult)
			return true;
		return m_ffactor->symbolObjectReferences(pEE, bResult, ORD_set);
	}

}


bool FTerm::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (isFFactor()) {
		return m_ffactor->symbolObjectReferences2(pEE);
	} else {
		if (!m_fterm->symbolObjectReferences2(pEE))
			return false;
		return m_ffactor->symbolObjectReferences2(pEE);
	}

}


void FTerm::symbolAddFeatures(MQLObject* pObj, EMdFDB *pDB)
{
	if (isFFactor()) {
		m_ffactor->symbolAddFeatures(pObj, pDB);
	} else {
		m_fterm->symbolAddFeatures(pObj, pDB);
		m_ffactor->symbolAddFeatures(pObj, pDB);
	}
}
	
bool FTerm::symbolSetFeatureIndex(const std::vector<std::string>& feature_name_vec, bool& bResult)
{
	if (isFFactor()) {
		return m_ffactor->symbolSetFeatureIndex(feature_name_vec, bResult);
	} else {
		if (!m_fterm->symbolSetFeatureIndex(feature_name_vec, bResult)) {
			return false;
		}
		if (!bResult) {
			return true;
		}
		return m_ffactor->symbolSetFeatureIndex(feature_name_vec, bResult);
	}
}

void FTerm::symbolAddFeaturesToSet(std::set<std::string>& myset) const
{
	if (isFFactor()) {
		m_ffactor->symbolAddFeaturesToSet(myset);
	} else {
		m_fterm->symbolAddFeaturesToSet(myset);
		m_ffactor->symbolAddFeaturesToSet(myset);
	}
}

bool FTerm::type(MQLExecEnv *pEE, bool& bResult)
{
	if (isFFactor()) {
		return m_ffactor->type(pEE, bResult);
	} else {
		if (!m_fterm->type(pEE, bResult))
			return false;
		if (!bResult)
			return true;
		return m_ffactor->type(pEE, bResult);
	}
}

bool FTerm::exec(MQLExecEnv *pEE, const InstObject *pInstObj)
{
	if (isFFactor()) {
		// fterm ::= ffactor
		return m_ffactor->exec(pEE, pInstObj);
	} else {
		// fterm ::= fterm "AND" ffactor
		if (!m_fterm->exec(pEE, pInstObj))
			return false;
		
		return m_ffactor->exec(pEE, pInstObj);
	}
}

void FTerm::pretty(std::ostream *pOut, int indent, NonParentORDSolution *pNonParentORDSolution)
{
	print_indent(pOut, indent);
	(*pOut) << "FTerm {\n";
	if (isFFactor()) {
		m_ffactor->pretty(pOut, indent+1, pNonParentORDSolution);
	} else {
		m_fterm->pretty(pOut, indent+1, pNonParentORDSolution);
		print_indent(pOut, indent);
		(*pOut) << "AND\n";
		m_ffactor->pretty(pOut, indent+1, pNonParentORDSolution);
	}	
	print_indent(pOut, indent);
	(*pOut) << "} FTerm\n";
}



EMdFFTerm *FTerm::makeConstraints(EMdFDB *pDB, bool bContextIsPositive, bool bContextHasBeenNegative) const
{
	if (isFFactor()) {
		EMdFFFactor *pFFactor = m_ffactor->makeConstraints(pDB, bContextIsPositive, bContextHasBeenNegative);
		if (pFFactor == 0) {
			return 0;
		} else {
			return pDB->getEMdFFTerm(0, pFFactor);
		}
	} else {
		EMdFFTerm *A;
		EMdFFFactor *B;
		A = m_fterm->makeConstraints(pDB, bContextIsPositive, bContextHasBeenNegative);
		B = m_ffactor->makeConstraints(pDB, bContextIsPositive, bContextHasBeenNegative);
		if (!bContextIsPositive) {
			// In a negative context, AND works like OR.  Therefore,
			// if even one feature cannot be prequeried, neither 
			// must be prequeried. This is because it would miss
			// an extension to the query.
			if (A == 0 || B == 0) {
				m_fterm->resetCanBePreQueried();
				delete A;

				m_ffactor->resetCanBePreQueried();
				delete B;

				return 0;
			} else {
				return pDB->getEMdFFTerm(A, B);
			}
		} else { // bContextIsPositive == true
			// In a positive context, it does no harm to leave one out. 
			// This is because taking one away from an AND does not restrict 
			// but rather enlarge the output from the SQL engine, and hence
			// it doesn't matter; the non-prequeriable feature
			// can be checked in the backend.
			if (A == 0 && B == 0) {
				m_fterm->resetCanBePreQueried();
				m_ffactor->resetCanBePreQueried();
				return 0;
			} else if (A == 0 && B != 0) {
				m_fterm->resetCanBePreQueried();
				return pDB->getEMdFFTerm(0, B);
			} else if (A != 0 && B == 0) {
				m_ffactor->resetCanBePreQueried();
				return A;
			} else {
				return pDB->getEMdFFTerm(A, B);
			}
		}
	}
}


////////////////////////////////////////////////////////////
//
// FFeatures 
//
////////////////////////////////////////////////////////////
FFeatures::FFeatures(FTerm* fterm)
	: QueryNode(kQNKFFeatures),
	  m_fterm(fterm),
	  m_ffeatures(0),
	  m_constraints(0),
	  m_block_string2_parent(-1)
{
}

FFeatures::FFeatures(FFeatures* ffeatures, FTerm* fterm)
	: QueryNode(kQNKFFeatures),
	  m_fterm(fterm),
	  m_ffeatures(ffeatures),
	  m_constraints(0),
	  m_block_string2_parent(-1)
{
}

FFeatures::~FFeatures()
{
	delete m_fterm;
	delete m_ffeatures;
	delete m_constraints;
}

void FFeatures::weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult, node_number_t ffeatures_parent)
{
	// ffeatures_parent is set in the ObjectBlock which calls this
	// method.  We deliberately do not pass our own
	// this->m_node_number, for two reasons:
	//
	// - First, we only want the top-level FFeatures to be used,
	//   since the Object Reference Usage needs to tell only the
	//   top-level FFeatures that it wants the sheaf to be
	//   searched for a non-parent ORD's EMdFValues.
	//
	// - Second, not all FFeatures get a node number at all: Only
	//   the top-level one does.  This is assigned in
	//   ObjectBlock::weedFeatureConstraints().
	//
	if (isFTerm()) {
		m_fterm->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
	} else {
		m_ffeatures->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
		if (!bResult)
			return;
		m_fterm->weedFeatureConstraints(pEE, bResult, ffeatures_parent);
	}
}


void FFeatures::addNonParentORU(ObjectReferenceUsage *pORU)
{
	m_NonParentORU_vector.push_back(pORU);
	node_number_t OBBNodeNumber = pORU->getControllingObjectBlockNodeNumber();
	m_OBBNodeNumbersToUseForNonParentORDs.insert(OBBNodeNumber);
}

void FFeatures::resetCanBePreQueried()
{
	if (isFTerm()) {
		m_fterm->resetCanBePreQueried();
	} else {
		m_ffeatures->resetCanBePreQueried();
		m_fterm->resetCanBePreQueried();
	}
}


bool FFeatures::hasORU() const
{
	if (isFTerm()) {
		return m_fterm->hasORU();
	} else {
		return m_ffeatures->hasORU()
			|| m_fterm->hasORU();
	}
}

bool FFeatures::symbol(MQLExecEnv *pEE, 
		       const std::string& object_type_name,
		       id_d_t enclosing_object_type_id, 
		       bool& bResult)
{
	if (isFTerm()) {
		return m_fterm->symbol(pEE, 
				       object_type_name,
				       enclosing_object_type_id, 
				       bResult);
	} else {
		if (!m_ffeatures->symbol(pEE, 
					 object_type_name,
					 enclosing_object_type_id, 
					 bResult))
			return false;
		if (!bResult)
			return true;
		return m_fterm->symbol(pEE, 
				       object_type_name,
				       enclosing_object_type_id, 
				       bResult);
	}
}

bool FFeatures::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (isFTerm()) {
		return m_fterm->symbolObjectReferences(pEE, bResult, ORD_set);
	} else {
		if (!m_ffeatures->symbolObjectReferences(pEE, bResult, ORD_set))
			return false;
		if (!bResult)
			return true;
		return m_fterm->symbolObjectReferences(pEE, bResult, ORD_set);
	}
}


bool FFeatures::symbolObjectReferences2(MQLExecEnv *pEE)
{
	// This vector of bools will have those entries set for which
	// we must get the MO, indexed by ObjectBlock Node Number.
	m_NonParentORDOBBMustBeRetrieved.resize(pEE->getHighestNodeNumber()+1, false);

	// First find all BlockString2 node numbers which have 
	// an ORD child which we reference.
	// The children can be direct or indirect
	NodeNumberList tmp_BS2_list;
	PORUVector::iterator oru_it = m_NonParentORU_vector.begin();
	node_number_t highest_node_number = pEE->getHighestNodeNumber();
	node_number_t nn = 0;
	while (oru_it != m_NonParentORU_vector.end()) {
		ObjectReferenceUsage *pORU = *oru_it;
		for (nn = 0;
		     nn <= highest_node_number;
		     ++nn) {
			QueryNode *pQN = pEE->getNodeFromNodeNumber(nn);
			if (pQN->getKind() == kQNKBlockString2) {
				BlockString2* pBS2 = (BlockString2*) pQN;
				if (pBS2->hasORDChild(pORU->getObjectReference())) {
					tmp_BS2_list.push_back(nn);
				}
			}
		}
		++oru_it;
	}

	// Then get a set of parents of ourselves.
	NodeNumberSet BS2_parents_of_us;
	node_number_t bs2_parent = m_block_string2_parent;
	while (bs2_parent != -1) {
		BS2_parents_of_us.insert(bs2_parent);
		BlockString2* pBS2 = (BlockString2*) pEE->getNodeFromNodeNumber(bs2_parent);
		bs2_parent = pBS2->getBlockString2Parent();
	}
	
	// Then sift the tmp_BS2_list such that nodes in the list
	// which are parents (directly or indirectly) of an
	// ObjectBlock on which we are, are removed.
	NodeNumberList tmp_BS2_list_2;
	NodeNumberList::iterator bs2_it = tmp_BS2_list.begin();
	while (bs2_it != tmp_BS2_list.end()) {
		node_number_t bs2_nn = *bs2_it;
		if (BS2_parents_of_us.find(bs2_nn) != BS2_parents_of_us.end()) {
			; // The bs2 is in our set of parents, so we
			  // don't add it!
		} else {
			tmp_BS2_list_2.push_back(bs2_nn);
		}
		++bs2_it;
	}

	// Then sift tmp_BS2_list_2 such that nodes int he list which
	// have a parent (direct or indirect) which is also in the
	// list, are removed.
	//
	// Also keep the new list in the same order as tmp_BS2_list.
	// I.e., keep the order of a depth-first search.
	// 
	// This is necessary because of the algorithm called by
	// R_features which traverses the sheaf to find MatchedObjects
	// which are necessary for resolving Object References on us.
	//
	NodeNumberList BS2_list;
	std::set<node_number_t> BS2_set;
	bs2_it = tmp_BS2_list_2.begin();
	while (bs2_it != tmp_BS2_list_2.end()) {
		node_number_t bs2_nn = *bs2_it;
		BS2_set.insert(bs2_nn);
		++bs2_it;
	}

	bs2_it = tmp_BS2_list_2.begin();
	while (bs2_it != tmp_BS2_list_2.end()) {
		node_number_t bs2_nn = *bs2_it;
		BlockString2 *pBS2 = (BlockString2*) pEE->getNodeFromNodeNumber(bs2_nn);

		bool bAdd = true;
		node_number_t bs2_parent = pBS2->getBlockString2Parent();
		while (bAdd
		       && bs2_parent != -1) {
			if (BS2_set.find(bs2_parent) != BS2_set.end()) {
				bAdd = false;
			} else {
				BlockString2 *pBS2_parent = (BlockString2*) pEE->getNodeFromNodeNumber(bs2_parent);
				bs2_parent = pBS2_parent->getBlockString2Parent();
			}
		}

		if (bAdd) {
			BS2_list.push_back(bs2_nn);
			m_block_string2_list.push_back(pBS2);
		}

		++bs2_it;
	}

	// Now let the ORUs know which ObjectBlockBase are their
	// controlling ObjectBlockBase, and which are their
	// controlling BlockString2.
	bs2_it = BS2_list.begin();
	while (bs2_it != BS2_list.end()) {
		node_number_t bs2_nn = *bs2_it;
		BlockString2 *pBS2 = (BlockString2*) pEE->getNodeFromNodeNumber(bs2_nn);
		
		oru_it = m_NonParentORU_vector.begin();

		while (oru_it != m_NonParentORU_vector.end()) {
			ObjectReferenceUsage *pORU = *oru_it;
			const std::string& ORD = pORU->getObjectReference();
			// The criterion is precisely that the
			// BlockString2 has this ORD as a child ORD.
			// The above algorithm has taken care of
			// sifting BS2_list to where this is true.
			if (pBS2->hasORDChild(ORD)) {
				node_number_t ORDObjectBlockNodeNumber = 
					pEE->m_ORD2NodeNumberMap[ORD];
				unsigned int matchedObjectIndex =
					this->getMatchedObjectIndex(pEE, ORDObjectBlockNodeNumber);
				pORU->setMatchedObjectIndex(matchedObjectIndex);
			}
			++oru_it;
		}		
		++bs2_it;
	}


	     
	// Now recurse down through what we have...
	if (isFTerm()) {
		return m_fterm->symbolObjectReferences2(pEE);
	} else {
		if (!m_ffeatures->symbolObjectReferences2(pEE))
			return false;
		return m_fterm->symbolObjectReferences2(pEE);
	}
}


unsigned int FFeatures::getMatchedObjectIndex(MQLExecEnv *pEE,
					      node_number_t ORDObjectBlockNodeNumber)
{
	UNUSED(pEE);

	// Implement a vector of ORDObjectBlockNodeNumber values which
	// is an ordered set: The incoming values are added to the end
	// if they are not there already.  If they are there already,
	// the vector is left alone, and the index where it was found
	// is returned.
	for (unsigned int index = 0;
	     index < m_ORDOBBNodeNumber_vector.size();
	     ++index) {
		node_number_t nn = m_ORDOBBNodeNumber_vector[index];
		if (nn == ORDObjectBlockNodeNumber) {
			return index;
		}
	}

	// It wasn't there, so add it!
	m_ORDOBBNodeNumber_vector.push_back(ORDObjectBlockNodeNumber);

	// Also make a note that we must retrieve those MOs which have
	// an ObjectBlockBase with the same node number as
	// ORDObjectBlockNodeNumber.
	m_NonParentORDOBBMustBeRetrieved[ORDObjectBlockNodeNumber] = true;

	// Now return the index of the newly created entry.
	return m_ORDOBBNodeNumber_vector.size() - 1;
}


bool FFeatures::symbolSetFeatureIndex(const std::vector<std::string>& feature_name_vec, bool& bResult)
{
	if (isFTerm()) {
		return m_fterm->symbolSetFeatureIndex(feature_name_vec, bResult);
	} else {
		if (!m_ffeatures->symbolSetFeatureIndex(feature_name_vec, bResult)) {
			return false;
		}
		if (!bResult) {
			return true;
		}
		
		return m_fterm->symbolSetFeatureIndex(feature_name_vec, bResult);
	}
}

void FFeatures::symbolAddFeatures(MQLObject* pObj, EMdFDB *pDB)
{
	if (isFTerm()) {
		m_fterm->symbolAddFeatures(pObj, pDB);
	} else {
		m_ffeatures->symbolAddFeatures(pObj, pDB);
		m_fterm->symbolAddFeatures(pObj, pDB);
	}
}

void FFeatures::symbolAddFeaturesToSet(std::set<std::string>& myset) const
{
	if (isFTerm()) {
		m_fterm->symbolAddFeaturesToSet(myset);
	} else {
		m_ffeatures->symbolAddFeaturesToSet(myset);
		m_fterm->symbolAddFeaturesToSet(myset);
	}
}


bool FFeatures::type(MQLExecEnv *pEE, bool& bResult)
{
	if (isFTerm()) {
		return m_fterm->type(pEE, bResult);
	} else {
		if (!m_ffeatures->type(pEE, bResult))
			return false;
		if (!bResult)
			return true;
		return m_fterm->type(pEE, bResult);
	}
}

bool FFeatures::exec(MQLExecEnv *pEE, const InstObject *pInstObj)
{
	if (isFTerm()) {
		// ffeatures ::= fterm
		return m_fterm->exec(pEE, pInstObj);
	} else {
		// ffeatures ::= ffeatures "OR" fterm
		if (m_ffeatures->exec(pEE, pInstObj))
			return true;
		return m_fterm->exec(pEE,  pInstObj);
	}
}

void FFeatures::pretty(std::ostream *pOut, int indent, NonParentORDSolution *pNonParentORDSolution)
{
	print_indent(pOut, indent);
	(*pOut) << "FFeatures {\n";
	if (isFTerm()) {
		m_fterm->pretty(pOut, indent+1, pNonParentORDSolution);
	} else {
		m_ffeatures->pretty(pOut, indent+1, pNonParentORDSolution);
		print_indent(pOut, indent);
		(*pOut) << "OR\n";
		m_fterm->pretty(pOut, indent+1, pNonParentORDSolution);
	}	
	print_indent(pOut, indent);
	(*pOut) << "} FFeatures\n";
}


const std::string& FFeatures::getConstraints(EMdFDB *pDB)
{
	static const std::string empty = "";
	if (m_constraints == 0) {
		m_constraints = makeConstraints(pDB, true, false);
	} 
	if (m_constraints == 0) {
		return empty;
	} else {
		return m_constraints->getConstraints(pDB);
	}
}

EMdFFFeatures *FFeatures::getEMdFConstraints(EMdFDB *pDB)
{
	if (m_constraints == 0) {
		m_constraints = makeConstraints(pDB, true, false);
	} 
	return m_constraints;
}

EMdFFFeatures *FFeatures::makeConstraints(EMdFDB *pDB, bool bContextIsPositive, bool bContextHasBeenNegative) const
{
	if (isFTerm()) {
		EMdFFTerm *pTerm = m_fterm->makeConstraints(pDB, bContextIsPositive, bContextHasBeenNegative);
		if (pTerm == 0) {
			return 0;
		} else {
			return pDB->getEMdFFFeatures(0, pTerm);
		}
	} else {
		EMdFFFeatures *A;
		EMdFFTerm *B;
		A = m_ffeatures->makeConstraints(pDB, bContextIsPositive, bContextHasBeenNegative);
		B = m_fterm->makeConstraints(pDB, bContextIsPositive, bContextHasBeenNegative);
		if (bContextIsPositive) {
			// In a positive context, if even one cannot be prequeried,
			// neither must be prequeried. This is because it would miss
			// an extension to the query.
			if (A == 0 || B == 0) {
				m_ffeatures->resetCanBePreQueried();
				delete A;
				
				m_fterm->resetCanBePreQueried();
				delete B;

				return 0;
			} else {
				return pDB->getEMdFFFeatures(A, B);
			}
		} else { // bContextIsPositive == false
			// In a negative context, OR works like AND, and hence
			// it does no harm to leave one out. This is because
			// taking one away from an AND does not restrict but rather
			// enlarge the output from the SQL engine, and hence
			// it doesn't matter; the non-prequeriable feature
			// can be checked in the backend.

			if (A == 0 && B == 0) {
				m_fterm->resetCanBePreQueried();
				m_ffeatures->resetCanBePreQueried();
				return 0;
			} else if (A != 0 && B == 0) {
				m_fterm->resetCanBePreQueried();
				return A;
			} else if (A == 0 && B != 0) {
				m_ffeatures->resetCanBePreQueried();
				return pDB->getEMdFFFeatures(0, B);
			} else {
				return pDB->getEMdFFFeatures(A, B);
			}
		}
	}
}


////////////////////////////////////////////////////////////
//
// ObjectBlockBase
//
////////////////////////////////////////////////////////////
ObjectBlockBase::ObjectBlockBase(std::string* object_type_name,
				 std::string* mark_declaration,
				 eRetrieval retrieval,
				 GrammarFeature* feature_retrieval,
				 bool bIsNOTEXIST)
	: QueryNode(kQNKObjectBlock),
	  m_retrieval(retrieval),
	  m_object_type_name(object_type_name),
	  m_object_type_id(NIL),
	  m_objectRangeType(kORTMultipleRange),
	  m_monadUniquenessType(kMUTNonUniqueMonads),
	  m_bIsNOTEXIST(bIsNOTEXIST),
	  m_mark(mark_declaration)
{
	if (feature_retrieval != 0) {
		Feature *pFeatureListTurnedTheRightWay = Feature::FromGrammarFeature(feature_retrieval);
		delete feature_retrieval;
		m_nNoOfGET_features = pFeatureListTurnedTheRightWay->getLength();
		copyFeatureRetrievalToVector(pFeatureListTurnedTheRightWay);
	} else {
		m_nNoOfGET_features = 0;
	}
}

ObjectBlockBase::ObjectBlockBase(std::string* object_type_name,
				 std::string* mark_declaration,
				 eRetrieval retrieval,
				 Feature* feature_retrieval,
				 bool bIsNOTEXIST)
	: QueryNode(kQNKObjectBlock),
	  m_retrieval(retrieval),
	  m_object_type_name(object_type_name),
	  m_object_type_id(NIL),
	  m_objectRangeType(kORTMultipleRange),
	  m_monadUniquenessType(kMUTNonUniqueMonads),
	  m_bIsNOTEXIST(bIsNOTEXIST),
	  m_mark(mark_declaration)
{
	if (feature_retrieval != 0) {
		m_nNoOfGET_features = feature_retrieval->getLength();
		copyFeatureRetrievalToVector(feature_retrieval);
	} else {
		m_nNoOfGET_features = 0;
	}
}


ObjectBlockBase::~ObjectBlockBase() 
{ 
	delete m_object_type_name; 

	for (unsigned int i = 0;
	     i < m_Feature_retrieval_vec.size();
	     ++i) {
		Feature *pFeature = m_Feature_retrieval_vec[i];
		delete pFeature;
	}

	delete m_mark;
}

ObjectBlockBase::ObjectBlockBase(const ObjectBlockBase& other)
	: QueryNode(other)
{
	m_retrieval = other.m_retrieval;
	m_bIsNOTEXIST = other.m_bIsNOTEXIST;
	m_object_type_name = new std::string(*other.m_object_type_name);
	m_mark = new std::string(*other.m_mark);
	m_nNoOfGET_features = other.m_nNoOfGET_features;

	unsigned int vector_length = other.m_Feature_retrieval_vec.size();
	if (vector_length != 0) {
		m_Feature_retrieval_vec.resize(vector_length, 0);
		for (unsigned int index = 0;
		     index < vector_length;
		     ++index) {
			m_Feature_retrieval_vec[index] =
				new Feature(*(other.m_Feature_retrieval_vec[index]));
		}
	}
	m_object_type_id = other.m_object_type_id;
	m_objectRangeType = other.m_objectRangeType;
	m_monadUniquenessType = other.m_monadUniquenessType;
}


void ObjectBlockBase::copyFeatureRetrievalToVector(Feature *pFeature)
{
	m_Feature_retrieval_vec.clear();
	while (pFeature != 0) {
		m_Feature_retrieval_vec.push_back(pFeature);
		
		Feature *pNextFeature = pFeature->getNext();

		// Break the chain
		pFeature->setNext(0);

		// Iterate
		pFeature = pNextFeature;
	}
}

Feature* ObjectBlockBase::getFeatureRetrievalFeature(unsigned int index)
{
	ASSERT_THROW(index < m_Feature_retrieval_vec.size(),
		     "ObjectBlockBase::getFeatureRetrievalFeature(): index " + long2string(index) + " out of range, should be within 0-" + long2string(m_Feature_retrieval_vec.size()-1));
	return m_Feature_retrieval_vec[index];
}


void ObjectBlockBase::weed(MQLExecEnv *pEE)
{
	// Set m_node_number, and add to pEE->m_node_vector.
	pEE->getNextNodeNumber(this);
}

// The feature is assumed to exist on the object type!
void ObjectBlock::addFeatureToBeRetrieved(MQLExecEnv *pEE, const FeatureInfo& feature_info, unsigned int& index_assigned)
{
	// Adds a the feature in feature_info to
	// m_Feature_retrieval_vec, if it is not already there.

	// This is used for adding features to be retrieved by a
	// MatchedObject, for resolving object references.

	for (unsigned int i = 0; 
	     i < m_Feature_retrieval_vec.size(); ++i) {
		Feature *pFeature = m_Feature_retrieval_vec[i];
		if (strcmp_nocase(pFeature->getFeature(), feature_info.getName()) == 0) {
			index_assigned = i;
			return;
		}
	}

	// If we got this far, it wasn't in the list already, so we must
	// add it to the end
	index_assigned = m_Feature_retrieval_vec.size();

	Feature *pNewFeature = new Feature(new std::string(feature_info.getName()),
					   0); // 0 = next

	// We assume that it has already been checked, so we don't
	// check the return values now.  It is, however, important to
	// call these, so that the internal state of the pNewFeature
	// Feature object is consistent.
	bool bDummyCompilerResult;
	pNewFeature->symbolFeaturesExist(pEE, this->getObjectTypeId(), bDummyCompilerResult);

	pNewFeature->typeFeatureName(pEE, bDummyCompilerResult);

	m_Feature_retrieval_vec.push_back(pNewFeature);

	// Also make sure that the MQLObject knows that we must
	// retrieve it, so that it gets into the Inst.
	MQLObject *pMQLObject = getObject();
	pMQLObject->addFeature(feature_info);
}

//
// - Check that object_type_name is a valid type
//
// - Check FFeatures
// 
// - Add all features used to local symbol table
//
bool ObjectBlockBase::symbol(MQLExecEnv *pEE, bool& bResult)
{
	//
	// - Check that object_type_name is a valid type
	//
	if (!symbolObjectTypeExists(pEE, bResult))
		return false;
	if (!bResult)
		return true;

	//
	// - Check feature_retrieval
	//
	if (m_Feature_retrieval_vec.size() != 0) {
		for (unsigned int index = 0;
		     index < m_Feature_retrieval_vec.size();
		     ++index) {
			Feature *pFeature = m_Feature_retrieval_vec[index];
			// Check that feature exists
			if (!pFeature->symbolFeaturesExist(pEE, m_object_type_id, bResult))
				return false;
			if (!bResult)
				return true;
		}
	}

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}


//
// Checks whether object type exists.
// Sets m_object_type_id if it does.
bool ObjectBlockBase::symbolObjectTypeExists(MQLExecEnv *pEE, bool& bObjectTypeExists)
{
	bool bDBResult;
	bDBResult = pEE->pDB->objectTypeExists(*m_object_type_name,
					       bObjectTypeExists, 
					       m_object_type_id,
					       m_objectRangeType,
					       m_monadUniquenessType);

	// Was there a DB error?
	if (!bDBResult) {
		pEE->pError->appendError("DB error checking existence of object type name '" + *m_object_type_name + "'.\n");
		return false;
	}

	// Was the object type there?
	if (!bObjectTypeExists) {
		pEE->pError->appendError("Object type name '" + *m_object_type_name + "' does not exist.\n");
		return true;
	}

	// If we got this far, we might as well get the largest object
	// length.
	if (!pEE->pDB->getLargestObjectLength(*m_object_type_name,
					      m_objectRangeType,
					      m_largest_object_length)) {
		pEE->pError->appendError("Could not get largest object length for object type name '" + *m_object_type_name + "' .\n");
		return false;
	}

	// If we got this far, there were no DB errors.
	return true;
}





////////////////////////////////////////////////////////////
//
// ObjectBlock 
//
////////////////////////////////////////////////////////////
ObjectBlock::ObjectBlock(std::string* object_type_name,
			 std::string* mark_declaration,
			 std::string* object_reference,
			 eRetrieval retrieval,
			 eFirstLast first_last,
			 MonadSetRelationClause *pMSRC,
			 FFeatures* feature_constraints,
			 GrammarFeature* pFeature_retrieval,
			 Blocks* opt_blocks,
			 bool bIsNOTEXIST)
	: ObjectBlockBase(object_type_name,
			  mark_declaration,
			  retrieval,
			  pFeature_retrieval,
			  bIsNOTEXIST),
	  m_object_reference(object_reference),
	  m_first_last(first_last),
	  m_feature_constraints(feature_constraints),
	  m_opt_blocks(opt_blocks),
	  m_bDoCalculatePreQueryString(false),
	  m_bIsFirstInBlockString(false),
	  m_bIsLastInBlockString(false),
	  m_pMSRC(pMSRC),
	  m_inst(0)
{
	m_pOBB = 0;
	m_object = new MQLObject();
	*m_object_type_name = normalizeOTName(*m_object_type_name);
	str_tolower(*m_object_reference, m_object_reference_lower);
}

ObjectBlock::ObjectBlock(std::string* object_type_name,
			 std::string* mark_declaration,
			 std::string* object_reference,
			 eRetrieval retrieval,
			 eFirstLast first_last,
			 MonadSetRelationClause *pMSRC,
			 FFeatures* feature_constraints,
			 Feature *GET_feature_retrieval,
			 Blocks* opt_blocks,
			 bool bIsNOTEXIST)
	: ObjectBlockBase(object_type_name,
			  mark_declaration,
			  retrieval,
			  GET_feature_retrieval,
			  bIsNOTEXIST),
	  m_object_reference(object_reference),
	  m_first_last(first_last),
	  m_feature_constraints(feature_constraints),
	  m_opt_blocks(opt_blocks),
	  m_bDoCalculatePreQueryString(false),
	  m_bIsFirstInBlockString(false),
	  m_bIsLastInBlockString(false),
	  m_pMSRC(pMSRC),
	  m_inst(0)
{
	m_pOBB = 0;
	m_object = new MQLObject();
	*m_object_type_name = normalizeOTName(*m_object_type_name);
	str_tolower(*m_object_reference, m_object_reference_lower);
}



ObjectBlock::~ObjectBlock()
{
	// Do not delete m_pOBB; it has been added to an OBBVec.
	delete m_object_reference;
	delete m_feature_constraints;
	delete m_opt_blocks;
	delete m_object; 
	delete m_pMSRC;
	delete m_inst;
}

void ObjectBlock::weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult)
{
	if (m_feature_constraints != 0) {
		// Set m_node_number, and add to pEE->m_node_vector.
		pEE->getNextNodeNumber(m_feature_constraints);

		m_feature_constraints->setBlockString2Parent(m_block_string2_parent);

		m_feature_constraints->weedFeatureConstraints(pEE, bResult, m_feature_constraints->getNodeNumber());
		if (!bResult)
			return;
	}
}




//
// - First check ourselves
//
// - Then check the opt_blocks
//
void ObjectBlock::weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, node_number_t block_string2_parent)
{
	ObjectBlockBase::weed(pEE);
	
	m_block_string2_parent = block_string2_parent;

	switch (m_first_last) {
	case kFirst:
		bResult = is_first;
		if (!bResult) {
			pEE->pError->appendError("The 'first' modifier must only be used for the first object block in a blocks.\n");
		}
		break;
	case kLast:
		bResult = is_last;
		if (!bResult) {
			pEE->pError->appendError("The 'last' modifier must only be used for the last object block in a blocks.\n");
		}
		break;
	case kFirstAndLast:
		bResult = is_first && is_last;
		if (!bResult) {
			pEE->pError->appendError("The 'first and last' modifier must only be used object blocks that are both first and last in their blocks.\n");
		}
		break;
	case kNoFirstLast:
		bResult = true;
		break;
	default:
		// We might add more later...
		ASSERT_THROW(false,
			     "Unknown first/last kind");
		bResult = false;
	}

	// NOTEXIST must be made "noretrieve"
	if (isNOTEXIST()) {
		// Make sure we are NORETRIEVE
		m_retrieval = kNoRetrieve;
	}

	// Specify whether we are first and/or last
	m_bIsFirstInBlockString = is_first;
	m_bIsLastInBlockString = is_last;

	if (bResult && m_pMSRC) {
		m_pMSRC->weed(pEE, bResult);
	}

	if (bResult) {
		weedFeatureConstraints(pEE, bResult);
	}


	if (bResult && m_opt_blocks != 0) {
		m_opt_blocks->weed(pEE, bResult, block_string2_parent);
	}
}
//
// - Check that object_type_name is a valid type
//
// - Check that the object reference declaration is only declared once
//
// - Declare object reference
//
// - Check FFeatures
// 
// - Add all features used to local symbol table
//
// - Check the opt_blocks
//
bool ObjectBlock::symbol(MQLExecEnv *pEE, bool bCalculatePreQueryString, bool& bResult)
{
        m_bDoCalculatePreQueryString = bCalculatePreQueryString;

	//
	// Well, now we can...
	//
	/*
	// Currently, we cannot use pre query strings when doing an
	// overlap, since we cannot use it when calling
	// EMdFDB::getObjectsHavingMonadsIn in R_inst().
	if (m_pMSRC->getOperation() == kMSROOverlap) {
		m_bDoCalculatePreQueryString = false;
	}
	*/

	// - Check base
	//
	if (!ObjectBlockBase::symbol(pEE, bResult))
		return false;
	if (!bResult)
		return true;

	//
	// - Check that the object reference declaration is only declared once,
	//   and declare it if it has not already been declared.
	//
	if (!m_object_reference->empty()) {
		if (pEE->m_ORD2NodeNumberMap.find(m_object_reference_lower)
		    != pEE->m_ORD2NodeNumberMap.end()) {
			// It was already declared
			pEE->pError->appendError("Object reference " + *m_object_reference + " has already been declared once.  An object reference must not be declared more than once.\n");
			bResult = false;
			return true;
		} else {
			// 
			// - Declare object reference
			// 
			pEE->m_ORD2NodeNumberMap.insert(std::make_pair(m_object_reference_lower, this->getNodeNumber()));
		}
	}
		
	//
	// - Check FFeatures
	// 
	if (m_feature_constraints != 0) {
		if (!m_feature_constraints->symbol(pEE, 
						   *m_object_type_name, 
						   m_object_type_id, 
						   bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Make sure that FeatureComparison::m_bAddToInst has been set
	if (m_bDoCalculatePreQueryString
	    && !calculatePreQueryString(pEE->pDB)) {
		pEE->pError->appendError("Database error while calculating pre-query string.\n");
		return false;
	}

	//
	// - Add all features used to local symbol table
	//
	if (m_feature_constraints != 0) {
		m_feature_constraints->symbolAddFeatures(m_object, pEE->pDB);
	}

	// Symbol-check the MonadSetRelationClause
	if (bResult && m_pMSRC) {
		m_pMSRC->symbol(pEE, getObjectTypeId(), bResult);

		if (!bResult)
			return true;
	}



	//
	// - Check the blocks
	//
	if (bResult && m_opt_blocks != 0) {
		if (!m_opt_blocks->symbol(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

  
	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}


bool ObjectBlock::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (!m_object_reference->empty()) {
		ORD_set.insert(m_object_reference_lower);
	}

	if (!m_object_reference->empty()) {
		node_number_t bs2_parent = m_block_string2_parent;
		while (bs2_parent != -1) {
			BlockString2* pBlockString2Node = 
				(BlockString2*) pEE->getNodeFromNodeNumber(bs2_parent);
			pBlockString2Node->addORDChild(m_object_reference_lower);
			bs2_parent = pBlockString2Node->getBlockString2Parent();
		}
	}
	if (m_feature_constraints != 0) {
		if (!m_feature_constraints->symbolObjectReferences(pEE, bResult, ORD_set)) {
			return false;
		}
		if (!bResult) {
			return true;
		}
	}
	if (m_opt_blocks != 0) {
		if (!m_opt_blocks->symbolObjectReferences(pEE, bResult, ORD_set))
			return false;
		if (!bResult)
			return true;
	}
	return true;
}



bool ObjectBlock::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (m_feature_constraints != 0) {
		if (!m_feature_constraints->symbolObjectReferences2(pEE)) {
			return false;
		}
	}
	if (m_opt_blocks != 0) {
		if (!m_opt_blocks->symbolObjectReferences2(pEE))
			return false;
	}

	return true;
}


//
// - First check the ffeatures
//
// - Then check the opt_blocks
//
bool ObjectBlock::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	UNUSED(contextRangeType);

	// Check ffeatures
	if (m_feature_constraints != 0) {
		if (!m_feature_constraints->type(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Type-check the MonadSetRelationClause
	if (bResult && m_pMSRC) {
		m_pMSRC->type(pEE, bResult);

		if (!bResult)
			return true;
	}

	// Check opt_blocks
	if (m_opt_blocks != 0) {
		if (!m_opt_blocks->type(pEE, m_objectRangeType, bResult))
			return false;
		if (!bResult)
			return true;
	}



	//
	// - Add feature retrievals to MQLObject.
	// 
	// This should have been a symbol operation, but it must come after
	// all of the symbolObjectReferences2 process, which is currently
	// the last process in the symbol stage.
	//
	// It doesn't really matter that we don't do it in the symbol stage,
	// so long as it is done before the execution stage.
	//
	if (m_Feature_retrieval_vec.size() != 0) {
		for (unsigned int index = 0;
		     index < m_Feature_retrieval_vec.size();
		     ++index) {
			Feature *pFeature = m_Feature_retrieval_vec[index];

			// Make sure that the Feature knows where in
			// this vector it is.
			pFeature->assignListIndex(index);

			// Add to object
			pFeature->symbolAddToObject(pEE, m_object);
		}
	}


	// If we got this far, there were no DB errors and no compiler-errors
	bResult = true;
	return true;
}

bool ObjectBlock::makeInst(MQLExecEnv *pEE, const SetOfMonads& Su, eMonadSetRelationOperation *pMonadSetOperation, String2COBPtrMMap& mmap)
{
	// Get characteristic string
	std::string characteristic_string = getCharacteristicString();
	
	// See if we can find it already
	std::pair<String2COBPtrMMap::iterator,String2COBPtrMMap::iterator> mypair = mmap.equal_range(characteristic_string);
	String2COBPtrMMap::iterator p;
	bool bDoContinue = true;
	for (p = mypair.first; bDoContinue && p != mypair.second; ++p) {
		Inst *pInst = (p->second)->m_inst;
		if (pInst != 0) {
			m_inst = new Inst(pInst);
			bDoContinue = false;
		}
	}
	
	if (m_inst == 0) {
		// We didn't find it.  Calculate it.
		try {
			m_inst = R_inst(pEE, Su, this, pMonadSetOperation);
			m_inst->setIsAggregate(true);
		} catch (EMdFDBException& e) {
			return false;
		}
	}
		
	return true;
}

void ObjectBlock::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB)
{
	// Calculate pre-query string
	calculatePreQueryString(pDB);

	// Get characteristic string
	std::string characteristic_string;
	calculateCharacteristicStringOutermost(prefix);
	characteristic_string = getCharacteristicString();

	// Add this to the mmap
	mmap.insert(String2COBPtrMMap::value_type(characteristic_string, this));

	// Recurse down through inner blocks
	if (m_opt_blocks != 0) {
		std::string prefix_opt_blocks = characteristic_string + ";";
		m_opt_blocks->calculateMMapOutermost(mmap, 
					    prefix_opt_blocks,
					    pDB);
	}
}

void ObjectBlock::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	// Recurse down through inner blocks
	std::string opt_blocks_inner_charstring;
	if (m_opt_blocks != 0) {
		m_opt_blocks->calculateMMapInnermost(mmap, 
						     opt_blocks_inner_charstring,
						     pDB);
	}
	
	// Calculate pre-query string
	calculatePreQueryString(pDB);

	// Get characteristic string
	std::string characteristic_string;
	calculateCharacteristicStringInnermost(opt_blocks_inner_charstring + ";");
	characteristic_string = getCharacteristicString();

	// Add this to the mmap
	mmap.insert(String2COBPtrMMap::value_type(characteristic_string, this));

	inner_charstring = characteristic_string;
}
      

EMdFFFeatures *ObjectBlock::getEMdFConstraints(EMdFDB *pDB)
{
	try {
		if (m_feature_constraints == 0
		    || !m_bDoCalculatePreQueryString) {
			return 0;
		} else {
			return m_feature_constraints->getEMdFConstraints(pDB);
		}
	} catch (EMdFDBDBError& e) {
		// If we come here, there was a DB error.
		return 0;
	}
}

bool ObjectBlock::calculatePreQueryString(EMdFDB *pDB)
{
	try {
		if (m_feature_constraints == 0
		    || !m_bDoCalculatePreQueryString) {
			m_pre_query_string = "";
		} else {
			m_pre_query_string = m_feature_constraints->getConstraints(pDB);
		}
    
		// If we came this far, there were no DB errors
		return true;
	} catch (EMdFDBDBError& e) {
		// If we come here, there was a DB error.
		return false;
	}
}

void ObjectBlock::calculateCharacteristicStringOutermost(const std::string& prefix)
{
	// Get object type name
	m_characteristic_string = prefix + *m_object_type_name + ",";

	// Get pre_query_string
	m_characteristic_string += getPreQueryString();

	m_characteristic_string += "#MSRC#";

	m_characteristic_string += m_pMSRC->calculateCharacteristicString();

	m_characteristic_string += "#FEAT#";

	// Get vector of feature names
	const std::vector<std::string>& feature_name_vector_inst = m_object->getFeatureNames();

	// Add feature names to set
	std::vector<std::string>::const_iterator civ;
	std::vector<std::string>::const_iterator civend(feature_name_vector_inst.end());
	for (civ = feature_name_vector_inst.begin(); civ != civend; ++civ) {
		m_characteristic_string += "," + *civ;
	}
}

void ObjectBlock::calculateCharacteristicStringInnermost(const std::string& prefix)
{
	// Get object type name
	m_characteristic_string = prefix + *m_object_type_name + ",";

	// Get pre_query_string
	m_characteristic_string += getPreQueryString();

	m_characteristic_string += "#MSRC#";

	// We always use kMSROOverlap for the InnermostFirst strategy.
	// So make a local copy, and set its operation to kMSROOverlap
	MonadSetRelationClause myMSRC(*m_pMSRC);
	myMSRC.setOperation(kMSROOverlap);

	m_characteristic_string += myMSRC.calculateCharacteristicString();

	m_characteristic_string += "#FEAT#";

	// Get vector of feature names
	const std::vector<std::string>& feature_name_vector_inst = m_object->getFeatureNames();

	// Add feature names to set
	std::vector<std::string>::const_iterator civ;
	std::vector<std::string>::const_iterator civend(feature_name_vector_inst.end());
	for (civ = feature_name_vector_inst.begin(); civ != civend; ++civ) {
		m_characteristic_string += "," + *civ;
	}
}

bool ObjectBlock::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	// Write to log
	// LOG_WRITE_TIME("ObjectBlock::aggregateQuery", *m_object_type_name + " Making inst.");

	monad_m lola = this->getLargestObjectLength();
	if (lola > largest_object_length_above) {
		largest_object_length_above = lola;
	}

	switch (strategy) {
	case kAQSOutermostFirst: {
		// Do it for this
		if (!makeInst(pEE, characteristic_set, 0, mmap))
			return false;
		// LOG_WRITE_TIME("ObjectBlock::aggregateQuery", *m_object_type_name + " Inst-making finished.");
		
		// Recurse down through the opt_blocks
		if (m_opt_blocks != 0) {
			// Get big-union of inst.
			FastSetOfMonads CS;
			m_inst->bigUnion(CS);
			
			// Recurse downwards.
			// Do it, both if CS is empty, and if CS is non-empty.
			// We need to calculate empty m_inst's if we are to
			// be able to run a query at all.
			if (CS.isEmpty()) {
				if (!m_opt_blocks->aggregateQuery(pEE, CS, Su, strategy, largest_object_length_above, mmap)) {
					return false;
				}
			} else {
				// LOG_WRITE_TIME("ObjectBlock::aggregateQuery ", *m_object_type_name + " Recursing down through opt_blocks.");
				if (!m_opt_blocks->aggregateQuery(pEE, CS, Su, strategy, largest_object_length_above, mmap)) {
					return false;
				}
				// LOG_WRITE_TIME("ObjectBlock::aggregateQuery ", *m_object_type_name + " Recursion finished.");
			}
		}
	}
		break;
	case kAQSInnermostFirst: {
		if (m_opt_blocks == 0) {
			SetOfMonads local_characteristic_set;

			// We fill the gaps because we must.
			local_characteristic_set = Su.fillGaps(largest_object_length_above);

			eMonadSetRelationOperation msr_op = kMSROOverlap;

			// Do it for this
			if (!makeInst(pEE, local_characteristic_set, &msr_op, mmap))
				return false;


			// Get big-union of inst into characteristic set
			FastSetOfMonads CS1;
			m_inst->almostRealBigUnion(CS1);

			characteristic_set.unionWith(CS1.fillGaps(largest_object_length_above));
		} else {
			FastSetOfMonads CS;

			// LOG_WRITE_TIME("ObjectBlock::aggregateQuery ", *m_object_type_name + " Recursing down through opt_blocks.");
			if (!m_opt_blocks->aggregateQuery(pEE, CS, Su, strategy, largest_object_length_above, mmap))
				return false;
			// LOG_WRITE_TIME("ObjectBlock::aggregateQuery ", *m_object_type_name + " Recursion finished.");

			if (CS.isEmpty()) {
				characteristic_set = CS;
				std::vector<std::string> empty_feature_names;
				m_inst = new Inst(empty_feature_names); // Empty Inst
				// We need to set it aggregate,
				// because it is.  It is just empty,
				// but we did create it before running
				// the query. If we don' we'll delete
				// it twice.
				m_inst->setIsAggregate(true);

			} else {
				eMonadSetRelationOperation msr_op = kMSROOverlap;

				// We fill the gaps because we must.
				CS = CS.fillGaps(largest_object_length_above);

				// Do it for this
				if (!makeInst(pEE, CS, &msr_op, mmap))
					return false;
				
				// Get big-union of inst into characteristic set
				FastSetOfMonads CS1;
				m_inst->almostRealBigUnion(CS1);
				
				characteristic_set.unionWith(CS);
				characteristic_set.unionWith(CS1.fillGaps(largest_object_length_above));
			} 
		}
	}
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown aggregate query strategy");
		break;
	}

	// If we came this far, there were no DB errors
	return true;
}

StartMonadIterator* ObjectBlock::getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm)
{
	UNUSED(U);

	SetOfMonads Suping, Uping;
	monad_m lower_bound_Sm;
	monad_m upper_bound_Universe_end;
	if (getMSRC()->getOperation() == kMSROPartOf) {
		// If we are doing other monad sets than
		// 'monads', all bets are off as to where to
		// start, so we start at 1, and end at
		// MAX_MONAD.
		//
		// FIXME: What we really should be doing is to
		// have a way of knowing what the largest
		// object length is of any given monad set, as
		// well as the largest last monad of any set
		// of monads.
		if (strcmp_nocase(getMSRC()->getOurMonadSet(), "monads") != 0) {
			lower_bound_Sm = 1L;
			upper_bound_Universe_end = MAX_MONAD;
		} else {
			lower_bound_Sm = MYMAX(pEE->m_all_m_1.first(),
					       Sm);
					
			// Even for set of monads being the
			// regular "monads" feature, we should
			// not rely on Su having the last
			// monad to be found.  Instead, we
			// should set the end of the Su (and
			// the end of the Univese) to be
			// all_m_1's last monad.
			monad_m max_m = pEE->m_all_m_1.last();

			if (Su.isEmpty()) {
				upper_bound_Universe_end = max_m;
			} else {
				upper_bound_Universe_end = MYMIN(max_m,
								 Su.last());
			}
		}

		if (Su.isEmpty()) {
			Suping = Su;
		} else {
			Suping = SetOfMonads::intersect(Su, 
							SetOfMonads(lower_bound_Sm, upper_bound_Universe_end));
		}
				
		if (Suping.isEmpty()) {
			Uping = Suping;
		} else {
			Uping = SetOfMonads(Suping.first(), Suping.last());
		}
	} else if (getMSRC()->getOperation() == kMSROOverlap) {
		
		// If we are doing other monad sets than
		// 'monads', all bets are off as to where to
		// start, so we start at 1, and end at
		// MAX_MONAD.
		//
		// FIXME: What we really should be doing is to
		// have a way of knowing what the largest
		// object length is of any given monad set, as
		// well as the largest last monad of any set
		// of monads.
		if (strcmp_nocase(getMSRC()->getOurMonadSet(), "monads") != 0) {
			lower_bound_Sm = 1L;
			upper_bound_Universe_end = MAX_MONAD;
		} else {
			monad_m largest_object_length = getLargestObjectLength();
			lower_bound_Sm = MYMAX(1L,
					       Sm - largest_object_length
					       );
					
			// Even for set of monads being the
			// regular "monads" feature, we should
			// not rely on Su having the last
			// monad to be found.  Instead, we
			// should set the end of the Su (and
			// the end of the Univese) to be
			// all_m_1's last monad.
			monad_m max_m = pEE->m_all_m_1.last();
					
			upper_bound_Universe_end = max_m;
		}
				
		Suping = Su;
		if (Suping.isEmpty()) {
			Suping.add(lower_bound_Sm, upper_bound_Universe_end);
		} else {
			Suping.add(lower_bound_Sm, Suping.first());
			Suping.add(Suping.last(), upper_bound_Universe_end);
		}
				
		if (Suping.isEmpty()) {
			Uping = Suping;
		} else {
			Uping = SetOfMonads(Suping.first(), Suping.last());
		}
	} else if (getMSRC()->getOperation() == kMSROStartsIn) {
		// If we are doing other monad sets than
		// 'monads', all bets are off as to where to
		// start, so we start at 1, and end at
		// MAX_MONAD.
		//
		// FIXME: What we really should be doing is to
		// have a way of knowing what the largest
		// object length is of any given monad set, as
		// well as the largest last monad of any set
		// of monads.
		if (strcmp_nocase(getMSRC()->getOurMonadSet(), "monads") != 0) {
			lower_bound_Sm = 1L;
			upper_bound_Universe_end = MAX_MONAD;
		} else {
			lower_bound_Sm = MYMAX(pEE->m_all_m_1.first(),
					       Sm);
					
			// Even for set of monads being the
			// regular "monads" feature, we should
			// not rely on Su having the last
			// monad to be found.  Instead, we
			// should set the end of the Su (and
			// the end of the Univese) to be
			// all_m_1's last monad.
			monad_m max_m = pEE->m_all_m_1.last();

			if (Su.isEmpty()) {
				upper_bound_Universe_end = max_m;
			} else {
				monad_m largest_object_length = getLargestObjectLength();
				upper_bound_Universe_end = MYMIN(max_m,
								 Su.last() + largest_object_length);
			}
		}

		if (Su.isEmpty()) {
			Suping = Su;
		} else {
			Suping = SetOfMonads::intersect(Su, 
							SetOfMonads(lower_bound_Sm, upper_bound_Universe_end));
		}
				
		if (Suping.isEmpty()) {
			Uping = Suping;
		} else {
			Uping = SetOfMonads(Suping.first(), Suping.last());
		}

	} else {
		ASSERT_THROW(false,
			     "Unknown object block monad set relation operator in Block::getSMI().");
	}
			
	if (isNOTEXIST()) {
		// NOTEXIST won't have any objects in Su if it truly
		// does not exist, so the inst is empty!  Therefore,
		// we can't use the pInst constructor.
		return new MonadSetStartMonadIterator(Suping, lower_bound_Sm, false);
	} else {
		// Normal object block
		Inst *pInst = R_inst(pEE, Uping, this, 0);
				
		return new InstStartMonadIterator(pInst, Uping, Suping, lower_bound_Sm);
	} 
}

void ObjectBlock::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	/*
	if (m_pMSRC->getOperation() == kMSROOverlap) {
		bResult = false;
	}
	*/

	if (bResult) {
		if (m_feature_constraints != 0
		    && m_feature_constraints->hasORU()) {
			bResult = false;
		}
	}

	if (bResult) {
		if (m_opt_blocks != 0) {
			m_opt_blocks->canChooseAQStrategyInnermostFirst(bResult);
		} else {
			if (m_feature_constraints == 0) {
				bResult = false;
			}
		}
	}
}


void ObjectBlock::addOBBToVec(OBBVec *pOBBVec)
{
	m_pOBB = new ObjectBlockBase(*this);
	pOBBVec->push_back(m_pOBB);
	if (m_opt_blocks != 0) {
		m_opt_blocks->addOBBToVec(pOBBVec);
	}
}





////////////////////////////////////////////////////////////
//
// Power 
//
////////////////////////////////////////////////////////////
// This gets called by the parser when it is ".. (<|<=) integer".
// "<" gets "integer-1", "<=" gets "integer".
Power::Power(long limit)
	: m_limit_low(0), m_limit_high(limit)
{
}

Power::Power(long limit_low, long limit_high)
	: m_limit_low(limit_low), m_limit_high(limit_high)
{
}

Power::~Power()
{
}

// Tests that it is monotonic
void Power::weed(bool &bResult)
{
	bResult = m_limit_low <= m_limit_high;
}




////////////////////////////////////////////////////////////
//
// BlockString0
//
////////////////////////////////////////////////////////////
BlockString0::BlockString0(Block *block)
	: m_block(block),
	  m_block_string(0)
{
}

BlockString0::BlockString0(BlockString *block_string)
	: m_block(0),
	  m_block_string(block_string)
{
}

BlockString0::~BlockString0()
{
	delete m_block;
	delete m_block_string;
}

void BlockString0::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	if (isBlock()) {
		m_block->getOutermostSetOfMonadsFeatureSet(som_name_set);
	} else {
		m_block_string->getOutermostSetOfMonadsFeatureSet(som_name_set);
	}
}


bool BlockString0::firstBlockIsPower()
{
	if (isBlock()) {
		return m_block->isPowerBlock();
	} else {
		return m_block_string->firstBlockIsPower();
	}
}


void BlockString0::weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, bool bPrevIsGap, bool bPrevIsPower, node_number_t block_string2_parent)
{
	if (isBlock()) {
		if (bPrevIsGap && m_block->isGapOrOptGapBlock()) {
			bResult = false;
			pEE->pError->appendError("Error: You cannot have two gap_blocks or opt_gap_blocks next to each other,\n"
						 "       nor can you have an opt_gap_block next to a gap_block.\n");
		}
		if (bResult &&
		    bPrevIsPower && m_block->isPowerBlock()) {
			bResult = false;
			pEE->pError->appendError("Error: You cannot have two power blocks (i.e., \"..\") next to each other.\n");
		}
		if (bResult) {
			m_block->weed(pEE, bResult, is_first, is_last, block_string2_parent);
		}
	} else {
		m_block_string->weed(pEE,bResult, is_first, is_last, false, false, block_string2_parent);
	}
}


bool BlockString0::symbol(MQLExecEnv *pEE, bool& bResult)
{
	if (isBlock()) {
		return m_block->symbol(pEE, bResult);
	} else {
		return m_block_string->symbol(pEE, bResult);
	}
}

bool BlockString0::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (isBlock()) {
		return m_block->symbolObjectReferences(pEE, bResult, ORD_set);
	} else {
		return m_block_string->symbolObjectReferences(pEE, bResult, ORD_set);
	}
}


bool BlockString0::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (isBlock()) {
		return m_block->symbolObjectReferences2(pEE);
	} else {
		return m_block_string->symbolObjectReferences2(pEE);
	}
}

bool BlockString0::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	if (isBlock()) {
		return m_block->type(pEE, contextRangeType, bResult);
	} else {
		return m_block_string->type(pEE, contextRangeType, bResult);
	}
}

void BlockString0::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB) 
{
	if (isBlock()) {
		m_block->calculateMMapOutermost(mmap, prefix, pDB);
 
	} else {
		m_block_string->calculateMMapOutermost(mmap, prefix, pDB);
	}
}

void BlockString0::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	if (isBlock()) {
		m_block->calculateMMapInnermost(mmap, inner_charstring, pDB);
 
	} else {
		m_block_string->calculateMMapInnermost(mmap, inner_charstring, pDB);
	}
}
      

bool BlockString0::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	// Do it for this object
	if (isBlock()) {
		if (!m_block->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap))
			return false;
	} else {
		if (!m_block_string->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap))
			return false;
	}

	return true;
}

void BlockString0::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	if (isBlock()) {
		// Recurse
		m_block->canChooseAQStrategyInnermostFirst(bResult);
	} else {
		// Recurse
		m_block_string->canChooseAQStrategyInnermostFirst(bResult);
	}
}

bool BlockString0::hasObjectBlockInBlockString()
{
	if (isBlock()) {
		return m_block->hasObjectBlockInBlockString();
	} else {
		return m_block_string->hasObjectBlockInBlockString();
	}
}



void BlockString0::addOBBToVec(OBBVec *pOBBVec)
{
	if (isBlock()) {
		m_block->addOBBToVec(pOBBVec);
	} else {
		m_block_string->addOBBToVec(pOBBVec);
	}
}

StartMonadIterator* BlockString0::getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm)
{
	if (isBlock()) {
		return m_block->getSMI(pEE, U, Su, Sm);
	} else {
		return m_block_string->getSMI(pEE, U, Su, Sm);
	}
}



////////////////////////////////////////////////////////////
//
// BlockString1
//
////////////////////////////////////////////////////////////
BlockString1::BlockString1(BlockString0 *block_string0)
	: m_block_string0(block_string0),
	  m_pStarSOM(0),
	  m_pMQLMSE(0)
{
}

BlockString1::BlockString1(BlockString0 *block_string0, MQLMonadSetElement *pMSEs)
	: m_block_string0(block_string0),
	  m_pStarSOM(0),
	  m_pMQLMSE(pMSEs)
{
}

BlockString1::~BlockString1()
{
	delete m_block_string0;
	delete m_pStarSOM;
	delete m_pMQLMSE;
}

bool BlockString1::firstBlockIsPower()
{
	return m_block_string0->firstBlockIsPower();
}

void BlockString1::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	m_block_string0->getOutermostSetOfMonadsFeatureSet(som_name_set);
}



void BlockString1::weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, bool bPrevIsGap, bool bPrevIsPower, node_number_t block_string2_parent)
{
	m_block_string0->weed(pEE,bResult,is_first,is_last,bPrevIsGap,bPrevIsPower, block_string2_parent);
	if (!bResult) {
		return;
	}
	if (m_pMQLMSE != 0) {
		m_pMQLMSE->weed(pEE, false, bResult);

		if (!bResult) {
			pEE->pError->appendError("An error occurred in weeding the monad set related to a * (STAR) construct.");
			return;
		}
	}
	
	// We cannot ise isStar() in weed(), since we only get
	// m_pStarSOM in symbol().
	if (m_pMQLMSE != 0) {
		if (m_block_string0->isBlock()) {
			eBlockKind blockKind = m_block_string0->getBlock()->getKind();
			if (blockKind == kObjectBlockNOTEXIST) {
				pEE->pError->appendError("You may not have a NOTEXIST on an object block with a * operator.\n");
				bResult = false;
			} else if (blockKind == kObjectBlock) {
				if (m_block_string0->getBlock()->getObjectBlock()->getObjectReference().length() > 0) {
					pEE->pError->appendError("You may not have an object reference on an object block with a * operator.\n");
					bResult = false;
				}
			}
		}
	}
}


bool BlockString1::symbol(MQLExecEnv *pEE, bool& bResult)
{
	if (m_pMQLMSE != 0) {
		m_pStarSOM = new SetOfMonads();
		m_pMQLMSE->monadsBuildSet(*m_pStarSOM);
	}
	return m_block_string0->symbol(pEE, bResult);
}

bool BlockString1::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (m_pMQLMSE == 0) {
		return m_block_string0->symbolObjectReferences(pEE, bResult, ORD_set);
	} else {
		std::set<std::string> ObjectBlockInnerORD_set;
		// If any ORD is in ObjectBlockInnerORD_set but not in ORD_set, 
		// it will not get passed back up, since we have not touched
		// ORD_set.  This is as it should be: We cannot use it 
		// above this STAR.
		ObjectBlockInnerORD_set = ORD_set;
		if (!m_block_string0->symbolObjectReferences(pEE, bResult, ObjectBlockInnerORD_set))
			return false;

		return true;
	}
}

bool BlockString1::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (m_pMQLMSE == 0) {
		return m_block_string0->symbolObjectReferences2(pEE);
	} else {
		std::set<std::string> ObjectBlockInnerORD_set;
		// If any ORD is in ObjectBlockInnerORD_set but not in ORD_set, 
		// it will not get passed back up, since we have not touched
		// ORD_set.  This is as it should be: We cannot use it 
		// above this STAR.
		if (!m_block_string0->symbolObjectReferences2(pEE))
			return false;

		return true;
	}
}

bool BlockString1::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	return m_block_string0->type(pEE, contextRangeType, bResult);
}

void BlockString1::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB) 
{
	m_block_string0->calculateMMapOutermost(mmap, prefix, pDB);
}

void BlockString1::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	m_block_string0->calculateMMapInnermost(mmap, inner_charstring, pDB);
}
      


bool BlockString1::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	// Do it for this object
	if (!m_block_string0->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap))
		return false;
	return true;
}

void BlockString1::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	if (isNonStar()) {
		// Recurse
		m_block_string0->canChooseAQStrategyInnermostFirst(bResult);
	} else {
		// We currently don't know how to do Innermost First if we have a Kleene Star
		bResult = false;
	}
}

bool BlockString1::hasObjectBlockInBlockString()
{
	return m_block_string0->hasObjectBlockInBlockString();
}


void BlockString1::addOBBToVec(OBBVec *pOBBVec)
{
	m_block_string0->addOBBToVec(pOBBVec);
}

StartMonadIterator* BlockString1::getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm)
{
	if (m_pMQLMSE == 0) {
		return m_block_string0->getSMI(pEE, U, Su, Sm);
	} else {
		// We are block_string0 STAR { set } 
		if (m_pStarSOM->isMemberOf(0)) {
			// If the set contains 0,
			// we cannot rely on the inst
			// that might arise from calling
			// m_block_string0->getSMI(...).
			// Instead, we will try the whole substrate...
			return new MonadSetStartMonadIterator(Su, Sm, false);
		} else {
			return m_block_string0->getSMI(pEE, U, Su, Sm);
		}
	}
}

////////////////////////////////////////////////////////////
//
// BlockString2
//
////////////////////////////////////////////////////////////
BlockString2::BlockString2(BlockString1 *block_string1)
	: QueryNode(kQNKBlockString2),
	  m_block_string1(block_string1),
	  m_block_string2(0),
	  m_bIsExclamation(false)
{
}

BlockString2::BlockString2(BlockString1 *block_string1, BlockString2 *block_string2, bool bHasExclamation)
	: QueryNode(kQNKBlockString2),
	  m_block_string1(block_string1),
	  m_block_string2(block_string2),
	  m_bIsExclamation(bHasExclamation)
{
}

BlockString2::~BlockString2()
{
	delete m_block_string1;
	delete m_block_string2;
}

bool BlockString2::firstBlockIsPower()
{
	return m_block_string1->firstBlockIsPower();
}

void BlockString2::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	if (isBlockString1()) {
		m_block_string1->getOutermostSetOfMonadsFeatureSet(som_name_set);
	} else {
		// We are an OR at the outermost level
		m_block_string1->getOutermostSetOfMonadsFeatureSet(som_name_set);
		m_block_string2->getOutermostSetOfMonadsFeatureSet(som_name_set);
	}
}



void BlockString2::weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, bool bPrevIsGap, bool bPrevIsPower, node_number_t block_string2_parent)
{
	// Set m_node_number, and add to pEE->m_node_vector.
	pEE->getNextNodeNumber(this);

	m_block_string2_parent = block_string2_parent;

	bool bBS1BlockIsGap = false;
	bool bBS1BlockIsPower = false;
	
	if (m_block_string1->getBlockString0()->isBlock()) {
		eBlockKind BS1BlockKind = m_block_string1->getBlockString0()->getBlock()->getKind();
		if (BS1BlockKind == kOptGapBlock
		    || BS1BlockKind == kGapBlock) {
			bBS1BlockIsGap = true;
		}
		bBS1BlockIsPower = BS1BlockKind == kPowerBlock;
	}
	
	bool mybPrevIsGap = bBS1BlockIsGap;
	bool mybPrevIsPower = bBS1BlockIsPower;

	if (isBlockString1()) {
		m_block_string1->weed(pEE,bResult,is_first,is_last,bPrevIsGap,bPrevIsPower, getNodeNumber());
		if (bResult 
		    && bBS1BlockIsPower && isBlockString1()) {
			bResult = false;
			pEE->pError->appendError("Error: You cannot have a power block (i.e., \"..\") at the end!\n");
		}
	} else {
		if (bPrevIsGap 
		    && bBS1BlockIsGap) {
			bResult = false;
			pEE->pError->appendError("Error: You cannot have two gap_blocks or opt_gap_blocks next to each other,\n"
						 "       nor can you have an opt_gap_block next to a gap_block.\n");
		}

		if (bPrevIsPower
		    && bBS1BlockIsPower) {
			bResult = false;
			pEE->pError->appendError("Error: You cannot have two power blocks (i.e., \"..\") next to each other!\n");
		}

		if (bResult) {
			m_block_string1->weed(pEE,bResult, is_first, false, bPrevIsGap, bPrevIsPower, getNodeNumber());

			if (!bResult)
				return;

			//
			// We deliberately do not use m_node_number
			// for passing to the siblings.  We are *not*
			// their parent.  block_string2_parent is!
			//
			m_block_string2->weed(pEE, bResult, false, is_last, mybPrevIsGap, mybPrevIsPower, block_string2_parent);
		}
	}
}


bool BlockString2::symbol(MQLExecEnv *pEE, bool& bResult)
{
	if (isBlockString1()) {
		return m_block_string1->symbol(pEE, bResult);
	} else {
		if (!m_block_string1->symbol(pEE, bResult))
			return false;
		if (!bResult)
			return true;
		return m_block_string2->symbol(pEE, bResult);
	}
}

bool BlockString2::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (isBlockString1()) {
		return m_block_string1->symbolObjectReferences(pEE, bResult, ORD_set);
	} else {
		if (!m_block_string1->symbolObjectReferences(pEE, bResult, ORD_set))
			return false;
		if (!bResult)
			return true;
		return m_block_string2->symbolObjectReferences(pEE, bResult, ORD_set);
	}
}

bool BlockString2::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (isBlockString1()) {
		return m_block_string1->symbolObjectReferences2(pEE);
	} else {
		if (!m_block_string1->symbolObjectReferences2(pEE))
			return false;
		return m_block_string2->symbolObjectReferences2(pEE);
	}
}

bool BlockString2::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	if (isBlockString1()) {
		return m_block_string1->type(pEE, contextRangeType, bResult);
	} else {
		if (!m_block_string1->type(pEE, contextRangeType, bResult))
			return false;
		if (!bResult)
			return true;
		bool bReturnValue = 
			m_block_string2->type(pEE, contextRangeType, bResult);

		// Insert opt_gap_block between block
		// and block_str if contextRangeType is kORTMultipleRange
		// and the first block of rest_of_block_str is not a gap block
		// or an opt_gap_block.
		bool bPrevBlockKindBad = false;
		if (m_block_string1->getBlockString0()->isBlock()) {
			eBlockKind BS1BlockKind = m_block_string1->getBlockString0()->getBlock()->getKind();
			if (BS1BlockKind == kOptGapBlock
			    || BS1BlockKind == kGapBlock
			    || BS1BlockKind == kPowerBlock) {
				bPrevBlockKindBad = true;
			}
		}
		bool bNextBlockKindBad = false;
		if (m_block_string2->getBlockString1()->getBlockString0()->isBlock()) {
			eBlockKind nextBlockKind = m_block_string2->getBlockString1()->getBlockString0()->getBlock()->getKind();
			if (nextBlockKind == kOptGapBlock
			    || nextBlockKind == kGapBlock
			    || nextBlockKind == kPowerBlock) {
				bNextBlockKindBad = true;
			}
		}
		if (!m_bIsExclamation
		    && contextRangeType == kORTMultipleRange
		    && !bPrevBlockKindBad
		    && !bNextBlockKindBad) {
			// Insert an opt_gap_block between block_string1 and block_string2
			// if the conditions are right...
			OptGapBlock* pOGB = new OptGapBlock(kNoRetrieve, new std::string(""), 0);
			Block* pB = new Block(pOGB);
			BlockString0 *pBS0 = new BlockString0(pB);
			BlockString1 *pBS1 = new BlockString1(pBS0);
			BlockString2 *pBS2 = new BlockString2(pBS1, m_block_string2, false);
			m_block_string2 = pBS2;
		}
		
		return bReturnValue;
	}
}

void BlockString2::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB) 
{
	m_block_string1->calculateMMapOutermost(mmap, prefix, pDB);
	if (m_block_string2 != 0) {
		m_block_string2->calculateMMapOutermost(mmap, prefix, pDB);
	}
}

void BlockString2::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	std::string block_string1_inner_charstring;
	m_block_string1->calculateMMapInnermost(mmap, block_string1_inner_charstring, pDB);
	if (m_block_string2 != 0) {
		std::string block_string2_inner_charstring;
		m_block_string2->calculateMMapInnermost(mmap, block_string2_inner_charstring, pDB);
		inner_charstring = block_string1_inner_charstring + ";" + block_string2_inner_charstring;
	} else {
		inner_charstring = block_string1_inner_charstring;
	}
}
      


bool BlockString2::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	switch (strategy) {
	case kAQSOutermostFirst: {
		// Do it for this object
		if (!m_block_string1->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap))
			return false;
		
		// Recurse down through the rest_of_block_str
		if (m_block_string2 != 0) {
			if (!m_block_string2->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap))
			return false;
		}
	}
		break;
	case kAQSInnermostFirst: {
		FastSetOfMonads CS1;
		// Do it for this object
		if (!m_block_string1->aggregateQuery(pEE, CS1, Su, strategy, largest_object_length_above, mmap))
			return false;
		
		characteristic_set.unionWith(CS1);

		if (m_block_string2 != 0) {
			FastSetOfMonads CS2;
			
			// Recurse down through the rest_of_block_str
			if (!m_block_string2->aggregateQuery(pEE, CS2, Su, strategy, largest_object_length_above, mmap))
				return false;

			characteristic_set.unionWith(CS2);
		}
	}
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown aggregate query strategy");
		break;
	}
	return true;
}

bool BlockString2::hasObjectBlockInBlockString()
{
	if (isBlockString1()) {
		return m_block_string1->hasObjectBlockInBlockString();
	} else {
		return m_block_string1->hasObjectBlockInBlockString()
			|| m_block_string2->hasObjectBlockInBlockString();
	}
}


void BlockString2::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	if (!hasObjectBlockInBlockString()) {
		// We must have at least one object block (not gap
		// block) in order to succeed with the InnermostFirst
		// strategy.
		bResult = false;
	}

	if (bResult) {
		if (isBlockString1()) {
			// Recurse
			m_block_string1->canChooseAQStrategyInnermostFirst(bResult);
		} else {
			// Recurse
			m_block_string1->canChooseAQStrategyInnermostFirst(bResult);
			if (bResult) {
				m_block_string2->canChooseAQStrategyInnermostFirst(bResult);
			}
		}
	}
}


void BlockString2::addOBBToVec(OBBVec *pOBBVec)
{
	if (isBlockString1()) {
		m_block_string1->addOBBToVec(pOBBVec);
	} else {
		m_block_string1->addOBBToVec(pOBBVec);
		m_block_string2->addOBBToVec(pOBBVec);
	}
}

Power *BlockString2::getPowerBlock()
{
	ASSERT_THROW(!isBlockString1(),
		     "BlockString2::getPowerBlock() should not be called if this is not a sequence!");
	BlockString0 *pBS0 = getBlockString2()->getBlockString1()->getBlockString0();
	if (pBS0->isBlock()) {
		Power *pPowerBlock = pBS0->getBlock()->getPowerBlock();
		ASSERT_THROW(pPowerBlock != 0,
			     "BlockString2::getPowerBlock(): block should be a power block!");
		return pPowerBlock;
	} else {
		ASSERT_THROW(false,
			     "BlockString2::getPowerBlock() should not be called if this is not a sequence\nwith a power block at the beginning.");
	}
}

bool BlockString2::nextIsPowerBlock()
{
	ASSERT_THROW(!isBlockString1(),
		     "BlockString2::nextIsPowerBlock() should not be called if this is not a sequence!");

	BlockString0 *pBS0 = getBlockString2()->getBlockString1()->getBlockString0();
	if (pBS0->isBlock()) {
		eBlockKind nextBlockKind = pBS0->getBlock()->getKind();
		return nextBlockKind == kPowerBlock;
	} else {
		return false;
	}
}


void BlockString2::addORDChild(const std::string& ORD)
{
	std::string ORD_lower;
	str_tolower(ORD, ORD_lower);
	m_ORD_children.insert(ORD_lower);
}


bool BlockString2::hasORDChild(const std::string& ORD)
{
	std::string ORD_lower;
	str_tolower(ORD, ORD_lower);
	return m_ORD_children.find(ORD_lower) != m_ORD_children.end();
}


ObjectBlock* BlockString2::getImmediateObjectBlock(void)
{
	BlockString0 *pBS0 = getBlockString1()->getBlockString0();
	
	if (pBS0->isBlock()) {
		Block *pBlock = pBS0->getBlock();

		eBlockKind nextBlockKind = pBlock->getKind();

		bool bIsObjectBlock = (nextBlockKind == kObjectBlock)
			|| (nextBlockKind == kObjectBlockNOTEXIST);

		if (bIsObjectBlock) {
			return pBlock->getObjectBlock();
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}


bool BlockString2::hasImmediateORDChild(const std::string& ORD)
{
	ObjectBlock *pObjectBlockOrNil = this->getImmediateObjectBlock();
	if (pObjectBlockOrNil == 0) {
		return false;
	} else {
		const std::string& OB_ORD = pObjectBlockOrNil->getObjectReference();
		if (strcmp_nocase(ORD, OB_ORD) == 0) {
			return true;
		} else {
			return false;
		}
	}
}



////////////////////////////////////////////////////////////
//
// BlockString
//
////////////////////////////////////////////////////////////
BlockString::BlockString(BlockString2 *block_string2)
	: m_block_string2(block_string2),
	  m_block_string(0)
{
}

BlockString::BlockString(BlockString2 *block_string2, BlockString *block_string)
	: m_block_string2(block_string2),
	  m_block_string(block_string)
{
}

BlockString::~BlockString()
{
	delete m_block_string2;
	delete m_block_string;
}

void BlockString::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	if (isBlockString2()) {
		m_block_string2->getOutermostSetOfMonadsFeatureSet(som_name_set);
	} else {
		// We are an OR at the outermost level
		m_block_string2->getOutermostSetOfMonadsFeatureSet(som_name_set);
		m_block_string->getOutermostSetOfMonadsFeatureSet(som_name_set);
	}
}



void BlockString::weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, bool bPrevIsGap, bool bPrevIsPower, node_number_t block_string2_parent)
{
	if (m_block_string2->firstBlockIsPower()) {
		bResult = false;
		pEE->pError->appendError("The first block in a context cannot be a power block (i.e., \"..\").\n");
	}
	if (!bResult)
		return;
	if (isBlockString2()) {
		m_block_string2->weed(pEE, bResult, is_first, is_last, bPrevIsGap, bPrevIsPower, block_string2_parent);
	} else {
		m_block_string2->weed(pEE, bResult, true, true, false, false, block_string2_parent);
		if (bResult) {
			m_block_string->weed(pEE, bResult, true, true, false, false, block_string2_parent);
		}
	}
}

bool BlockString::firstBlockIsPower()
{
	if (isBlockString2()) {
		return m_block_string2->firstBlockIsPower();
	} else {
		return m_block_string2->firstBlockIsPower() 
			|| m_block_string->firstBlockIsPower();
	}
}


bool BlockString::symbol(MQLExecEnv *pEE, bool& bResult)
{
	if (isBlockString2()) {
		return m_block_string2->symbol(pEE, bResult);
	} else {
		if (!m_block_string2->symbol(pEE, bResult))
			return false;
		if (!bResult)
			return true;
		return m_block_string->symbol(pEE, bResult);
	}
}

bool BlockString::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (isBlockString2()) {
		return m_block_string2->symbolObjectReferences(pEE, bResult, ORD_set);
	} else {
		// Being in the set at this point means that it has been 
		// declared at a higher level, so it is OK to reference it 
		// from here on.
		std::set<std::string> BlockStrORD_set = ORD_set;
		if (!m_block_string2->symbolObjectReferences(pEE, bResult, BlockStrORD_set))
			return false;
		if (!bResult)
			return true;

		// However, if it was declared in BlockStrORD_set but
		// it is not in ORD_set at this point, then we remove it 
		// now, to show that we should not use it...
		std::set<std::string> BlockStringORD_set = BlockStrORD_set;
		std::set<std::string>::const_iterator blockstrord_ci;
		for (blockstrord_ci = BlockStrORD_set.begin();
		     blockstrord_ci != BlockStrORD_set.end();
		     ++blockstrord_ci) {
			// If it was not in the ORD_set (the parent set),
			// it was declared in the block_str, which means
			// we have an export barrier right here.
			if (ORD_set.find(*blockstrord_ci) == ORD_set.end()) {
				// so we remove it from BlockStringORD_set.
				BlockStringORD_set.erase(BlockStringORD_set.find(*blockstrord_ci));
			}
		}

		if (!m_block_string->symbolObjectReferences(pEE, bResult, BlockStringORD_set)) {
			return false;
		}
		if (!bResult) {
			return true;
		}

		// Now we can pass it back up the chain safely,
		// since we have passed the OR.
		ORD_set = BlockStringORD_set;

		return true;
	}
}

bool BlockString::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (isBlockString2()) {
		return m_block_string2->symbolObjectReferences2(pEE);
	} else {
		if (!m_block_string2->symbolObjectReferences2(pEE))
			return false;

		if (!m_block_string->symbolObjectReferences2(pEE)) {
			return false;
		}

		return true;
	}
}

bool BlockString::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	if (isBlockString2()) {
		return m_block_string2->type(pEE, contextRangeType, bResult);
	} else {
		if (!m_block_string2->type(pEE, contextRangeType, bResult))
			return false;
		if (!bResult)
			return true;
		return m_block_string->type(pEE, contextRangeType, bResult);
	}
}

void BlockString::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB) 
{
	m_block_string2->calculateMMapOutermost(mmap, prefix, pDB);
	if (m_block_string != 0) {
		m_block_string->calculateMMapOutermost(mmap, prefix, pDB);
	}
}

void BlockString::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	std::string block_string2_inner_charstring;
	m_block_string2->calculateMMapInnermost(mmap, block_string2_inner_charstring, pDB);
	if (m_block_string != 0) {
		std::string block_string_inner_charstring;
		m_block_string->calculateMMapInnermost(mmap, block_string_inner_charstring, pDB);
		inner_charstring = block_string2_inner_charstring + ";" + block_string_inner_charstring;
	} else {
		inner_charstring = block_string2_inner_charstring;
	}
}
      


bool BlockString::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	// Do it for this object
	switch (strategy) {
	case kAQSOutermostFirst: {
		if (!m_block_string2->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap))
			
			return false;
		
		// Recurse down through the rest_of_block_str
		if (m_block_string != 0) {
			if (!m_block_string->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap))
				return false;
		}
	}
		break;
	case kAQSInnermostFirst: {
		FastSetOfMonads CS;
		if (!m_block_string2->aggregateQuery(pEE, CS, Su, strategy, largest_object_length_above, mmap))
			return false;
		
		characteristic_set.unionWith(CS);
		
		if (m_block_string != 0) {
			FastSetOfMonads CS1;
			if (!m_block_string->aggregateQuery(pEE, CS1, Su, strategy, largest_object_length_above, mmap))
				return false;

			characteristic_set.unionWith(CS1);
		}
	}
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown aggregate query strategy");
	}

	return true;
}

void BlockString::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	if (isBlockString2()) {
		// Recurse
		m_block_string2->canChooseAQStrategyInnermostFirst(bResult);
	} else {
		m_block_string2->canChooseAQStrategyInnermostFirst(bResult);
		if (bResult) {
			m_block_string->canChooseAQStrategyInnermostFirst(bResult);
		}
	}
}

bool BlockString::hasObjectBlockInBlockString()
{
	if (isBlockString2()) {
		return m_block_string2->hasObjectBlockInBlockString();
	} else {
		return m_block_string2->hasObjectBlockInBlockString()
			|| m_block_string->hasObjectBlockInBlockString();
	}
}


void BlockString::addOBBToVec(OBBVec *pOBBVec)
{
	if (isBlockString2()) {
		m_block_string2->addOBBToVec(pOBBVec);
	} else {
		m_block_string2->addOBBToVec(pOBBVec);
		m_block_string->addOBBToVec(pOBBVec);
	}
}

StartMonadIterator* BlockString::getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm)
{
	if (isBlockString2()) {
		return m_block_string2->getBlockString1()->getSMI(pEE, U, Su, Sm);
	} else {
		StartMonadIterator *pBS2_SMI = m_block_string2->getBlockString1()->getSMI(pEE, U, Su, Sm);
		StartMonadIterator *pBS_SMI = m_block_string->getSMI(pEE, U, Su, Sm);
		monad_m new_Sm = Sm;
		SetOfMonads new_Su = pBS2_SMI->getSu();
		if (!new_Su.isEmpty()) {
			monad_m new_Su_first = new_Su.first();
			new_Sm = MYMIN(new_Su_first, new_Sm);
		}
		new_Su.unionWith(pBS_SMI->getSu());
		if (!new_Su.isEmpty()) {
			monad_m new_Su_first = new_Su.first();
			new_Sm = MYMIN(new_Su_first, new_Sm);
		}

		delete pBS2_SMI;
		delete pBS_SMI;

		return new MonadSetStartMonadIterator(new_Su, new_Sm, false);
	}
}



////////////////////////////////////////////////////////////
//
// ObjectBlockString
//
////////////////////////////////////////////////////////////
ObjectBlockString::ObjectBlockString(ObjectBlock *object_block)
	: m_object_block_string(0),
	  m_object_block(object_block)
{
}


ObjectBlockString::ObjectBlockString(ObjectBlockString *object_block_string, ObjectBlock *object_block)
	: m_object_block_string(object_block_string),
	  m_object_block(object_block)
{
}


ObjectBlockString::~ObjectBlockString()
{
	delete m_object_block_string;
	delete m_object_block;
}


long ObjectBlockString::getObjectBlockCount()
{
	if (m_object_block_string == 0) {
		return 1;
	} else {
		return m_object_block_string->getObjectBlockCount() + 1;
	}
}


void ObjectBlockString::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	if (m_object_block_string != 0) {
		m_object_block_string->getOutermostSetOfMonadsFeatureSet(som_name_set);
	}

	std::string som_name;
	str_tolower(m_object_block->getMSRC()->getOurMonadSet(), som_name);
	som_name_set.insert(som_name);
}


void ObjectBlockString::weed(MQLExecEnv *pEE, bool& bResult, node_number_t block_string2_parent)
{
	if (m_object_block_string != 0) {
		m_object_block_string->weed(pEE, bResult, block_string2_parent);
	}
	if (!bResult) {
		return;
	}
	
	const bool is_first = false;
	const bool is_last = false;
	m_object_block->weed(pEE, bResult, is_first, is_last, block_string2_parent);
	if (!bResult) {
		return; // Defensive programming.
	}
}


bool ObjectBlockString::symbol(MQLExecEnv *pEE, bool& bResult)
{
	if (m_object_block_string != 0) {
		if (!m_object_block_string->symbol(pEE, bResult)) {
			return false;
		}
		if (!bResult) {
			return true;
		}
	}

	const bool bCalculatePreQueryString = true;
	if (!m_object_block->symbol(pEE, bCalculatePreQueryString, bResult)) {
		return false;
	}
	if (!bResult) {
		return true; // Defensive programming.
	}
	return true;
}

bool ObjectBlockString::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (m_object_block_string != 0) {
		if (!m_object_block_string->symbolObjectReferences(pEE, bResult, ORD_set)) {
			return false;
		}
		if (!bResult) {
			return true;
		}
	}

	// If any ORD is in ObjectBlockInnerORD_set but not in
	// ORD_set, it will not get passed back up, since we have not
	// touched ORD_set.  This is as it should be: We cannot use it
	// above this ObjectBlock, nor can we use it between the
	// ObjectBlocks in the UnorderedBlock.
	std::set<std::string> ObjectBlockInnerORD_set = ORD_set;
	if (!m_object_block->symbolObjectReferences(pEE, bResult, ObjectBlockInnerORD_set)) {
		return false;
	}
	
	if (!bResult) {
		return true; // Defensive programming
	}

	// If we got this far: No DB errors, no compiler errors.
	return true;
}

bool ObjectBlockString::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (m_object_block_string != 0) {
		if (!m_object_block_string->symbolObjectReferences2(pEE)) {
			return false;
		}
	}

	if (!m_object_block->symbolObjectReferences2(pEE)) {
		return false;
	}
	
	// If we got this far: No DB errors.
	return true;
}

bool ObjectBlockString::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	if (m_object_block_string != 0) {
		if (!m_object_block_string->type(pEE, contextRangeType, bResult)) {
			return false;
		}
		if (!bResult) {
			return true;
		}
	}

	if (!m_object_block->type(pEE, contextRangeType, bResult)) {
		return false;
	}

	if (!bResult) {
		return true; // Defensive programming.
	}

	return true;
}


StartMonadIterator* ObjectBlockString::getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm)
{
	return m_object_block->getSMI(pEE, U, Su, Sm);
}

void ObjectBlockString::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB)
{
	std::string localprefix = prefix + "unorderedgroup,";
	if (m_object_block_string != 0) {
		m_object_block_string->calculateMMapOutermost(mmap, localprefix, pDB);
	}
	m_object_block->calculateMMapOutermost(mmap, localprefix, pDB);
}

void ObjectBlockString::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	std::string object_block_string_inner_charstring;
	if (m_object_block_string != 0) {
		m_object_block_string->calculateMMapInnermost(mmap, object_block_string_inner_charstring, pDB);
	}
	std::string object_block_inner_charstring;
	m_object_block->calculateMMapInnermost(mmap, object_block_inner_charstring, pDB);
	inner_charstring = object_block_inner_charstring + ";" + object_block_string_inner_charstring;
}
      


void ObjectBlockString::addOBBToVec(OBBVec *pOBBVec)
{
	if (m_object_block_string != 0) {
		m_object_block_string->addOBBToVec(pOBBVec);
	}
						
	m_object_block->addOBBToVec(pOBBVec);
}

bool ObjectBlockString::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	switch (strategy) {
	case kAQSOutermostFirst: {
		if (m_object_block_string != 0) {
			if (!m_object_block_string->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap)) {
				return false;
			}
		}
		
		if (!m_object_block->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap)) {
				return false;
		}
	}
		break;
	case kAQSInnermostFirst: {
		FastSetOfMonads CS1;
		if (m_object_block_string != 0) {
			if (!m_object_block_string->aggregateQuery(pEE, CS1, Su, strategy, largest_object_length_above, mmap)) {
				return false;
			}
		}
		
		FastSetOfMonads CS2;
		if (!m_object_block->aggregateQuery(pEE, CS2, Su, strategy, largest_object_length_above, mmap)) {
			return false;
		}
		characteristic_set.unionWith(CS1);
		characteristic_set.unionWith(CS2);
	}
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown aggregate query strategy");
	}			
	return true;
}


////////////////////////////////////////////////////////////
//
// UnorderedGroup
//
////////////////////////////////////////////////////////////
UnorderedGroup::UnorderedGroup(ObjectBlockString *object_block_string)
	: m_object_block_string(object_block_string)
{
}


UnorderedGroup::~UnorderedGroup()
{
	delete m_object_block_string;
}


void UnorderedGroup::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	m_object_block_string->getOutermostSetOfMonadsFeatureSet(som_name_set);
}


void UnorderedGroup::weed(MQLExecEnv *pEE, bool& bResult, node_number_t block_string2_parent)
{
	m_object_block_string->weed(pEE, bResult, block_string2_parent);
}


bool UnorderedGroup::symbol(MQLExecEnv *pEE, bool& bResult)
{
	return m_object_block_string->symbol(pEE, bResult);
}


bool UnorderedGroup::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	return m_object_block_string->symbolObjectReferences(pEE, bResult, ORD_set);
}


bool UnorderedGroup::symbolObjectReferences2(MQLExecEnv *pEE)
{
	return m_object_block_string->symbolObjectReferences2(pEE);
}


bool UnorderedGroup::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	return m_object_block_string->type(pEE, contextRangeType, bResult);
}


bool UnorderedGroup::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	return m_object_block_string->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap);
}


void UnorderedGroup::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	// We don't yet know how to choose innermost as an
	// AggregateQuery strategy when using an UnorderedGroup.
	bResult = false;
}



bool UnorderedGroup::firstBlockIsPower()
{
	return false;
}


void UnorderedGroup::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB)
{
	m_object_block_string->calculateMMapOutermost(mmap, prefix, pDB);
}

void UnorderedGroup::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	std::string object_block_string_inner_charstring;
	m_object_block_string->calculateMMapInnermost(mmap, object_block_string_inner_charstring, pDB);
	inner_charstring = "unorderedgroup," + object_block_string_inner_charstring;
}
      



void UnorderedGroup::addOBBToVec(OBBVec *pOBBVec)
{
	m_object_block_string->addOBBToVec(pOBBVec);
}




////////////////////////////////////////////////////////////
//
// Blocks 
//
////////////////////////////////////////////////////////////
Blocks::Blocks(BlockString* block_string, UsingRange *using_range)
	: m_block_string(block_string),
	  m_using_range(using_range),
	  m_unordered_group(0)
{
}

Blocks::Blocks(UnorderedGroup *unordered_group, UsingRange *using_range)
	: m_block_string(0),
	  m_using_range(using_range),
	  m_unordered_group(unordered_group)
{
}

Blocks::~Blocks()
{
	delete m_block_string;
	delete m_using_range;
	delete m_unordered_group;
}

void Blocks::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	if (isBlockString()) {
		m_block_string->getOutermostSetOfMonadsFeatureSet(som_name_set);
	} else {
		m_unordered_group->getOutermostSetOfMonadsFeatureSet(som_name_set);
	}
}


void Blocks::weed(MQLExecEnv *pEE, bool& bResult, node_number_t block_string2_parent)
{
	if (isBlockString()) {
		m_block_string->weed(pEE, bResult, true, true, false, false, block_string2_parent);
		if (bResult) {
			if (m_using_range) {
				m_using_range->weed(pEE, bResult);
			}
		}
	} else {
		m_unordered_group->weed(pEE, bResult, block_string2_parent);
	}
}

bool Blocks::symbol(MQLExecEnv *pEE, bool& bResult)
{
	if (isBlockString()) {
		return m_block_string->symbol(pEE, bResult);
	} else {
		return m_unordered_group->symbol(pEE, bResult);
	}
}

bool Blocks::symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set)
{
	if (isBlockString()) {
		return m_block_string->symbolObjectReferences(pEE, bResult, ORD_set);
	} else {
		return m_unordered_group->symbolObjectReferences(pEE, bResult, ORD_set);
	}
}


bool Blocks::symbolObjectReferences2(MQLExecEnv *pEE)
{
	if (isBlockString()) {
		return m_block_string->symbolObjectReferences2(pEE);
	} else {
		return m_unordered_group->symbolObjectReferences2(pEE);
	}
}


bool Blocks::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	if (isBlockString()) {
		return m_block_string->type(pEE, contextRangeType, bResult);	
	} else {
		return m_unordered_group->type(pEE, contextRangeType, bResult);
	}
}

bool Blocks::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap)
{
	if (isBlockString()) {
		return m_block_string->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap);
	} else {
		return m_unordered_group->aggregateQuery(pEE, characteristic_set, Su, strategy, largest_object_length_above, mmap);
	}
}

void Blocks::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	if (isBlockString()) {
		// Recurse
		m_block_string->canChooseAQStrategyInnermostFirst(bResult);
	} else {
		m_unordered_group->canChooseAQStrategyInnermostFirst(bResult);
	}
}



void Blocks::calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB) 
{
	if (isBlockString()) {
		m_block_string->calculateMMapOutermost(mmap, prefix, pDB);
	} else {
		m_unordered_group->calculateMMapOutermost(mmap, prefix, pDB);
	}
}

void Blocks::calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB)
{
	if (isBlockString()) {
		m_block_string->calculateMMapInnermost(mmap, inner_charstring, pDB);
	} else {
		m_unordered_group->calculateMMapInnermost(mmap, inner_charstring, pDB);
	}
}
      


void Blocks::addOBBToVec(OBBVec *pOBBVec)
{
	if (isBlockString()) {
		m_block_string->addOBBToVec(pOBBVec);
	} else {
		m_unordered_group->addOBBToVec(pOBBVec);
	}
}



////////////////////////////////////////////////////////////
//
// Topograph 
//
////////////////////////////////////////////////////////////
Topograph::Topograph(Blocks* blocks)
	: m_blocks(blocks)
{
}

Topograph::~Topograph()
{
	delete m_blocks;
}

void Topograph::getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set)
{
	m_blocks->getOutermostSetOfMonadsFeatureSet(som_name_set);
}


void Topograph::weed(MQLExecEnv *pEE, bool& bResult)
{
	m_blocks->weed(pEE, bResult, -1);
}

bool Topograph::symbol(MQLExecEnv *pEE, bool& bResult)
{ 
	// Symbol-check
	if (!m_blocks->symbol(pEE, bResult)) {
		return false;
	} else {
		if (bResult) {
			std::set<std::string> ORD_set;
			bool bDBOK = m_blocks->symbolObjectReferences(pEE, bResult, ORD_set);

			if (!bDBOK || !bResult) {
				return bDBOK;
			}

			return m_blocks->symbolObjectReferences2(pEE);
		} else {
			return true;
		}
	}
}

bool Topograph::type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult)
{
	// Type-check
	return m_blocks->type(pEE, contextRangeType, bResult);
}

bool Topograph::aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy)
{
	return m_blocks->aggregateQuery(pEE, characteristic_set, Su, strategy, 0, m_mmap);
}

void Topograph::canChooseAQStrategyInnermostFirst(bool &bResult)
{
	// Assume we can, until proven otherwise
	bResult = true;

	// Recurse
	m_blocks->canChooseAQStrategyInnermostFirst(bResult);
}

void Topograph::calculateMMap(EMdFDB *pDB, eAggregateQueryStrategy strategy)
{
	std::string inner_charstr;
	switch (strategy) {
	case kAQSOutermostFirst:
		m_blocks->calculateMMapOutermost(m_mmap, "", pDB);
		break;
	case kAQSInnermostFirst: 
		m_blocks->calculateMMapInnermost(m_mmap, inner_charstr, pDB);
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown aggregate query strategy");
		break;
	}
}

void Topograph::addOBBToVec(OBBVec *pOBBVec)
{
	m_blocks->addOBBToVec(pOBBVec);
}
