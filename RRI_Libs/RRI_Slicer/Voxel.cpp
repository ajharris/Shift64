//#include "StdAfx.h"
#include "Voxel.h"




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
Voxel::Voxel(TvoxelInit voxelInit)
{
	
	m_pPixelData = voxelInit.data;

	m_cubeX = voxelInit.acq.width;
	m_cubeY = voxelInit.acq.height;
	m_cubeZ = voxelInit.acq.numFrames;

	m_voxX = voxelInit.cal.xVoxelSize;
	m_voxY = voxelInit.cal.yVoxelSize;
	m_voxZ = voxelInit.cal.zVoxelSize;

	m_horReflect = voxelInit.acq.horizontalReflection;
	m_scanDirect = voxelInit.acq.scanDirection; 
	m_probeOrient = voxelInit.acq.probeOrientation; 

	m_xySize = m_cubeX * m_cubeY;
	m_xyzSize = m_cubeX * m_cubeY * m_cubeZ;

	setModelToArray();
	
}

	
Voxel::Voxel()
{
}


//----------------------------------------------------------------------
// Voxel destrucror
//
//----------------------------------------------------------------------							
Voxel::~Voxel()
{
}


//----------------------------------------------------------------------
// GetVoxValue
//
// Returns volume voxel value 
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// none 
//----------------------------------------------------------------------	
tVoxel Voxel::GetVoxValue(long x, long y, long z)
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
	
    long index = x + y * m_cubeX + z * m_xySize;
    if (index >= 0 && index < m_cubeX*m_cubeY*m_cubeZ)
    {
        return m_pPixelData[x + y * m_cubeX + z * m_xySize];
    }
    else
    {
        return 0;
    }
	
}

long Voxel::GetVoxIndex(Vector3 point)
{
	return 0;
}


//----------------------------------------------------------------------
// GetVoxValue
//
// Returns volume voxel value 
//
// Input:
// Vector3F vF - coordinates of the volume voxel in the reconstructed volume
//
// Output:
// none 
//----------------------------------------------------------------------	
tVoxel Voxel::GetVoxValue(Vector3F vF)
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

    long index = x + y * m_cubeX + z * m_xySize;
    if (index >= 0 && index < m_cubeX*m_cubeY*m_cubeZ)
    {
        return m_pPixelData[x + y * m_cubeX + z * m_xySize];
    }
    else
    {
        return 0;
    }
}


//----------------------------------------------------------------------
// OutOfBounds
//
// Performs check whether a voxel is inside of the volume bounds
//
// Input:
// x coordinate of the volume voxel in the reconstructed volume
// y coordinate of the volume voxel in the reconstructed volume
// z coordinate of the volume voxel in the reconstructed volume
//
// Output:
// true if outside, false if inside 
//----------------------------------------------------------------------	
bool Voxel::CheckOutOfBounds(long x, long y, long z)
{
	return ((x > (m_cubeX - 1)) || (y > (m_cubeY - 1)) || (z > (m_cubeZ - 1)) || (x < 0) || (y < 0) || (z < 0));
}


//----------------------------------------------------------------------
// GetModel2Array
//
// Returns the translation matrix from model to not reconstructed volume
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------	
Matrix4x4 Voxel::GetModel2Array()
{
	return m_model2Array;
}


//----------------------------------------------------------------------
// GetSizeXmm
//
// Returns the width of the reconstructed volume in mm
//
// Input:
// none
//
// Output:
// width of the reconstructed volume in mm
//----------------------------------------------------------------------	
float Voxel::GetSizeXmm()
{
	return (m_cubeX * m_voxX);
}


//----------------------------------------------------------------------
// float Voxel::GetSizeYmm()
//
// Returns the height of the reconstructed volume in mm
//
// Input:
// none
//
// Output:
// height of the reconstructed volume in mm
//----------------------------------------------------------------------	
float Voxel::GetSizeYmm()
{
	return (m_cubeY * m_voxY);
}


//----------------------------------------------------------------------
// GetSizeZmm
//
// Returns the z dimension size of the reconstructed volume in mm
//
// Input:
// none
//
// Output:
// z dimension size of the reconstructed volume in mm
//----------------------------------------------------------------------	
float Voxel::GetSizeZmm()
{
	return (m_cubeZ * m_voxZ);
}


//----------------------------------------------------------------------
// GetDimX
//
// Returns the width of the reconstructed volume in pixels
//
// Input:
// none
//
// Output:
// width of the reconstructed volume in pixels
//----------------------------------------------------------------------	
long Voxel::GetDimX()
{
	return m_cubeX;
}


//----------------------------------------------------------------------
// GetDimY
//
// Returns the height of the reconstructed volume in pixels
//
// Input:
// none
//
// Output:
// height of the reconstructed volume in pixels
//----------------------------------------------------------------------	
long Voxel::GetDimY()
{
	return m_cubeY;
}


//----------------------------------------------------------------------
// GetDimZ
//
// Returns the number of frames of the reconstructed volume
//
// Input:
// none
//
// Output:
// number of frames of the reconstructed volume
//----------------------------------------------------------------------	
long Voxel::GetDimZ()
{
	return m_cubeZ;
}


//----------------------------------------------------------------------
// setModelToArray
//
// Generate model to array translation matrix
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------	
void Voxel::setModelToArray()
{
	m_model2Array =	Matrix4x4
				   (1.0 / m_voxX,	        0.0,			        0.0,			        0.0,
			        0.0,			        1.0/m_voxY,	            0.0,			        0.0,
			        0.0,			        0.0,		            1.0 / m_voxZ,	        0.0,
					float(m_cubeX - 1)/2.0, float(m_cubeY - 1)/2.0, float(m_cubeZ - 1)/2.0, 1.0);
}


//----------------------------------------------------------------------
// setLeftRight
//
// Set the window for 16 bits data  
//
// Input:
// left, right extent of the window
//
// Output:
// number of faces in the cube model
//----------------------------------------------------------------------
void Voxel::setLeftRight(long left, long right)
{
	m_left = left;
	m_right = right;

	m_slope = float(m_right - m_left) / 255.0;
}


tVoxel Voxel::GetVoxRvalue(Vector3F vF) 
{	
	return tVoxel(0);
}

tVoxel Voxel::GetVoxGvalue(Vector3F vF) 
{	
	return tVoxel(0);
}

tVoxel Voxel::GetVoxBvalue(Vector3F vF) 
{	
	return tVoxel(0);
}


void Voxel::SetLeftRight(long left, long right)
{
	setLeftRight(left, right);

}

//----------------------------------------------------------------------
// SetVoxValue
//----------------------------------------------------------------------
void Voxel::SetVoxValue(long x, long y, long z, unsigned char value)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (z < 0) z = 0;

    if (x >(m_cubeX - 1)) x = m_cubeX - 1;
    if (y >(m_cubeY - 1)) y = m_cubeY - 1;
    if (z >(m_cubeZ - 1)) z = m_cubeZ - 1;

    if (m_scanDirect == SD_AntiClockwise)
    {
        z = m_cubeZ - 1 - z;
    }
    if (m_horReflect == HIR_Reflected)
    {
        x = m_cubeX - 1 - x;
    }

    if (m_probeOrient == PO_Bottom)
    {
        y = m_cubeY - 1 - y;
    }


   
    m_pPixelData[(x + y * m_cubeX + z * m_cubeX * m_cubeY)] = (tVoxel)value;

   

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
tVoxel Voxel::GetVoxRvalue(long x, long y, long z)
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
tVoxel Voxel::GetVoxGvalue(long x, long y, long z) 
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
tVoxel Voxel::GetVoxBvalue(long x, long y, long z) 
{
    return 0;
}
