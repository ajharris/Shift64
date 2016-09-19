#pragma once


#include "Voxel.h"


class Voxel16 : public Voxel 
{

public:
	Voxel16(TvoxelInit voxelInit);
	virtual ~Voxel16();
	virtual tVoxel GetVoxValue(Vector3F vF);
	virtual	tVoxel GetVoxValue(long x, long y, long z, long n);
	void SetVoxValue(long x, long y, long z, long n, unsigned char value);
	virtual void GetVoxValue(long x, long y, long z, tVoxel& lower, tVoxel& upper);
	virtual long GetVoxIndex(Vector3 point);


    virtual tVoxel	GetVoxRvalue(long x, long y, long z);
	virtual tVoxel	GetVoxGvalue(long x, long y, long z);
	virtual tVoxel	GetVoxBvalue(long x, long y, long z);
};