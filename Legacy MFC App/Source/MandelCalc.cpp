//
//	MandelCalc.cpp
//

#include "MandelCalc.hpp"
#include "math.h"


extern CRITICAL_SECTION	OutputCS;


IMPLEMENT_SERIAL(CMandelbrot,CObject,1)



CMandelbrot::CMandelbrot()
{
	// initialise members
	m_lArraySize		= DEFAULT_ARRAYSIZE;
	m_lMaxIterations	= DEFAULT_MAXITERATIONS;
	m_dRealCorner		= DEFAULT_REALCORNER;
	m_dImagCorner		= DEFAULT_IMAGCORNER;
	m_dSide				= DEFAULT_SIDE;

	m_pArray = NULL;
	m_lpProgressFunc = NULL;
	m_bCalculate = FALSE;
}


CMandelbrot::~CMandelbrot()
{
	// delete any allocated memory
	DeleteMemory();
}


BOOL CMandelbrot::AllocateMemory()
{
	// first delete any currently allocated memory
	DeleteMemory();

	// allocate memory for array
	long lNumberOfPixels = m_lArraySize*m_lArraySize;

	m_pArray = new long[lNumberOfPixels];
	if (m_pArray == NULL)
		return FALSE;

	return TRUE;
}


void CMandelbrot::DeleteMemory()
{
	if (m_pArray)
	{
		delete m_pArray;
		m_pArray = NULL;
	}
}


long CMandelbrot::GetArraySize()
{
	return m_lArraySize;
}


BOOL CMandelbrot::SetArraySize(long lArraySize)
{
	if (lArraySize < MIN_ARRAY_SIZE ||
		lArraySize > MAX_ARRAY_SIZE)
		return FALSE;

	m_lArraySize = lArraySize;
	return TRUE;
}


long CMandelbrot::GetMaxIterations()
{
	return m_lMaxIterations;
}


BOOL CMandelbrot::SetMaxIterations(long lMaxIterations)
{
	if (lMaxIterations < MIN_ITERATIONS ||
		lMaxIterations > MAX_ITERATIONS)
		return FALSE;

	m_lMaxIterations = lMaxIterations;
	return TRUE;
}


double CMandelbrot::GetRealCorner()
{
	return m_dRealCorner;
}


BOOL CMandelbrot::SetRealCorner(double dRealCorner)
{
	m_dRealCorner = dRealCorner;
	return TRUE;
}


double CMandelbrot::GetImaginaryCorner()
{
	return m_dImagCorner;
}


BOOL CMandelbrot::SetImaginaryCorner(double dImagCorner)
{
	m_dImagCorner = dImagCorner;
	return TRUE;
}


double CMandelbrot::GetSideLength()
{
	return m_dSide;
}


BOOL CMandelbrot::SetSideLength(double dSide)
{
	if (dSide == 0.0)
		return FALSE;

	m_dSide = dSide;
	return TRUE;
}


void CMandelbrot::Calculate(LPPROGRESSCALLBACKFUNC lpProgressFunc)
{
	// set callback function pointer
	m_lpProgressFunc = lpProgressFunc;

	// allocate memory
	if (!AllocateMemory())
	{
		AfxMessageBox("ERROR: Unable to allocate memory");
		return;
	}

	// fire off calculation thread
	m_bCalculate = TRUE;
	AfxBeginThread((AFX_THREADPROC)CalculateThread, (LPVOID)this);
}



UINT CMandelbrot::CalculateThread(LPVOID lpData)
{
	//CWaitCursor wait;

	CMandelbrot *pThis = (CMandelbrot*)lpData;
	ASSERT(pThis);

	long lArraySize = pThis->m_lArraySize;
	ASSERT(lArraySize > 0);

	long lMaxIter = pThis->m_lMaxIterations;
	long *pArray   = pThis->m_pArray;
	long *pRowData = pArray;
	LPPROGRESSCALLBACKFUNC lpCallback = pThis->m_lpProgressFunc;

	// calculate gap value
	double dGap = pThis->m_dSide / (double)lArraySize;

	int  iRow = lArraySize -1;
	long lRow, lColumn, lCount;
	double	ac, bc, az, bz, fa, fb, fSize;

	// set imaginary component
	bc = pThis->m_dImagCorner;
	
	// row loop
	for (lRow = 0; lRow < lArraySize; lRow++)
	{
		if (!pThis->m_bCalculate)
			break;

		// set real component
		ac = pThis->m_dRealCorner;

		// column loop
		for (lColumn = 0; lColumn < lArraySize; lColumn++)
		{
			az = 0.0;
			bz = 0.0;
			lCount = 0;

			while (lCount++ < lMaxIter && pThis->m_bCalculate)
			{
				fa = az;
				fb = bz;
				az = (fa*fa)-(fb*fb)+ac;
				bz = (2.0*fa*fb)+bc;
				fSize = sqrt((az*az)+(bz*bz));

				if (fSize > 2.0)
					break;
			}

			// exit if we're terminating calculation
			if (!pThis->m_bCalculate)
				break;

			// save iteration value in array
			*pArray++ = lCount;

			// increment real component
			ac += dGap;
		}
	
		if (!pThis->m_bCalculate)
			break;

		// output row
		if (lpCallback)
			(*lpCallback)(iRow, pRowData);

		bc += dGap;

		iRow--;
		pRowData += lArraySize;
	}

	// exit & terminate thread
	pThis->m_bCalculate = FALSE;
	return 0;
}


void CMandelbrot::SaveData(LPCTSTR lpszFilename)
{
	CFile SaveFile(lpszFilename, CFile::modeCreate | CFile::modeWrite);
	CArchive SaveArchive (&SaveFile, CArchive::store);
	//SerializeData(SaveArchive);	
	
	SaveArchive.Close();
	SaveFile.Close();
}

void CMandelbrot::LoadData(LPCTSTR lpszFilename)
{
	CFile LoadFile(lpszFilename, CFile::modeRead);
	CArchive LoadArchive (&LoadFile, CArchive::load);
	//SerializeData(LoadArchive);	
	LoadArchive.Close();
	LoadFile.Close();
}


void CMandelbrot::Serialize(CArchive &ar)
{
	// serialize base class first
	CObject::Serialize(ar);

	long lIndex, lImageSize;
	CString strID;
	int  iVersion;

	if (ar.IsStoring())
	{
		// we're saving the data

		strID = (LPCTSTR)DATA_ARCHIVE_ID;
		iVersion = ARCHIVE_VERSION;

		ar << strID;				// archive ID
		ar << iVersion;				// archive version number
		ar << m_lArraySize;			// image size
		ar << m_lMaxIterations;		// max number of iterations
		ar << m_dRealCorner;		// real corner
		ar << m_dImagCorner;		// imaginary corner
		ar << m_dSide;				// length of side

		// save array data
		lImageSize = m_lArraySize*m_lArraySize;
		for (lIndex = 0; lIndex < lImageSize; lIndex++)
			ar << m_pArray[lIndex];
	}
	else
	{
		// we're loading the data
		long lArraySize, lMaxIterations;
		double dRealCorner, dImagCorner, dSide;

		ar >> strID;					// archive ID

		// check archive ID string
		if (strID.Compare((LPCTSTR)DATA_ARCHIVE_ID) != 0)
		{
			// invalid archive - throw an exception
			return;
		}

		ar >> iVersion;				// archive version number
		ar >> lArraySize;			// image size
		ar >> lMaxIterations;		// max number of iterations
		ar >> dRealCorner;			// real corner
		ar >> dImagCorner;			// imaginary corner
		ar >> dSide;				// length of side

		//
		//ar >> m_lArraySize;			// image size
		//ar >> m_lMaxIterations;		// max number of iterations
		//ar >> m_dRealCorner;		// real corner
		//ar >> m_dImagCorner;		// imaginary corner
		//ar >> m_dSide;				// length of side
		//

		// allocate memory
		AllocateMemory();

		m_lArraySize = lArraySize;		
		m_lMaxIterations = lMaxIterations;	// max number of iterations
		m_dRealCorner = dRealCorner;		// real corner
		m_dImagCorner = dImagCorner;		// imaginary corner
		m_dSide = dSide;

		// save array data
		lImageSize = lArraySize*lArraySize;
		for (lIndex = 0; lIndex < lImageSize; lIndex++)
			ar >> m_pArray[lIndex];
	}
}


/*
void CMandelbrot::SerializeData(CArchive &ar)
{
	//ASSERT(m_pArray);
	//ASSERT(m_pRGB);
	
	// serialize base class first
	//CObject::Serialize(ar);

	long lIndex, lImageSize;
	CString strID;
	int  iVersion;

	if (ar.IsStoring())
	{
		// we're saving the data

		strID = (LPCTSTR)DATA_ARCHIVE_ID;
		iVersion = ARCHIVE_VERSION;

		ar << strID;				// archive ID
		ar << iVersion;				// archive version number
		ar << m_lArraySize;			// image size
		ar << m_lMaxIterations;		// max number of iterations
		ar << m_dRealCorner;		// real corner
		ar << m_dImagCorner;		// imaginary corner
		ar << m_dSide;				// length of side

		// save array data
		lImageSize = m_lArraySize*m_lArraySize;
		for (lIndex = 0; lIndex < lImageSize; lIndex++)
			ar << m_pArray[lIndex];
	}
	else
	{
		// we're loading the data
		long lArraySize, lMaxIterations;
		double dRealCorner, dImagCorner, dSide;

		ar >> strID;					// archive ID

		// check archive ID string
		if (strID.Compare((LPCTSTR)DATA_ARCHIVE_ID) != 0)
		{
			// invalid archive - throw an exception
			return;
		}

		ar >> iVersion;				// archive version number
		ar >> lArraySize;			// image size
		ar >> lMaxIterations;		// max number of iterations
		ar >> dRealCorner;			// real corner
		ar >> dImagCorner;			// imaginary corner
		ar >> dSide;				// length of side

		//
		//ar >> m_lArraySize;			// image size
		//ar >> m_lMaxIterations;		// max number of iterations
		//ar >> m_dRealCorner;		// real corner
		//ar >> m_dImagCorner;		// imaginary corner
		//ar >> m_dSide;				// length of side
		//

		// allocate memory
		AllocateMemory();

		m_lArraySize = lArraySize;		
		m_lMaxIterations = lMaxIterations;	// max number of iterations
		m_dRealCorner = dRealCorner;		// real corner
		m_dImagCorner = dImagCorner;		// imaginary corner
		m_dSide = dSide;

		// save array data
		lImageSize = lArraySize*lArraySize;
		for (lIndex = 0; lIndex < lImageSize; lIndex++)
			ar >> m_pArray[lIndex];
	}
}
*/


/*
CColourTable::CColourTable()
{
	// set default Mandelbrot pixel colour (black)
	m_MandelbrotPixelRGB = MANDELBROT_PIXEL_COLOUR;

	m_lNumberOfColours = 0;
	m_pMinIteration = NULL;
	m_pMaxIteration = NULL;
	m_pRGB = NULL;
}


CColourTable::~CColourTable()
{
	// delete allocated memory
	DeleteMemory();
}


BOOL CColourTable::AllocateMemory()
{
	// delete any allocated memory
	DeleteMemory();

	m_pRGB = new COLORREF[DEFAULT_COLOURS];
	if (m_pRGB == NULL)
		return FALSE;

	m_pMinIteration = new long [DEFAULT_COLOURS];
	m_pMaxIteration = new long [DEFAULT_COLOURS];
	if (m_pMinIteration == NULL ||
		m_pMaxIteration == NULL)
	{
		DeleteMemory();
		return FALSE;
	}

	return TRUE;
}


void CColourTable::DeleteMemory()
{
	if (m_pMinIteration)
	{
		delete m_pMinIteration;
		m_pMinIteration = NULL;
	}

	if (m_pMaxIteration)
	{
		delete m_pMaxIteration;
		m_pMaxIteration = NULL;
	}

	if (m_pRGB)
	{
		delete m_pRGB;
		m_pRGB = NULL;
	}
}


void CColourTable::CreateColourTable(long lMaxIterations)
{
	DeleteMemory();

	// set number of colours
	m_lNumberOfColours = lMaxIterations-1;

	// set maximum iterations
	m_lMaxIterations = lMaxIterations;

	m_pRGB = new COLORREF [m_lNumberOfColours];
	ASSERT(m_pRGB);

	long lColSeperation = (long)RGB(255,0,255) - (long)RGB(255,0,0);

	for (long lIndex = 0; lIndex < m_lNumberOfColours; lIndex++)
		m_pRGB[lIndex] = (COLORREF)((long)(((double)lIndex / (double)m_lNumberOfColours) * (double)lColSeperation) + (long)RGB(255,0,0));
}


long CColourTable::GetNumberOfColours()
{
	return m_lNumberOfColours;
}


long CColourTable::GetMaxIterations()
{
	return m_lMaxIterations;
}


int CColourTable::GetRedValue(long lIndex)
{
	ASSERT(m_pRGB);
	ASSERT(lIndex >= 0 && lIndex < m_lNumberOfColours);

	return (int)GetRValue(m_pRGB[lIndex]);
}


int CColourTable::GetGreenValue(long lIndex)
{
	ASSERT(m_pRGB);
	ASSERT(lIndex >= 0 && lIndex < m_lNumberOfColours);

	return (int)GetGValue(m_pRGB[lIndex]);
}


int CColourTable::GetBlueValue(long lIndex)
{
	ASSERT(m_pRGB);
	ASSERT(lIndex >= 0 && lIndex < m_lNumberOfColours);

	return (int)GetBValue(m_pRGB[lIndex]);
}


COLORREF CColourTable::GetColourFromIndex(long lIndex)
{
	ASSERT(m_pRGB);
	ASSERT(lIndex >= 0 && lIndex < m_lNumberOfColours);

	return m_pRGB[lIndex];
}


COLORREF CColourTable::GetColourFromIterations(long lIterations)
{
	ASSERT(m_pRGB);
	//ASSERT(m_pMinIteration);
	//ASSERT(m_pMaxIteration);
	ASSERT(lIterations >= 0);

	// check for Mandelbrot pixel
	if (lIterations >= m_lMaxIterations)
		return m_MandelbrotPixelRGB;

	return m_pRGB[lIterations];




}


void CColourTable::SetColour(long lIndex, COLORREF rgbColour,
							 long lMinIteration,
							 long lMaxIteration)
{
	ASSERT(m_pRGB);
	ASSERT(m_pMinIteration);
	ASSERT(m_pMaxIteration);
	ASSERT(lIndex >= 0 && lIndex < m_lNumberOfColours);

	m_pRGB[lIndex] = rgbColour;
	m_pMinIteration[lIndex] = lMinIteration;
	m_pMaxIteration[lIndex] = lMaxIteration;
}
*/

/*
void CColourTable::Serialize(CArchive &ar)
{
	// perform base class serialization
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		// we're saving the data
		
		ASSERT(m_pRGB);
		ASSERT(m_pMinIteration);
		ASSERT(m_pMaxIteration);

		ar << m_lNumberOfColours;		// number of colours
		ar << m_lMaxIterations;			// max number of iterations
		ar << m_MandelbrotPixelRGB;		// Mandelbrot pixel colour

		for (long lIndex = 0; lIndex < m_lNumberOfColours; lIndex++)
		{
			ar << m_pRGB[lIndex];
			ar << m_pMinIteration[lIndex];
			ar << m_pMaxIteration[lIndex];
		}
	}
	else
	{
		// we're loading the data

		ar >> m_lNumberOfColours;		// number of colours
		ar >> m_lMaxIterations;			// max number of iterations
		ar >> m_MandelbrotPixelRGB;		// Mandelbrot pixel colour

		AllocateMemory();

		for (long lIndex = 0; lIndex < m_lNumberOfColours; lIndex++)
		{
			ar >> m_pRGB[lIndex];
			ar >> m_pMinIteration[lIndex];
			ar >> m_pMaxIteration[lIndex];
		}
	}
}
*/
