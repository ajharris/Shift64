#pragma once


#include "VoxelHybrid.h"


class VoxelHybrid24 : public VoxelHybrid
{

public:

	VoxelHybrid24(TvoxelInit voxelInit);
	virtual ~VoxelHybrid24();

	virtual tVoxel	GetVoxRvalue(Vector3F vF);
	virtual tVoxel	GetVoxGvalue(Vector3F vF);
	virtual tVoxel	GetVoxBvalue(Vector3F vF);

    virtual tVoxel	GetVoxRvalue(long x, long y, long z);
	virtual tVoxel	GetVoxGvalue(long x, long y, long z);
	virtual tVoxel	GetVoxBvalue(long x, long y, long z);
};
