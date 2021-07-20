// ColourSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Mandel.h"
#include "ColourSetupDlg.h"
#include "MandelCalc.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColourSetupDlg dialog


CColourSetupDlg::CColourSetupDlg(CColourTable *pColourTable,
								 CWnd* pParent /*=NULL*/)
	: CDialog(CColourSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColourSetupDlg)
	m_Blue = 0;
	m_Green = 0;
	m_Red = 0;
	m_MaxIteration = 0;
	m_MinIteration = 0;
	m_MaxIterations = _T("");
	//}}AFX_DATA_INIT

	m_pColourTable = pColourTable;
}


void CColourSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColourSetupDlg)
	DDX_Control(pDX, IDC_COLOUR_SAMPLE, m_ColourSample);
	DDX_Control(pDX, IDC_COLOUR_INDEX, m_ColourIndex);
	DDX_Text(pDX, IDC_BLUE, m_Blue);
	DDX_Text(pDX, IDC_GREEN, m_Green);
	DDX_Text(pDX, IDC_RED, m_Red);
	DDX_Text(pDX, IDC_MAX_ITERATION, m_MaxIteration);
	DDX_Text(pDX, IDC_MIN_ITERATION, m_MinIteration);
	DDX_Text(pDX, IDC_MAX_ITERATIONS, m_MaxIterations);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColourSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CColourSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CColourSetupDlg::SelectColourIndex(long lIndex)
{
	ASSERT(m_pColourTable);
	ASSERT(lIndex >= 0);

	m_Red = m_pColourTable->GetRedValue(lIndex);
	m_Green = m_pColourTable->GetGreenValue(lIndex);
	m_Blue = m_pColourTable->GetBlueValue(lIndex);


}


/////////////////////////////////////////////////////////////////////////////
// CColourSetupDlg message handlers

BOOL CColourSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_lIndex = 0;
	m_lMaxIterations = m_pColourTable->GetMaxIterations();
	m_lNumberOfColours = m_pColourTable->GetNumberOfColours();

	char szBuf[80];
	wsprintf((LPTSTR)szBuf, 
			 (LPCTSTR)"Max Iterations: %ld", m_lMaxIterations);
	m_MaxIterations = (LPCTSTR)szBuf;

	for (long lIndex = 0; lIndex < m_lNumberOfColours; lIndex++)
	{
		wsprintf((LPTSTR)szBuf, (LPCTSTR)"Index %ld", lIndex);
		m_ColourIndex.AddString((LPCTSTR)szBuf);
	}

	m_ColourIndex.SetCurSel(0);
	SelectColourIndex(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
