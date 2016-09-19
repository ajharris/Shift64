//#include "stdafx.h"
#include "texture.h"
#include "PaletteDefinitions.h"

#include <omp.h>
//----------------------------------------------------------------------
// Texture constructor
//
// Create texture object to texture map the cube model faces
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
Texture::Texture(TvoxelInit voxelInit, TtexInit texInit) :
		m_pImage(0)

{

	switch (texInit.reconType)
	{
		case SG_LinearRaw:
			m_pVoxel = new Voxel(voxelInit);
			break;	

		case SG_Fan:
			m_pVoxel = new VoxelFAN(voxelInit);
			break;

		case SG_AxialNoOffsets:
			m_pVoxel = new VoxelAXIAL(voxelInit);
			break;

		case SG_Linear16:
			m_pVoxel = new Voxel16(voxelInit);
			break;

        case SG_Linear24:
            m_pVoxel = new Voxel24(voxelInit);
            break;

//        case SG_Fan24:
//            m_pVoxel = new VoxelFAN24(voxelInit);
//            break;

        case SG_Hybrid:
			m_pVoxel = new VoxelHybrid(voxelInit);
			break;

//		case SG_Hybrid24:
//			m_pVoxel = new VoxelHybrid24(voxelInit);
//			break;

		default:
			m_pVoxel = new Voxel(voxelInit);
			break;
	}


	m_model2Array = m_pVoxel->GetModel2Array();

	ResizeImBuff(texInit.clientX, texInit.clientY);

	double step = (double)(VIS_GREYS - 1) / VIS_MAX_GREY;
	double val = VIS_MIN_GREY;
	memset(m_greyMap, 0, sizeof(m_greyMap));
	
	for (long i = 0; i < ACQ_MAX_COLOURS; i++)
	{
		m_greyMap[i] = (tVoxel)(val);
		val += step;
	}


}


//----------------------------------------------------------------------
// Texture destrucror
//
//----------------------------------------------------------------------
Texture::~Texture()
{
	delete m_pVoxel;
	delete[] m_pImage;
}





//----------------------------------------------------------------------
// GetImBuff
//
// Return the image buffer
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
FilledBuff Texture::GetImBuff()
{
	return FilledBuff(m_pImage, m_polyClip.right, m_polyClip.bottom);
}


//----------------------------------------------------------------------
// GetVoxValue
//
// Return the volume voxel value
//
// Input:
// Vector3 pt contains the coordinate of the volume voxel 
// in the reconstructed volume
//
// Output:
// voxel value
//----------------------------------------------------------------------
tVoxel Texture::GetVoxValue(Vector3 pt)
{
	if (!m_pVoxel->CheckOutOfBounds(pt.x, pt.y, pt.z))
	{
		return m_pVoxel->GetVoxValue(pt.x, pt.y, pt.z);
	}
	else
	{
		return 0;
	}
}

tVoxel Texture::GetVoxRValue(Vector3 pt)
{
	if (!m_pVoxel->CheckOutOfBounds(pt.x, pt.y, pt.z))
	{
		return m_pVoxel->GetVoxRvalue(pt.x, pt.y, pt.z);
	}
	else
	{
		return 0;
	}
}


tVoxel Texture::GetVoxGValue(Vector3 pt)
{
	if (!m_pVoxel->CheckOutOfBounds(pt.x, pt.y, pt.z))
	{
		return m_pVoxel->GetVoxGvalue(pt.x, pt.y, pt.z);
	}
	else
	{
		return 0;
	}
}


tVoxel Texture::GetVoxBValue(Vector3 pt)
{
	if (!m_pVoxel->CheckOutOfBounds(pt.x, pt.y, pt.z))
	{
		return m_pVoxel->GetVoxBvalue(pt.x, pt.y, pt.z);
	}
	else
	{
		return 0;
	}
}


tVoxel Texture::GetVoxValue(long x, long y, long z, long n)
{
	if (!m_pVoxel->CheckOutOfBounds(x, y, z))
	{
		return m_pVoxel->GetVoxValue(x, y, z, n);
	}
	else
	{
		return 0;
	}
}

void Texture::GetVoxValue(long x, long y, long z, tVoxel& lower, tVoxel& upper)
{
	if (!m_pVoxel->CheckOutOfBounds(x, y, z))
	{
		return m_pVoxel->GetVoxValue(x, y, z, lower, upper);
	}
	else
	{
		upper = 0;
		lower = 0;
	}
}

void Texture::SetVoxValue(long x, long y, long z, long n, unsigned char value)
{
	if (!m_pVoxel->CheckOutOfBounds(x, y, z))
	{
		 m_pVoxel->SetVoxValue(x, y, z, n, value);
	}
	else
	{
		return;
	}
}

//get index into voxel buffer for current point
long Texture::GetVoxelIndex(Vector3 point)
{
	long index = m_pVoxel->GetVoxIndex(point);

	return index;

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
void Texture::Render(viewORTHO view, long face)
{

     m_view2Array = view.View2Model * m_model2Array;
 
     memset(m_pImage, 0, m_polyClip.right * m_polyClip.bottom);
 
     if (face == -1)//no face defined
     {
          
          for (long i = 0; i < view.face.size(); i++)
          {
               Face f = view.face[i];
   
               if (f.plane.normal.z < 0)
               {
                    fill(&f, m_pImage, m_polyClip);
               }
          }
     }
     else//here, face is defined
     {
          Face f = view.face[face];
   
          if (f.plane.normal.z < 0)
          {
                fill(&f, m_pImage, m_polyClip);
          }
     }

}



//----------------------------------------------------------------------
// GetVoxPos
//
// Convert from virtual model coordinates to virtual array coordinates
// 
//
// Input:
// Vector3 Vector3 point the model coordinates
//
// Output:
// Vector3 returns Vector3 point in virtual array coordinates
//----------------------------------------------------------------------
Vector3 Texture::GetVoxPos(Vector3 point)
{
        return (Vector3)(point * m_model2Array);
}



//----------------------------------------------------------------------
// ViewPtToArrayPt
//
// Convert a point from screen view coordinates to not reconstructed
// volume coordinates 
//
// Input:
// Vector3 structure vw contains the input point in the screen coordinates
//
// Output:
// Vector3 structure ar contains the output point in the raw data coordinates
//----------------------------------------------------------------------
void Texture::ViewPtToArrayPt(Vector3 vw, Vector3 *ar)
{
	*ar = vw * m_view2Array;
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
void Texture::ResizeImBuff(long clientX, long clientY)
{
	m_polyClip = Rect(0, 0, clientY, clientX);

	if (m_pImage)
	{
		delete[] m_pImage;
	}

	m_pImage = new tVoxel[m_polyClip.right * m_polyClip.bottom];
}


//----------------------------------------------------------------------
// GetSizeXmm
//
// Return the width of the reconstructed volume in mm
//
// Input:
// none
//
// Output:
// width of the reconstructed volume in mm
//----------------------------------------------------------------------
float Texture::GetSizeXmm()
{
	return m_pVoxel->GetSizeXmm();
}


//----------------------------------------------------------------------
// GetSizeYmm
//
// Return the height of the reconstructed volume in mm
//
// Input:
// none
//
// Output:
// height of the reconstructed volume in mm
//----------------------------------------------------------------------
float Texture::GetSizeYmm()
{
	return m_pVoxel->GetSizeYmm();
}


//----------------------------------------------------------------------
// GetSizeZmm
//
// Return the z dimension size of the reconstructed volume in mm
//
// Input:
// none
//
// Output:
// z dimension size of the reconstructed volume in mm
//----------------------------------------------------------------------
float Texture::GetSizeZmm()
{
	return m_pVoxel->GetSizeZmm();
}


//----------------------------------------------------------------------
// GetDimX
//
// Return the width of the reconstructed volume in pixels
//
// Input:
// none
//
// Output:
// width of the reconstructed volume in pixels
//----------------------------------------------------------------------
float Texture::GetDimX()
{
	return m_pVoxel->GetDimX();
}


//----------------------------------------------------------------------
// GetDimY
//
// Return the height of the reconstructed volume in pixels
//
// Input:
// none
//
// Output:
// height of the reconstructed volume in pixels
//----------------------------------------------------------------------
float Texture::GetDimY()
{
	return m_pVoxel->GetDimY();
}


//----------------------------------------------------------------------
// GetDimZ
//
// Return the number of frames of the reconstructed volume
//
// Input:
// none
//
// Output:
// number of frames of the reconstructed volume
//----------------------------------------------------------------------
float Texture::GetDimZ()
{
	return m_pVoxel->GetDimZ();
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
void Texture::setLeftRight(long left, long right)
{
	m_pVoxel->SetLeftRight(left, right);
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
void Texture::fill(Face* face, tVoxel *buff, Rect polyClip)
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
void Texture::drawline(long y, long xl, long xr, float z, float dz, Vector3 vCoordStart, Vector3 vCoordIncrement, tVoxel *buff, Rect polyClip)
{

//NOTE: this needs to be done because converting to fixed point has a rounding error. We need to round up the points first.
    vCoordStart.x += 0.5f;
    vCoordStart.y += 0.5f;
    vCoordStart.z += 0.5f;

	tVoxel* PixBaseAddr = buff +  y*polyClip.right + xl;
	Vector3F arrayCoordIncF = fxd::FtoFP(vCoordIncrement);
	Vector3F arrayCoordF = fxd::FtoFP(vCoordStart) + Vector3F(FPhalf, FPhalf, FPhalf);
	tVoxel vox;


	for (long loop = xr - xl; loop > 0; --loop)
	{		
		Vector3F vF = fxd::FPint(arrayCoordF); 	
        {

            vox = m_pVoxel->GetVoxValue(arrayCoordF);
        }
		
		*PixBaseAddr = m_greyMap[vox];
		PixBaseAddr++;
		arrayCoordF += arrayCoordIncF;
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
void Texture::draw_lines(long scan, long x_int_count, long index, tVoxel *buff, Rect polyClip)
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
void Texture::paint_line(long y, long x1, long x2, float z1, float z2, float delta_z,Vector3 tex1, Vector3 tex2, Vector3 delta_tex, tVoxel *buff, Rect polyClip)
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
		dtx = (float)(la) * delta_tex;
		tex = tex + dtx;
	}

	drawline(y,xmin,xmax,z,delta_z,tex,delta_tex, buff, polyClip);
}

//----------------------------------------------------------------------
// next_Y
//
// Find next y coordinate
//
// Input:
// npts number of vertices in the face
// k is the next vertex
//
// Output:
// y coordinate in the view coordinates
//----------------------------------------------------------------------
long Texture::next_Y(long npts, long k)
{
	long next = k;

	while (m_vertices[next].y == m_vertices[k].y)
	{
		++next;
	
		if (next == npts)
		{
			next = 0;
		}
	}

	return m_vertices[next].y;
}

//----------------------------------------------------------------------
// put_in_sides_list
//
// Generate the list 
//
// Input:
// entry - number of lines to texture map
// p1 - number of vertices in a face
// p2 - next vertex in a face
// next_y - y coordinate of the next vertex
//
// Output:
// m_sides contains sorted on x lines y coordinates
//----------------------------------------------------------------------
void Texture::put_in_sides_list(long entry, long p1, long p2, long next_y)
{
	long y1,y2,miny;
	float z2,c;
	Vector3 tex2,vec;
	Fixed x2, x_inc;
	y1 = m_vertices[p1].y;
	y2 = m_vertices[p2].y;
	x2 = fxd::ItoFP(long(m_vertices[p2].scrn.x));
	x_inc = fxd::FtoFP((m_vertices[p2].scrn.x - m_vertices[p1].scrn.x) / (m_vertices[p2].scrn.y - m_vertices[p1].scrn.y));
	z2 = m_vertices[p2].scrn.z;
	tex2 = m_vertices[p2].tex;

	if (y2 > y1 && y2 < next_y)
	{
		y2--;
		x2 -= x_inc;
	}
	else
	{
		if (y2 < y1 && y2 > next_y)
		{
			y2++;
			c = float(-fxd::FPround(x2));
			x2 += x_inc;
			c += fxd::FPround(x2);
			z2 += c * m_delta_Zx + m_delta_Zy;
			vec = c * m_delta_Tx;
			tex2 = tex2 + vec + m_delta_Ty;
		}
	}

	miny = (y1 < y2) ? y1 : y2;

	while (entry > 0 && miny < m_sides[entry-1].ymin)
	{
		m_sides[entry] = m_sides[entry-1];
		entry--;
	}

	m_sides[entry].ymin    = (short)miny;
	m_sides[entry].delta_y = abs(y2 - y1) + 1;
	m_sides[entry].x_inc   = x_inc;

	if (y1 < y2)
	{
		m_sides[entry].x_int = fxd::ItoFP(m_vertices[p1].x);
		m_sides[entry].z     = m_vertices[p1].scrn.z;
		m_sides[entry].tex   = m_vertices[p1].tex;
	}
	else
	{
		m_sides[entry].x_int  = x2;
		m_sides[entry].z		= z2;
		m_sides[entry].tex    = tex2;
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
void Texture::sort_on_ymin(long n, long* side_count, long* bottomscan, tVoxel *buff, Rect polyClip)
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
// update_first_and_last
//
// Calculate the start and end y coordinates of lines to texture map 
//
// Input:
// count - number of lines to texture map
// scan - y coordinate of the line 
//
// Output:
// firstScan - y index of the first line
// lastScan - y index of the last line
//----------------------------------------------------------------------
void Texture::update_first_and_last(long count, long scan, long* firstScan, long* lastScan)
{
	while ((*lastScan < (count-1)) && (m_sides[*lastScan + 1].ymin <= scan))
	{
		(*lastScan)++;
	}
	while (m_sides[*firstScan].delta_y == 0)
	{
		(*firstScan)++;
	}
}

//----------------------------------------------------------------------
// sort_on_x
//
// Sort lines to texture map on x 
//
// Input:
// scan is an index in the list of lines to texture map
// firstScan is the first in the list of lines to texture map
//
// Output:
// m_sides contains sorted on x lines y coordinates
//----------------------------------------------------------------------
void Texture::sort_on_x(long scan, long firstScan)
{
	EL_entry tmp;

	while ((scan > firstScan)  && (m_sides[scan].x_int < m_sides[scan-1].x_int))
	{
		tmp = m_sides[scan-1];
		m_sides[scan-1] = m_sides[scan];
		m_sides[scan] = tmp;
		scan--;
	}
}

//----------------------------------------------------------------------
// process_x_intersections
//
// Calculate the x intersection for the lines
//
// Input:
// scan - y coordinate of the line
// firstScan - y index of the first line
// lastScan - y index of the last line
//
// Output:
// x_int_count - number of x intersections
//----------------------------------------------------------------------
void Texture::process_x_intersections(long scan, long firstScan, long lastScan, long* x_int_count)
{
	long k;
	*x_int_count = 0;

	for (k = firstScan; k <= lastScan; k++)
	{
		if (m_sides[k].delta_y > 0)
		{
			(*x_int_count)++;
			sort_on_x(k, firstScan);
		}
	}
}

//----------------------------------------------------------------------
// rnd
//
// Calculate the ceiling of a float value and cast it to a short
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
short Texture::rnd(float x)
{
	if (x > 0)
	{
		return (short)(x + 0.5f);
	}
	else
	{
		return (short)(x - 0.5f);
	}
}
