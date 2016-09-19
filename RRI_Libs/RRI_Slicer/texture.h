#if !defined(TEXTURE_H_INCLUDED)
#define TEXTURE_H_INCLUDED


#include "geometry.h"
#include "fixed.h"
#include "Voxel.h"
#include "VoxelFAN.h"
#include "VoxelAXIAL.h"
#include "Voxel16.h"
#include "Voxel24.h"
//#include "VoxelFan24.h"
#include "VoxelHybrid.h"
//#include "VoxelHybrid24.h"



const long MAX_SIDES = 20;	

struct VL_entry
{
	short x,y;		
	Vector3 scrn;		
	Vector3 tex;		
};
	
struct EL_entry
{
	Fixed	x_int;	
	Fixed	x_inc;	
	short	ymin;		
	short	delta_y;
	float	z;			
	Vector3 tex;		
};
	



class Texture
{

public:

	Texture(TvoxelInit voxelInit, TtexInit texInit);
	virtual ~Texture();

	virtual void Render(viewORTHO view, long facetotex = -1);
	virtual void ResizeImBuff(long x, long y);
	virtual void SetParameters(long min, long max){}

	virtual FilledBuff GetImBuff();

	float GetSizeXmm();
	float GetSizeYmm();
	float GetSizeZmm();

	float GetDimX();
	float GetDimY();
	float GetDimZ();

	void setLeftRight(long left, long right);

	void ViewPtToArrayPt(Vector3 vw, Vector3 *ar);
	tVoxel GetVoxValue(Vector3 pt);
	tVoxel GetVoxValue(long x, long y, long z, long n);
	void GetVoxValue(long x, long y, long z, tVoxel& lower, tVoxel& upper);
	void SetVoxValue(long x, long y, long z, long n, unsigned char value);
    Vector3 GetVoxPos(Vector3 point);

	long GetVoxelIndex(Vector3 point);

    tVoxel GetVoxRValue(Vector3 pt);
    tVoxel GetVoxGValue(Vector3 pt);
    tVoxel GetVoxBValue(Vector3 pt);


protected :

	Voxel *m_pVoxel;
	tVoxel *m_pImage;

	tVoxel m_greyMap[256];

	Matrix4x4 m_view2Array, m_model2Array;
	
	Rect m_polyClip;					

	VL_entry m_vertices[MAX_SIDES];
	EL_entry m_sides[MAX_SIDES];	

	float m_delta_Zx, m_delta_Zy;	
	Vector3 m_delta_Tx, m_delta_Ty;	


	void drawline(long y, long xl, long xr, float z, float dz, Vector3 vCoordStart, Vector3 vCoordIncrement, tVoxel *buff, Rect polyClip );
	void fill(Face* face, tVoxel *buff, Rect polyClip);
	void draw_lines(long scan, long x_int_count, long index, tVoxel *buff, Rect polyClip);
	void paint_line(long y, long x1, long x2, float z1, float z2, float delta_z, Vector3 tex1, Vector3 tex2, Vector3 delta_tex, tVoxel *buff, Rect polyClip);
	short rnd(float x);void sort_on_ymin(long n, long* side_count, long* bottomscan, tVoxel *buff, Rect polyClip);
	long next_Y(long npts, long k);
	void put_in_sides_list(long entry, long p1, long p2, long next_y);
	void update_first_and_last(long count, long scan, long* first_s, long* last_s);
	void sort_on_x(long entry, long first_s);
	void process_x_intersections(long scan, long first_s, long last_s, long* x_int_count);
};


#endif 