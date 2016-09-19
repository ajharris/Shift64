//---------------------------------------------------------------------
// VolumeClass.h
//---------------------------------------------------------------------

#if !defined(VOLUMECLASS_H_INCLUDED)
#define VOLUMECLASS_H_INCLUDED

#include "VisLib.h"
using namespace std;
class SLICER_EXPORT VolumeClass
{

public:
	//constructors
	VolumeClass();
	//destructor
	virtual ~VolumeClass();
	//initialize volume
	void InitVol(AcqParam acq, CalParam cal);//here, we assume the parameters for initializing the volume have already been set.
    void InitVol(eScanGeometry geometry, long width, long height, long numFrames, long channels, float xvox, float yvox, float zvox, float sweepAngle = 0.0, float sweepDistance = 0.0);
	//set acquisition and calibration parameters
	//void SetAcq(AcqParam acq){m_Acq = acq;}
	//void SetCal(CalParam cal){m_Cal = cal;}
	//AcqParam GetAcq(){return m_Acq;}
	//CalParam GetCal(){return m_Cal;}
	AcqParam m_Acq;	
	CalParam m_Cal;
	//import raw data image
	long ImportRaw(string filepath);
    //get pointer to raw data
	unsigned char* GetData(){return m_imageData;} 
	long GetVolumeWidth(){return m_Acq.width;}
	long GetVolumeHeight(){return m_Acq.height;}
	long GetVolumeDepth(){return m_Acq.numFrames;}
	double GetVoxelX(){return (double)m_Cal.xVoxelSize;}
	double GetVoxelY(){return (double)m_Cal.yVoxelSize;}
	double GetVoxelZ(){return (double)m_Cal.zVoxelSize;}
	eScanGeometry GetScanGeometry(){return m_Acq.scanGeometry;}
	eHorizontalImageReflection GetReflection(){return m_Acq.horizontalReflection;}
	eScanDirection GetScanDirection(){return m_Acq.scanDirection;}
	eProbeOrientation GetProbeOrientation(){return m_Acq.probeOrientation;}
	
protected:
	unsigned char*	m_imageData; 

	void initParam();//initialize default AcqParam and CalParam which are used to define the volume
	bool replace(std::string& str, const std::string& from, const std::string& to);
};

#endif // !defined(VOLUMECLASS_H_INCLUDED)
