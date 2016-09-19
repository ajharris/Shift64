// TextureColour24.h: interface for the TextureColour24 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURECOLOUR24_H__755D53DD_5A18_434E_843A_C02A2BDB9E3F__INCLUDED_)
#define AFX_TEXTURECOLOUR24_H__755D53DD_5A18_434E_843A_C02A2BDB9E3F__INCLUDED_

#include "texture.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class TextureColour24 : public Texture  
{
public:

	TextureColour24(TvoxelInit voxelInit, TtexInit texInit);
	virtual ~TextureColour24();

	virtual void Render(viewORTHO view, long facetotex = -1);
	virtual void ResizeImBuff(long x, long y);
	virtual FilledBuff GetImBuff();


protected:

	tVoxel *redPlane;
	tVoxel *greenPlane;
	tVoxel *bluePlane;


	void fxedToVox(tVoxel *imBuff, Rect clip);
	void fill(Face* face, tVoxel *buff, Rect polyClip);
	void draw_lines(long scan, long x_int_count, long index, tVoxel *buff, Rect polyClip);
	void sort_on_ymin(long n, long* side_count, long* bottomscan, tVoxel *buff, Rect polyClip);
	void paint_line(long y, long x1, long x2, float z1, float z2, float delta_z, Vector3 tex1, Vector3 tex2, Vector3 delta_tex, tVoxel *buff, Rect polyClip);
	void drawline(long y, long xl, long xr, float z, float dz, Vector3 vCoordStart, Vector3 vCoordIncrement, tVoxel *buff, Rect polyClip);
};


#endif // !defined(AFX_TEXTURECOLOUR24_H__755D53DD_5A18_434E_843A_C02A2BDB9E3F__INCLUDED_)
