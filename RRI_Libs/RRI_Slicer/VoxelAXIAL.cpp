//#include "StdAfx.h"
#include "VoxelAXIAL.h"



//----------------------------------------------------------------------
// VoxelAXIAL constructor
//
// 
//----------------------------------------------------------------------
VoxelAXIAL::VoxelAXIAL(TvoxelInit voxelInit) :

					Voxel()
{
	m_pPixelData  = voxelInit.data;

	m_probeOrient = voxelInit.acq.probeOrientation;			

	buildReverseMap(voxelInit.cal, voxelInit.acq);

	setModelToArray();
}


//----------------------------------------------------------------------
// VoxelAXIAL destrucror
//
// 
//----------------------------------------------------------------------
VoxelAXIAL::~VoxelAXIAL()
{
	delete[] m_pReverseMap;

	if(m_pWeightTable != 0)
	{
		delete[] m_pWeightTable;
	}
}

//----------------------------------------------------------------------
// GetVoxValue
//
// Returns a volume voxel value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//-----------------------------------------------------------------------
tVoxel VoxelAXIAL::GetVoxValue(Vector3F vF)
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


    //-------------------------------------------------------

    //HACK_ALERT, y is z and z is y in axial reconstruction

	if (m_scanDirect == SD_AntiClockwise)
	{
        y = m_cubeY - 1 - y;
	}

	if (m_probeOrient == PO_Bottom)
	{
		z = m_cubeZ - 1 - z;
	}

    
	if (m_horReflect == HIR_Reflected)
	{
		x = m_cubeX - 1 - x;
	}

    //-------------------------------------------------------


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


            if (index1 >= 0 && index1 <  m_cubeX*m_cubeY*m_cubeZ)
            {
           
			    vox1 = m_pPixelData[index1];
			
			    long lineNum = indexNum / m_cubeX;
			    long inOnLineNum = indexNum % m_cubeX; 

			    inOnLineNum = m_cubeX * lineNum + (m_cubeX - inOnLineNum - 1);

			    index2 = m_size180Sweep + frameNum * (m_cubeX * m_cubeZ) + inOnLineNum;

                if (index2 >= 0 && index2 < index2 < m_cubeX*m_cubeY*m_cubeZ)
                {
                
			        vox2 = m_pPixelData[index2];

			        vox = vox1 * m_pWeightTable[frameNum] + vox2 * (1.0 - m_pWeightTable[frameNum]);
                }
                else
                {
                    vox = 0;
                }
            }
            else
            {
                vox = 0;
            }

			return (tVoxel)vox;
		}

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
// GetVoxValue
//
// Returns a volume voxel value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//-----------------------------------------------------------------------
tVoxel VoxelAXIAL::GetVoxValue(long x, long y, long z)
{
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(z < 0) z = 0;

	if(x > (m_cubeX - 1)) x = m_cubeX - 1;
	if(y > (m_cubeY - 1)) y = m_cubeY - 1;
	if(z > (m_cubeZ - 1)) z = m_cubeZ - 1;

	//-------------------------------------------------------

    //HACK_ALERT, y is z and z is y in axial reconstruction

	if (m_scanDirect == SD_AntiClockwise)
	{
        y = m_cubeY - 1 - y;
	}

	if (m_probeOrient == PO_Bottom)
	{
		z = m_cubeZ - 1 - z;
	}

    
	if (m_horReflect == HIR_Reflected)
	{
		x = m_cubeX - 1 - x;
	}

    //-------------------------------------------------------

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
            if (index1 >= 0 && index1 <  m_cubeX*m_cubeY*m_cubeZ)
            {
           
			    vox1 = m_pPixelData[index1];
			
			    long lineNum = indexNum / m_cubeX;
			    long inOnLineNum = indexNum % m_cubeX; 

			    inOnLineNum = m_cubeX * lineNum + (m_cubeX - inOnLineNum - 1);

			    index2 = m_size180Sweep + frameNum * (m_cubeX * m_cubeZ) + inOnLineNum;

                if (index2 >= 0 && index2 < index2 < m_cubeX*m_cubeY*m_cubeZ)
                {
                
			        vox2 = m_pPixelData[index2];

			        vox = vox1 * m_pWeightTable[frameNum] + vox2 * (1.0 - m_pWeightTable[frameNum]);
                }
                else
                {
                    vox = 0;
                }
            }
            else
            {
                vox = 0;
            }


			return (tVoxel)vox;
		}

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
// the voxel coordinates from axial reconstructed indices to unreconstructed
// See Axial reconstruction Robarts patent.
//
// Input:
// CalParam structure filled with volume information
// AcqParam structure filled with volume information
//
// Output:
// none
//----------------------------------------------------------------------
void VoxelAXIAL::buildReverseMap(CalParam cal, AcqParam acq)
{
	m_cubeX = acq.width;			
	m_cubeY = acq.width;			
	m_cubeZ = acq.height; 

	m_voxX = cal.xVoxelSize;
	m_voxY = cal.xVoxelSize;
	m_voxZ = cal.yVoxelSize;

	long width = m_cubeX;
	long height = m_cubeY;

	long rawSliceOffset = acq.height * acq.width;

	m_pReverseMap = new unsigned long[m_cubeX * m_cubeY];

	unsigned long *rMap = m_pReverseMap;

	float AOR = cal.axisOfRotation;
	float MAX_R = width / 2;
	
	float shiftX = float(acq.width) / 2.0;
	float shiftY = float(acq.width) / 2.0;
	
	float slicesPerDegree = float(acq.numFrames - 1) / acq.sweepAngle;


	for(long i = 0; i < height; i++)
	{
		for(long j = 0; j < width; j++)
		{

			float x =  float(j) - shiftX;			
			float y = shiftY - float(i);	
		
			float f_r = sqrt(x * x + y * y);	
			float f_a = atan2(y,x);	
				

			if(f_r >= MAX_R)
			{
				rMap[j + i * width] = 0; 
			}
			else
			{
				float fSlice_num, fPix_num;
			
				f_a = RAD_TO_DEG(f_a);	
				
				//if((acq.scanDirection == SD_AntiClockwise && m_probeOrient == PO_Top) || 
				//   (acq.scanDirection == SD_Clockwise && m_probeOrient == PO_Bottom))

                if (acq.scanDirection == SD_Clockwise)
				{
					if(f_a <= 0)	
					{		
						fSlice_num = (180.0 + f_a) * slicesPerDegree;
						fPix_num = AOR - f_r;	
					}
					else 
					{
						fSlice_num = f_a * slicesPerDegree;	
						fPix_num = f_r  + AOR;	
					}	
				}
				else 
				{
					if(f_a <= 0)
					{			
						fSlice_num = (-f_a) * slicesPerDegree;
						fPix_num = f_r  + AOR;	
					}
					else
					{
						fSlice_num = (180.0 - f_a) * slicesPerDegree;		
						fPix_num = AOR - f_r;	
					}
				}

				long slice_num = floor(fSlice_num);
				long pix_num = floor(fPix_num);

				rMap[j + i * width] = pix_num  + slice_num * rawSliceOffset;
			}
		}
	}
	
	// extra frames
	m_extraFrames = acq.sweepAngle - slicesPerDegree * 180 - 1;
	long frames180Sweep = acq.numFrames - m_extraFrames;
	m_size180Sweep = rawSliceOffset * frames180Sweep;
	
	m_pWeightTable = 0;
	if(m_extraFrames > 0)
	{
		m_pWeightTable = new float[m_extraFrames];

		for(long i = 0; i < m_extraFrames; i++)
		{
			m_pWeightTable[i] = ((float)m_extraFrames - 2.0) / float(m_extraFrames * m_extraFrames) * float(i) + 1.0 / (float)m_extraFrames;
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
tVoxel VoxelAXIAL::GetVoxRvalue(long x, long y, long z)
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
tVoxel VoxelAXIAL::GetVoxGvalue(long x, long y, long z) 
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
tVoxel VoxelAXIAL::GetVoxBvalue(long x, long y, long z) 
{
    return 0;
}
