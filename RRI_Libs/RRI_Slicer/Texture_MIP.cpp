// TextureMIP.cpp: implementation of the TextureMIP class.
//
//////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "VisLib.h"
#include "geometry.h"
#include "fixed.h"
#include "Voxel.h"
#include "VoxelFAN.h"
#include "VoxelAXIAL.h"
#include "Voxel16.h"
#include "Texture_MIP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------
// Texture constructor
//
// Create texture object to texture map the cube model faces
//
//----------------------------------------------------------------------
TextureMIP::TextureMIP(TvoxelInit voxelInit, TtexInit texInit) :
			
			Texture(voxelInit, texInit)
{
	m_pZbuff = new Fixed[m_polyClip.right * m_polyClip.bottom]; 

	m_min = 0;
	m_max = 255;
}


TextureMIP::~TextureMIP()
{
	delete[] m_pZbuff;
}


//----------------------------------------------------------------------
// ResizeImBuff
//
// clientX is the width of the view window
// clienty is the height of the view window
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void TextureMIP::ResizeImBuff(long x, long y)
{
	Texture::ResizeImBuff(x,y);

	if (m_pZbuff)
	{
		delete[] m_pZbuff;
	}
	m_pZbuff = new Fixed[m_polyClip.right * m_polyClip.bottom ];
}


//----------------------------------------------------------------------
// SetParameters
//
// Set the cutoff values for the input voxel intensities
//
// Input:
// min is a minimum cutoff of the input voxel intensities in the ray
// max is a maximum cutoff of the input voxel intensities in the ray
//
// Output:
// none
//----------------------------------------------------------------------
void TextureMIP::SetParameters(long min, long max)
{
	m_min = min;
	m_max = max;
}


//----------------------------------------------------------------------
// Render
//
// Given a model view texture map it and fill the image buffer  
//
// Input:
// viewORTHO structure to get the information on current view
//
// Output:
// none
//----------------------------------------------------------------------
void TextureMIP::Render(viewORTHO view, long facetotex)
{
	m_view2Array = view.View2Model * m_model2Array;

	Vector3 vec = Vector3(0, 0, 1) % m_view2Array; 
	
	m_length = static_cast<float>(sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
	
	Vector3 ray = !(Vector3(0, 0, 1) % m_view2Array); 
	m_RayU = fxd::FtoFP(ray.x);
	m_RayV = fxd::FtoFP(ray.y);
	m_RayW = fxd::FtoFP(ray.z);

	m_renderMode = ZBUFF;

	Face* iFace;

	//for (FaceVec::iterator iFace = view.face.begin(); iFace != view.face.end(); ++iFace)
	for (long i = 0; i < view.face.size(); i++)
	{
		iFace = &view.face[i];

		if (iFace->plane.normal.z > 0 && iFace->vert.size() > 2)
		{
			fill(iFace, 0, m_polyClip);
		}
	}

	m_renderMode = HIGHRES;
	memset(m_pImage, 0, m_polyClip.right * m_polyClip.bottom);


	//for (iFace = view.face.begin(); iFace != view.face.end(); ++iFace)
	for (long i = 0; i < view.face.size(); i++)
	{
		iFace = &view.face[i];
		if (iFace->plane.normal.z < 0 && iFace->vert.size() > 2)
		{
			fill(iFace, m_pImage, m_polyClip);
		}
	}	
}


//----------------------------------------------------------------------
// fill
//
// Texure map one cube model face
//
// Input:
// face of the cube model in the view coordinates
// tVoxel buff is the image buffer
// Rect polyClip contains the view window parameters
//
// Output:
// none
//----------------------------------------------------------------------
void TextureMIP::fill(Face* face, tVoxel *buff, Rect polyClip)
{
	long sideCount;
	long firstScan;
	long lastScan;
	long bottomScan;
	long xIntCount;

 	float A = face->plane.normal.x;
	float B = face->plane.normal.y;
	float C = face->plane.normal.z;
	float D = face->plane.D;

	m_delta_Zx = -A / C;
	m_delta_Zy = -B / C;
	m_delta_Tx = Vector3(1, 0, m_delta_Zx) % m_view2Array;
	m_delta_Ty = Vector3(0, 1, m_delta_Zy) % m_view2Array;

	for (long k = face->vert.size() - 1; k >= 0; k--)
	{
		m_vertices[k].x =  rnd(face->vert[k].x);
		m_vertices[k].y =  rnd(face->vert[k].y);
		m_vertices[k].scrn = Vector3(m_vertices[k].x, m_vertices[k].y, (D - A * m_vertices[k].x - B * m_vertices[k].y) / C);
		m_vertices[k].tex = m_vertices[k].scrn * m_view2Array;
	}

	sort_on_ymin(face->vert.size(),  &sideCount,  &bottomScan,  buff, polyClip);

	firstScan = 0;
	lastScan = 0;

	for (long y = m_sides[0].ymin; y <= bottomScan; y++)
	{
		update_first_and_last(sideCount, y, &firstScan, &lastScan);
		process_x_intersections(y, firstScan, lastScan, &xIntCount);
	
		draw_lines(y, xIntCount, firstScan, buff, polyClip);
	
		for (long scanLine = firstScan; scanLine <= lastScan; scanLine++)
		{
			if (m_sides[scanLine].delta_y > 0) 
			{
				m_sides[scanLine].delta_y--;
				float c = float(-fxd::FPround(m_sides[scanLine].x_int));
				m_sides[scanLine].x_int += m_sides[scanLine].x_inc;
				c += fxd::FPround(m_sides[scanLine].x_int);
				m_sides[scanLine].z += c * m_delta_Zx + m_delta_Zy;
				m_sides[scanLine].tex = m_sides[scanLine].tex + c * m_delta_Tx + m_delta_Ty;
			}
		}
	}
}


//----------------------------------------------------------------------
// draw_lines
//
// Texure map all line from the left side to the right side of the cube 
//
// Input:
// long scan - y coordinate of the line
// long x_int_count
// long index
// tVoxel buff is the image buffer
// Rect polyClip contains the view window parameters
//
// Output:
// none 
//----------------------------------------------------------------------
void TextureMIP::draw_lines(long scan, long x_int_count, long index, tVoxel *buff, Rect polyClip)
{
	long k,x1,x2;
	float z;
	Vector3 tex;

	for (k = 1; k <= x_int_count/2 ; k++)
	{
		while (m_sides[index].delta_y == 0)
		{
			index++;
		}

		x1 = fxd::FPround(m_sides[index].x_int);
		z = m_sides[index].z;
		tex = m_sides[index].tex;
		index++;

		while (m_sides[index].delta_y == 0)
		{
			index++;
		}

		x2 = fxd::FPround(m_sides[index].x_int);
		paint_line(scan,x1,x2,z,m_sides[index].z, m_delta_Zx, tex, m_sides[index].tex, m_delta_Tx, buff, polyClip);
		index++;
	}
}


//----------------------------------------------------------------------
// sort_on_ymin
//
// Sort vertices of a cube model face
//
// Input:
// n number of vertices in the face
// tVoxel buff is the image buffer
// Rect polyClip contains the view window parameters
//
// Output:
// side_count - number of lines to texture map
// bottomscan - y coordinate of the last line to texture map
//----------------------------------------------------------------------
void TextureMIP::sort_on_ymin(long n, long* side_count, long* bottomscan, tVoxel *buff, Rect polyClip)
{
	long p1 = n-1;
	long p2;

	*side_count = 0;
	*bottomscan = m_vertices[n - 1].y;

	for (p2 = 0; p2 < n; p2++)
	{
		if (m_vertices[p1].y != m_vertices[p2].y)
		{
			put_in_sides_list(*side_count,p1,p2,next_Y(n,p2));
			(*side_count)++;
		}
		else
		{
			if (m_vertices[p1].x < m_vertices[p2].x)
			{
				paint_line(m_vertices[p1].y, m_vertices[p1].x, m_vertices[p2].x,
						   m_vertices[p1].scrn.z, m_vertices[p2].scrn.z, m_delta_Zx,
						   m_vertices[p1].tex, m_vertices[p2].tex, m_delta_Tx, buff, polyClip);
			}
		}

		if (m_vertices[p2].y > *bottomscan)
		{
			*bottomscan = m_vertices[p2].y;
		}

		p1 = p2;
	}
}


//----------------------------------------------------------------------
// paint_line
//
// Texure map a line from the left side to the right side of the cube 
// model face
//
// Input:
// long y - y coordinate of the line 
// long x1 - x left coordinate of the line 
// long x2 - x right coordinate of the line 
// float z1 - position of first voxel in the reconstructed volume
// float z2 - position of first voxel in the reconstructed volume
// float delta_z - increment on the vector in the reconstructed volume
// Vector3 tex1 - not used
// Vector3 tex2 - not used
// Vector3 delta_tex - not used
// tVoxel buff is the image buffer
// Rect polyClip contains the view window parameters
//
// Output:
// none
//----------------------------------------------------------------------
void TextureMIP::paint_line(long y, long x1, long x2, float z1, float z2, float delta_z,Vector3 tex1, Vector3 tex2, Vector3 delta_tex, tVoxel *buff, Rect polyClip)
{
	long xmin, xmax;
	float z;
	Vector3 tex;
	long la;		
	Vector3 dtx;

	if (y < polyClip.top || y >= polyClip.bottom)
	{
		return;
	}

	if (x1 < x2)
	{ 
		xmin = x1;
		xmax = x2; 
		z = z1; 
		tex = tex1;
	}
	else
	{
		xmin = x2;
		xmax = x1; 
		z = z2; 
		tex = tex2;
	}

	if ((la = polyClip.left - xmin) > 0)
	{
		xmin = polyClip.left;
	}

	if (xmax >= polyClip.right)
	{
		xmax = polyClip.right - 1;
	}

	if (xmax < xmin)
	{
		return;	
	}

	if (la > 0)
	{
		z += la * delta_z;
		dtx = static_cast<float>(la) * delta_tex;
		tex = tex + dtx;
	}

	long loop;
	Fixed *theZvalue, zF, dzF;

	switch(m_renderMode)
	{
		case HIGHRES:
			drawline(y, xmin, xmax, z, delta_z, tex, delta_tex, m_pImage, polyClip);
			break;

		case ZBUFF:
			theZvalue = m_pZbuff + y * polyClip.right + xmin;
			zF = fxd::FtoFP(z * m_length);       
			dzF = fxd::FtoFP(delta_z * m_length);

			for (loop = xmax - xmin; loop > -1; loop--)
			{
				*theZvalue = zF;
				zF += dzF;
				++theZvalue;
			}
			break;

		default:
			break;
	}
}


//----------------------------------------------------------------------
// drawline
//
// Texure map a line from the left side to the right side of the cube 
// model face
//
// Input:
// long y - y coordinate of the line 
// long xl - x left coordinate of the line 
// long xr - x right coordinate of the line
// float z - not used
// float dz - not used
// Vector3 vCoordStart - position of first voxel in the reconstructed volume
// Vector3 vCoordIncrement - increment on the vector in the reconstructed volume 
// tVoxel buff is the image buffer
// Rect polyClip contains the view window parameters
//
// Output:
// none
//----------------------------------------------------------------------
void TextureMIP::drawline( long y, long xl, long xr, float z, float dz, Vector3 vCoordStart, Vector3 vCoordIncrement, tVoxel *buff, Rect polyClip)
{
	tVoxel *theMag	 = m_pImage + y * polyClip.right + xl;    
	Fixed* theZvalue = m_pZbuff + y * polyClip.right + xl;		  
	unsigned long uuL, vvL, wwL;
	Fixed uu,vv,ww;	
	
	Fixed u = fxd::FtoFP(vCoordStart.x) + FPhalf;
	Fixed v = fxd::FtoFP(vCoordStart.y) + FPhalf;
	Fixed w = fxd::FtoFP(vCoordStart.z) + FPhalf;

	Fixed du = fxd::FtoFP(vCoordIncrement.x);
	Fixed dv = fxd::FtoFP(vCoordIncrement.y);
	Fixed dw = fxd::FtoFP(vCoordIncrement.z);

	tVoxel voxVal, maxVox;
	Fixed dzF = fxd::FtoFP(dz * m_length); 
	Fixed zF = fxd::FtoFP(z * m_length);
	long rayLenZ = fxd::FPint(*theZvalue - zF);  
 
	for (long loop = xr - xl; loop > 0; loop--)
	{
		uu = u; 
		vv = v; 
		ww = w;
		uuL= uu >> 16;
		vvL= vv >> 16;
		wwL= ww >> 16;

		long count = rayLenZ;

		maxVox = 0;
	

		while (count > 0)
		{
			voxVal = m_pVoxel->GetVoxValue(uuL, vvL, wwL);

			if((voxVal > m_min) && (voxVal < m_max) && (voxVal != 255) && (voxVal > maxVox))    
			{
				maxVox  = voxVal;
			}
		
			--count;
			uu += m_RayU;
			vv += m_RayV;
			ww += m_RayW;
			uuL = uu >> 16;
			vvL = vv >> 16;
			wwL = ww >> 16;
		}

		*theMag++ = m_greyMap[maxVox];
		++theZvalue;
		zF += dzF;
		rayLenZ = fxd::FPint(*theZvalue - zF);

		u += du;
		v += dv; 
		w += dw;
	}
}	