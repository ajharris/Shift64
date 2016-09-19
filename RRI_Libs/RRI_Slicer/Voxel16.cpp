//#include "StdAfx.h"
#include "Voxel16.h"


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
Voxel16::Voxel16(TvoxelInit voxelInit) :

					Voxel(voxelInit)
{
	m_left = 0;
	m_right = 4095;
	m_max = 4095;

	m_slope = float(m_right - m_left) / 255.0;
}


//----------------------------------------------------------------------
// Voxel destrucror
//
//----------------------------------------------------------------------
Voxel16::~Voxel16()
{

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
tVoxel Voxel16::GetVoxValue(Vector3F vF)
{
	// take integer part of voxel indices						
	Vector3F vI = fxd::FPint(vF);

	// original index 
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

	// compute fractional part of voxel indices, and their complements
	long Uf = fxd::FPfrac(vF.x); long Uc = FPone - Uf;
	long Vf = fxd::FPfrac(vF.y); long Vc = FPone - Vf;
	long Wf = fxd::FPfrac(vF.z); long Wc = FPone - Wf;
	
	// around original index 
	long xp = vI.x + 1;
	long yp = vI.y + 1;
	long zp = vI.z + 1;
		  
	if(xp < 0) xp = 0;
	if(yp < 0) yp = 0;
	if(zp < 0) zp = 0;

	if(xp > (m_cubeX - 1)) xp = m_cubeX - 1;
	if(yp > (m_cubeY - 1)) yp = m_cubeY - 1;
	if(zp > (m_cubeZ - 1)) zp = m_cubeZ - 1;
	
	if (m_scanDirect == SD_AntiClockwise) 
	{
		zp = m_cubeZ - 1 - zp;
	}
	if (m_probeOrient == PO_Bottom)
	{
		yp = m_cubeY - 1 - yp;
	}
	if (m_horReflect == HIR_Reflected)
	{
		xp = m_cubeX - 1 - xp;
	}
		  
	///////////////////////////////////////////////////////////////////
	/*
	tVoxel a_u = m_pPixelData[(x + y * m_cubeX + z * m_xySize)*2];
	tVoxel b_u = m_pPixelData[(xp + y * m_cubeX + z * m_xySize)*2];
	tVoxel c_u = m_pPixelData[(x + yp * m_cubeX + z * m_xySize)*2];
	tVoxel d_u = m_pPixelData[(xp + yp * m_cubeX + z * m_xySize)*2];
	tVoxel e_u = m_pPixelData[(x + y * m_cubeX + zp * m_xySize)*2];
	tVoxel f_u = m_pPixelData[(xp + y * m_cubeX + zp * m_xySize)*2];
	tVoxel g_u = m_pPixelData[(x + yp * m_cubeX + zp * m_xySize)*2];
	tVoxel h_u = m_pPixelData[(xp + yp * m_cubeX + zp * m_xySize)*2];
				
	tVoxel a_l = m_pPixelData[(x + y * m_cubeX + z * m_xySize)*2+1];
	tVoxel b_l = m_pPixelData[(xp + y * m_cubeX + z * m_xySize)*2+1];
	tVoxel c_l = m_pPixelData[(x + yp * m_cubeX + z * m_xySize)*2+1];
	tVoxel d_l = m_pPixelData[(xp + yp * m_cubeX + z * m_xySize)*2+1];
	tVoxel e_l = m_pPixelData[(x + y * m_cubeX + zp * m_xySize)*2+1];
	tVoxel f_l = m_pPixelData[(xp + y * m_cubeX + zp * m_xySize)*2+1];
	tVoxel g_l = m_pPixelData[(x + yp * m_cubeX + zp * m_xySize)*2+1];
	tVoxel h_l = m_pPixelData[(xp + yp * m_cubeX + zp * m_xySize)*2+1];
	*/

	tVoxel a_u = m_pPixelData[(x + y * m_cubeX + z * m_xySize)*2+1];
	tVoxel b_u = m_pPixelData[(xp + y * m_cubeX + z * m_xySize)*2+1];
	tVoxel c_u = m_pPixelData[(x + yp * m_cubeX + z * m_xySize)*2+1];
	tVoxel d_u = m_pPixelData[(xp + yp * m_cubeX + z * m_xySize)*2+1];
	tVoxel e_u = m_pPixelData[(x + y * m_cubeX + zp * m_xySize)*2+1];
	tVoxel f_u = m_pPixelData[(xp + y * m_cubeX + zp * m_xySize)*2+1];
	tVoxel g_u = m_pPixelData[(x + yp * m_cubeX + zp * m_xySize)*2+1];
	tVoxel h_u = m_pPixelData[(xp + yp * m_cubeX + zp * m_xySize)*2+1];
				
	tVoxel a_l = m_pPixelData[(x + y * m_cubeX + z * m_xySize)*2];
	tVoxel b_l = m_pPixelData[(xp + y * m_cubeX + z * m_xySize)*2];
	tVoxel c_l = m_pPixelData[(x + yp * m_cubeX + z * m_xySize)*2];
	tVoxel d_l = m_pPixelData[(xp + yp * m_cubeX + z * m_xySize)*2];
	tVoxel e_l = m_pPixelData[(x + y * m_cubeX + zp * m_xySize)*2];
	tVoxel f_l = m_pPixelData[(xp + y * m_cubeX + zp * m_xySize)*2];
	tVoxel g_l = m_pPixelData[(x + yp * m_cubeX + zp * m_xySize)*2];
	tVoxel h_l = m_pPixelData[(xp + yp * m_cubeX + zp * m_xySize)*2];
	
	long a = a_u;
	a = (a << 8) + a_l;
	if(a < m_left)  a = 0;
	else if(a > m_right) a = 254;
	else a = (a - m_left) / m_slope;
	
	long b = b_u;
	b = (b << 8) + b_l;
	if(b < m_left)  b = 0;
	else if(b > m_right) b = 254;
	else b = (b - m_left) / m_slope;
	
	long c = c_u;
	c = (c << 8) + c_l;
	if(c < m_left)  c = 0;
	else if(c > m_right) c = 254;
	else c = (c - m_left) / m_slope;
	
	long d = d_u;
	d = (d << 8) + d_l;
	if(d < m_left)  d = 0;
	else if(d > m_right) d = 254;
	else d = (d - m_left) / m_slope;
	
	long e = e_u;
	e = (e << 8) + e_l;
	if(e < m_left)  e = 0;
	else if(e > m_right) e = 254;
	else e = (e - m_left) / m_slope;
	
	long f = f_u;
	f = (f << 8) + f_l;
	if(f < m_left)  f = 0;
	else if(f > m_right) f = 254;
	else f = (f - m_left) / m_slope;
	
	long g = g_u;
	g = (g << 8) + g_l;
	if(g < m_left)  g = 0;
	else if(g > m_right) g = 254;
	else g = (g - m_left) / m_slope;
	
	long h = h_u;
	h = (h << 8) + h_l;
	if(h < m_left)  h = 0;
	else if(h > m_right) h = 254;
	else h = (h - m_left) / m_slope;
	
	
	// interpolate in U-direction (giving 24-bit numbers; cut down to 12-bit)
	long ab = (a*Uc + b*Uf) >> 12;
	long cd = (c*Uc + d*Uf) >> 12;
	long ef = (e*Uc + f*Uf) >> 12;
	long gh = (g*Uc + h*Uf) >> 12;
	
	// interpolate in V-direction (giving 28-bit numbers; cut down to 12-bit)
	long abcd = (ab*Vc + cd*Vf) >> 16;
	long efgh = (ef*Vc + gh*Vf) >> 16;
	
	// interpolate in W-direction (giving 28-bit number; cut down to 8-bit)
	tVoxel voxel = (abcd*Wc + efgh*Wf) >> 20;
	
	//	if((voxel * m_slope) < 0) return 0;
	//	else if((voxel * m_slope + m_left) > m_right) return 254;
	
	if(voxel > 254) voxel = 254;
	
	return tVoxel(voxel);
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
tVoxel Voxel16::GetVoxValue(long x, long y, long z, long n)
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

	tVoxel upper = m_pPixelData[(x + y * m_cubeX + z * m_cubeX * m_cubeY)*2];
	tVoxel lower = m_pPixelData[(x + y * m_cubeX + z * m_cubeX * m_cubeY)*2+1];

	tVoxel vox = 0;

	if(n == 0)
	{
		vox = upper;
	}
	else
	{
		vox = lower;
	}
		
	return tVoxel(vox);
}


long Voxel16::GetVoxIndex(Vector3 point)
{
	return 0;
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
// tVoxel lower: lower byte of 16 bit value
// tVoxel upper: upper byte of 16 bit value
//----------------------------------------------------------------------
void Voxel16::GetVoxValue(long x, long y, long z, tVoxel& lower, tVoxel& upper)
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

	upper = m_pPixelData[(x + y * m_cubeX + z * m_cubeX * m_cubeY)*2];
	lower = m_pPixelData[(x + y * m_cubeX + z * m_cubeX * m_cubeY)*2+1];

}

//----------------------------------------------------------------------
// SetVoxValue
//----------------------------------------------------------------------
void Voxel16::SetVoxValue(long x, long y, long z, long n, unsigned char value)
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
	if (m_horReflect == HIR_Reflected)
	{
		x = m_cubeX - 1 - x;
	}

	if (m_probeOrient == PO_Bottom)
	{
		y = m_cubeY - 1 - y;
	}


	if(n == 0)
	{
		m_pPixelData[(x + y * m_cubeX + z * m_cubeX * m_cubeY)*2] = (tVoxel)value;
		
	}
	else
	{
		m_pPixelData[(x + y * m_cubeX + z * m_cubeX * m_cubeY)*2 + 1] = (tVoxel)value;
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
tVoxel Voxel16::GetVoxRvalue(long x, long y, long z)
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
tVoxel Voxel16::GetVoxGvalue(long x, long y, long z) 
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
tVoxel Voxel16::GetVoxBvalue(long x, long y, long z) 
{
    return 0;
}
