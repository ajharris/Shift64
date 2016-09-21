// VGAFrameGrabberWindow.cpp : implementation file
//

//#include "stdafx.h"
#include "VGAFrameGrabberWindow.h"
#include "frmgrab.h"

/////////////////////////////////////////////////////////////////////////////
// VGAFrameGrabberWindow dialog

using namespace std;


VGAFrameGrabber::VGAFrameGrabber()
	:m_ROIBuffer(0)
	,m_frameGrabber(0)
    ,m_ROILeft(100)
    ,m_ROITop(100)
    ,m_ROIWidth(300)
    ,m_ROIHeight(300)
    
	,m_videoBuffer(0)
	,m_buffer8(0)
    ,m_videoWidth(640)
    ,m_videoHeight(480)
    ,m_videoLeft(0)
    ,m_videoTop(0)
	,m_verticalShift(0)
	,m_ROIEnabled(true)
	,m_isScanning(false)
	,m_isColourMode(false)
	,m_showROI(true)
	,m_showNeedle(false)
    ,m_showProbeTip(false)
    ,m_showPoints(true)
	,m_isShiftingROI(false) //flag indicating if user is shifting ROI
    ,m_isSizingROI(false)   //flag indicating if user is resizing ROI
    ,m_isCloseToROILeft(false)
    ,m_isCloseToROIRight(false)
    ,m_isCloseToROITop(false)
    ,m_isCloseToROIBottom(false)
	,m_divisibleBy4(false)
    ,m_constrainROI(false)
	,m_frameDelay_ms(33)
	,m_hDC(NULL)
	,m_bitmap(NULL)
	,m_needleOffsetY(0)
    ,m_frameCount(0)

#ifdef USE_TIME_STAMPS
    , m_millisecondsFromStartOfScan(0)
#endif
{

	//{{AFX_DATA_INIT(VGAFrameGrabberWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    

    m_pointList.clear();

    m_roiBrush = CreateSolidBrush(RGB(0,255,0));
    SelectObject(m_hDC, m_roiBrush);

	
}



VGAFrameGrabber::~VGAFrameGrabber()
{

    FrmGrab_Stop(m_frameGrabber);
    m_pointList.clear();

	if (m_bitmap != NULL)
		DeleteObject( m_bitmap );

    DeleteObject(m_roiBrush);

	if (m_hDC) ::DeleteDC(m_hDC);

	if (m_buffer8) GlobalFree(m_buffer8);
	if (m_videoBuffer) GlobalFree(m_videoBuffer);
	if (m_ROIBuffer) GlobalFree(m_ROIBuffer);
    
#ifdef USE_TIME_STAMPS
    ClearImageSet();//delete ROI buffers from time stamp sequence
#endif

}

#ifdef USE_TIME_STAMPS
void VGAFrameGrabber::ClearImageSet()
{
    long size = m_imageList.size();

    //delete all the image buffers
    for (int i = 0; i<size; i++)
    {
        delete m_imageList.at(i).imagePointer;//GlobalFree((unsigned char*)m_imageList.at(i).imagePointer);//
    }

    m_imageList.clear();
    
}

void VGAFrameGrabber::AddImageSet(unsigned char* buffer)
{

    double diff = EndPerformanceCounterGetDiff();

    m_millisecondsFromStartOfScan += diff;

#ifdef USE_GETTICKCOUNT
//get time stamp for this image
    SYSTEMTIME st;
    GetSystemTime(&st);
    long milliseconds = (long)GetTickCount();
#endif

    TimeStampImage tsImage;
    tsImage.milliSecond = m_millisecondsFromStartOfScan;
    tsImage.imagePointer = buffer;
    m_imageList.push_back(tsImage);

    StartPerformanceCounter();//for next time
}

void VGAFrameGrabber::StartPerformanceCounter()
{
    QueryPerformanceCounter(&counterStart);
   

}

double VGAFrameGrabber::EndPerformanceCounterGetDiff()
{
    // Get second snapshot
    QueryPerformanceCounter(&counterEnd);

    //calculate difference (in ms)
    double diff = double(counterEnd.QuadPart - counterStart.QuadPart) / double(freq.QuadPart) * 1000;//convert to milliseconds
    return diff;

}

#endif

long VGAFrameGrabber::InitializeVideo(long videoWidth, long videoHeight, long videoLeft, long videoTop)
{

#ifdef USE_TIME_STAMPS
QueryPerformanceFrequency(&freq);
#endif

	m_videoWidth = videoWidth;
    m_videoHeight = videoHeight;
    m_videoLeft = videoLeft;
    m_videoTop = videoTop;

	//create 8 bit buffer
	if (m_buffer8) GlobalFree(m_buffer8);
    m_buffer8 = (unsigned char*)GlobalAlloc(GMEM_FIXED, videoWidth*videoHeight);
	initHDC(m_videoWidth, m_videoHeight);//create device context for painting

    

    return Connect();
}

long VGAFrameGrabber::initHDC(long width, long height)
{
	//initialize device context and bitmap
	m_hDC = CreateCompatibleDC(NULL);

	//create points to bitmap info
	BITMAPINFO *bmi = ( BITMAPINFO*) (new unsigned char[sizeof(BITMAPINFOHEADER)]);
	BITMAPINFOHEADER bmpInfoHdr;

	//fill the BITMAPINFOHEADER
	bmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHdr.biWidth = m_videoWidth;
	bmpInfoHdr.biHeight = -m_videoHeight; // negative indicates a top-down DIB.
	bmpInfoHdr.biPlanes = 1;
	bmpInfoHdr.biCompression = BI_RGB;
	bmpInfoHdr.biSizeImage = m_videoWidth * m_videoHeight;
	bmpInfoHdr.biBitCount = 24;
	bmpInfoHdr.biClrUsed = 0;
	bmpInfoHdr.biClrImportant = 0;

	//set bmi header
	bmi->bmiHeader = bmpInfoHdr;

	m_bitmap = CreateDIBSection(m_hDC, bmi, DIB_RGB_COLORS, (void **)&m_videoBuffer, NULL, 0);
	delete []bmi;
	
	if (m_hDC)
	{
		SelectObject(m_hDC, m_bitmap);
		SelectObject(m_hDC, GetStockObject(HOLLOW_BRUSH));
	}

	return 0;

}

void VGAFrameGrabber::InitializeROI(long ROIWidth, long ROIHeight, long ROILeft, long ROITop)
{
	
	m_ROIWidth = ROIWidth;
    m_ROIHeight = ROIHeight;
    m_ROILeft = ROILeft;
    m_ROITop = ROITop;

	if (m_ROIBuffer) GlobalFree(m_ROIBuffer);

    m_ROIBuffer = (unsigned char*)GlobalAlloc(GMEM_FIXED, ROIWidth*ROIHeight);

}

bool VGAFrameGrabber::IsLeftRightMarkerHere(long x, long y)
{
	//get pixel value from buffer
	long index = y * m_videoWidth + x;

	unsigned char value = m_videoBuffer[index*3];

	if (value > 100)
	{
		return true;
	}
	else
	{
		return false;
	}

}

long VGAFrameGrabber::Connect()
{
	long count = FrmGrabLocal_Count();

	if (count > 0)
	{
		m_frameGrabber = FrmGrabLocal_Open();

		if (!m_frameGrabber)
		{
			//AfxMessageBox("Error connecting to frame grabber");
			return -1;
		}

		const char* sn = FrmGrab_GetSN(m_frameGrabber);

		V2U_VideoMode viewMode;
		FrmGrab_DetectVideoMode(m_frameGrabber, &viewMode);

		if (viewMode.width == 0 || viewMode.height == 0)
		{
			return -2;//live video not available
		}
	}
	else
	{
		return -1;//framegrabber not available
	}

    FrmGrab_SetMaxFps(m_frameGrabber, 30);
    FrmGrab_Start(m_frameGrabber);//NOTE: this doesn't seem to make much difference in frame rate

    return 0;

}

void VGAFrameGrabber::SetNeedle(long needleOffsetY)
{
	m_needleOffsetY = needleOffsetY;
}

void VGAFrameGrabber::SetProbeTipLocation(long probeTipX, long probeTipY)
{
	m_probeTip.x = probeTipX;
    m_probeTip.y = probeTipY;
}


void VGAFrameGrabber::UpdateDisplay()
{
	//draw ROI
	if (m_showROI)
    {
		RECT rcBmp;

		SetRect(&rcBmp, m_ROILeft, m_ROITop, m_ROILeft + m_ROIWidth, m_ROITop + m_ROIHeight); 
        FrameRect(m_hDC, &rcBmp, m_roiBrush);
		SetRect(&rcBmp, m_ROILeft-1, m_ROITop-1, m_ROILeft + m_ROIWidth+1, m_ROITop + m_ROIHeight+1); 
        FrameRect(m_hDC, &rcBmp, m_roiBrush);


	}

	if (m_showNeedle)
	{
	
		Vector3 needleEntry, needleExit;
		needleEntry = Vector3(m_ROILeft,m_ROITop + m_needleOffsetY,0);
		needleExit = Vector3(m_ROILeft+m_ROIWidth, m_ROITop + m_needleOffsetY,0);

		HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));

		SelectObject(m_hDC, greenPen);

		//draw upper line of needle
		MoveToEx(m_hDC, needleEntry.x, needleEntry.y-5, NULL);
		LineTo(m_hDC, needleExit.x, needleExit.y-5); 

		MoveToEx(m_hDC, needleEntry.x, needleEntry.y+5, NULL);
		LineTo(m_hDC, needleExit.x, needleExit.y+5); 

		DeleteObject(greenPen);

	}

    if (m_showProbeTip)
	{
	    HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));

		SelectObject(m_hDC, greenPen);

		//draw upper line of needle
		
		Ellipse(m_hDC, m_probeTip.x-5, m_probeTip.y-5, m_probeTip.x+5, m_probeTip.y+5);


		DeleteObject(greenPen);
		

	}

    if (m_showPoints)
    {
        HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
		SelectObject(m_hDC, greenPen);

        for (int i = 0; i < m_pointList.size(); i++)
        {
            Vector3 point = m_pointList.at(i);
            Ellipse(m_hDC, point.x-5, point.y-5, point.x+5, point.y+5);
        }

        DeleteObject(greenPen);
    }

}

void VGAFrameGrabber::AddPoint(Vector3 point)
{
    m_pointList.push_back(point);

}

void VGAFrameGrabber::ResetFrameCount()
{

    m_frameCount = 0;
}

long VGAFrameGrabber::GrabFrame()
{
	V2U_UINT32 format = V2U_GRABFRAME_FORMAT_BGR24;
   // V2U_UINT32 format = V2U_GRABFRAME_FORMAT_RGB8;
    V2U_GrabFrame2* frame = NULL;
        
    V2URect rect;
    rect.x = m_videoLeft;
    rect.y = m_videoTop;
    rect.width = m_videoWidth;
    rect.height = m_videoHeight;



    frame = FrmGrab_Frame(m_frameGrabber, format, &rect);



	if (frame)
    {


        unsigned char* tempBuffer;
        tempBuffer = (unsigned char*)frame->pixbuf;



        //copy into live video buffer
        long index = 0;
        for (int i=0; i<m_videoWidth*m_videoHeight; i++)
        {
            m_buffer8[i] = tempBuffer[index];
            m_videoBuffer[index+0] = tempBuffer[index+0];
            m_videoBuffer[index+1] = tempBuffer[index+1];
            m_videoBuffer[index+2] = tempBuffer[index+2];
            index += 3;
        }


		if (m_isScanning)
		{



			for (int y=0; y<m_ROIHeight; y++)
			{
				long newROITop = m_ROITop + m_verticalShift;

                if (m_isColourMode)
                {
                    //copy colour buffer
                    unsigned char* source = (unsigned char*)m_videoBuffer + (y+newROITop)*m_videoWidth*3 + m_ROILeft*3;
				    unsigned char* destination = (unsigned char*)m_ROIBuffer + y*m_ROIWidth*3;
				    memcpy(destination,source,m_ROIWidth*3);
                }
                else
                {
                   //copy greyscale buffer
				    unsigned char* source = (unsigned char*)m_buffer8 + (y+newROITop)*m_videoWidth + m_ROILeft;
				    unsigned char* destination = (unsigned char*)m_ROIBuffer + y*m_ROIWidth;
				    memcpy(destination,source,m_ROIWidth);
                }
			}//for (int y=0; y<m_ROIHeight; y++)

#ifdef USE_TIME_STAMPS
            unsigned char* timeStampBuffer = new unsigned char [m_ROIWidth*m_ROIHeight];//(unsigned char*)GlobalAlloc(GMEM_FIXED, m_ROIWidth*m_ROIHeight);
            //memset(timeStampBuffer, 200, m_ROIWidth*m_ROIHeight);
            memcpy(timeStampBuffer, m_ROIBuffer, m_ROIWidth * m_ROIHeight);
            AddImageSet(timeStampBuffer);
#endif


#ifndef USE_TIME_STAMPS
			//setup event then send to client
			RRIEvent event;
			event.SetControlObject(m_ROIBuffer);
			event.SetWidth(m_ROIWidth);
			event.SetHeight(m_ROIHeight);
			if (m_isColourMode)
			{
				event.SetChannels(3);
			}
			else
			{
				event.SetChannels(1);
			}


			event.SetEventCode(FRAME_COMPLETE);
			Notify(event);//notify client of event		
#endif

			//Sleep(66);

		}//if (m_isScanning)

       FrmGrab_Release(m_frameGrabber, frame);
       m_frameCount++;

    }


	return 0;

}

long VGAFrameGrabber::StartScan()
{
    

#ifdef USE_TIME_STAMPS
    ClearImageSet();
#endif

    //allocate ROI buffer
    if (m_ROIBuffer) GlobalFree(m_ROIBuffer);

    if (!m_isColourMode)
    {
        m_ROIBuffer = (unsigned char*)GlobalAlloc(GMEM_FIXED, m_ROIWidth*m_ROIHeight);//greyscale
    }
    else
    {
        m_ROIBuffer = (unsigned char*)GlobalAlloc(GMEM_FIXED, m_ROIWidth*m_ROIHeight*3);//colour
    }

    m_isScanning = true;

#ifdef USE_TIME_STAMPS
    StartPerformanceCounter();
    m_millisecondsFromStartOfScan = 0;
#endif
    
    return 0;
}

long VGAFrameGrabber::StopScan()
{
    m_isScanning = false;
    return 0;
}

void VGAFrameGrabber::OnMouseDown(UINT nFlags, long x, long y)
{
	 if (!m_ROIEnabled) return;

	//keep track of current mouse position
    m_oldLeftPosition = x;
    m_oldTopPosition = y;

    int leftPosition = m_ROILeft;
    int topPosition  = m_ROITop;

    int rightPosition = leftPosition + m_ROIWidth;
    int bottomPosition = topPosition + m_ROIHeight;

    if (nFlags & MK_SHIFT)
    {
        AddPoint(Vector3(x, y, 0));
    }
    else
	//These flags are set in the FGOnMouseMove() function.
    if ((m_isCloseToROILeft || m_isCloseToROIRight || m_isCloseToROITop || m_isCloseToROIBottom))
    {
        m_isSizingROI = true;
    }
    else
	{
		//check to see if the cursor is inside the ROI box
		RECT ROIRect;
		ROIRect.left = leftPosition;
		ROIRect.top = topPosition;
		ROIRect.right = rightPosition;
		ROIRect.bottom = bottomPosition;
		POINT point;
		point.x = x;
		point.y = y;
		if (PtInRect(&ROIRect, point))
		{
			m_isShiftingROI = true;
		}
	}

}

void VGAFrameGrabber::OnMouseUp(UINT nFlags, long x, long y) 
{
    ::ReleaseCapture();//release mouse capture

    if (!m_ROIEnabled) return;

    //if the ROI is visible, then we can size and move the ROI.

    m_mouseUpX = x;
    m_mouseUpY = y;

	long min = 20;
    if (m_isSizingROI) 
    {

        //note: sometimes we need the width of the ROI to be a multiple of 4
        if (m_divisibleBy4)
            m_ROIWidth &= 0xfffffffc;

        if (m_ROIWidth < min) m_ROIWidth = min;
        if (m_ROIHeight < min) m_ROIHeight = min; 
        if (m_ROILeft < 4) m_ROILeft = 4;
        if (m_ROITop < 4) m_ROITop = 4;
    }


    m_isShiftingROI = false;
    m_isSizingROI = false;

    if (m_ROILeft < 1) m_ROILeft = 1;
    if (m_ROITop < 1) m_ROITop = 1;


	//are we constraining center of ROI
	if (m_constrainROI)
	{
		if ( abs((m_centerOfROI-m_ROILeft) - ((m_ROILeft+m_ROIWidth)-m_centerOfROI)) > 1)
		{
			m_ROILeft = m_centerOfROI - m_ROIWidth/2;
		}
	}
}

void VGAFrameGrabber::OnMouseMove(UINT nFlags, long x, long y) 
{
    if (!m_ROIEnabled) return;

	m_currentMouseMove.x = x;
    m_currentMouseMove.y = y;


    UpdateROI(x,y);   

}

//--------------------------------------------------------------------------
//This method modifies the position and/or size of ROI box depending on
//mouse conditions.
//Called from FGOnMouseMove()
//--------------------------------------------------------------------------
void VGAFrameGrabber::UpdateROI(long x, long y) 
{
    //check to see if we are close to the left edge of the ROI

	//cache these values
    int leftPosition = m_ROILeft;
    int topPosition  = m_ROITop;

    int rightPosition = leftPosition + m_ROIWidth;
    int bottomPosition = topPosition + m_ROIHeight;


    int dist = 5;//distance from cursor to ROI (for detection purposes)


    //get new position, calculate difference from old position (for tracking mouse moves)
	int xdiff = m_oldLeftPosition - x;
    int ydiff = m_oldTopPosition - y;


	//Here, we are resizing the ROI box
    if (m_isSizingROI)
    {
		//check to see which edge we are close to
        if (m_isCloseToROILeft && y > topPosition && y < topPosition+bottomPosition)
        {
            m_ROILeft -= xdiff;
            m_ROIWidth += xdiff;

			if (m_constrainROI)//NOTE: ROI is constrained only in left/right direction
			{
				m_ROIWidth += xdiff;
			}
           
        }
        else
        if (m_isCloseToROIRight && y > topPosition && y < topPosition+bottomPosition)
        {
            m_ROIWidth -= xdiff;

			if (m_constrainROI)//NOTE: ROI is constrained only in left/right direction
			{
				m_ROILeft += xdiff;
				m_ROIWidth -= xdiff;
			}
        }
        else
        if (m_isCloseToROITop && x > leftPosition && x < leftPosition+rightPosition)
        {
            m_ROITop -= ydiff;
            m_ROIHeight += ydiff;

        }
        else
        if (m_isCloseToROIBottom && x > leftPosition && x < leftPosition+rightPosition)
        {
            m_ROIHeight -= ydiff;
        }



		//make sure the ROI does not move off the window and is > minimum size
		int min = 20;


		//validate size of ROI
		if (m_ROIWidth > m_videoWidth) 
			m_ROIWidth = m_videoWidth;

		if (m_ROIHeight > m_videoHeight) 
			m_ROIHeight = m_videoHeight;


        if (m_ROIWidth < min) m_ROIWidth = min;
        if (m_ROIHeight < min) m_ROIHeight = min;


		//validate positin of ROI
		if (m_ROILeft+m_ROIWidth > m_videoWidth)
			m_ROILeft = m_videoWidth - m_ROIWidth;

		if (m_ROITop+m_ROIHeight > m_videoHeight)
			m_ROITop = m_videoHeight - m_ROIHeight;

		if (m_ROILeft < 1) m_ROILeft = 1;
		if (m_ROITop < 1) m_ROITop = 1;
		


    }



    else


    //Shift ROI: base on previous and current mouse movements
	if (m_isShiftingROI)
	{
        //setup new offset position
		m_ROILeft -= xdiff;
		m_ROITop -= ydiff;

        if (m_ROILeft < 1) m_ROILeft = 1;
        if (m_ROITop < 1) m_ROITop = 1;
        //update frame grabber ROI



		if (m_constrainROI)
		{
			if ( abs((m_centerOfROI-m_ROILeft) - ((m_ROILeft+m_ROIWidth)-m_centerOfROI)) > 1)
			{
				m_ROILeft = m_centerOfROI - m_ROIWidth/2;
			}
		}


		//make sure the ROI does not move off the window and 
		//is > minimum size
		int min = 20;
		int offset = 4;

        

		if (m_ROILeft+m_ROIWidth > (m_videoWidth-offset))
			m_ROILeft = (m_videoLeft-offset) - m_ROIWidth;

		if (m_ROITop+m_ROIHeight > (m_videoHeight-offset))
			m_ROITop = (m_videoHeight-offset) - m_ROIHeight;

		if (m_ROILeft < 1) m_ROILeft = 1;
		if (m_ROITop < 1) m_ROITop = 1;

	 
	}//if PanningROI




    else



    //LEFT: check to see if we are close to the LEFT edge of the ROI
    if ( (abs(x - leftPosition) < dist)
        && y > topPosition && y < topPosition+bottomPosition)//within "dist" pixels of the edge
    {
        m_isCloseToROILeft = true;
       // m_cursorType = CUR_LEFT_RIGHT;
       // ::SetCursor(AfxGetApp()->LoadCursor(IDC_LEFT_RIGHT));

    }
    else



    //RIGHT: check to see if we are close to the RIGHT edge of the ROI
    if ( (abs(x - rightPosition ) < dist)
        && y > topPosition && y < topPosition+bottomPosition)//within "dist" pixels of the edge) 
    {
        m_isCloseToROIRight = true;
       // m_cursorType = CUR_LEFT_RIGHT;
       // ::SetCursor(AfxGetApp()->LoadCursor(IDC_LEFT_RIGHT));
    }
    else


    
    //TOP: check to see if we are close to the TOP edge of the ROI
    if ( (abs(y - topPosition) < dist)
        && x > leftPosition && x < leftPosition+rightPosition)//within "dist" pixels of the edge)
    {
        m_isCloseToROITop = true;
       // m_cursorType = CUR_TOP_BOTTOM;
       // ::SetCursor(AfxGetApp()->LoadCursor(IDC_TOP_BOTTOM));
    }
    else


    
    //BOTTOM: check to see if we are close to the BOTTOM edge of the ROI
    if ( (abs(y - bottomPosition ) < dist)
        && x > leftPosition && x < leftPosition+rightPosition)//within "dist" pixels of the edge)
    {
        m_isCloseToROIBottom = true;
       // m_cursorType = CUR_TOP_BOTTOM;
       // ::SetCursor(AfxGetApp()->LoadCursor(IDC_TOP_BOTTOM));
    }
    else
    //none of the above
    {
		//turn all flags off
        m_isCloseToROILeft = false;
        m_isCloseToROIRight = false;
        m_isCloseToROITop = false;
        m_isCloseToROIBottom = false;

        //change cursor back to normal
       // m_cursorType = CUR_CROSS;
       // ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
    }

    
    //keep track of current position for next time
    m_oldLeftPosition = x;
    m_oldTopPosition = y;
}

// szPathName : Specifies the pathname
// lpBits	 : Specifies the bitmap bits
// width	: Specifies the image width
// height	: Specifies the image height
//channels : number of channels in the bitmap

bool VGAFrameGrabber::SaveImage(char* szPathName, void* lpBits, int width, int height, int channels)
{ 

    //Create a new file for writing
    FILE *pFile = fopen(szPathName, "wb");
    if(pFile == NULL)
    { 
        return false;
    }

    BITMAPINFOHEADER BMIH;
    BMIH.biSize = sizeof(BITMAPINFOHEADER);
    BMIH.biSizeImage = width * height * 3;
    // Create the bitmap for this OpenGL context
    BMIH.biSize = sizeof(BITMAPINFOHEADER);
    BMIH.biWidth = width;
    BMIH.biHeight = -height;
    BMIH.biPlanes = 1;

    if (channels == 1)
    {
        BMIH.biBitCount = 8;
    }
    else
    {
        BMIH.biBitCount = 24;
    }
    
    BMIH.biCompression = BI_RGB;
    BMIH.biSizeImage = width * height* 3; 
    BITMAPFILEHEADER bmfh;
    int nBitsOffset = sizeof(BITMAPFILEHEADER) + BMIH.biSize; 
    LONG lImageSize = BMIH.biSizeImage;
    LONG lFileSize = nBitsOffset + lImageSize;
    bmfh.bfType = 'B'+('M'<<8);
    bmfh.bfOffBits = nBitsOffset;
    bmfh.bfSize = lFileSize;
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

    //Write the bitmap file header
    UINT nWrittenFileHeaderSize = fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);

    //And then the bitmap info header
    UINT nWrittenInfoHeaderSize = fwrite(&BMIH, 1, sizeof(BITMAPINFOHEADER), pFile);

    //Finally, write the image data itself 
    //-- the data represents our drawing
    UINT nWrittenDIBDataSize = fwrite(lpBits, 1, lImageSize, pFile);
    fclose(pFile);

 
    return true;
}