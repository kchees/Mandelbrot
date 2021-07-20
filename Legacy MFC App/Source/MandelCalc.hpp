
//
//	MandelCalc.hpp
//

#include "stdafx.h"


#pragma once

#define MIN_ARRAY_SIZE		100
#define MAX_ARRAY_SIZE		10000
#define MAX_COLOURS			1000

#define MIN_ITERATIONS		100
#define MAX_ITERATIONS		100000

#define DEFAULT_ARRAYSIZE		1000
#define	DEFAULT_MAXITERATIONS	1000
#define DEFAULT_REALCORNER		-2.0
#define DEFAULT_IMAGCORNER		-1.25
#define DEFAULT_SIDE			2.50
#define DEFAULT_COLOURS			16

#define MANDELBROT_PIXEL_COLOUR		RGB(0,0,0)


#define DATA_ARCHIVE_ID			"Mandelbrot Data Archive"
#define ARCHIVE_VERSION			1



typedef int (CALLBACK *LPPROGRESSCALLBACKFUNC)(int, long *);





/*
class CColourTable //: public CObject
{
public:

	CColourTable();
	virtual ~CColourTable();

	//void		CreateDefaultColourTable(long lMaxIterations);
	void		CreateColourTable(long lMaxIterations);

	COLORREF	GetColourFromIndex(long lIndex);
	COLORREF	GetColourFromIterations(long lIterations);
	void		SetColour(long lIndex, COLORREF rgbColour,
						  long lMinIteration, long lMaxIteration);
	
	long		GetNumberOfColours();
	long		GetMaxIterations();
	int			GetRedValue(long lIndex);
	int			GetGreenValue(long lIndex);
	int			GetBlueValue(long lIndex);

	//void		Serialize(CArchive &ar);


private:
	BOOL		AllocateMemory();
	void		DeleteMemory();


	long		m_lNumberOfColours;
	long		m_lMaxIterations;
	long		*m_pMinIteration, *m_pMaxIteration;
	COLORREF	*m_pRGB;
	COLORREF	m_MandelbrotPixelRGB;
};
*/


class CMandelbrot : public CObject
{
public:
	CMandelbrot();
	virtual ~CMandelbrot();
	virtual void Serialize(CArchive& ar); 
	

	long	GetArraySize();
	long	GetMaxIterations();
	double	GetRealCorner();
	double	GetImaginaryCorner();
	double	GetSideLength();

	BOOL	SetArraySize(long lArraySize);
	BOOL	SetMaxIterations(long lMaxIterations);
	BOOL	SetRealCorner(double dRealCorner);
	BOOL	SetImaginaryCorner(double dImagCorner);
	BOOL	SetSideLength(double dSide);

	void	Calculate(LPPROGRESSCALLBACKFUNC = NULL);

	void	SaveData(LPCTSTR lpszFilename);
	void	LoadData(LPCTSTR lpszFilename);
	
	static UINT CalculateThread(LPVOID lpData);
	
	BOOL		m_bCalculate;
	LPPROGRESSCALLBACKFUNC	m_lpProgressFunc;

	long		m_lArraySize;
	long		m_lMaxIterations;
	double		m_dRealCorner, m_dImagCorner;
	double		m_dSide;
	long		*m_pArray;


protected:
	DECLARE_SERIAL(CMandelbrot);


private:
	BOOL	AllocateMemory();
	void	DeleteMemory();
	//void	SerializeData(CArchive &ar);

};
