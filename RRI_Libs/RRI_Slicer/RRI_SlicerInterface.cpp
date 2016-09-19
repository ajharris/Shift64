//#include "stdafx.h"
#include "RRI_SlicerInterface.h"

#include "doubledouble.h"
#include <iostream>
#include <sstream>
#include <fstream>


//using namespace gdcm;

RRI_SlicerInterface::RRI_SlicerInterface()
:m_vis(0)
,m_activeFace(0)
,m_slicing(false)
,m_panning(false)
,m_showWireframe(true)
,m_showUserPoints(true)
,m_showSurfaces(true)
,m_showMeasurements(true)
,m_isRendering(false)
,m_isPainting(false)
,m_isErasing(false)
, m_isEditing(false)
,m_paintBrushSize(6.0)//mm
,m_currentMode(MODE_SLICER)
,m_currentState(ST_ROTATE)
,m_oldPositionX(-1)
,m_oldPositionY(-1)
,m_isSelected(false)
,m_targets(0)
,m_surfaces(0)
,m_measurements(0)
,m_displayText(0)
,m_windowLevelText(0)
,m_currentPositionText(0)
,m_displayTextSize(0)
,m_currentSurfaceObject(-1)
,m_previousRightPointX(-1)
,m_previousRightPointY(-1)
,m_previousLeftPointX(-1)
,m_previousLeftPointY(-1)
,m_window(255)
,m_level(128) 
,m_windowMin(0)
,m_windowMax(8000)
,m_measureMode(MLINE)
#ifdef VTK_AVAILABLE
,m_dicomMatrix(NULL)
,m_regMatrix(NULL)
#endif
,m_volumeLabel("temp")
,m_currentCursorX(-1)
,m_currentCursorY(-1)
,m_extraCursorX(-1)
,m_extraCursorY(-1)
,m_alphaBlendEnabled(false)
,m_blendOpacity(0)
,m_graphics(0)
,m_alphaGraphics(0)
,m_currentScanningIndex(0)//invalid
,m_isScanning(false)
,m_isShiftingSurfaces(false)
,m_isRotatingSurfaces(false)
,m_isRegMatrixSet(false)
,m_isDicomMatrixSet(false)
,m_slicerLabel("")
,m_currentPositionTextSize(0)
,m_windowLevelTextSize(0)
,m_reconstructedBuffer(0)
,m_binaryBuffer(0)
,m_currentPaintMode(1)//1:X,Y 2:X,Z 3:Y,Z
,m_use3DBrush(false)
,m_isDroppingPoints(false)
,m_bitmapBuffer(0)
,m_bitmapBuffer_24(0)
,m_bitmapBuffer_32(0)
,m_surfaceContains("")
,m_centerOfRotation(Vector3(0,0,0))
,m_transformingNeedles(false)
,m_targetSurfaceRadius(0.5)
{
	m_targets = new CMeasure();
	m_surfaces = new CMeasure();
	m_template = new CMeasure();
	m_measurements = new CMeasure();

	#ifdef VTK_AVAILABLE
			m_dicomMatrix = vtkMatrix4x4::New();
			m_regMatrix = vtkMatrix4x4::New();
	#endif

#ifdef VTK_AVAILABLE
	MID_BASE_locator = vtkCellLocator::New();
	MID_APEX_locator = vtkCellLocator::New();

	CENTER_HORIZONTAL_locator = vtkCellLocator::New();
	CENTER_VERTICAL_locator = vtkCellLocator::New();
	BASE_LEFT_locator = vtkCellLocator::New();
	BASE_RIGHT_locator = vtkCellLocator::New();
	MID_LEFT_locator = vtkCellLocator::New();
	MID_RIGHT_locator = vtkCellLocator::New();
	APEX_RIGHT_locator = vtkCellLocator::New();
	APEX_LEFT_locator = vtkCellLocator::New();
#endif

	m_p1 = Vector3(0,0,0);
	m_p2 = Vector3(3,0,0);
	m_p2 = Vector3(0,3,0);

    QueryPerformanceFrequency(&freq);
}
RRI_SlicerInterface::~RRI_SlicerInterface(void)
{
    ClearPhaseImages();

    if (m_bitmapBuffer) GlobalFree(m_bitmapBuffer);
    if (m_bitmapBuffer_24) GlobalFree(m_bitmapBuffer_24);
    if (m_bitmapBuffer_32) GlobalFree(m_bitmapBuffer_32);


	if (m_graphics) delete m_graphics;
	if (m_alphaGraphics) delete m_alphaGraphics;
	if (m_targets) delete m_targets;
	if (m_surfaces) delete m_surfaces;
	if (m_template) delete m_template;//to contain the surfaces for the template needle trajectories
	if (m_measurements) delete m_measurements;
	if (m_vis) delete (m_vis);

	#ifdef VTK_AVAILABLE
		if(m_dicomMatrix!=NULL)
		{
			m_dicomMatrix->Delete();
			m_dicomMatrix=NULL;
			m_regMatrix->Delete();
			m_regMatrix = NULL;
		}
	#endif

#ifdef VTK_AVAILABLE
	MID_BASE_locator->Delete();
	MID_APEX_locator->Delete();
	CENTER_HORIZONTAL_locator->Delete();
	CENTER_VERTICAL_locator->Delete();
	BASE_LEFT_locator->Delete();
	BASE_RIGHT_locator->Delete();
	MID_LEFT_locator->Delete();
	MID_RIGHT_locator->Delete();
	APEX_RIGHT_locator->Delete();
	APEX_LEFT_locator->Delete();
#endif
}

//default initialization
long RRI_SlicerInterface::Initialize(long width, long height, std::string label, bool fullInit)
{
	m_slicerLabel = label;


if (fullInit)
{
	//setup default parameters
	AcqParam acq;										//parameters for acquisition
    CalParam cal;										//parameters for calibration
    acq.width = 100;								//width of ROI (video sub region)
    acq.height = 100;								//height of ROI (video sub region)
    acq.numFrames = 100;							//number of images to capture
    acq.horizontalReflection = HIR_PowerupDefault;		//no image reflection
    acq.scanDirection = SD_Clockwise;					//counter clockwise scan
    acq.probeOrientation = PO_Top;						//bottom orientation
    acq.scanGeometry = SG_LinearRaw;        
    acq.sweepAngle = 0.0;								//no shearing

    cal.axisOfRotation = 0;//acq.width/2;					//center of ROI is center of rotation
    cal.inPlaneTilt = 0.0f;								//not currently used
    cal.outPlaneDisplacement = 0.0f;					//not currently used
    cal.outPlaneTilt = 0.0f;							//not currently used
    cal.probeDistance = 0;								//not currently used
    cal.xVoxelSize = 0.15f;						//size of voxel in x direction (mm)
    cal.yVoxelSize = 0.15f;						//size of voxel in y direction (mm)
    cal.zVoxelSize = 1.0f;						//size of voxel in z direction (mm)

	if (m_vis) delete m_vis; m_vis = 0;
	m_vis = new Visualizer();


	m_vis->Initialize(width, height, acq, cal);

    
	
}
else
{
	m_vis->Initialize(width, height);
    
}


//if not full init, just initialize device context
	initHDC(width, height);
	ResetCube(1);//axial reset
	m_vis->Zoom(0.7);//zoom out
	m_width = width;
	m_height = height;

	

	return 0;
}

#ifdef VIDEO_AVAILABLE

void RRI_SlicerInterface::StartScan()
{ 
	m_isScanning = true; 
	m_currentScanningIndex = 0; 
	m_previousFrame = -1;
}

void RRI_SlicerInterface::StopScan()
{ 
	m_isScanning = false; 
}


bool RRI_SlicerInterface::Update(RRIObservable* op, const RRIEvent& m) 
{
	RRIEvent event = m;
    int eventCode = event.GetEventCode();

	switch (eventCode)
	{
	case FRAME_COMPLETE:
		{
			StartPerformanceCounter();

			unsigned char* buffer = (unsigned char*)event.GetControlObject();

			//correct for dropped frames
			long diff = m_currentScanningIndex - m_previousFrame;

			//this corrects for rounding error in acquisition
			if (diff>1)
			{
				m_currentScanningIndex -= 1;
			}

			diff = m_currentScanningIndex - m_previousFrame;

//this corrects for actual dropped frames
#define CORRECT_FOR_DROPPED_FRAMES
#ifdef CORRECT_FOR_DROPPED_FRAMES

			
			if (diff == 2)//this accounts for the error where a frame index comes in before the previous frame was stored.
			{
				//m_currentScanningIndex--;
                InsertFrame(buffer, m_currentScanningIndex-1, event.GetWidth(), event.GetHeight(), event.GetChannels());
			}
            else
            if (diff == 3)
            {
                InsertFrame(buffer, m_currentScanningIndex-1, event.GetWidth(), event.GetHeight(), event.GetChannels());
                InsertFrame(buffer, m_currentScanningIndex-2, event.GetWidth(), event.GetHeight(), event.GetChannels());
            }
#endif

			if (m_currentScanningIndex >= 0 && m_currentScanningIndex < GetVolumeDepth())
			{
				InsertFrame(buffer, m_currentScanningIndex, event.GetWidth(), event.GetHeight(), event.GetChannels());

				m_previousFrame = m_currentScanningIndex;
			}

			double time = EndPerformanceCounterGetDiff();
			std::ostringstream ss;
			ss << time;
			std::string s(ss.str());

			//m_graphics->DrawText(s.c_str(), 10, 10, GetHeight() - 22, GetWidth(), 50);
			//SetDisplayText((char*)s.c_str(), s.length());
			//m_graphics->DrawText(s.c_str(), 10, 1, 1, GetWidth(), 1);
		}
		break;

	}


	return true;
}
#endif

//here, we assume that the size of the frame is the same as one cube image
long RRI_SlicerInterface::InsertFrame(unsigned char* frame, long index, long width, long height, long channels) 
{
	unsigned char* buffer = GetBuffer();
	unsigned char* pointer = buffer + index*width*height;
	memcpy(pointer, frame, width*height*channels);

	return 0;
}

template<typename T> T stringTo( const std::string& s )
   {
      std::istringstream iss(s);
      T x;
      iss >> x;
      return x;
   };

template<typename T> inline std::string toString( const T& x )
   {
      std::ostringstream o;
      o << x;
      return o.str();
   }

bool RRI_SlicerInterface::readNFOfile(std::string filePath, long* width, long* height, long* count, float* xvoxel, float* yvoxel, float* zvoxel, float* linearExtent, float* angularExtent, 
                                      long* scanGeometry, long* probeOrientation, long* probeReflection, long* scanDirection, float* axisOfRotation) 
{
	std::string nfoFilePath = filePath;
	ReplaceString(nfoFilePath, ".raw", ".nfo");

	std::ifstream  nfoFile(nfoFilePath);
	if (nfoFile.is_open())
	{
		
		std::string line, name;
		*scanGeometry = SG_LinearRaw;//default
        *probeOrientation = PO_Top;//default
		//get a list of all the surfaces to load
		while ( nfoFile.good() )
		{

			getline (nfoFile,line);
			std::string tempString = line;

			{
			
				/*
				typedef enum
				{
					SG_Invalid			= -1,
					SG_ViewRaw			= 0,
					SG_LinearRaw		= 1,
					SG_Fan				= 2,
					SG_AxialNoOffsets	= 3,
					SG_AxialGeneral		= 4,
					SG_Pullback			= 6,
					SG_Linear16         = 7,
					SG_Linear24         = 8,
					SG_Fan24            = 9,
					SG_Axial24          = 10,
					SG_Hybrid           = 11,
					SG_Hybrid24         = 12
				}
				eScanGeometry;
				*/

               
                
                
           
                if (-1 != tempString.find("scanDirection"))
                {
                    *scanDirection = SD_Clockwise;//default

                    if (-1 != tempString.find("SD_Clockwise"))
                    {
                        *scanDirection = SD_Clockwise;
                    }
                    else
                    {
                        *scanDirection = SD_AntiClockwise;
                    }
                }
                else

                //parse reflection
                if (-1 != tempString.find("reflection"))
                {
                    *probeReflection = HIR_PowerupDefault;//default
                    
                    if (-1 != tempString.find("HIR_Reflected"))
                    {
                        *probeReflection = HIR_Reflected;
                    }
                    else
                    {
                        *probeReflection = HIR_PowerupDefault;
                    }
                }
                else
				//parse probe orientation
                if (-1 != tempString.find("probeOrientation"))
                {
                    *probeOrientation = PO_Top;//default

                    if (-1 != tempString.find("PO_Top"))
                    {
                        *probeOrientation = PO_Top;//default
                    }
                    else
                    {
                        *probeOrientation = PO_Bottom;//default
                    }
                }
                else
                //parse scan geometry
				if (-1 != tempString.find("scanGeometry"))
				{
					*scanGeometry = SG_LinearRaw;//default
                    
					if (-1 != tempString.find("SG_LinearRaw"))
					{
						*scanGeometry = SG_LinearRaw;
					}
					else
					if (-1 != tempString.find("SG_Fan"))
					{
						*scanGeometry = SG_Fan;
					}
					else
					if (-1 != tempString.find("SG_AxialNoOffsets"))
					{
						*scanGeometry = SG_AxialNoOffsets;
					}
					else
					if (-1 != tempString.find("SG_Linear16"))
					{
						*scanGeometry = SG_Linear16;
					}
					else
					if (-1 != tempString.find("SG_Linear24"))
					{
						*scanGeometry = SG_Linear24;
					}
					else
					if (-1 != tempString.find("SG_Fan24"))
					{
						*scanGeometry = SG_Fan24;
					}
					else
					if (-1 != tempString.find("SG_Axial24"))
					{
						*scanGeometry = SG_Axial24;
					}
					else
					if (-1 != tempString.find("SG_Hybrid"))
					{
						*scanGeometry = SG_Hybrid;
					}
					else
					if (-1 != tempString.find("SG_Hybrid24"))
					{
						*scanGeometry = SG_Hybrid24;
					}
					else
					{
						*scanGeometry = SG_LinearRaw;
					}

				}
				else
				if (-1 != tempString.find("width:"))
				{

					ReplaceString(tempString, "width: ", "");
					*width = atoi(tempString.c_str());
				}
				else if (-1 != tempString.find("height"))
				{

					ReplaceString(tempString, "height: ", "");
					*height = atoi(tempString.c_str());
					
				}
				else if (-1 != tempString.find("numframes"))
				{
					ReplaceString(tempString, "numframes: ", "");
					*count = atoi(tempString.c_str());
						
				}
				else if (-1 != tempString.find("xvoxelsize"))
				{
					ReplaceString(tempString, "xvoxelsize: ", "");
					*xvoxel = atof(tempString.c_str());
							
				}
				else if (-1 != tempString.find("yvoxelsize"))
				{
					ReplaceString(tempString, "yvoxelsize: ", "");
					*yvoxel = atof(tempString.c_str());
								
				}
				else if (-1 != tempString.find("zvoxelsize"))
				{
					ReplaceString(tempString, "zvoxelsize: ", "");
					*zvoxel = atof(tempString.c_str());
				}
				else if (-1 != tempString.find("linearExtent"))
				{
					ReplaceString(tempString, "linearExtent: ", "");
					*linearExtent = atof(tempString.c_str());
				}
				else if (-1 != tempString.find("angularExtent"))
				{
					ReplaceString(tempString, "angularExtent: ", "");
					*angularExtent = atof(tempString.c_str());
				}
				else if (-1 != tempString.find("axisOfRotation"))
				{
					ReplaceString(tempString, "axisOfRotation: ", "");
					*axisOfRotation = atof(tempString.c_str());
				}
			}
		}

		if(*width > 0 && *height > 0 && *count > 0 && *xvoxel > 0 && *yvoxel > 0 && *zvoxel > 0)
		{
			return true;
		}
		else 
		{
			return false;
		}

	}

	return true;
}

long RRI_SlicerInterface::ImportRaw(std::string filePath)
{
    long dx, dy, dz;
    float px, py, pz;
	float linearExtent = 0.0;
	float angularExtent = 0.0;
	float axisOfRotation = 0.0;

	long channels = 1;

    std::string nfoPath = filePath;

	long scanGeometry;
    long probeOrientation;
    long probeReflection;
    long scanDirection;

	readNFOfile(nfoPath, &dx, &dy, &dz, &px, &py, &pz, &linearExtent, &angularExtent, &scanGeometry, &probeOrientation, &probeReflection, &scanDirection, &axisOfRotation);

    if (scanGeometry == SG_Linear16)
    {
        channels = 2;
    }


    AcqParam acq;                                 //parameters for acquisition
    CalParam cal;                                 //parameters for calibration
    acq.width = dx;                              //width of ROI (video sub region)
    acq.height = dy;                             //height of ROI (video sub region)
    acq.numFrames = dz;                          //number of images to capture

    acq.horizontalReflection = (eHorizontalImageReflection)probeReflection;//no image reflection
    acq.scanDirection = (eScanDirection)scanDirection;          //counter clockwise scan
    acq.probeOrientation = (eProbeOrientation)probeOrientation;             //bottom orientation
    acq.scanGeometry = (eScanGeometry)scanGeometry;         //reconstruction, linear only
    acq.sweepAngle = 0.0;                      //no shearing

	cal.axisOfRotation = axisOfRotation;           //center of ROI is center of rotation
    cal.inPlaneTilt = 0.0f;                       //not currently used
    cal.outPlaneDisplacement = 0.0f;              //not currently used
    cal.outPlaneTilt = 0.0f;                      //not currently used
    cal.probeDistance = 0;                        //not currently used
    cal.xVoxelSize = px;                      //size of voxel in x direction (mm)
    cal.yVoxelSize = py;                      //size of voxel in y direction (mm)
    cal.zVoxelSize = pz;                      //size of voxel in z direction (mm)

	if (scanGeometry == SG_Hybrid || scanGeometry == SG_Hybrid24)
	{
		if (linearExtent > 0 && angularExtent > 0)//Hybrid Scan
		{
			acq.sweepAngle = angularExtent;
			cal.probeDistance = linearExtent;
		}
	}
	else
	if (scanGeometry == SG_Fan || scanGeometry == SG_Fan24)
	{
		if (angularExtent > 0)
		{
			acq.sweepAngle = angularExtent;
		}

	}


    CreateVolume(acq,cal,channels);//clear contours

	m_isRegMatrixSet = false;

	
//read data
    tVoxel* buffer = GetBuffer();

    std::ifstream input(filePath, STD::ios::in | STD::ios::binary); 
    if (input.good()) 
	{

        input.read((char*)buffer, dx*dy*dz*channels);
    }

    input.close();

	bool done = false;
	while (!done)
	{
		if (!ReplaceString(filePath, "\\", "/"))
		{
			done = true;
		}
	}

	int found = filePath.rfind( "/");
	std::string label;
	std::string folderPath;
	if( found != -1 )
	{
		label = filePath.substr( found+1, filePath.size() - 1 );
		folderPath = filePath.substr(0, found+1);
	}

	SetVolumeLabel(label);
	SetVolumeFolderPath(folderPath);
	SetVolumeFilePath(filePath);
	
    return 0;
}


//for Hybrid, Fan and Linear
long  RRI_SlicerInterface::ExportRaw(std::string filePath)
{
	//double resolution = 0.5;//mm

    if (-1 != filePath.find(".raw"))
	{
		fstream dFile = fstream(filePath.c_str(), ios::out | ios::binary);

		//reconstructed cube dimensions and size
		Vector3 cubedim = m_vis->GetCubeDim(); 
		Vector3 cubeSize = m_vis->GetCubeSize();

		long xdim = cubedim.x;
		long ydim = cubedim.y;
		long zdim = cubedim.z;

		float xvoxelReco = cubeSize.x / cubedim.x;
		float yvoxelReco = cubeSize.y / cubedim.y;
		float zvoxelReco = cubeSize.z / cubedim.z;

		
		//unreconstructed parameters for non-linear geometries
		long width = GetVisualizer()->GetVolume()->m_Acq.width;
		long height = GetVisualizer()->GetVolume()->m_Acq.height;
		long depth = GetVisualizer()->GetVolume()->m_Acq.numFrames;

		float xvoxel = GetVoxelX();
		float yvoxel = GetVoxelY();
		float zvoxel = GetVoxelZ();

		float axisOfRotation = GetVisualizer()->GetVolume()->m_Cal.axisOfRotation;

		//parameters for fan and hybrid scans
		float linearExtent = GetVisualizer()->GetVolume()->m_Cal.probeDistance;//used in hybrid
		float angularExtent = GetVisualizer()->GetVolume()->m_Acq.sweepAngle;//used in fan, axial and hybrid
		
		std::string scanGeometry = "SG_LinearRaw";//8-bit greyscale

		long channels = 1;
		
		if (GetScanGeometry() == SG_LinearRaw)
		{
			scanGeometry = "SG_LinearRaw";
			width = xdim;
			height = ydim;
			depth = zdim;
			xvoxel = xvoxelReco;
			yvoxel = yvoxelReco;
			zvoxel = zvoxelReco;
			
		}
		else
		if (GetScanGeometry() == SG_Linear16)
		{
			channels = 2;
			scanGeometry = "SG_Linear16";
			width = xdim;
			height = ydim;
			depth = zdim;
			xvoxel = xvoxelReco;
			yvoxel = yvoxelReco;
			zvoxel = zvoxelReco;
			
		}
		else
		if (GetScanGeometry() == SG_Linear24)
		{
			channels = 3;
			scanGeometry = "SG_Linear24";
			width = xdim;
			height = ydim;
			depth = zdim;
			xvoxel = xvoxelReco;
			yvoxel = yvoxelReco;
			zvoxel = zvoxelReco;
			
		}
		else
		if (GetScanGeometry() == SG_Hybrid)
		{
			channels = 1;
			scanGeometry = "SG_Hybrid";
			

		}
		else
		if (GetScanGeometry() == SG_Hybrid24)
		{
			channels = 3;
			scanGeometry = "SG_Hybrid24";
			
        }
        else
		if (GetScanGeometry() == SG_Fan)
		{
			channels = 1;
			scanGeometry = "SG_Fan";
			
		}
		else
        if (GetScanGeometry() == SG_Fan24)
        {
            channels = 3;
            scanGeometry = "SG_Fan24";
			
        }
        else
        if (GetScanGeometry() == SG_AxialNoOffsets)
        {
            channels = 1;
            scanGeometry = "SG_AxialNoOffsets";
			
        }
        else
        if (GetScanGeometry() == SG_Axial24)
        {
            channels = 1;
            scanGeometry = "SG_Axial24";
			
        }

		dFile.write((char*)GetBuffer(), width * height * depth * channels);
		dFile.close();


//write .nfo companion file
		// write raw data parameters in separate file
		std::string textParamPath = filePath;

		ReplaceString(textParamPath, ".raw", ".nfo");
	
		fstream tFile = fstream(textParamPath.c_str(), ios::out | ios::binary);

		char outstring[100];
	

		sprintf(outstring,     "width: %d\n", width);
		tFile.write(outstring, strlen(outstring));
	
		sprintf(outstring,     "height: %d\n", height);
		tFile.write(outstring, strlen(outstring));
	
		sprintf(outstring,     "numframes: %d\n", depth);
		tFile.write(outstring, strlen(outstring));
	
		sprintf(outstring,     "xvoxelsize: %f\n", xvoxel);
		tFile.write(outstring, strlen(outstring));
	
		sprintf(outstring,     "yvoxelsize: %f\n", yvoxel);
		tFile.write(outstring, strlen(outstring));
	
		sprintf(outstring,     "zvoxelsize: %f\n", zvoxel);
		tFile.write(outstring, strlen(outstring));


        //output scan geometry
		std::string temp =  "scanGeometry: ";
		temp += scanGeometry;
		temp += "\n";
		sprintf(outstring,  temp.c_str());
		tFile.write(outstring, strlen(outstring));

        //output probe orientation
        temp = "probeOrientation: ";

        if (GetProbeOrientation() == PO_Top)
        {
            temp += "PO_Top\n";
        }
        else
        {
            temp += "PO_Bottom\n";
        }

        sprintf(outstring,  temp.c_str());
		tFile.write(outstring, strlen(outstring));

        //output scan reflection
        temp = "reflection: ";

        if (GetReflection() == HIR_Reflected)
        {
            temp += " HIR_Reflected\n";
        }
        else
        {
            temp += " HIR_PowerupDefault\n";
        }

		sprintf(outstring, temp.c_str());
		tFile.write(outstring, strlen(outstring));

        //output scan direction
        temp = "scanDirection: ";

        if (GetScanDirection() == SD_Clockwise)
        {
            temp += " SD_Clockwise\n";
        }
        else
        {
            temp += " SD_AntiClockwise\n";
        }

        sprintf(outstring,  temp.c_str());
		tFile.write(outstring, strlen(outstring));

        //output linear extent
		sprintf(outstring,     "linearExtent: %f\n", linearExtent);
		tFile.write(outstring, strlen(outstring));

        //output angular extent
		sprintf(outstring,     "angularExtent: %f\n", angularExtent);
		tFile.write(outstring, strlen(outstring));

        //output axis of rotation
		sprintf(outstring, "axisOfRotation: %f\n", axisOfRotation);
		tFile.write(outstring, strlen(outstring));
	
		tFile.close();
		
	}
																				
    return 0;


}



long RRI_SlicerInterface::UpdateVolumeInfo(std::string filepath)
{
    bool done = false;
	while (!done)
	{
		if (!ReplaceString(filepath, "\\", "/"))
		{
			done = true;
		}
	}

	int found = filepath.rfind( "/");
	std::string label;
	std::string folderPath;
	if( found != -1 )
	{
		label = filepath.substr( found+1, filepath.size() - 1 );
		folderPath = filepath.substr(0, found+1);
	}

	SetVolumeLabel(label);
	SetVolumeFolderPath(folderPath);
	SetVolumeFilePath(filepath);

    return 0;

}

long RRI_SlicerInterface::ImportMHA(std::string filepath)
{
    ClearSurfaces();
	ClearTargets();

    vtkMetaImageReader* reader = vtkMetaImageReader::New();
    reader->SetFileName(filepath.c_str());
    reader->SetSwapBytes(true);
    reader->SetDataByteOrderToLittleEndian();
    reader->Update();

    vtkImageData* image = reader->GetOutput();

    CreateVolumeFromVtkImageData(image, 128, 250);//can convert 8 and 16 bit

    

	m_isRegMatrixSet = false;

	bool done = false;
	while (!done)
	{
		if (!ReplaceString(filepath, "\\", "/"))
		{
			done = true;
		}
	}

	int found = filepath.rfind( "/");
	std::string label;
	std::string folderPath;
	if( found != -1 )
	{
		label = filepath.substr( found+1, filepath.size() - 1 );
		folderPath = filepath.substr(0, found+1);
	}

	SetVolumeLabel(label);
	SetVolumeFolderPath(folderPath);
	SetVolumeFilePath(filepath);

    reader->Delete();
	
    return 0;


}




long RRI_SlicerInterface::ExportMHA(std::string filePath)
{
 
    long width = GetVolumeWidth();
    long height = GetVolumeHeight();
    long depth = GetVolumeDepth();

    double voxelX = GetVoxelX();
    double voxelY = GetVoxelY();
    double voxelZ = GetVoxelZ();

    vtkImageImport* inputData = vtkImageImport::New();
	inputData->SetDataSpacing(voxelX, voxelY, voxelZ);
    inputData->SetDataOrigin(0, 0, 0);
    inputData->SetWholeExtent(0, width-1, 0, height-1, 0, depth-1);
	inputData->SetDataExtent(0, width-1, 0, height-1, 0, depth-1);
    inputData->SetDataScalarTypeToUnsignedChar();
    inputData->SetNumberOfScalarComponents(1);
    inputData->SetImportVoidPointer(GetBuffer());
    inputData->Update();

    vtkImageData* data = inputData->GetOutput();
    


    vtkMetaImageWriter* writer = vtkMetaImageWriter::New();
    writer->SetFileName(filePath.c_str());
    writer->SetInput(data);
    writer->SetCompression(false);
    writer->Write();

    inputData->Delete();
    writer->Delete();
   
    return 0;
}




long  RRI_SlicerInterface::ExportBinary(std::string filePath)
{


    if (-1 != filePath.find(".raw"))
    {
        fstream dFile = fstream(filePath.c_str(), ios::out | ios::binary);

        Vector3 cubedim = m_vis->GetCubeDim();
        Vector3 cubeSize = m_vis->GetCubeSize();

        long xdim = cubedim.x;
        long ydim = cubedim.y;
        long zdim = cubedim.z;

        float xvoxel = cubeSize.x/cubedim.x;
        float yvoxel = cubeSize.y/cubedim.y;
        float zvoxel = cubeSize.z/cubedim.z;

        std::string scanGeometry = "SG_LinearRaw";//8-bit greyscale
   
        dFile.write((char*)m_binaryBuffer, xdim * ydim * zdim);
        dFile.close();


        //write .nfo companion file
        // write raw data parameters in separate file
        std::string textParamPath = filePath;

        ReplaceString(textParamPath, ".raw", ".nfo");

        fstream tFile = fstream(textParamPath.c_str(), ios::out | ios::binary);

        char outstring[100];

        sprintf(outstring, "width: %d\n", xdim);
        tFile.write(outstring, strlen(outstring));

        sprintf(outstring, "height: %d\n", ydim);
        tFile.write(outstring, strlen(outstring));

        sprintf(outstring, "numframes: %d\n", zdim);
        tFile.write(outstring, strlen(outstring));

        sprintf(outstring, "xvoxelsize: %f\n", xvoxel);
        tFile.write(outstring, strlen(outstring));

        sprintf(outstring, "yvoxelsize: %f\n", yvoxel);
        tFile.write(outstring, strlen(outstring));

        sprintf(outstring, "zvoxelsize: %f\n", zvoxel);
        tFile.write(outstring, strlen(outstring));

        std::string temp = "scanGeometry: ";
        temp += scanGeometry;
        temp += "\n";
        sprintf(outstring, temp.c_str());
        tFile.write(outstring, strlen(outstring));

        tFile.close();

    }

    return 0;


}

//::TODO::
long RRI_SlicerInterface::ImportVolume(const char* filePath, long stringLength)
{
//read nfo file first to see the dimensions of the scan
/*
width: 448
height: 448
numframes: 350
xvoxelsize: 0.184998
yvoxelsize: 0.184998
zvoxelsize: 0.190002
*/
/*

	string filePathString (filePath);

	if (-1 == filePathString.find(".raw"))
	{
		return -1;
	}

	string companionFile = filePathString;


	//assume this is a .raw file for now
	ReplaceString(companionFile, ".raw", ".nfo");

	string width, height, numFrames, xvoxel, yvoxel, zvoxel;
	long iWidth=100, iHeight=100, iDepth=100;
	float fXvoxel=1.0, fYvoxel=1.0, fZvoxel=1.0;
	ifstream file(companionFile.c_str());
	if (file.is_open())
	{
		//get width
		getline(file, width);
		ReplaceString(width, "width: ", "");
		iWidth = stringTo<long>(width);
		
		//get height
		getline(file, height);
		ReplaceString(height, "height: ", "");
		iHeight = stringTo<long>(height);

		//get number of frames
		getline(file, numFrames);
		ReplaceString(numFrames, "numframes: ", "");
		iDepth = stringTo<long>(numFrames);

		//get xvoxel
		getline(file, xvoxel);
		ReplaceString(xvoxel, "xvoxelsize: ", "");
		fXvoxel = stringTo<float>(xvoxel);

		//get yvoxel
		getline(file, yvoxel);
		ReplaceString(yvoxel, "yvoxelsize: ", "");
		fYvoxel = stringTo<float>(yvoxel);

		//get zvoxel
		getline(file, zvoxel);
		ReplaceString(zvoxel, "zvoxelsize: ", "");
		fZvoxel = stringTo<float>(zvoxel);

		file.close();

		//initialize visualizer with new parameters
		GetVisualizer()->Initialize(m_width, m_height, SG_LinearRaw, iWidth,iHeight,iDepth,1, fXvoxel, fYvoxel, fZvoxel,0.0,0.0);//initialize with default volume SG_LinearRaw, 100x100x100
		
		GetVisualizer()->GetVolume()->ImportRaw(filePathString);//now import the data into the volume class

		GetVisualizer()->Zoom(0.7);//zoom out

	}
	{
		//unable to open file
	}
*/

	return 0;
}

void RRI_SlicerInterface::ClearAlphaBlend()
{
	m_alphaBlendEnabled = false;//stop alphablending
}

void RRI_SlicerInterface::SetAlphaBlend(long blendOpacity)
{
	m_blendOpacity = blendOpacity;
	if (blendOpacity == 0)
	{
		m_alphaBlendEnabled = false;
	}
	else
	{
		m_alphaBlendEnabled = true;
	
	}

}

void RRI_SlicerInterface::DoAlphaBlend()
{
	if (!m_alphaBlendEnabled) return;

	HDC sourceDC = m_alphaGraphics->GetDeviceContext();
	if (sourceDC == NULL) return;

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = m_blendOpacity;
	bf.AlphaFormat = 0;//use SourceConstantAlpha blend

	/*
	INGDIAPI BOOL WINAPI AlphaBlend(
    __in HDC hdcDest,
    __in int xoriginDest,
    __in int yoriginDest,
    __in int wDest,
    __in int hDest,
    __in HDC hdcSrc,
    __in int xoriginSrc,
    __in int yoriginSrc,
    __in int wSrc,
    __in int hSrc,
    __in BLENDFUNCTION ftn);*/

	BOOL error = AlphaBlend(m_graphics->GetDeviceContext(), 0, 0, GetWidth(), GetHeight(), sourceDC, 0,0,GetWidth(), GetHeight(), bf);

	if (FALSE == error)
	{

	}

	return;
}

long RRI_SlicerInterface::UpdateDisplay()
{
	if (m_isRendering) return -1;
    


    LARGE_INTEGER	c1, c2;
    // Get first snapshot of High-Res Timer value
    QueryPerformanceCounter(&c1);



	m_isRendering = true;
	if (NULL == m_graphics) return -1;

	GetVisualizer()->Render(-1);
	FilledBuff buffer = GetVisualizer()->GetImBuff();
	long channels = 1;
	if (SG_Linear24 == GetScanGeometry())
		channels = 3;

	m_graphics->SetBitmapBits(buffer.buff, m_width, m_height, channels);


	if (m_showWireframe)
	{
		if (m_activeFace >= 0)	
			m_graphics->DrawWireFrame(GetVisualizer()->GetView(),   m_activeFace);
		
	}

	if (m_showUserPoints)
	{

	    const PlotVec &plotVector = m_targets->GetPoints(GetVisualizer()->GetView(), &m_target_surface);

        m_graphics->DrawSurfaceContours(plotVector, RGB(255,0,0));//red
		
	}

	if (m_showSurfaces && m_template)
	{
		for (int i=0; i<m_templateList.size(); i++)
        {

            SurfaceObject so = m_templateList.at(i);
			if (so.visible == true)
			{
				TriangleVec tv = (so.triangles);

				COLORREF colour = so.colour;

				const PlotVec &plot = m_template->GetPoints(GetVisualizer()->GetView(),  &tv);
				m_graphics->DrawSurfaceContours(plot, so.colour);
			}

        }
	}

	//show surfaces that are stored in m_surfaceList
	if (m_showSurfaces && m_surfaces)
    {
        m_currentPlotVector.clear();
        long size = m_surfaceList.size();
        for (int i=0; i< size; i++)
        {
            
            SurfaceObject so = m_surfaceList.at(i);
			if (so.visible == true)
			{
				TriangleVec tv = (so.triangles);

				COLORREF colour = so.colour;

                BYTE red = GetRValue(colour);
                BYTE green = GetGValue(colour);
                BYTE blue = GetBValue(colour);


				const PlotVec &plot = m_surfaces->GetPoints(GetVisualizer()->GetView(),  &tv);

                //-------------------------------------------------------
                ////copy plot vector into public vector so we can paint onto live video window when necessary
                int size = plot.size();

                if (size > 0)
                {
                    
                    //copy into current
                    for (int i=0; i<size; i++)
                    {
                        Plot p = plot.at(i);

                        //set colour of surface object
                        p.red = red;
                        p.green = green;
                        p.blue = blue;

                        m_currentPlotVector.push_back(p);
                    }
                }
                //-------------------------------------------------------

				m_graphics->DrawSurfaceContours(plot, so.colour);


			}

        }
    }

	if (m_showMeasurements && m_measurements)
	{

		//draw measurements
		const PlotVec &plot = m_measurements->GetPoints(GetVisualizer()->GetView());
		const MObjpVec &mobjlist = m_measurements->GetMeasureObjects();

		//draw measurement objects in the volume
		m_graphics->DrawMeasurements(plot);			           
	}

	//rubber band
	if (!measure.New && !mPoly.empty()) 
	{
		POINT currentPoint;
		currentPoint.x = m_currentX;
		currentPoint.y = m_currentY;

		m_graphics->MeasureRubberBand(m_measureMode, measure.polyMode, mPoly, currentPoint);
		// Draw ruler
		if (m_measureMode == MLINE && mPoly.size()>0)
		{
			Vector3 a, b;
			GetVisualizer()->ScrnPtToModelPt(mPoly[0].x, mPoly[0].y, &a);
			long ret = GetVisualizer()->ScrnPtToModelPt(currentPoint.x, currentPoint.y, &b);
			a.x -= b.x; a.y -= b.y; a.z -= b.z; 
			if (ret && m_measureFace == GetVisualizer()->GetFaceID(currentPoint.x, currentPoint.y))
			{
				m_graphics->DrawRuler(mPoly[0], currentPoint, sqrt(a.x*a.x + a.y*a.y + a.z*a.z));
			}

		}

	}	

    //draw extra cursor
    if (m_isPainting || m_isErasing || m_isEditing)
    {
        COLORREF color = RGB(255,0,0);//painting inside
		if (m_isErasing || m_isEditing)
		{
			color = RGB(155, 155, 155);
		}
        else
        if (m_isEditing)
        {
            color = RGB(200, 200, 200);
        }
		else
        if (!m_paintingInside)
        {
            color = RGB(170, 170, 255);
        }
        

		m_graphics->DrawCircle(m_currentCursorX - m_paintBrushSize,
                               m_currentCursorY - m_paintBrushSize, 
							   m_currentCursorX + m_paintBrushSize,
							   m_currentCursorY + m_paintBrushSize, color);
    }

	//Draw border around slicer area is slicer view is selected
    if (m_isSelected)
    {
        m_graphics->DrawRectangle(1,1,m_width-1, m_height-1, RGB(255,255,0));
        m_graphics->DrawRectangle(2,2,m_width-2, m_height-2, RGB(255,255,0));
        m_graphics->DrawRectangle(3,3,m_width-3, m_height-3, RGB(255,255,0));
    }
	else
	{
		m_graphics->DrawRectangle(1,1,m_width-1, m_height-1, RGB(0,0,255));//blue pen
        m_graphics->DrawRectangle(2,2,m_width-2, m_height-2, RGB(0,0,255));
        m_graphics->DrawRectangle(3,3,m_width-3, m_height-3, RGB(0,0,255));
	}

	//DrawText(LPCTSTR lpchText, long length, long posX, long posY, long width, long height)
	if (m_displayTextSize > 0 && GetWidth() > 100)
	{
		m_graphics->DrawText((LPCTSTR)m_displayText, m_displayTextSize, 10, 10, GetWidth(), 50);
	}

	//DrawText(LPCTSTR lpchText, long length, long posX, long posY, long width, long height)
	if (m_windowLevelTextSize > 0 && GetWidth() > 100)
	{
		m_graphics->DrawText((LPCTSTR)m_windowLevelText, m_windowLevelTextSize, 10, GetHeight()-22, GetWidth(), 50);
	}
 
	//DrawText(LPCTSTR lpchText, long length, long posX, long posY, long width, long height)
	if (m_currentPositionTextSize > 0 && GetWidth() > 100)
	{
		m_graphics->DrawText((LPCTSTR)m_currentPositionText, m_currentPositionTextSize, 200, GetHeight()-22, GetWidth(), 50);
	}

	if (m_alphaBlendEnabled)
	{
		DoAlphaBlend();
	}


	
    // Get second snapshot
    QueryPerformanceCounter(&c2);

    //calculate difference (in ms)
    float diff = float(c2.QuadPart - c1.QuadPart) / float(freq.QuadPart) * 1000.0;
    std::ostringstream ss;
    ss << diff;
    std::string s(ss.str());

   // m_graphics->DrawText(s.c_str(), 10, 10, GetHeight() - 22, GetWidth(), 50);
	
	m_isRendering = false;

	return 0;
}

void RRI_SlicerInterface::StartPerformanceCounter()
{
    QueryPerformanceCounter(&counterStart);

}

double RRI_SlicerInterface::EndPerformanceCounterGetDiff()
{
    // Get second snapshot
    QueryPerformanceCounter(&counterEnd);

    //calculate difference (in ms)
    float diff = float(counterEnd.QuadPart - counterStart.QuadPart) / float(freq.QuadPart) * 1000.0;
    return diff;

}

long RRI_SlicerInterface::initHDC(long width, long height)
{
	if (m_graphics) delete m_graphics;
	m_graphics = new RRI_Graphics();
	m_graphics->Initialize(width, height);

	//create alpha window the same width and height as the graphics window
	if (m_alphaGraphics) delete m_alphaGraphics;
	m_alphaGraphics = new RRI_Graphics();
	m_alphaGraphics->Initialize(width, height);


	return 0;
}
long RRI_SlicerInterface::GetWidth()
{
	return m_width;
}

long RRI_SlicerInterface::GetHeight()
{
	return m_height;
}

unsigned char* RRI_SlicerInterface::GetBitmapBits()
{
	if (m_graphics == NULL)
		return 0;

	return m_graphics->GetBitmapBits();
}

unsigned char* RRI_SlicerInterface::GetOverlayBits()
{
	if (m_alphaGraphics == NULL)
		return 0;

	return m_alphaGraphics->GetBitmapBits();
}

long RRI_SlicerInterface::SetActiveFace(long x, long y)
{
	if (InsideCube(x,y))
	{
		m_activeFace = GetVisualizer()->GetFaceID(x,y);

		if (m_activeFace > -1 && m_activeFace < 6)//original faces
		{
			//simulate mouse down event
			SetSlicing(true);
			StartSlicing(0, x, y);
			StopSlicing(0,x,y);
			m_activeFace = GetVisualizer()->GetFaceID(x,y);//get it again
		}
	}

	return 0;
}

void RRI_SlicerInterface::CenterCube()
{
	//get center of volume in screen coordinates (ie. projected to the screen)
	Vector3 screenPoint = GetVolumeToScreen(Vector3(0,0,0));
	long centerX = GetWidth()/2;
	long centerY = GetHeight()/2;

	long diffX = centerX - screenPoint.x;
	long diffY = centerY - screenPoint.y;

	DoPan(diffX, diffY);

}

void RRI_SlicerInterface::DoPan(long x, long y)
{
	GetVisualizer()->ActionModelView(0,0, BEGIN, ST_PAN);
	GetVisualizer()->ActionModelView(x,y,CONTINUE, ST_ALREADY_SET);
	GetVisualizer()->ActionModelView(x,y,END, ST_ALREADY_SET);

}

Vector3 RRI_SlicerInterface::GetVolumeToScreen(Vector3 point)
{
	const viewORTHO& view = GetVisualizer()->GetView();
	Vector3 point2D = point * view.Model2View;
	return point2D;
}

bool RRI_SlicerInterface::InsideCube(long x, long y)
{
    Vector3 p;
    return GetVisualizer()->ScrnPtToModelPt(x, y, &p);
}

bool RRI_SlicerInterface::InsideCube(float x, float y, float z)
{
	Vector3 cubeSize = GetVisualizer()->GetCubeSize();

	if (x>(-cubeSize.x/2.0) && x<(cubeSize.x/2.0)  && 
		y>(-cubeSize.y/2.0) && y<(cubeSize.y/2.0)  &&
		z>(-cubeSize.z/2.0) && z<(cubeSize.z/2.0))
	{
		return true;
	}
	else
	{
		return false;
	}
}


void RRI_SlicerInterface::StartSlicing(long nFlags, long x, long y)
{

    long selectedFace = GetVisualizer()->GetFaceID(x, y);
    

    if (selectedFace >= 0)//did user click inside cube area
    {
        m_activeFace = selectedFace;
    }
   
    //PANNING
    if ( IsPanning() && selectedFace != -1)
	{
		GetVisualizer()->ActionModelView(x, y, BEGIN, ST_PAN);
        SetState(ST_PAN);
        return;
	}

    //ROTATE cube
	if (selectedFace == -1)
	{
		GetVisualizer()->ActionModelView(x, y, BEGIN, ST_ROTATE);
        SetState(ST_ROTATE);
        return;
	}

    //SLICE face
	long edgeFace = GetVisualizer()->GetEdgeFace(x, y);
	if ( edgeFace == -1)
	{
		GetVisualizer()->ActionModelView(x, y, BEGIN, ST_SLICE);
		SetState(ST_SLICE);
	}

    //ROTATE face
	else
	{
		GetVisualizer()->ActionModelView(x, y, BEGIN, ST_SLICE_ROTATE);
		SetState(ST_SLICE_ROTATE);
	}


}

/*
void RRI_SlicerInterface::ContinueMeasuring(long nFlags, long x, long y)
{
   // currentPoint = CPoint(x,y);
    UpdateDisplay();
}
*/


void RRI_SlicerInterface::ContinueSlicing(long nFlags, long x, long y)
{
//	if (!IsInitialized() || !m_displayWindow) return;

	
    if (m_slicing || m_panning)
	{
		
		GetVisualizer()->ActionModelView(x, y, CONTINUE, ST_ALREADY_SET);//this calls m_vis->render(LORES)
		GetVisualizer()->Render();
        //***UpdateDisplay();
	}
	else
	{
		//***UpdateDisplay();
	}

    //update slicer state based on current pointer position and flags
    UpdateState(nFlags, x, y);

}

void RRI_SlicerInterface::StopSlicing(long nFlags, long x, long y)
{
//    x-=m_xoffset;
//    y-=m_yoffset;

//    ReleaseCapture();
	GetVisualizer()->ActionModelView(x, y, END, ST_ALREADY_SET);
    GetVisualizer()->Render();
    //***UpdateDisplay();

}
void RRI_SlicerInterface::SetState(long state)
{
	m_currentState = state;
}

void RRI_SlicerInterface::SetPanning(bool panning)
{
	m_panning = panning;
}

bool RRI_SlicerInterface::IsPanning()
{
	return m_panning;
}

void RRI_SlicerInterface::SetSlicing(bool slicing)
{
	m_slicing = slicing;
}

bool RRI_SlicerInterface::IsSlicing()
{
	return m_slicing;
}

void RRI_SlicerInterface::UpdateState(long flags, long x, long y)
{

    long edgeFace = GetVisualizer()->GetEdgeFace(x, y);

    // Update state
	if ( InsideCube(x,y) ) 
	{
        if (IsPanning())
        {
            SetState(ST_PAN);
        }

#ifdef OBLIQUE_SLICING_AVAILABLE
        else
        if (edgeFace >= 0)
			SetState(ST_SLICE_ROTATE);
#endif

		else
			SetState(ST_SLICE);
	}

    // outside cube
	else 
	{
		/*
        if (IsRotatingLines())
        {
            SetState(ST_MEASURE);

        }
        else
		*/
        {
	        SetState(ST_ROTATE);
        }
	}
}
void RRI_SlicerInterface::UpdateLinkingPoints()
{
	long centerX = GetWidth()/2;//x
	long centerY = GetHeight()/2;//y
	//find three points in the plane
	m_p1 = ScreenToModel(centerX,centerY);
	m_p2 = ScreenToModel(centerX-3, centerY);
	m_p3 = ScreenToModel(centerX-3, centerY-3);

}
//mouse functions
void RRI_SlicerInterface:: MouseDownEvent(UINT nFlags, long x, long y)
{

	if (InsideCube(x,y) && nFlags & MK_LBUTTON)
	{
		long centerX = x;//GetWidth()/2;//x
		long centerY = y;//GetHeight()/2;//y
		//find three points in the plane

        UpdateLinkingPoints();
		//m_p1 = ScreenToModel(centerX,centerY);
		//m_p2 = ScreenToModel(centerX-3, centerY);
		//m_p3 = ScreenToModel(centerX-3, centerY-3);

		m_currentPosition = ScreenToModel(x,y);

		if (m_isPainting || m_isErasing || m_isEditing)
		{
			m_isDroppingPoints = true;
		}

	}
	else
    if (!InsideCube(x, y) && nFlags & MK_LBUTTON)
	{
        m_isPainting = false;//turn off painting when user clicks outside the cube
        m_isErasing = false;
        m_isEditing = false;
	}

	if (m_isRotatingSurfaces || m_isShiftingSurfaces)//left button down)
	{
		m_previousLeftPointX = x;
		m_previousLeftPointY = y;
		return;
	}
	else
	if (nFlags & MK_LBUTTON)//left button down
	{
		m_previousLeftPointX = x;
		m_previousLeftPointY = y;

		if (nFlags & MK_SHIFT)//if shift key is down
		{
			if (InsideCube(x,y))
			{
				SetPanning(true);
				StartSlicing(nFlags, x, y);

			}
			else
			{

				

			}

		}else
		if (nFlags & MK_CONTROL)//if control key is down
		{

		}
		else//no shift or control key pressed
		{
			if (InsideCube(x,y) && m_currentState == ST_SLICE)
			{
				long centerX = x;//GetWidth()/2;//x
				long centerY = y;//GetHeight()/2;//y
				//find three points in the plane
                UpdateLinkingPoints();
				//m_p1 = ScreenToModel(centerX,centerY);
				//m_p2 = ScreenToModel(centerX-3, centerY);
				//m_p3 = ScreenToModel(centerX-3, centerY-3);
			}

			SetSlicing(true);
			switch (m_currentMode)
			{
				case MODE_SLICER:
					StartSlicing(nFlags, x, y);
					break;
				case MODE_MEASURE:
					AddMeasurePoint(x,y);
					break;
				case MODE_ADD_POINT:
					AddPoint(x, y);
					break;
				case MODE_DELETE_POINT:
					DeletePoint(x, y);
					break;
				case MODE_MOVE_POINT:
					MovePoint(x, y);
					break;
				default:
					break;  
			}
		}
	}//if (nFlags & MK_LBUTTON)//left button down


	else
	if (nFlags & MK_RBUTTON)//right button down
	{
		m_previousRightPointX = x;
		m_previousRightPointY = y;

		if (nFlags & MK_SHIFT)//if shift key is down
		{

		}else
		if (nFlags & MK_CONTROL)//if control key is down
		{

		}else//no shift or control key pressed
		{
			SetMode(MODE_SLICER);
		}
	}
}

void RRI_SlicerInterface:: MouseUpEvent(UINT nFlags, long x, long y)
{
	if (InsideCube(x,y) && nFlags & MK_LBUTTON)
	{
		long centerX = x;//GetWidth()/2;//x
		long centerY = y;//GetHeight()/2;//y
		//find three points in the plane
        UpdateLinkingPoints();
		//m_p1 = ScreenToModel(centerX,centerY);
		//m_p2 = ScreenToModel(centerX-3, centerY);
		//m_p3 = ScreenToModel(centerX-3, centerY-3);
	}
	else
	{
			
	}

	if (m_isPainting || m_isErasing || m_isEditing)
	{
		m_isDroppingPoints = false;
	}


	SetSlicing(false);
    SetPanning(false);

	m_previousLeftPointX = -1;
	m_previousLeftPointY = -1;

    switch (m_currentMode)
    {
        case MODE_SLICER:
            StopSlicing(nFlags, x, y);
            break;
      
        default:
            break;
    }


}


void RRI_SlicerInterface:: MouseMoveEvent(UINT nFlags, long x, long y)
{
    if (!m_slicingEnabled) return;


	bool insideCube = InsideCube(x, y);

	m_currentCursorX = x;
	m_currentCursorY = y;
	
#ifdef DISPLAY_DICOM_CONVERTED_POINT
	if (InsideCube(x,y))
	{
		//display converted point
		Vector3 dicomPoint = helperTransformPointDICOM(m_currentPosition);
		//vtkstd::string windowLevelText = "hello";//window.ToString();//"Window: " + window.ToString() + "Level: " + level.ToString();
		std::ostringstream stringStream;
		stringStream << "X: ";
		stringStream << dicomPoint.x;
		stringStream << " Y: ";
		stringStream << dicomPoint.y;
		stringStream << " Z: ";
		stringStream << dicomPoint.z;
		std::string currentPositionText = stringStream.str();
						
		long size = currentPositionText.size();
		(SetCurrentPositionText&currentPositionText[0], size);
						
	}
#endif

	if (insideCube)
	{
		if (nFlags & MK_LBUTTON)
		{
			m_currentPosition = ScreenToModel(x, y);
			long centerX = x;//GetWidth()/2;//x
			long centerY = y;//GetHeight()/2;//y
			//find three points in the plane
            UpdateLinkingPoints();
			//m_p1 = ScreenToModel(centerX, centerY);
           // m_p2 = ScreenToModel(centerX-3, centerY);
            //m_p3 = ScreenToModel(centerX-3, centerY-3);
            std::ostringstream stringStream;
            stringStream << "X: ";
            stringStream << m_p1.x;
            stringStream << " Y: ";
            stringStream << m_p1.y;
            stringStream << " Z: ";
            stringStream << m_p1.z;
            std::string currentPositionText = stringStream.str();
            long size = currentPositionText.size();
            //SetCurrentPositionText((char*)(currentPositionText.c_str()), size);

		}
						
	}

	

    //Here, we are painting into the binary cube for segmentation purposes
	if (insideCube && (m_isPainting || m_isErasing || m_isEditing) && (nFlags & MK_LBUTTON))//paint into binary cube current brush size
    {
        if (m_isPainting || m_isEditing)
        {
			Vector3 point;
			point.x = x;
			point.y = y;
			m_paintVector.push_back(point);
			//PaintIntoBinaryCube(x, y, true);
        }
        else
        if (m_isErasing)
        {
			Vector3 point;
			point.x = x;
			point.y = y;
			m_eraseVector.push_back(point);
			//PaintIntoBinaryCube(x, y, false);
        }
    }

    //ROTATE surfaces
    else
	if (m_isRotatingSurfaces && (nFlags & MK_LBUTTON))//left button down)
	{
		if (m_previousLeftPointX == -1 && m_previousLeftPointY == -1)
		{
			m_previousLeftPointX = x;
			m_previousLeftPointY = y;
		}
		else
		{
			Vector3 start = ScreenToModel(m_previousLeftPointX, m_previousLeftPointY);
			Vector3 end = ScreenToModel(x, y);

			float diffx = m_previousLeftPointX - x;
            float diffy = m_previousLeftPointY - y;

			float shiftX = diffx;
            float shiftY = diffy;

			if (start.y < 0)
			{
				shiftY = -diffy;
			}
			else
			{
				shiftX = diffx;
			}


#ifdef VTK_AVAILABLE
            //
            if (m_transformingNeedles)
            {
                RotateSurfaces(1, shiftY);
                RotateSurfaces(2, shiftX);
            }
            else
            {
                RotateSurfaces(m_rotatingSurfacesMode, shiftX);
            }
            

#endif
			m_previousLeftPointX = x;
			m_previousLeftPointY = y;
		}

	}
	else
	if (m_isShiftingSurfaces && (nFlags & MK_LBUTTON))
	{
		if (m_previousLeftPointX == -1 && m_previousLeftPointY == -1)
		{
			m_previousLeftPointX = x;
			m_previousLeftPointY = y;
		}
		else
		{
			Vector3 start = ScreenToModel(m_previousLeftPointX, m_previousLeftPointY);
			Vector3 end = ScreenToModel(x, y);

			float xShift = start.x - end.x;
			float yShift = start.y - end.y;
			float zShift = start.z - end.z;

#ifdef VTK_AVAILABLE
			ShiftSurfaces(xShift, yShift, zShift);
#endif

			m_previousLeftPointX = x;
			m_previousLeftPointY = y;
		}
	}
	else
	if (nFlags & MK_LBUTTON)//left button down
	{

		if (nFlags & MK_SHIFT)//if shift key is down
		{
			if (insideCube)
			{
				SetPanning(true);
				ContinueSlicing(nFlags, x, y);
			}
			else
			{
				

			}

		}else
		if (nFlags & MK_CONTROL)//if control key is down
		{
			if (-1 != m_previousLeftPointX && -1 != m_previousLeftPointY)
			{
				float zoomAmount = 0.05f;

				long diffX = x - m_previousLeftPointX;
				long diffY = y - m_previousLeftPointY;

                if (diffX > 0)
                {
                    Vector3 normal(0, 0, -10);
                    GetVisualizer()->PureRotate(normal, -1.0);
                    //Zoom(1.0 -zoomAmount);
                }
                else
                {
                    Vector3 normal(0, 0, -10);
                    GetVisualizer()->PureRotate(normal, 1.0);
                    //Zoom(1.0 + zoomAmount);
                }

                if (diffY > 0)
                {
                    Vector3 normal(0, 0, -10);
                    GetVisualizer()->PureRotate(normal, -1.0);
                    //Zoom(1.0 -zoomAmount);
                }
                else
                {
                    Vector3 normal(0, 0, -10);
                    GetVisualizer()->PureRotate(normal, 1.0);
                    //Zoom(1.0 + zoomAmount);
                }
					

				//***UpdateDisplay();
			}

			m_previousLeftPointX = x;
			m_previousLeftPointY = y;

		}
		else
		{
			switch (m_currentMode)
			{
				case MODE_SLICER:
			
					if (m_oldPositionX == -1 && m_oldPositionY == -1)
					{
						m_oldPositionX = x;
						m_oldPositionY = y;
					}
					else
					if (m_oldPositionX != x || m_oldPositionY != y)
					{
						ContinueSlicing(nFlags, x, y);//renders visualizer
						m_oldPositionX = x;
						m_oldPositionY = y;
					}
					break;
    
				default:
					break;
			}

		}
	}

	else
	if (nFlags & MK_RBUTTON)//right button down
	{
		if (insideCube && (m_isPainting || m_isEditing) && (nFlags & MK_RBUTTON))//paint into binary cube current brush size
        {
			//PaintIntoBinaryCube(x, y, false);
			Vector3 point;
			point.x = x;
			point.y = y;
			m_eraseVector.push_back(point);
        }
        else
		//if (nFlags & MK_SHIFT)//if shift key is down
		{
			if (insideCube)
			{

				{
					long window, level;
					window = m_window;
					level =  m_level;
					long windowMin = m_windowMin;
					long windowMax = m_windowMax;
					long windowRange = m_windowMax - m_windowMin;
					long windowMid = (windowRange)/2 + m_windowMin;

					if (-1 != m_previousRightPointX && -1 != m_previousRightPointY)
					{
						long diffX = x - m_previousRightPointX;//window range
						long diffY = y - m_previousRightPointY;//level (mid range)
		

						if (GetScanGeometry() == SG_Linear16)
						{
							//calculate new window range
							windowRange += diffX*10;
							if (windowRange < 20) windowRange = 20;
							if (windowRange > 8000) windowRange = 8000;

							//calculate new level (mid range)
							windowMid  += diffY*10;

							m_windowMin = windowMid - (windowRange/2);
							m_windowMax = windowMid + (windowRange/2);
							if (m_windowMin < 0) m_windowMin = 0;
							if (m_windowMax > 8000) m_windowMax = 8000;

							windowRange = m_windowMax - m_windowMin;
							windowMid = (windowRange)/2 + m_windowMin;
	

							//vtkstd::string windowLevelText = "hello";//window.ToString();//"Window: " + window.ToString() + "Level: " + level.ToString();
							std::ostringstream stringStream;
							stringStream << "Window: ";
							stringStream << windowRange;
							stringStream << "  Level: ";
							stringStream << windowMid;
							std::string windowLevelText = stringStream.str();
						
							long size = windowLevelText.size();
							SetWindowLevelText(&windowLevelText[0], size);

							SetWindowMinMax(m_windowMin, m_windowMax);

							m_window = windowRange;
							m_level = windowMid;

						}
						else
						{
							window -= diffX;
							level -= diffY;
							if (window < 0) window = 0;
							if (window > 255) window = 255;
							if (level < 0) level = 0;
							if (level > 255) level = 255;
							SetWindowLevel(window, level);
		
						}

            
					}				
				}
				m_previousRightPointX = x;
				m_previousRightPointY = y;

			}
		}
	}
	else
	{
		//no mouse buttons pressed
		//keep track of current mouse position for rubber band functionality
		m_currentX = x;
		m_currentY = y;
	}

}

void RRI_SlicerInterface::SetWindowMinMax(long windowMin, long windowMax)
{
	if (GetScanGeometry() == SG_Linear16)//MRI or CT
	{
		GetVisualizer()->SetLeftRight(windowMin, windowMax);
		m_windowMin = windowMin;
		m_windowMax = windowMax;
		long windowRange = m_windowMax - m_windowMin;
		long windowMid = windowRange/2 + m_windowMin;
		
		std::ostringstream stringStream;
		stringStream << "Window: ";
		stringStream << windowRange;
		stringStream << "  Level: ";
		stringStream << windowMid;
		std::string windowLevelText = stringStream.str();
		
						
		long size = windowLevelText.size();
		SetWindowLevelText(&windowLevelText[0], size);
	}
}

void RRI_SlicerInterface::SetWindowLevel(long window, long level)
{
	if (GetScanGeometry()==SG_LinearRaw || GetScanGeometry()==SG_AxialNoOffsets || GetScanGeometry()==SG_Fan || GetScanGeometry()==SG_Hybrid)
	{
		m_window = window;
		m_level = level;
		if (m_window < 0) m_window = 255;//reset window to default
		if (m_window > 255) m_window = 255;
		if (m_level < 0) m_level = 128;//reset level to default
		if (m_level > 255) m_level = 128;
		m_graphics->SetWindowLevel(window, level);
		
	}
	
	std::ostringstream stringStream;
	stringStream << "Window: ";
	stringStream << m_window;
	stringStream << "  Level: ";
	stringStream << m_level;
	std::string windowLevelText = stringStream.str();
						
	long size = windowLevelText.size();
	SetWindowLevelText(&windowLevelText[0], size);

}

void RRI_SlicerInterface::ClearTargetSurfaces()
{
	m_target_surface.clear();
}

void RRI_SlicerInterface::ClearTargets()
{
	m_userTargets.clear();
	m_target_surface.clear();
}


long RRI_SlicerInterface::AddTargetSurface(Vector3 target, float radius, long red, long green, long blue)
{

#ifdef VTK_AVAILABLE


    vtkSphereSource *sphere = vtkSphereSource::New();
    sphere->SetRadius(radius);
    sphere->SetCenter(target.x, target.y, target.z);
    sphere->SetThetaResolution(22);//100
    sphere->SetPhiResolution(22);//22
    sphere->Update();

    vtkPolyData *points;// = vtkPolyData::New();
    points = (sphere->GetOutput());

    vtkCellArray *triPolys = points->GetPolys();
    vtkPoints *triPoints = points->GetPoints();

    

    long count = 0;
	//int npts, *pts;
    vtkIdType npts, *pts;
	double vert[3];

    float corners[3][3];
	Triangle triangle;

//Error	33	error C2664: 'int vtkCellArray::GetNextCell(vtkIdList *)' : cannot convert argument 1 from 'int' to 'vtkIdType &'	E:\1-Projects_2013\0-SHIFT Workstation Projects\SHIFT-64\RRI_Libs\RRI_Slicer\RRI_SlicerInterface.cpp	2308	1	SHIFT64

    for (triPolys->InitTraversal(); triPolys->GetNextCell(npts,pts); count++)
	{ 
		for (long loopIn=0; loopIn<npts; loopIn++)
		{
			
			triPoints->GetPoint(pts[loopIn], vert);
			corners[loopIn][0] = vert[0];
			corners[loopIn][1] = vert[1];
			corners[loopIn][2] = vert[2];
		
		}

        triangle.V1.x = corners[0][0];
		triangle.V1.y = corners[0][1];
		triangle.V1.z = corners[0][2];
		
		triangle.V2.x = corners[1][0];
		triangle.V2.y = corners[1][1];
		triangle.V2.z = corners[1][2];
		
		triangle.V3.x = corners[2][0];
		triangle.V3.y = corners[2][1];
		triangle.V3.z = corners[2][2];
		
		
		triangle.N1.x = 1;
		triangle.N1.y = 1;
		triangle.N1.z = 1;
		
		triangle.N2.x = 1;
		triangle.N2.y = 1;
		triangle.N2.z = 1;
		
		triangle.N3.x = 1;
		triangle.N3.y = 1;
		triangle.N3.z = 1;

        //triangle.contour = m_numberOfTargets;
        

        m_target_surface.push_back(triangle);	
		
	}

    //m_numberOfTargets++;

    sphere->Delete();

#endif//VTK_AVAILABLES
    return 0;
}

long RRI_SlicerInterface::FindClosestPoint(Vector3Vec* points, long x, long y)
{
	if (!points) return -1;
	
	long index = -1;
	float minDistance = 10000;
	Vector3 point3D = ScreenToModel(x, y);

	for (int i=0; i<points->size(); i++)
	{
		Vector3 point = points->at(i);
		float distance = sqrt((point3D.x-point.x)*(point3D.x-point.x) 
                            + (point3D.y-point.y)*(point3D.y-point.y)
                            + (point3D.z-point.z)*(point3D.z-point.z));

		if (distance < minDistance)
		{
			minDistance = distance;
			index = i;
		}


	}

	return index;
}

#ifdef VTK_AVAILABLE
void RRI_SlicerInterface::Test()
{
	/*
	double realStart = 0.0;
	double imagStart = 0.0;
	double realConst = 0.0;
	double imagConst = 0.0;

	double maxOut = 2;

	double out_R, out_I, out_J, out_K;

	long maxIterations = 100;

	double in_R = 0.0;
	double in_I = 0.0;
	double in_J = 0.0;
	double in_K = 0.0;

	double c_R = 0.23782;
	double c_I = 0.2837;
	double c_J = 0.38473;
	double c_K = 0.238782;

	Triangle tri;
	TriangleVec triangles;

	bool done = false;
	long count = 0;
	while(!done)
	{
		count++;

		out_R = (in_R * in_R) - in_I*in_I + in_J*in_J - in_K*in_K;
		out_I = (in_I*in_R*2.0);
		out_J = (in_J*in_R*2.0);
		out_K = (in_K*in_R*2.0);

		out_R += c_R;
		out_I += c_I;
		out_J += c_J;
		out_K += c_K;

		
		in_R = out_R;//for next time
		in_I = out_I;//for next time
		in_J = out_J;//for next time
		in_K = out_K;//for next time

		if (count <= 3)
		{
			if (count==1)
			{
				tri.V1.x = out_R;
				tri.V1.y = out_I;
				tri.V1.z = out_J;
			}
			else
			if (count==2)
			{
				tri.V2.x = out_R;
				tri.V2.y = out_I;
				tri.V2.z = out_J;
			}
			else
			if (count==3)
			{
				tri.V3.x = out_R;
				tri.V3.y = out_I;
				tri.V3.z = out_J;

				triangles.push_back(tri);
			}

		}
		else
		{
			tri.V1.x = tri.V2.x;
			tri.V1.y = tri.V2.y;
			tri.V1.z = tri.V2.z;

			tri.V2.x = tri.V3.x;
			tri.V2.y = tri.V3.y;
			tri.V2.z = tri.V3.z;

			tri.V3.x = out_R;
			tri.V3.y = out_I;
			tri.V3.z = out_J;

			triangles.push_back(tri);
		}
		

		if ( sqrt(out_R*out_R + out_I*out_I) > maxOut)
		{
			done = true;
		}
		
		if (count >= maxIterations)
		{
			done = true;
		}

	}


	vtkPolyData* polyData = vtkPolyData::New();
	helperConvertTrianglesToPolyData(&triangles,polyData);
    //export surface in.vtk format
    vtkPolyDataWriter* polyWriter = vtkPolyDataWriter::New();
    polyWriter->SetInput(polyData);
	polyWriter->SetFileName("c:/FractalData/vtkTest.vtk");
	polyWriter->SetFileTypeToASCII();
	polyWriter->Write();
    polyWriter->Update();
    polyWriter->Delete();
	polyData->Delete();
	*/

}

float RRI_SlicerInterface::HelperFindDistanceFromPointToLine(Vector3 startLine, Vector3 endLine, Vector3 point)
{

    //AB = <x2-x1, y2-y1, z2-z1> = <x3, y3, z3>
    //AP = < x-x1,  y-y1, z-z1 > = <x4, y4, z4>
    Vector3 AB, AP;

    //(x3, y3, z3)
    AB.x = endLine.x - startLine.x; 
    AB.y = endLine.y - startLine.y;
    AB.z = endLine.z - startLine.z;

    //(x4, y4, z4)
    AP.x = point.x - startLine.x; 
    AP.y = point.y - startLine.y;
    AP.z = point.z - startLine.z;

    //AB X AP = < y3*z4 - z3*y4, -(x3*z4 - x4*z3), x3*y4 - x4*y3 >

    Vector3 ABxAP;
    ABxAP.x = AB.y*AP.z - AB.z*AP.y;
    ABxAP.y = -(AB.x*AP.z - AP.x*AB.z);
    ABxAP.z = AB.x*AP.y - AP.x*AB.y;

    //Let's let (a) be the length of AB X AP.  Then
    //a = Sqrt{ (y3*z4 - z3*y4)^2 + (x3*z4 - x4*z3)^2 + (x3*y4 - x4*y3)^2 }

    float a = sqrt( pow((AB.y*AP.z - AB.z*AP.y),2) + pow((AB.x*AP.z - AP.x*AB.z),2) + pow((AB.x*AP.y - AP.x*AB.y),2));

    //If you divide (a) by the distance AB you will get the distance of P 
    //from line AB. Distance AB can be found using the distance formula as,
    //AB = square root of (x3^2 + y3^2 + z3^2)
    //Thus the distance we are looking for is a/AB.
    float distAB = sqrt(AB.x*AB.x + AB.y*AB.y + AB.z*AB.z);



    float distance = a/distAB;

    return distance;
}


//NOTE: this only works when visualizer has 3 channels
long RRI_SlicerInterface::ColourizePolydata(vtkPolyData* polyData, double radius)
{
    if (GetNumberOfChannels() != 3) return -1;
	
	unsigned char* buffer = GetBuffer();//get 24 bit buffer
	Vector3 cubeSize = GetVisualizer()->GetCubeSize();
	Vector3 cubeDim = GetVisualizer()->GetCubeDim();
	double stepX = cubeSize.x / cubeDim.x;
	double stepY = cubeSize.y / cubeDim.y;
	double stepZ = cubeSize.z / cubeDim.z;
	
	// Generate the colors for each point based on the color map
	vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	long numberOfPoints = polyData->GetNumberOfPoints();
	for(int i = 0; i < numberOfPoints; i++)
    {
		
		double point[3];
		polyData->GetPoint(i,point);
 
		unsigned char color[3];
		//convert from world to array coordinates
		long indexX = (point[0] + cubeSize.x/2.0)/stepX;
		long indexY = (point[1] + cubeSize.y/2.0)/stepY;
		long indexZ = (point[2] + cubeSize.z/2.0)/stepZ;

		//find index into buffer
		long bufferIndex = indexZ*cubeDim.x*cubeDim.y*3 + indexY*cubeDim.x*3 + indexX*3;

		color[0] = buffer[bufferIndex+0];
		color[1] = buffer[bufferIndex+1];
		color[2] = buffer[bufferIndex+2];
 
		colors->InsertNextTupleValue(color);
 
    }

	polyData->GetPointData()->SetScalars(colors);
	

    return 0;


}


long RRI_SlicerInterface::helperConvertTrianglesToPolyData(TriangleVec* triangles, vtkPolyData* polyData, bool useDirectionCosines)
{

    if (!triangles) return -1;
    if (!polyData) return -2;

    vtkPoints       *pointsSource =  vtkPoints::New();
	vtkCellArray    *polysSource   = vtkCellArray::New();
    vtkPolyData     *tempPoly =      vtkPolyData::New();

	Triangle triangle;
	long pointNumber = 0;
	long sizeSource = triangles->size();
	for(long i = 0; i < sizeSource; i++)
	{	
		triangle = triangles->at(i);

		if (useDirectionCosines)
		{
			triangle.V1 = helperTransformPointDICOM(triangle.V1);
			triangle.V2 = helperTransformPointDICOM(triangle.V2);
			triangle.V3 = helperTransformPointDICOM(triangle.V3);
		}

		polysSource->InsertNextCell(3);
		pointsSource->InsertPoint(pointNumber, triangle.V1.x, triangle.V1.y, triangle.V1.z);
		polysSource->InsertCellPoint(pointNumber++);
		pointsSource->InsertPoint(pointNumber, triangle.V2.x, triangle.V2.y, triangle.V2.z);
		polysSource->InsertCellPoint(pointNumber++);
		pointsSource->InsertPoint(pointNumber, triangle.V3.x, triangle.V3.y, triangle.V3.z); 
		polysSource->InsertCellPoint(pointNumber++);

        
	}

    tempPoly->SetPoints(pointsSource);
	tempPoly->SetPolys(polysSource);

    //clean polydata
    vtkCleanPolyData *clean = vtkCleanPolyData::New();
	clean->SetInput(tempPoly);
	clean->ConvertPolysToLinesOff(); 
	clean->ConvertLinesToPointsOff(); 


    //generate normals
    vtkPolyDataNormals *norm = vtkPolyDataNormals::New();
	norm->SetFeatureAngle(60);
    norm->SplittingOff();
    norm->SetInput( clean->GetOutput());
    norm->Update();

    //return polydata with normals
    vtkPolyData* temp = norm->GetOutput();
    polyData->DeepCopy(temp);
//-----------------------------------------------------
//clean up scalars and create appropriate lookup table
//-----------------------------------------------------

    vtkLookupTable* lookup = vtkLookupTable::New();
    lookup->SetTableRange(0,255);
    lookup->SetHueRange(0.7, 0.1);
    lookup->SetSaturationRange(0,0.1);
    lookup->SetValueRange(1,1);
    lookup->Build();
 
    if (polyData->GetPointData()->GetScalars())
    {
        polyData->GetPointData()->GetScalars()->SetLookupTable(lookup);

        float value[3];
        value[0] = 128;
        value[1] = 128;
        value[2] = 128;


        vtkDataArray* dataArray1 = polyData->GetPointData()->GetScalars();
        int numberOfTuples = dataArray1->GetNumberOfTuples();
        {
            for (int k=0; k<numberOfTuples; k++)
            {
                dataArray1->SetTuple(k, value);
            }
        }
    }

//-----------------------------------------------------


    
  
    lookup->Delete();
    tempPoly->Delete();
    pointsSource->Delete();
    polysSource->Delete();
    norm->Delete();
    clean->Delete();


    return 0;
}

//***takes the triangles from a needle model and orients the needle to the angle represented by
//the two points, tip and entry and places the needle tip at the user selected tip point.
void RRI_SlicerInterface::AddSegmentedNeedle(SurfaceObject* so, Vector3 tip, Vector3 entry)
{

	Vector3 needleBegin = tip;//m_View1->GetSlicer()->extend_line(tip, entry, 50);
	Vector3 needleEnd = entry;//m_View1->GetSlicer()->extend_line(entry, tip, -50);


	//calculate needle length
	double distanceBetweenPoints = helperDistanceBetweenTwoPoints(needleBegin,needleEnd);
	vtkPolyData* needleData = vtkPolyData::New();

#define USING_VTK_MODEL
#ifdef USING_VTK_MODEL
	//load needle model
	std::string filePath = "c:/MRPingResources/needle.STL";
	vtkSTLReader*  surfaceReader = vtkSTLReader::New();
	surfaceReader->SetFileName(filePath.c_str());
	surfaceReader->Update();
    needleData->DeepCopy(surfaceReader->GetOutput());
#endif

#ifndef USING_VTK_MODEL
	vtkCylinderSource* needle = vtkCylinderSource::New();
	needle->SetCenter(0.0, 0.0, 0.0);
	needle->SetRadius(5.0);
	needle->SetHeight(distanceBetweenPoints);
	needle->SetResolution(100);
	needle->Update();
	//needleData->DeepCopy(needle->GetOutput());
#endif

    //get needle center
    Vector3 center;
    center.x = (needleBegin.x + needleEnd.x)/2.0;
    center.y = (needleBegin.y + needleEnd.y)/2.0;
    center.z = (needleBegin.z + needleEnd.z)/2.0;

    //calculate needle length
    float length  = distanceBetweenPoints;//sqrt(lengthX*lengthX + lengthY*lengthY + lengthZ*lengthZ);

    //map from default orientation to cylindar orientation
    double* temp1 = new double[3];
    double* temp2 = new double[3];
    temp1[0] = 0.0;
    temp1[1] = 0.0;
    temp1[2] = 0.0;
    temp2[0] = 0.0;
    temp2[1] = 0.0;
    temp2[2] = -length;
   
    vtkPoints*  sourcePoints = vtkPoints::New();
	vtkPoints*  targetPoints = vtkPoints::New();

    //use landmark transform to transform model to the correct needle orientation
    vtkLandmarkTransform* trans = vtkLandmarkTransform::New();
    sourcePoints->InsertNextPoint(temp1[0], temp1[1], temp1[2]);
    sourcePoints->InsertNextPoint(temp2[0], temp2[1], temp2[2]);
    targetPoints->InsertNextPoint(needleBegin.x, needleBegin.y, needleBegin.z);//US
    targetPoints->InsertNextPoint(needleEnd.x, needleEnd.y, needleEnd.z);
    trans->SetSourceLandmarks(sourcePoints);	
	trans->SetTargetLandmarks(targetPoints);
	trans->SetModeToRigidBody();
    trans->Update();

 //now apply transform to cylindar
    vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
    filter->SetInput(needleData);
    filter->SetTransform(trans);
	filter->Update();

	//get transformed data out of transformation filter
    vtkPolyData* surfaceData = filter->GetOutput();


	
HelperConvertPolyDataToTriangles(surfaceData, &(so->triangles), true);//clear old triangles
ClearTargets();

	
	

    delete [] temp1;
    delete [] temp2;

    sourcePoints->Delete();
    targetPoints->Delete();
    trans->Delete();
    filter->Delete();
	surfaceReader->Delete();
   
}

Vector3 RRI_SlicerInterface::helperFindCentroid(TriangleVec* triangles)
{
    Vector3 centroid(0,0,0);
 
//generate source polydata

	Triangle triangle;

    for (int i=0; i<triangles->size(); i++)
    {
        triangle = triangles->at(i);
        centroid = centroid + triangle.V1 + triangle.V2 + triangle.V3;

    }   
    

    long sizeSource = triangles->size();
    centroid = centroid / (sizeSource * 3);

    return centroid;


}



float RRI_SlicerInterface::CalculateVolume(TriangleVec *triangles)
{
    vtkPolyData *data = vtkPolyData::New();

    helperConvertTrianglesToPolyData(triangles, data, false);
	
    //calculate volume of prostate from surface
    vtkMassProperties* massProperties = vtkMassProperties::New();
    massProperties->SetInput(data);
    float volume = massProperties->GetVolume() / 1000.0;
	massProperties->Delete();

    data->Delete();

    return volume;
}



void RRI_SlicerInterface::BuildCellLocatorsForPlanes()
{
	SurfaceList* surfaceList = GetSurfaceList();

	for (int i=0; i<surfaceList->size(); i++)
	{
		SurfaceObject so = surfaceList->at(i);
		TriangleVec* tri = &(so.triangles);
		vtkPolyData* polyData = vtkPolyData::New();
		HelperConvertTrianglesToPolyData(tri, polyData, false);


		if (so.name == "MID-BASE")
		{
			if (MID_BASE_locator) MID_BASE_locator->Delete();
			MID_BASE_locator = vtkCellLocator::New();
			MID_BASE_locator->SetDataSet(polyData);
			MID_BASE_locator->BuildLocator();
		}
		else
		if (so.name == "MID-APEX")
		{
			if (MID_APEX_locator) MID_APEX_locator->Delete();
			MID_APEX_locator = vtkCellLocator::New();
			MID_APEX_locator->SetDataSet(polyData);
			MID_APEX_locator->BuildLocator();
		}
		else
		if (so.name == "CENTER-HORIZONTAL")
		{
			if (CENTER_HORIZONTAL_locator) CENTER_HORIZONTAL_locator->Delete();
			CENTER_HORIZONTAL_locator = vtkCellLocator::New();
			CENTER_HORIZONTAL_locator->SetDataSet(polyData);
			CENTER_HORIZONTAL_locator->BuildLocator();
		}
		else
		if (so.name == "CENTER-VERTICAL")
		{
			if (CENTER_VERTICAL_locator) CENTER_VERTICAL_locator->Delete();
			CENTER_VERTICAL_locator = vtkCellLocator::New();
			CENTER_VERTICAL_locator->SetDataSet(polyData);
			CENTER_VERTICAL_locator->BuildLocator();
		}
		else
		if (so.name == "BASE-LEFT")
		{
			if (BASE_LEFT_locator) BASE_LEFT_locator->Delete();
			BASE_LEFT_locator = vtkCellLocator::New();
			BASE_LEFT_locator->SetDataSet(polyData);
			BASE_LEFT_locator->BuildLocator();
		}
	    else
		if (so.name == "BASE-RIGHT")
		{
			if (BASE_RIGHT_locator) BASE_RIGHT_locator->Delete();
			BASE_RIGHT_locator = vtkCellLocator::New();
			BASE_RIGHT_locator->SetDataSet(polyData);
			BASE_RIGHT_locator->BuildLocator();
		}
		else
		if (so.name == "MID-LEFT")
		{
			if (MID_LEFT_locator) MID_LEFT_locator->Delete();
			MID_LEFT_locator = vtkCellLocator::New();
			MID_LEFT_locator->SetDataSet(polyData);
			MID_LEFT_locator->BuildLocator();
			
		}
		else
		if (so.name == "MID-RIGHT")
		{
			if (MID_RIGHT_locator) MID_RIGHT_locator->Delete();
			MID_RIGHT_locator = vtkCellLocator::New();
			MID_RIGHT_locator->SetDataSet(polyData);
			MID_RIGHT_locator->BuildLocator();
		}
		else
		if (so.name == "APEX-LEFT")
		{
			if (APEX_LEFT_locator) MID_LEFT_locator->Delete();
			APEX_LEFT_locator = vtkCellLocator::New();
			APEX_LEFT_locator->SetDataSet(polyData);
			APEX_LEFT_locator->BuildLocator();
		}
		else
		if (so.name == "APEX-RIGHT")
		{
			if (APEX_RIGHT_locator) APEX_RIGHT_locator->Delete();
			APEX_RIGHT_locator = vtkCellLocator::New();
			APEX_RIGHT_locator->SetDataSet(polyData);
			APEX_RIGHT_locator->BuildLocator();
			
		}

		polyData->Delete();
	}

}

//given two user points, add two perpendicular planes.
//one along the line specified by the two points
//one perpendicular to the first plane
long RRI_SlicerInterface::AddPlanes()
{
	if (m_userTargets.size() != 8) 
	{

		return -1;
	}	

	Vector3 cubeSize = GetVisualizer()->GetCubeSize();
    float imageSizeX = cubeSize.x;
    float imageSizeY = cubeSize.y;
	float imageSizeZ = cubeSize.z;

	Vector3 point1 = m_userTargets.at(0);
	Vector3 point2 = m_userTargets.at(1);
	Vector3 point3 = m_userTargets.at(2);
	Vector3 point4 = m_userTargets.at(3);
	Vector3 point5 = m_userTargets.at(4);
	Vector3 point6 = m_userTargets.at(5);
	Vector3 point7 = m_userTargets.at(6);
	Vector3 point8 = m_userTargets.at(7);


	
	{

		//Add MID to BASE plane
		{
			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			
			transform->Translate(0.0, 0.0, point1.z);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "MID-BASE";
			so.objectType = 22;
			so.colour = RGB(0, 255, 255);//red
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			
			plane->Delete();
			transform->Delete();
			filter->Delete();									
		}
	
		//Add MID to APEX plane
		{
			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	

			transform->Translate(0.0, 0.0, point5.z);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "MID-APEX";
			so.objectType = 22;
			so.colour = RGB(0, 255, 255);
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			
			plane->Delete();
			transform->Delete();
			filter->Delete();									
		}

	}
	

	//add horizontal and vertical center planes
	{
		//calculate z-axis rotation
		double xLength = abs(point3.x - point1.x);
		double yLength = abs(point3.y - point1.y);
		double cLength = sqrt(xLength*xLength + yLength*yLength);
		double angle = sin(yLength/xLength) / PI * 180.0;

		if (point1.y > point3.y)
			angle *= -1.0;

		Vector3 center;
		center.x = (point1.x + point3.x)/2.0;
		center.y = (point1.y + point3.y)/2.0;
		center.z = (point1.z + point3.z)/2.0;

		//Add central horizontal plane
		{
			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			transform->RotateX(90);//verticle
			transform->RotateZ(angle);
			transform->Translate(center.x, center.y, 0);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "CENTER-HORIZONTAL";
			so.objectType = 22;
			so.colour = RGB(255, 0, 0);//red
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			
			plane->Delete();
			transform->Delete();
			filter->Delete();				
					
		}
	
		//Add central vertical plane
		{
			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			transform->RotateY(90);//verticle
			transform->RotateZ(angle);
			transform->Translate(center.x, center.y, 0);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "CENTER-VERTICAL";
			so.objectType = 22;
			so.colour = RGB(128, 255, 0);//yellow
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			

			plane->Delete();
			transform->Delete();
			filter->Delete();				

			
		}

	}
	
	//add left and right base zones
	{
		//Add patient left base zone
		{
			//calculate z-axis rotation
			double xLength = abs(point4.x - point3.x);
			double yLength = abs(point4.y - point3.y);
			double cLength = sqrt(xLength*xLength + yLength*yLength);
			double angle = sin(xLength/yLength) / PI * 180.0;

			if (point4.x > point3.x)
				angle *= -1.0;

			Vector3 center;
			center.x = (point4.x + point3.x)/2.0;
			center.y = (point4.y + point3.y)/2.0;
			center.z = (point4.z + point3.z)/2.0;

			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			transform->RotateY(90);//verticle
			transform->RotateZ(angle);
			transform->Translate(center.x, center.y, 0);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "BASE-LEFT";
			so.objectType = 22;
			so.colour = RGB(128, 0, 155);//dark yellow
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			plane->Delete();
			transform->Delete();
			filter->Delete();			

			
		}

		//Add patient right mid zone
		{
			//calculate z-axis rotation
			double xLength = abs(point2.x - point1.x);
			double yLength = abs(point2.y - point1.y);
			double cLength = sqrt(xLength*xLength + yLength*yLength);
			double angle = sin(xLength/yLength) / PI * 180.0;

			if (point1.x < point2.x)
				angle *= -1.0;

			Vector3 center;
			center.x = (point1.x + point2.x)/2.0;
			center.y = (point1.y + point2.y)/2.0;
			center.z = (point1.z + point2.z)/2.0;

			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			transform->RotateY(90);//verticle
			transform->RotateZ(angle);
			transform->Translate(center.x, center.y, 0);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "BASE-RIGHT";
			so.objectType = 22;
			so.colour = RGB(128, 0, 155);//yellow
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			
			plane->Delete();
			transform->Delete();
			filter->Delete();	

			
		}


	}

	//add left and right mid zones
	{
		//Add patient left mid zone
		{
			//calculate z-axis rotation
			double xLength = abs(point4.x - point3.x);
			double yLength = abs(point4.y - point3.y);
			double cLength = sqrt(xLength*xLength + yLength*yLength);
			double angle = sin(xLength/yLength) / PI * 180.0;

			if (point4.x > point3.x)
				angle *= -1.0;

			Vector3 center;
			center.x = (point4.x + point3.x)/2.0;
			center.y = (point4.y + point3.y)/2.0;
			center.z = (point4.z + point3.z)/2.0;

			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			transform->RotateY(90);//verticle
			transform->RotateZ(angle);
			transform->Translate(center.x, center.y, 0);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "MID-LEFT";
			so.objectType = 22;
			so.colour = RGB(0, 255, 0);//red
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			plane->Delete();
			transform->Delete();
			filter->Delete();			

			
		}

		//Add patient right mid zone
		{
			//calculate z-axis rotation
			double xLength = abs(point2.x - point1.x);
			double yLength = abs(point2.y - point1.y);
			double cLength = sqrt(xLength*xLength + yLength*yLength);
			double angle = sin(xLength/yLength) / PI * 180.0;

			if (point1.x < point2.x)
				angle *= -1.0;

			Vector3 center;
			center.x = (point1.x + point2.x)/2.0;
			center.y = (point1.y + point2.y)/2.0;
			center.z = (point1.z + point2.z)/2.0;

			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			transform->RotateY(90);//verticle
			transform->RotateZ(angle);
			transform->Translate(center.x, center.y, 0);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "MID-RIGHT";
			so.objectType = 22;
			so.colour = RGB(0, 255, 0);//red
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			
			plane->Delete();
			transform->Delete();
			filter->Delete();	

			
		}


	}
	
	
	//add left and right apex zone
	{
		

		//Add patient right apex zone
		{
			//calculate z-axis rotation
			double xLength = abs(point6.x - point5.x);
			double yLength = abs(point6.y - point5.y);
			double cLength = sqrt(xLength*xLength + yLength*yLength);
			double angle = sin(xLength/yLength) / PI * 180.0;

			if (point6.x > point5.x)
				angle *= -1.0;

			Vector3 center;
			center.x = (point6.x + point5.x)/2.0;
			center.y = (point6.y + point5.y)/2.0;
			center.z = (point6.z + point5.z)/2.0;

			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			transform->RotateY(90);//verticle
			transform->RotateZ(angle);
			transform->Translate(center.x, center.y, 0);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "APEX-RIGHT";
			so.objectType = 22;
			so.colour = RGB(255, 0, 255);//red
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			plane->Delete();
			transform->Delete();
			filter->Delete();			

			
		}

		//Add patient left apex zone
		{
			//calculate z-axis rotation
			double xLength = abs(point8.x - point7.x);
			double yLength = abs(point8.y - point7.y);
			double cLength = sqrt(xLength*xLength + yLength*yLength);
			double angle = sin(xLength/yLength) / PI * 180.0;

			if (point7.x < point8.x)
				angle *= -1.0;

			Vector3 center;
			center.x = (point8.x + point7.x)/2.0;
			center.y = (point8.y + point7.y)/2.0;
			center.z = (point8.z + point7.z)/2.0;

			vtkPlaneSource  *plane = vtkPlaneSource::New();
			plane->SetResolution(GetVoxelX(), GetVoxelY());
			plane->SetOrigin(0, 0, 0);
			plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
			plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
			plane->SetCenter(0, 0, 0);

			vtkTransform* transform = vtkTransform::New();
			transform->PostMultiply();
	
			transform->RotateY(90);//verticle
			transform->RotateZ(angle);
			transform->Translate(center.x, center.y, 0);

			vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
			filter->SetInput(plane->GetOutput());
			filter->SetTransform(transform);
		
			//apply transform
			filter->Update();

			//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
			SurfaceObject so;
			so.name = "APEX-LEFT";
			so.objectType = 22;
			so.colour = RGB(255, 0, 255);//red
			so.opacity = 1.0;
			so.visible = true;

			//generate triangles for surface
			vtkPolyData* polyData = filter->GetOutput();
			vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
			long count = polyData->GetNumberOfVerts();

			double vert[3];
			Vector3 corner1, corner2, corner3, corner4;

			polyData->GetPoint(0, vert);
			corner1.x = vert[0];
			corner1.y = vert[1];
			corner1.z = vert[2];

			polyData->GetPoint(1, vert);
			corner2.x = vert[0];
			corner2.y = vert[1];
			corner2.z = vert[2];

			polyData->GetPoint(2, vert);
			corner3.x = vert[0];
			corner3.y = vert[1];
			corner3.z = vert[2];

			polyData->GetPoint(3, vert);
			corner4.x = vert[0];
			corner4.y = vert[1];
			corner4.z = vert[2];


			Triangle t1, t2;
			t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
			t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
			t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

			t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
			t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
			t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);
			so.triangles.push_back(t1);
			so.triangles.push_back(t2);

			m_surfaceList.push_back(so);

			
			
			plane->Delete();
			transform->Delete();
			filter->Delete();		

			

		}

		
	}

	ClearTargets();
	StopAddingPoints();

	

	return 0;

}

long RRI_SlicerInterface::GenerateCodesForUserPoints()
{
	for (int i=0; i<m_userTargets.size(); i++)
	{
		std::string code = GetCodeForPoint(m_userTargets.at(i));
	}

	return 0;
}

//long helperConvertTrianglesToPolyData(TriangleVec* triangles, vtkPolyData* polyData);
std::string RRI_SlicerInterface::GetCodeForPoint(Vector3 point)
{
	std::string returnCode = "";

//determine where in the volume this point is and return a string code
	//apex, mid, base
	bool apex = false;
	bool mid = false;
	bool base = false;
	bool patientLeft = false;
	bool patientRight = false;
	bool posterior = false;
	bool anterior = false;

	//determine patient left or patient right
	{
		vtkPolyData* pData = vtkPolyData::New();

		//find plane in surface list with label CENTER-VERTICAL
		SurfaceObject so;
		bool done = false;
		long index = 0;
		while (!done)
		{
			so = m_surfaceList.at(index);

			if (so.name == "CENTER-VERTICAL")
			{
				done = true;
			}
			else
			{
				index++;
			}

			if (index >= m_surfaceList.size())
			{
				return "Error";
			}
		}

		Vector3 extendedPoint1 = point;
		extendedPoint1.x -= 1000.0;

		double point1[] = {point.x, point.y, point.z};
		double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
        double t, ptline[3], pcoords[3];
        int subId;
        int result = CENTER_VERTICAL_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
		if (result == 1)
		{
			//the point is in patient left
			returnCode += "PLEFT";
			patientLeft = true;
			
		}
		else
		{
			//the point is in patient right
			//the point is in patient left
			returnCode += "PRIGHT";
			patientRight = true;
		}

		pData->Delete();
	}

	//patient posterior (bottom) or anterior (top)
	{
		vtkPolyData* pData = vtkPolyData::New();

		//find plane in surface list with label CENTER-HORIZONTAL
		SurfaceObject so;
		bool done = false;
		long index = 0;
		while (!done)
		{
			so = m_surfaceList.at(index);

			if (so.name == "CENTER-HORIZONTAL")
			{
				done = true;
			}
			else
			{
				index++;
			}

			if (index >= m_surfaceList.size())
			{
				return "Error";
			}
		}

		Vector3 extendedPoint1 = point;
		extendedPoint1.y -= 1000.0;

		double point1[] = {point.x, point.y, point.z};
		double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
        double t, ptline[3], pcoords[3];
        int subId;
        int result = CENTER_HORIZONTAL_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
		if (result == 1)
		{
			//the point is in patient left
			returnCode += "_POSTERIOR";
			posterior = true;
			
		}
		else
		{
			//the point is in patient right
			//the point is in patient left
			returnCode += "_ANTERIOR";
			anterior = true;
		}

		pData->Delete();

	}



	
	{
		
		//APEX?
		{
			vtkPolyData* pData = vtkPolyData::New();

			//find plane in surface list with label MID-APEX
			SurfaceObject so;
			bool done = false;
			long index = 0;
			while (!done)
			{
				so = m_surfaceList.at(index);

				if (so.name == "MID-APEX")
				{
					done = true;
				}
				else
				{
					index++;
				}

				if (index >= m_surfaceList.size())
				{
					return "Error";
				}
			}

			Vector3 extendedPoint1 = point;
			extendedPoint1.z -= 1000.0;

			double point1[] = {point.x, point.y, point.z};
			double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
			double t, ptline[3], pcoords[3];
			int subId;
			int result = MID_APEX_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
			if (result == 0)
			{
				//the point is in the apex
				returnCode += "_APEX";
				apex = true;
			
			}

			pData->Delete();
		}

		//MID-BASE
		{
			vtkPolyData* pData = vtkPolyData::New();

			//find plane in surface list with label MID-BASE
			SurfaceObject so;
			bool done = false;
			long index = 0;
			while (!done)
			{
				so = m_surfaceList.at(index);

				if (so.name == "MID-BASE")
				{
					done = true;
				}
				else
				{
					index++;
				}

				if (index >= m_surfaceList.size())
				{
					return "Error";
				}
			}

			Vector3 extendedPoint1 = point;
			extendedPoint1.z += 1000.0;

			double point1[] = {point.x, point.y, point.z};
			double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
			double t, ptline[3], pcoords[3];
			int subId;
			int result = MID_BASE_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
			if (result == 0)
			{
				returnCode += "_BASE";
				base = true;
			}

			pData->Delete();
		}

		//MID
		{
			if (!apex && !base)
			{
				returnCode += "_MID";
				mid = true;
			}

		}
	}

	//lateral - medial
	{
		if (apex)//use purple planes
		{
			if (patientLeft)
			{
				//use MID-LEFT plane
				{
					vtkPolyData* pData = vtkPolyData::New();

					//find plane in surface list with label APEX-LEFT
					SurfaceObject so;
					bool done = false;
					long index = 0;
					while (!done)
					{
						so = m_surfaceList.at(index);

						if (so.name == "APEX-LEFT")
						{
							done = true;
						}
						else
						{
							index++;
						}

						if (index >= m_surfaceList.size())
						{
							return "Error";
						}
					}

					Vector3 extendedPoint1 = point;
					extendedPoint1.x += 1000.0;

					double point1[] = {point.x, point.y, point.z};
					double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
					double t, ptline[3], pcoords[3];
					int subId;
					int result = APEX_LEFT_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
					if (result == 0)
					{
						returnCode += "_LATERAL";
					}
					else
					{
						returnCode += "_MEDIAL";

					}

					pData->Delete();
				}


			}
			else//assume patient right
			{
				//use MID-RIGHT plane
				{
					vtkPolyData* pData = vtkPolyData::New();

					//find plane in surface list with label CENTER-VERTICAL
					SurfaceObject so;
					bool done = false;
					long index = 0;
					while (!done)
					{
						so = m_surfaceList.at(index);

						if (so.name == "APEX-RIGHT")
						{
							done = true;
						}
						else
						{
							index++;
						}

						if (index >= m_surfaceList.size())
						{
							return "Error";
						}
					}

					Vector3 extendedPoint1 = point;
					extendedPoint1.x -= 1000.0;

					double point1[] = {point.x, point.y, point.z};
					double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
					double t, ptline[3], pcoords[3];
					int subId;
					int result = APEX_RIGHT_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
					if (result == 0)
					{
						returnCode += "_LATERAL";
					}
					else
					{
						returnCode += "_MEDIAL";

					}

					pData->Delete();
				}

			}

		}
		else
		if (mid)
		//if (base)//or mid, use green planes
		{
			if (patientLeft)
			{
				//use MID-LEFT plane
				{
					vtkPolyData* pData = vtkPolyData::New();

					//find plane in surface list with label MID-LEFT
					SurfaceObject so;
					bool done = false;
					long index = 0;
					while (!done)
					{
						so = m_surfaceList.at(index);

						if (so.name == "MID-LEFT")
						{
							done = true;
						}
						else
						{
							index++;
						}

						if (index >= m_surfaceList.size())
						{
							return "Error";
						}
					}

					Vector3 extendedPoint1 = point;
					extendedPoint1.x += 1000.0;

					double point1[] = {point.x, point.y, point.z};
					double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
					double t, ptline[3], pcoords[3];
					int subId;
					int result = MID_LEFT_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
					if (result == 0)
					{
						returnCode += "_LATERAL";
					}
					else
					{
						returnCode += "_MEDIAL";

					}

					pData->Delete();
				}


			}
			else//assume patient right
			{
				//use MID-RIGHT plane
				{
					vtkPolyData* pData = vtkPolyData::New();

					//find plane in surface list with label MID-RIGHT
					SurfaceObject so;
					bool done = false;
					long index = 0;
					while (!done)
					{
						so = m_surfaceList.at(index);

						if (so.name == "MID-RIGHT")
						{
							done = true;
						}
						else
						{
							index++;
						}

						if (index >= m_surfaceList.size())
						{
							return "Error";
						}
					}

					Vector3 extendedPoint1 = point;
					extendedPoint1.x -= 1000.0;

					double point1[] = {point.x, point.y, point.z};
					double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
					double t, ptline[3], pcoords[3];
					int subId;
					int result = MID_RIGHT_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
					if (result == 0)
					{
						returnCode += "_LATERAL";
					}
					else
					{
						returnCode += "_MEDIAL";

					}

					pData->Delete();

				}

			}

		}

		else
		if (base)
		//if base
		{
			if (patientLeft)
			{
				//use MID-LEFT plane
				{
					vtkPolyData* pData = vtkPolyData::New();

					//find plane in surface list with label MID-LEFT
					SurfaceObject so;
					bool done = false;
					long index = 0;
					while (!done)
					{
						so = m_surfaceList.at(index);

						if (so.name == "BASE-LEFT")
						{
							done = true;
						}
						else
						{
							index++;
						}

						if (index >= m_surfaceList.size())
						{
							return "Error";
						}
					}

					Vector3 extendedPoint1 = point;
					extendedPoint1.x += 1000.0;

					double point1[] = {point.x, point.y, point.z};
					double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
					double t, ptline[3], pcoords[3];
					int subId;
					int result = BASE_LEFT_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
					if (result == 0)
					{
						returnCode += "_LATERAL";
					}
					else
					{
						returnCode += "_MEDIAL";

					}

					pData->Delete();
				}


			}
			else//assume patient right
			{
				//use BASE-RIGHT plane
				{
					vtkPolyData* pData = vtkPolyData::New();

					//find plane in surface list with label MID-RIGHT
					SurfaceObject so;
					bool done = false;
					long index = 0;
					while (!done)
					{
						so = m_surfaceList.at(index);

						if (so.name == "BASE-RIGHT")
						{
							done = true;
						}
						else
						{
							index++;
						}

						if (index >= m_surfaceList.size())
						{
							return "Error";
						}
					}

					Vector3 extendedPoint1 = point;
					extendedPoint1.x -= 1000.0;

					double point1[] = {point.x, point.y, point.z};
					double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
					double t, ptline[3], pcoords[3];
					int subId;
					int result = BASE_RIGHT_locator->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
					if (result == 0)
					{
						returnCode += "_LATERAL";
					}
					else
					{
						returnCode += "_MEDIAL";

					}

					pData->Delete();

				}

			}

		}

	}
	
	return returnCode;
}
#endif

void RRI_SlicerInterface::AddMeasurePoint(long x, long y)
{
	if (InsideCube(x,y))
	{
		DoMeasurements(x,y);
	}
	else
	{
		StopMeasurement();
	}

}

void RRI_SlicerInterface::AddTarget(Vector3 point)
{
	//if (InsideCube(point))
	{
		m_userTargets.push_back(point);
		AddTargetSurface(point, m_targetSurfaceRadius);
	}
}

void RRI_SlicerInterface::AddPoint(long x, long y)
{
	if (InsideCube(x,y)) 
	{
		Vector3 point = ScreenToModel(x,y);
		m_userTargets.push_back(point);
		AddTargetSurface(point, m_targetSurfaceRadius);
		//m_currentMode = MODE_SLICER;
		//***UpdateDisplay();
	}
	else
	{
		m_currentMode = MODE_SLICER;

	}
}

void RRI_SlicerInterface::DeletePoint(long x, long y)
{
	if (InsideCube(x,y))
	{
		
		Vector3 point = ScreenToModel(x,y);

		if (m_userTargets.size() != 0)
		{
			long index = FindClosestPoint(&m_userTargets, x,y);

			if (index >= 0 && index < m_userTargets.size())
			{
				m_userTargets.erase(m_userTargets.begin()+index);

				m_target_surface.clear();
				for (int i=0; i<m_userTargets.size(); i++)
				{
					Vector3 point = m_userTargets.at(i);
					AddTargetSurface(point, m_targetSurfaceRadius);
				}
			}
		}
		else
		{
			if (m_surfaceList.size() > 0 && m_currentSurfaceObject != -1)//if there are any surface objects
			{
				//check to see if there are any points in the current surface object.
				SurfaceObject* so = &(m_surfaceList.at(m_currentSurfaceObject));
				if (so->userPoints.size() != 0)
				{
					long index = FindClosestPoint(&(so->userPoints), x, y);
					so->userPoints.erase(so->userPoints.begin()+index);
					m_target_surface.clear();
					for (int i=0; i<so->userPoints.size(); i++)
					{
						Vector3 point = so->userPoints.at(i);
						AddTargetSurface(point, m_targetSurfaceRadius);
					}

				}

#ifdef VTK_AVAILABLE
				//GenerateSurfaceFromUserPoints();//assume we want to update the surface here
#endif
			}

		}

		//***UpdateDisplay();
		
	}
}

void RRI_SlicerInterface::MovePoint(long x, long y)
{
	if (InsideCube(x,y))
	{
		Vector3 point = ScreenToModel(x,y);

		if (m_userTargets.size() == 0)
		{
			//find closest point in the currently selected surface object
			if (m_currentSurfaceObject >=0)
			{
				SurfaceObject* so = &(m_surfaceList.at(m_currentSurfaceObject));
				long index = FindClosestPoint(&(so->userPoints), x,y);
				so->userPoints.erase((so->userPoints.begin()+index));
				so->userPoints.push_back(point);
				ClearTargetSurfaces();
				for (int i=0; i<so->userPoints.size(); i++)
				{
					Vector3 point = so->userPoints.at(i);
					AddTargetSurface(point, m_targetSurfaceRadius);
				}

			}

		}
		else
		{
			
			long index = FindClosestPoint(&m_userTargets, x,y);
			m_userTargets.erase(m_userTargets.begin()+index);
			m_userTargets.push_back(point);

			m_target_surface.clear();
			for (int i=0; i<m_userTargets.size(); i++)
			{
				Vector3 point = m_userTargets.at(i);
				AddTargetSurface(point, m_targetSurfaceRadius);
			}
		}

		//***UpdateDisplay();
		
	}
	
}



//NEW WAY
Vector3 RRI_SlicerInterface::ScreenToModel(long x, long y)
{
    Vector3 modelPt(-1000,-1000,-1000);
	//returns screen to model coordinates from screen point
	GetVisualizer()->ScrnPtToModelPt(x,y,&modelPt);
    return modelPt;
}

//--------------------------
//Helper functions
//--------------------------

bool RRI_SlicerInterface::ReplaceString(std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

float RRI_SlicerInterface::helperDistanceBetweenTwoPoints(Vector3 vec1, Vector3 vec2)
{
	//calculate radius of sphere of interest
    float lengthX = (float)(vec1.x-vec2.x);
    float lengthY = (float)(vec1.y-vec2.y);
    float lengthZ = (float)(vec1.z-vec2.z);

    float distance = sqrt(lengthX*lengthX + lengthY*lengthY + lengthZ*lengthZ);
	return distance;
}


long RRI_SlicerInterface::ScaleImage(vtkImageData* Image)
{
//NOTE: when we import dicom using VTK, the image is flipped in y direction. We need to flip it back.
    vtkImageFlip* flipImageY = vtkImageFlip::New();
    flipImageY->SetFilteredAxis(1);//y
    flipImageY->SetInput(Image);
    flipImageY->Update();

    Image = flipImageY->GetOutput();

    flipImageY->Delete();

	//get scalar range for dicom image (maybe this can be used for window and level?)
	double Rng[2];
	Image->GetScalarRange(Rng);
	cout << "Range=" << Rng[0] << ", " << Rng[1] << endl;

	long windowMin = (long)Rng[0];
	long windowMax = (long)Rng[1];

	double Scale, Shift;
	Scale = 1.0;
	Shift = -Rng[0];

    std::string channelString = Image->GetScalarTypeAsString();

    long channels = 1;

    if (-1 != channelString.find("short"))
    {
        channels = 2;
    }

	vtkImageShiftScale *imageScale = vtkImageShiftScale::New();

    if (channels == 1)
    {
        imageScale->SetOutputScalarTypeToUnsignedChar();
        imageScale->SetScale(Scale);
        imageScale->SetShift(Shift);
        imageScale->SetInput(Image);
        imageScale->Update();
    }
    else
    if (channels == 2)
    {
        imageScale->SetOutputScalarTypeToUnsignedShort();
        imageScale->SetScale(Scale);
        imageScale->SetShift(Shift);
        imageScale->SetInput(Image);
        imageScale->Update();
    }

	Image = imageScale->GetOutput();
	Image->Register(NULL);
	Image->SetSource(NULL);
	Image->Update();
	imageScale->Delete();


    return 0;
}


/**
Unwraps a 1D array of phase values. The algorithm implemented in this function is the same as that in
the Matlab unwrap function.

@param Phase The array of wrapped phase values fed into the function as well as the unwrapped phase values
produced by the function.
*/
void RRI_SlicerInterface::UnwrapPhase1D(FloatArray* phaseArray)
{

	int array_size = phaseArray->size();

	FloatArray UnwrappedPhase;

    //setup array and clear all values to zero
	for (int i_cntr = 0; i_cntr < array_size; i_cntr++)
	{
        UnwrappedPhase.push_back(0.0);
	}

	float pm1 = phaseArray->at(0);
	UnwrappedPhase[0] = pm1;

	float po = 0.0;

	float pi = 2.0 * acos(0);
	float thr = pi - 1e-30;

	for (int i_cntr = 1; i_cntr < array_size; i_cntr++)
	{
		float cp = phaseArray->at(i_cntr) + po;
		float dp = cp-pm1;
		pm1 = cp;

		if (dp > thr)
		{
			while (dp > thr)
			{
				po = po - 2*pi;
				dp = dp - 2*pi;
			}
		}

		if (dp < -thr)
		{
			while (dp < -thr)
			{
				po = po + 2*pi;
				dp = dp + 2*pi;
			}
		}

		cp = phaseArray->at(i_cntr) + po;
		pm1 = cp;
		UnwrappedPhase[i_cntr] = cp;
	}

    //replace original phase array with new unwrapped phase array
    phaseArray->clear();
	for (int i_cntr = 0; i_cntr < array_size; i_cntr++)
	{
		phaseArray->push_back(UnwrappedPhase.at(i_cntr));
	}
}

//pixelA contains unwrapped phase
//pixelB contains phase that needs to be unwrapped
float RRI_SlicerInterface::UnwrapPhase(float pixelA, float pixelB)
{
    float unwrappedPhase = 0.0;
    float phaseOffset = 0.0;
	float pi = 2.0 * acos(0);
	float threshold = pi - 1e-30;

    float cp = pixelB;
	float phaseDiff = pixelB-pixelA;//subtract pixelA from pixelB

	if (phaseDiff > threshold)
	{
        //unwrap phase
		while (phaseDiff > threshold)
		{
			phaseOffset = phaseOffset - 2.0*pi;
			phaseDiff   = phaseDiff - 2.0*pi;
		}
	}

	if (phaseDiff < -threshold)
	{
        //unwrap phase
		while (phaseDiff < -threshold)
		{
			phaseOffset = phaseOffset + 2.0*pi;
			phaseDiff   = phaseDiff + 2.0*pi;
		}
	}

    unwrappedPhase = pixelB + phaseOffset;

    return unwrappedPhase;
}


void RRI_SlicerInterface::UnwrapPhase(int StartScan)
{
    /*
	int NumImagesPerScan = this->ImageSet->GetNumberOfImagesPerScan();
	int NumScans = this->ImageSet->GetNumberOfScans();

	int u_range[2], v_range[2];
	this->GetThermometryCalculationRange(u_range, v_range);

	// Iterate through every pixel in the first scan of the phase angle data.
	for (int k_cntr = 0; k_cntr < NumImagesPerScan; k_cntr++)
	{
		for (int j_cntr = v_range[0]; j_cntr <= v_range[1]; j_cntr++)
		{
			for (int i_cntr = u_range[0]; i_cntr <= u_range[1]; i_cntr++)
			{
				// Extract the phase at the current pixel as a function of time.
				CArray<float, float> TimeVaryingPhase;

				for (int time_cntr = StartScan; time_cntr < NumScans; time_cntr++)
				{
					float * buf = (float *)this->PhaseImageArray[time_cntr * NumImagesPerScan + k_cntr]->GetScalarPointer(i_cntr, j_cntr, 0);
					TimeVaryingPhase.Add(*buf);
				}

				// Unwrap the phase at the current pixel.
				this->UnwrapPhase1D(TimeVaryingPhase);

				// Copy the unwrapped phase into the phase angle data field.
				for (time_cntr = 0; time_cntr < TimeVaryingPhase.GetSize(); time_cntr++)
				{
					float * buf = (float *)this->PhaseImageArray[(time_cntr + StartScan) * NumImagesPerScan + k_cntr]->GetScalarPointer(i_cntr, j_cntr, 0);

					*buf = TimeVaryingPhase[time_cntr];
				}
			}
		}
	}
    */
}

//passes phase unwrapped image and returns the image with phase wrapped
long RRI_SlicerInterface::UnwrapAndAddPhaseImage(float* phaseImage, long width, long height, long depth)
{
    //allocate memory for unwrapped phase image
    float* wrappedPhaseImage = phaseImage;
    float* previousPhaseImage = m_phaseImages.at(m_phaseImages.size()-1);//get pointer to previous image
    for (int z = 0; z < depth; z++)
    {
        for (int y=0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                long pointer = z*(width*height) + y*width + x;
                //unwrap from previous image
                float pixelA = previousPhaseImage[pointer];
                float pixelB = wrappedPhaseImage[pointer];
                float unwrappedPhase = UnwrapPhase(pixelA, pixelB);
                pixelB = unwrappedPhase;//overwrite wrapped phase with unwrapped phase
            }

        }
    }

    //this image has now been unwrapped, so we can add it to the list
    m_phaseImages.push_back(phaseImage);//store pointer to plase image in phase image list

    return 0;
}

void RRI_SlicerInterface::ClearPhaseImages()
{
    for (int i = 0; i<m_phaseImages.size(); i++)
    {
        float* image = m_phaseImages.at(i);
        delete[] image;
    }

    m_phaseImages.clear();
}

//this function is used to import thermometry images
//the 2D images come in pairs, 1 real and 1 imaginary
//there are 9 image pairs per temporal position 
//filepath is the path that contains the thermometry images
long RRI_SlicerInterface::ImportThermometry(const char* folderPath, long baseLineIndex, Vector3 thermalRegion, float radius)
{
    Vector3 cubeSize = GetVisualizer()->GetCubeSize();
    Vector3 thermalRegion2D;
    thermalRegion2D.x = (thermalRegion.x + cubeSize.x/2.0)  / GetVoxelX();
    thermalRegion2D.y = (thermalRegion.y + cubeSize.y/2.0) / GetVoxelY();
    thermalRegion2D.z = (thermalRegion.z + cubeSize.z/2.0) / GetVoxelZ();
    long radius2D = radius/GetVoxelX();
    
    ClearPhaseImages();

    ImportColoursFromFile("c:/MRPingResources/ThermalColourMap.col");

    std::string outputFilePath = "c:/Scans/Outputfile.txt";
	ofstream outputFile(outputFilePath, ios_base::binary);



	std::string fname(folderPath);
	std::string dir;
	unsigned int pos;
	pos=fname.find_last_of('\\');
	dir=fname.substr(0,pos);
	std::string path = fname + "*";

	//long fileIndex = index * 18;//

	StringList files;
    StringList sortedFiles;
	

	//-----------------------------------------------------------------------------------------
	//get the names of all the files for this image
	WIN32_FIND_DATA findFileData;
	std::string dicomFileName;
	std::string dicomFilePath = folderPath;

	HANDLE hFind = ::FindFirstFile((LPCTSTR)path.c_str(), &findFileData);
	dicomFileName = (unsigned char)findFileData.cFileName;


    //=====================================================================================================
	if (INVALID_HANDLE_VALUE == hFind) 
	{
		return -1;
	} 
	else
	{   
		//put all files in this folder onto a list. Excluse . and .. 
		bool done = false;
		while (!done)
		{
			dicomFileName = (unsigned char)findFileData.cFileName;

			if (dicomFileName == "." || dicomFileName == "..")
			{
                //do nothing
			}
			else
			{
				std::string completeFilePath = folderPath + dicomFileName;
				files.push_back(completeFilePath);
			}

			if (FindNextFile(hFind, &findFileData))
			{
					
			}
			else
			{
				done = true;
				break;//no more files
			}

		}//while
	}

    long size = files.size();

    if (size-baseLineIndex < (2*18)) return -2;//not enough images to do the unwrapping
    //=====================================================================================================

    long numberOfTemporalImages = size/18;//there are 18 images per temporal volume

    long baselineImageIndex = (baseLineIndex-1) * 18;//index of baseline image
    


    //=====================================================================================================
    ////sort images so they can be more easily parsed
    //=====================================================================================================
    StringList temporalImages;
    bool done = false;
    long index = 1;
    while (!done)
    {
        //convert image index to string
        ostringstream imageStream;
		imageStream << (baselineImageIndex+index);//NOTE: baselineImageIndex is zero based and index is 1 based (naming of files uses 1 based index)
		std::string baselineIndexString = "." + imageStream.str();//generate file extension

        //file the file name that has baselineIndexString as extension
        for (int i = 0; i < files.size(); i++)
        {
            std::string file = files.at(i);

            if (-1 != file.find(baselineIndexString))
            {
                temporalImages.push_back(file);//put file into temporal images list
                files.erase(files.begin()+i);//remove this element from the old list since it is already on the new list
                break;//done with the loop
            }

        }

        index ++;
        if (index > size)
        {
            done = true;
        }
    }
    //=====================================================================================================

    long width = 256;
    long height = 256;
    long depth = 9;
    float pixelX = 1.0;
    float pixelY = 1.0;
    float pixelZ = 1.0;

    
    //baseline index is the index of the image that we want to start unwrapping from
    //the temporalImages list only contains the file names we want to process
    long temporalImagesSize = temporalImages.size();
    long temporalVolumeCount = temporalImagesSize/18;

    //now, process each volume from the list
    //each volume contains 18 images (9 real and 9 imaginary)
    //generate phase images for each image
    //create phase image and store pointer in list
    vtkDICOMImageReader *reader1 = vtkDICOMImageReader::New();
    vtkDICOMImageReader *reader2 = vtkDICOMImageReader::New();
    vtkImageData* imageReal = vtkImageData::New();
    vtkImageData* imageImaginary = vtkImageData::New();
    unsigned char* colourBuffer = 0;

    float *dircos;
	float *LPosition;
	float dircosX[3], dircosY[3], dircosZ[3];

    long offset = 75;//used to determine how much of the image to ignore

    long sizeOfFloat = sizeof(float);
     //create first phase image
    float* newPhaseImage = 0;//new float[width*height*depth];
    bool baseLineImageSaved = false;
   // temporalVolumeCount = 20;//
    bool firstTimeIn = true;
    //long image = 0;
    for (int imageNumber=0; imageNumber<temporalVolumeCount; imageNumber++)//for each volume set in the list
    {
        if (firstTimeIn)
        {
            //read in the last image to get the dimensions of the images and the direction cosines
            std::string realFile = temporalImages.at(17);
            reader1->SetFileName(realFile.c_str());
	        reader1->UpdateInformation();
	        reader1->Update();
            imageReal->DeepCopy(reader1->GetOutput());
    
            //get size of thermal image
            double spacing[3];
	        int dim[3];
	        imageReal->GetSpacing(spacing);
	        imageReal->GetDimensions(dim);
            width = dim[0];
            height = dim[1];
            //depth = 9;//
            pixelX = spacing[0];
            pixelY = spacing[1];
            pixelZ = spacing[2];

             //NOW, parse through all the unwrapped phase images to create a volume
            long channels = 3;
            CreateVolumeForThermometry(width, height, depth, pixelX, pixelY, pixelZ, channels);
            colourBuffer = GetBuffer();
            memset(colourBuffer, 0 , width*height*depth*channels);

            // collect dicom information from the reader before deleting it
	        
	        dircos = reader1->GetImageOrientationPatient();
	        LPosition = reader1->GetImagePositionPatient();
	        dircosX[0] = dircos[0];
	        dircosX[1] = dircos[1];
	        dircosX[2] = dircos[2];
	        dircosY[0] = dircos[3];
	        dircosY[1] = dircos[4];
	        dircosY[2] = dircos[5];

	        double DicomOrigin[3];
	        DicomOrigin[0] = LPosition[0];
	        DicomOrigin[1] = LPosition[1];
	        DicomOrigin[2] = LPosition[2];
            vtkMath::Cross(dircosX, dircosY, dircosZ);//fills in direcosZ

	        //fill interface members---------------
	        m_dicomOrigin.x = LPosition[0];
	        m_dicomOrigin.y = LPosition[1];
	        m_dicomOrigin.z = LPosition[2];
	        //
	        m_dicomX.x = dircosX[0];
	        m_dicomX.y = dircosX[1];
	        m_dicomX.z = dircosX[2];
	        //
	        m_dicomY.x = dircosY[0];
	        m_dicomY.y = dircosY[1];
	        m_dicomY.z = dircosY[2];
	        //
	        m_dicomZ.x = dircosZ[0];
	        m_dicomZ.y = dircosZ[1];
	        m_dicomZ.z = dircosZ[2];
	        //-------------------------------------


	        double DicomCenter[3];
	        double VisualizerCenter[3];
	        double DicomBounds[6];


	        VisualizerCenter[0] = 0.0;
	        VisualizerCenter[1] = 0.0;
	        VisualizerCenter[2] = 0.0;

	        imageReal->GetCenter(DicomCenter);
	        imageReal->SetOrigin(DicomOrigin);
	        imageReal->GetBounds(DicomBounds);

	        //get the transform from the SlicerWindow class so we can fill it in here
	        vtkMatrix4x4* matrix = vtkMatrix4x4::New();
	        matrix->Identity();

	        //set the rotation part of the matrix
	        for (int i = 0; i<3; i++)
	        {
		        matrix->SetElement(i, 0, dircosX[i]);
		        matrix->SetElement(i, 1, dircosY[i]);
		        matrix->SetElement(i, 2, dircosZ[i]);
	        }

	        Vector3 dicomOrigin1;
	        dicomOrigin1.x = DicomOrigin[0];
	        dicomOrigin1.y = DicomOrigin[1];
	        dicomOrigin1.z = DicomOrigin[2];


	        //get cube extents
	        Vector3 cubeSize1 = GetVisualizer()->GetCubeSize();
	        float xExtent1 = cubeSize1.x;
	        float yExtent1 = cubeSize1.y;
	        float zExtent1 = cubeSize1.z;


	        //calculate cube origin
	        Vector3 usOrigin1 = Vector3(xExtent1 / 2.0, yExtent1 / 2.0, zExtent1 / 2.0);


	        Vector3 bestCorner1 = Vector3(-usOrigin1.x, -usOrigin1.y, -usOrigin1.z);

	        Vector3 offset1;
	        offset1.x = bestCorner1.x - dicomOrigin1.x;
	        offset1.y = bestCorner1.y - dicomOrigin1.y;
	        offset1.z = bestCorner1.z - dicomOrigin1.z;


	        //Transform from Cube to World
	        vtkTransform* transform1 = vtkTransform::New();
	        transform1->PostMultiply();

	        //translate origin to (0,0,0)
	        transform1->Translate(-bestCorner1.x, -bestCorner1.y, -bestCorner1.z);//translate to origin (0,0,0)
	        transform1->Concatenate(matrix);//rotate to world
	        transform1->Translate(dicomOrigin1.x, dicomOrigin1.y, dicomOrigin1.z);//translate to world


	        transform1->GetMatrix(matrix);

	        //delete vtk objects
	        transform1->Delete();

	        SetDicomMatrix(matrix);//NOTE: SlicerWindow::SetTransformation() does a deep copy of the transform so we can delete it here.

        }

        //----------------------------------------------------------------------------------------
        //create new phase image, clear the image, store pointer in list
        newPhaseImage = new float[width*height*depth*sizeOfFloat];
        memset(newPhaseImage, 0, width*height*depth*sizeOfFloat);
        m_phaseImages.push_back(newPhaseImage);//NOTE: these will have to be deleted later
        //----------------------------------------------------------------------------------------

        //process real and imaginary images for each volume
        
        //int j=6;//***HACK_ALERT*** only look at the middle slice
        for (int volImage=0; volImage<18; volImage+=2)
        {
            //calculate real and imaginary index
            long realIndex = imageNumber*18 + volImage;//
            long imagIndex = realIndex+1;
           
            std::string realFile = temporalImages.at(realIndex);
	        std::string imagFile = temporalImages.at(imagIndex);
            
            //read real component-----------------------------------------------
	        reader1->SetFileName(realFile.c_str());
	        reader1->UpdateInformation();
	        reader1->Update();
            imageReal->DeepCopy(reader1->GetOutput());

            //NOTE: when we import dicom using VTK, the image is flipped in y direction. We need to flip it back.
            vtkImageFlip* flipImageY = vtkImageFlip::New();
            flipImageY->SetFilteredAxis(1);//y
            flipImageY->SetInput(imageReal);
            flipImageY->Update();

            
            imageReal->DeepCopy(flipImageY->GetOutput());

	       
            
            //read imaginary component-----------------------------------------------
	        reader2->SetFileName(imagFile.c_str());
	        reader2->UpdateInformation();
	        reader2->Update();
            imageImaginary->DeepCopy(reader2->GetOutput());


            flipImageY->SetFilteredAxis(1);//y
            flipImageY->SetInput(imageImaginary);
            flipImageY->Update();

            

            imageImaginary->DeepCopy(flipImageY->GetOutput());

            flipImageY->Delete();



            signed short* imagBuffer = (signed short*)imageImaginary->GetScalarPointer();
            signed short* realBuffer = (signed short*)imageReal->GetScalarPointer();
                    

          
            //------------------------------------------------------------------------------
            //parse through pixels in corresponding real and imaginary images
            //and generate phase images

            

            //for (int col = offset; col < height-offset; col++)
            for (int col = 0; col < height; col++)
            {
                //for (int row = offset; row < width-offset; row++)
                for (int row = 0; row < width; row++)
			    {
                    bool pointInRange = true;
                    long xoffset = abs(thermalRegion2D.x - row);
                    long yoffset = abs(thermalRegion2D.y - col);
                    long radius = sqrt(xoffset*xoffset + yoffset*yoffset);
                    if (radius < radius2D)
                    {
                        pointInRange = true;
                    }
                    //thermalRegion2D
                    //radius2D

                    if (pointInRange)
                    {
                        signed short realValue;
                        signed  short imagValue;
                        signed short* realPointer = (signed short*)imageReal->GetScalarPointer(row, col, 0);
                        signed short* imagPointer = (signed short*)imageImaginary->GetScalarPointer(row, col, 0);
                        realValue = *realPointer;
                        imagValue = *imagPointer;


                        //calculate phase and store in latest phase image
                        float newPhaseCalculation = atan2(imagValue, realValue);

                        long indexFloat = (volImage / 2)*width*height*sizeOfFloat + col*width*sizeOfFloat + row*sizeOfFloat;
                        memcpy(newPhaseImage + indexFloat, &newPhaseCalculation, sizeOfFloat);
                    }
			    }
            }

           
        }//for (int j=0; j<18; j+=2)

        firstTimeIn = false;
        
    }//for (int image=0; image<temporalVolumeCount; image++)//for each volume set in the list


    //here, the phase images are created so we can delete the imaginary and real images from the dicom files
    imageReal->Delete();
    imageImaginary->Delete();
    reader1->Delete();
    reader2->Delete();

   
    long phaseImageCount = m_phaseImages.size();


    float minTemp = 10000000;
    float maxTemp = -10000000;

    float minDiff = 10000000;
    float maxDiff = -10000000;

//UNWRAP PHASES=====================================================================
        
    FloatArray* phaseValues = new FloatArray();

    //done = false;
    //long floatIndex = 0;
    //while (!done)
    for (int z = 0; z<depth; z++)
    {
        for (int y = 0; y<height; y++)
        //for (int y = 0; y<height-offset; y++)
        {
            for (int x = 0; x<width; x++)
           // for (int x = offset; x<width-offset; x++)
            {
                bool pointInRange = true;
                long xoffset = abs(thermalRegion2D.x - x);
                long yoffset = abs(thermalRegion2D.y - y);
                long radius = sqrt(xoffset*xoffset + yoffset*yoffset);
                if (radius < radius2D)
                {
                    pointInRange = true;
                }

                long floatIndex = z*(width*height*sizeOfFloat) + y*width*sizeOfFloat + x*sizeOfFloat;//index into image
                if (pointInRange)
                {
                    //start over with new phase array for current pixel
                    phaseValues->clear();
                    //parse through each phase image and unwrap the phases
                    for (int i = 0; i < phaseImageCount; i++)
                    {
                        float* phaseImage = m_phaseImages.at(i);//pointer to image


                        //copy value from old phase image
                        float phaseValue;
                        memcpy(&phaseValue, phaseImage + floatIndex, sizeOfFloat);

                        phaseValues->push_back(phaseValue);

//#define TEST_POINT
#ifdef TEST_POINT
                        //******************************************************************************************************
                        if (x == 166 && y == 166)
                        {
                            ostringstream oldPhaseStream;
                            oldPhaseStream << (phaseValue);
                            std::string wrappedPhaseString = oldPhaseStream.str();
                            std::string temp = "Wrapped: " + wrappedPhaseString + "\n";

                            outputFile << temp;
                        }
                        //******************************************************************************************************
#endif

                    }
                }

                    

                //This function replaces the oldPhaseValues with updated values from the unwrap function
                if (pointInRange)
                {
                    UnwrapPhase1D(phaseValues);
                }


#ifdef TEST_POINT
                //******************************************************************************************************
                if (x==166 && y==166)
                {
                    for (int i = 0; i<phaseValues->size(); i++)
                    {
                        float phaseValue = phaseValues->at(i);
                        ostringstream oldPhaseStream;
		                oldPhaseStream << (phaseValue);
		                std::string unwrappedPhaseString = oldPhaseStream.str();
                        std::string temp = "Unwrapped: " + unwrappedPhaseString + "\n";

                        outputFile << temp;
                    }
                }
                //******************************************************************************************************
#endif


                //copy unwrapped values back into phase images
                if (pointInRange)
                {
                    for (int i = 0; i < phaseImageCount; i++)
                    {
                        float* phaseImage = m_phaseImages.at(i);//pointer to image
                        //long floatIndex = z*(width*height*sizeOfFloat) + y*width*sizeOfFloat + x*sizeOfFloat;//index into image

                        //copy value from old phase image
                        float phaseValue = phaseValues->at(i);
                        memcpy(phaseImage + floatIndex, &phaseValue, sizeOfFloat);

                    }//for (int i = 0; i<phaseImageCount; i++)
                }

            }//for (int x = 0; x<width; x++)

        }//for (int y = 0; y<height; y++)

        //floatIndex+=sizeOfFloat;
        //if (floatIndex >= width*height*depth*sizeOfFloat)
       // {
        //    done = true;
       // }
        
    }//for (int z = 0; z<depth; z++)
 

 //DONE UNWRAP PHASES=====================================================================
        


 //Generate TEMPAREATURE MAP=====================================================================
    float pi = 2.0 * acos(0);
    double alpha = -0.0101 * pow(10.0, -6);//1.010 e-8
    double gamma = -2.68 * pow(10.0, 8);//rad/s/T
    double fieldStrength = 1.5;//or 3.0
    double echoTime = 20.0/1000.0;//in seconds.
    double Gamma = -42.58e6;//= gamma / (2*pi)
    //		gamma / (2 * pi)	-42653524.749847077	double


    float* baselinePhaseImage = m_phaseImages.at(0);
    float* newPlaseImage = m_phaseImages.at(phaseImageCount-5);//complare first image with last image

#ifdef USE_WHILE_LOOP
    done = false;
    floatIndex = 0;
    long colourIndex = 0;
    while (!done)
#endif
    
    for (int z = 0; z<depth; z++)
    {
        //for (int y = 0; y<height; y++)
        for (int y = offset; y<height-offset; y++)
        {
            //for (int x = 0; x<width; x++)
            for (int x = offset; x<width-offset; x++)
            {
                bool pointInRange = true;
                long xoffset = abs(thermalRegion2D.x - x);
                long yoffset = abs(thermalRegion2D.y - y);
                long radius = sqrt(xoffset*xoffset + yoffset*yoffset);
                if (radius < radius2D)
                {
                    pointInRange = true;
                }

                if (pointInRange)
                {
                    long floatIndex = (z*width*height*sizeOfFloat + y*width*sizeOfFloat + x*sizeOfFloat);//index into image

                    //NOTE: here, we want to reverse z to match the coordinate system of the other MRI scans
                    long colourIndex = ((depth - z - 1)*width*height * 3 + y*width * 3 + x * 3);//


                    //copy value from baseline phase image
                    float baselinePhaseValue;
                    memcpy(&baselinePhaseValue, baselinePhaseImage + floatIndex, sizeOfFloat);
                    //copy value from new phase image
                    float newPhaseValue;
                    memcpy(&newPhaseValue, newPlaseImage + floatIndex, sizeOfFloat);

                    float phaseDiff = fabs(newPhaseValue - baselinePhaseValue);

                    //calculate temperature map and store in colour cube
                    if (phaseDiff != 0.0)
                    {

                        // double gamma2 = -268.0 * pow(10.0, 6);//rad/s/T

                        //From Sean's code
                        //  double Alpha = -0.0101;
                        
                        //	double Gamma = 42.58e6;// equals gamma2 / (2.0*pi) 
                        //*temperature_buf = this->BaselineTemperature - fabs(*phase_buf - reference_phase) * (180.0 / pi) / (this->Alpha * this->Gamma * this->GetThermometryImageSet()->GetMagneticFieldStrength() * 360.0 * this->GetThermometryImageSet()->GetEchoTime() * (1e-6));

                        //float temperature = (phaseDiff*(180.0 / pi)) / (alpha * gamma * fieldStrength * 360.0 * echoTime);

                        //Sean's way
                        float temperature = (phaseDiff) / (alpha * gamma * fieldStrength * echoTime);// * 4.0 * pi

                        //NOTE that 1/2pi is almost exactly 1/(alpha * gamma * fieldStrength * echoTime) when the field strength is 30
                        float test = (1.0) / (alpha * gamma * fieldStrength * echoTime);

                        if (temperature < minTemp) minTemp = temperature;
                        if (temperature > maxTemp) maxTemp = temperature;

                        if (phaseDiff < minDiff) minDiff = phaseDiff;
                        if (phaseDiff > maxDiff) maxDiff = phaseDiff;

                        unsigned char* pointer = colourBuffer + colourIndex;
                        unsigned char value = (unsigned char)temperature;

                        long colourCount = redArray.size();

                        long maxTemp = 70.0;
                        long fraction = colourCount / maxTemp;
                        long index = ((long)(temperature * fraction)) % colourCount;

                        unsigned char red = redArray[index];
                        unsigned char green = greenArray[index];
                        unsigned char blue = blueArray[index];


                        pointer[0] = red;
                        pointer[1] = green;
                        pointer[2] = blue;


                    }//if (phaseDiff != 0.0)
                    else
                    {
                        unsigned char* pointer = colourBuffer + colourIndex;
                        pointer[0] = 120;
                        pointer[1] = 120;
                        pointer[2] = 120;
                    }
                }

            }//for (int x = 0; x<width; x++)

        }//for (int y = 0; y<height; y++)


    }//for (int z = 0; z<depth; z++)

    bool test = false;

    return 0;
}



void RRI_SlicerInterface::ImportColoursFromFile(std::string filePath)
{
	std::ifstream  dataFile(filePath);

	this->redArray.clear();
	this->greenArray.clear();
	this->blueArray.clear();


	if (dataFile.is_open())
	{
		long red, green, blue;

		std::string line, name;
		while (dataFile.good())
		{
			getline(dataFile, line);
			std::string tempString = line;
			if (-1 != tempString.find("RGB"))
			{
				ReplaceString(tempString, "<RGB>", "");
				ReplaceString(tempString, "</RGB>", "");
				sscanf_s(tempString.c_str(), "%d,%d,%d", &red, &green, &blue);

				redArray.push_back(red);
				greenArray.push_back(green);
				blueArray.push_back(blue);
			}
			
		}

	}
}

//can import 8-bit US and 16-bit MRI and CT
long RRI_SlicerInterface::ImportDicomSingle(std::string folderPath)
{
	ClearSurfaces();
	ClearTargets();

	std::string fname(folderPath);
	std::string dir;
	unsigned int pos;
	pos = fname.find_last_of('\\');
	dir = fname.substr(0, pos);
	std::string path = fname + "*";


	//-----------------------------------------------------------------------------------------
	//Get first DICOM file in this folder in order to read the DICOM header
	WIN32_FIND_DATA findFileData;
	std::string dicomFileName;
	std::string dicomFilePath = folderPath;

	//find all the files in the folder and collect the file names into a string list
	StringList fileList;
	//HANDLE hFind = ::FindFirstFile((LPWCSTR)path.c_str(), &findFileData);
    HANDLE hFind = ::FindFirstFile((LPCSTR)path.c_str(), &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return -1;
	}
	else
	{
		dicomFileName = (unsigned char)findFileData.cFileName;
		bool done = false;
		while (!done)
		{
			if (dicomFileName == "." || dicomFileName == "..")
			{
				if (FindNextFile(hFind, &findFileData))
				{
					dicomFileName = (unsigned char)findFileData.cFileName;
				}
				else
				{
					fileList.push_back(dicomFileName);

					done = true;
					FindClose(hFind);
				}
				
			}
			else
			{

				if (FindNextFile(hFind, &findFileData))
				{
					dicomFileName = (unsigned char)findFileData.cFileName;
					fileList.push_back(dicomFileName);
				}
				else
				{
					done = true;
					FindClose(hFind);

				}

			}
		}

	}

	long fileCount = fileList.size();

	std::string imageFilePath = dir + "/" + fileList.at(fileCount / 2);//select the middle image

	SetVolumeFilePath(dir);
	vtkDICOMImageReader *reader = vtkDICOMImageReader::New();
	reader->SetFileName(imageFilePath.c_str());
	//reader->SetDirectoryName(dir.c_str());
	reader->UpdateInformation();
	reader->Update();

	vtkImageData* Image = reader->GetOutput();
	if (Image == NULL)
	{
		reader->Delete();
		return -1;
	}

    //NOTE: when we import dicom using VTK, the image is flipped in y direction. We need to flip it back.
    vtkImageFlip* flipImageY = vtkImageFlip::New();
    flipImageY->SetFilteredAxis(1);//y
    flipImageY->SetInput(Image);
    flipImageY->Update();

    Image = flipImageY->GetOutput();


    

	//get scalar range for dicom image (maybe this can be used for window and level?)
	double Rng[2];
	Image->GetScalarRange(Rng);
	cout << "Range=" << Rng[0] << ", " << Rng[1] << endl;

	long windowMin = (long)Rng[0];
	long windowMax = (long)Rng[1];

	double Scale, Shift;
	Scale = 1.0;
	Shift = -Rng[0];

    std::string channelString = Image->GetScalarTypeAsString();

    long channels = 1;

    if (-1 != channelString.find("short"))
    {
        channels = 2;
    }

	vtkImageShiftScale *imageScale = vtkImageShiftScale::New();

    if (channels == 1)
    {
        imageScale->SetOutputScalarTypeToUnsignedChar();
        imageScale->SetScale(Scale);
        imageScale->SetShift(Shift);
        imageScale->SetInput(Image);
        imageScale->Update();
    }
    else
    if (channels == 2)
    {
        imageScale->SetOutputScalarTypeToUnsignedShort();
        imageScale->SetScale(Scale);
        imageScale->SetShift(Shift);
        imageScale->SetInput(Image);
        imageScale->Update();
    }


#ifdef VTK_AVAILABLE
	// collect dicom information from the reader before deleting it
	float *dircos;
	float *LPosition;
	float dircosX[3], dircosY[3], dircosZ[3];
	dircos = reader->GetImageOrientationPatient();
	LPosition = reader->GetImagePositionPatient();
	dircosX[0] = dircos[0];
	dircosX[1] = dircos[1];
	dircosX[2] = dircos[2];
	dircosY[0] = dircos[3];
	dircosY[1] = dircos[4];
	dircosY[2] = dircos[5];

	double DicomOrigin[3];
	DicomOrigin[0] = LPosition[0];
	DicomOrigin[1] = LPosition[1];
	DicomOrigin[2] = LPosition[2];

	//vtkstd::string text1 = reader->GetDirectoryName();
	vtkstd::string text2 = reader->GetDescriptiveName();

	reader->Delete();

	//--------------------------------------------------------------------------------------------
	//get header data from DICOM file and read window and level information
    //--------------------------------------------------------------------------------------------
	vtkGDCMImageReader * medReader = vtkGDCMImageReader::New();
	dicomFilePath += dicomFileName;
	medReader->SetFileName(dicomFilePath.c_str());
	medReader->Update();

	vtkMedicalImageProperties* properties = medReader->GetMedicalImageProperties();
	if (properties->GetStudyDate() != 0)
	{
		vtkstd::string seriesDescription = properties->GetSeriesDescription(); //"0010|0010"

		long size = seriesDescription.size();
		SetDisplayText(&seriesDescription[0], size);
		double window, level;
		int count = properties->GetNumberOfWindowLevelPresets();
		if (count > 0)
		{
			properties->GetNthWindowLevelPreset(0, &window, &level);
			m_window = (long)window;
			m_level = (long)level;
		}
	}

	medReader->Delete();
    //--------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------
    


	Image = imageScale->GetOutput();
	Image->Register(NULL);
	Image->SetSource(NULL);
	Image->Update();
	imageScale->Delete();
	CreateVolumeFromVtkImageData(Image, windowMin, windowMax);
	m_isRegMatrixSet = false;

	vtkMath::Cross(dircosX, dircosY, dircosZ);

	//fill interface members---------------
	m_dicomOrigin.x = LPosition[0];
	m_dicomOrigin.y = LPosition[1];
	m_dicomOrigin.z = LPosition[2];
	//
	m_dicomX.x = dircosX[0];
	m_dicomX.y = dircosX[1];
	m_dicomX.z = dircosX[2];
	//
	m_dicomY.x = dircosY[0];
	m_dicomY.y = dircosY[1];
	m_dicomY.z = dircosY[2];
	//
	m_dicomZ.x = dircosZ[0];
	m_dicomZ.y = dircosZ[1];
	m_dicomZ.z = dircosZ[2];
	//-------------------------------------


	double DicomCenter[3];
	double VisualizerCenter[3];
	double DicomBounds[6];


	VisualizerCenter[0] = 0.0;
	VisualizerCenter[1] = 0.0;
	VisualizerCenter[2] = 0.0;

	Image->GetCenter(DicomCenter);
	Image->SetOrigin(DicomOrigin);
	Image->GetBounds(DicomBounds);

	//get the transform from the SlicerWindow class so we can fill it in here
	vtkMatrix4x4* matrix = vtkMatrix4x4::New();
	matrix->Identity();

	//set the rotation part of the matrix
	for (int i = 0; i<3; i++)
	{
		matrix->SetElement(i, 0, dircosX[i]);
		matrix->SetElement(i, 1, dircosY[i]);
		matrix->SetElement(i, 2, dircosZ[i]);
	}

	Vector3 dicomOrigin1;
	dicomOrigin1.x = DicomOrigin[0];
	dicomOrigin1.y = DicomOrigin[1];
	dicomOrigin1.z = DicomOrigin[2];


	//get cube extents
	Vector3 cubeSize1 = GetVisualizer()->GetCubeSize();
	float xExtent1 = cubeSize1.x;
	float yExtent1 = cubeSize1.y;
	float zExtent1 = cubeSize1.z;


	//calculate cube origin
	Vector3 usOrigin1 = Vector3(xExtent1 / 2.0, yExtent1 / 2.0, zExtent1 / 2.0);


	Vector3 bestCorner1 = Vector3(-usOrigin1.x, -usOrigin1.y, -usOrigin1.z);

	Vector3 offset1;
	offset1.x = bestCorner1.x - dicomOrigin1.x;
	offset1.y = bestCorner1.y - dicomOrigin1.y;
	offset1.z = bestCorner1.z - dicomOrigin1.z;


	//Transform from Cube to World
	vtkTransform* transform1 = vtkTransform::New();
	transform1->PostMultiply();

	//translate origin to (0,0,0)
	transform1->Translate(-bestCorner1.x, -bestCorner1.y, -bestCorner1.z);//translate to origin (0,0,0)
	transform1->Concatenate(matrix);//rotate to world
	transform1->Translate(dicomOrigin1.x, dicomOrigin1.y, dicomOrigin1.z);//translate to world


	transform1->GetMatrix(matrix);

	//delete vtk objects
	transform1->Delete();

	SetDicomMatrix(matrix);//NOTE: SlicerWindow::SetTransformation() does a deep copy of the transform so we can delete it here.

	std::string label;
	std::string newFileName = fname;
	std::string newFolderName = fname;

	newFileName = fname.substr(0, fname.size() - 1);//remove last "/" from name


	int found = newFileName.rfind("/");

	if (found != -1)
	{
		label = newFileName.substr(found + 1, newFileName.size() - 1);
	}

	ReplaceString(newFileName, label, "");

	SetVolumeFolderPath(newFileName);

	SetVolumeLabel(label);


	matrix->Delete();
	
	Image->Delete();
    flipImageY->Delete();


#endif	
return 0;

}




void RRI_SlicerInterface::SetStringValueFromTag(const char *s, const gdcm::Tag& t, gdcm::Anonymizer & ano)
{
    if (s && *s)
    {
#if 0
        gdcm::DataElement de(t);
        de.SetByteValue(s, strlen(s));
        const gdcm::Global& g = gdcm::Global::GetInstance();
        const gdcm::Dicts &dicts = g.GetDicts();
        // FIXME: we know the tag at compile time we could save some time
        // Using the static dict instead of the run-time one:
        const gdcm::DictEntry &dictentry = dicts.GetDictEntry(t);
        de.SetVR(dictentry.GetVR());
        ds.Insert(de);
#else
        ano.Replace(t, s);
#endif
    }
}

long RRI_SlicerInterface::GetNumberOfChannels()
{
    long channels = 1;
    if (GetScanGeometry() == SG_Linear16)
    {
        channels = 2;
    }
    else
    if (GetScanGeometry() == SG_Linear24 || GetScanGeometry() == SG_Axial24 || GetScanGeometry() == SG_Fan24 || GetScanGeometry() == SG_Hybrid24)
    {
        channels = 3;
    }

    return channels;
}
long RRI_SlicerInterface::ExportDicom(std::string folderPath, std::string fileLabel, std::string studyID, std::string patientID, long seriesNumber)
{

    long channels = GetNumberOfChannels();

    time_t long_time;
    char tmp[512];
    time(&long_time);
    strftime(tmp, 512, "%Y%m%d", localtime(&long_time));
    std::string date = tmp;
    strftime(tmp, 512, "%H%M%S", localtime(&long_time));
    std::string time = tmp;

    std::string  filename;
    std::string  folder = folderPath;
    std::string  infile = fileLabel;
    filename = folder + "\\" + infile + ".dcm";



    Vector3	cubesize = GetVisualizer()->GetCubeSize();
    Vector3 cubedim =  GetVisualizer()->GetCubeDim();

    float m_xDim = cubedim.x;
    float m_yDim = cubedim.y;
    float m_zDim = cubedim.z;

    float m_xVox = cubesize.x / cubedim.x;
    float m_yVox = cubesize.y / cubedim.y;
    float m_zVox = cubesize.z / cubedim.z;


    unsigned int dims[3] = {};
    dims[0] = m_xDim;
    dims[1] = m_yDim;

    double spacing[3] = {};
    spacing[0] = m_xVox;
    spacing[1] = m_yVox;

    // generate proper UID for study and series
    gdcm::UIDGenerator::SetRoot("1.2.826.0.1.3680043.2.1125"); //NOTE: must include in project Rpcrt4.lib

    gdcm::UIDGenerator uidgen;
    const char *uidStudy = uidgen.Generate();
    const char *uidSeries = uidgen.Generate();

    unsigned char* buffer = GetBuffer();



    for (long z = 0; z < m_zDim; z++)
    {
       
        gdcm::ImageWriter writer;

        gdcm::Image &image = writer.GetImage();

        image.SetTransferSyntax(gdcm::TransferSyntax::ExplicitVRLittleEndian);

        image.SetNumberOfDimensions(2);

        image.SetDimension(0, dims[0]);
        image.SetDimension(1, dims[1]);

        image.SetSpacing(0, spacing[0]);
        image.SetSpacing(1, spacing[1]);

        
        gdcm::PixelFormat pixeltype = gdcm::PixelFormat::UINT8;
        if (channels == 2)
            pixeltype = gdcm::PixelFormat::UINT16;
        
        gdcm::PhotometricInterpretation pi = gdcm::PhotometricInterpretation::MONOCHROME2;


        if (channels == 1)
        {
            pixeltype.SetSamplesPerPixel(1);
        }
        else
        if (channels == 2)
        {
            pixeltype.SetSamplesPerPixel(2);
        }
        else
        if (channels == 3)
        {
            pixeltype.SetSamplesPerPixel(3);
        }
        
        gdcm::MediaStorage ms = gdcm::MediaStorage::UltrasoundImageStorage;

        
        image.SetPhotometricInterpretation(pi);
        image.SetPixelFormat(pixeltype);
        image.SetPlanarConfiguration(0);

        //NOTE: here the volume is assumed to be already reconstructed.
        ///////////////////////////////////////////////////////////////////////////
        /*
        for (long y = 0; y < m_yDim; y++)
        {
            for (long x = 0; x < m_xDim; x++)
            {
                *p = vis->GetVoxValue(x, y, z);
                p++;
            }
        }
        */

        unsigned char* pointer = buffer + (z * dims[0] * dims[1] * channels);
  

        gdcm::DataElement pixeldata(gdcm::Tag(0x7fe0, 0x0010));
        pixeldata.SetByteValue((const char *)pointer, m_xDim * m_yDim * channels);
        image.SetDataElement(pixeldata);

        ///////////////////////////////////////////////////////////////////////////

        gdcm::File& file = writer.GetFile();
        gdcm::DataSet& ds = file.GetDataSet();

        gdcm::Anonymizer ano;
        ano.SetFile(file);


        std::string value;


        // patient name
        value = patientID;
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0010, 0x0010), ano);

        // patient ID
        value = patientID;
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0010, 0x0020), ano);

        // patient age
        value = " ";
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0010, 0x1010), ano);

        // patient sex
        value = " ";
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0010, 0x0040), ano);

        // patient birth date
        value = " ";
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0010, 0x0030), ano);

        // study date
        value = date;
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x0020), ano);

        // study time
        value = time;
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x0030), ano);

        // acquisition time
        value = time;
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x0032), ano);

        // image date
        value = date;
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x0023), ano);

        // image time
        value = time;
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x0033), ano);

        // image number
        char imagenum[8];
        _itoa_s((z + 1), imagenum, 10);
        SetStringValueFromTag(imagenum, gdcm::Tag(0x0020, 0x0013), ano);

        // spacing between slices
        char spacebetween[16];
        sprintf_s(spacebetween, "%f", m_zVox);
        SetStringValueFromTag(spacebetween, gdcm::Tag(0x0018, 0x0088), ano);

        // series number
        ostringstream seriesNumberOS;
        seriesNumberOS << (seriesNumber);
        value = seriesNumberOS.str();
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0020, 0x0011), ano);

        // series description
        value = "3D Ultrasound";
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x103e), ano);

        // study ID
        value = studyID;
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0020, 0x0010), ano);

        // modality
        value = "US";
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x0060), ano);

        // manufacturer
        value = "Philips";
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x0070), ano);

        // institution name
        value = "Robarts Research Institute";
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0008, 0x0080), ano);

        // slice thickness
        char thick[16];
        sprintf_s(thick, "%f", m_zVox);
        SetStringValueFromTag(thick, gdcm::Tag(0x0018, 0x0050), ano);

        //window extent
        ostringstream windowValue;
        windowValue << (m_window);
        value = windowValue.str();
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0028, 0x1051), ano);

        //window center
        ostringstream levelValue;
        levelValue << (m_level);
        value = levelValue.str();
        SetStringValueFromTag(value.c_str(), gdcm::Tag(0x0028, 0x1050), ano);

        //pixel spacing
        ostringstream xVox;
        xVox << (m_xVox);
        ostringstream yVox;
        yVox << (m_yVox);
        std::string temp = xVox.str() + "\\" + yVox.str();
        SetStringValueFromTag(temp.c_str(), gdcm::Tag(0x0028, 0x0030), ano);

        //pixel aspect ratio
        temp = "1\\1";
        SetStringValueFromTag(temp.c_str(), gdcm::Tag(0x0028, 0x0034), ano);

        //samples per pixel
        temp = "1";
        SetStringValueFromTag(temp.c_str(), gdcm::Tag(0x0028, 0x0002), ano);

        //Pixel representation
        temp = "1";
        SetStringValueFromTag(temp.c_str(), gdcm::Tag(0x0028, 0x0103), ano);



        // Image Position
        double location = z * m_zVox;
        ostringstream str;
        str << "0" << "\\" << "0" << "\\" << location;
        temp = str.str();
        //header->ReplaceOrCreateByNumber(str.str(), 0x0020, 0x0032);
        SetStringValueFromTag(temp.c_str(), gdcm::Tag(0x0020, 0x0032), ano);


        // Slice Location
        str.str("");
        str << location;
        temp = str.str();
        //header->ReplaceOrCreateByNumber(str.str(), 0x0020, 0x1041);
        SetStringValueFromTag(temp.c_str(), gdcm::Tag(0x0020, 0x1041), ano);


        // image type
        gdcm::Attribute<0x0008, 0x0008> imagetype;
        const gdcm::CSComp values[] = { "ORIGINAL", "PRIMARY" };
        imagetype.SetValues(values, 2);
        ds.Insert(imagetype.GetAsDataElement());

        {
            gdcm::DataElement de(gdcm::Tag(0x0020, 0x000d));
            de.SetByteValue(uidStudy, strlen(uidStudy));
            de.SetVR(gdcm::Attribute<0x0020, 0x000d>::GetVR());
            ds.Insert(de);
        }

        {
            gdcm::DataElement de(gdcm::Tag(0x0020, 0x000e));
            de.SetByteValue(uidSeries, strlen(uidSeries));
            de.SetVR(gdcm::Attribute<0x0020, 0x000e>::GetVR());
            ds.Insert(de);
        }

        ///////////////////////////////////////////////////////////////////////////

        filename = folder + "\\" + infile;

        char tmp[8];
        sprintf_s(tmp, "%d", z + 1);
        long len = strlen(tmp);

        if (len == 1)
        {
            filename = filename + "000" + tmp + ".dcm";
        }
        else if (len == 2)
        {
            filename = filename + "00" + tmp + ".dcm";
        }
        else if (len == 3)
        {
            filename = filename + "0" + tmp + ".dcm";
        }
        else
        {
            filename = filename + tmp + ".dcm";
        }

        writer.SetFileName(filename.c_str());

        if (!writer.Write())
        {
            bool problem = true;
        }

    }

    return 0;
}

//can import 8-bit US and 16-bit MRI and CT
long RRI_SlicerInterface::ImportDicom(const char* folderPath)
{
	ClearSurfaces();
	ClearTargets();

	{
		std::string fname(folderPath);
		std::string dir;
		unsigned int pos;
		pos=fname.find_last_of('\\');
		dir=fname.substr(0,pos);
		std::string path = fname + "*";

		//-----------------------------------------------------------------------------------------
		//Get first DICOM file in this folder in order to read the DICOM header
		WIN32_FIND_DATA findFileData;
		std::string dicomFileName;
		std::string dicomFilePath = folderPath;

		HANDLE hFind = ::FindFirstFile((LPCSTR)path.c_str(), &findFileData);
        //HANDLE hFind = ::FindFirstFile((LPCWSTR)path.c_str(), &findFileData);

		if (INVALID_HANDLE_VALUE == hFind) 
		{
			return -1;
		} 
		else
		{
			dicomFileName = (unsigned char)findFileData.cFileName;
			bool done = false;
			while (!done)
			{
				if (dicomFileName == "." || dicomFileName == "..")
				{
					if (FindNextFile(hFind, &findFileData))
					{
						dicomFileName = (unsigned char)findFileData.cFileName;
					}
					else
					{
						done = true;
						FindClose(hFind);
					}

				}
				else
				{
					done = true;
					FindClose(hFind);
				}
			}
			
		}

		//-----------------------------------------------------------------------------------------
		
		SetVolumeFilePath(dir);
		vtkDICOMImageReader *reader = vtkDICOMImageReader::New();
		reader->SetDirectoryName(dir.c_str());
		reader->UpdateInformation();
		reader->Update();

		vtkImageData* Image = reader->GetOutput();
		if (Image==NULL)
		{
			reader->Delete();
			return -1;
		}

#define FLIPYZ
#ifdef FLIPYZ
        //NOTE: when importing DICOM using vtk, the image is flipped in both the Y and Z directions. 
        //Here, we need to flip it back.
        vtkImageFlip* flipImageY = vtkImageFlip::New();
        flipImageY->SetFilteredAxis(1);//y
        flipImageY->SetInput(Image);
        flipImageY->Update();

        vtkImageFlip* flipImageZ = vtkImageFlip::New();
        flipImageZ->SetFilteredAxis(2);//z
        flipImageZ->SetInput(flipImageY->GetOutput());
        flipImageZ->Update();
        
        Image = flipImageZ->GetOutput();
        
#endif

        int extent[6];
        Image->GetExtent(extent);

        int dimensions[3];
        Image->GetDimensions(dimensions);


        std::string channelString = Image->GetScalarTypeAsString();

        long channels = 1;

        if (-1 != channelString.find("short"))
        {
            channels = 2;
        }

		//get scalar range for dicom image (maybe this can be used for window and level?)
		double Rng[2];
		Image->GetScalarRange(Rng);
		cout <<"Range="<<Rng[0]<<", "<<Rng[1]<<endl;

		long windowMin = (long)Rng[0];
		long windowMax = (long)Rng[1];

		double Scale,Shift;
		Scale=1.0;
		Shift=-Rng[0];

		vtkImageShiftScale *imageScale = vtkImageShiftScale::New();

        if (channels == 1)
        {
            imageScale->SetOutputScalarTypeToUnsignedChar();
            imageScale->SetScale(Scale);
            imageScale->SetShift(Shift);
            imageScale->SetInput(Image);
            imageScale->Update();
        }
        else
        if (channels == 2)
        {
            imageScale->SetOutputScalarTypeToUnsignedShort();
            imageScale->SetScale(Scale);
            imageScale->SetShift(Shift);
            imageScale->SetInput(Image);
            imageScale->Update();
        }
        

#ifdef VTK_AVAILABLE
		// collect dicom information from the reader before deleting it
		float *dircos;
		float *LPosition;
		float dircosX[3],dircosY[3],dircosZ[3];
		dircos=reader->GetImageOrientationPatient();
		LPosition=reader->GetImagePositionPatient();
		dircosX[0]=dircos[0];
		dircosX[1]=dircos[1];
		dircosX[2]=dircos[2];
		dircosY[0]=dircos[3];
		dircosY[1]=dircos[4];
		dircosY[2]=dircos[5];

		double DicomOrigin[3];
		DicomOrigin[0]=LPosition[0];
		DicomOrigin[1]=LPosition[1];
		DicomOrigin[2]=LPosition[2];
#endif	
		
		vtkstd::string text1 = reader->GetDirectoryName();
		vtkstd::string text2 = reader->GetDescriptiveName();

		reader->Delete();

//-------------
		//get header data from DICOM file
		vtkGDCMImageReader * medReader = vtkGDCMImageReader::New();
		dicomFilePath += dicomFileName;
		medReader->SetFileName(dicomFilePath.c_str());
		medReader->Update();

		vtkMedicalImageProperties* properties = medReader->GetMedicalImageProperties();
		if (properties->GetStudyDate() != 0)
		{
			vtkstd::string seriesDescription = properties->GetSeriesDescription(); //"0010|0010"

			long size = seriesDescription.size();
			SetDisplayText(&seriesDescription[0], size);
			double window, level;
			int count = properties->GetNumberOfWindowLevelPresets();
			if (count > 0)
			{
				properties->GetNthWindowLevelPreset(0, &window, &level);
				m_window = (long)window;
				m_level = (long)level;
			}

			
		}

		medReader->Delete();
//-------------
		
		Image=imageScale->GetOutput();
		Image->Register(NULL);
		Image->SetSource(NULL);
		Image->Update();
		imageScale->Delete();
		CreateVolumeFromVtkImageData(Image, windowMin, windowMax);
		m_isRegMatrixSet = false;
       


#ifdef VTK_AVAILABLE

		vtkMath::Cross(dircosX,dircosY,dircosZ);
		
		//fill interface members---------------
		m_dicomOrigin.x = LPosition[0];
		m_dicomOrigin.y = LPosition[1];
		m_dicomOrigin.z = LPosition[2];
		//
		m_dicomX.x = dircosX[0];
		m_dicomX.y = dircosX[1];
		m_dicomX.z = dircosX[2];
		//
		m_dicomY.x = dircosY[0];
		m_dicomY.y = dircosY[1];
		m_dicomY.z = dircosY[2];
		//
		m_dicomZ.x = dircosZ[0];
		m_dicomZ.y = dircosZ[1];
		m_dicomZ.z = dircosZ[2];
		//-------------------------------------


		double DicomCenter[3];
		double VisualizerCenter[3];
		double DicomBounds[6];


		VisualizerCenter[0]=0.0;
		VisualizerCenter[1]=0.0;
		VisualizerCenter[2]=0.0;

		Image->GetCenter(DicomCenter);
		Image->SetOrigin(DicomOrigin);
		Image->GetBounds(DicomBounds);

		//get the transform from the SlicerWindow class so we can fill it in here
		vtkMatrix4x4* matrix = vtkMatrix4x4::New();
		matrix->Identity();

		//set the rotation part of the matrix
		for (int i=0; i<3; i++)
		{
			matrix->SetElement(i, 0, dircosX[i]);
			matrix->SetElement(i, 1, dircosY[i]);
			matrix->SetElement(i, 2, dircosZ[i]);
		}
		
		Vector3 dicomOrigin1;
		dicomOrigin1.x = DicomOrigin[0];
		dicomOrigin1.y = DicomOrigin[1];
		dicomOrigin1.z = DicomOrigin[2];

         m_isDicomMatrixSet = true;


		//get cube extents
		Vector3 cubeSize1 = GetVisualizer()->GetCubeSize();
		float xExtent1 = cubeSize1.x;
		float yExtent1 = cubeSize1.y;
		float zExtent1 = cubeSize1.z;


		//calculate cube origin
		Vector3 usOrigin1 = Vector3(xExtent1/2.0, yExtent1/2.0, zExtent1/2.0);

        Vector3 offset1;
        Vector3 bestCorner1;//the corner of the visualizer that corrsponds to the DICOM origin of the imported 3D image.



        //DICOM origin is always (-,-,-) in the Visualizer coordinate system.
        bestCorner1 = Vector3(-usOrigin1.x, -usOrigin1.y, -usOrigin1.z);


		
		offset1.x = bestCorner1.x - dicomOrigin1.x;
		offset1.y = bestCorner1.y - dicomOrigin1.y;
		offset1.z = bestCorner1.z - dicomOrigin1.z;



		//Transform from Cube to World
		vtkTransform* transform1 = vtkTransform::New();
		transform1->PostMultiply();

		//translate origin to (0,0,0)
		transform1->Translate(-bestCorner1.x, -bestCorner1.y,-bestCorner1.z);//translate to origin (0,0,0)
		transform1->Concatenate(matrix);//rotate to world
		transform1->Translate(dicomOrigin1.x,dicomOrigin1.y, dicomOrigin1.z);//translate to world


		transform1->GetMatrix(matrix);

	    //delete vtk objects
		transform1->Delete();

		SetDicomMatrix(matrix);//NOTE: SlicerWindow::SetTransformation() does a deep copy of the transform so we can delete it here.

		std::string label;
		std::string newFileName = fname;
		std::string newFolderName = fname;
		
		newFileName = fname.substr( 0, fname.size()-1);//remove last "/" from name


		int found = newFileName.rfind( "/");

		if( found != -1 )
		{
			label = newFileName.substr( found+1, newFileName.size() - 1 );
		}

		ReplaceString(newFileName, label, "");
		
		SetVolumeFolderPath(newFileName);

        long size = label.size();
		SetDisplayText(&label[0], size);//set the file path as the display text

		SetVolumeLabel(label);
		

		
		matrix->Delete();
#endif		
		Image->Delete();
        flipImageZ->Delete();
        flipImageY->Delete();
	}

	return 0;

}

// CImTeC_Joseph Transformation Matrix, Joseph

void RRI_SlicerInterface::GetDicomMatrix(vtkMatrix4x4* matrix) 
{
	matrix->DeepCopy(m_dicomMatrix);
}
void RRI_SlicerInterface::SetDicomMatrix(vtkMatrix4x4* matrix)
{
	m_dicomMatrix->DeepCopy(matrix);
    m_isDicomMatrixSet = true;
    m_isRegMatrixSet = false;
}


void RRI_SlicerInterface::GetRegMatrix(vtkMatrix4x4* matrix) 
{
	matrix->DeepCopy(m_regMatrix);
}

void RRI_SlicerInterface::SetRegMatrix(vtkMatrix4x4* matrix)
{
	m_regMatrix->DeepCopy(matrix);
	m_isRegMatrixSet = true;//set from the client side
    m_isDicomMatrixSet = false;//this overrides the Dicom Matrix
}

//Painting functionality allows the user to paint into 
//the hidden binary cube. This binary file is blended
//into the greyscale file for display.
void RRI_SlicerInterface::StartPainting()
{

    m_isPainting = true;
    m_isErasing = false;
    m_isEditing = false;
    
}

void RRI_SlicerInterface::StopPainting()
{
    m_isPainting = false;
    m_isErasing = false;
    m_isEditing = false;
}

void RRI_SlicerInterface::StartErasing()
{
    m_isErasing = true;
    m_isPainting = false;
    m_isEditing = false;
}

void RRI_SlicerInterface::StopErasing()
{
    m_isPainting = false;
    m_isErasing = false;
    m_isEditing = false;
}

void RRI_SlicerInterface::StartEditing()
{
    m_isEditing = true;
    m_isPainting = false;
    m_isErasing = false;
}

void RRI_SlicerInterface::StopEditing()
{
    m_isPainting = false;
    m_isErasing = false;
    m_isEditing = false;
}


void RRI_SlicerInterface::SetBrushSize(double brushSize)
{
    Vector3 cubeDim = GetVisualizer()->GetCubeDim();
    Vector3 cubeSize = GetVisualizer()->GetCubeSize();

    long cubeDimSize = cubeDim.x * cubeDim.y * cubeDim.z;

    float voxelX = cubeSize.x / (float)cubeDim.x;
    float voxelY = cubeSize.y / (float)cubeDim.y;
    float voxelZ = cubeSize.z / (float)cubeDim.z;

    m_paintBrushSize = brushSize;

	m_paintBrushRadiusPixels = m_paintBrushSize;// (long)(m_paintBrushSize / voxelX / 2.0) * GetVisualizer()->GetZoomFactor() / 5.0;
}



void RRI_SlicerInterface::StartMeasurement(long type)
{
	m_currentMode = MODE_MEASURE;
    m_currentState = ST_MEASURE;

	m_measureMode = type;
	measure.New = true;
    measure.Done = false;
	mPoly.erase(mPoly.begin(), mPoly.end());
	mUndo.erase(mUndo.begin(), mUndo.end());
}


void RRI_SlicerInterface::StopMeasurement()
{
    m_currentMode = MODE_SLICER;
    m_currentState = ST_ROTATE;

    measure.New = true;
    measure.Done = true;
	mPoly.erase(mPoly.begin(), mPoly.end());
	mUndo.erase(mUndo.begin(), mUndo.end());
}

bool RRI_SlicerInterface::DoMeasurements(long x, long y, bool end)
{
	POINT point;
	point.x = x;
	point.y = y;

    //NOTE: this will return -1 if the point is not in the cube
	long thisFace = GetVisualizer()->GetFaceID(point.x, point.y);

    //if point is not in the cube then the face is not valid, therefore return
    if (-1 == thisFace) 
	{
        return false;
	}

	bool endMeasure = end;

    //this should only happen for the first point in the measurement
	if (measure.New)
	{
		m_measureFace = thisFace;
		measure.New = false;
        measure.Done = false;
	}


//------------------------------------------------------------------
//NEW:LG
//don't store duplicate points
    if (mPoly.size() >= 1)//if there is at least one point on the list
    {
        POINT lastPoint = mPoly.at(mPoly.size()-1);
        POINT firstPoint = mPoly.at(0);
        if (  (x == lastPoint.x && y == lastPoint.y)
            ||(x == firstPoint.x && y == firstPoint.y))//is the point different than the last point
        {
            //don't store duplicate points
        }
        else
        {
			
            mPoly.push_back(point);
        }
    }
    else
    {
        mPoly.push_back(point);//store first point
    }
//---------------------------------------------------------------------	



	switch (m_measureMode) {

	case MLINE:
        if (mPoly.size() == 2)
        {
			endMeasure = true;
            StoreMeasurements();
            measure.New = true;
			m_measurements->Clear();
#ifdef ONE_MEAUSRE_MODE
			SetMode(MODE_SLICER);
#endif
        }
		break;

    default:
        break;
	} // switch (m_measureMode)


    return measure.New;//return measure done flag. (true if measure complete, false otherwise)
}


void RRI_SlicerInterface::StoreMeasurements()
{	

    //mPoly is a set of 2D ponts in screen coordinates
    //poly (created below) is a set of 3D points in volume coordinates
    bool validMeasurement = true;
	Vector3Vec poly;
	for(long i = 0; i < mPoly.size(); i++)
	{
        long ptx = mPoly[i].x;
        long pty = mPoly[i].y;

        Vector3 pt;


//==================================================================================

//NOTE: here is was having problems with the screen points not ending
//      up inside the cube for some reason especially during segmentation
//      NEW_WAY calculates the model point correctly even though the point
//      is outside the cube. This allows contours to "slip" out if the cube
//      but the surface reconstruction still works.


        //this ScrnPtToModelPt function returns false if point is not inside cube
		
		long ret = GetVisualizer()->ScrnPtToModelPt(ptx, pty, &pt);

//====================================================================================


		if (ret)
	        poly.push_back(pt);
        

		//NOTE: out of plane measurements are only allowed for linear measurements
		if (/*m_measureMode != MLINE && */GetVisualizer()->GetFaceID(mPoly[i].x, mPoly[i].y) != m_measureFace)
			validMeasurement = false;

	}

    unsigned short subtype = 0;
    unsigned short attribute = MATTR_NEW;//***LG can't remember what this is for???

    m_measurements->SetPoints(GetVisualizer()->GetView(), GetVisualizer()->GetStateModelView(), m_measureMode, poly, m_measureFace, subtype, attribute);

	mPoly.erase(mPoly.begin(), mPoly.end());
	mUndo.erase(mUndo.begin(), mUndo.end());

    MObjpVec mobjlist = m_measurements->GetMeasureObjects();

	int measureCount = mobjlist.size();

	if (measureCount <= 0) return;//no measure points in the list so nothing to do here...

	MObj* oi = mobjlist.at(measureCount-1);//get last one

/*
CString desc;
	switch (m_measureMode) 
	{
	case MPOINT: 
		desc.Format("%1d=(%5.2f, %5.2f, %5.2f)", measureCount, (oi)->param[0], (oi)->param[1], (oi)->param[2]);
		break;
	case MLINE:
		desc.Format("%1d=%.5f mm", measureCount, (oi)->param[0]);
		break;
	case MAREA:
		desc.Format("%1d=%.2f cm%c", measureCount, (oi)->param[0]/100.0, 178);
		break;
	case MVOLUME:

		desc.Format("%1d=%.2f cm%c", measureCount, (oi)->param[0]/1000.0, 179);
		break;
	case MANGLE:
		desc.Format("%1d=%.2f %c", measureCount, (oi)->param[0], 176);
		break;
	}

	SetMeasureString(desc);//to be displayed on the screen after a measurement is completed
*/

}

void RRI_SlicerInterface::CleanUpForNewVolume()
{
	//clear user defined targets
	m_surfaceList.clear();
	m_userTargets.clear();
	m_target_surface.clear();


}
//

void RRI_SlicerInterface::CreateVolumeForThermometry(long width, long height, long imageCount, double xSpacing, double ySpacing, double zSpacing, long channels)
{
    
	CleanUpForNewVolume();//delete surfaces
	   
    AcqParam acq;										//parameters for acquisition
    CalParam cal;										//parameters for calibration
    acq.width = width;								//width of ROI (video sub region)
    acq.height = height;								//height of ROI (video sub region)
    acq.numFrames = imageCount;							//number of images to capture
    acq.horizontalReflection = HIR_PowerupDefault;		//no image reflection
    acq.scanDirection = SD_Clockwise;				//counter clockwise scan
    acq.probeOrientation = PO_Top;//					//bottom orientation
    if (channels == 2)
    {
        acq.scanGeometry = SG_Linear16;        
    }
    else
    if (channels == 3)
    {
        acq.scanGeometry = SG_Linear24;   
    }     
    else
    {
        acq.scanGeometry = SG_LinearRaw;
    }
    acq.sweepAngle = 0.0;								//no shearing

    cal.axisOfRotation = 0;//acq.width/2;					//center of ROI is center of rotation
    cal.inPlaneTilt = 0.0f;								//not currently used
    cal.outPlaneDisplacement = 0.0f;					//not currently used
    cal.outPlaneTilt = 0.0f;							//not currently used
    cal.probeDistance = 0;								//not currently used
    cal.xVoxelSize = xSpacing;						//size of voxel in x direction (mm)
    cal.yVoxelSize = ySpacing;						//size of voxel in y direction (mm)
    cal.zVoxelSize = zSpacing;						//size of voxel in z direction (mm)


    CreateVolume(acq,cal, channels);//24-bit three channels
	m_isRegMatrixSet = false;

	SetModelView(-1, 0.001f, 0.001f, 0.001f, false);

	GetVisualizer()->Zoom(0.7);//zoom out

    UpdateDisplay();

}

void RRI_SlicerInterface::CreateVolumeFromBitmap(unsigned char* bitmapBuffer, long width, long height, long depth, float voxelX, float voxelY, float voxelZ, long windowMin, long windowMax, long channels)
{
	CleanUpForNewVolume();//delete surfaces
	   
    AcqParam acq;										//parameters for acquisition
    CalParam cal;										//parameters for calibration
    acq.width = width;								//width of ROI (video sub region)
    acq.height = height;								//height of ROI (video sub region)
    acq.numFrames = depth;							//number of images to capture
    acq.horizontalReflection = HIR_PowerupDefault;		//no image reflection
	acq.scanDirection = SD_Clockwise;					//counter clockwise scan
    acq.probeOrientation = PO_Top;					    //bottom orientation
    if (channels == 1)
    {
        acq.scanGeometry = SG_LinearRaw;    
    }
    else
    if (channels == 2)
    {
        acq.scanGeometry = SG_Linear16;        
    }
    else
    if (channels == 3)
    {
        acq.scanGeometry = SG_Linear24;        
    }

    acq.sweepAngle = 0.0;								//no shearing

    cal.axisOfRotation = 0;//acq.width/2;					//center of ROI is center of rotation
    cal.inPlaneTilt = 0.0f;								//not currently used
    cal.outPlaneDisplacement = 0.0f;					//not currently used
    cal.outPlaneTilt = 0.0f;							//not currently used
    cal.probeDistance = 0;								//not currently used
    cal.xVoxelSize = voxelX;						//size of voxel in x direction (mm)
    cal.yVoxelSize = voxelY;						//size of voxel in y direction (mm)
    cal.zVoxelSize = 1.0;						//size of voxel in z direction (mm)

	if (cal.zVoxelSize <0.00000001)
		cal.zVoxelSize = 4.0;


    CreateVolume(acq,cal, channels);//8-bit bmp

	m_isRegMatrixSet = false;
    m_isDicomMatrixSet = false;

    

    //here, we are only storing one image into the center of the volume
	long volSize = acq.width * acq.height * channels;

    for (int i = 0; i < acq.numFrames; i++)
    {
        //float centerZ = (float)depth/2 + 1;
	    unsigned char* buffer = GetBuffer() + i*(width*height*channels);

        if (channels == 3)
        {

            bool done = false;
            long index = 0;
            while (!done)
            {
                buffer[index + 0] = bitmapBuffer[index + 2];
                buffer[index + 1] = bitmapBuffer[index + 1];
                buffer[index + 2] = bitmapBuffer[index + 0];

                index += 3;
                if (index >= volSize)
                {

                    done = true;
                }
            }

        }
        else
        if (channels == 2)
        {

            memcpy(buffer, bitmapBuffer, volSize);
        }
        else
        if (channels == 1)
        {

            memcpy(buffer, bitmapBuffer, volSize);
        }
    }
    

	SetModelView(-1, 0.001f, 0.001f, 0.001f, true);

	 
	SetWindowMinMax(windowMin, windowMax);
	GetVisualizer()->Zoom(0.7);//zoom out

    //***UpdateDisplay();

 
}



void RRI_SlicerInterface::CreateVolumeFromVtkImageData(vtkImageData *Img, long windowMin, long windowMax)
{

    std::string channelString = Img->GetScalarTypeAsString();

    long channels = 1;

    if (-1 != channelString.find("short"))
    {
        channels = 2;
    }

	CleanUpForNewVolume();//delete surfaces
	
	double spacing[3];
	int dims[3];
	Img->GetSpacing(spacing);
	Img->GetDimensions(dims);
	   
    AcqParam acq;										//parameters for acquisition
    CalParam cal;										//parameters for calibration
    acq.width = dims[0];								//width of ROI (video sub region)
    acq.height = dims[1];								//height of ROI (video sub region)
    acq.numFrames = dims[2];							//number of images to capture
    acq.horizontalReflection = HIR_PowerupDefault;		//no image reflection
	acq.scanDirection = SD_Clockwise;					//counter clockwise scan
    acq.probeOrientation = PO_Top;					    //bottom orientation
    if (channels == 1)
    {
        acq.scanGeometry = SG_LinearRaw;    
        acq.scanDirection = SD_Clockwise;
        acq.probeOrientation = PO_Top;
    }
    else
    if (channels == 2)
    {
        acq.scanGeometry = SG_Linear16;  
        acq.scanDirection = SD_Clockwise;
        acq.probeOrientation = PO_Top;
    }
    acq.sweepAngle = 0.0;								//no shearing

    cal.axisOfRotation = 0;//acq.width/2;					//center of ROI is center of rotation
    cal.inPlaneTilt = 0.0f;								//not currently used
    cal.outPlaneDisplacement = 0.0f;					//not currently used
    cal.outPlaneTilt = 0.0f;							//not currently used
    cal.probeDistance = 0;								//not currently used
    cal.xVoxelSize = spacing[0];						//size of voxel in x direction (mm)
    cal.yVoxelSize = spacing[1];						//size of voxel in y direction (mm)
    cal.zVoxelSize = spacing[2];						//size of voxel in z direction (mm)

	if (cal.zVoxelSize <0.00000001)
		cal.zVoxelSize = 4.0;


    CreateVolume(acq,cal, channels);
	m_isRegMatrixSet = false;
    m_isDicomMatrixSet = false;


	unsigned char* buffer = GetBuffer();
	long volSize = acq.width * acq.height * acq.numFrames * channels;

    if (channels == 2)
    {
	    unsigned short* vtkBuffer=(unsigned short*)Img->GetScalarPointer();
        memcpy(buffer, vtkBuffer, volSize);
    }
    else
    if (channels == 1)
    {
        unsigned char* vtkBuffer=(unsigned char*)Img->GetScalarPointer();
        memcpy(buffer, vtkBuffer, volSize);
    }
    

	SetModelView(-1, 0.001f, 0.001f, 0.001f, true);

    if (channels == 2)
	    SetWindowMinMax(windowMin, windowMax);

	GetVisualizer()->Zoom(0.7);//zoom out
 
}



//Mode 1: axial reset, Mode 2: sagittal reset, Mode 2: coronal reset
void RRI_SlicerInterface::ResetCube()
{
	
	SetModelView(-1, 0.001f, 0.001f, 0.001f, true);
}

//Mode 1: axial reset, Mode 2: sagittal reset, Mode 2: coronal reset
void RRI_SlicerInterface::ResetCube(long mode)
{
	if (mode == 1)
	{
		SetModelView(-1, 0.1f, 0.1f, 0.1f, true);
	}

	else
	if (mode == 2)
	{
		SetModelView(-1, 0.1f, 90.1f, 0.1f, true);
	}

	else
	if (mode == 3)
	{
		SetModelView(-1, 91.1f, 1.1f, 1.1f, true);
	}

    else
	if (mode == 4)
	{
	
        SetModelView(-1, 180.1f, 90.1f, 0.1f, true);
	}

    else
    if (mode == 5)
    {
       SetModelView(-1, 180.1f, 0.1f, 0.1f, true);
    }

    else
    if (mode == 6)
    {
       SetModelView(-1, 90.1f, 0.1f, 90.1f, true);
    }


}

void RRI_SlicerInterface::ClearBuffer(unsigned char value)
{
	long width = GetVolumeWidth();
	long height = GetVolumeHeight();
	long depth = GetVolumeDepth();
	unsigned char* buffer = GetBuffer();
	memset(buffer, value, width*height*depth);
}

/*
//create 8-bit binary buffer to store the initialization points for the segmentation algorithm
void RRI_SlicerInterface::CreateBinaryBuffer()
{
    Vector3 cubesize = GetVisualizer()->GetCubeSize(); //mm 
    Vector3 cubedim = GetVisualizer()->GetCubeDim(); //pixels

    if (m_binaryBuffer) GlobalFree(m_binaryBuffer);

    m_binaryBuffer = (unsigned char*)GlobalAlloc(GMEM_FIXED, cubedim.x*cubedim.y*cubedim.z);
    memset(m_binaryBuffer, 0, cubedim.x*cubedim.y*cubedim.z);//set to zero
}
*/




//convert current scan geometry to linear scan geometry
void RRI_SlicerInterface::ConvertToLinear()
{

	Vector3 cubesize = GetVisualizer()->GetCubeSize(); //mm 
	Vector3 cubedim = GetVisualizer()->GetCubeDim(); //pixels

	long width = cubedim.x;
	long height = cubedim.y;
	long depth = cubedim.z;

	float voxelX = cubesize.x / cubedim.x;
	float voxelY = cubesize.y / cubedim.y;
	float voxelZ = cubesize.z / cubedim.z;

	//acquisition parameters
	AcqParam acq;       
    acq.width = width;                              //width of ROI (video sub region)
    acq.height = height;                            //height of ROI (video sub region)
    acq.numFrames = depth;                          //number of images to capture
    acq.horizontalReflection = (eHorizontalImageReflection)HIR_PowerupDefault;	//no image reflection
    acq.scanDirection = (eScanDirection)SD_Clockwise;				//clockwise scan
    acq.probeOrientation = (eProbeOrientation)PO_Top;					//top orientation
    acq.scanGeometry = (eScanGeometry)SG_LinearRaw; //reconstruction, linear only
    acq.sweepAngle = 0.0;							//no shearing

	//calibration parameters
	CalParam cal;
	cal.axisOfRotation = 1;							//Not required for linear reconstruction
    cal.inPlaneTilt = 0.0f;							//not currently used
    cal.outPlaneDisplacement = 0.0f;				//not currently used
    cal.outPlaneTilt = 0.0f;						//not currently used
    cal.probeDistance = 0;							//not currently used
	cal.xVoxelSize = voxelX;						//size of voxel in x direction (mm)
	cal.yVoxelSize = voxelY;						//size of voxel in y direction (mm)
	cal.zVoxelSize = voxelZ;						//size of voxel in z direction (mm)

	unsigned char* newBuffer = (unsigned char*)GlobalAlloc(GMEM_FIXED, width*height*depth);
    memset(newBuffer, 0, width*height*depth);

	//fill buffer with converted data

	for (int z = 0; z<depth; z ++)
	{

		for (int y = 0; y<height; y ++)
		{

			for (int x = 0; x<width; x ++)
			{

				long bufferIndex = z*width*height + y*width + x;

				if (bufferIndex >= 0 && bufferIndex < width*height*depth)
				{
					unsigned char value = GetVisualizer()->GetVoxValue(x, y, z);
					if (value != 0)
					{
						newBuffer[bufferIndex] = value;
					}


					//unsigned char* pointer = m_reconstructedBuffer + bufferIndex;
					//memcpy(pointer, &value, 1);
				}

				
			}
		}
	}

	long channels = 1;

	CreateVolume(acq, cal, channels, newBuffer);

	GlobalFree(newBuffer);

}

void RRI_SlicerInterface::ReduceVolume(long factorX, long factorY, long factorZ)
{
	Vector3 cubesize = GetVisualizer()->GetCubeSize(); //mm 
	Vector3 cubedim = GetVisualizer()->GetCubeDim(); //pixels

	long width = cubedim.x;
	long height = cubedim.y;
	long depth = cubedim.z;

	float voxelX = cubesize.x / cubedim.x;
	float voxelY = cubesize.y / cubedim.y;
	float voxelZ = cubesize.z / cubedim.z;

	long newWidth = cubedim.x/factorX;
	long newHeight = cubedim.y/factorY;
	long newDepth = cubedim.z/factorZ;

	float newVoxelX = voxelX * (float)factorX;
	float newVoxelY = voxelY * (float)factorY;
	float newVoxelZ = voxelZ * (float)factorZ;


	//acquisition parameters
	AcqParam acq;
	acq.width = newWidth;                              //width of ROI (video sub region)
	acq.height = newHeight;                            //height of ROI (video sub region)
	acq.numFrames = newDepth;                          //number of images to capture
	acq.horizontalReflection = HIR_PowerupDefault;	//no image reflection
	acq.scanDirection = SD_Clockwise;				//clockwise scan
	acq.probeOrientation = PO_Top;					//top orientation
	acq.scanGeometry = (eScanGeometry)SG_LinearRaw; //reconstruction, linear only
	acq.sweepAngle = 0.0;							//no shearing

	//calibration parameters
	CalParam cal;
	cal.axisOfRotation = 1;							//Not required for linear reconstruction
	cal.inPlaneTilt = 0.0f;							//not currently used
	cal.outPlaneDisplacement = 0.0f;				//not currently used
	cal.outPlaneTilt = 0.0f;						//not currently used
	cal.probeDistance = 0;							//not currently used
	cal.xVoxelSize = newVoxelX;						//size of voxel in x direction (mm)
	cal.yVoxelSize = newVoxelY;						//size of voxel in y direction (mm)
	cal.zVoxelSize = newVoxelZ;						//size of voxel in z direction (mm)

	unsigned char* oldBuffer = GetBuffer();
	unsigned char* newBuffer = (unsigned char*)GlobalAlloc(GMEM_FIXED, newWidth*newHeight*newDepth);
    memset(newBuffer, 0, newWidth*newHeight*newDepth);

	//fill buffer with converted data
	long newZ = -1;
	for (int z = 0; z<depth; z+=factorZ)
	{
		newZ++;
		long newY = -1;

		for (int y = 0; y<height; y+=factorY)
		{
			newY++;
			long newX = -1;

			for (int x = 0; x<width; x+=factorX)
			{
				newX++;
				long oldBufferIndex = z*width*height + y*width + x;
				long newBufferIndex = newZ*newWidth*newHeight + newY*newWidth + newX;

				if (oldBufferIndex >= 0 && oldBufferIndex < width*height*depth && newBufferIndex >= 0 && newBufferIndex < newWidth*newHeight*newDepth)
				{
					unsigned char value = GetVisualizer()->GetVoxValue(x, y, z);
					if (value != 0)
					{
						newBuffer[newBufferIndex] = value;
					}
				}
			}
		}
	}

	long channels = 1;

	CreateVolume(acq, cal, channels, newBuffer);

	GlobalFree(newBuffer);

}

void RRI_SlicerInterface::ReduceVolume(unsigned char* buffer, long factorX, long factorY, long factorZ)
{
	Vector3 cubesize = GetVisualizer()->GetCubeSize(); //mm 
	Vector3 cubedim = GetVisualizer()->GetCubeDim(); //pixels

	long width = cubedim.x;
	long height = cubedim.y;
	long depth = cubedim.z;

	float voxelX = cubesize.x / cubedim.x;
	float voxelY = cubesize.y / cubedim.y;
	float voxelZ = cubesize.z / cubedim.z;

	long newWidth = cubedim.x / factorX;
	long newHeight = cubedim.y / factorY;
	long newDepth = cubedim.z / factorZ;

	unsigned char* oldBuffer = GetBuffer();
    unsigned char* newBuffer = buffer;//fill in buffer that we passed in

	//fill buffer with converted data
	long newZ = -1;
	for (int z = 0; z<depth; z+=factorZ)
	{
		newZ++;
		long newY = -1;

		for (int y = 0; y<height; y+=factorY)
		{
			newY++;
			long newX = -1;

			for (int x = 0; x<width; x+=factorX)
			{
				newX++;
				long oldBufferIndex = z*width*height + y*width + x;
				long newBufferIndex = newZ*newWidth*newHeight + newY*newWidth + newX;

				if (oldBufferIndex >= 0 && oldBufferIndex < width*height*depth && newBufferIndex >= 0 && newBufferIndex < newWidth*newHeight*newDepth)
				{
					unsigned char value = GetVisualizer()->GetVoxValue(x, y, z);
					//if (value != 0)
					{
						newBuffer[newBufferIndex] = value;
					}
				}
			}
		}
	}
}


void RRI_SlicerInterface::ReduceImage(unsigned char* source, unsigned char* destination, long width, long height, long reductionFactor)
{

	long newWidth = width / reductionFactor;
	long newHeight = height/ reductionFactor;
	
	unsigned char* oldBuffer = source;
    unsigned char* newBuffer = destination;//fill in buffer that we passed in

	//fill buffer with converted data
	
	long newY = -1;

	for (int y = 0; y<height; y+=reductionFactor)
	{
		newY++;
		long newX = -1;

		for (int x = 0; x<width; x+=reductionFactor)
		{
			newX++;
			long oldBufferIndex = y*width + x;
			long newBufferIndex = newY*newWidth + newX;

			if (oldBufferIndex >= 0 && oldBufferIndex < width*height && newBufferIndex >= 0 && newBufferIndex < newWidth*newHeight)
			{
				//unsigned char value = GetVisualizer()->GetVoxValue(x, y, z);
				//if (value != 0)
				{
					newBuffer[newBufferIndex] = oldBuffer[oldBufferIndex];
				}
			}
		}//for (int x = 0; x<width; x+=factorX)
	}//for (int y = 0; y<height; y+=factorY)

}


/*
//convert current scan geometry to linear scan geometry
unsigned char* RRI_SlicerInterface::ConvertToLinear(long reductionX, long reductionY, long reductionZ, long channels)
{

	Vector3 cubesize =  GetVisualizer()->GetCubeSize(); //mm 
	Vector3 cubedim  =  GetVisualizer()->GetCubeDim(); //pixels
		
	if (m_reconstructedBuffer) GlobalFree(m_reconstructedBuffer);

	m_reconstructedBuffer =  (unsigned char*)GlobalAlloc(GMEM_FIXED, cubedim.x*cubedim.y*cubedim.z*channels);

	//fill buffer with converted data
	long bufferIndex = 0;
	for (int z=0; z<cubedim.z; z+=reductionZ)
	{
		for (int y=0; y<cubedim.y; y+=reductionY)
		{
			for (int x=0; x<cubedim.x; x+=reductionX)
			{
				if (channels == 1)
				{
					tVoxel value;
					value = GetVisualizer()->GetVoxValue(x, y, z);
					m_reconstructedBuffer[bufferIndex] = value;
					bufferIndex+=channels;
				}
				else
				if (channels == 2)
				{
					tVoxel upper;
					tVoxel lower;
					GetVisualizer()->GetVoxValue(x, y, z, lower, upper);
					m_reconstructedBuffer[bufferIndex] = upper;
					m_reconstructedBuffer[bufferIndex+1] = lower;
					bufferIndex+=channels;
				}
				else
				if (channels == 3)
				{
					//::TODO::
				}
					
			}
		}
	}


	return m_reconstructedBuffer;
		
}
*/

long RRI_SlicerInterface::CreateVolume(AcqParam acq, CalParam cal, long channels, tVoxel* replacementBuffer)
{
	

    GetVisualizer()->Initialize(GetWidth(), GetHeight(), acq, cal);

	if (replacementBuffer != 0)
	{
		memcpy(GetBuffer(), replacementBuffer, acq.width*acq.height*acq.numFrames*channels);
	}


	return 0;
}

//display functions
void RRI_SlicerInterface::SetDisplayText(char* text, long textLength)
{
	m_displayTextSize = textLength;
	if (m_displayText) delete [] m_displayText;
	m_displayText = new char[textLength];
	for (int i=0; i<textLength; i++)
	{
		m_displayText[i] = text[i];
	}
}

std::string RRI_SlicerInterface::GetDisplayText()
{
	std::string temp(m_displayText, m_displayTextSize);//= m_displayText;
	return temp;
}
//display functions
void RRI_SlicerInterface::SetWindowLevelText(char* text, long textLength)
{
	m_windowLevelTextSize = textLength;
	if (m_windowLevelText) delete [] m_windowLevelText;
	m_windowLevelText = new char[textLength];
	for (int i=0; i<textLength; i++)
	{
		m_windowLevelText[i] = text[i];
	}
}

//display functions
void RRI_SlicerInterface::SetCurrentPositionText(char* text, long textLength)
{
	m_currentPositionTextSize = textLength;
	if (m_currentPositionText) delete [] m_currentPositionText;
	m_currentPositionText = new char[textLength];
	for (int i=0; i<textLength; i++)
	{
		m_currentPositionText[i] = text[i];
	}
}

#ifdef VTK_AVAILABLE
Vector3 RRI_SlicerInterface::extend_line(Vector3 ptBegin, Vector3 ptEnd, float factor)
{
    Vector3 ptOut;
    ptOut.x = ptBegin.x + (ptEnd.x - ptBegin.x) * factor;
    ptOut.y = ptBegin.y + (ptEnd.y - ptBegin.y) * factor;
    ptOut.z = ptBegin.z + (ptEnd.z - ptBegin.z) * factor;

    return ptOut;
}


void RRI_SlicerInterface::TransformUserPointsDICOM()
{
	for (int i=0; i<m_userTargets.size(); i++)
	{
		Vector3 point = m_userTargets.at(i);
		Vector3 transformedPoint = helperTransformPointDICOM(point);
	}
}


//calculated centroid on selected object
Vector3 RRI_SlicerInterface::CalculateCentroid(long selectedObject)//based on Prostate surface
{
	Vector3 centroid(0,0,0);
	SurfaceList* surfaceList = GetSurfaceList();
	if (surfaceList->size() > selectedObject)
	{
		SurfaceObject so = surfaceList->at(selectedObject);
		centroid = helperFindCentroid(&(so.triangles));
	}
	
	return centroid;
}

//calculated centroid on selected object
Vector3 RRI_SlicerInterface::FindCentroid(TriangleVec* triangles)//based on Prostate surface
{
	Vector3 centroid(0,0,0);
	
	centroid = helperFindCentroid(triangles);

	return centroid;
}


//NOTE centroid defaults to (0,0,0);
Vector3 RRI_SlicerInterface::helperTransformPointLinear(vtkTransform* trans, Vector3 point, Vector3 centroid)
{
	Vector3 transformedPoint(0,0,0);

	if (trans != NULL)
	{
		
		float inPoint[4];
		float outPoint[4];
		inPoint[0] = point.x - centroid.x;
		inPoint[1] = point.y - centroid.y;
		inPoint[2] = point.z - centroid.z;
		inPoint[3] = 1;

        trans->TransformPoint(inPoint, outPoint);

		transformedPoint.x = outPoint[0] + centroid.x;
		transformedPoint.y = outPoint[1] + centroid.y;
		transformedPoint.z = outPoint[2] + centroid.z;
	}

    return transformedPoint;

}


//NOTE centroid defaults to (0,0,0);
Vector3 RRI_SlicerInterface::helperTransformPointLinear(vtkMatrix4x4* matrix, Vector3 point, Vector3 centroid)
{
	Vector3 transformedPoint(0,0,0);

	if (matrix != NULL)
	{
		
		float inPoint[4];
		float outPoint[4];
		inPoint[0] = point.x - centroid.x;
		inPoint[1] = point.y - centroid.y;
		inPoint[2] = point.z - centroid.z;
		inPoint[3] = 1;

		matrix->MultiplyPoint(inPoint,outPoint);

		transformedPoint.x = outPoint[0] + centroid.x;
		transformedPoint.y = outPoint[1] + centroid.y;
		transformedPoint.z = outPoint[2] + centroid.z;
	}

    return transformedPoint;

}

Vector3 RRI_SlicerInterface::helperTransformPointDICOM(Vector3 point)
{
	Vector3 transformedPoint(0,0,0);

	if (m_dicomMatrix != NULL)
	{
		
		float inPoint[4];
		float outPoint[4];
		inPoint[0] = point.x;
		inPoint[1] = point.y;
		inPoint[2] = point.z;
		inPoint[3] = 1;

		m_dicomMatrix->MultiplyPoint(inPoint,outPoint);

		transformedPoint.x = outPoint[0];
		transformedPoint.y = outPoint[1];
		transformedPoint.z = outPoint[2];
	}
	else
	{
		return point;
	}

    return transformedPoint;

}


Vector3 RRI_SlicerInterface::helperTransformPointDICOMInverse(Vector3 point)
{
	Vector3 transformedPoint(0,0,0);
	if (m_dicomMatrix == NULL)
	{
		return point;
	}

	vtkMatrix4x4* matrix = vtkMatrix4x4::New();
	matrix->DeepCopy(m_dicomMatrix);

	matrix->Invert();

	float inPoint[4];
	float outPoint[4];
	inPoint[0] = point.x;
	inPoint[1] = point.y;
	inPoint[2] = point.z;
	inPoint[3] = 1;

	matrix->MultiplyPoint(inPoint,outPoint);

	transformedPoint.x = outPoint[0];
	transformedPoint.y = outPoint[1];
	transformedPoint.z = outPoint[2];

	matrix->Delete();

    return transformedPoint;

}

long RRI_SlicerInterface::helperConvertPolyDataToTriangles(vtkSmartPointer<vtkPolyData> polyData, TriangleVec* triangles, bool clear)
{
    if (!polyData) return -1;
    if (!triangles) return -2;

    if (clear)
        triangles->clear();

    vtkPoints *triPoints;
	vtkCellArray *triPolys;
	vtkDataArray *triNormals;

    long num = polyData->GetNumberOfPolys();

	triPolys = polyData->GetPolys();
	triPoints = polyData->GetPoints();
	triNormals = polyData->GetPointData()->GetNormals();
    
    vtkIdType npts, *pts;
    //int npts, *pts;
	double vert[3];
    long count = 0;
    float corners[3][3];


    Triangle triangle;
	for (triPolys->InitTraversal(); triPolys->GetNextCell(npts,pts); count++)
    { 
		for (long loopIn=0; loopIn<npts; loopIn++)
		{
            //if (loopIn > 4) break;

			triPoints->GetPoint(pts[loopIn], vert);
			corners[loopIn][0] = vert[0];
			corners[loopIn][1] = vert[1];
			corners[loopIn][2] = vert[2];
			
		}
		
		triangle.V1.x = corners[0][0];
		triangle.V1.y = corners[0][1];
		triangle.V1.z = corners[0][2];
		
		triangle.V2.x = corners[1][0];
		triangle.V2.y = corners[1][1];
		triangle.V2.z = corners[1][2];
		
		triangle.V3.x = corners[2][0];
		triangle.V3.y = corners[2][1];
		triangle.V3.z = corners[2][2];

		
		triangles->push_back(triangle);	

    }

    return 0;


}




long RRI_SlicerInterface::helperConvertPolyDataToTriangles(vtkPolyData* polyData, TriangleVec* triangles, bool clear)
{
    if (!polyData) return -1;
    if (!triangles) return -2;

    if (clear)
        triangles->clear();

    vtkPoints *triPoints;
	vtkCellArray *triPolys;
	vtkDataArray *triNormals;

    long num = polyData->GetNumberOfPolys();

	triPolys = polyData->GetPolys();
	triPoints = polyData->GetPoints();
	triNormals = polyData->GetPointData()->GetNormals();
    
    vtkIdType npts, *pts;
    //int npts, *pts;

	double vert[3];
    long count = 0;
    float corners[3][3];


    Triangle triangle;
	for (triPolys->InitTraversal(); triPolys->GetNextCell(npts,pts); count++)
    { 
		for (long loopIn=0; loopIn<npts; loopIn++)
		{
            //if (loopIn > 4) break;

			triPoints->GetPoint(pts[loopIn], vert);
			corners[loopIn][0] = vert[0];
			corners[loopIn][1] = vert[1];
			corners[loopIn][2] = vert[2];
			
		}
		
		triangle.V1.x = corners[0][0];
		triangle.V1.y = corners[0][1];
		triangle.V1.z = corners[0][2];
		
		triangle.V2.x = corners[1][0];
		triangle.V2.y = corners[1][1];
		triangle.V2.z = corners[1][2];
		
		triangle.V3.x = corners[2][0];
		triangle.V3.y = corners[2][1];
		triangle.V3.z = corners[2][2];

		
		triangles->push_back(triangle);	

    }

    return 0;


}



#ifdef SURFACE_OBJECT
void RRI_SlicerInterface::SetCurrentSurfaceObject(long index)
{
	m_currentSurfaceObject = index;
	//recreate m_target_surface object with all user points
	m_target_surface.clear();

#ifdef ADD_SURFACE_TARGETS
	if (m_currentSurfaceObject >=0)
	{
	
		SurfaceObject so = m_surfaceList.at(m_currentSurfaceObject);
		
		{
			for (int i=0; i<so.userPoints.size(); i++)
			{
				Vector3 point = so.userPoints.at(i);
				AddTargetSurface(point, m_targetSurfaceRadius);
			}
		}
	}
#endif

	//re-add user points if any were selected
	for (int i=0; i<m_userTargets.size(); i++)
	{
		Vector3 point = m_userTargets.at(i);
		AddTargetSurface(point, m_targetSurfaceRadius);

	}

}




long RRI_SlicerInterface::GeneratePlaneSurface()
{
	if (m_currentSurfaceObject < 0 || m_currentSurfaceObject >= m_surfaceList.size())
	{
		return -1;//non valid surface index
	}

	//get surface object
	SurfaceObject* so = &(m_surfaceList.at(m_currentSurfaceObject));

	//
	for (int i=0; i<m_userTargets.size(); i++)
	{
		Vector3 point = m_userTargets.at(i);
        so->userPoints.push_back(point);

	}

	m_userTargets.clear();//now that user points have been added to surface object, we can clear these
	m_target_surface.clear();//clear target surfaces from cube display

	//recreate m_target_surface object with all user points
	{
		for (int i=0; i<so->userPoints.size(); i++)
		{
			Vector3 point = so->userPoints.at(i);
			AddTargetSurface(point, m_targetSurfaceRadius);
		}
	}

	//now reconstruct the surface
	vtkPoints*  sourcePoints = vtkPoints::New();
	vtkSurfaceReconstructionFilter* reco = vtkSurfaceReconstructionFilter::New();

	for (int i=0; i<so->userPoints.size(); i++)
	{
		Vector3 point = so->userPoints.at(i);
		sourcePoints->InsertNextPoint((double)point.x, (double)point.y, (double)point.z);
	}

	vtkPolyData* polyData = vtkPolyData::New();
	polyData->SetPoints(sourcePoints);

	reco->SetInput(polyData);
	reco->Update();


	vtkContourFilter* filter = vtkContourFilter::New();
	filter->SetInput(reco->GetOutput());
	//filter->SetValue(0, 0.0); 

	vtkReverseSense* reverse = vtkReverseSense::New();
	reverse->SetInput(filter->GetOutput());
	reverse->ReverseCellsOn();
	reverse->ReverseNormalsOn();
	reverse->Update();


	vtkPolyData* newPolyData = reverse->GetOutput();//vtkPolyData::New();

	so->triangles.clear();
	TriangleVec triangles;
	HelperConvertPolyDataToTriangles(newPolyData, &(so->triangles));

	sourcePoints->Delete();
	reco->Delete();
	polyData->Delete();
	filter->Delete();
	reverse->Delete();

	return 0;
}




#ifdef NOT_WORKING
long RRI_SlicerInterface::GeneratePlaneSurface()
{
	if (m_currentSurfaceObject < 0 || m_currentSurfaceObject >= m_surfaceList.size())
	{
		return -1;//non valid surface index
	}

	//get surface object
	SurfaceObject* so = &(m_surfaceList.at(m_currentSurfaceObject));

	//
	for (int i=0; i<m_userTargets.size(); i++)
	{
		Vector3 point = m_userTargets.at(i);
        so->userPoints.push_back(point);

	}

	m_userTargets.clear();//now that user points have been added to surface object, we can clear these
	m_target_surface.clear();//clear target surfaces from cube display

	//recreate m_target_surface object with all user points
	{
		for (int i=0; i<so->userPoints.size(); i++)
		{
			Vector3 point = so->userPoints.at(i);
			AddTargetSurface(point, m_targetSurfaceRadius);
		}
	}

//==================================================================================
// find min and max user points
//==================================================================================

    double minx = 1000.0, miny = 1000.0, minz = 1000.0, maxx = -1000.0, maxy = -1000.0, maxz = -1000.0;
	Vector3 minPoint, maxPoint;
    {
        //for (int i=0; i<m_segmentationPoints.size(); i++)
        for (int i=0; i<so->userPoints.size(); i++)
        {
            Vector3 point3D = so->userPoints.at(i);

            if (point3D.x < minx) minx = point3D.x;
            if (point3D.y < miny) miny = point3D.y;
            if (point3D.z < minz) 
			{
					minz = point3D.z;
					minPoint = point3D;
			}
            if (point3D.x > maxx) maxx = point3D.x;
            if (point3D.y > maxy) maxy = point3D.y;
            if (point3D.z > maxz) 
			{
					maxz = point3D.z;
					maxPoint = point3D;
			}
        }
    }

	Vector3 cubeSize = GetVisualizer()->GetCubeSize();
    float imageSizeX = maxx-minx;
    float imageSizeY = maxy-miny;
	float imageSizeZ = maxz-minz;

	

	//Add plane to surface object
	{
		//calculate z-axis rotation
		double zLength = abs(imageSizeZ);
		double yLength = abs(imageSizeY);
		double cLength = sqrt(zLength*zLength + yLength*yLength);
		double angle = sin(zLength/yLength) / PI * 180.0;

		if (minPoint.y > maxPoint.y)
		{
			angle *= -1.0;
		}

		Vector3 center;
		center.x = (minx + maxx)/2.0;
		center.y = (miny + maxy)/2.0;
		center.z = (minz + maxz)/2.0;

		vtkPlaneSource  *plane = vtkPlaneSource::New();
		plane->SetResolution(GetVoxelX(), GetVoxelY());
		plane->SetOrigin(0, 0, 0);
		plane->SetPoint1(imageSizeX * 2.0, 0.0, 0.0);
		plane->SetPoint2( 0.0, imageSizeY * 2.0, 0);
		plane->SetCenter(0, 0, 0);

		vtkTransform* transform = vtkTransform::New();
		transform->PostMultiply();
	
		transform->RotateX(angle);
		transform->Translate(center.x, center.y, center.z);

		vtkTransformPolyDataFilter* filter = vtkTransformPolyDataFilter::New();
		filter->SetInput(plane->GetOutput());
		filter->SetTransform(transform);
		
		//apply transform
		filter->Update();

		//void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
		so->colour = RGB(128, 128, 0);//dark yellow
		so->opacity = 1.0;
		so->visible = true;

		//generate triangles for surface
		vtkPolyData* polyData = filter->GetOutput();
		vtkCellArray* cellArray = polyData->GetVerts();//get verticies if plane
		long count = polyData->GetNumberOfVerts();

		double vert[3];
		Vector3 corner1, corner2, corner3, corner4;

		polyData->GetPoint(0, vert);
		corner1.x = vert[0];
		corner1.y = vert[1];
		corner1.z = vert[2];

		polyData->GetPoint(1, vert);
		corner2.x = vert[0];
		corner2.y = vert[1];
		corner2.z = vert[2];

		polyData->GetPoint(2, vert);
		corner3.x = vert[0];
		corner3.y = vert[1];
		corner3.z = vert[2];

		polyData->GetPoint(3, vert);
		corner4.x = vert[0];
		corner4.y = vert[1];
		corner4.z = vert[2];


		Triangle t1, t2;
		t1.V1 = Vector3(corner1.x, corner1.y, corner1.z);
		t1.V2 = Vector3(corner2.x, corner2.y, corner2.z);
		t1.V3 = Vector3(corner3.x, corner3.y, corner3.z);

		t2.V1 = Vector3(corner2.x, corner2.y, corner2.z);
		t2.V2 = Vector3(corner3.x, corner3.y, corner3.z);
		t2.V3 = Vector3(corner4.x, corner4.y, corner4.z);

		so->triangles.push_back(t1);
		so->triangles.push_back(t2);


		plane->Delete();
		transform->Delete();
		filter->Delete();		

	}

	//build cell locator
	TriangleVec* tri = &(so->triangles);
	vtkPolyData* polyData = vtkPolyData::New();
	HelperConvertTrianglesToPolyData(tri, polyData, false);

	//cell locator is used to locate points on surface intersecting a line
    vtkCellLocator* locator1 = vtkCellLocator::New();
    vtkPoints*  sourcePoints = vtkPoints::New();
	vtkPoints*  targetPoints = vtkPoints::New();
    locator1->SetDataSet(polyData);
    locator1->BuildLocator();

	//for each user point, extend line, find intersection with plane
	for (int i=0; i<so->userPoints.size(); i++)
	{
		bool intersect = false;

		Vector3 point = so->userPoints.at(i);
		Vector3 extendedPoint1 = point;
		extendedPoint1.y += 1000.0;

		double point1[] = {point.x, point.y, point.z};
		double point2[] = {extendedPoint1.x, extendedPoint1.y, extendedPoint1.z};//far away point
		double t, ptline[3], pcoords[3];
		int subId;
		int result = locator1->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
		if (result == 0)
		{
			intersect = true;
		}
		else
		{
			Vector3 extendedPoint1 = point;
			extendedPoint1.y -= 1000.0;
			result = locator1->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);

			if (result == 0)
			{
				intersect = true;
			}
		}

		if (intersect)
		{

			Vector3 closestSpherePoint = Vector3(ptline[0], ptline[1], ptline[2]);

			sourcePoints->InsertNextPoint(closestSpherePoint.x, closestSpherePoint.y, closestSpherePoint.z);
			targetPoints->InsertNextPoint(point.x, point.y, point.z);
		}


	}

	//calculate transformation
	vtkThinPlateSplineTransform* trans = vtkThinPlateSplineTransform::New();

	//do transformation
	trans->SetSourceLandmarks(sourcePoints);
	trans->SetTargetLandmarks(targetPoints);
	trans->SetBasisToR();
	trans->Update();

	//transform all the plane triangles using this transform

		
    Vector3 in, out;
    double* inPoint = new double[3];
    double* outPoint = new double[3];

	//copy so triangles into temp buffer
	TriangleVec tempTriangles;
	for (int i=0; i<so->triangles.size(); i++)
    {
		Triangle tri = so->triangles.at(i);
		tempTriangles.push_back(tri);
	}


    so->triangles.clear();

    for (int i=0; i<tempTriangles.size(); i++)
    {
            
        Triangle inTriangle = tempTriangles.at(i);
        Triangle outTriangle;

        //--------------------
        in = inTriangle.V1;
        inPoint[0] = (double)in.x;
        inPoint[1] = (double)in.y;
        inPoint[2] = (double)in.z;

        trans->InternalTransformPoint(inPoint, outPoint);

        outTriangle.V1.x = outPoint[0];
        outTriangle.V1.y = outPoint[1];
        outTriangle.V1.z = outPoint[2];

        //--------------------
        in = inTriangle.V2;
        inPoint[0] = (double)in.x;
        inPoint[1] = (double)in.y;
        inPoint[2] = (double)in.z;

        trans->InternalTransformPoint(inPoint, outPoint);

        outTriangle.V2.x = outPoint[0];
        outTriangle.V2.y = outPoint[1];
        outTriangle.V2.z = outPoint[2];


        //--------------------
        in = inTriangle.V3;
        inPoint[0] = (double)in.x;
        inPoint[1] = (double)in.y;
        inPoint[2] = (double)in.z;

        trans->InternalTransformPoint(inPoint, outPoint);

        outTriangle.V3.x = outPoint[0];
        outTriangle.V3.y = outPoint[1];
        outTriangle.V3.z = outPoint[2];


        so->triangles.push_back(outTriangle);
        
    }


    delete [] inPoint;
    delete [] outPoint;

	locator1->Delete();
	sourcePoints->Delete();
	targetPoints->Delete();
	trans->Delete();
	return 0;
}
#endif

/*
//for each user point, find the closest point in the newly created plane

		double minDistance = 10000.0;
		Vector3Vec targetVec;
		targetVec.clear();
		for (int i=0; i<so->userPoints.size(); i++)
		{
			Vector3 userPoint = so->userPoints.at(i);
			//find closest point in triangle vector
			Vector3 targetPoint;
			for (int j=0; j<so->triangles.size(); j++)
			{
				Triangle tri = so->triangles.at(j);

				Vector3 V1 = tri.V1;

				double xLength = abs(userPoint.x - V1.x);
				double yLength = abs(userPoint.y - V1.y);
				double zLength = abs(userPoint.z - V1.z);
				double cLength = sqrt(xLength*xLength + yLength*yLength + zLength*zLength);
				if (cLength < minDistance)
				{
					minDistance = cLength;
					targetPoint = V1;
				}
			}

			targetVec.push_back(targetPoint);
		}
		
		vtkPoints*  sourcePoints = vtkPoints::New();
		vtkPoints*  targetPoints = vtkPoints::New();

		for (int i=0; i<so->userPoints.size(); i++)//i+=6
		{
			//Vector3 contourPoint = m_segmentationPoints.at(i);
			Vector3 sourcePoint = so->userPoints.at(i);
			Vector3 targetPoint = targetVec.at(i);

			sourcePoints->InsertNextPoint(sourcePoint.x, sourcePoint.y, sourcePoint.z);
			targetPoints->InsertNextPoint(targetPoint.x, targetPoint.y, targetPoint.z);

		}

		vtkThinPlateSplineTransform* trans = vtkThinPlateSplineTransform::New();

		//do transformation
		trans->SetSourceLandmarks(sourcePoints);
		trans->SetTargetLandmarks(targetPoints);
		trans->SetBasisToR();
		trans->Update();

		//transform all the plane triangles using this transform

		
        Vector3 in, out;
        double* inPoint = new double[3];
        double* outPoint = new double[3];

		//copy so triangles into temp buffer
		TriangleVec tempTriangles;
		for (int i=0; i<so->triangles.size(); i++)
        {
			Triangle tri = so->triangles.at(i);
			tempTriangles.push_back(tri);
		}


        so->triangles.clear();

        for (int i=0; i<tempTriangles.size(); i++)
        {
            
            Triangle inTriangle = tempTriangles.at(i);
            Triangle outTriangle;

            //--------------------
            in = inTriangle.V1;
            inPoint[0] = (double)in.x;
            inPoint[1] = (double)in.y;
            inPoint[2] = (double)in.z;

            trans->InternalTransformPoint(inPoint, outPoint);

            outTriangle.V1.x = outPoint[0];
            outTriangle.V1.y = outPoint[1];
            outTriangle.V1.z = outPoint[2];

            //--------------------
            in = inTriangle.V2;
            inPoint[0] = (double)in.x;
            inPoint[1] = (double)in.y;
            inPoint[2] = (double)in.z;

            trans->InternalTransformPoint(inPoint, outPoint);

            outTriangle.V2.x = outPoint[0];
            outTriangle.V2.y = outPoint[1];
            outTriangle.V2.z = outPoint[2];


            //--------------------
            in = inTriangle.V3;
            inPoint[0] = (double)in.x;
            inPoint[1] = (double)in.y;
            inPoint[2] = (double)in.z;

            trans->InternalTransformPoint(inPoint, outPoint);

            outTriangle.V3.x = outPoint[0];
            outTriangle.V3.y = outPoint[1];
            outTriangle.V3.z = outPoint[2];


            so->triangles.push_back(outTriangle);
        
        }


        delete [] inPoint;
        delete [] outPoint;
    

		trans->Delete();
		sourcePoints->Delete();
		targetPoints->Delete();
	}
	*/


long RRI_SlicerInterface::ExportUserPoints(std::string filePath)
{
    //create file based on file path
    std::string pointPath = filePath;
	ofstream pointFile(pointPath, ios_base::binary);

 
	for (int i=0; i<m_userTargets.size(); i++)
	{
        ostringstream pointX, pointY, pointZ;
		Vector3 point = m_userTargets.at(i);
        pointX << point.x;
        pointY << point.y;
        pointZ << point.z;

        std::string data = pointX.str() + " " + pointY.str() + " " + pointZ.str() + "\n";

        pointFile << data;//output string to file

	}

    return 0;
}



long RRI_SlicerInterface::StoreUserPointsNoSurface()
{
	if (m_currentSurfaceObject < 0 || m_currentSurfaceObject >= m_surfaceList.size())
	{
		return -1;//non valid surface index
	}

	//get surface object
	SurfaceObject* so = &(m_surfaceList.at(m_currentSurfaceObject));

	//
	for (int i=0; i<m_userTargets.size(); i++)
	{
		Vector3 point = m_userTargets.at(i);
        so->userPoints.push_back(point);

	}

	m_userTargets.clear();//now that user points have been added to surface object, we can clear these
	m_target_surface.clear();//clear target surfaces from cube display

	//recreate m_target_surface object with all user points
	{
		for (int i=0; i<so->userPoints.size(); i++)
		{
			Vector3 point = so->userPoints.at(i);
			AddTargetSurface(point, m_targetSurfaceRadius);
		}
	}

	return 0;
}


//generate selected surfacesurface based on user selected points
long RRI_SlicerInterface::GenerateSurfaceFromUserPoints()
{
	if (m_currentSurfaceObject < 0 || m_currentSurfaceObject >= m_surfaceList.size())
	{
		return -1;//non valid surface index
	}

	//get surface object
	SurfaceObject* so = &(m_surfaceList.at(m_currentSurfaceObject));

	//
	for (int i=0; i<m_userTargets.size(); i++)
	{
		Vector3 point = m_userTargets.at(i);
        so->userPoints.push_back(point);

	}

	m_userTargets.clear();//now that user points have been added to surface object, we can clear these
	m_target_surface.clear();//clear target surfaces from cube display

	//recreate m_target_surface object with all user points
	{
		for (int i=0; i<so->userPoints.size(); i++)
		{
			Vector3 point = so->userPoints.at(i);
			AddTargetSurface(point, m_targetSurfaceRadius);
		}
	}

	if (so->userPoints.size() <= 4)
	{
		return -3;//not enough points for segmentation
	}


//==================================================================================
//calculate centroid based on min,max of these points
//==================================================================================
     
    //find centroid of points
    double minx = 1000.0, miny = 1000.0, minz = 1000.0, maxx = -1000.0, maxy = -1000.0, maxz = -1000.0;
    {
        //for (int i=0; i<m_segmentationPoints.size(); i++)
        for (int i=0; i<so->userPoints.size(); i++)
        {
            Vector3 point3D = so->userPoints.at(i);

            if (point3D.x < minx) minx = point3D.x;
            if (point3D.y < miny) miny = point3D.y;
            if (point3D.z < minz) minz = point3D.z;
            if (point3D.x > maxx) maxx = point3D.x;
            if (point3D.y > maxy) maxy = point3D.y;
            if (point3D.z > maxz) maxz = point3D.z;
        }
    }

    Vector3 centroid;
    centroid.x = (minx + maxx)/2.0f;
    centroid.y = (miny + maxy)/2.0f;
    centroid.z = (minz + maxz)/2.0f;

    double sizex = maxx - minx;
    double sizey = maxy - miny;
    double sizez = maxz - minz;

    double maxsize = -1000;
    if (sizex > maxsize) maxsize = sizex;
    if (sizey > maxsize) maxsize = sizey;
    if (sizez > maxsize) maxsize = sizez;


//==================================================================================
//generate sphere around  contours. Warp the sphere to the three orthogonal contours.
//==================================================================================

    double sphereRadius = maxsize/2.0 + 3.0;
    vtkSphereSource* sphere = vtkSphereSource::New();
    sphere->SetRadius(sphereRadius);//make sure sphere is slightly larger than max contour dim.
    sphere->SetCenter(centroid.x, centroid.y, centroid.z);
    sphere->SetThetaResolution(50);
    sphere->SetPhiResolution(50);
    sphere->Update();

    vtkPolyData* sphereData = sphere->GetOutput();

    //==================================================================================
    //convert to triangles
    //==================================================================================

    TriangleVec sphereTriangles;
    helperConvertPolyDataToTriangles(sphereData, &sphereTriangles);


//==================================================================================
//for eachpoint in the contour set
// 1: extend a line from the centroid, through the point, and past the sphere
// 2: find the point in the sphere closest to the line
// 3: add sphere point to source points and contour point to target points
//==================================================================================

    //cell locator is used to locate points on surface intersecting a line
    vtkCellLocator* locator1 = vtkCellLocator::New();
    vtkPoints*  sourcePoints = vtkPoints::New();
	vtkPoints*  targetPoints = vtkPoints::New();
    locator1->SetDataSet(sphereData);
    locator1->BuildLocator();


    
 
    for (int i=0; i<so->userPoints.size(); i++)//i+=6
    {
        //Vector3 contourPoint = m_segmentationPoints.at(i);
        Vector3 contourPoint = so->userPoints.at(i);
        Vector3 extendedPoint = extend_line(centroid, contourPoint, sphereRadius + 5.0);
        double point2[] = {extendedPoint.x, extendedPoint.y, extendedPoint.z};//far away point
        double point1[] = {centroid.x, centroid.y, centroid.z};

        double t, ptline[3], pcoords[3];
        int subId;
        int result = locator1->IntersectWithLine(point1, point2, 0.001, t, ptline, pcoords, subId);
        Vector3 closestSpherePoint = Vector3(ptline[0], ptline[1], ptline[2]);

        sourcePoints->InsertNextPoint(closestSpherePoint.x, closestSpherePoint.y, closestSpherePoint.z);
        targetPoints->InsertNextPoint(contourPoint.x, contourPoint.y, contourPoint.z);

    }

   

//==================================================================================
//now apply non-linear transform to the sphere
//==================================================================================

//Now apply thin plate spline to warp the sphere to the contour set
    vtkThinPlateSplineTransform* trans = vtkThinPlateSplineTransform::New();


    //do transformation
    trans->SetSourceLandmarks(sourcePoints);
	trans->SetTargetLandmarks(targetPoints);
	trans->SetBasisToR();
	trans->Update();


//and transform the sphere points using the transform
    {
        Vector3 in, out;
        double* inPoint = new double[3];
        double* outPoint = new double[3];

        so->triangles.clear();

        for (int i=0; i<sphereTriangles.size(); i++)
        {
            
            Triangle inTriangle = sphereTriangles.at(i);
            Triangle outTriangle;

            //--------------------
            in = inTriangle.V1;
            inPoint[0] = (double)in.x;
            inPoint[1] = (double)in.y;
            inPoint[2] = (double)in.z;

            trans->InternalTransformPoint(inPoint, outPoint);

            outTriangle.V1.x = outPoint[0];
            outTriangle.V1.y = outPoint[1];
            outTriangle.V1.z = outPoint[2];

            //--------------------
            in = inTriangle.V2;
            inPoint[0] = (double)in.x;
            inPoint[1] = (double)in.y;
            inPoint[2] = (double)in.z;

            trans->InternalTransformPoint(inPoint, outPoint);

            outTriangle.V2.x = outPoint[0];
            outTriangle.V2.y = outPoint[1];
            outTriangle.V2.z = outPoint[2];


            //--------------------
            in = inTriangle.V3;
            inPoint[0] = (double)in.x;
            inPoint[1] = (double)in.y;
            inPoint[2] = (double)in.z;

            trans->InternalTransformPoint(inPoint, outPoint);

            outTriangle.V3.x = outPoint[0];
            outTriangle.V3.y = outPoint[1];
            outTriangle.V3.z = outPoint[2];


            so->triangles.push_back(outTriangle);
        
        }

        sphereTriangles.clear();//clear the temporary triangles

        delete [] inPoint;
        delete [] outPoint;
    }

    locator1->Delete();
    sphere->Delete();
    sourcePoints->Delete();
    targetPoints->Delete();
    trans->Delete();

	m_currentMode = MODE_SLICER;



	return 0;

}
#endif


void RRI_SlicerInterface::DeleteSurfaceObject(long index)
{
	m_surfaceList.erase(m_surfaceList.begin()+index);
}

void RRI_SlicerInterface::ShowSurfaceObject(long index, bool show)
{
	if (index >=0 && index < m_surfaceList.size())
	{
		SurfaceObject* so = &(m_surfaceList.at(index));
		so->visible = show;
	}

}

void RRI_SlicerInterface::AddSurfaceObject(std::string text, long type, long red, long green, long blue)
{
	/*
	TriangleVec triangles;
    Vector3Vec userPoints;
    CString name;
    COLORREF colour;//default green
    long objectType;//0:freeform, 1:parallel, 3: radial, 4: needle
    float opacity;
    float measurement;
	*/

	SurfaceObject so;
	so.name = text;
	so.objectType = type;
	so.colour = RGB(red, green, blue);
	so.opacity = 1.0;
	so.visible = true;

	m_surfaceList.push_back(so);


	m_currentSurfaceObject = m_surfaceList.size()-1;

}


void RRI_SlicerInterface::AddTemplateObject(std::string text, long type, long red, long green, long blue)
{
	/*
	TriangleVec triangles;
    Vector3Vec userPoints;
    CString name;
    COLORREF colour;//default green
    long objectType;//0:freeform, 1:parallel, 3: radial, 4: needle
    float opacity;
    float measurement;
	*/

	SurfaceObject so;
	so.name = text;
	so.objectType = type;
	so.colour = RGB(red, green, blue);
	so.opacity = 1.0;
	so.visible = true;

	m_templateList.push_back(so);

}


//find nearest template position for selected point
Vector3 RRI_SlicerInterface::FindNearestTemplatePositionYZ(Vector3 point)
{
	double minDistance = 100000.0;//invalid
	Vector3 foundPoint;
	
	for (int i=0; i<m_templateList.size(); i++)
	{
		SurfaceObject so = m_templateList.at(i);
		Vector3 hole = so.userPoints.at(0);//only need to look at entry point
		float distance = sqrt((point.x-hole.x)*(point.x-hole.x) 
                            + (point.y-hole.y)*(point.y-hole.y)
                            + (point.z-hole.z)*(point.z-hole.z));

		if (distance < minDistance)
		{
			minDistance = distance;
			foundPoint = hole;
		}

	}

	return foundPoint;

}
#endif

Vector3 RRI_SlicerInterface::FindRobotFiducial(Vector3 userPoint)
{

    float fiducialRadius = 17.0;//mm (for now)

    float voxelX = GetVoxelX();
    float voxelY = GetVoxelY();
    float voxelZ = GetVoxelZ();

    Vector3 selectedPoint = userPoint;
    float minx = selectedPoint.x - fiducialRadius;
    float maxx = selectedPoint.x + fiducialRadius;
    float miny = selectedPoint.y - fiducialRadius;
    float maxy = selectedPoint.y + fiducialRadius;
    int ysize = (int)((maxy-miny)/voxelY);
    int xsize = (int)((maxx-minx)/voxelX);

    
    int** pixelIn = 0;
    pixelIn = new int *[ysize];
    {
        for (int i=0; i<ysize; i++)
            pixelIn[i] = new int[xsize];
    }

    int** pixelOut = 0;
    pixelOut = new int *[ysize];
    {
        for (int i=0; i<ysize; i++)
            pixelOut[i] = new int[xsize];
    }


    unsigned char* buffer = GetBuffer();
    Vector3 cubeDim = GetVisualizer()->GetCubeDim();
    Vector3 cubeSize = GetVisualizer()->GetCubeSize();
    float xstart = -cubeSize.x/2.0; 
    float ystart = -cubeSize.y/2.0;
    float zstart = -cubeSize.z/2.0;


    int ypos = 0;
    int xpos = 0;
    //for (int ypos = 0; ypos < ysize; ypos++)
    for (float y=miny; y<maxy; y+= voxelY)
	{
        xpos = 0;
		//for (int xpos = 0; xpos < xsize; xpos++)
        for (float x=minx; x<maxx; x+=voxelX)
		{
            Vector3 point = Vector3(x, y, selectedPoint.z); 
           

            long xoffset = (long)((point.x - xstart)/voxelX);
            long yoffset = (long)((point.y - ystart)/voxelY);
            long zoffset = (long)((point.z - zstart)/voxelZ);

            unsigned char highValue = GetVisualizer()->GetVoxValue(xoffset, yoffset, zoffset, 1);//buffer[index+1];
            unsigned char lowValue  = GetVisualizer()->GetVoxValue(xoffset, yoffset, zoffset, 0);//buffer[index];


            int value = (int)highValue * 256 + (int)lowValue;

			//GetVisualizer()->SetVoxValue(xoffset, yoffset, zoffset,0, 0);
			//GetVisualizer()->SetVoxValue(xoffset, yoffset, zoffset,1, 0);

            if (ypos<ysize && xpos<xsize)
                pixelIn[ypos][xpos] = value;

            xpos++;

			
		}

        ypos++;
	}


	// Prepare two variables to hold the position of the centroid.
	double u_centroid = 0, v_centroid = 0;

    // Call the centroiding procedure to find the centroid of the fiducial.
 	intensityWeightedCentroid(pixelIn, xsize, ysize, true, 0, voxelX, voxelY, &u_centroid, &v_centroid, pixelOut);


    Vector3 fiducialPoint;
    fiducialPoint.x = (u_centroid) * voxelX + minx;
    fiducialPoint.y = (v_centroid) * voxelY + miny;
    fiducialPoint.z = selectedPoint.z;

    {
        for( int i = 0 ; i < ysize ; i++ )
        delete [] pixelIn[i] ;
    }
    {
        for( int i = 0 ; i < ysize ; i++ )
        delete [] pixelOut[i] ;
    }
    delete [] pixelIn;
    delete [] pixelOut;

    return fiducialPoint;
}

void RRI_SlicerInterface::intensityWeightedCentroid(int **ptrin, int sizex, int sizey, bool autothresh, int manualthreshlevel,
                               double pixsizex, double pixsizey, double *xbar, double *ybar, int **ptrout)
{
    // Inputs
    // *ptrin - pointer to the first element of the image array
    // sizex - x dimension of the image
    // sizey - y dimension of the image
    // autothresh - if true, the algorithm will automatically compute a threshold level based on the known
    //              size of the fiducials
    // manualthreshlevel - if autothresh is false, this value will be used
    // pixsizex - size of each pixel in mm in x direction
    // pixsizey - size of each pixel in mm in y direction

    // Outputs
    // *xbar, *ybar - pointers to the centroid of the image
    // *ptrout - pointer to the output image (filtered and thresholded)

    // Temporary Variables
    double mean,sum,threshlevel;  // mean of the image, sum of pixels for centroiding, threshold level to be used
    int min,max,N,Ng,idx;
    double totareamm2,fidareamm2,pbg;
    double imfilter[5][5] =       // image filter array (read in from the file imfilter.dat)
    {
        {0.0000, 0.0170, 0.0381, 0.0170, 0.0000},
        {0.0170, 0.0784, 0.0796, 0.0784, 0.0170},
        {0.0381, 0.0796, 0.0796, 0.0796, 0.0381},
        {0.0170, 0.0784, 0.0796, 0.0784, 0.0170},
        {0.0000, 0.0170, 0.0381, 0.0170, 0.0000},
    };
    int i;

    int **im;
    im = new int *[sizey];
    for (i=0; i<sizey; i++) im[i] = new int[sizex];

    int **filtim;
    filtim = new int *[sizey];
    for (i=0; i<sizey; i++) filtim[i] = new int[sizex];

    double **thresh;
    thresh = new double *[sizey];
    for (i=0; i<sizey; i++) thresh[i] = new double[sizex];

    // store image in local array
    for (i=0; i<sizey; i++)
    {
        for (int j=0; j<sizex; j++)
        {
            im[i][j] = ptrin[i][j];
        }
    }

    // pad image with zeros for ease of filter application
    for (i=0; i<2; i++)
    {
        for (int j=0; j<sizex; j++)
        {
            im[i][j] = 0;
        }
    }
    for (i=sizey-2; i<sizey; i++)
    {
        for (int j=0; j<sizex; j++)
        {
            im[i][j] = 0;
        }
    }
    for (i=0; i<sizey; i++)
    {
        for (int j=0; j<2; j++)
        {
            im[i][j] = 0;
        }
    }
    for (i=0; i<sizey; i++)
    {
        for (int j=sizex-2; j<sizex; j++)
        {
            im[i][j] = 0;
        }
    }

    // zero the filtered image array
    for (i=0; i<sizey; i++)
    {
        for (int j=0; j<sizex; j++)
        {
            filtim[i][j] = 0;
        }
    }

    // filter image using a circular filter of radius 2
    for (i=2; i<sizey-2; i++)
    {
        for (int j=2; j<sizex-2; j++)
        {
            for (int k=0; k<5; k++)
            {
                for (int l=0; l<5; l++)
                {
                    filtim[i][j] += (int)(imfilter[k][l]*((double)im[i+k-2][j+l-2]));
                }
            }
        }
    }

    if (autothresh)
    {
        // compute mean of filtered image as initial threshold in case
        // the good algorithm screws up
        mean = 0;
        for (int i=2; i<sizey-2; i++)
        {
            for (int j=2; j<sizex-2; j++)
            {
                mean += filtim[i][j];
            }
        }
        mean = mean/(double)((sizex-4)*(sizey-4));
        // use mean as threshold level
        threshlevel = mean;

        // Find max and min of filtered sub image
        min = INT_MAX;
        for (int i=0; i<sizey; i++)
        {
            for (int j=0; j<sizex; j++)
            {
                if (filtim[i][j] <= min) min = filtim[i][j];
            }
        }

        max = INT_MIN;
        for (int i=0; i<sizey; i++)
        {
            for (int j=0; j<sizex; j++)
            {
                if (filtim[i][j] >= max) max = filtim[i][j];
            }
        }

        // Compute size of filtered sub image, and number of gray
        // levels in it
        N = sizex*sizey;
        Ng = max - min + 1;

        // Compute image histogram
        double *pgvec = new double [Ng];
        for (int i=0; i<Ng; i++) pgvec[i] = 0.0;
        for (int i=0; i<sizey; i++)
        {
            for (int j=0; j<sizex; j++)
            {
                idx = filtim[i][j] - min;
                pgvec[idx]++;
            }
        }
        // Normalize the histogram
        for (int i=0; i<Ng; i++) pgvec[i] = pgvec[i]/((double)N);

        // Compute cumulative histogram
        double *cgvec;
        cgvec = new double [Ng];
        for (int i=0; i<Ng; i++)
        {
            cgvec[i] = 0;
            for (int j=0; j<=i; j++) cgvec[i] += pgvec[j];
        }
        totareamm2 = ((double)(sizex*sizey))*pixsizex*pixsizey;
        fidareamm2 = 3.14*pow(3.0,2);
        pbg = 1.0 - fidareamm2/totareamm2;

        // Find threshold value at which we get the right proportion
        for (int i=0; i<Ng; i++)
        {
            if (cgvec[i] >= pbg)
            {
                threshlevel = min + i;
                break;
            }
        }

        delete cgvec;
        delete pgvec;

    }
    else
    {
        // use manual threshold level
        threshlevel = manualthreshlevel;
    }

    // threshold image
    for (int i=0; i<sizey; i++)
    {
        for (int j=0; j<sizex; j++)
        {
            if (filtim[i][j] >= threshlevel)
            {
                thresh[i][j] = filtim[i][j];
            }
            else
            {
                thresh[i][j] = 0;
            }
            // store the result in the output array
            //ptrout[i][j] = (int)(thresh[i][j] + 0.5);
			ptrout[i][j] = (int)(thresh[i][j]);
        }
    }

    // compute centroid of filtered, thresholded image
    *xbar = 0;
    *ybar = 0;
    sum = 0;
    for (int i=0; i<sizey; i++)
    {
        for (int j=0; j<sizex; j++)
        {
            *xbar += thresh[i][j]*j; 
            *ybar += thresh[i][j]*i;
            sum += thresh[i][j];
        }
    }
    *xbar = *xbar/sum;
    *ybar = *ybar/sum;

    for (int i=0; i<sizey; i++) delete [] im[i];
    delete im;
    for (int i=0; i<sizey; i++) delete [] filtim[i];
    delete filtim;
    for (int i=0; i<sizey; i++) delete [] thresh[i];
    delete thresh;
    
}

#ifdef VTK_AVAILABLE



long RRI_SlicerInterface::ImportSurface(std::string filePath, std::string surfaceName)
{
    vtkPolyData* surfaceData = vtkPolyData::New();
    std::string xmlDataFilePath = filePath;

    if (-1 != filePath.find(".vtk"))
    {
        
	    ReplaceString(xmlDataFilePath, ".vtk", ".xml");
        vtkPolyDataReader*  surfaceReader = vtkPolyDataReader::New();
        surfaceReader->SetFileName(filePath.c_str());
        surfaceReader->Update();
        surfaceData->DeepCopy(surfaceReader->GetOutput());
    }
    else
    if (-1 != filePath.find(".stl"))
    {
	    ReplaceString(xmlDataFilePath, ".stl", ".xml");
        vtkSTLReader*  surfaceReader = vtkSTLReader::New();
        surfaceReader->SetFileName(filePath.c_str());
        surfaceReader->Update();
        surfaceData->DeepCopy(surfaceReader->GetOutput());
    }
	

    
	/*
	TriangleVec triangles;
    Vector3Vec userPoints;
    std::string name;
    COLORREF colour;//default green
    long objectType;//0:freeform, 1:parallel, 3: radial, 4: needle
    float opacity;
	bool visible;
    float measurement;
	*/



	SurfaceObject so;
    so.name = surfaceName;//modify name to indicate that these are registered surfaces
    so.objectType = 0;//freeform segmentation surface
    so.opacity = 1.0f;
	so.colour = RGB(  0, 255, 0 );
	so.visible = true;

	long userPointCount = 0;

    helperConvertPolyDataToTriangles(surfaceData, &(so.triangles));

	//read colour from xml file
	//sscanf (sentence,"%s %*s %d",str,&i);
	std::ifstream  metaFile(xmlDataFilePath);
	if (metaFile.is_open())
	{
		
		std::string line, name;
		//get a list of all the surfaces to load
		while ( metaFile.good() )
		{
			getline (metaFile,line);

			name = line;
			
			
			std::string stringToFind1 = "<SegmentationColour>";
			std::string stringToFind2 = "<SegmentationName>";
			std::string stringToFind3 = "<Visible>";
			std::string stringToFind4 = "<UserPoint";
            std::string stringToFind5 = "<ObjectType>";

			if (-1 != line.find(stringToFind1))
			{
				std::string startTag = "<SegmentationColour>";
				ReplaceString(name, startTag, "");

				std::string endTag = "</SegmentationColour>";
				ReplaceString(name, endTag, "");

				//parse what's left of the string
				int red, green, blue;
				sscanf_s(name.c_str(),"%d %d %d",&red, &green, &blue);
				COLORREF colour = RGB(red, green, blue);
				so.colour = colour;

			}
			else
			if (-1 != line.find(stringToFind2))
			{
				std::string startTag = "<SegmentationName>";
				ReplaceString(name, startTag, "");

				std::string endTag = "</SegmentationName>";
				ReplaceString(name, endTag, "");

				std::string spaceTag = "    ";
				ReplaceString(name, spaceTag, "");

				so.name = name;
			}
			else//is visible
			if (-1 != line.find(stringToFind3))
			{
				if (-1 != name.find("true"))
					so.visible = true;
				else
					so.visible = false;
			}
			else//user points
			if (-1 != line.find(stringToFind4))
			{
				userPointCount++;
				ostringstream convert;
				convert << (userPointCount);
				std::string userPointCountString = convert.str();

				float userX, userY, userZ;
				ReplaceString(name, "<UserPoint", "");
				ReplaceString(name, userPointCountString, "");
				ReplaceString(name, ">", "");
				ReplaceString(name, "</UserPoint", "");
				ReplaceString(name, userPointCountString+">", "");
				ReplaceString(name, "    ", "");

				sscanf_s(name.c_str(),"%f %f %f", &userX, &userY, &userZ);
				Vector3 point(userX,userY,userZ);
				so.userPoints.push_back(point);

			}
            else
            if (-1 != line.find(stringToFind5))
			{
				std::string startTag = "<ObjectType>";
				ReplaceString(name, startTag, "");

				std::string endTag = "</ObjectType>";
				ReplaceString(name, endTag, "");

				//parse what's left of the string
				int objectType;
				sscanf_s(name.c_str(),"%d",&objectType);
                so.objectType = objectType;
				
			}
			
		}

		 metaFile.close();

	}



	m_surfaceList.push_back(so);

	surfaceData->Delete();


	return 0;
}



long RRI_SlicerInterface::ImportSurfaces(std::string folderPath, std::string filePath)
{
	//this is the file path of the meta file containing the list of all surfaces in the surface folder.

	m_surfaceList.clear();//assume are loading in new surfaces for now

	std::ifstream  metaFile(filePath);
	if (metaFile.is_open())
	{
		long surfaceCount = 0;
		std::string line, name;
		//get a list of all the surfaces to load
		while ( metaFile.good() )
		{
			getline (metaFile,line);

			if (-1 != line.find("<Surface"))
			{
				name = line;

				surfaceCount++;
				ostringstream convert;
				convert << (surfaceCount);
				std::string surfaceCountString = convert.str();

				std::string subString1 = "<Surface" + surfaceCountString + ">";
				ReplaceString(name, subString1, "");

				std::string subString2 = "</Surface" + surfaceCountString + ">";
				ReplaceString(name, subString2, "");

				std::string surfaceFilePath = folderPath + "/Surfaces_" + m_volumeLabel + "/" + name;

				ImportSurface(surfaceFilePath, "");

			}
			
		}
		 metaFile.close();

	}

	//ImportMetaData(folderPath, filePath);

	return 0;
}


long RRI_SlicerInterface::ImportMetaData(std::string folderPath, std::string filePath)
{
    long count = 0;
	std::ifstream  metaFile(filePath);
	if (metaFile.is_open())
	{
		long surfaceCount = 0;
		std::string line, name;
		//get a list of all the surfaces to load
		while ( metaFile.good() )
		{
			getline (metaFile,line);

#define IMPORT_MANUAL_TRANSFORMS	
#ifdef IMPORT_MANUAL_TRANSFORMS
			//if manually registered, use these transforms
            //NOTE: manual registration overrided DICOM registration
            //PreIntraRegTo
			{
                if (-1 != line.find("<PreIntraRegTo>"))
                {
                    name = line;

					std::string subString1 = "<PreIntraRegTo>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraRegTo>";
					ReplaceString(name, subString2, "");

                    m_registrationLabel = name;

                    m_isRegMatrixSet = true;
                }
                else
				if (-1 != line.find("<PreIntraReg1>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg1>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg1>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(0,0,value);

				}
				else
				if (-1 != line.find("<PreIntraReg2>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg2>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg2>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(0,1,value);

				}
				else
				if (-1 != line.find("<PreIntraReg3>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg3>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg3>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(0,2,value);

				}
				else
				if (-1 != line.find("<PreIntraReg4>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg4>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg4>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(0,3,value);

				}
				//----------------------------------------------------------
				if (-1 != line.find("<PreIntraReg5>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg5>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg5>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(1,0,value);

				}
				else
				if (-1 != line.find("<PreIntraReg6>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg6>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg6>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(1,1,value);

				}
				else
				if (-1 != line.find("<PreIntraReg7>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg7>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg7>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(1,2,value);

				}
				else
				if (-1 != line.find("<PreIntraReg8>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg8>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg8>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(1,3,value);

				}
				//----------------------------------------------

				if (-1 != line.find("<PreIntraReg9>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg9>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg9>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(2,0,value);

				}
				else
				if (-1 != line.find("<PreIntraReg10>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg10>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg10>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(2,1,value);

				}
				else
				if (-1 != line.find("<PreIntraReg11>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg11>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg11>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(2,2,value);

				}
				else
				if (-1 != line.find("<PreIntraReg12>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg12>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg12>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(2,3,value);

				}
				//------------------------------------------------
				if (-1 != line.find("<PreIntraReg13>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg13>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg13>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(3,0,value);

				}
				else
				if (-1 != line.find("<PreIntraReg14>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg14>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg14>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(3,1,value);

				}
				else
				if (-1 != line.find("<PreIntraReg15>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg15>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg15>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(3,2,value);

				}
				else
				if (-1 != line.find("<PreIntraReg16>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<PreIntraReg16>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</PreIntraReg16>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_regMatrix->SetElement(3,3,value);

				}

			}
#endif
#define IMPORT_DICOM_TRANSFORMS	
#ifdef IMPORT_DICOM_TRANSFORMS
			//if manually registered, use these transforms
            //NOTE: manual registration overrided DICOM registration
            //PreIntraRegTo
			{
                
				if (-1 != line.find("<DicomReg1>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg1>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg1>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(0,0,value);

                    

				}
				else
				if (-1 != line.find("<DicomReg2>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg2>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg2>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(0,1,value);

				}
				else
				if (-1 != line.find("<DicomReg3>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg3>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg3>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(0,2,value);

				}
				else
				if (-1 != line.find("<DicomReg4>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg4>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg4>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(0,3,value);

				}
				//----------------------------------------------------------
				if (-1 != line.find("<DicomReg5>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg5>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg5>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(1,0,value);

				}
				else
				if (-1 != line.find("<DicomReg6>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg6>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg6>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(1,1,value);

				}
				else
				if (-1 != line.find("<DicomReg7>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg7>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg7>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(1,2,value);

				}
				else
				if (-1 != line.find("<DicomReg8>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg8>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg8>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(1,3,value);

				}
				//----------------------------------------------

				if (-1 != line.find("<DicomReg9>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg9>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg9>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(2,0,value);

				}
				else
				if (-1 != line.find("<DicomReg10>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg10>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg10>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(2,1,value);

				}
				else
				if (-1 != line.find("<DicomReg11>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg11>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg11>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(2,2,value);

				}
				else
				if (-1 != line.find("<DicomReg12>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg12>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg12>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(2,3,value);

				}
				//------------------------------------------------
				if (-1 != line.find("<DicomReg13>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg13>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg13>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(3,0,value);

				}
				else
				if (-1 != line.find("<DicomReg14>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg14>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg14>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(3,1,value);

				}
				else
				if (-1 != line.find("<DicomReg15>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg15>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg15>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(3,2,value);

				}
				else
				if (-1 != line.find("<DicomReg16>"))
				{
                    count++;

					name = line;

					std::string subString1 = "<DicomReg16>";
					ReplaceString(name, subString1, "");

					std::string subString2 = "</DicomReg16>";
					ReplaceString(name, subString2, "");

					double value;
					sscanf_s(name.c_str(),"%lf",&value);
					m_dicomMatrix->SetElement(3,3,value);

                    m_isDicomMatrixSet = true;

				}

			}
#endif
			
			if (-1 != line.find("<Window"))
			{
				name = line;

				std::string subString1 = "<Window>";
				ReplaceString(name, subString1, "");

				std::string subString2 = "</Window>";
				ReplaceString(name, subString2, "");


				sscanf_s(name.c_str(),"%ld",&m_window);

				SetWindowLevel(m_window, m_level);

			}
			else
			if (-1 != line.find("<Level"))
			{
				name = line;

				std::string subString1 = "<Level>";
				ReplaceString(name, subString1, "");

				std::string subString2 = "</Level>";
				ReplaceString(name, subString2, "");


				sscanf_s(name.c_str(),"%ld",&m_level);
				SetWindowLevel(m_window, m_level);
			}
			
		}
		 metaFile.close();

	}


	if (GetScanGeometry()==SG_Linear16)
	{
		long windowMin = m_level - m_window/2;
		long windowMax = m_level + m_window/2;
		SetWindowMinMax(windowMin, windowMax);
	}

	return 0;
}


long RRI_SlicerInterface::ImportMatrixFromFile(std::string filePath, vtkMatrix4x4* matrix)
{

    return 0;
}


long RRI_SlicerInterface::ExportMatrixToFile(std::string filePath, vtkMatrix4x4* matrix)
{

#ifdef SAVE_TO_VTK_FORMAT
    vtkIndent* indent = vtkIndent::New();
    std::string outputFilePath = filePath;
	ofstream outputFile(outputFilePath, ios_base::binary);
    matrix->PrintSelf(outputFile, *indent);

    indent->Delete();

    return 0;
#endif

	std::string matrixFilePath = filePath;
	ofstream matrixFile(matrixFilePath, ios_base::binary);
	float element1, element2, element3, element4;
    {

        matrixFile << "Linear Transform\n\n";

//#define USE_SPRINT_F
#define EXPORT_MATRIX_FILE
#ifdef EXPORT_MATRIX_FILE

#ifdef USE_SPRINT_F
		std::string data;
		data.resize(256);
#endif

		//ROW 1
		{
			ostringstream matrix1, matrix2, matrix3, matrix4;
			
#ifdef USE_SPRINT_F
			char buff[256];
			std::string data;
			sprintf("%3.3f %3.3f %3.3f %3.3f\n", &buff[0],
				(float)matrix->GetElement(0,0), 
				(float)matrix->GetElement(1,0), 
				(float)matrix->GetElement(2,0), 
				(float)matrix->GetElement(3,0));

			data = buff;
			matrixFile << data;//output string to file
#endif          
#ifndef USE_SPRINT_F
			element1 = (float)matrix->GetElement(0, 0);
			element2 = (float)matrix->GetElement(1, 0);
			element3 = (float)matrix->GetElement(2, 0);
			element4 = (float)matrix->GetElement(3, 0);

			if (fabs(element1) < 0.00001) element1 = 0.0f;
			if (fabs(element2) < 0.00001) element2 = 0.0f;
			if (fabs(element3) < 0.00001) element3 = 0.0f;
			if (fabs(element4) < 0.00001) element4 = 0.0f;


			matrix1 << element1;
			matrix2 << element2;
			matrix3 << element3;
			matrix4 << element4;

            std::string data = matrix1.str() + " " + matrix2.str() + " " + matrix3.str() + " " + matrix4.str() + "\n";
			matrixFile << data;//output string to file
#endif
		}

		//ROW 2
		{
			ostringstream matrix1, matrix2, matrix3, matrix4;


#ifdef USE_SPRINT_F
         
			char buff[256];
			sprintf("%3.3f %3.3f %3.3f %3.3f\n", &buff[0],
				(float)matrix->GetElement(0, 0),
				(float)matrix->GetElement(1, 0),
				(float)matrix->GetElement(2, 0),
				(float)matrix->GetElement(3, 0));

			data = buff;

			matrixFile << data;//output string to file
#endif

#ifndef USE_SPRINT_F
			element1 = (float)matrix->GetElement(0, 1);
			element2 = (float)matrix->GetElement(1, 1);
			element3 = (float)matrix->GetElement(2, 1);
			element4 = (float)matrix->GetElement(3, 1);

			if (fabs(element1) < 0.00001) element1 = 0.0f;
			if (fabs(element2) < 0.00001) element2 = 0.0f;
			if (fabs(element3) < 0.00001) element3 = 0.0f;
			if (fabs(element4) < 0.00001) element4 = 0.0f;


			matrix1 << element1;
			matrix2 << element2;
			matrix3 << element3;
			matrix4 << element4;
			//construct string to write to file
			std::string data = matrix1.str() + " " + matrix2.str() + " " + matrix3.str() + " " + matrix4.str() + "\n";
			matrixFile << data;//output string to file
#endif
		}

		//ROW 3
		{
			ostringstream matrix1, matrix2, matrix3, matrix4;


#ifdef USE_SPRINT_F

			char buff[256];
			sprintf("%3.3f %3.3f %3.3f %3.3f\n", &buff[0],
				(float)matrix->GetElement(0, 0),
				(float)matrix->GetElement(1, 0),
				(float)matrix->GetElement(2, 0),
				(float)matrix->GetElement(3, 0));

			data = buff;
			matrixFile << data;//output string to file
#endif

#ifndef USE_SPRINT_F
			element1 = (float)matrix->GetElement(0, 2);
			element2 = (float)matrix->GetElement(1, 2);
			element3 = (float)matrix->GetElement(2, 2);
			element4 = (float)matrix->GetElement(3, 2);

			if (fabs(element1) < 0.00001) element1 = 0.0f;
			if (fabs(element2) < 0.00001) element2 = 0.0f;
			if (fabs(element3) < 0.00001) element3 = 0.0f;
			if (fabs(element4) < 0.00001) element4 = 0.0f;


			matrix1 << element1;
			matrix2 << element2;
			matrix3 << element3;
			matrix4 << element4;
			//construct string to write to file
			std::string data = matrix1.str() + " " + matrix2.str() + " " + matrix3.str() + " " + matrix4.str() + "\n";
			matrixFile << data;//output string to file
#endif
		}

		//ROW 4
		{
			ostringstream matrix1, matrix2, matrix3, matrix4;


#ifdef USE_SPRINT_F

			char buff[256];
			sprintf("%3.3f %3.3f %3.3f %3.3f\n", &buff[0],
				(float)matrix->GetElement(0, 0),
				(float)matrix->GetElement(1, 0),
				(float)matrix->GetElement(2, 0),
				(float)matrix->GetElement(3, 0));

			data = buff;
			matrixFile << data;//output string to file
#endif

#ifndef USE_SPRINT_F

			element1 = (float)matrix->GetElement(0, 3);
			element2 = (float)matrix->GetElement(1, 3);
			element3 = (float)matrix->GetElement(2, 3);
			element4 = (float)matrix->GetElement(3, 3);

			if (fabs(element1) < 0.00001) element1 = 0.0f;
			if (fabs(element2) < 0.00001) element2 = 0.0f;
			if (fabs(element3) < 0.00001) element3 = 0.0f;
			if (fabs(element4) < 0.00001) element4 = 0.0f;


			matrix1 << element1;
			matrix2 << element2;
			matrix3 << element3;
			matrix4 << element4;
			//construct string to write to file

			std::string data = matrix1.str() + " " + matrix2.str() + " " + matrix3.str() + " " + matrix4.str() + "\n";
			matrixFile << data;//output string to file
#endif
		}
#endif

    }

	return 0;
}

//export surfaces that contain specified string
long RRI_SlicerInterface::ExportSurface(std::string folderPath, std::string contains)
{
  
	long surfaceListSize = m_surfaceList.size();

    if (surfaceListSize <=0)
    {
        return -1;//no surfaces to export
    }

	for (int i=0; i<surfaceListSize; i++)
	{
		SurfaceObject so = m_surfaceList.at(i);
		if (-1 != so.name.find(contains))
		{
			std::string vtkFilePath = "c:/BBKingResources/Data/" + so.name + ".vtk";
			//convert triangles to polydata
			vtkPolyData     *polyData   = vtkPolyData::New();
			if (0!=helperConvertTrianglesToPolyData(&(so.triangles), polyData, false))
			{
				polyData->Delete();
				return -1;
			}
			else
			{
				//export surface in.vtk format
				vtkPolyDataWriter* polyWriter = vtkPolyDataWriter::New();
				polyWriter->SetInput(polyData);
				polyWriter->SetFileName(vtkFilePath.c_str());
				polyWriter->SetFileTypeToBinary();
				polyWriter->Write();
				polyWriter->Update();
				polyWriter->Delete();

			}

			polyData->Delete();
		}
	}

	return 0;
}


//Export all surfaces from SurfaceList vector of SurfaceObjects.
//This will generate one .vtk file and one .xml file for each surface in the list.
//The .vtk file contains the surface data.
//The .xml file contains user points, surface name and surface colour.
//Write the MetaXML file that contains references to all the surfaces
//this also stores the window and level into this file (this is turning out to be a volume state file)
long RRI_SlicerInterface::ExportSurfaces(std::string folderPath, bool useDicomTransform) 
{
   
	std::string temp;

	//label assigned to this volume
    std::string xmlMetaFilePath = folderPath + "/" +  m_volumeLabel + ".xml";
	ofstream metaFile(xmlMetaFilePath, ios_base::binary);
	metaFile << "<MetaSurfaceData>\n";


    //output window and level
	ostringstream convert1;
	convert1 << (int)(m_window);
	temp = "    <Window>" + convert1.str() + "</Window>\n";
	metaFile << temp;
	ostringstream convert2;
	convert2 << (int)(m_level);
	temp = "    <Level>" + convert2.str() + "</Level>\n";
	metaFile << temp;
	
	//set the rotation part of the matrix
    long surfaceListSize = m_surfaceList.size();


//NOTE: DICOM matrix goes from view to world coordinates
#define EXPORT_DICOM_MATRIX
#ifdef EXPORT_DICOM_MATRIX
    if (IsDicomMatrixSet())
    {
        int count = 0;
	    for (int i=0; i<4; i++)
	    {
		    double value1  = m_dicomMatrix->GetElement(i, 0);

		    {
			    count++;
			    ostringstream valueString, countString;
			    valueString << (double)value1;
			    countString << count;
			    temp = "    <DicomReg" + countString.str() + ">" + valueString.str() + "</DicomReg" + countString.str() + ">\n";
			    metaFile << temp;
		    }


	        double value2 = m_dicomMatrix->GetElement(i, 1);

		    {
			    count++;
			    ostringstream valueString, countString;
			    valueString << (double)value2;
			    countString << count;
			    temp = "    <DicomReg" + countString.str() + ">" + valueString.str() + "</DicomReg" + countString.str() + ">\n";
			    metaFile << temp;
		    }

		    double value3 = m_dicomMatrix->GetElement(i, 2);

		    {
			    count++;
			    ostringstream valueString, countString;
			    valueString << (double)value3;
			    countString << count;
			    temp = "    <DicomReg" + countString.str() + ">" + valueString.str() + "</DicomReg" + countString.str() + ">\n";
			    metaFile << temp;
		    }

		    double value4 = m_dicomMatrix->GetElement(i, 3);

		    {
			    count++;
			    ostringstream valueString, countString;
			    valueString << (double)value4;
			    countString << count;
			    temp = "    <DicomReg" + countString.str() + ">" + valueString.str() + "</DicomReg" + countString.str() + ">\n";
			    metaFile << temp;
		    }

        }
    }
#endif

    //NOTE: manual registration should override DICOM registration
    //manual matrix goes from one view to another view
    if (IsManuallyRegistered())
    {
        temp = "    <PreIntraRegTo>" + m_registrationLabel + "</PreIntraRegTo>\n";//label which scan this was registered to
	    metaFile << temp;


        int count = 0;
	    for (int i=0; i<4; i++)
	    {
		    double value1  = m_regMatrix->GetElement(i, 0);

		    {
			    count++;
			    ostringstream valueString, countString;
			    valueString << (double)value1;
			    countString << count;
			    temp = "    <PreIntraReg" + countString.str() + ">" + valueString.str() + "</PreIntraReg" + countString.str() + ">\n";
			    metaFile << temp;
		    }


	        double value2 = m_regMatrix->GetElement(i, 1);

		    {
			    count++;
			    ostringstream valueString, countString;
			    valueString << (double)value2;
			    countString << count;
			    temp = "    <PreIntraReg" + countString.str() + ">" + valueString.str() + "</PreIntraReg" + countString.str() + ">\n";
			    metaFile << temp;
		    }

		    double value3 = m_regMatrix->GetElement(i, 2);

		    {
			    count++;
			    ostringstream valueString, countString;
			    valueString << (double)value3;
			    countString << count;
			    temp = "    <PreIntraReg" + countString.str() + ">" + valueString.str() + "</PreIntraReg" + countString.str() + ">\n";
			    metaFile << temp;
		    }

		    double value4 = m_regMatrix->GetElement(i, 3);

		    {
			    count++;
			    ostringstream valueString, countString;
			    valueString << (double)value4;
			    countString << count;
			    temp = "    <PreIntraReg" + countString.str() + ">" + valueString.str() + "</PreIntraReg" + countString.str() + ">\n";
			    metaFile << temp;
		    }

        }

	}

    if (surfaceListSize <=0)
    {
        metaFile << "</MetaSurfaceData>\n";
        return -1;//no surfaces to export
    }

    for (int s=0; s<surfaceListSize; s++)
    {
        
        SurfaceObject so = m_surfaceList.at(s);
        std::string surfaceName = so.name;
        COLORREF surfaceColour = so.colour;
        long objectType = so.objectType;

		//convert triangles to polydata
        vtkPolyData     *polyData   = vtkPolyData::New();
        if (0!=helperConvertTrianglesToPolyData(&(so.triangles), polyData, false))
        {
            
            polyData->Delete();
            return -1;

        }


        {
			//create sub directory called Surfaces
            std::string surfaceFolder = folderPath + "/Surfaces_" + m_volumeLabel + "/";
			//CreateDirectory((LPCWSTR)surfaceFolder.c_str(), NULL);
            CreateDirectory((LPCSTR)surfaceFolder.c_str(), NULL);

			//
            std::string vtkFilePath = surfaceFolder + surfaceName  + "_" + m_volumeLabel + ".vtk";

            //add DICOM to file name if using DICOM transform
            if (useDicomTransform)
            {
                vtkFilePath = surfaceFolder + surfaceName + "_" + m_volumeLabel + "_DICOM" + ".vtk";
            }
            

            //export surface in.vtk format
            vtkPolyDataWriter* polyWriter = vtkPolyDataWriter::New();
            polyWriter->SetInput(polyData);
	        polyWriter->SetFileName(vtkFilePath.c_str());
	        polyWriter->SetFileTypeToBinary();
	        polyWriter->Write();
            polyWriter->Update();
            polyWriter->Delete();
            
        
            //now write this name to the meta xml surface file
            {
				ReplaceString(vtkFilePath, "\\", "/");
                long index = vtkFilePath.find_last_of('/');//find last slash
				std::string directory = vtkFilePath.substr(0,index+1);
				std::string name = vtkFilePath.substr(index+1, vtkFilePath.npos);
				std::string indexString;
				{
					ostringstream convert;
					convert << (s+1);
					//construct string to write to file
					std::string data = "<Surface" + convert.str() + ">"+name+"</Surface" + convert.str() + ">\n";
					metaFile << data;//output string to file
				}
              

            }

            //export user selected points for segmentation
			ReplaceString(vtkFilePath, ".vtk", ".xml");
			ofstream xmlFile(vtkFilePath, ios_base::binary);
            {

				

               
			   xmlFile << "<SegmentationUserPoints>\n";

//segmentation name
              
			   temp = "    <SegmentationName>" + surfaceName + "</SegmentationName>\n";
			   xmlFile << temp;

//object type

			   {
				   ostringstream convert;
				   convert << (objectType);
				   temp = "    <ObjectType>" + convert.str() + "</ObjectType>\n";
				    xmlFile << temp;
			   }

			  
//object colour
                unsigned char red = GetRValue(surfaceColour);
                unsigned char green = GetGValue(surfaceColour);
                unsigned char blue = GetBValue(surfaceColour);
				std::string redString;
				std::string greenString;
				std::string blueString;

				{
					ostringstream convert;
					convert << (int)(red);
					redString = convert.str();
				}
				{
					ostringstream convert;
					convert << (int)(green);
					greenString = convert.str();
				}
				{
					ostringstream convert;
					convert << (int)(blue);
					blueString = convert.str();
					temp = "    <SegmentationColour>" + redString + " " + greenString + " " + blueString + "</SegmentationColour>\n";
					xmlFile << temp;
				}
		
//object visibility
				 {
					 if (so.visible == true)
						temp = "    <Visible>true</Visible>\n";
					 else
						temp = "    <Visible>false</Visible>\n";
					 xmlFile << temp;
				 }
    
                for (int i=0; i<so.userPoints.size(); i++)
                {
					ostringstream convert1;
					convert1 << (i+1);
					std::string indexString = convert1.str();

					ostringstream convert2;
                    Vector3 point = so.userPoints.at(i);
					convert2 << point.x;
					std::string xString = convert2.str();

					ostringstream convert3;
					convert3 << point.y;
					std::string yString = convert3.str();

					ostringstream convert4;
					convert4 << point.z;
					std::string zString = convert4.str();

					temp = "    <UserPoint" + indexString + ">" + xString + " " + yString + " " + zString + "</UserPoint" + indexString + ">\n";
					xmlFile << temp;
                   
                }

			    temp = "</SegmentationUserPoints>\n";
			    xmlFile << temp;
            }
        
        }

        polyData->Delete();


    }


	

	metaFile << "</MetaSurfaceData>\n";
 
	return 0;
}

#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkStripper.h"

long RRI_SlicerInterface::ExportVOI(std::string filePath, TriangleVec* triangles)
{
	long numberOfTargets = 0;

    //calculate centroid of surface
    Vector3 centroid3D = FindCentroid(triangles);
    
//---------------------------------------------------------------
    SetModelView(-1, 0.001f, 0.001f, 0.001f);
    Zoom(0.8);

    long width = GetWidth();
    long height = GetHeight();
    long px = width/2;
    long py = height/2;

    long activeFace = GetVisualizer()->GetFaceID(px,py);
    Slice(activeFace, -1.0);//slice in my 1mm, prime the pump
    UpdateDisplay();

//-------------------------------------------------------------------------------------------
//convert triangles to vtkPolyData
//--------------------------------------------------------------------------------------------

 // get surface data into vtk pipeline
    vtkPoints *points = vtkPoints::New();
    vtkCellArray *polys = vtkCellArray::New();
    vtkPolyData *surface = vtkPolyData::New();


    Triangle triangle;

    long pointNumber = 0;
    for(long in = 0; in < triangles->size(); in++)
    {
        triangle = triangles->at(in);//[in];

        polys->InsertNextCell(3);
        points->InsertPoint(pointNumber, triangle.V1.x, triangle.V1.y, triangle.V1.z);
        polys->InsertCellPoint(pointNumber++);
        points->InsertPoint(pointNumber, triangle.V2.x, triangle.V2.y, triangle.V2.z);
        polys->InsertCellPoint(pointNumber++);
        points->InsertPoint(pointNumber, triangle.V3.x, triangle.V3.y, triangle.V3.z); 
        polys->InsertCellPoint(pointNumber++);
    }




    //put points and polygons into vtkPolyData object called cube
    surface->SetPoints(points);
    surface->SetPolys(polys);

    vtkPlane* plane = vtkPlane::New();
    vtkCutter* cutter = vtkCutter::New();
    vtkStripper* stripper = vtkStripper::New();

    cutter->SetInput(surface);
    cutter->SetValue(0, 0.0);


    float spacing = GetVoxelZ() * 2.0;
//--------------------------------------------------------------------------------------------
//cut parallel slices at 1mm intervals out of surface
//--------------------------------------------------------------------------------------------
    Vector3 cubeSize = GetVisualizer()->GetCubeSize();
    long face = GetVisualizer()->GetFaceID(px,py);//get current face

    Vector3VecVec surfaceVector;

    long startSlice = -1;//invalid until first non-empty slice is found.
    long endSlice = -1;

    long numberOfContours = cubeSize.z / spacing;
    for (int i=0; i<numberOfContours; i++)
    {
        //slice in, get new face, calculate normal
        Slice(face, -spacing);

        face = GetVisualizer()->GetFaceID(px,py);//get current face
        Vector3 normal = GetVisualizer()->GetNormal(face);
        
        //use centroid to calculate mid point for contour
        Vector3 centroid2D = GetVolumeToScreen(centroid3D);
        
        Vector3 midPoint3D;
		GetVisualizer()->ScreenToModel(face, centroid2D.x, centroid2D.y, &midPoint3D);

        UpdateDisplay();//render visualizer

        //setup cutter and stripper to regenerate contour from triangle surface
        plane->SetOrigin(midPoint3D.x, midPoint3D.y, midPoint3D.z);
        plane->SetNormal(normal.x, normal.y, normal.z);
        cutter->SetCutFunction(plane);
        stripper->SetInput(cutter->GetOutput());
        stripper->Update();

        // get points and lines out of pipeline
        vtkPoints *outpoints = stripper->GetOutput()->GetPoints();
        vtkCellArray *outlines = stripper->GetOutput()->GetLines();

        int npts = 0, *pts; double vert[3];
        long count = 0;

        Vector3Vec contourVector;
        Vector3 modelPt;
        if (outpoints)
        {
            for (int j=0; j<outpoints->GetNumberOfPoints(); j++)
            {
                outpoints->GetPoint(j, vert);
                modelPt.x = vert[0];
                modelPt.y = vert[1];
                modelPt.z = vert[2];

                //if we need to convert to image coordinates (NOT screen coordinates)
                //Vector3 point2D;
                //Vector3 cubeDim = GetVisualizer()->GetCubeDim();
                //point2D.x = (modelPt.x + cubeSize.x/2.0f)/GetVoxelX();
                //point2D.y = (modelPt.y + cubeSize.y/2.0f)/GetVoxelY();
                //don't care about Z

                numberOfTargets++;

                contourVector.push_back(Vector3(modelPt.x, modelPt.y, modelPt.z));

            }
            if (contourVector.size() > 0)
            {
                if (startSlice == -1)
                {
                    startSlice = i+1;
                }
            
         
                surfaceVector.push_back(contourVector);

            }
            else
            {
                if (startSlice != -1 && endSlice == -1)
                {
                    endSlice = i+1;
                    break;//done
                }

            }


        }
        
    }

    //now we want to write the file
    std::string temp;
    fstream tFile = fstream(filePath.c_str(), ios::out | ios::binary);
	char outstring[200];

	sprintf(outstring,"______________________________________________________________________\n\n");
	tFile.write(outstring, strlen(outstring));

	sprintf(outstring,"                              IPSA                                    \n");
	tFile.write(outstring, strlen(outstring));

	sprintf(outstring,"______________________________________________________________________\n\n");
	tFile.write(outstring, strlen(outstring));

	sprintf(outstring,"TARGET\n");
	tFile.write(outstring, strlen(outstring));

	//write number of targets
	sprintf(outstring,     "%d\n", numberOfTargets);
	tFile.write(outstring, strlen(outstring));


	//wride data
	long currentSlice = startSlice;
    for (int i=0; i<surfaceVector.size(); i++)//for each contour
    {
        Vector3Vec contourVec = surfaceVector.at(i);
    
        for (int j=0; j<contourVec.size(); j++)//for each point in the contour
        {
            Vector3 point = contourVec.at(j);
			sprintf(outstring, "%f %f %f\n", point.x, point.y, point.z);
			tFile.write(outstring, strlen(outstring));
        }

        currentSlice++;
    }
 
    // clean up vtk 
    points->Delete();
    polys->Delete();
    surface->Delete();
    plane->Delete();
    cutter->Delete();
    stripper->Delete();

    return 0;
}

void RRI_SlicerInterface::GenerateSurfaceMarchingCubes(vtkImageData* Volume, SurfaceObject* so)
{
    vtkMarchingCubes* marchingCubes = vtkMarchingCubes::New();
    vtkWindowedSincPolyDataFilter* smoother = vtkWindowedSincPolyDataFilter::New();

    marchingCubes->SetInput(Volume);
    marchingCubes->ComputeNormalsOff();
    marchingCubes->ComputeGradientsOff();
    marchingCubes->SetValue(0, 0.5);
    marchingCubes->Update();

    
    smoother->SetInput(marchingCubes->GetOutput());
    smoother->SetNumberOfIterations(15);//15
    smoother->BoundarySmoothingOn();
    smoother->FeatureEdgeSmoothingOn();
    smoother->SetFeatureAngle(120.0);
    smoother->SetPassBand(0.001);
    smoother->NonManifoldSmoothingOn();
    smoother->NormalizeCoordinatesOn();
    smoother->Update();

#define DECIMATE_SURFACE
#ifdef DECIMATE_SURFACE
    //now, decimate triangles
    vtkDecimatePro* decimate = vtkDecimatePro::New();
    decimate->SetInput(smoother->GetOutput());
    decimate->SetTargetReduction(0.10);//10 percent reduction
    decimate->Update();
#endif
    
    
    vtkPolyData* polyData = smoother->GetOutput();

//#define TESTING
#ifdef TESTING
    //export surface in.vtk format
    vtkSTLWriter* stlWriter = vtkSTLWriter::New();
    stlWriter->SetInput(polyData);
    stlWriter->SetFileName("c:/Scans/test.stl");
    stlWriter->SetFileTypeToASCII();
    stlWriter->Write();
    stlWriter->Update();
    stlWriter->Delete();
#endif
#ifndef TESTING
    HelperConvertPolyDataToTriangles(polyData, &(so->triangles), true);
#endif
    marchingCubes->Delete();
    smoother->Delete();

#ifdef DECIMATE_SURFACE
    decimate->Delete();
#endif

}


void RRI_SlicerInterface::GenerateSurfaceMarchingCubes(vtkImageData* Volume, SurfaceObject* so, vtkPolyData* polyData)
{
    vtkMarchingCubes* marchingCubes = vtkMarchingCubes::New();
    vtkWindowedSincPolyDataFilter* smoother = vtkWindowedSincPolyDataFilter::New();

    marchingCubes->SetInput(Volume);
    marchingCubes->ComputeNormalsOff();
    marchingCubes->ComputeGradientsOff();
    marchingCubes->SetValue(0, 0.5);
    marchingCubes->Update();

    
    smoother->SetInput(marchingCubes->GetOutput());
    smoother->SetNumberOfIterations(15);//15
    smoother->BoundarySmoothingOn();
    smoother->FeatureEdgeSmoothingOn();
    smoother->SetFeatureAngle(120.0);
    smoother->SetPassBand(0.001);
    smoother->NonManifoldSmoothingOn();
    smoother->NormalizeCoordinatesOn();
    smoother->Update();

#define DECIMATE_SURFACE
#ifdef DECIMATE_SURFACE
    //now, decimate triangles
    vtkDecimatePro* decimate = vtkDecimatePro::New();
    decimate->SetInput(smoother->GetOutput());
    decimate->SetTargetReduction(0.10);//10 percent reduction
    decimate->Update();
#endif
    
    
    vtkPolyData* smoothPolyData = smoother->GetOutput();

//#define TESTING
#ifdef TESTING
    //export surface in.vtk format
    vtkSTLWriter* stlWriter = vtkSTLWriter::New();
    stlWriter->SetInput(polyData);
    stlWriter->SetFileName("c:/Scans/test.stl");
    stlWriter->SetFileTypeToASCII();
    stlWriter->Write();
    stlWriter->Update();
    stlWriter->Delete();
#endif
#ifndef TESTING
    HelperConvertPolyDataToTriangles(smoothPolyData, &(so->triangles), true);
    polyData->DeepCopy(smoothPolyData);
#endif
    marchingCubes->Delete();
    smoother->Delete();

#ifdef DECIMATE_SURFACE
    decimate->Delete();
#endif

}


void RRI_SlicerInterface::ShowAllSurfaces()
{
	
	for (int i=0; i<m_surfaceList.size(); i++)
	{
		SurfaceObject* so = &(m_surfaceList.at(i));
		so->visible = true;
	}

}

void RRI_SlicerInterface::HideAllSurfaces()
{
	for (int i=0; i<m_surfaceList.size(); i++)
	{
		SurfaceObject* so = &(m_surfaceList.at(i));
		so->visible = false;
	}

	ClearTargets();//also clears target surfaces

}

void RRI_SlicerInterface::ShiftSurfacesEnable(std::string surfaceContains, bool set)//
{
	m_isShiftingSurfaces = set;
	m_surfaceContains = surfaceContains; 
}


void RRI_SlicerInterface::RotateSurfacesEnable(long mode, std::string surfaceContains, bool set, Vector3 center)
{
	m_rotatingSurfacesMode = mode; 
	m_surfaceContains = surfaceContains; 
	m_isRotatingSurfaces = set;
	m_centerOfRotation = center;
    if (-1 != surfaceContains.find("Planned"))//Planned needles need to be rotated differently than the other surfaces
    {
        m_transformingNeedles = true;
    }
    else
    {
        m_transformingNeedles = false;
    }
}

void RRI_SlicerInterface::RotateSurfaces(long mode, float rotation)
{
//mode 1: rotation about x axis
	//mode 2: rotation about y axis
	//mode 3: rotation about z axis

	vtkTransform* transform = vtkTransform::New();
	transform->PostMultiply();
	
	vtkMatrix4x4* matrix = vtkMatrix4x4::New();

	
	if (mode == 1)
	{
		transform->RotateX(-rotation);
	}
	else
	if (mode == 2)
	{
		transform->RotateY(rotation);
	}
	else
	if (mode == 3)
	{
		transform->RotateZ(rotation);
	}

	transform->GetMatrix(matrix);

	//update manual registration transformation matrix
	if (m_isRegMatrixSet)
	{
		vtkTransform* transform = vtkTransform::New();
		transform->PostMultiply();
		transform->SetMatrix(m_regMatrix);//new rotation
		transform->Concatenate(matrix);
		//transform->SetMatrix(matrix);//new rotation
		//transform->Concatenate(m_regMatrix);
		transform->GetMatrix(m_regMatrix);//update registration matrix
		transform->Delete();
	}


	//calculate centroid------------------------------------------------
	TriangleVec tempTriangles;
	for (int i=0; i<m_surfaceList.size(); i++)
	{
		SurfaceObject* so = &(m_surfaceList.at(i));

		if (so->visible)
		{

			if (m_surfaceContains == "" || -1 != so->name.find(m_surfaceContains))
			{
				TriangleVec* tv = &(so->triangles);
				for (int j=0; j<tv->size(); j++)
				{
					Triangle t = tv->at(j);
					tempTriangles.push_back(t);
				}
			}
			
		}
	}

	Vector3 centroid = helperFindCentroid(&tempTriangles);
	//-------------------------------------------------------------------

	for (int i=0; i<m_surfaceList.size(); i++)
	{
		SurfaceObject* so = &(m_surfaceList.at(i));
		if (so->visible)
		{
			if (m_surfaceContains == "" || -1 != so->name.find(m_surfaceContains))
			{

				//transform user triangles
				TriangleVec* tv = &(so->triangles);

				TriangleVec newVec;

			
				
				for (int j=0; j<tv->size(); j++)
				{
					Triangle t = tv->at(j);
					Triangle newT;
			
					//use centroid
					if (m_centerOfRotation.x == 0 && m_centerOfRotation.y == 0 && m_centerOfRotation.z == 0)
					{
						newT.V1 = helperTransformPointLinear(matrix, t.V1, centroid);
						newT.V2 = helperTransformPointLinear(matrix, t.V2, centroid);
						newT.V3 = helperTransformPointLinear(matrix, t.V3, centroid);
					}
					else
					{
						newT.V1 = helperTransformPointLinear(matrix, t.V1, m_centerOfRotation);
						newT.V2 = helperTransformPointLinear(matrix, t.V2, m_centerOfRotation);
						newT.V3 = helperTransformPointLinear(matrix, t.V3, m_centerOfRotation);

					}
					

					newVec.push_back(newT);
				}


				tv->clear();

		
				for (int j=0; j<newVec.size(); j++)
				{
					Triangle t = newVec.at(j);
					tv->push_back(t);
				}
		
				//-----------------------------------------------------------------

				//transform user points
				Vector3Vec* userPoints = &(so->userPoints);
				Vector3Vec transformedPoints;
				for (int j=0; j<userPoints->size(); j++)
				{
                    //use centroid
					if (m_centerOfRotation.x == 0 && m_centerOfRotation.y == 0 && m_centerOfRotation.z == 0)
					{
						Vector3 point = userPoints->at(j);
					    Vector3 transformedPoint = helperTransformPointLinear(matrix, point, centroid);
                        userPoints->at(j) = transformedPoint;
					    //transformedPoints.push_back(transformedPoint);
					}
					else
					{
						Vector3 point = userPoints->at(j);
					    Vector3 transformedPoint = helperTransformPointLinear(matrix, point, m_centerOfRotation);
                        userPoints->at(j) = transformedPoint;
					    //transformedPoints.push_back(transformedPoint);

					}

				}
			}
			//-------------------------------------------------------------------------------

		}
	
	}

	transform->Delete();
	matrix->Delete();
	//***UpdateDisplay();


}

//this function assumes that the shift is in screen coordinates
void RRI_SlicerInterface::ShiftSurfaces(float xshift, float yshift, float zshift)
{
	//update manual registration transformation matrix
	if (m_isRegMatrixSet)
	{
		vtkTransform* transform = vtkTransform::New();
		transform->SetMatrix(m_regMatrix);
		transform->Translate(-xshift, -yshift, -zshift);
		transform->GetMatrix(m_regMatrix);//update registration matrix
		transform->Delete();
	}

	for (int i=0; i<m_surfaceList.size(); i++)
	{
		SurfaceObject* so = &(m_surfaceList.at(i));
		if (so->visible)
		{
			if (m_surfaceContains == "" || -1 != so->name.find(m_surfaceContains))
			{
				TriangleVec* tv = &(so->triangles);

				TriangleVec newVec;
				for (int j=0; j<tv->size(); j++)
				{
					Triangle t = tv->at(j);
					t.V1.x -= xshift;
					t.V2.x -= xshift;
					t.V3.x -= xshift;

					t.V1.y -= yshift;
					t.V2.y -= yshift;
					t.V3.y -= yshift;

					t.V1.z -= zshift;
					t.V2.z -= zshift;
					t.V3.z -= zshift;

					newVec.push_back(t);
				}

				tv->clear();

				{
					for (int j=0; j<newVec.size(); j++)
					{
						Triangle t = newVec.at(j);
						tv->push_back(t);
					}
				}

				//transform user points----------------------------------------------------------------------
				Vector3Vec* userPoints = &(so->userPoints);
				Vector3Vec transformedPoints;
				for (int j=0; j<userPoints->size(); j++)
				{
					Vector3 point = userPoints->at(j);
					//Vector3 transformedPoint = helperTransformPointLinear(matrix, point, centroid);
					point.x -= xshift;
					point.y -= yshift;
					point.z -= zshift;

					transformedPoints.push_back(point);

				}

				userPoints->clear();//replace with transformed points
				{
					for (int j=0; j<transformedPoints.size(); j++)
					{
						Vector3 point = transformedPoints.at(j);
						userPoints->push_back(point);
					}
				}
			}
			//------------------------------------------------------------------------------------------


		}

	}


	//***UpdateDisplay();
}


/*
	SetAlphaBlend(100);
	//center of brush
	long centerX = x;
	long centerY = y;

	Vector3 cubeDim = GetVisualizer()->GetCubeDim();
	Vector3 cubeSize = GetVisualizer()->GetCubeSize();

	long width = GetWidth();
	long height = GetHeight();

	float voxelX = cubeSize.x / (float)cubeDim.x;
	float voxelY = cubeSize.y / (float)cubeDim.y;
	float voxelZ = cubeSize.z / (float)cubeDim.z;

	long brushRadius = (long)(m_paintBrushSize / voxelX / 2.0);//calculate brush radius in pixels
	m_paintBrushRadiusPixels = (long)(m_paintBrushSize / voxelX / 2.0) * GetVisualizer()->GetZoomFactor() / 5.0;

	for (long i = -brushRadius; i <= brushRadius; i++)
	{
		for (long j = -brushRadius; j <= brushRadius; j++)
		{
			double radius = sqrt((double)i*(double)i + (double)j*(double)j);

			//paint into overlay buffer.
			if (radius < (double)brushRadius)
			{
				long pointValueX = centerX + i;
				long pointValueY = centerY + j;
				unsigned char* alphaBuffer = m_alphaGraphics->GetBitmapBits();
				long index = pointValueY*width * 3 + pointValueX * 3;//24 bit
				if (paint)
				{
					alphaBuffer[index + 0] = 255;
					alphaBuffer[index + 1] = 0;
					alphaBuffer[index + 2] = 0;
					
				}
				else
				{
					alphaBuffer[index + 0] = 0;
					alphaBuffer[index + 1] = 0;
					alphaBuffer[index + 2] = 0;


				}
				
			}

		}

	}
*/


void RRI_SlicerInterface::PaintIntoBinaryCube(long x, long y, bool paint)
{
	long width = GetWidth();
	long height = GetHeight();
	long size = width * height;

	unsigned char* overlayBuffer = GetOverlayBits();

	//center of brush
	long centerX = x;
	long centerY = y;

	long brushRadius = m_paintBrushSize;

	Vector3 modelPoint = ScreenToModel(centerX, centerY);
	
	Vector3 cubeDim = GetVisualizer()->GetCubeDim();
	Vector3 cubeSize = GetVisualizer()->GetCubeSize();
	long cubeDimSize = cubeDim.x * cubeDim.y * cubeDim.z;
	float voxelX = cubeSize.x / (float)cubeDim.x;
	float voxelY = cubeSize.y / (float)cubeDim.y;
	float voxelZ = cubeSize.z / (float)cubeDim.z;

	

	//calculate the voxel difference between two pixels on the screen
	Vector3 p1 = ScreenToModel(centerX, centerY);
	Vector3 p2 = ScreenToModel(centerX + brushRadius, centerY);
	float distance = helperDistanceBetweenTwoPoints(p1, p2);

	float floatRadius = distance/voxelX;// brushRadius * distance;//get the voxel size of the radius
	

	long volSize = cubeDim.x*cubeDim.y*cubeDim.z;
	long imageSize = cubeDim.y*cubeDim.z;

	long centerPointX = (long)(((modelPoint.x + (cubeSize.x / 2.0)) / voxelX) + 0.5);
	long centerPointY = (long)(((modelPoint.y + (cubeSize.y / 2.0)) / voxelY) + 0.5);
	long centerPointZ = (long)(((modelPoint.z + (cubeSize.z / 2.0)) / voxelZ) + 0.5);

	if (m_use3DBrush)
	{
		for (int z = centerPointZ - brushRadius; z < centerPointZ + brushRadius; z++)
		{
			for (int y = centerPointY - brushRadius; y < centerPointY + brushRadius; y++)
			{
				for (int x = centerPointX - brushRadius; x < centerPointX + brushRadius; x++)
				{

					long diffX = abs(x - centerPointX);
					long diffY = abs(y - centerPointY);
					long diffZ = abs(z - centerPointZ);
					double radius = sqrt((double)diffX*(double)diffX + (double)diffY*(double)diffY + (double)diffZ*(double)diffZ);
					long volIndex = z * cubeDim.x*cubeDim.y + y * cubeDim.x + x;
					//paint into binary cube
					if (paint)
					{
						if (radius < floatRadius)
						{
                            if (m_isEditing)
                            {
                                if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
                                    m_binaryBuffer[volIndex] = (unsigned char)255;//255
                            }
                            else
							if (m_paintingInside)
							{
								if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
									m_binaryBuffer[volIndex] = (unsigned char)200;
							}
							else
							{
								if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
									m_binaryBuffer[volIndex] = (unsigned char)100;
							}
						}

					}
					else//erasing
					{
						if (radius < floatRadius)
						{
							if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
								m_binaryBuffer[volIndex] = (unsigned char)0;

							
						}

					}
				}
			}
		}
	}//if (m_use3DBrush)
	else
	{


		for (long i = -brushRadius; i <= brushRadius; i++)
		{
			for (long j = -brushRadius; j <= brushRadius; j++)
			{
				double radius = sqrt((double)i*(double)i + (double)j*(double)j);

				//paint into overlay buffer. On mouse up, copy into binary cube
				if (radius < (double)brushRadius)
				{

					long pointValueX, pointValueY;
					//convert from model to voxel coordinates
					pointValueX = centerX + i;//(long)(((pointVoxel.x + cubeSize.x / 2.0) / voxelX) + 0.5);
					pointValueY = centerY + j;//(long)(((pointVoxel.y + cubeSize.y / 2.0) / voxelY) + 0.5);

					Vector3 modelPoint = ScreenToModel(pointValueX, pointValueY);
					//convert from model to voxel coordinates
					long modelX = (long)(((modelPoint.x + (cubeSize.x / 2.0)) / voxelX) + 0.5);
					long modelY = (long)(((modelPoint.y + (cubeSize.y / 2.0)) / voxelY) + 0.5);
					long modelZ = (long)(((modelPoint.z + (cubeSize.z / 2.0)) / voxelZ) + 0.5);

					long volIndex = modelZ * cubeDim.x*cubeDim.y + modelY * cubeDim.x + modelX;

					long overlayIndex = (pointValueY*(long)width) * 3 + (pointValueX + 0) * 3;


					//paint into binary cube
					if (paint)
					{
						if (overlayIndex >= 0 && overlayIndex < size * 3)
						{
                            if (m_isEditing)
                            {
                                if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
                                    m_binaryBuffer[volIndex] = (unsigned char)255;
                            }
                            else
							if (m_paintingInside)
							{
								if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
									m_binaryBuffer[volIndex] = (unsigned char)200;
							}
							else
							{
								if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
									m_binaryBuffer[volIndex] = (unsigned char)100;	
							}
						}


					}
					//erace binary cube
					else
					{
						if (overlayIndex >= 0 && overlayIndex < size * 3)
						{
							if (m_paintingInside)
							{
								if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
									m_binaryBuffer[volIndex] = (unsigned char)0;
							}
							else
							{
								if (volIndex >= 0 && volIndex < cubeDim.x*cubeDim.y*cubeDim.z)
									m_binaryBuffer[volIndex] = (unsigned char)0;

							}
						}

					}

				}
			}
		}
	}//use 2D brush
}





#endif