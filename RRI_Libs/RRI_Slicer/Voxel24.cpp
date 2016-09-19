////#include "StdAfx.h"
#include "Voxel24.h"


//----------------------------------------------------------------------
// Voxel constructor
//
// Create voxel object 
//
// Input:
// none
//
// Output:
// none 
//----------------------------------------------------------------------
Voxel24::Voxel24(TvoxelInit voxelInit) :

					Voxel(voxelInit)
{
}


//----------------------------------------------------------------------
// Voxel24 destrucror
//
//----------------------------------------------------------------------
Voxel24::~Voxel24()
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
tVoxel Voxel24::GetVoxBvalue(Vector3F vF) 
{
	// take integer part of voxel indices						
	vF = fxd::FPint(vF);

	if(vF.x < 0) vF.x = 0;
	if(vF.y < 0) vF.y = 0;
	if(vF.z < 0) vF.z = 0;

	if(vF.x > (m_cubeX - 1)) vF.x = m_cubeX - 1;
	if(vF.y > (m_cubeY - 1)) vF.y = m_cubeY - 1;
	if(vF.z > (m_cubeZ - 1)) vF.z = m_cubeZ - 1;

	if (m_scanDirect == SD_AntiClockwise)
	{
		vF.z = m_cubeZ - 1 - vF.z;
	}
	if (m_probeOrient == PO_Bottom)
	{
		vF.y = m_cubeY - 1 - vF.y;
	}
	if (m_horReflect == HIR_Reflected)
	{
		vF.x = m_cubeX - 1 - vF.x;
	}

	tVoxel vox = m_pPixelData[(vF.x + vF.y * m_cubeX + vF.z * m_xySize)*3 + 2];
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
tVoxel Voxel24::GetVoxGvalue(Vector3F vF) 
{
	// take integer part of voxel indices						
	vF = fxd::FPint(vF);

	if(vF.x < 0) vF.x = 0;
	if(vF.y < 0) vF.y = 0;
	if(vF.z < 0) vF.z = 0;

	if(vF.x > (m_cubeX - 1)) vF.x = m_cubeX - 1;
	if(vF.y > (m_cubeY - 1)) vF.y = m_cubeY - 1;
	if(vF.z > (m_cubeZ - 1)) vF.z = m_cubeZ - 1;

	if (m_scanDirect == SD_AntiClockwise) 
	{
		vF.z = m_cubeZ - 1 - vF.z;
	}
	if (m_probeOrient == PO_Bottom)
	{
		vF.y = m_cubeY - 1 - vF.y;
	}
	if (m_horReflect == HIR_Reflected)
	{
		vF.x = m_cubeX - 1 - vF.x;
	}

	tVoxel vox = m_pPixelData[(vF.x + vF.y * m_cubeX + vF.z * m_xySize)*3 + 1];
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
tVoxel Voxel24::GetVoxRvalue(Vector3F vF)
{
	// take integer part of voxel indices						
	vF = fxd::FPint(vF);

	if(vF.x < 0) vF.x = 0;
	if(vF.y < 0) vF.y = 0;
	if(vF.z < 0) vF.z = 0;

	if(vF.x > (m_cubeX - 1)) vF.x = m_cubeX - 1;
	if(vF.y > (m_cubeY - 1)) vF.y = m_cubeY - 1;
	if(vF.z > (m_cubeZ - 1)) vF.z = m_cubeZ - 1;

	if (m_scanDirect == SD_AntiClockwise) 
	{
		vF.z = m_cubeZ - 1 - vF.z;
	}
	if (m_probeOrient == PO_Bottom)
	{
		vF.y = m_cubeY - 1 - vF.y;
	}
	if (m_horReflect == HIR_Reflected)
	{
		vF.x = m_cubeX - 1 - vF.x;
	}

	tVoxel vox = m_pPixelData[(vF.x + vF.y * m_cubeX + vF.z * m_xySize)*3 + 0];
	return tVoxel(vox);
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
tVoxel Voxel24::GetVoxRvalue(long x, long y, long z)
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
tVoxel Voxel24::GetVoxGvalue(long x, long y, long z) 
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
tVoxel Voxel24::GetVoxBvalue(long x, long y, long z) 
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
