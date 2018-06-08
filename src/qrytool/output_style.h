/*
 * output_style.h
 *
 * Base class for all output styles.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 11/10-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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

#ifndef OUTPUT_STYLE__H__
#define OUTPUT_STYLE__H__

#include "emdros-lconfig.h"
#include "conf.h"

#include "solution.h"
#include "qtoutput.h"


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
  virtual void print_reference(const Solution *pSolution);
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
