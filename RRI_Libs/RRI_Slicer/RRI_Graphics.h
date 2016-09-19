#if !defined(RRI_GRAPHICS_H_INCLUDED)
#define RRI_GRAPHICS_H_INCLUDED

#pragma once

#include <windows.h>
#include "VisLib.h"

const enum {POINT_PEN, LINE_PEN, ANGLE_PEN, AREA_PEN, VOLUME_PEN, SELECT_PEN, ORIENT_PEN, WIREFRAME_PEN, ACTIVEFACE_PEN, ANNOTATE_PEN, ACQROI_PEN, CUSTOM1_PEN, CUSTOM2_PEN, BLUE_PEN, PEN_COUNT};
#define LIS_EditBackgroundColour RGB(50,75,120)
#define LIS_TextColour RGB(200,200,200)
#define LIS_BrightGreen RGB(200,255,200)
#define LIS_TransparentColour RGB(255,0,255)
#define LIS_ToolTipBkColour RGB(50,50,100)
#define LIS_Black RGB(0,0,0)
#define LIS_DarkBackgroundColour RGB(0,0,0)

//NewFreeMenu colours
#define    LIS_BackGroundColor	 RGB(27,63,39)
#define    LIS_EdgeColor	     RGB(116,157,79)
#define    LIS_HighlightColor	 RGB(102,140,58)
#define    LIS_FlyoverColor		 RGB(92,130,48)
#define    LIS_MenuTextColor	 RGB(180,200,180)

//Graphic class colours
#define    LIS_DefaultColour RGB(255,255,0)//yellow
#define    LIS_OrientationMarkerColour RGB(255,255,0)
#define    LIS_WireFrameColour RGB(102,153,255)
#define    LIS_ActiveFaceColour RGB(255,0,0)
#define    LIS_AnnotationArrowColour RGB(255,255,0)
#define    LIS_AcquisitionROIColour RGB(200,200,200)
#define    LIS_CustomPen1 RGB(0,200,0)
#define    LIS_CustomPen2 RGB(0,0,0)
#define    LIS_MeasurementIDColour RGB(0,255,0) //green
#define    LIS_MeasurementDetailColour RGB(200,200,200) //unsaturated white
#define    LIS_ScanInfoColour RGB(200,200,200)
#define    LIS_AnnotationTextColour RGB(200,200,200)

//font definitions for Graphics.cpp
#define    LIS_GraphicsFont "Lucida Console"
#define    LIS_SmallFontSize 10
#define    LIS_NormalFontSize 14
#define    LIS_MediumFontSize 20


//font definitions for LifeDialog
//#define    LIS_DialogFont "Lucida Console"
#define    LIS_DialogFont "Arial"
#define    LIS_DialogSmallFontSize 14
#define    LIS_DialogMediumFontSize 22
#define    LIS_DialogLargeFontSize 36

#define    LIS_GUIFont "Arial"
#define    LIS_GUISmallFontSize 12
#define    LIS_GUIMediumFontSize 16
#define    LIS_GUILargeFontSize 20

//Dialog border colour definitions
#define    LIS_OuterBorder RGB(55,74,38)
#define    LIS_MiddleBorder RGB(110,149,75)
#define    LIS_InnerBorder RGB(134,175,97)

//new dialog borders
#define    LIS_OuterBorderBlue RGB(0,30,130)
#define    LIS_MiddleBorderBlue RGB(30,65,140)
#define    LIS_InnerBorderBlue RGB(255, 0, 0)//border around selected view window

//---------------------------------------------------------------------
// palette group
//
const long ACQ_MAX_COLOURS	= 256;                            // total palette size
const long VIS_MIN_GREY     = 1;
const long VIS_MAX_GREY	    = ACQ_MAX_COLOURS - 1;            // 255 index of largest greyscale voxel
const long VIS_GREYS		= ACQ_MAX_COLOURS - VIS_MIN_GREY; // 246	


typedef STL::vector<POINT> PointVec;

class RRI_Graphics
{
public:
	RRI_Graphics(void);
	virtual ~RRI_Graphics(void);
	long Initialize(long width, long height);
	//copy 8-bit greyscale buffer from visualizer into 24-bit color bitmap
	long SetBitmapBits(unsigned char* buff, long width, long height, long bytesPerPixel = 1);
	void DrawWireFrame(const viewORTHO &view, long hiLiteFace);
	void DrawRectangle(long x1, long y1, long x2, long y2, COLORREF colour);
    void DrawCircle(long x1, long y1, long x2, long y2, COLORREF colour);
	void DrawText(LPCTSTR lpchText, long length, long posX, long posY, long width, long height);
	void DrawText(LPCTSTR lpchText, COLORREF colour, long length, long posX, long posY, long width, long height);
	unsigned char* GetBitmapBits();
	void DrawSurfaceContours(const PlotVec &plot, COLORREF colour);//pass in colour
	void DrawMeasurements(const PlotVec &plot);
	void SetWindowLevel(long win, long level);
	void MeasureRubberBand(long measureMode, long areaMode, PointVec mPoly, POINT thisPt);
	void DrawRuler(POINT a, POINT b, float length_mm);
	void MoveTo(POINT point);
	void DrawLine(POINT point, long thickness, COLORREF colour);
	HDC  GetDeviceContext(){return m_hDC;}
private:
	void drawFace(Face face);
	void drawLine(long x1, long y1, long x2, long y2);
	void drawLine(Vector3 v1, Vector3 v2);

	
	void setPixel(long x, long y, long color);
	void setCurrentPt(long x, long y);
	void DrawLineFromCurrent(long x, long y);
	void DrawLine(long x1, long y1, long x2, long y2);
	void DrawLine(Vector3 v1, Vector3 v2);
	
	Trgb m_CLUT[256];

	HDC m_hDC;
	HBITMAP m_bitmap;
	unsigned char* m_buffer;
	long m_hiLiteFace;
	long m_width, m_height;
	long currentPt[2];

	//used to hold greyscale palette
	Trgb m_GrayPalette[256];


};
#endif

