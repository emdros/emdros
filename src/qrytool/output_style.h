/*
 * output_style.h
 *
 * Base class for all output styles.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 4/19-2019
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



#ifndef OUTPUT_STYLE__H__
#define OUTPUT_STYLE__H__


#include <conf.h>
#include "solution.h"
#include "qtoutput.h"
#include <emdros-lconfig.h>

#include <ostream>


class Output_style {
 protected:
  Configuration *m_pConf;
  bool m_bApplyFocus;
  bool m_bFocus_on;
  bool m_bBreakAfterRaster;
  QTOutputBase *m_pOut;
public:
  Output_style(Configuration *pConf, QTOutputBase* pOut);
  virtual ~Output_style() {}; // Don't delete anything!
  virtual void show(const Solution *pSolution, int solution_number);
protected:
  virtual void writeStr(const std::string& str);
  virtual std::string del_accents(const std::string& s) { return s; /* TODO: Implement. */ };
  virtual void turn_focus_off(void);
  virtual void do_raster_unit(MyTable *ras_list, 
			      const OTTableMap& ref_list, const OTTableMap& dat_list, 
			      const SetOfMonads& monads, const SetOfMonads& focus);
  virtual void do_data(monad_m m, const std::string& unit_type, 
		       const OTTableMap& unit_list, 
		       const SetOfMonads& monads, 
		       const SetOfMonads& focus);
  virtual bool do_data_unit(monad_m m, 
			    const std::string& unit,
			    MyTable *pUnitTable,
			    MyRow U,
			    const SetOfMonads& monads,
			    const SetOfMonads& focus,
			    bool bIsLeft,
			    bool bWasAlreadyPrinted);
  virtual void do_reference(monad_m m, 
			    const std::string& unit_type, 
			    const OTTableMap& unit_list);
  virtual void start_reference(void);
  virtual void print_reference(const std::string& reference);
  virtual void end_reference(void);
  virtual void start_raster_unit_row(void);
  virtual void end_raster_unit_row(void);
  virtual void start_raster_unit(void);
  virtual void end_raster_unit(void);
  virtual void start_solution(int solution_number);
  virtual void end_solution(void);
};

class KWIC_Output_style : public Output_style {
 protected:
	typedef enum { kKWICBeforeKeyword, kKWICKeyword, kKWICAfterKeyword } eKWICState;
	eKWICState m_state;
 public:
	KWIC_Output_style(Configuration *pConf, QTOutputBase* pOut);
	virtual ~KWIC_Output_style();
 protected:
	virtual void do_data(monad_m m, const std::string& unit_type, 
			     const OTTableMap& unit_list, 
			     const SetOfMonads& monads, 
			     const SetOfMonads& focus);
	virtual void start_reference(void);
	virtual void print_reference(const std::string& reference);
	virtual void end_reference(void);
	virtual void start_raster_unit_row(void);
	virtual void end_raster_unit_row(void);
	virtual void start_raster_unit(void);
	virtual void end_raster_unit(void);
	virtual void start_KWIC_BeforeKeyword(void);
	virtual void end_KWIC_BeforeKeyword(void);
	virtual void start_KWIC_Keyword(void);
	virtual void end_KWIC_Keyword();
	virtual void start_KWIC_AfterKeyword(void);
	virtual void end_KWIC_AfterKeyword();
	virtual void start_solution(int solution_number);
	virtual void end_solution(void);
};

class Tree_Output_style : public Output_style {
public:
  Tree_Output_style(Configuration *pConf, QTOutputBase* pOut);
  virtual ~Tree_Output_style() {}; // Don't delete anything!
  virtual void show(const Solution *pSolution, int solution_number);
 protected:
  virtual void print_reference(const std::string& reference);
  virtual void print_solution_reference(const Solution *pSolution);
  virtual void print_tree(const Solution *pSolution);
};



class XML_Output_style : public Output_style {
public:
  XML_Output_style(Configuration *pConf, QTOutputBase* pOut);
  virtual ~XML_Output_style() {}; // Don't delete anything!
  virtual void turn_focus_off(void);
  virtual void do_raster_unit(MyTable *ras_list, 
			      const OTTableMap& ref_list, const OTTableMap& dat_list, 
			      const SetOfMonads& monads, const SetOfMonads& focus);
  virtual void do_data(monad_m m, const std::string& unit_type, 
		       const OTTableMap& unit_list, 
		       const SetOfMonads& monads, 
		       const SetOfMonads& focus);
  virtual bool do_data_unit(monad_m m, 
			    const std::string& unit,
			    MyTable *pUnitTable,
			    MyRow U,
			    const SetOfMonads& monads,
			    const SetOfMonads& focus,
			    bool bIsLeft,
			    bool bWasAlreadyPrinted);
  virtual void do_reference(monad_m m, 
			    const std::string& unit_type, 
			    const OTTableMap& unit_list,
			    std::string& /* out */ reference);
  virtual void show(const Solution *pSolution, int solution_number);
 protected:
  virtual void start_raster_unit_row(const std::string& reference);
  virtual void end_raster_unit_row(void);
  virtual void start_raster_unit(void);
  virtual void end_raster_unit(void);
  virtual void start_solution(int solution_number);
  virtual void end_solution(void);
  virtual void start_reference(void);
  virtual void print_reference(const std::string& reference);
  virtual void end_reference(void);
};



#endif 
