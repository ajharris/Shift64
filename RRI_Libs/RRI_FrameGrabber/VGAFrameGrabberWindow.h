#ifndef VGA_FRAMEGRABBER_H_INCLUDED
#define VGA_FRAMEGRABBER_H_INCLUDED

#pragma once

// VGAFrameGrabberWindow.h : header file
//
#include <windows.h>
#include "frmgrab.h"
#include "Visualizer.h"
#include "RRIObserver.h"
#include "RRIObservable.h"
#include "RRIEvent.h"
#include <ctime>

//#define USE_TIME_STAMPS

#define FRAME_COMPLETE              31001
#define FRAME_TIMEOUT               31002
#define FG_MOUSE_UP                 31003

#define CUR_LEFT_RIGHT 100
#define CUR_TOP_BOTTOM 101
#define CUR_NONE 102
#define CUR_ARROW 103
#define CUR_CROSS 104

#define MARKER_TYPE_PROBE_TIP     229301
#define MARKER_TYPE_LEFT          229302
#define MARKER_TYPE_RIGHT         229303
#define MARKER_TYPE_VIDEO_MASK	  229304
#define MARKER_TYPE_NEEDLE_GUIDE  229305
#define MARKER_TYPE_PATIENT_INFO  229306


#ifdef USE_TIME_STAMPS
struct TimeStampImage {
    unsigned char* imagePointer;
    double milliSecond;
};
typedef std::vector<TimeStampImage> ImageListWithTimeStamp;




#endif

typedef std::vector<Vector3> PointList;




/////////////////////////////////////////////////////////////////////////////
// VGAFrameGrabberWindow dialog

public class VGAFrameGrabber :  public RRIObservable
{
// Construction

public:
	VGAFrameGrabber();
	virtual ~VGAFrameGrabber(void);

	long InitializeVideo(long videoWidth, long videoHeight, long videoLeft, long videoTop);
	void InitializeROI(long ROIWidth, long ROIHeight, long ROILeft, long ROITop);
	long Connect();
	long StartLiveVideo();
	long StopLiveVideo();
	long StartScan();
	long StopScan();
	long GrabFrame();
	bool IsScanning(){return m_isScanning;}
	long GetWidth(){return m_videoWidth;}
	long GetHeight(){return m_videoHeight;}
	long GetROIWidth(){return m_ROIWidth;}
	long GetROIHeight(){return m_ROIHeight;}
	long GetROILeft(){return m_ROILeft;}
	long GetROITop(){return m_ROITop;}
    void SetProbeTip(Vector3 probeTip){ m_probeTip = probeTip; }
    Vector3 GetProbeTip(){ return m_probeTip; }

    void SetROIHeight(long height);
	void UpdateDisplay();

	bool IsLeftRightMarkerHere(long x, long y);
	
	unsigned char* GetVideoBuffer(){return m_videoBuffer;}
	unsigned char* GetROIBuffer(){return m_ROIBuffer;}

	void UpdateROI(long x, long y);
	void OnMouseDown(UINT nFlags, long x, long y);
	void OnMouseUp(UINT nFlags, long x, long y);
	void OnMouseMove(UINT nFlags, long x, long y);

	//current mouse position
    long GetMouseX(){return m_mouseUpX;}//get latest mouse X location after mouse click
	long GetMouseY(){return m_mouseUpY;}//get latest mouse Y location after mouse click
    long m_mouseUpX, m_mouseUpY;
	POINT m_currentMouseMove;

	long GetFrameDelay_ms(){return m_frameDelay_ms;}
	void SetFrameDelay_ms(long delay){m_frameDelay_ms = delay;}
	long m_frameDelay_ms;

	//NOTE: this is used for correcting geometric errors caused by hybrid scanner
	void SetVerticalShift(long shift){m_verticalShift = shift;}

    //points
    void AddPoint(Vector3 point);
    void ShowPoints(bool show){ m_showPoints = show; }
    PointList GetPoints(){ return m_pointList; }
    bool m_showPoints;


	//set location of needle in video coordinates
    void SetProbeTipLocation(long probeTipX, long probeTipY);
    Vector3 m_probeTip;
	void SetNeedle(long needleOffsetY);//{m_needleOffsetY = needleOffsetY;}
	long m_needleOffsetY;

	VGAFrameGrabber* GetFrameGrabber(){return this;}

    //std::string GetCurrentTime();

private:
	long initHDC(long width, long height);//device context to contain video image

	FrmGrabber* m_frameGrabber;

	long m_ROILeft;
	long m_ROITop;
	long m_ROIWidth;
	long m_ROIHeight;
	long m_videoLeft;
	long m_videoTop;
	long m_videoWidth;
	long m_videoHeight;
	long m_verticalShift;//shift the ROI in vertical direction by shift amount

    

#ifdef USE_TIME_STAMPS
    ImageListWithTimeStamp m_imageList;
    ImageListWithTimeStamp m_imageListCopy;

    void StartPerformanceCounter();
    double EndPerformanceCounterGetDiff();
    LARGE_INTEGER freq;
    public: LARGE_INTEGER counterStart;
    public: LARGE_INTEGER counterEnd;
    
   
public:
    void AddImageSet(unsigned char* buffer);
    void ClearImageSet();
    ImageListWithTimeStamp* GetImageList(){return &m_imageList;}
    ImageListWithTimeStamp* GetImageListCopy(){return &m_imageListCopy;}

    double m_millisecondsFromStartOfScan;

    

#endif//#ifdef USE_TIME_STAMPS


public:
    long GetFrameCount(){return m_frameCount;}
    void ResetFrameCount();

bool VGAFrameGrabber::SaveImage(char* szPathName, void* lpBits, int width, int height, int channels);

private:
	//ROI manipulation parameters
	long m_oldLeftPosition, m_oldTopPosition;
    bool m_isSizingROI;
    bool m_isCloseToROILeft, m_isCloseToROIRight, m_isCloseToROITop, m_isCloseToROIBottom;
    bool m_constrainROI;
    bool m_isShiftingROI;
	bool m_ROIEnabled;
	bool m_divisibleBy4;
	long m_centerOfROI;

	unsigned char* m_ROIBuffer;

	bool m_isScanning;
	bool m_isColourMode;
	bool m_showROI;
	bool m_showNeedle;
    bool m_showProbeTip;

    long m_frameCount;//for detecting frame rate


	//device context members
	HDC m_hDC;
	HBITMAP m_bitmap;
	unsigned char* m_videoBuffer;
	unsigned char* m_buffer8;

    PointList m_pointList;

    HBRUSH m_roiBrush; 

protected:
};
#endif // VGA_FRAMEGRABBER_H_INCLUDED
