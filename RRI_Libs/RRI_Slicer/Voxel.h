#pragma once

#include "VisLib.h"
#include "geometry.h"
#include "Voxel.h"
#include "fixed.h"

//typedef unsigned char tVoxel;
//typedef long Fixed;

//struct TvoxelInit;
//struct Matrix4x4;
//struct Vector3F;



class Voxel
{

public:
	Voxel(TvoxelInit voxelInit);
	Voxel();
	virtual ~Voxel();
	
	virtual tVoxel GetVoxValue(long x, long y, long z);
	virtual	tVoxel GetVoxValue(Vector3F vF);
	virtual tVoxel GetVoxValue(long x, long y, long z, long n){return 0;};
	virtual void GetVoxValue(long x, long y, long z, tVoxel& lower, tVoxel& upper){};
	virtual void SetVoxValue(long x, long y, long z, long n, unsigned char value){};
    virtual void SetVoxValue(long x, long y, long z, unsigned char value);

	virtual tVoxel GetVoxRvalue(Vector3F vF);
	virtual tVoxel GetVoxGvalue(Vector3F vF);
	virtual tVoxel GetVoxBvalue(Vector3F vF);

    virtual tVoxel GetVoxRvalue(long x, long y, long z);
	virtual tVoxel GetVoxGvalue(long x, long y, long z);
	virtual tVoxel GetVoxBvalue(long x, long y, long z);

	virtual long GetVoxIndex(Vector3 point);

	void SetLeftRight(long left, long right);

	bool CheckOutOfBounds(long x, long y, long z);
	Matrix4x4 GetModel2Array();

	void setLeftRight(long left, long right);

	float GetSizeXmm();
	float GetSizeYmm();
	float GetSizeZmm();
	
	long GetDimX();
	long GetDimY();
	long GetDimZ();	



protected:

	tVoxel *m_pPixelData;
	
	Matrix4x4 m_model2Array;

	long m_cubeX, m_cubeY, m_cubeZ;	
	float m_voxX, m_voxY, m_voxZ;

	long m_xySize;
	long m_xyzSize;
	eScanDirection m_scanDirect;			
	eHorizontalImageReflection m_horReflect;	
	eProbeOrientation m_probeOrient;


protected:

	long m_left, m_right, m_max;
	float m_slope;

	void setModelToArray();
};
