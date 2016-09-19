//#include "StdAfx.h"
#include "VoxelFAN24.h"



//----------------------------------------------------------------------
// VoxelFAN24 constructor
//
// 
//----------------------------------------------------------------------
VoxelFAN24::VoxelFAN24(TvoxelInit voxelInit) :

					VoxelFAN(voxelInit)
{
}


//----------------------------------------------------------------------
// VoxelFAN24 destrucror
//
// 
//----------------------------------------------------------------------
VoxelFAN24::~VoxelFAN24()
{
}



//----------------------------------------------------------------------
// GetVoxRvalue
//
// Returns a volume voxel R value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//-----------------------------------------------------------------------
tVoxel VoxelFAN24::GetVoxBvalue(Vector3F vF) 
{
	Vector3F vI = fxd::FPint(vF);

	long x = vI.x;
	long y = vI.y;
	long z = vI.z;

	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(z < 0) z = 0;

	if(x > (m_cubeX - 1)) x = m_cubeX - 1;
	if(y > (m_cubeY - 1)) y = m_cubeY - 1;
	if(z > (m_cubeZ - 1)) z = m_cubeZ - 1;


	if(m_probeOrient == PO_Bottom)
	{
		y = m_cubeY - 1 - y;
	}

	unsigned long rmi = m_pReverseMap[y + z * m_cubeY];

	if (rmi <= 0)
	{
		return 0;
	}
	else 
	{
		long index = rmi + x;;
	
		if (index >= 0 && index < m_cubeX*m_cubeY*m_cubeZ)
        {

            return m_pPixelData[index];
        }
        else
        {
            return 0;
        }
	}
}


//----------------------------------------------------------------------
// GetVoxGvalue
//
// Returns a volume voxel G value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//-----------------------------------------------------------------------
tVoxel VoxelFAN24::GetVoxGvalue(Vector3F vF) 
{
	Vector3F vI = fxd::FPint(vF);

	long x = vI.x;
	long y = vI.y;
	long z = vI.z;

	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(z < 0) z = 0;

	if(x > (m_cubeX - 1)) x = m_cubeX - 1;
	if(y > (m_cubeY - 1)) y = m_cubeY - 1;
	if(z > (m_cubeZ - 1)) z = m_cubeZ - 1;


	if (m_scanDirect == SD_AntiClockwise)
	{
		z = m_cubeZ - 1 - z;
	}
	if (m_probeOrient == PO_Bottom)
	{
		y = m_cubeY - 1 - y;
	}
	if (m_horReflect == HIR_Reflected)
	{
		x = m_cubeX - 1 - x;
	}

	unsigned long rmi = m_pReverseMap[y + z * m_cubeY];

	if (rmi <= 0)
	{
		return 0;
	}
	else 
	{
		long index = rmi + x;;
	
		if (index >= 0 && index < m_cubeX*m_cubeY*m_cubeZ)
        {

            return m_pPixelData[index];
        }
        else
        {
            return 0;
        }
	}
}


//----------------------------------------------------------------------
// GetVoxBvalue
//
// Returns a volume voxel B value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//-----------------------------------------------------------------------
tVoxel VoxelFAN24::GetVoxRvalue(Vector3F vF)
{
	Vector3F vI = fxd::FPint(vF);

	long x = vI.x;
	long y = vI.y;
	long z = vI.z;

	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(z < 0) z = 0;

	if(x > (m_cubeX - 1)) x = m_cubeX - 1;
	if(y > (m_cubeY - 1)) y = m_cubeY - 1;
	if(z > (m_cubeZ - 1)) z = m_cubeZ - 1;


	if (m_scanDirect == SD_AntiClockwise)
	{
		z = m_cubeZ - 1 - z;
	}
	if (m_probeOrient == PO_Bottom)
	{
		y = m_cubeY - 1 - y;
	}
	if (m_horReflect == HIR_Reflected)
	{
		x = m_cubeX - 1 - x;
	}

	unsigned long rmi = m_pReverseMap[y + z * m_cubeY];

	if (rmi <= 0)
	{
		return 0;
	}
	else 
	{
		long index = rmi + x;;
	
		if (index >= 0 && index < m_cubeX*m_cubeY*m_cubeZ)
        {

            return m_pPixelData[index];
        }
        else
        {
            return 0;
        }
	}
}
//----------------------------------------------------------------------
// GetVoxRvalue
//
// Returns a volume voxel R value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//-----------------------------------------------------------------------
tVoxel VoxelFAN24::GetVoxRvalue(long x, long y, long z)
{
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(z < 0) z = 0;

	if(x > (m_cubeX - 1)) x = m_cubeX - 1;
	if(y > (m_cubeY - 1)) y = m_cubeY - 1;
	if(z > (m_cubeZ - 1)) z = m_cubeZ - 1;

	if (m_scanDirect == SD_AntiClockwise) 
	{
		z = m_cubeZ - 1 - z;
	}
	if (m_probeOrient == PO_Bottom)
	{
		y = m_cubeY - 1 - y;
	}
	if (m_horReflect == HIR_Reflected)
	{
		x = m_cubeX - 1 - x;
	}

	tVoxel vox = m_pPixelData[(x + y * m_cubeX + z * m_xySize)*3 + 0];
	return tVoxel(vox);
}

//----------------------------------------------------------------------
// GetVoxGvalue
//
// Returns a volume voxel G value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//-----------------------------------------------------------------------
tVoxel VoxelFAN24::GetVoxGvalue(long x, long y, long z) 
{
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(z < 0) z = 0;

	if(x > (m_cubeX - 1)) x = m_cubeX - 1;
	if(y > (m_cubeY - 1)) y = m_cubeY - 1;
	if(z > (m_cubeZ - 1)) z = m_cubeZ - 1;

	if (m_scanDirect == SD_AntiClockwise) 
	{
		z = m_cubeZ - 1 - z;
	}
	if (m_probeOrient == PO_Bottom)
	{
		y = m_cubeY - 1 - y;
	}
	if (m_horReflect == HIR_Reflected)
	{
		x = m_cubeX - 1 - x;
	}

	tVoxel vox = m_pPixelData[(x + y * m_cubeX + z * m_xySize)*3 + 1];
	return tVoxel(vox);
}

//----------------------------------------------------------------------
// GetVoxBvalue
//
// Returns a volume voxel B value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//-----------------------------------------------------------------------
tVoxel VoxelFAN24::GetVoxBvalue(long x, long y, long z) 
{
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(z < 0) z = 0;

	if(x > (m_cubeX - 1)) x = m_cubeX - 1;
	if(y > (m_cubeY - 1)) y = m_cubeY - 1;
	if(z > (m_cubeZ - 1)) z = m_cubeZ - 1;

	if (m_scanDirect == SD_AntiClockwise)
	{
		z = m_cubeZ - 1 - z;
	}
	if (m_probeOrient == PO_Bottom)
	{
		y = m_cubeY - 1 - y;
	}
	if (m_horReflect == HIR_Reflected)
	{
		x = m_cubeX - 1 - x;
	}

	tVoxel vox = m_pPixelData[(x + y * m_cubeX + z * m_xySize)*3 + 2];
	return tVoxel(vox);
}
