//#include "StdAfx.h"
#include "VoxelFAN.h"





//----------------------------------------------------------------------
// VoxelFAN constructor
//
// 
//----------------------------------------------------------------------
VoxelFAN::VoxelFAN(TvoxelInit voxelInit) :

					Voxel()
{
	m_pPixelData  = voxelInit.data;

	m_widthM = voxelInit.acq.width - 1;

	m_probeOrient = voxelInit.acq.probeOrientation;			

	buildReverseMap(voxelInit.cal, voxelInit.acq);

	float tX = m_cubeX;
	float tY = m_cubeY;
	float tZ = m_cubeZ;
	m_cubeY = tX;
	m_cubeX = tZ;
	m_cubeZ = tY;

	tX = m_voxX;
	tY = m_voxY;
	tZ = m_voxZ;	
	m_voxY = tX;
	m_voxX = tZ;
	m_voxZ = tY;

	setModelToArray();
}


//----------------------------------------------------------------------
// VoxelFAN destructor
//
// 
//----------------------------------------------------------------------
VoxelFAN::~VoxelFAN()
{
	delete[] m_pReverseMap;
}


//----------------------------------------------------------------------
// GetVoxValue
//
// Returns the volume voxel value
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none
//----------------------------------------------------------------------
tVoxel VoxelFAN::GetVoxValue(long x, long y, long z)
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


	unsigned long rmi = m_pReverseMap[y + z * m_cubeY];

	if (rmi <= 0)
	{
		return 0;
	}
	else 
	{

        long index = rmi + x;

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

tVoxel VoxelFAN::GetVoxValue(Vector3F vF)
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
// buildReverseMap
//
// Builds the reverse map which is used by GetVoxValue to translate 
// the voxel coordinates from fan reconstructed indices to unreconstructed
// See Fan reconstruction Robarts patent
//
// Input:
// CalParam structure filled with volume information
// AcqParam structure filled with volume information
//
// Output:
// none
//----------------------------------------------------------------------
void VoxelFAN::buildReverseMap(CalParam cal, AcqParam acq)
{		
	long rawPixOffset = acq.width;
	long rawSliceOffset = (long)acq.height * (long)acq.width;
				
	float halfSectorAngle = (DEG_TO_RAD(acq.sweepAngle) / 2.0f); 			
	float slicesPerDegree = float(acq.numFrames) / acq.sweepAngle;

	float shiftY = (float(acq.height) + (float)cal.axisOfRotation) * float(sin(halfSectorAngle));
	float d = (float)cal.axisOfRotation * (1.0f - float(cos(halfSectorAngle)));
	float shiftX = (float)cal.axisOfRotation - d;

	m_cubeX = (long)acq.height + long(d);
	m_cubeY = long(2.0f * shiftY);
	m_cubeZ = acq.width;

	m_voxX = cal.yVoxelSize;	
	m_voxY = cal.yVoxelSize;	
	m_voxZ = cal.xVoxelSize; 

	m_pReverseMap = new unsigned long[m_cubeX * m_cubeY];

	unsigned long *rMap = m_pReverseMap;	

	long width = m_cubeX;
	long height = m_cubeY;

	float AOR = cal.axisOfRotation;
	float MAX_R = ((float)acq.height - 1.00f) + AOR; 		
	float MIN_R = AOR;		 							
	float MIN_A = 0; 										
	float MAX_A = DEG_TO_RAD(((float)acq.numFrames - 1.00f) / slicesPerDegree);
	

	for(long i = 0; i < height; i++)
	{
		for(long j = 0; j < width; j++)
		{
			float x = shiftX + float(j);	
			float y = shiftY - float(i);	

			float f_r = float(sqrt(x * x + y * y));

			float f_a = float(atan2(y, x));
			f_a += halfSectorAngle;	
		
			
			if(f_r >= MAX_R || f_r < MIN_R || f_a < MIN_A || f_a >= MAX_A)
			{
				rMap[j + i * width] = 0; 
			}

			else
			{
				f_a = RAD_TO_DEG(f_a);	
				
				float fSlice_num = f_a * slicesPerDegree;
				float fPix_num = f_r  - AOR;

				if(acq.scanDirection == SD_Clockwise)		
				{
					fSlice_num = (acq.numFrames - 0.9) - fSlice_num;	
				}

				if (acq.probeOrientation == PO_Bottom)
				{
					fPix_num = (acq.height - 0.9) - fPix_num;	
				}

				long slice_num = long(floor(fSlice_num));
				long pix_num = long(floor(fPix_num));

				float fracSlice = fSlice_num - (float)slice_num;
				float fracPix = fPix_num - (float)pix_num;
			
				long index = pix_num * rawPixOffset  + slice_num * rawSliceOffset;	
			
				if(fracSlice > 0.5)
				{
					index += rawSliceOffset;
				}
				
				if(fracPix > 0.5)
				{
					index += rawPixOffset;
				}
			
				rMap[j + i*width] = index;
			}
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
tVoxel VoxelFAN::GetVoxRvalue(long x, long y, long z)
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
tVoxel VoxelFAN::GetVoxGvalue(long x, long y, long z) 
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
tVoxel VoxelFAN::GetVoxBvalue(long x, long y, long z) 
{
    return 0;
}
