////#include "StdAfx.h"
#include "VoxelHybrid.h"

VoxelHybrid::VoxelHybrid(TvoxelInit voxelInit) :

					Voxel()
{
	
	m_pPixelData  = voxelInit.data;

	buildReverseMap(voxelInit.cal, voxelInit.acq);

	setModelToArray();
	
}


VoxelHybrid::~VoxelHybrid(void)
{
	//delete[] m_pReverseMap;
}


tVoxel VoxelHybrid::GetVoxValue(long x, long y, long z)
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

	long index = m_cubeZ * y + z;

	long rmi = m_pReverseMap[index];

	if (rmi < 0 )
	{
		return 0;
	}
	else 
	{
		index = rmi + x;

		return m_pPixelData[index];
	} 
}

long VoxelHybrid::GetVoxIndex(Vector3 point)
{
	long x = point.x;
	long y = point.y;
	long z = point.z;


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


	long index = m_cubeZ * y + z;

	long rmi = m_pReverseMap[index];

	if (rmi < 0 )
	{
		return -1;
	}
	else 
	{
		index = rmi + x;

		return index;
	} 

	return -1;
}

tVoxel VoxelHybrid::GetVoxValue(Vector3F vF)
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


	long index = m_cubeZ * y + z;

	long rmi = m_pReverseMap[index];

	if (rmi < 0 )
	{
		return 0;
	}
	else 
	{
		index = rmi + x;

		return m_pPixelData[index];
	} 
}




void VoxelHybrid::buildReverseMap(CalParam cal, AcqParam acq)
{
	
	long numOfFrames = acq.numFrames;

	m_voxX = cal.xVoxelSize;
	m_voxY = cal.yVoxelSize;
	m_voxZ = cal.probeDistance / (numOfFrames - 1);

	double theta = DEG_TO_RAD(acq.sweepAngle);
	double dtt = theta / (numOfFrames - 1);

	double lengthZ = cal.probeDistance + 2 * acq.height * m_voxY * sin(theta / 2.0);

	m_cubeX = acq.width;
	m_cubeY = acq.height;
	m_cubeZ = lengthZ / m_voxZ;

	long maxSize = m_cubeZ * m_cubeY;
	long* mapZ = new long[maxSize];
	long* mapY = new long[maxSize];

	for(long i = 0; i < maxSize; i++)
	{
		mapY[i] = -1;
		mapZ[i] = -1;
	}

	long index;
	float Y, Z; 

	//////////////////////////////////////////////////////////////////////////////
	
	for(long z = numOfFrames / 2; z > 0; z--)
	{
		for(long y = 0; y < m_cubeY; y++)
		{	
			
			Y = y * cos(z  * dtt);
			Z = m_cubeZ / 2 - (z * m_voxZ + y * m_voxY * sin(z * dtt)) / m_voxZ;

			
			///////////////////////

			index = m_cubeZ * (long)Y + (long)Z;
			if (index >=0 && index < maxSize)
			{
				mapZ[index] = numOfFrames / 2 - z;
				mapY[index] = y;
			}

			///////////////////////

			index = m_cubeZ * (long)Y + (m_cubeZ - (long)Z - 1);

			if (index >=0 && index < maxSize)
			{
				mapZ[index] = numOfFrames - 1 - (numOfFrames / 2 - z);
				mapY[index] = y;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////

	for(long y = 0; y < m_cubeY; y++)
	{
		for(long z = 0; z < m_cubeZ / 2; z++)
		{
			index = m_cubeZ * y + z;

			if (mapZ[index] >= 0 || mapY[index] >= 0)
			{
				for(long i = z; i < (m_cubeZ - z); i++)
				{
					index = m_cubeZ * y + i;

					if(mapZ[index] < 0 || mapY[index] < 0)
					{
						mapZ[index] = -2;
						mapY[index] = -2;
					}
				}
				
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////

	long Ypos, Zpos;
	long howMany = 10;
	long left, right;

	m_pReverseMap = new unsigned long[m_cubeZ * m_cubeY];

	for(long i = 0; i < m_cubeZ * m_cubeY; i++)
	{
		m_pReverseMap[i] = -1;
	}

	for(long y = 0; y < m_cubeY; y++)
	{
		for(long z = 0; z < m_cubeZ; z++)
		{
			index = m_cubeZ * y + z;

			Ypos = mapY[index];		
			Zpos = mapZ[index];

			if(Ypos >= 0 || Zpos >= 0) 
			{
				m_pReverseMap[index] = m_cubeX * m_cubeY * Zpos + m_cubeX * Ypos;
			}

			if(Ypos == -2 || Zpos == -2) 
			{
				for(long i = 1; i <= howMany; i++)
				{
					left = index - i;
					right = index + i;

					if(left >= 0)
					{
						if(mapZ[left] > -1)
						{
							mapZ[index] = mapZ[left];
							mapY[index] = mapY[left];

							m_pReverseMap[index] = m_cubeX * m_cubeY * mapZ[index] + m_cubeX * mapY[index];

							break;
						}
					}

					if(right <= (m_cubeZ - 1))
					{
						if(mapZ[right] > -1)
						{
							mapZ[index] = mapZ[right];
							mapY[index] = mapY[right];

							m_pReverseMap[index] = m_cubeX * m_cubeY * mapZ[index] + m_cubeX * mapY[index];

							break;
						}
					}
				}
			}
		}
	}

	delete[] mapZ;
	delete[] mapY;
	
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
tVoxel VoxelHybrid::GetVoxRvalue(long x, long y, long z)
{
    return 0;
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
tVoxel VoxelHybrid::GetVoxGvalue(long x, long y, long z) 
{
    return 0;
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
tVoxel VoxelHybrid::GetVoxBvalue(long x, long y, long z) 
{
    return 0;
}

