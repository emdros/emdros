/*
 * mql_R.cpp
 *
 * MQL retrieval functions
 *
 * Ulrik Petersen
 * Created: 3/16-2001
 * Last update: 2/4-2019
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


/**@file mql_R.cpp
 *@brief Implementation of MQL R_ functions (topographic queries) (MQL layer)
 */

#include <vector>
#include <algorithm>
#include <emdfdb.h>
#include <inst.h>
#include <logging.h>
#include <mql_object.h>
#include <mql_extern.h>
#include <mql_sheaf.h>
#include <mql_utility.h>
#include <mql_query.h>
#include <mql_execution_environment.h>
#include <mql_R.h>
#include <debug.h>

#undef MYMIN
#define MYMIN(A, B) ((A < B) ? A : B)


/*
 * NOTE: On January 2, 2005, I made the following experiment: Take the
 * parameters pEE, U, Su, pInst, limit_low, and limit_high, and wrap
 * them in a class with public members.  Then allocate a new object of
 * this class on the stack each time we needed to pass new parameters.
 * Then see if this made any difference to the execution time.
 *
 * It did.  The engine became about 3% slower.
 *
 */

/* NOTE: On January 2, 2005, I made the following experiment: Wrap
 * limit_low and limit_high in the Power object that they came from
 * (i.e., pass a const Power& reference).  This gave a slowdown of
 * about 1%.
 */

/* NOTE: On January 2, 2005, I made the following experiment: Add a
 * ListOfMoNR *pLOMN member to ObjectBlock and GapBlock, also adding
 * accessor functions, and modifying Block to forward the setter
 * function.  Then wrap the pLOMN argument to the R_ functions inside
 * the block.  As is probably clear by now, this did not produce a
 * speed increase, but rather a speed decrease!
 */

/*
 * Invariants:
 * 
 * I1) There may not be two gap_blocks next to each other; not even an
 * opt_gap_block next to a gap_block. (Weeder)
 * 
 * I2) There may not be an object reference declaration on a STAR or
 * NOTEXIST block. (Weeder)
 *
 * I3) Obsolete -- it used to be that you could only have NOTEXIST as
 * the only block within a blocks.  No more.
 *
 * I4) STAR blocks are pre-queried.  However, if 0 is in the set of
 * integers to use with STAR, it gets no pre-query string.  This is
 * because we wish to retrieve all such objects, so that we can ignore
 * the ones that don't match (i.e., make an EMPTY_MO for them).
 * (Block::symbol).
 *
 * I5) NOTEXIST blocks are always marked as NORETRIEVE. (mql_query.cpp)
 * 
 * I6) EMPTY_MO has an associated monad that is the monad 1 before the
 * thing that "matched": If an opt_gap_block matched an "empty gap" at
 * monad Sm, we store monad Sm-1.  Likewise, if a STAR block matched 0
 * times at monad Sm, we store Sm-1. (mql_R.cpp).
 *
 * I7) A ListOfStraws is a failed ListOfStraws if and only if it is
 * empty.  Thus the old way of saying failure, namely "NIL_mo" no
 * longer applies.
 *
 * I8) The last monad of any given straw is always used to say where
 * the next match is to occur (namely 1 monad after it!).
 *
 * I9) Things like NORETRIEVE and EMPTY_MO are taken out of the straw
 * at the last minute only, namely in R_blocks, where it is put into a
 * Sheaf.  Thus the invariant above about ListOfStraws being failed if
 * and only if they are empty do not apply to Sheaves.  It is still
 * the case that an empty sheaf need not be a failed sheaf.
 *
 * I10) Any iterators (whether on a set of monads or on an Inst) are
 * only ever advanced at R_block_string_XXX level.
 * 
 * I11) An empty straw is a failed straw. An empty straw will never
 * get into any ListOfStraws.
 *
 */


//
// Forward declarations
//
void R_object_reference_declaration_inst(MQLExecEnv *pEE, id_d_t object_id_d, MQLObject *pMQLObject, short int index);
bool R_retrieval_retrieval(eRetrieval retrieval);
bool R_features(MQLExecEnv *pEE, MQLObject* pObj, FFeatures* pFFeatures);
bool R_ffeatures(MQLExecEnv *pEE, MQLObject* pObj, NonParentORDSolution *pNonParentORDSolution, FFeatures* pFFeatures, bool bContextHasBeenNegative);
Sheaf* R_opt_blocks(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m max_range, Blocks* pOpt_blocks);
void R_object_block(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm, ObjectBlock* pObject_block, ListOfStraws *pResultLOS);
Sheaf *R_blocks(MQLExecEnv *pEE, 
		const SetOfMonads& U, 
		const SetOfMonads& Su, 
		Blocks *pBlocks,
		bool bIsTheTopograph,
		monad_m max_range);
ListOfStraws *R_block_string0(MQLExecEnv *pEE, 
			     const SetOfMonads& U, 
			     const SetOfMonads& Su,
			     monad_m Sm,
			      monad_m max_range,
			     BlockString0 *pBlockString0);
ListOfStraws *R_block_string1(MQLExecEnv *pEE, 
			      const SetOfMonads& U, 
			      const SetOfMonads& Su,
			      monad_m Sm,
			      monad_m max_range,
			      BlockString1 *pBlockString1);
ListOfStraws *R_block_string2(MQLExecEnv *pEE, 
			      const SetOfMonads& U, 
			      const SetOfMonads& Su,
			      monad_m Sm,
			      monad_m max_range,
			      BlockString2 *pBlockString2);
ListOfStraws *R_block_string2_power_block(MQLExecEnv *pEE, 
					  const SetOfMonads& U, 
					  const SetOfMonads& Su, 
					  monad_m Sm, 
					  monad_m max_range,
					  BlockString2 *pBlockString2);
ListOfStraws *R_block_string2_non_power_block(MQLExecEnv *pEE, 
					      const SetOfMonads& U, 
					      const SetOfMonads& Su, 
					      monad_m Sm, 
					      monad_m max_range,
					      BlockString2 *pBlockString2);

ListOfStraws *R_object_block_string(MQLExecEnv *pEE, 
				    const SetOfMonads& U, 
				    const SetOfMonads& Su,
				    monad_m Sm,
				    monad_m max_range,
				    ObjectBlockString *pObjectBlockString);

ListOfStraws *R_block_string(MQLExecEnv *pEE, 
			     const SetOfMonads& U, 
			     const SetOfMonads& Su,
			     monad_m Sm,
			     monad_m max_range,
			     BlockString *pBlockString);
	
bool R_NOTEXIST_object_block(MQLExecEnv *pEE, 
			     const SetOfMonads& U, 
			     const SetOfMonads& Su, 
			     monad_m Sm, 
			     ObjectBlock *pObjectBlock);

bool R_retrieval_gap_retrieval(eRetrieval gap_retrieval);
ListOfStraws *R_block(MQLExecEnv *pEE, 
		      const SetOfMonads& U, 
		      const SetOfMonads& Su, 
		      monad_m Sm,
		      Block *pBlock);



Sheaf *R_topograph(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m max_range, Topograph* pTopograph)
{
	return R_blocks(pEE,U,Su,pTopograph->getBlocks(), true, max_range);
}


//
// Local helper function
//
Inst* R_inst(MQLExecEnv *pEE, const SetOfMonads& Su, ObjectBlock *object_block, eMonadSetRelationOperation *pMonadSetOperation)
{
	// Declare variable
	Inst* pInst;

	// Choose whether to use object's own inst or to calculate the inst
	if ((pInst = object_block->m_inst) != 0) {
		//
		// Use object's own inst.
		// This has been made before letting R_topograph loose on the query.
		//
	} else {
		//
		// Make inst from scratch
		//

		// Write to log
		// LOG_WRITE_TIME("mql_R.cpp:R_inst()", "Starting inst-creation.");

		// Get object
		MQLObject *pObj = object_block->getObject();

		// Get list of feature names to get
		const std::list<FeatureInfo>& FeatureInfos = pObj->getFeatureInfos();

		// Get pre-query string
		std::string pre_query_string(object_block->getPreQueryString());
		EMdFFFeatures *pre_query_constraints = object_block->getEMdFConstraints(pEE->pDB);

		// Make inst
		pInst = new Inst(pObj->getFeatureNames());

		// Calculate set of monads to pass to GetInst
		SetOfMonads characteristic_set = Su;

		// If the MonadSetRelationClause says it is the Universe
		// which we are to use, do so!
		if (object_block->getMSRC()->getUniverseOrSubstrate() == kMSNUniverse) {
			SetOfMonads U = SetOfMonads(Su.first(), Su.last());
			characteristic_set = U;
		}

		// Calculate operation
		eMonadSetRelationOperation operation;
		if (pMonadSetOperation != 0) {
			operation = *pMonadSetOperation;
		} else {
			operation = object_block->getMSRC()->getOperation();
		}
		

		// Get inst
        switch (operation) {
          case kMSROPartOf:
          case kMSROStartsIn:
              if (!pEE->pDB->getInst(object_block->getObjectTypeName(),
                                     object_block->getObjectTypeId(),
                                     characteristic_set, 
                                     pEE->m_all_m_1,
                                     pre_query_string,
                                     pre_query_constraints,
                                     FeatureInfos,
                                     object_block->getMSRC()->getOurMonadSet(),
                                     operation,
                                     *pInst)) {
                  throw EMdFDBDBError();
              }
              break;

          case kMSROOverlap:
                if (!pEE->pDB->getObjectsHavingMonadsIn(object_block->getObjectTypeName(),
                                                        object_block->getObjectTypeId(),
                                                        object_block->getObjectRangeType(),
                                                        characteristic_set, 
                                                        pEE->m_all_m_1,
							pre_query_string,
							pre_query_constraints,
                                                        FeatureInfos,
                                                        object_block->getMSRC()->getOurMonadSet(),
                                                        pInst)) {
                    throw EMdFDBDBError();
                }
                break;

          default:
                ASSERT_THROW(false,
                             "Unknown MSRC operation in R_inst.");
                break;
        }
		// Write to log
		// LOG_WRITE_TIME("mql_R.cpp:R_inst()", "Inst-creation finished.");
	} 
	return pInst;
}

Sheaf *R_blocks(MQLExecEnv *pEE, 
		const SetOfMonads& U, 
		const SetOfMonads& Su, 
		Blocks *pBlocks,
		bool bIsTheTopograph,
		monad_m max_range)
{
	ListOfStraws *pResultLOS = new ListOfStraws();
	bool bSuccess = false;

	if (pBlocks->isBlockString()) {
		bool bStoreSheafInMemory = true;
		if (bIsTheTopograph
		    && pEE->pCallback != 0) {
			pEE->m_bContinueExecution = pEE->pCallback->startSheaf(bStoreSheafInMemory);
			if (!pEE->m_bContinueExecution) {
				delete pResultLOS;
				
				pEE->pCallback->endSheaf(true);
				
				// A failed sheaf
				Sheaf *pSheaf = new Sheaf(true);
				return pSheaf;
			}
		}
		
		
		BlockString *pBS = pBlocks->getBlockString();
		
		UsingRange *pUR = pBlocks->getUsingRange();
		
		StartMonadIterator *pSMI = pBS->getSMI(pEE, U, Su, Su.first());
		
		long count = 0;
		
		while (pEE->m_bContinueExecution 
		       && pSMI->hasNext()) {
			monad_m Sm = pSMI->next();
			ListOfStraws *pLOS = R_block_string(pEE, U, Su, Sm, max_range, pBS);
			if (!pLOS->isEmpty()) {
				bSuccess = true;

				ListOfStraws *pIntermediateLOS = 0;

				if (pUR) {
					pIntermediateLOS = new ListOfStraws();
					long LOS_size = pIntermediateLOS->appendAndSubsumeWhileFiltering(pLOS);
					if (LOS_size == 0) {
						delete pIntermediateLOS;
						continue;
					}

					if (count > pUR->getEnd()) {
						// Case 5: We have come past pUR
						count += LOS_size;
						delete pIntermediateLOS;
						break;
					} else if (count + LOS_size < pUR->getStart()) {
						// Case 1: We won't reach
						// pUR's start
						count += LOS_size;
						delete pIntermediateLOS;
						continue;
					} else if (count >= pUR->getStart()
						   && count + LOS_size <= pUR->getEnd()) {
						ASSERT_THROW(count <= pUR->getEnd(),
							     "Error in logic: I thought that count <= pUR->getEnd().");

						// Case 3: pLOS is a subset or
						// proper subset of pUR.
						// Just add it.
						count += LOS_size;
						pIntermediateLOS = pLOS;
					} else if (count < pUR->getStart()
						   && count + LOS_size <= pUR->getEnd()) {
						// Case 2: We start before pUR,
						// but end within it.	
						pIntermediateLOS = new ListOfStraws();
						long mystart = pUR->getStart() - count;
						pIntermediateLOS->appendAndSubsumeWhileCounting(pLOS, mystart, LOS_size - mystart);

						count += LOS_size;

						delete pLOS;
					} else if (count < pUR->getStart()
						   && count + LOS_size > pUR->getEnd()) {
						//Case 6: We start before pUR,
						//and end after it.

						ListOfStraws *pIntermediateLOS2 = new ListOfStraws();


						// Cut off some inside.
						long mystart = pUR->getStart() - count;

						pIntermediateLOS2->appendAndSubsumeWhileCounting(pIntermediateLOS, mystart, pUR->getEnd() - pUR->getStart() + 1);

						pIntermediateLOS = pIntermediateLOS2;

						count += LOS_size;

						delete pLOS;
					} else {
						ASSERT_THROW(false,
							     "Logic error: We should not be able to get here!")
							}
				} else {
					pIntermediateLOS = pLOS;
				}

				if (bIsTheTopograph
				    && pEE->pCallback != 0) {
					pEE->m_bContinueExecution = pEE->pCallback->nextLOS(pIntermediateLOS);
					if (bStoreSheafInMemory) {
						pResultLOS->appendAndSubsumeWhileFiltering(pIntermediateLOS);
					} else {
						delete pIntermediateLOS;
					}
				} else {
					pResultLOS->appendAndSubsumeWhileFiltering(pIntermediateLOS);
				}
			} else {
				delete pLOS;
			}
		}

		delete pSMI;
	} else {
		// blocks ::= unordered_group .
		UnorderedGroup *pUG = pBlocks->getUnorderedGroup();

		long object_block_count = pUG->getObjectBlockCount();

		ObjectBlockString *pOBS = pUG->getObjectBlockString();

		std::list<ListOfStraws*> los_list;
		for (long i = 0; i < object_block_count; ++i) {
			los_list.push_back(new ListOfStraws());
		}

		ObjectBlockString *pMyOBS = pOBS;
		std::list<ListOfStraws*>::iterator los_list_it = los_list.begin();
		while (pMyOBS != 0) {
			ObjectBlock *pObjectBlock = pMyOBS->getObjectBlock();
			ObjectBlockString *pNextOBS = pMyOBS->getObjectBlockString();
			StartMonadIterator *pSMI = pMyOBS->getSMI(pEE, U, Su, Su.first());
			ListOfStraws *pOBLOS = *los_list_it;
			
			ListOfStraws *pLocalOBLOS = new ListOfStraws();
			
			while (pEE->m_bContinueExecution 
			       && pSMI->hasNext()) {
				monad_m Sm = pSMI->next();
				
				R_object_block(pEE, U, Su, Sm, pObjectBlock, pLocalOBLOS);
				if (!pLocalOBLOS->isEmpty()) {
					pOBLOS->appendAndSubsume(pLocalOBLOS);
					pLocalOBLOS = new ListOfStraws();
				}
			}
			
			delete pLocalOBLOS;

			delete pSMI;

			pMyOBS = pNextOBS;
			++los_list_it;
		}

		bool bHasNoEmptyLOS = true;
		std::list<ListOfStraws*>::reverse_iterator los_list_reverse_it = los_list.rbegin();
		while (los_list_reverse_it != los_list.rend()) {
			ListOfStraws *pLOS = *los_list_reverse_it;

			if (pLOS->isEmpty()) {
				bHasNoEmptyLOS = false;
				delete pLOS;
			} else {
				pResultLOS->appendAndSubsume(pLOS);
			}

			++los_list_reverse_it;
		}
		
		los_list.clear();
		
		if (bHasNoEmptyLOS) {
			bSuccess = true;
		} else {
			bSuccess = false;
		}
	}

	if (bIsTheTopograph
	    && pEE->pCallback != 0) {
		if (!pEE->m_bContinueExecution)  {
			delete pResultLOS;
			
			// A failed sheaf
			pEE->pCallback->endSheaf(true);

			// A failed sheaf
			Sheaf *pSheaf = new Sheaf(true);
			return pSheaf;
		} else if (bSuccess) {
			delete pResultLOS;

			// A non-failed sheaf
			pEE->pCallback->endSheaf(false);

			// A successful, but empty sheaf
			Sheaf *pSheaf = new Sheaf(false);
			return pSheaf;
		} else {
			delete pResultLOS;
			
			pEE->pCallback->endSheaf(true);

			// A failed sheaf
			Sheaf *pSheaf = new Sheaf(true);
			return pSheaf;
		}
	} else {
		if (!pEE->m_bContinueExecution)  {
			delete pResultLOS;
			
			// A failed sheaf
			Sheaf *pSheaf = new Sheaf(true);
			return pSheaf;
		} else if (bSuccess) {
			// A non-failed sheaf
			Sheaf *pSheaf = new Sheaf(pResultLOS);
			return pSheaf;
		} else {
			delete pResultLOS;
			
			// A failed sheaf
			Sheaf *pSheaf = new Sheaf(true);
			return pSheaf;
		}
	}
}

ListOfStraws *R_block_string(MQLExecEnv *pEE, 
			     const SetOfMonads& U, 
			     const SetOfMonads& Su,
			     monad_m Sm,
			     monad_m max_range,
			     BlockString *pBlockString)
{
	if (pBlockString->isBlockString2()) {
		// block_string ::= block_string2 .
		return R_block_string2(pEE, U, Su, Sm, max_range, pBlockString->getBlockString2());
	} else {
		// block_string ::= block_string2 "OR" block_string .
		ListOfStraws *pBlockString2LOS = 
			R_block_string2(pEE,
					U, Su,
					Sm,
					max_range,
					pBlockString->getBlockString2());

		ListOfStraws *pBlockStringLOS = 
			R_block_string(pEE,
				       U, Su,
				       Sm,
				       max_range, 
				       pBlockString->getBlockString());

		if (pBlockString2LOS->isEmpty()) {
			if (pBlockStringLOS->isEmpty()) {
				// Both are fails, so delete one 
				// and return fail.
				// (false OR false == false)
				delete pBlockString2LOS;
				return pBlockStringLOS;
			} else {
				// pBlockString2LOS is a fail, so delete it.
				// But return the other, since it is not a fail.
				// (false OR true == true)
				delete pBlockString2LOS;
				return pBlockStringLOS;
			}
		} else {
			// pBlockString2LOS is not a fail
			if (pBlockStringLOS->isEmpty()) {
				// pBlockStringLOS is a fail, 
				// so delete it, but return the other
				// (true OR false == true)
				delete pBlockStringLOS;
				return pBlockString2LOS;
			} else {
				// Both of them are not fails
				// Add the second to the first,
				// deleting it in the process,
				pBlockString2LOS->appendAndSubsume(pBlockStringLOS);
				return pBlockString2LOS;
			}
		}
		   
	}
}


ListOfStraws *R_block_string2(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm, monad_m max_range, BlockString2 *pBlockString2)
{
	if (pBlockString2->isBlockString1()) {
		return R_block_string1(pEE, U, Su, Sm, max_range, pBlockString2->getBlockString1());
	} else {
		if (pBlockString2->nextIsPowerBlock()) {
			return R_block_string2_power_block(pEE, U, Su, Sm, max_range, pBlockString2);
		} else {
			return R_block_string2_non_power_block(pEE, U, Su, Sm, max_range, pBlockString2);
		}
	}
}




ListOfStraws *R_block_string2_power_block(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm, monad_m max_range, BlockString2 *pBlockString2)
{
	ListOfStraws *pResultLOS = new ListOfStraws();
	ListOfStraws *pBS1LOS = R_block_string1(pEE, U, Su, Sm, max_range, pBlockString2->getBlockString1());
	if (pBS1LOS->isEmpty()) {
		delete pBS1LOS;
	} else {
		SheafConstIterator ci = pBS1LOS->const_iterator();

		Power *pPower = pBlockString2->getPowerBlock();

		monad_m limit_high = MYMIN(pPower->getLimitHigh(), max_range);
		monad_m limit_low = MYMIN(pPower->getLimitLow(), limit_high);

		while (ci.hasNext()) {
			const Straw *pBS1Straw = ci.next();

			pBlockString2->setORUStraw(pBS1Straw);

			monad_m strawlastPlus1 = pBS1Straw->getLast() + 1;
			monad_m strawlastPlus1PlusLimitLow = strawlastPlus1 + limit_low;

			SetOfMonads Uping; 
			R_restrict(U, strawlastPlus1PlusLimitLow, Uping);

			SetOfMonads Suping;
			R_restrict(Su, strawlastPlus1PlusLimitLow, Suping);

			if (!Uping.isEmpty() && !Suping.isEmpty()) {
				monad_m Supinglast = Suping.last();

				monad_m upperLimit;
				if (limit_high == MAX_MONAD) {
					upperLimit = Supinglast;
				} else {
					upperLimit = strawlastPlus1 + limit_high;
					if (upperLimit > Supinglast) {
						upperLimit = Supinglast;
					}
				}

				// We go past the ..
				// to the next block_string2.
				// Note that pBlockString2->m_block_string1
				// IS the power block, and no other blocks.
				// It cannot be anything else, given that
				// we are here.
				BlockString2 *pBS2AnteNext
					= pBlockString2->getBlockString2();
				ASSERT_THROW(!pBS2AnteNext->isBlockString1(),
					     "R_block_string2_power_block(): We should not have a .. at the end of something!")
				BlockString2 *pBS2Next
					= pBS2AnteNext->getBlockString2();

				StartMonadIterator *pSMI = pBS2Next->getBlockString1()->getSMI(pEE, U, Su, strawlastPlus1PlusLimitLow);

				ListOfStraws* pBS2LOS;
				while (pSMI->hasNext()) {
					monad_m Smping = pSMI->next();
					if (Smping > upperLimit) {
						break;
					} else {
						pBS2LOS = R_block_string2(pEE,
									  U, Su,
									  Smping,  // Start monad
									  max_range,
									  pBS2Next);
						SheafIterator BS2LOS_ci = pBS2LOS->iterator();
	    
						while (BS2LOS_ci.hasNext()) {
							Straw *pBS2Straw = BS2LOS_ci.next();
							if (!pBS2Straw->isEmpty()) {
								pBS2Straw->prepend(*pBS1Straw);
								pResultLOS->append(pBS2Straw);
							} else {
								delete pBS2Straw;
							}
						}

						pBS2LOS->clearButDontDeleteStraws();

						delete pBS2LOS;
					}
				}
				delete pSMI;
			}
		}

		delete pBS1LOS;
	}
  
	return pResultLOS;
}

ListOfStraws *R_block_string2_non_power_block(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm, monad_m max_range, BlockString2 *pBlockString2)
{
	ListOfStraws *pResultLOS = new ListOfStraws();
	ListOfStraws *pBS1LOS = R_block_string1(pEE, U, Su, Sm, max_range, pBlockString2->getBlockString1());
	if (pBS1LOS->isEmpty()) {
		delete pBS1LOS;
	} else {
		SheafConstIterator ci = pBS1LOS->const_iterator();
		
		BlockString2 *pBlockString2Next = pBlockString2->getBlockString2();
		
		while (ci.hasNext()) {
			const Straw *pBS1Straw = ci.next();
			pBlockString2->setORUStraw(pBS1Straw);
			
			monad_m Smping = pBS1Straw->getLast()+1;
			
			ListOfStraws *pBS2LOS = R_block_string2(pEE,U,Su,Smping,max_range, pBlockString2Next);
			
			SheafIterator blockString2LOS_ci = pBS2LOS->iterator();
			
			while (blockString2LOS_ci.hasNext()) {
				Straw *pBlockString2Straw = blockString2LOS_ci.next();
				if (!pBlockString2Straw->isEmpty()) {
					pBlockString2Straw->prepend(*pBS1Straw);
					pResultLOS->append(pBlockString2Straw);
				} else {
					delete pBlockString2Straw;
				}
			}
			
			pBS2LOS->clearButDontDeleteStraws();
			
			delete pBS2LOS;
		}
		
		delete pBS1LOS;
	}
	
	return pResultLOS;
}


ListOfStraws *R_block_string1(MQLExecEnv *pEE, 
			      const SetOfMonads& U, 
			      const SetOfMonads& Su,
			      monad_m Sm,
			      monad_m max_range,
			      BlockString1 *pBlockString1)
{
	if (pBlockString1->isNonStar()) {
		return R_block_string0(pEE, U, Su, Sm, max_range, pBlockString1->getBlockString0());
	} else {
		// block_string0 STAR { set }

		ListOfStraws *pResultLOS = new ListOfStraws();

		// For use if pLOS is not empty, and we
		// have 0 in the KleeneSOM.
		ListOfStraws *pLOS = R_block_string0(pEE, U, Su, Sm, max_range, pBlockString1->getBlockString0());
      
		// Return now if we failed
		if (pLOS->isEmpty()) {
			// This is a failed LOS.
			// However, if we have 0 in the set of monads,
			// we need it to be EMPTY_mo (not fail) rather 
			// than empty (fail)
			if (pBlockString1->getStarSOM()->isMemberOf(0)) {
				MatchedObject *pMO_Empty = new MatchedObject(Sm-1); // kMOKEMPTY_mo
				Straw *pParentStraw = new Straw();
				pParentStraw->append(pMO_Empty);
				pResultLOS->append(pParentStraw);
			} else {
				// If we don't have 0, we need to signal that this is a failure,
				// (by keeping pResultLOS empty) and return
				// Do nothing here... pResultLOS is already empty.
			}
			delete pLOS;
			return pResultLOS;
		} else {
			// Now we have a  bonafide list of straws

			// Get the "Set of monads" that is associated with the Block.
			// This will tells us which sequences to get and which
			// ones to pass by.
			SOMConstIterator som_it = pBlockString1->getStarSOM()->const_iterator();

			// If we have the "0" member, we must add "empty".
			if (pBlockString1->getStarSOM()->isMemberOf((monad_m) 0, som_it)) {
				MatchedObject *pMO_Empty = new MatchedObject(Sm-1); // kMOKEMPTY_mo
				Straw *pParentStraw = new Straw();
				pParentStraw->append(pMO_Empty);
				pResultLOS->append(pParentStraw);
			} 

			// Copy pLOS into result if and only if the "set of monads"
			// associated with the Kleene star has the monad "1"
			// 
			if (pBlockString1->getStarSOM()->isMemberOf((monad_m) 1, som_it)) {
				pResultLOS->appendACopy(pLOS);
			}

			bool bContinue = true;
			// Only go into the loop if 2 could be in the set. 
			if (!(pBlockString1->getStarSOM()->last() >= 2)) {
				bContinue = false;
			}
			ListOfStraws *pLOS4 = 0;
			monad_m count = 1;
			while (bContinue) {
				count++;
				SheafConstIterator losit = pLOS->const_iterator();
				bContinue = false;
				pLOS4 = new ListOfStraws();
				while (losit.hasNext()) {
					const Straw *pStraw = losit.next();
					monad_m Smping = pStraw->getLast() + 1;
					ListOfStraws *pLOS3 = R_block_string0(pEE, U, Su, Smping, max_range, pBlockString1->getBlockString0());
					if (pLOS3->isEmpty()) {
						; // Delete the stuff, and leave bContinue as false.
						delete pLOS3;
					} else {
						pLOS3->join(pStraw);
						pLOS4->appendAndSubsume(pLOS3);
						//pEE->pOut->flush(); // FIXME: Why is this here?
						bContinue = true;
					}
				}
				// bContinue is only false if all of the paths failed.
				if (bContinue) {
					// We only add this batch if the current count
					// is in the set of monads.
					if (pBlockString1->getStarSOM()->isMemberOf(count, som_it)) {
						// Yes, the count was in it. So we add a copy of the results.
						pResultLOS->appendACopy(pLOS4);
					} else {
						// No, the count was not in it, so we don't add the results
						;
					}
					delete pLOS;
					pLOS = pLOS4;
				} else {
					delete pLOS4;
				}
				// Have we gone past the end of the pBlockString1->getStarSOM()?
				if (!som_it.hasNext()) {
					// Yes, so we don't continue any more.
					bContinue = false;
				}
			}

			delete pLOS;

			return pResultLOS;
		}
		//ListOfStraws *pResultLOS = new ListOfStraws();
	}
}
	


ListOfStraws *R_block_string0(MQLExecEnv *pEE, 
			      const SetOfMonads& U, 
			      const SetOfMonads& Su,
			      monad_m Sm,
			      monad_m max_range,
			      BlockString0 *pBlockString0)
{
	if (pBlockString0->isBlock()) {
		return R_block(pEE, U, Su, Sm, pBlockString0->getBlock());
	} else {
		return R_block_string(pEE, U, Su, Sm, max_range, pBlockString0->getBlockString());
	}
}
	

Sheaf* R_opt_blocks(MQLExecEnv *pEE, 
		    const SetOfMonads& U, 
		    const SetOfMonads& Su, 
		    monad_m max_range, 
		    Blocks* pOpt_blocks)
{
	// Decide syntactic incarnation
	if (pOpt_blocks != 0) {
		// It was a blocks
		return R_blocks(pEE, U, Su, pOpt_blocks, false, max_range);
	} else {
		// It was /* empty */
		// Return // < >
		return 0;
	}
}



// Invariant:
// I1: Returns true if and only if there is at least one
//     objects at Sm matching the criteria.
bool R_NOTEXIST_object_block(MQLExecEnv *pEE, 
			     const SetOfMonads& U, 
			     const SetOfMonads& Su, 
			     monad_m Sm, 
			     ObjectBlock *pObjectBlock)
{
	// Let's be lazy and simply call 
	// R_object_block(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm,pObjectBlock).
	// To speed things up, we could build our own, and stop at the first
	// one.
	// This would give marginally better performance when
	// there were many objects of this object type 
	// that started at the same monad.
	ListOfStraws resultLOS;

	R_object_block(pEE, U, Su, Sm, pObjectBlock, &resultLOS);

	bool bItWasThere = !(resultLOS.isEmpty());

	return bItWasThere;
}

// Invariants:
// I1) If a gap is found at Sm, then an ID_M MatchedObject is 
//     returned.
// I2) If a gap is not found at Sm, the return value depends on
//     mo_kind: If kMOKEMPTY_mo, then such an MO. If kMOKNIL_mo,
//     then 0 (nil-pointer).
MatchedObject *R_gap(MQLExecEnv *pEE, 
		     const SetOfMonads& Su, 
		     monad_m Sm, 
		     GapBlock *pGapBlock, 
		     eMOKind mo_kind)
{
	monad_m m;
	if (Su.gapExists(Sm, m)) {
		bool bNoRetrieve;
    
		// Calculate NoRetrieve
		eRetrieval retrieval;
		retrieval = pGapBlock->getRetrieval();
		bNoRetrieve = R_retrieval_gap_retrieval(retrieval);
    
		// Make somn to be Sm..m
		SetOfMonads somn(Sm,m);
    
		Sheaf* pInner = 0;
		Blocks *pOptBlocks;
		pOptBlocks = pGapBlock->getOptBlocks();
		if (pOptBlocks != 0) {
      
			//
			// Calculate opt_blocks
			//
      
			// Compute the inner blocks
			try {
				pInner = R_opt_blocks(pEE, somn, somn, MAX_MONAD, pOptBlocks);
			} catch (EMdFDBException& e) {
				throw e;
			}
		}
    
		if (pInner != 0 && pInner->isFail()) {
			// Clean up
			delete pInner;
      
			MatchedObject *pMo = 0;
      
			return pMo;
		} else { // Else we return a real matched_object
			// Make [ somn bFocus pInner ]
			MatchedObject* pMo;
			pMo = new MatchedObject(somn, 
						pGapBlock->getOBB(),
						!bNoRetrieve, pGapBlock->isFocus(), 
						pInner);
			return pMo;
		}
	} else { // If there was no gap, we return < mo_kind , true >
		MatchedObject *pMo;
		if (mo_kind == kMOKEMPTY_mo) {
			pMo = new MatchedObject(Sm-1); // kMOKEMPTY_mo
		} else if (mo_kind == kMOKNIL_mo) {
			pMo = 0;
		} else {
			ASSERT_THROW(false, "mo_kind was neither EMPTY_mo nor NIL_mo");
		}
		return pMo;
	}
}

MatchedObject *R_opt_gap_block(MQLExecEnv *pEE, 
			       const SetOfMonads& Su, 
			       monad_m Sm, 
			       OptGapBlock* pOpt_gap_block)
{
	return R_gap(pEE, Su, Sm, pOpt_gap_block, kMOKEMPTY_mo);
}

bool R_retrieval_gap_retrieval(eRetrieval gap_retrieval)
{
	if (gap_retrieval == kRetrieve
	    || gap_retrieval == kRetrieveFocus)
		return false;
	else
		return true;
}

MatchedObject *R_gap_block(MQLExecEnv *pEE, 
			   const SetOfMonads& Su, 
			   monad_m Sm, 
			   GapBlock* pGap_block)
{
	return R_gap(pEE, Su, Sm, pGap_block, kMOKNIL_mo);
}







// Invariants:
// I1: Only an empty LOS is a failed LOS.
// I2: If a LOS has at least one Straw, it
//     it is not failed.
ListOfStraws *R_block(MQLExecEnv *pEE, 
		      const SetOfMonads& U, 
		      const SetOfMonads& Su, 
		      monad_m Sm, 
		      Block *pBlock)
{
	MatchedObject *pMO = 0;
	ListOfStraws *pResultLOS = new ListOfStraws();
	switch (pBlock->getKind()) {
	case kGapBlock:
		pMO = R_gap_block(pEE, Su, Sm, pBlock->getGapBlock());
		if (pMO == 0) {
			return pResultLOS; // It is empty now, so it is a failed LOS
		} else {
			Straw *pStraw = new Straw();
			pStraw->append(pMO);
			pResultLOS->append(pStraw);
			return pResultLOS;
		}
		break;
	case kOptGapBlock:
		pMO = R_opt_gap_block(pEE, Su, Sm, pBlock->getOptGapBlock());
		if (pMO == 0) {
			return pResultLOS; // It is empty now, so it is a failed LOS
		} else {
			Straw *pStraw = new Straw();
			pStraw->append(pMO);
			pResultLOS->append(pStraw);
			return pResultLOS;
		}
		break;
	case kObjectBlock: 
		{
			R_object_block(pEE, U, Su, Sm, pBlock->getObjectBlock(), pResultLOS);
			
			return pResultLOS;
		} 
		break;
	case kObjectBlockNOTEXIST: {
		ObjectBlock *pObjectBlock = pBlock->getObjectBlock();
		
		SetOfMonads Uping; 
		SetOfMonads Suping;
		monad_m Smping;
		
		bool bIsAfterNonNOTEXISTInBlockString = pObjectBlock->isAfterNonNOTEXISTInBlockString();
		
		if (bIsAfterNonNOTEXISTInBlockString) {
			R_restrict(U, Sm, Uping);
			R_restrict(Su, Sm, Suping);
			Smping = Sm;
		} else {
			Uping = U;
			Suping = Su;
			Smping = U.first();
		}
		
		StartMonadIterator *pSMI = 0;
		pSMI = pBlock->getSMI(pEE, Uping, Suping, Smping);
		
		bool bSuccess = true;
		while (pSMI->hasNext()) {
			bool bItWasThere;
			bItWasThere = 
				R_NOTEXIST_object_block(pEE,
							Uping, Suping,
							pSMI->next(),
							pObjectBlock);
			
			if (bItWasThere) {
				bSuccess = false;
				break;
			}
		}
		
		delete pSMI;

		if (bSuccess) {
			Straw *pStraw = new Straw();
			pStraw->append(new MatchedObject(Sm-1)); // An EMPTY_mo.
			pResultLOS->append(pStraw);
			return pResultLOS;
		} else {
			return pResultLOS; // It is empty now, so it is a failed LOS.
		}
	}
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown object block kind"); // Right now, there are no others, but we might
		// expand in the future...

		return 0; // Just to fool the compiler into giving no warning...
		break;
	}
}

void R_object_block_inst(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm, ObjectBlock* pObject_block, ListOfStraws *pResultLOS)
{
	// Declare some variables outside the loop
	const InstObject *pInstObject;
	MatchedObject* pMo;

	// Get last/first_and_last
	eFirstLast fl;
	fl = pObject_block->getFirstLast();

	// Get local symbol table (pObj)
	MQLObject *pObj;
	pObj = pObject_block->getObject();

	// Get range
	Inst::const_iterator ci;
	pObject_block->m_inst->objectsAtSm(Sm, ci);

	while (ci.hasNext()) {
		// Get object
		pInstObject = ci.next();

		// Get local first variables, so we don't need to
		// call first() every time.
		monad_m som_first;
		som_first = pInstObject->first();

		// Break if we are past Sm
		if (som_first != Sm) {
			break;
		}

		monad_m su_first;
		su_first = Su.first();

		//
		// Check that rules are obeyed
		//
		bool bDoObject;

		// Get local last variables, so we don't need to
		// call last() every time.
		monad_m som_last;
		som_last = pInstObject->last();
		monad_m su_last;
		su_last = Su.last();

		// Check that first/last are obeyed
		switch (fl) {
		case kNoFirstLast:
			bDoObject = true;
			break;
		case kFirst:
			bDoObject = som_first == su_first;
			break;
		case kLast:
			bDoObject = som_last == su_last;
			break;
		case kFirstAndLast:
			bDoObject = som_first == su_first && som_last == su_last;
			break;
		default:
			ASSERT_THROW(false, "Unknown first/last kind");
			break;
		}

		// Get set of monads for object
		SetOfMonads som;

		// Check that object is part_of Su (or whatever
		// operation was chosen by the
		// monad_set_relation_clause)
		if (bDoObject) {
			pInstObject->getMonads(som);
			bDoObject = pObject_block->getMSRC()->exec(som, U, Su);
		}

		// Check that feature-constraints are met.
		bool bFeatureConstraintsMatched = false;
		if (bDoObject) {
			pObj->retrieveFeatureValues(pInstObject);
			bFeatureConstraintsMatched 
				= R_features(pEE, 
					     pObj, 
					     pObject_block->getFeatureConstraints());
			bDoObject = bFeatureConstraintsMatched;
		}

		// Did we pass the test?
		if (bDoObject) {
			// So we passed the test

			// Get id_d of O
			id_d_t id_d_O;
			id_d_O = pInstObject->getID_D();


			Sheaf* pInner = 0;
			Blocks *pOptBlocks = pObject_block->getOptBlocks();
			if (pOptBlocks != 0) {
				// Make new universe (O.first()..O.last())
				SetOfMonads Uping(som_first, som_last);
				//hat(som, Uping);
	
				// Compute the inner blocks
				try {
					pInner = R_opt_blocks(pEE, Uping, som, MAX_MONAD, pOptBlocks);
				} catch (EMdFDBException& e) {
					throw e;
				}
			}

			// Only return the object if the inner was empty or did not fail
			if (pInner == 0 || !pInner->isFail()) {
				//eRetrieval retrieval;
				//retrieval = pObject_block->getRetrieval();
				//bool bNoRetrieve;
				//bNoRetrieve = R_retrieval_retrieval(retrieval);
				pMo = new MatchedObject(id_d_O, 
							som, 
							pInner,
							pObject_block->getOBB());
				ASSERT_THROW(pObject_block->getOBB() != 0,
					     "ObjectBlockBase was 0");
				// If this is to be retrieved, get features
				//if (!bNoRetrieve) {
				// Get feature retrieval
				unsigned int nNoOfFeatureRetrievalFeatures = 
					pObject_block->getNoOfFeatureRetrievalFeatures();				
				for (unsigned int index = 0;
				     index < nNoOfFeatureRetrievalFeatures;
				     ++index) {
					Feature *pFeature = pObject_block->getFeatureRetrievalFeature(index);
					pFeature->addEMdFValue(pMo, pObj, 
							       false); // false means: Don't iterate through pFeature->m_next->addEMdFValue()
				}

				Straw *pParentStraw = new Straw();
				pParentStraw->append(pMo);
				pResultLOS->append(pParentStraw);
			} else {
				// Clean up pInner
				delete pInner;
			}
		}
	}
}

void R_object_block(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm, ObjectBlock* pObject_block, ListOfStraws *pResultLOS)
{
	// We only have one kind, namely the one that has the Inst
	R_object_block_inst(pEE, U, Su, Sm, pObject_block, pResultLOS);
}



bool R_retrieval_retrieval(eRetrieval retrieval)
{
	return retrieval == kNoRetrieve;
}

bool R_feature_comparison(MQLExecEnv *pEE, MQLObject* pObj, NonParentORDSolution *pNonParentSolution, FeatureComparison* pFeature_comparison, bool bContextHasBeenNegative)
{
	UNUSED(bContextHasBeenNegative);
	
	// Declare result
	bool bResult;

	// Get left value
	const EMdFValue *left_value;
	left_value = pObj->getFeatureValue(pFeature_comparison->getFeatureIndex());
	
	// Compare
	bResult = pFeature_comparison->compare(pEE, left_value, pNonParentSolution);

	// Return result
	return bResult;
}

bool R_ffactor(MQLExecEnv *pEE, MQLObject* pObj, NonParentORDSolution *pNonParentORDSolution, FFactor* pFFactor, bool bContextHasBeenNegative)
{
	if (pFFactor->isNOT()) {
		// ffactor ::= "NOT" ffactor
		return !R_ffactor(pEE, pObj, pNonParentORDSolution, pFFactor->getFFactor(), true);
	} else if (pFFactor->isParenthesis()) {
		// ffactor ::= "(" ffeatures ")"
		return R_ffeatures(pEE, pObj, pNonParentORDSolution, pFFactor->getFFeatures(), bContextHasBeenNegative);
	} else { // isFeatureComparison() == true
		// ffactor ::= feature_comparison
		return R_feature_comparison(pEE, pObj, pNonParentORDSolution, pFFactor->getFeatureComparison(), bContextHasBeenNegative);
	}
}

bool R_fterm(MQLExecEnv *pEE, MQLObject* pObj, NonParentORDSolution *pNonParentORDSolution, FTerm* pFTerm, bool bContextHasBeenNegative)
{
	if (pFTerm->isFFactor()) {
		// fterm ::= ffactor .
		return R_ffactor(pEE, pObj, pNonParentORDSolution, pFTerm->getFFactor(), bContextHasBeenNegative);
	} else {
		bool bFTerm;
		// fterm ::= fterm "AND" ffactor .
		bFTerm = R_fterm(pEE, pObj, pNonParentORDSolution, pFTerm->getFTerm(), bContextHasBeenNegative);
		if (!bFTerm)
			return false;
		else {
			// true AND ffactor
			return R_ffactor(pEE, pObj, pNonParentORDSolution, pFTerm->getFFactor(), bContextHasBeenNegative);
		}
	}
}

bool R_ffeatures(MQLExecEnv *pEE, MQLObject* pObj, NonParentORDSolution *pNonParentORDSolution, FFeatures* pFFeatures, bool bContextHasBeenNegative)
{
	if (pFFeatures == 0)
		// feature_contraints ::= /* empty */ .
		return true;
	else {
		if (pFFeatures->isFTerm()) {
			// ffeatures ::= fterm .
			return R_fterm(pEE, pObj, pNonParentORDSolution, pFFeatures->getFTerm(), bContextHasBeenNegative);
		} else {
			// ffeatures ::= ffeatures "OR" fterm .
			bool bFFeatures;
			bFFeatures = R_ffeatures(pEE, pObj, pNonParentORDSolution, pFFeatures->getFFeatures(), bContextHasBeenNegative);
			if (bFFeatures)
				return true;
			else {
				return R_fterm(pEE, pObj, pNonParentORDSolution, pFFeatures->getFTerm(), bContextHasBeenNegative); // false || fterm
			}
		}
	}
}

NonParentORDSolutionList *R_add_two_lists(NonParentORDSolutionList *pList1, NonParentORDSolutionList *pList2)
{
	NonParentORDSolutionList *pResultList = new NonParentORDSolutionList;
	if (pList2->empty()) {
		NonParentORDSolutionListIterator it = pList1->iterator();
		while (it.hasNext()) {
			NonParentORDSolution* pSolution = it.next();
			if (!pSolution->empty()) {
				pResultList->push_back(new NonParentORDSolution(*pSolution));
			}
		}
	} else if (pList1->empty()) {
		NonParentORDSolutionListIterator it = pList2->iterator();
		while (it.hasNext()) {
			NonParentORDSolution* pSolution = it.next();
			if (!pSolution->empty()) {
				pResultList->push_back(new NonParentORDSolution(*pSolution));
			}
		}
	} else {
		NonParentORDSolutionListIterator it1 = pList1->iterator();
		while (it1.hasNext()) {
			NonParentORDSolution* pL1 = it1.next();
			bool bDoBoth = !pL1->empty();
			NonParentORDSolutionListIterator it2 = pList2->iterator();
			while (it2.hasNext()) {
				NonParentORDSolution* pL2 = it2.next();
				if (!pL2->empty()) {
					if (bDoBoth) {
						NonParentORDSolution *pNewSolution = new NonParentORDSolution();
						pL1->extendWithMe(pNewSolution);
						pL2->extendWithMe(pNewSolution);
						pResultList->push_back(pNewSolution);
					} else {
						NonParentORDSolution *pNewSolution = new NonParentORDSolution(*pL2);
						pResultList->push_back(pNewSolution);
					}
				}
			}
		}
	}
	return pResultList;
}

NonParentORDSolutionList *R_harvest_mo_for_list(const MatchedObject* pMO, FFeatures* pFFeatures)
{
	NonParentORDSolutionList *pMO_list = new NonParentORDSolutionList();

	bool bCheckChildren = false;
	
	node_number_t OBBNN = pMO->getObjectBlockBase()->getNodeNumber();

	// Can be implemented with a std::set<node_number_t>.	
	if (pFFeatures->mustGetNonParentORDSolutionForOBB(OBBNN)) {
		if (pMO_list->empty()) {
			pMO_list->push_back(new NonParentORDSolution());
		}
		pMO_list->back()->push_back(pMO);	
	}

	// FIXME: This is inefficient, sometimes.
	// If this pMO doesn't have any ORDs beneath it which 
	// must be checked, then there is no point in checking the sheaf.
	if (!pMO->sheafIsEmpty()) {
		bCheckChildren = true;
	}

	if (bCheckChildren) {
		const Sheaf *pSheaf = pMO->getSheaf();
		SheafConstIterator sheaf_ci = pSheaf->const_iterator();
		NonParentORDSolutionList* pResultList = new NonParentORDSolutionList;
		while (sheaf_ci.hasNext()) {
			const Straw *pStraw = sheaf_ci.next();
			StrawConstIterator straw_ci = pStraw->const_iterator();
			while (straw_ci.hasNext()) {
				const MatchedObject* pMO2 = straw_ci.next();
				NonParentORDSolutionList *pInner_MO_list
					= R_harvest_mo_for_list(pMO2, pFFeatures);
				NonParentORDSolutionList* pAddedList = R_add_two_lists(pMO_list, pInner_MO_list);
				pInner_MO_list->clear(true);
				delete pInner_MO_list;
				pAddedList->extendWithMe(pResultList);
				pAddedList->clear(true);
				delete pAddedList;
			}
		}
		pMO_list->clear(true);
		delete pMO_list;
		return pResultList;
	} else {
		return pMO_list;
	}
}



NonParentORDSolutionList * R_build_solition_list_when_ORD_is_non_parent(MQLExecEnv* pEE, FFeatures* pFFeatures)
{
	UNUSED(pEE);
	
	NonParentORDSolutionList *pResultList = new NonParentORDSolutionList;

	// Build straw list
	std::list<const Straw*> straw_list;
	BlockString2List::const_iterator bs2_it = pFFeatures->getBlockString2List().begin();
	while (bs2_it != pFFeatures->getBlockString2List().end()) {
		BlockString2 *pBS2 = *bs2_it;
		const Straw* pStraw = pBS2->getORUStraw();
		straw_list.push_back(pStraw);
		++bs2_it;
	}

	// Build solution list
	std::list<const Straw*>::const_iterator straw_it = straw_list.begin();

	while (straw_it != straw_list.end()) {
		const Straw *pStraw = *straw_it;
		

		StrawConstIterator straw_ci = pStraw->const_iterator();
		while (straw_ci.hasNext()) {
			const MatchedObject* pMO = straw_ci.next();
			NonParentORDSolutionList *pThis_mo_solution_list = 
				R_harvest_mo_for_list(pMO, pFFeatures);

			NonParentORDSolutionList *pNewResultList = R_add_two_lists(pResultList, pThis_mo_solution_list);

			pResultList->clear(true);
			delete pResultList;

			pResultList = pNewResultList;

			pThis_mo_solution_list->clear(true);
			delete pThis_mo_solution_list;
		}

		++straw_it;
	}
	return pResultList;
}


bool R_features(MQLExecEnv *pEE, MQLObject* pObj, FFeatures* pFFeatures)
{
	if (pFFeatures == 0)
		// feature_contraints ::= /* empty */ .
		return true;
	else {
		if (pFFeatures->hasNonParentORUs()) {
			NonParentORDSolutionList* pORD_solution_list =
				R_build_solition_list_when_ORD_is_non_parent(pEE, pFFeatures);
			NonParentORDSolutionListIterator it 
				= pORD_solution_list->iterator();
			bool bResult = false;
			while (!bResult
			       && it.hasNext()) {
				NonParentORDSolution *pNonParentORDSolution = 
					it.next();
				// pFFeatures->pretty(&std::cerr, 0, pNonParentORDSolution); // UP200
				if (R_ffeatures(pEE,
						pObj, pNonParentORDSolution,
						pFFeatures, false)) {
					bResult = true;
				}
			}

			// Delete all solutions			
			pORD_solution_list->clear(true);
			delete pORD_solution_list;

			return bResult;
		} else {
			// pFFeatures->pretty(&std::cerr, 0, 0); // UP200
			return R_ffeatures(pEE, pObj, 0, pFFeatures, false);
		}
	}
}
