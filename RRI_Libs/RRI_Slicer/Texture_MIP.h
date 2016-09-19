// TextureMIP.h: interface for the TextureMIP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMIP_H__2966B35B_6843_4E02_A20F_4D8217EEE06A__INCLUDED_)
#define AFX_TEXTUREMIP_H__2966B35B_6843_4E02_A20F_4D8217EEE06A__INCLUDED_


#include "texture.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TextureMIP : public Texture  
{

public:
	TextureMIP(TvoxelInit voxelInit, TtexInit texInit);
	virtual ~TextureMIP();

	virtual void ResizeImBuff(long x, long y);
	virtual void Render(viewORTHO view, long facetotex);
	virtual void SetParameters(long min, long max);

	
protected :
	Fixed m_RayU, m_RayV, m_RayW;	
	Fixed *m_pZbuff;
	long  m_renderMode;
	long  m_min, m_max;
	float m_length;


	void fill(Face* face, tVoxel *buff, Rect polyClip);
	void draw_lines(long scan, long x_int_count, long index, tVoxel *buff, Rect polyClip);
	void sort_on_ymin(long n, long* side_count, long* bottomscan, tVoxel *buff, Rect polyClip);
	void paint_line(long y, long x1, long x2, float z1, float z2, float delta_z, Vector3 tex1, Vector3 tex2, Vector3 delta_tex, tVoxel *buff, Rect polyClip);
	void drawline(long y, long xl, long xr, float z, float dz, Vector3 vCoordStart, Vector3 vCoordIncrement, tVoxel *buff, Rect polyClip);
};

#endif // !defined(AFX_TEXTUREMIP_H__2966B35B_6843_4E02_A20F_4D8217EEE06A__INCLUDED_)
