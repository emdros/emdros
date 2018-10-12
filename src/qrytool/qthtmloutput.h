/*
 * qthtmloutput.h
 *
 * Classes to do output from the query tool in HTML.
 *
 * Ulrik Petersen
 * Created: 4/16-2005
 * Last update: 3/24-2008
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



#ifndef QTOUTPUT_HTML__H__
#define QTOUTPUT_HTML__H__

class QTHTMLOutput {
 protected:
	std::ostream *m_pOut;
	bool m_bDeleteOut;
	bool m_bDoHTMLPrePostAmble;
	bool m_bEmitStylesheet;
	bool m_bInTable;
 public:
	QTHTMLOutput(std::ostream* pOut, bool bDeleteOut, bool bDoHTMLPrePostAmble, bool bEmitStylesheet);
	virtual ~QTHTMLOutput();
	virtual void print(const std::string& toPrint);
	virtual void startUnit(const std::string& object_type_name);
	virtual void endUnit(const std::string& object_type_name);
	virtual void endSolution(void);
	virtual void startRaster(void);
	virtual void startLatin(void);
	virtual void startLatinParagraph(void); 
	virtual void printFeature(const std::string& object_type_name,
				  unsigned int feature_index,
				  const std::string& feature_name,
				  const std::string& feature_value);
	virtual void startFocus(void);
	virtual void endFocus(void);
	virtual void newline(void);
	virtual void endl(void);
	virtual void flush(void);

	virtual void startTable(const std::string& CSS_class);
	virtual void endTable(const std::string& CSS_class);
	virtual void startTableRow(const std::string& CSS_class);
	virtual void endTableRow(const std::string& CSS_class);
	virtual void startTableCell(const std::string& CSS_class);
	virtual void endTableCell(const std::string& CSS_class);

	virtual void MyYield(void);
};


#endif // QTOUTPUT_HTML__H__
