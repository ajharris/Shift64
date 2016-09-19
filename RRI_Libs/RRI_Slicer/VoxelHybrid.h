#pragma once


#include "voxel.h"



class VoxelHybrid : public Voxel
{
public:
	VoxelHybrid(TvoxelInit voxelInit);
	virtual ~VoxelHybrid(void);

	virtual tVoxel GetVoxValue(long x, long y, long z);
	virtual tVoxel GetVoxValue(Vector3F vF);
	virtual long GetVoxIndex(Vector3 point);

    virtual tVoxel	GetVoxRvalue(long x, long y, long z);
	virtual tVoxel	GetVoxGvalue(long x, long y, long z);
	virtual tVoxel	GetVoxBvalue(long x, long y, long z);

protected:

	unsigned long *m_pReverseMap;

	void buildReverseMap(CalParam cal, AcqParam acq);
};
