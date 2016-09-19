#pragma once



#include "Voxel.h"



class VoxelFAN : public Voxel  
{

public:

	VoxelFAN(TvoxelInit voxelInit);
	virtual ~VoxelFAN();

	virtual tVoxel GetVoxValue(long x, long y, long z);
	virtual tVoxel GetVoxValue(Vector3F vF);


    virtual tVoxel	GetVoxRvalue(long x, long y, long z);
	virtual tVoxel	GetVoxGvalue(long x, long y, long z);
	virtual tVoxel	GetVoxBvalue(long x, long y, long z);
	
protected:

	long m_widthM;

	eProbeOrientation m_probeOrient;
	//bool m_useWcomplement;

	unsigned long *m_pReverseMap;

	void buildReverseMap(CalParam cal, AcqParam acq);

};