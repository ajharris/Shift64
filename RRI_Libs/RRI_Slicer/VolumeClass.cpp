//---------------------------------------------------------------------
// VolumeClass.cpp
//
//---------------------------------------------------------------------
//#include "stdafx.h"
#include "VolumeClass.h"

#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
	
#include <stdio.h>
#include <io.h>

//---------------------------------------------------------------------
// VolumeClass constructor
//
//---------------------------------------------------------------------
VolumeClass::VolumeClass()
{
	initParam();//initialize default volume parameters
}




//---------------------------------------------------------------------
// VolumeClass destructor
//
//---------------------------------------------------------------------
VolumeClass::~VolumeClass()
{

	if (m_imageData)
	{
		delete [] m_imageData;
		m_imageData = 0;
	}	
}


//----------------------------------------------------------------------
// InitVol
//
// Initialize a volume 
//
// Input:
// eScanGeometry enum to describe acquisition type
// width of the frame
// height of the frame
// number of frames
// sweep angle of possible fan or axial acquisition
//
// Output:
// none
//----------------------------------------------------------------------
void VolumeClass::InitVol(eScanGeometry geometry, long width, long height, long numFrames, long channels, float xvox, float yvox, float zvox, float sweepAngle, float sweepDistance)
{
	initParam();//initialize default parameters
	
	//update acq and cal parameters with parameter passed by client
	m_Acq.width = width;
	m_Acq.height = height;
	m_Acq.numFrames = numFrames;
	m_Acq.scanGeometry = geometry;
	m_Acq.sweepAngle = sweepAngle;
	m_Cal.probeDistance = sweepDistance;
	m_Cal.xVoxelSize = xvox;
	m_Cal.yVoxelSize = yvox;
	m_Cal.zVoxelSize = zvox;

	//delete previous image data
	if (m_imageData)
	{
		delete[] m_imageData;
		m_imageData = 0;
	}
	//create new image data using new parameters
	m_imageData = new unsigned char[width * height * numFrames * channels];
	memset(m_imageData, 100, width * height * numFrames * channels);//clear buffer

}

void VolumeClass::InitVol(AcqParam acq, CalParam cal)
{
	//initialize local parameters with parameters supplied by client
	m_Acq.width					= acq.width;
	m_Acq.height				= acq.height;
	m_Acq.numFrames				= acq.numFrames;
	m_Acq.horizontalReflection	= acq.horizontalReflection;
	m_Acq.probeOrientation		= acq.probeOrientation;
	m_Acq.scanDirection			= acq.scanDirection;
	m_Acq.trackingMode			= acq.trackingMode;	
	m_Acq.sweepAngle			= acq.sweepAngle;
	m_Acq.scanGeometry			= acq.scanGeometry;
	

	m_Cal.xVoxelSize = cal.xVoxelSize;
	m_Cal.yVoxelSize = cal.yVoxelSize;
	m_Cal.zVoxelSize = cal.zVoxelSize;
	m_Cal.axisOfRotation = cal.axisOfRotation;
	m_Cal.inPlaneTilt = cal.inPlaneTilt;
	m_Cal.outPlaneDisplacement = cal.outPlaneDisplacement;
	m_Cal.outPlaneTilt = cal.outPlaneTilt;
	m_Cal.probeDistance = cal.probeDistance; 

	

	//delete previous image data
	if (m_imageData)
	{
		delete[] m_imageData;
		m_imageData = 0;
	}

	long channels = 1;
	if (acq.scanGeometry == SG_Linear16)
		channels = 2;
	if (acq.scanGeometry == SG_Linear24)
		channels = 3;

	//create new image data using new parameters
	m_imageData = new unsigned char[acq.width * acq.height * acq.numFrames * channels];
	memset(m_imageData, 100, acq.width * acq.height * acq.numFrames * channels);//clear buffer

	
	
}

//this just reads the data into the buffer. InitVol must be called first in order for the volume parameters to be setup correctly
long VolumeClass::ImportRaw(string filepath)
{

	ifstream file(filepath.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		ifstream::pos_type size = file.tellg();

		if (m_imageData) delete m_imageData;
		m_imageData = new unsigned char[size];
		file.seekg (0, ios::beg);
		file.read ((char*)m_imageData, size);
		file.close();
	}
	else
	{
		return -1;
	}
	
	return 0;
}

bool VolumeClass::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

//----------------------------------------------------------------------
// initParam
//
// Initialize volume parameters with default values
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void VolumeClass::initParam()
{
	m_Cal.xVoxelSize = 1.0f;
	m_Cal.yVoxelSize = 1.0f;
	m_Cal.zVoxelSize = 1.0f;
	m_Cal.axisOfRotation = 12.0f;
	m_Cal.inPlaneTilt = 0.0f;
	m_Cal.outPlaneDisplacement = 0.0f;
	m_Cal.outPlaneTilt = 0.0f;
	m_Cal.probeDistance = 0.0; 

	m_Acq.width					= 100;
	m_Acq.height				= 100;
	m_Acq.numFrames				= 100;
	m_Acq.horizontalReflection	= HIR_PowerupDefault;
	m_Acq.probeOrientation		= PO_Top;
	m_Acq.scanDirection			= SD_Clockwise;
	m_Acq.trackingMode			= TM_MCM;	
	m_Acq.sweepAngle			= 0.0f;
	m_Acq.scanGeometry			= SG_Hybrid;

	m_imageData = 0;

}
