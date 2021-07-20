// MandelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Mandel.h"
#include "MandelDlg.h"
#include <afxdlgs.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define FILE_FILTER		"Mandelbrot Data (*.mbt)|*.mbt|All Files (*.*)|*.*||"
#define EXPORT_FILTER	"Bitmap File (*.bmp)|*.bmp|All Files (*.*)|*.*||"
#define DATA_FILE_EXT	"MBT"
#define BITMAP_EXT		"BMP"



CMandelDlg *pMainDlg = NULL;
CRITICAL_SECTION	OutputCS;

int CALLBACK CalculateCallback(int iRow, long *pRowData);



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMandelDlg dialog

CMandelDlg::CMandelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMandelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMandelDlg)
	m_ImageSize = 1000;
	m_ImagCorner = -1.25;
	m_MaxIterations = 1000;
	m_RealCorner = -2.0;
	m_Side = 2.50;
	m_ImagSideBottom = _T("");
	m_ImagSideMid = _T("");
	m_ImagSideTop = _T("");
	m_RealSideLeft = _T("");
	m_RealSideMid = _T("");
	m_RealSideRight = _T("");
	m_ProcessingTime = _T("");
	m_bAutoZoom = TRUE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMandelDlg::~CMandelDlg()
{
	if (m_pWhitePen)
		delete m_pWhitePen;

	if (m_pMemBitmap)
		delete m_pMemBitmap;

	if (m_pMemDC)
		delete m_pMemDC;

	if (m_pRGB)
		delete m_pRGB;

	DeleteCriticalSection(&OutputCS);
}


void CMandelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMandelDlg)
	DDX_Control(pDX, IDC_EXPORT, m_ExportBtn);
	DDX_Control(pDX, IDC_CALCULATE, m_CalculateBtn);
	DDX_Control(pDX, IDC_LOAD, m_LoadBtn);
	DDX_Control(pDX, IDC_SAVE, m_SaveBtn);
	DDX_Control(pDX, IDC_OUTPUT, m_Output);
	DDX_Text(pDX, IDC_IMAGE_SIZE, m_ImageSize);
	DDV_MinMaxLong(pDX, m_ImageSize, 100, 10000);
	DDX_Text(pDX, IDC_IMAGINARY_CORNER, m_ImagCorner);
	DDX_Text(pDX, IDC_MAX_ITERATIONS, m_MaxIterations);
	DDV_MinMaxLong(pDX, m_MaxIterations, 100, 100000);
	DDX_Text(pDX, IDC_REAL_CORNER, m_RealCorner);
	DDX_Text(pDX, IDC_SIDE, m_Side);
	DDX_Text(pDX, IDC_IMAGSIDE_BOTTOM, m_ImagSideBottom);
	DDX_Text(pDX, IDC_IMAGSIDE_MID, m_ImagSideMid);
	DDX_Text(pDX, IDC_IMAGSIDE_TOP, m_ImagSideTop);
	DDX_Text(pDX, IDC_REALSIDE_LEFT, m_RealSideLeft);
	DDX_Text(pDX, IDC_REALSIDE_MID, m_RealSideMid);
	DDX_Text(pDX, IDC_REALSIDE_RIGHT, m_RealSideRight);
	DDX_Text(pDX, IDC_PROCESSING_TIME, m_ProcessingTime);
	DDX_Check(pDX, IDC_AUTO_ZOOM, m_bAutoZoom);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMandelDlg, CDialog)
	//{{AFX_MSG_MAP(CMandelDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_CALCULATE, OnCalculate)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_AUTO_ZOOM, OnAutoZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMandelDlg message handlers

BOOL CMandelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// disable save button
	m_SaveBtn.EnableWindow(FALSE);

	// disable export button
	m_ExportBtn.EnableWindow(FALSE);

	InitializeCriticalSection(&OutputCS);

	m_pMandel = NULL;
	pMainDlg = this;	
	m_bDragArea = FALSE;
	m_bAreaSelected = FALSE;
	m_pWhitePen = new CPen(PS_SOLID, 1, RGB(255,255,255));
	m_pMemDC = NULL;
	m_pMemBitmap = NULL;
	m_pRGB = NULL;
	m_lNumberOfColours = 0;
	m_lProcessingTime = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMandelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMandelDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

		EnterCriticalSection(&OutputCS);
		
		DrawOutput();

		LeaveCriticalSection(&OutputCS);
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMandelDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CMandelDlg::DrawSelectArea(CPoint &rPt1, CPoint &rPt2)
{
	CWnd *pWnd = GetDlgItem(IDC_OUTPUT);
	ASSERT(pWnd);

	CDC *pDC = pWnd->GetDC();

	CPen *pPen = pDC->SelectObject(m_pWhitePen);
	int nROP2 = pDC->SetROP2(R2_XORPEN);
	CBrush *pBrush = pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));

	pDC->Rectangle(min(rPt1.x, rPt2.x), min(rPt1.y, rPt2.y),
				   max(rPt1.x, rPt2.x), max(rPt1.y, rPt2.y));

	pDC->SetROP2(nROP2);
	pDC->SelectObject(pPen);
	pDC->SelectObject(pBrush);
	pWnd->ReleaseDC(pDC);
}


void CMandelDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CMandelDlg::OnCalculate() 
{
	// disable SAVE button
	m_SaveBtn.EnableWindow(FALSE);

	// disable export button
	m_ExportBtn.EnableWindow(FALSE);

	if (m_pMandel)
	{
		// if we're already processing an image then stop it
		if (m_pMandel->m_bCalculate)
		{
			if (AfxMessageBox("Abort processing?", MB_YESNO|MB_ICONQUESTION) == IDYES)
			{
				EnterCriticalSection(&OutputCS);

				m_pMandel->m_bCalculate = FALSE;

				// clear output window
				ClearOutput();

				// show the processing time
				UpdateProcessingTime();

				// set caption
				SetCaption(0);

				// set button text back to "CALCULATE"
				m_CalculateBtn.SetWindowText((LPCTSTR)"CALCULATE");

				// delete bitmap & memory DC
				if (m_pMemBitmap)
				{
					delete m_pMemBitmap;
					m_pMemBitmap = NULL;
				}

				if (m_pMemDC)
				{
					delete m_pMemDC;
					m_pMemDC = NULL;
				}

				LeaveCriticalSection(&OutputCS);
			}
				
			// exit
			return;
		}
		else
		{
			// delete current Mandelbrot object
			delete m_pMandel;
			m_pMandel = NULL;
		}
	}

	ClearOutput();
	ClearProcessingTime();

	// create new Mandelbrot
	m_pMandel = new CMandelbrot();
	ASSERT(m_pMandel);

	// update dialog box data
	UpdateData();

	m_pMandel->SetArraySize(m_ImageSize);
	m_pMandel->SetMaxIterations(m_MaxIterations);
	m_pMandel->SetRealCorner(m_RealCorner);
	m_pMandel->SetImaginaryCorner(m_ImagCorner);
	m_pMandel->SetSideLength(m_Side);

	// create memory DC
	CreateMemDC((int)m_ImageSize);

	// create RGB colour values
	SetupColourTable(m_MaxIterations);

	// update image values
	DrawImageValues();

	// set name to "Untitled"
	m_strName = "Untitled";

	m_dwStartTime = GetTickCount();

	// start calculating the image array
	m_pMandel->Calculate(CalculateCallback);
	
	// set text in CALCULATE button to "STOP"
	m_CalculateBtn.SetWindowText((LPCTSTR)"STOP");
}


int CALLBACK CalculateCallback(int iRow, long *pRowData)
{
	if (pMainDlg)
	{
		if (pMainDlg->m_pMandel == NULL)
			return 0;

		EnterCriticalSection(&OutputCS);

		CWnd *pWnd = pMainDlg->GetDlgItem(IDC_OUTPUT);
		ASSERT(pWnd);

		CRect rcClient;
		pWnd->GetClientRect(&rcClient);

		CDC *pDC = pWnd->GetDC();
		ASSERT(pDC);

		int iImageSize = (int)pMainDlg->m_ImageSize;
		int nMapMode = pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(iImageSize, iImageSize);
		pDC->SetViewportExt(rcClient.right, rcClient.bottom);

		CRect rc;
		CBrush *pBrush;

		for (int iCol = 0; iCol < iImageSize; iCol++)
		{
			rc.SetRect(iCol, iRow, (iCol+1), (iRow+1));
			pBrush = new CBrush(pMainDlg->m_pRGB[*pRowData]);
			pMainDlg->m_pMemDC->FillRect(&rc, pBrush);
			delete pBrush;

			pRowData++;
		}

		pDC->StretchBlt(0, iRow, iImageSize, 1, pMainDlg->m_pMemDC,
						0, iRow, iImageSize, 1, SRCCOPY);
	
		pDC->SetMapMode(nMapMode);
		pWnd->ReleaseDC(pDC);

		LeaveCriticalSection(&OutputCS);

		// set percentage completed caption
		int iPercentCompleted = (int)(((double)(iImageSize-iRow) / (double)iImageSize) * 100.0);
		pMainDlg->SetCaption(iPercentCompleted);

		// show processing time if we're 100% complete &
		// enable SAVE button
		if (iPercentCompleted == 100)
		{
			pMainDlg->UpdateProcessingTime();

			// set text in CALCULATE button back to "CALCULATE"
			pMainDlg->m_CalculateBtn.SetWindowText((LPCTSTR)"CALCULATE");

			// enable save button
			pMainDlg->m_SaveBtn.EnableWindow(TRUE);

			// enable export button
			pMainDlg->m_ExportBtn.EnableWindow(TRUE);
		}
	}

	return 0;
}


void CMandelDlg::CreateMemDC(int iImageSize)
{
	if (m_pMemBitmap)
	{
		delete m_pMemBitmap;
		m_pMemBitmap = NULL;
	}

	if (m_pMemDC)
	{
		delete m_pMemDC;
		m_pMemDC = NULL;
	}

	CWnd *pWnd = GetDlgItem(IDC_OUTPUT);
	ASSERT(pWnd);

	CDC *pDC = pWnd->GetDC();
	ASSERT(pDC);

	m_pMemDC = new CDC();
	m_pMemDC->CreateCompatibleDC(pDC);

	m_pMemBitmap = new CBitmap();
	m_pMemBitmap->CreateCompatibleBitmap(pDC, iImageSize, iImageSize);

	m_pMemDC->SelectObject(m_pMemBitmap);
}


void CMandelDlg::DrawOutput()
{
	if (m_pMandel == NULL || m_pMemDC == NULL)
	{
		ClearOutput();
		return;
	}

	// draw mandelbrot image
	CWnd *pWnd = GetDlgItem(IDC_OUTPUT);
	ASSERT(pWnd);

	CRect rcClient;
	pWnd->GetClientRect(&rcClient);

	CDC *pDC = pWnd->GetDC();
	ASSERT(pDC);

	int iImageSize = (int)m_pMandel->GetArraySize();

	int nMapMode = pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(iImageSize, iImageSize);
	pDC->SetViewportExt(rcClient.right, rcClient.bottom);

	pDC->StretchBlt(0, 0, iImageSize, iImageSize, m_pMemDC,
					0, 0, iImageSize, iImageSize, SRCCOPY);
	

	pDC->SetMapMode(nMapMode);
	pWnd->ReleaseDC(pDC);

	if (m_bAreaSelected)
		DrawSelectArea(m_ptMouseDown, m_ptDrag);
}


void CMandelDlg::ClearOutput()
{
	// clear output window
	CWnd *pWnd = GetDlgItem(IDC_OUTPUT);
	
	CRect rcClient;
	pWnd->GetClientRect(&rcClient);
	
	CDC *pDC = pWnd->GetDC();
	pDC->FillRect(&rcClient, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
	pWnd->ReleaseDC(pDC);

	m_bAreaSelected = FALSE;
}


void CMandelDlg::DrawImageValues()
{
	if (m_pMandel == NULL)
		return;

	double dImagBottom = m_pMandel->GetImaginaryCorner();
	double dRealLeft = m_pMandel->GetRealCorner();
	double dSide = m_pMandel->GetSideLength();
	double dSideDiv2 = dSide / 2.0;

	m_ImagSideBottom.Format((LPCTSTR)"%.3f", dImagBottom);
	m_ImagSideMid.Format((LPCTSTR)"%.3f", (dImagBottom+dSideDiv2));
	m_ImagSideTop.Format((LPCTSTR)"%.3f", (dImagBottom+dSide));
	m_RealSideLeft.Format((LPCTSTR)"%.3f", dRealLeft);
	m_RealSideMid.Format((LPCTSTR)"%.3f", (dRealLeft+dSideDiv2));
	m_RealSideRight.Format((LPCTSTR)"%.3f", (dRealLeft+dSide));

	UpdateData(FALSE);
}


void CMandelDlg::OnSave() 
{
	if (m_pMandel)
	{
		CFileDialog dlg(FALSE, (LPCTSTR)DATA_FILE_EXT, NULL,
						OFN_OVERWRITEPROMPT, (LPCTSTR)FILE_FILTER,
						this);

		if (dlg.DoModal() == IDOK)
		{
			// get filename from dialog
			CString strFilename = dlg.GetFileName();

			// create archive file
			CFile SaveFile((LPCTSTR)strFilename, 
							CFile::modeCreate | CFile::modeWrite);
			CArchive SaveArchive (&SaveFile, CArchive::store);

			// save Mandelbrot object
			SaveArchive << m_pMandel;

			// save processing time
			SaveArchive << m_lProcessingTime;

			// close archive & file
			SaveArchive.Close();
			SaveFile.Close();

			m_strName = dlg.GetFileTitle();
			SetCaption(100);
		}
	}
}

	
void CMandelDlg::OnLoad() 
{
	CFileDialog dlg(TRUE, (LPCTSTR)DATA_FILE_EXT, NULL,
					OFN_FILEMUSTEXIST, (LPCTSTR)FILE_FILTER,
					this);

	if (dlg.DoModal() == IDOK)
	{
		CWaitCursor wait;

		CString strFilename = dlg.GetFileName();

		// delete current Mandelbrot if any
		if (m_pMandel)
			delete m_pMandel;

		// create new Mandelbrot
		m_pMandel = new CMandelbrot();
		ASSERT(m_pMandel);

		CFile LoadFile((LPCTSTR)strFilename, CFile::modeRead);
		CArchive LoadArchive (&LoadFile, CArchive::load);

		// load Mandelbrot object
		LoadArchive	>> m_pMandel;

		// load processing time
		LoadArchive >> m_lProcessingTime;

		LoadArchive.Close();
		LoadFile.Close();

		m_strName = dlg.GetFileTitle();
		SetCaption(100);

		// setup colours
		SetupColourTable(m_pMandel->GetMaxIterations());

		m_ImageSize = m_pMandel->GetArraySize();
		m_MaxIterations = m_pMandel->GetMaxIterations();
		m_RealCorner = m_pMandel->GetRealCorner();
		m_ImagCorner = m_pMandel->GetImaginaryCorner();
		m_Side = m_pMandel->GetSideLength();
		ShowProcessingTime(m_lProcessingTime);
		UpdateData(FALSE);

		// create new memory DC
		CreateMemDC((int)m_ImageSize);
		ASSERT(m_pMemDC);

		CRect rc;
		CBrush *pBrush;
		int	  iCol, iRow = (int)m_ImageSize -1;
		long  *pData = m_pMandel->m_pArray;

		for (long lRow = 0; lRow < m_ImageSize; lRow++)
		{
			for (iCol = 0; iCol < (int)m_ImageSize; iCol++)
			{
				rc.SetRect(iCol, iRow, (iCol+1), (iRow+1));
				pBrush = new CBrush(m_pRGB[*pData]);
				m_pMemDC->FillRect(&rc, pBrush);
				delete pBrush;

				pData++;
			}

			iRow--;
		}

		// update image values
		DrawImageValues();

		// draw the image
		DrawOutput();

		// enable save button
		m_SaveBtn.EnableWindow(TRUE);

		// enable export button
		m_ExportBtn.EnableWindow(TRUE);
	}
}


void CMandelDlg::SetCaption(int iPercentCompleted)
{
	if (pMainDlg)
	{
		char szTitle[256];

		if (iPercentCompleted == 100)
		{
			wsprintf((LPTSTR)szTitle,
					 (LPCTSTR)"Mandelbrot - %s", (LPCTSTR)m_strName);
		}
		else
		{
			if (m_pMandel->m_bCalculate)
			{
				wsprintf((LPTSTR)szTitle,
						 (LPCTSTR)"Mandelbrot - %s (Processing, %d%c Completed)",
						 (LPCTSTR)m_strName, iPercentCompleted, '%');
			}
			else
			{
				wsprintf((LPTSTR)szTitle,
						 (LPCTSTR)"Mandelbrot - %s (Stopped)", (LPCTSTR)m_strName);
			}
		}

		pMainDlg->SetWindowText((LPCTSTR)szTitle);
	}
}

/*
void CMandelDlg::OnColours() 
{
	ASSERT(m_pMandel);

	CColourSetupDlg dlg(m_pMandel->GetColourTable(), this);

	if (dlg.DoModal() == IDOK)
	{

	}
}
*/


void CMandelDlg::ConvertPtToImage(CPoint &rPt)
{
	// convert dialog client point to screen coordinates
	ClientToScreen(&rPt);

	CWnd *pWnd = GetDlgItem(IDC_OUTPUT);
	ASSERT(pWnd);

	// convert screen coordinates to image client coordinates
	pWnd->ScreenToClient(&rPt);
}


void CMandelDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pMandel && m_pMemDC)
	{
		if (!m_bDragArea && !m_pMandel->m_bCalculate)
		{
			// clear previously selected area
			if (m_bAreaSelected)
				DrawSelectArea(m_ptMouseDown, m_ptDrag);

			SetCapture();
			m_bDragArea = TRUE;

			ConvertPtToImage(point);
			m_ptMouseDown = point;
			m_ptDrag = point;
			DrawSelectArea(m_ptMouseDown, m_ptDrag);
		}
	}
	
	//CDialog::OnLButtonDown(nFlags, point);
}


void CMandelDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// save point
	CPoint ptCursor = point;

	CWnd *pWnd = GetDlgItem(IDC_OUTPUT);
	ASSERT(pWnd);

	CRect rc;
	pWnd->GetWindowRect(&rc);
	ClientToScreen(&point);

	HCURSOR hCursor = (rc.PtInRect(point)) ? 
					   ::LoadCursor(NULL, IDC_CROSS) : 
					   ::LoadCursor(NULL, IDC_ARROW);
					   
	SetCursor(hCursor);

	if (m_bDragArea)
	{
		// erase previous selected area
		DrawSelectArea(m_ptMouseDown, m_ptDrag);

		// draw new selected area
		ConvertPtToImage(ptCursor);
		m_ptDrag = ptCursor;
		DrawSelectArea(m_ptMouseDown, ptCursor);
	}

	//CDialog::OnMouseMove(nFlags, point);
}

void CMandelDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_pMandel && m_bDragArea)
	{
		ReleaseCapture();
		m_bDragArea = FALSE;
		m_bAreaSelected = TRUE;
		SetSelectedAreaParameters();

		// if auto zoom enable fire Calculate
		if (m_bAutoZoom)
			OnCalculate();
	}

	CDialog::OnLButtonUp(nFlags, point);
}


void CMandelDlg::SetSelectedAreaParameters()
{
	int xMin = min(m_ptMouseDown.x, m_ptDrag.x);
	int yMax = max(m_ptMouseDown.y, m_ptDrag.y);
	int Side = min(abs(m_ptMouseDown.x - m_ptDrag.x),
				   abs(m_ptMouseDown.y - m_ptDrag.y));
	if (Side < 1)
		Side = 1;

	CWnd *pWnd = GetDlgItem(IDC_OUTPUT);
	ASSERT(pWnd);

	CRect rcClient;
	pWnd->GetClientRect(&rcClient);

	double dRealCorner = m_pMandel->GetRealCorner() + 
			(((double)xMin / (double)rcClient.right) * m_pMandel->GetSideLength());
	double dImagCorner = m_pMandel->GetImaginaryCorner() + 
			(((double)(rcClient.bottom-yMax-1) / (double)rcClient.bottom) * m_pMandel->GetSideLength());		
	double dSide = ((double)Side / (double)rcClient.right) * m_pMandel->GetSideLength();

	m_RealCorner = dRealCorner;
	m_ImagCorner = dImagCorner;
	m_Side = dSide;
	UpdateData(FALSE);
}



void CMandelDlg::SetupColourTable(long lNumColours)
{
	if (m_pRGB)
	{
		// has number of colours changed?
		if (lNumColours == m_lNumberOfColours)
			return;

		delete m_pRGB;

		m_pRGB = new COLORREF [lNumColours + 1];
		m_lNumberOfColours = lNumColours;
	}
	else
	{
		m_pRGB = new COLORREF [lNumColours + 1];
		m_lNumberOfColours = lNumColours;
	}

	ASSERT(m_pRGB);

	long lColSeperation = (long)RGB(255,255,255) - (long)RGB(255,0,0);

	for (long lIndex = 0; lIndex < lNumColours; lIndex++)
		m_pRGB[lIndex] = (COLORREF)((long)(((double)lIndex / (double)lNumColours) * (double)lColSeperation) + (long)RGB(255,0,0));

	m_pRGB[lNumColours] = MANDELBROT_PIXEL_COLOUR;
}


void CMandelDlg::UpdateProcessingTime()
{
	DWORD dwMilliseconds = GetTickCount() - m_dwStartTime;

	m_lProcessingTime = (long)dwMilliseconds / 1000;
	ShowProcessingTime(m_lProcessingTime);
}


void CMandelDlg::ClearProcessingTime()
{
	CWnd *pWnd = GetDlgItem(IDC_PROCESSING_TIME);
	ASSERT(pWnd);

	pWnd->SetWindowText((LPCTSTR)"");
}


void CMandelDlg::ShowProcessingTime(long lSeconds)
{
	long lMins = lSeconds / 60;
	lSeconds = lSeconds % 60;

	if (lMins > 1)
		m_ProcessingTime.Format((LPCTSTR)"%ld Mins  %ld Secs",
								lMins, lSeconds);
	else
		m_ProcessingTime.Format((LPCTSTR)"%ld Min  %ld Secs",
								lMins, lSeconds);

	CWnd *pWnd = GetDlgItem(IDC_PROCESSING_TIME);
	ASSERT(pWnd);

	pWnd->SetWindowText((LPCTSTR)m_ProcessingTime);
}



void CMandelDlg::OnOK() 
{
	// exit button

	if (m_pMandel)
	{
		// terminate calculation thread if necessary
		if (m_pMandel->m_bCalculate)
			m_pMandel->m_bCalculate = FALSE;
	}
	
	// call base class to end dialog
	CDialog::OnOK();
}

void CMandelDlg::OnExport() 
{
	if (m_pMandel)
	{
		CFileDialog dlg(FALSE, (LPCTSTR)BITMAP_EXT, NULL,
						OFN_OVERWRITEPROMPT, (LPCTSTR)EXPORT_FILTER,
						this);

		if (dlg.DoModal() == IDOK)
		{
			CWaitCursor wait;

			// get filename from dialog
			CString strFilename = dlg.GetFileName();

			BOOL bSuccess = ExportBitmap((LPCTSTR)strFilename);
			if (!bSuccess)
				AfxMessageBox("Error exporting bitmap", MB_ICONEXCLAMATION);
		}
	}
}


BOOL CMandelDlg::ExportBitmap(LPCTSTR lpszFilename)
{
	if (m_pMandel == NULL || m_pMemBitmap == NULL || 
		m_pRGB == NULL)
		return FALSE;

	// open file for writing
	FILE *fp = fopen(lpszFilename, "wb");
	if (fp == NULL)                                   
 		return FALSE;

    BITMAPFILEHEADER	bmfh;
    BITMAPINFOHEADER	bmi;
    DWORD	dwImageSize;
	BOOL	bSuccess = TRUE;
	long	*pArray;
	unsigned char *pucRowData;
	DWORD dwBytesPerLine;
	long	lMaxIterations = m_pMandel->GetMaxIterations();

	// get image array dimensions
	long	lArraySize = m_pMandel->GetArraySize();

	// calculate number of bytes for bitmap image
    dwImageSize   = (DWORD)lArraySize * (DWORD)lArraySize * 3;  

    // fill bitmapinfo header     
    memset (&bmi, 0, sizeof (BITMAPINFOHEADER));
    bmi.biSize       	= (DWORD)sizeof(BITMAPINFOHEADER);
    bmi.biWidth	   		= lArraySize;
    bmi.biHeight   		= lArraySize;
    bmi.biPlanes   		= 1;
    bmi.biBitCount 		= 24;
    bmi.biCompression	= BI_RGB;		// no compression scheme
    bmi.biSizeImage		= dwImageSize;

    // fill bitmap file header
    bmfh.bfType		= 0x4d42;      // bitmap file signature
    bmfh.bfSize		= (DWORD)sizeof(BITMAPFILEHEADER) + 
					  (DWORD)sizeof(BITMAPINFOHEADER) + dwImageSize;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits   = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // write the bitmap file header first
    if (!fwrite((void *)&bmfh, sizeof(BITMAPFILEHEADER), 1, fp))
	{
		// there was an error writing
		goto error_writing;
	}

   // next the bitmapinfo header
    if (!fwrite((void *)&bmi, sizeof(BITMAPINFOHEADER), 1, fp))
    {
		// there was an error writing
		goto error_writing;
    }
    
    // write pixel data
    // bits are written in order Blue, Green, Red  

    // calculate number of bytes per raster line
    dwBytesPerLine = (DWORD)lArraySize * 3;

    // alignment is on a DWORD boundary so calculate pad bytes
    if (dwBytesPerLine % 4)
		dwBytesPerLine = ((dwBytesPerLine >> 2) << 2) +4;
    	
    // Allocate memory for row data
    pucRowData = new unsigned char [dwBytesPerLine];  
    ASSERT(pucRowData);

	long lRow, lCol;
	unsigned char *pucPixel;
	COLORREF rgbCol;
	pArray = m_pMandel->m_pArray;
	lMaxIterations = m_pMandel->GetMaxIterations();

	for (lRow = 0; lRow < lArraySize; lRow++)
	{
		pucPixel = pucRowData;
		for (lCol = 0; lCol < lArraySize; lCol++)
		{
			if (*pArray >= lMaxIterations)
			{
				*pucPixel++ = 0;
				*pucPixel++ = 0;
				*pucPixel++ = 0;
			}
			else
			{
				rgbCol = m_pRGB[*pArray];
				*pucPixel++ = GetBValue(rgbCol);
				*pucPixel++ = GetGValue(rgbCol);
				*pucPixel++ = GetRValue(rgbCol);
			}

			pArray++;
		}

    	// write row to file
    	if (!fwrite((void *)pucRowData, dwBytesPerLine, 1, fp))
		{
			// an error occurred writing to file
			delete pucRowData;
			goto error_writing;
		}
	}
	
    // free row memory
    delete pucRowData;
    
	// close the file & exit
	fclose(fp);   

	return bSuccess;


error_writing:
	// an error occurred whilst writing the image file
	// so close the file & delete it
	fclose(fp);
	DeleteFile(lpszFilename);
	return FALSE;
}

void CMandelDlg::OnAutoZoom() 
{
	// toggle autozoom
	m_bAutoZoom = !m_bAutoZoom;
}
