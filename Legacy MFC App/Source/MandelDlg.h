// MandelDlg.h : header file
//

#if !defined(AFX_MANDELDLG_H__900FA8D7_12DF_11D5_B5B8_0020AFC6B581__INCLUDED_)
#define AFX_MANDELDLG_H__900FA8D7_12DF_11D5_B5B8_0020AFC6B581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MandelCalc.hpp"


/////////////////////////////////////////////////////////////////////////////
// CMandelDlg dialog

class CMandelDlg : public CDialog
{
// Construction
public:
	CMandelDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CMandelDlg();

// Dialog Data
	//{{AFX_DATA(CMandelDlg)
	enum { IDD = IDD_MANDEL_DIALOG };
	CButton	m_ExportBtn;
	CButton	m_CalculateBtn;
	CButton	m_LoadBtn;
	CButton	m_SaveBtn;
	CStatic	m_Output;
	long	m_ImageSize;
	double	m_ImagCorner;
	long	m_MaxIterations;
	double	m_RealCorner;
	double	m_Side;
	CString	m_ImagSideBottom;
	CString	m_ImagSideMid;
	CString	m_ImagSideTop;
	CString	m_RealSideLeft;
	CString	m_RealSideMid;
	CString	m_RealSideRight;
	CString	m_ProcessingTime;
	BOOL	m_bAutoZoom;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMandelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void	SetCaption(int iPercentCompleted);
	void	DrawRow();
	void	UpdateProcessingTime();

	CDC		*m_pMemDC;
	COLORREF	*m_pRGB;
	CMandelbrot	*m_pMandel;


// Implementation
private:
	CString		m_strName;
	BOOL		m_bDragArea, m_bAreaSelected;
	double		m_dRealSelect, m_dImagSelect;
	double		m_dSideSelect;
	CPoint		m_ptMouseDown, m_ptDrag;
	CPen		*m_pWhitePen;

	CBitmap		*m_pMemBitmap;
	long		m_lNumberOfColours;
	BOOL		m_bProcessing;
	DWORD		m_dwStartTime;
	long		m_lProcessingTime;		// processing time in seconds


	void	CreateMemDC(int iImageSize);
	void	SetupColourTable(long lNumColours);
	void	DrawOutput();
	void	ClearOutput();
	void	DrawImageValues();
	void	DrawSelectArea(CPoint &rPt1, CPoint &rPt2);
	void	ConvertPtToImage(CPoint &rPt);
	void	SetSelectedAreaParameters();
	void	ClearProcessingTime();
	void	ShowProcessingTime(long lSeconds);
	BOOL	ExportBitmap(LPCTSTR lpszFilename);


protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMandelDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnButton2();
	afx_msg void OnCalculate();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnExport();
	afx_msg void OnAutoZoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANDELDLG_H__900FA8D7_12DF_11D5_B5B8_0020AFC6B581__INCLUDED_)
