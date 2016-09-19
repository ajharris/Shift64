#pragma once


#include "Voxel.h"



class VoxelAXIAL : public Voxel  
{

public:

	VoxelAXIAL(TvoxelInit voxelInit);
	virtual ~VoxelAXIAL();

	virtual tVoxel GetVoxValue(long x, long y, long z);
	virtual tVoxel GetVoxValue(Vector3F vF);


    virtual tVoxel	GetVoxRvalue(long x, long y, long z);
	virtual tVoxel	GetVoxGvalue(long x, long y, long z);
	virtual tVoxel	GetVoxBvalue(long x, long y, long z);

protected:

	eProbeOrientation m_probeOrient;

	unsigned long *m_pReverseMap;
	
	long m_extraFrames; 
	long m_size180Sweep; 
	float *m_pWeightTable;


	void buildReverseMap(CalParam cal, AcqParam acq);
};