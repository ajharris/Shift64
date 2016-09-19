//#include "StdAfx.h"
#include "VoxelAXIAL24.h"



//----------------------------------------------------------------------
// VoxelAXIAL24 constructor
//
// 
//----------------------------------------------------------------------
VoxelAXIAL24::VoxelAXIAL24(TvoxelInit voxelInit) :

					VoxelAXIAL(voxelInit)
{
}


//----------------------------------------------------------------------
// VoxelAXIAL24 destrucror
//
// 
//----------------------------------------------------------------------
VoxelAXIAL24::~VoxelAXIAL24()
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
tVoxel VoxelAXIAL24::GetVoxBvalue(Vector3F vF) 
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
	unsigned long rmi = m_pReverseMap[y + x * m_cubeX];

	if (rmi <= 0)
	{
		return 0;
	}
	else 
	{
		long index;
	
		index = rmi + z * m_cubeX;

		long frameNum = index / (m_cubeX * m_cubeZ);
		long indexNum = index % (m_cubeX * m_cubeZ);
		
		if(frameNum < m_extraFrames)
		{
			long index1, index2;
			float vox1, vox2, vox;

			index1 = frameNum * (m_cubeX * m_cubeZ) + indexNum;
			vox1 = m_pPixelData[index1];
			
			long lineNum = indexNum / m_cubeX;
			long inOnLineNum = indexNum % m_cubeX; 

			inOnLineNum = m_cubeX * lineNum + (m_cubeX - inOnLineNum - 1);

			index2 = m_size180Sweep + frameNum * (m_cubeX * m_cubeZ) + inOnLineNum;
			vox2 = m_pPixelData[index2];

			vox = vox1 * m_pWeightTable[frameNum] + vox2 * (1.0 - m_pWeightTable[frameNum]);

			return (tVoxel)vox;
		}

		return m_pPixelData[index*3];
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
tVoxel VoxelAXIAL24::GetVoxGvalue(Vector3F vF) 
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

	unsigned long rmi = m_pReverseMap[y + x * m_cubeX];

	if (rmi <= 0)
	{
		return 0;
	}
	else 
	{
		long index;
	
		index = rmi + z * m_cubeX;

		long frameNum = index / (m_cubeX * m_cubeZ);
		long indexNum = index % (m_cubeX * m_cubeZ);
		
		if(frameNum < m_extraFrames)
		{
			long index1, index2;
			float vox1, vox2, vox;

			index1 = frameNum * (m_cubeX * m_cubeZ) + indexNum;
			vox1 = m_pPixelData[index1];
			
			long lineNum = indexNum / m_cubeX;
			long inOnLineNum = indexNum % m_cubeX; 

			inOnLineNum = m_cubeX * lineNum + (m_cubeX - inOnLineNum - 1);

			index2 = m_size180Sweep + frameNum * (m_cubeX * m_cubeZ) + inOnLineNum;
			vox2 = m_pPixelData[index2];

			vox = vox1 * m_pWeightTable[frameNum] + vox2 * (1.0 - m_pWeightTable[frameNum]);

			return (tVoxel)vox;
		}

		return m_pPixelData[index*3+1];
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
tVoxel VoxelAXIAL24::GetVoxRvalue(Vector3F vF)
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

	unsigned long rmi = m_pReverseMap[y + x * m_cubeX];

	if (rmi <= 0)
	{
		return 0;
	}
	else 
	{
		long index;
	
		index = rmi + z * m_cubeX;

		long frameNum = index / (m_cubeX * m_cubeZ);
		long indexNum = index % (m_cubeX * m_cubeZ);
		
		if(frameNum < m_extraFrames)
		{
			long index1, index2;
			float vox1, vox2, vox;

			index1 = frameNum * (m_cubeX * m_cubeZ) + indexNum;
			vox1 = m_pPixelData[index1];
			
			long lineNum = indexNum / m_cubeX;
			long inOnLineNum = indexNum % m_cubeX; 

			inOnLineNum = m_cubeX * lineNum + (m_cubeX - inOnLineNum - 1);

			index2 = m_size180Sweep + frameNum * (m_cubeX * m_cubeZ) + inOnLineNum;
			vox2 = m_pPixelData[index2];

			vox = vox1 * m_pWeightTable[frameNum] + vox2 * (1.0 - m_pWeightTable[frameNum]);

			return (tVoxel)vox;
		}

		return m_pPixelData[index*3+2];
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
tVoxel VoxelAXIAL24::GetVoxRvalue(long x, long y, long z)
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
tVoxel VoxelAXIAL24::GetVoxGvalue(long x, long y, long z) 
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
tVoxel VoxelAXIAL24::GetVoxBvalue(long x, long y, long z) 
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
