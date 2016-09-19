#pragma once


#include "Voxel.h"


class Voxel24 : public Voxel  
{

public:
	Voxel24(TvoxelInit voxelInit);
	virtual ~Voxel24();

	virtual tVoxel	GetVoxRvalue(Vector3F vF);
	virtual tVoxel	GetVoxGvalue(Vector3F vF);
	virtual tVoxel	GetVoxBvalue(Vector3F vF);

	virtual tVoxel	GetVoxRvalue(long x, long y, long z);
	virtual tVoxel	GetVoxGvalue(long x, long y, long z);
	virtual tVoxel	GetVoxBvalue(long x, long y, long z);

};