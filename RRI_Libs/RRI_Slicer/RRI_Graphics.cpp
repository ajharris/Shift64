//#include "stdafx.h"
#include "RRI_Graphics.h"

RRI_Graphics::RRI_Graphics(void)
:m_hDC(NULL)
,m_bitmap(NULL)
{
}
RRI_Graphics::~RRI_Graphics(void)
{
	if (m_bitmap != NULL)
		DeleteObject( m_bitmap );

	if (m_hDC) ::DeleteDC(m_hDC);
}

long RRI_Graphics::Initialize(long width, long height)
{
	m_width = width;
	m_height = height;
	
	//initialize device context and bitmap
	m_hDC = CreateCompatibleDC(NULL);

	//create points to bitmap info
	BITMAPINFO *bmi = ( BITMAPINFO*) (new unsigned char[sizeof(BITMAPINFOHEADER)]);
	BITMAPINFOHEADER bmpInfoHdr;

	//fill the BITMAPINFOHEADER
	bmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHdr.biWidth = width;
	bmpInfoHdr.biHeight = -height; // negative indicates a top-down DIB.
	bmpInfoHdr.biPlanes = 1;
	bmpInfoHdr.biCompression = BI_RGB;
	bmpInfoHdr.biSizeImage = width * height;
	bmpInfoHdr.biBitCount = 24;
	bmpInfoHdr.biClrUsed = 0;
	bmpInfoHdr.biClrImportant = 0;

	//set bmi header
	bmi->bmiHeader = bmpInfoHdr;

	m_bitmap = CreateDIBSection(m_hDC, bmi, DIB_RGB_COLORS, (void **)&m_buffer, NULL, 0);
	delete []bmi;
	
	if (m_hDC)
	{
		SelectObject(m_hDC, m_bitmap);
		SelectObject(m_hDC, GetStockObject(HOLLOW_BRUSH));
	}

	////////////////////////////////////////
	// Set Grey Ramp
	///////////////////////////////////////

	//initialize greyscale palette
	for(int i = 0; i < 256; i++)
	{
		m_CLUT[i].r = i;
		m_CLUT[i].g = i;
		m_CLUT[i].b = i;

		m_GrayPalette[i].r = i;
		m_GrayPalette[i].g = i;
		m_GrayPalette[i].b = i;
	}

	return 0;
}

long RRI_Graphics::SetBitmapBits(unsigned char* buff, long width, long height, long bytesPerPixel)
{
	if (bytesPerPixel == 1)
	{

		long index = 0; 
		long ind = 0; 
		unsigned char r, g, b;

		for(long i = 0; i < width * height; i++)
		{
			r = buff[ind];
			g = buff[ind];
			b = buff[ind++];
			
			m_buffer[index++] = m_GrayPalette[r].r;
			m_buffer[index++] = m_GrayPalette[g].g;
			m_buffer[index++] = m_GrayPalette[b].b;
		}

	}

	else
	if (bytesPerPixel == 3)
	{

		//copy 8-bit greyscale buffer into bitmap bits
		long index = 0; 
		long ind = 0; 
		unsigned char r, g, b;

		for(long i = 0; i < width * height; i++)
		{
			r = buff[ind++];
			g = buff[ind++];
			b = buff[ind++];

	    
			if((r == g) && (r == b) && (g == b))
			{
				m_buffer[index++] = m_GrayPalette[r].r;
				m_buffer[index++] = m_GrayPalette[g].g;
				m_buffer[index++] = m_GrayPalette[b].b;
			}
			else
	    
			{
				m_buffer[index++] = r;
				m_buffer[index++] = g;
				m_buffer[index++] = b;
			}

		}
	}

	return 0;
}

unsigned char* RRI_Graphics::GetBitmapBits()
{
	return m_buffer;
}


void RRI_Graphics::MoveTo(POINT point)
{
	MoveToEx(m_hDC, point.x, point.y, NULL);
}

void RRI_Graphics::DrawLine(POINT point, long thickness, COLORREF colour)
{
	HPEN colourPen = CreatePen(PS_SOLID, thickness, colour);

	LineTo(m_hDC, point.x, point.y);

    DeleteObject(colourPen);
}

void RRI_Graphics::MeasureRubberBand(long measureMode, long areaMode, PointVec mPoly, POINT thisPt)
{
			
    HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));

	HPEN oldPen = (HPEN)SelectObject(m_hDC, greenPen);

	MoveToEx(m_hDC, mPoly.front().x, mPoly.front().y, NULL);

	for (PointVec::iterator i = mPoly.begin()+1; i != mPoly.end(); i++)
	{
		LineTo(m_hDC, i->x, i->y);
	}

	LineTo(m_hDC, thisPt.x, thisPt.y); 

	SelectObject(m_hDC, oldPen);
    DeleteObject(greenPen);
}

void RRI_Graphics::DrawRuler(POINT a, POINT b, float length_mm)
{

    HPEN bluePen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(m_hDC, bluePen);

	float length = float(sqrt(double(((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y)))));
	float pixel_per_mm = length/length_mm;
	float gradient = (b.y - a.y)/float(b.x - a.x); // gradient
	float cosTheta = (b.x - a.x)/length;
	float sinTheta = (b.y - a.y)/length;

	// ruler line
	MoveToEx(m_hDC, a.x, a.y, NULL);
	LineTo(m_hDC, b.x, b.y);

	// Ticks
	const long MM_TICK_SIZE = 4;
	const long CM_TICK_SIZE = 8;
	const long TEXT_OFFSET = 20;
	for (long i=0; i < length_mm; i++)
	{
		long x1 = a.x + pixel_per_mm*i*cosTheta;
		long y1 = a.y + pixel_per_mm*i*sinTheta;

		long x2 = x1 - MM_TICK_SIZE*sinTheta;
		long y2 = y1 + MM_TICK_SIZE*cosTheta;

		if (i%10 == 0) // cm marker
		{
			long x3 = x1 + CM_TICK_SIZE*sinTheta;
			long y3 = y1 - CM_TICK_SIZE*cosTheta;
			MoveToEx(m_hDC, x1, y1, NULL); 
			LineTo(m_hDC, x3, y3);
		}
		
		MoveToEx(m_hDC, x1, y1, NULL);
		LineTo(m_hDC, x2, y2);

		x1 = b.x + TEXT_OFFSET*cosTheta;
		y1 = b.y + TEXT_OFFSET*sinTheta;

		// Write the number
		char buf[256];
		memset(buf, 0, 256);
		sprintf(buf, "%.3f mm", length_mm); 
		DrawText((LPCTSTR)buf, RGB(255, 0, 0), 10, x1, y1, 100, 50);
		DrawText((LPCTSTR)buf, RGB(200,200,200), 10, m_width-60, m_height-24, 100, 50);
	}

	SelectObject(m_hDC, oldPen);
    DeleteObject(bluePen);
}

void RRI_Graphics::DrawWireFrame(const viewORTHO &view, long hiFace)
{
	HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

	HPEN oldPen = (HPEN)SelectObject(m_hDC, greenPen);

	m_hiLiteFace = hiFace;

    int size = view.face.size();
	for (long i = 0; i < view.face.size(); ++i)
	{
		//if (i != hiFace)
            drawFace(view.face[i]);
	}

	if (m_hiLiteFace >= 0)
	{
//		SelectObject(m_hDC, redPen);

//		drawFace(view.face[m_hiLiteFace]);

	}

	SelectObject(m_hDC, oldPen);

	DeleteObject(greenPen);
	DeleteObject(redPen);
}

//private members
void RRI_Graphics::drawFace(Face face)
{

	long n = face.vert.size();

	if (n < 3 || face.plane.normal.z > 0)
	{
			return;
	}


	drawLine(face.vert[n-1], face.vert[0]);



	for (long j = 0; j < n - 1; ++j)
	{
		drawLine(face.vert[j], face.vert[j+1]);
	}

}

void RRI_Graphics::drawLine(Vector3 v1, Vector3 v2)
{

	MoveToEx(m_hDC, v1.x, v1.y, NULL);
	LineTo(m_hDC, v2.x, v2.y);
}

void RRI_Graphics::drawLine(long x1, long y1, long x2, long y2)
{

	MoveToEx(m_hDC, x1, y1, NULL);
	LineTo(m_hDC, x2, y2);
}

void RRI_Graphics::DrawRectangle(long x1, long y1, long x2, long y2, COLORREF colour)
{

	HPEN pen = CreatePen(PS_SOLID, 2, colour);
	
	HPEN oldPen = (HPEN)SelectObject(m_hDC, pen);

	Rectangle(m_hDC, x1, y1, x2, y2);

	SelectObject(m_hDC, oldPen);

	DeleteObject(pen);
}

void RRI_Graphics::DrawCircle(long x1, long y1, long x2, long y2, COLORREF colour)
{

    HPEN pen = CreatePen(PS_SOLID, 2, colour);

    HPEN oldPen = (HPEN)SelectObject(m_hDC, pen);

    Ellipse(m_hDC, x1, y1, x2, y2);

	SelectObject(m_hDC, oldPen);
    DeleteObject(pen);
}


void RRI_Graphics::DrawSurfaceContours(const PlotVec &plot, COLORREF colour)
{
	long pwidth;

    pwidth = 1;

	for ( PlotVec::const_iterator i=plot.begin(); i != plot.end(); i++)
	{
		
		HPEN surfacePen = CreatePen(PS_SOLID, 1, colour);
		HPEN oldPen = (HPEN)SelectObject(m_hDC, surfacePen);

		if (i->plotCode == MOVE_TO) 
		{
			setCurrentPt(i->x, i->y);		
		}

		else if (i->plotCode == LINE_TO)
		{
			DrawLineFromCurrent(i->x, i->y);
		}

		SelectObject(m_hDC, oldPen);
		DeleteObject(surfacePen);
	}

}

void RRI_Graphics::setCurrentPt(long x, long y)
{

	currentPt[0] = x;
	currentPt[1] = y;

}



void RRI_Graphics::DrawLineFromCurrent(long x, long y)
{

	DrawLine(currentPt[0], currentPt[1], x, y);

    //for thicker line
    DrawLine(currentPt[0], currentPt[1]-1, x, y-1);
    DrawLine(currentPt[0]-1, currentPt[1], x-1, y);

}

void RRI_Graphics::DrawLine(long x1, long y1, long x2, long y2)
{

	MoveToEx(m_hDC,x1, y1,NULL);
	LineTo(m_hDC,x2, y2);
}

void RRI_Graphics::DrawText(LPCTSTR lpchText, COLORREF colour, long length, long posX, long posY, long width, long height)
{

	SetTextColor(m_hDC, colour);
	SetBkColor(m_hDC, RGB(0,0,0));

	RECT lpRect;
	lpRect.left = posX;
	lpRect.top = posY;
	lpRect.right = posX + width;
	lpRect.bottom = posY + height;

	::DrawText(m_hDC, lpchText, length, &lpRect, DT_LEFT);

}

void RRI_Graphics::DrawText(LPCTSTR lpchText, long length, long posX, long posY, long width, long height)
{

	SetTextColor(m_hDC, RGB(100,100,100));
	SetBkColor(m_hDC, RGB(0,0,0));

	RECT lpRect;
	lpRect.left = posX;
	lpRect.top = posY;
	lpRect.right = posX + width;
	lpRect.bottom = posY + height;

	::DrawText(m_hDC, lpchText, length, &lpRect, DT_LEFT);

}

void RRI_Graphics::SetWindowLevel(long win, long level)
{

	PALETTEENTRY pe[VIS_GREYS];


	double scale  = VIS_GREYS / 256.0;
	long lutStart = long(scale * (level - win/2.0) + 0.5);
	long lutEnd   = long(scale * (level + win/2.0) + 0.5);
	double lutInc;


	lutStart = (lutStart < 0) ? 0 : lutStart;
	lutEnd   = (lutEnd > VIS_GREYS) ? VIS_GREYS:lutEnd;
	if (lutEnd - lutStart)
	{
		lutInc   = (VIS_GREYS - 1.0) / (lutEnd - lutStart);
	}
	else
	{
		lutInc = VIS_GREYS - 1.0;

		if (lutStart == 0)
			lutEnd = 1;
		else if (lutEnd == VIS_GREYS)	// not currently needed, just here for completeness
			lutStart = VIS_GREYS - 1;
		else
			lutEnd = lutStart + 1;
	}

	long i;
	double val = VIS_MIN_GREY;
	for (i=0; i<lutStart; i++) 
	{
		m_GrayPalette[i].r = pe[i].peRed   = m_CLUT[long(val)].r;
		m_GrayPalette[i].g = pe[i].peGreen = m_CLUT[long(val)].g;
		m_GrayPalette[i].b = pe[i].peBlue  = m_CLUT[long(val)].b;
		pe[i].peFlags = PC_NOCOLLAPSE|PC_RESERVED;			
	}

	for (; i<lutEnd; i++, val += lutInc)
	{
		m_GrayPalette[i].r = pe[i].peRed   = m_CLUT[long(val)].r;
		m_GrayPalette[i].g = pe[i].peGreen = m_CLUT[long(val)].g;
		m_GrayPalette[i].b = pe[i].peBlue  = m_CLUT[long(val)].b;
		pe[i].peFlags = PC_NOCOLLAPSE|PC_RESERVED;
	}


	for (; i<VIS_GREYS; i++) 
	{
		m_GrayPalette[i].r = pe[i].peRed   = m_CLUT[long(val)].r;
		m_GrayPalette[i].g = pe[i].peGreen = m_CLUT[long(val)].g;
		m_GrayPalette[i].b = pe[i].peBlue  = m_CLUT[long(val)].b;
		pe[i].peFlags = PC_NOCOLLAPSE|PC_RESERVED;
	}
		
	RGBQUAD rgbq[256];
		
	//GetDIBColorTable(m_memDC->m_hDC, 0, 256, (LPRGBQUAD)rgbq );
	GetDIBColorTable(m_hDC, 0, 256, (LPRGBQUAD)rgbq );
		

	// if the display modes are not palette_based, we simulate
	// the palette animation by "animating" a device-independent bitmap's
	// (DIBs) color table and redisplaying the DIB with the new colors.
			
	// copy color table from the pe
	for ( i = 0; i < VIS_GREYS; i++ )
	{
		rgbq[i+VIS_MIN_GREY].rgbBlue = pe[i].peBlue;
		rgbq[i+VIS_MIN_GREY].rgbGreen = pe[i].peGreen;
		rgbq[i+VIS_MIN_GREY].rgbRed = pe[i].peRed;
		rgbq[i+VIS_MIN_GREY].rgbReserved = 0;
	}
			
	// Set color table to new values
	int ret = SetDIBColorTable(m_hDC, 0, 256, (LPRGBQUAD)rgbq);
	
}

//note: the needle parameter is not currently used.
void RRI_Graphics::DrawMeasurements(const PlotVec &plot)
 {

	HPEN yellowPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
	

	HPEN oldPen = (HPEN)SelectObject(m_hDC, yellowPen);
	SetTextColor(m_hDC, RGB(100,100,100));
	SetBkColor(m_hDC, RGB(0,0,0));
	SetBkMode(m_hDC, TRANSPARENT);


	long previd = -1;
	long colorID;
	for ( PlotVec::const_iterator i=plot.begin(); i != plot.end(); i++)
	{
		if (i->plotCode == MOVE_TO) 
		{
			setCurrentPt(i->x, i->y);
		}
		else if (i->plotCode == LINE_TO)
		{
			DrawLineFromCurrent(i->x, i->y);
		}
	} 

	SelectObject(m_hDC, oldPen);
	DeleteObject(yellowPen);

}