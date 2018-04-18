// HTreeCanvas : header file
//
// Created: 9/8-2007
// Last update: 11/9-2009
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2009  Ulrik Sandborg-Petersen
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


#ifndef HTREECANVAS__H__
#define HTREECANVAS__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "htreecanvas.cpp"
#endif


#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/print.h>
#include "viewmetrics.h"
#include "layout.h"
#include "monads.h"
#include "environment_emdros.h"

class HTreeNode {
 protected:
	int m_x, m_y; // y-values grow from the bottom (0) upwards (positive)
	int m_width, m_height;
	int m_main_box_width;
	int m_least_vertical_riser_width, m_least_vertical_riser_height;
	int m_leftmost_x, m_rightmost_x;
	int m_vertical_separation_distance_between_main_box_and_branch_box;
	id_d_t m_self_id_d;
	id_d_t m_parent_id_d;
	SetOfMonads m_som;
	LayoutBoxBase *m_pBranchLabels;
	bool m_bOwnsBranchLabels;
	bool m_bBranchLabelsBelowMainBox;
	LayoutBoxBase *m_pMainBox;
	bool m_bOwnsMainBox;
 public:
	HTreeNode(id_d_t self_id_d,
		  id_d_t parent_id_d,
		  const SetOfMonads& som,
		  LayoutBoxBase *pBranchLabels,
		  bool bOwnsBranchLabels,
		  LayoutBoxBase *pMainBox,
		  bool bOwnsMainBox,
		  bool bBranchLabelsBelowMainBox = false);
	virtual ~HTreeNode();
	virtual bool isTerminal(void) const { return false; };
	virtual void RecalculateExtents(wxDC *pDC, ViewMetrics *pViewMetrics);
	int getVerticalRiserX(void) const;
	bool areBranchLabelsBelowMainBox(void) const { return m_bBranchLabelsBelowMainBox; };
	int getBranchLabelsTotalHeight(void) const;
	int getLeastVerticalRiserHeight(void) const { return m_least_vertical_riser_height; };
	int getLeastVerticalRiserWidth(void) const { return m_least_vertical_riser_width; };
	const SetOfMonads& getSOM(void) const { return m_som; };
	id_d_t getID_D(void) const { return m_self_id_d; };
	id_d_t getParentID_D(void) const { return m_parent_id_d; };
	void setParentID_D(id_d_t parent_id_d) { m_parent_id_d = parent_id_d; };
	monad_m first(void) const { return m_som.first(); };
	monad_m last(void) const { return m_som.last(); };
	int getMainBoxWidth(void) const { return m_main_box_width; };
	int getWidth(void) const { return m_width; };
	int getHeight(void) const { return m_height; };
	int getMiddleY(int y) const { return y + m_height/2; };
	int getBottomY(int y) const { return y + m_height; };
	virtual void calculateLeastVerticalRiserDimensions(ViewMetrics *pViewMetrics);
	virtual int calculateMainHeight(void);
	virtual int calculateMainWidth(void) { return 0; }; // Must be overridden
	virtual void calculateDimensions(ViewMetrics *pViewMetrics);
	void setX(int x) { m_x = x; };
	void setY(int y) { m_y = y; };
	int getX(void) const { return m_x; };
	int getY(void) const { return m_y; };
	void setLeftmostX(int leftmost_x) { m_leftmost_x = leftmost_x; };
	void setRightmostX(int rightmost_x) { m_rightmost_x = rightmost_x; };
	void setXRightToLeft(int total_canvas_width);

	virtual std::string getCharacteristicString(void) const;

	virtual void Draw(wxDC *pDC, int y, int yParentMiddle, int yParentBottom, int xParentLeft, int xParentRight, int xOffset, int yOffset, const wxPen& pen);
};

class HTerminalNode : public HTreeNode {
 public:
	HTerminalNode(id_d_t self_id_d,
		      id_d_t parent_id_d,
		      const SetOfMonads& som,
		      LayoutBoxBase *pBranchLabels,
		      bool bOwnsBranchLabels,
		      LayoutBoxBase *pTerminalBox,
		      bool bOwnsTerminalBox,
		      bool bBranchLabelsBelowMainBox = false);
	virtual ~HTerminalNode();
	
	virtual bool isTerminal(void) const { return true; };

	virtual int calculateMainWidth(void);

	virtual void Draw(wxDC *pDC, int y, int yParentMiddle, int yParentBottom, int xParentLeft, int xParentRight, int xOffset, int yOffset, const wxPen& pen);
};

class HNonTerminalNode : public HTreeNode {
 public:
	HNonTerminalNode(id_d_t self_id_d,
			 id_d_t parent_id_d,
			 const SetOfMonads& som,
			 LayoutBoxBase *pBranchLabels,
			 bool bOwnsBranchLabels,
			 LayoutBoxBase *pCategoryBox,
			 bool bOwnsCategoryBox,
			 bool bBranchLabelsBelowMainBox = false);
	virtual ~HNonTerminalNode();

	virtual int calculateMainWidth(void);

	virtual void Draw(wxDC *pDC, int y, int yParentMiddle, int yParentBottom, int xParentLeft, int xParentRight, int xOffset, int yOffset, const wxPen& pen);
};


class HTreeBox : public LayoutBoxBase {
 protected:
	typedef std::map<id_d_t, HTreeNode*> IDD2TreeNodeMap;
	typedef std::multimap<monad_m, id_d_t> Monad2IDDMMap;
	typedef std::map<monad_m, id_d_t> Monad2IDDMap;
	typedef std::map<id_d_t, Monad2IDDMMap> IDD2Monad2IDDMMap;
	IDD2TreeNodeMap m_objs;
	Monad2IDDMap m_terminals;
	IDD2Monad2IDDMMap m_children;
	// int m_width, m_height;
 public:
	HTreeBox(ViewMetrics *pMetrics, bool bSpaceAfter, bool bIsRightToLeft);
	virtual ~HTreeBox();

	virtual void Draw(wxDC *pDC, int xLP, int yLP);
	virtual void RecalculateExtents(wxDC *pDC);
	
	void addTerminal(HTerminalNode *pNode);
	void addNonTerminal(HNonTerminalNode *pNode);
	void treeIsFinished(void); // Call this when the tree is finished, i.e., when all objects have been added
	void layout();

	void clearLayout(); // Deletes all objects...

	void toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);

	void savePNG(const std::string& base_filename, bool bUseMagnificationInFilename, std::string& /* out */ filenameused, monad_m& first_monad);

	void DrawToBitmap(wxBitmap **ppBitmap);
	std::string getCharacteristicString();
 protected:
	void addObject(HTreeNode *pNode);
	HTreeNode *getNode(id_d_t id_d);
	int getInterWordSpace() const;
	void layoutNode(id_d_t id_d);
	void setXRightToLeft(id_d_t id_d);
	const Monad2IDDMMap& getRoots();
	void findRoots();
	const Monad2IDDMMap& getChildren(id_d_t parent_id_d) const;
	void DrawNode(wxDC *pDC, id_d_t id_d, int yParentMiddle, int yParentBottom, int xParentLeft, int xParentRight, int xOffset, int yOffset, const wxPen& pen);
};

class HTreeCanvas : public wxScrolledWindow {
protected:
	HTreeBox *m_pBox;
	ViewMetrics *m_pViewMetrics;
public:
	HTreeCanvas(ViewMetrics *pMetrics, wxWindow *pParent, const wxPoint& pos, const wxSize& size, const long style, bool bRightToLeft);
	void PrepareDC(wxDC *pDC);
	virtual void OnDraw(wxDC &dc);
	virtual void DrawToClipboard(void);
	virtual void DrawToBitmap(wxBitmap **ppBitmap);
	void OnMouseEvent(wxMouseEvent& event);
	void OnSize(wxSizeEvent& event);

	int GetWidth(void) const { return m_pBox->GetWidthLP(); };
	int GetHeight(void) const { return m_pBox->GetHeightLP(); };

	DECLARE_EVENT_TABLE()
public:
	wxMappingMode m_nMapMode;
	int m_nMagnification;
public:
	virtual void OnChangeInterlinearLine();
	virtual void OnUpdate();
	virtual void UpdateAllViews() {};  // Must be implemented.
	virtual void ChangeMagnification(int magnification);

	void addTerminal(HTerminalNode *pNode) { m_pBox->addTerminal(pNode); };
	void addNonTerminal(HNonTerminalNode *pNode)  { m_pBox->addNonTerminal(pNode); };
	void layout() { m_pBox->layout(); };

	void treeIsFinished(void) { m_pBox->treeIsFinished(); };

	int GetMagnification(void) const { return m_nMagnification; };
	ViewMetrics *GetViewMetrics(void) { return m_pViewMetrics; };
	virtual void InitializeViewMetrics(); // Call this AFTER initializing it!
	bool UpdateViewMetricsViewWidth(void);
	virtual void HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton);
	void ShowPopup(wxMenu *menuPopUp, wxPoint point);
 protected:
	virtual ~HTreeCanvas();
	void adjustHeights(wxDC* pDC);
	void GetClipBox(wxDC *pDC, wxRect& clipRect);
};


class HTreeDrawerConfiguration {
private:
        std::string m_terminal_object_type_name;
        std::string m_terminal_parent_feature;
        std::list<std::string> m_terminal_display_features; // Does not include parent
        std::list<std::string> m_terminal_display_feature_font_names; // Does not include parent, must be same order as terminal features
        std::map<std::string, std::list<std::string> > m_nonterminal_display_features; // OTN --> list(features-to-display)
        std::map<std::string, std::string> m_nonterminal_parent_features; // OTN --> parent-feature
        bool m_bIsRightToLeft;
        ViewMetrics *m_pViewMetrics;
public:
        HTreeDrawerConfiguration(bool bIsRightToLeft);
        ~HTreeDrawerConfiguration();
        void setTerminalType(const std::string& terminal_object_type_name,
                             const std::string& terminal_parent_feature);
        void addTerminalFeature(const std::string& feature_name,
                                const std::string& feature_font_name);
        void addNonTerminalType(const std::string& nonterminal_object_type_name,
                                const std::string& nonterminal_parent_feature);
        void addNonTerminalFeature(const std::string& nonterminal_object_type_name, 
                                   const std::string& feature_name);
        void freeze(); // Creates m_pViewMetrics
        std::list<std::string> getNonTerminalObjectTypeNames(void) const;
        std::string getNonTerminalParentFeature(const std::string& nonterminal_object_type_name) const;
        std::list<std::string> getNonTerminalDisplayFeatures(const std::string& nonterminal_object_type_name) const;
        std::string getTerminalObjectTypeName(void) const;
        std::string getTerminalParentFeature(void) const;
        std::list<std::string> getTerminalDisplayFeatures(void) const;  // font names are encapsulated in the view metrics
        ViewMetrics *getViewMetrics(void) { return m_pViewMetrics; };
        bool isRightToLeft(void) const { return m_bIsRightToLeft; };
};

class HTreeDrawer {
protected:
        HTreeDrawerConfiguration *m_pConf;
        HTreeBox *m_pTreeBox;
public:
        HTreeDrawer(HTreeDrawerConfiguration *pConf);
        ~HTreeDrawer();
        bool harvest(EmdrosEnv *pEnv, monad_m first_monad, monad_m last_monad);
        wxBitmap draw(int magnification = 100);
protected:
        void clearTree();
        bool harvestTerminals(EmdrosEnv *pEnv, monad_m first_monad, monad_m last_monad);
        bool harvestNonTerminals(EmdrosEnv *pEnv, monad_m first_monad, monad_m last_monad);
        bool harvestNonTerminalObjectType(const std::string& OTN, EmdrosEnv *pEnv, monad_m first_monad, monad_m last_monad);
        bool execQuery(EmdrosEnv *pEnv, const std::string& query);
};




#endif

