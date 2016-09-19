#pragma once


#include "VoxelAXIAL.h"


class VoxelAXIAL24 : public VoxelAXIAL  
{

public:

	VoxelAXIAL24(TvoxelInit voxelInit);
	virtual ~VoxelAXIAL24();

	virtual tVoxel	GetVoxRvalue(Vector3F vF);
	virtual tVoxel	GetVoxGvalue(Vector3F vF);
	virtual tVoxel	GetVoxBvalue(Vector3F vF);

    virtual tVoxel	GetVoxRvalue(long x, long y, long z);
	virtual tVoxel	GetVoxGvalue(long x, long y, long z);
	virtual tVoxel	GetVoxBvalue(long x, long y, long z);
};