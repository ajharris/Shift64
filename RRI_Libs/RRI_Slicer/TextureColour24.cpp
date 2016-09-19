// TextureColour24.cpp: implementation of the TextureColour24 class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "VisLib.h"
#include "geometry.h"
#include "fixed.h"
#include "Voxel.h"
#include "VoxelFAN.h"
#include "VoxelAXIAL.h"
#include "Voxel24.h"
#include "TextureColour24.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TextureColour24::TextureColour24(TvoxelInit voxelInit, TtexInit texInit) :
			
			Texture(voxelInit, texInit)
{		
	redPlane = new tVoxel[m_polyClip.right * m_polyClip.bottom];
	greenPlane = new tVoxel[m_polyClip.right * m_polyClip.bottom];
	bluePlane = new tVoxel[m_polyClip.right * m_polyClip.bottom];
}

TextureColour24::~TextureColour24()
{
	delete[] redPlane;
	delete[] greenPlane;
	delete[] bluePlane;
}

void TextureColour24::ResizeImBuff(long x, long y)
{
	Texture::ResizeImBuff(x,y);

	if (redPlane)
	{
		delete[] redPlane;
	}
	redPlane = new tVoxel[m_polyClip.right * m_polyClip.bottom];

	if (greenPlane)
	{
		delete[] greenPlane;
	}
	greenPlane = new tVoxel[m_polyClip.right * m_polyClip.bottom];

	if (bluePlane)
	{
		delete[] bluePlane;
	}
	bluePlane = new tVoxel[m_polyClip.right * m_polyClip.bottom];

	memset(redPlane, 0, m_polyClip.right * m_polyClip.bottom);
	memset(greenPlane, 0, m_polyClip.right * m_polyClip.bottom);
	memset(bluePlane, 0, m_polyClip.right * m_polyClip.bottom);
}

void TextureColour24::Render(viewORTHO view, long mode)
{
	m_view2Array = view.View2Model * m_model2Array;
	memset(m_pImage, 0, m_polyClip.right * m_polyClip.bottom);
	
	Face* iFace;
	
	//for (FaceVec::iterator i = view.face.begin(); i != view.face.end(); ++i)
	for (long i = 0; i < view.face.size(); i++)
	{
		iFace = &view.face[i];

		//if (i->plane.normal.z < 0)
		if (iFace->plane.normal.z < 0)
		{
			//fill(i, m_pImage, m_polyClip);
			fill(iFace, m_pImage, m_polyClip);
		}
	}
	
	fxedToVox(m_pImage, m_polyClip);

	memset(redPlane, 0, m_polyClip.right * m_polyClip.bottom);
	memset(greenPlane, 0, m_polyClip.right * m_polyClip.bottom);
	memset(bluePlane, 0, m_polyClip.right * m_polyClip.bottom);
}


void TextureColour24::fxedToVox(tVoxel *imBuff, Rect clip)
{
 	if (m_pImage) 
	{
		delete[] m_pImage;
	}
	m_pImage = new tVoxel[m_polyClip.right * m_polyClip.bottom * 3];
	memset(m_pImage, 0, m_polyClip.right * m_polyClip.bottom * 3);

	long index = 0;
	for(long i = 0; i < m_polyClip.right * m_polyClip.bottom; i++)
	{
		m_pImage[index++] = bluePlane[i];
		m_pImage[index++] = greenPlane[i];
		m_pImage[index++] = redPlane[i]; 
	} 
}



FilledBuff TextureColour24::GetImBuff(void)
{
	return FilledBuff(m_pImage, m_polyClip.right * 3, m_polyClip.bottom);
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
void TextureColour24::fill(Face* face, tVoxel *buff, Rect polyClip)
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
void TextureColour24::draw_lines(long scan, long x_int_count, long index, tVoxel *buff, Rect polyClip)
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
void TextureColour24::sort_on_ymin(long n, long* side_count, long* bottomscan, tVoxel *buff, Rect polyClip)
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
void TextureColour24::paint_line(long y, long x1, long x2, float z1, float z2, 
							float delta_z,Vector3 tex1, Vector3 tex2, Vector3 delta_tex, tVoxel *buff, Rect polyClip)
{
	long xmin, xmax;
	float z;
	long la;		
	Vector3 tex;
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

	drawline(y,xmin,xmax,z,delta_z,tex,delta_tex, buff, polyClip);
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
void TextureColour24::drawline(long y, long xl, long xr, float z, float dz,
									               Vector3 vCoordStart, Vector3 vCoordIncrement, tVoxel *buff, Rect polyClip)
{
	tVoxel* theR = redPlane + y*polyClip.right + xl;  
	tVoxel* theG = greenPlane + y*polyClip.right + xl;  
	tVoxel* theB = bluePlane + y*polyClip.right + xl;  

	Vector3F arrayCoordIncF = fxd::FtoFP(vCoordIncrement);
	Vector3F arrayCoordF    = fxd::FtoFP(vCoordStart) + Vector3F(FPhalf, FPhalf, FPhalf);
	tVoxel vox;
	for (long loop = xr - xl; loop > 0; --loop)
	{
		vox = m_pVoxel->GetVoxRvalue(arrayCoordF);
		*theR = vox;
		theR++;
		
		vox = m_pVoxel->GetVoxGvalue(arrayCoordF);
		*theG = vox;
		theG++;
		
		vox = m_pVoxel->GetVoxBvalue(arrayCoordF);
		*theB = vox;
		theB++;

		arrayCoordF += arrayCoordIncF;
	}
}

	
	