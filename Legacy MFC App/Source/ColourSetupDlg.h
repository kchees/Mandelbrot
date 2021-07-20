#if !defined(AFX_COLOURSETUPDLG_H__6D4120F3_147D_11D5_B5BB_0020AFC6B581__INCLUDED_)
#define AFX_COLOURSETUPDLG_H__6D4120F3_147D_11D5_B5BB_0020AFC6B581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColourSetupDlg.h : header file
//

#include "MandelCalc.hpp"


/////////////////////////////////////////////////////////////////////////////
// CColourSetupDlg dialog

class CColourSetupDlg : public CDialog
{
// Construction
public:
	CColourSetupDlg(CColourTable *pColourTable,
					CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColourSetupDlg)
	enum { IDD = IDD_COLOURSETUP_DLG };
	CStatic	m_ColourSample;
	CComboBox	m_ColourIndex;
	int		m_Blue;
	int		m_Green;
	int		m_Red;
	long	m_MaxIteration;
	long	m_MinIteration;
	CString	m_MaxIterations;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColourSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	CColourTable *m_pColourTable;
	long		 m_lIndex;
	long		 m_lMaxIterations;
	long		 m_lNumberOfColours;

	void SelectColourIndex(long lIndex);


protected:

	// Generated message map functions
	//{{AFX_MSG(CColourSetupDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOURSETUPDLG_H__6D4120F3_147D_11D5_B5BB_0020AFC6B581__INCLUDED_)
