//#include "stdafx.h"
#include "VisLib.h"
#include "geometry.h"
#include "float.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long geom::sgn(float num)
{
	if (num == 0)
	{
		return 0;
	}
	else
	{
		return ((num > 0) ? 1 : -1 );
	}

}

float geom::DegToRad(float d)
{
	return ( MIN_DELTA_THETA * d );
}

float geom::RadToDeg(float r)
{
	return (INV_MIN_DELTA_THETA * r);
}

bool geom::inside(Vector3* point, Plane* plane)
{
	return ( (*point) % plane->normal - plane->D < 0.0f);
}

long geom::detside(Vector3* point, const Plane* plane)
{
	float disp = (*point) % plane->normal - plane->D;
	if (disp < -1.5)
	{
		return -1;
	}
	else if (disp > 1.5)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void geom::intersect(Vector3* s, Vector3* p, const Plane* plane, Vector3* i)
{
	Vector3 disp = *p - *s;
	*i = *s - (*s % plane->normal  - plane->D)/ (disp % plane->normal) * disp;
}

void geom::TransformPlane(const viewORTHO& view, Vector3 *plane_normal, float *plane_D, long plane_no)
{
	Vector3 V;	
	V = view.face[plane_no].plane.normal;
	*plane_normal = V % view.View2Model;
	*plane_normal *= (1/~(*plane_normal));
	V *= view.face[plane_no].plane.D;
	V = V * view.View2Model;
	*plane_D = *plane_normal % V;
}

unsigned long geom::GET_EXP(unsigned long a) 
{
	return ( (a >> EXP_POS) & 0xFF);
}

unsigned long geom::SET_EXP(unsigned long a) 
{
	return ( a << EXP_POS );
}

unsigned long geom::GET_EMANT( unsigned long a)
{
	return ((a >> LOOKUP_POS) & LOOKUP_MASK);
}

unsigned long geom::SET_MANTSEED(unsigned long a)
{
	return ( a << SEED_POS);
}

float geom::FastInvSqrt(float x)
{
 	unsigned long a = (( _flint*)(&x))->i;
    float arg = x;
    _flint seed;
    float r;
    seed.i = SET_EXP(((3*EXP_BIAS-1) - GET_EXP(a)) >> 1) | 
		                           SET_MANTSEED(iSqt[GET_EMANT(a)]);
    r = seed.f;
    r = (3 - r * r * arg) * r/2;
    r = (3 - r * r * arg) * r/2;
    return r;
}

void geom::SortVertices(Vector3Vec& intersections, Face* face)
{
	Vector3 centroid(0,0,0);
	Vector3 V,V0,V1;
	int nverts = intersections.size();
	STL::vector<float> theta;
	theta.erase( theta.begin(), theta.end() );
	int j;
	float tmp,t1,t2;
	float epsilon;	
	if (intersections.empty())
	{
		return;
	}
	for (long i=0; i < nverts; i++)
	{
		centroid += intersections[i];
	}
	centroid /= nverts;
	V0 = intersections[0] - centroid;
	V1 = face->plane.normal * V0;
	epsilon = ~V1 * ~V0 * 0.001f;	
	float angle;
	theta.push_back(0.0f);	// by definition
    {
	for (int i=1; i < nverts; i++)
	{
		V = intersections[i] - centroid;
		t1 = V0 % V;
		t2 = t1 / ( ~V0 * ~V);
		if (t2 <= -1.0f || t2 >= 1.0f)
		{
			angle = (t2 < 0.0f) ? PI : 0.0f;
		}
		else
		{
			angle =  float(acos(t2));
		}
		tmp = V % V1;
		if (t1 < 0.0f && fabs(tmp) < epsilon)
		{
			angle = PI;
		}
		else
		{
			if (tmp > 0.)
			{
				if (fabs(angle) < MIN_DELTA_THETA)
				{
					angle = 0.0f;
				}
				else
				{
					angle = 2.0f * PI - angle;
				}
			}
		}

		theta.push_back(angle);
	}
    }
    {
	for (int i=2; i < nverts; i++)
	{
		tmp = theta[i];
		V = intersections[i];
		for (j=i-1; j >= 1 && theta[j] > tmp; j--)
		{
			theta[j+1] = theta[j];
			intersections[j+1] = intersections[j];
		}
		theta[j+1] = tmp;
		intersections[j+1] = V;
	}
    }
	assert(face->vert.empty());
	face->vert.push_back(intersections[0]);
	tmp = theta[0];	
    {
	for (int i=1; i < nverts; i++)
	{
		if (theta[i] - tmp > MIN_DELTA_THETA)
		{
			face->vert.push_back(intersections[i]);
			tmp = theta[i];
		}
	}
    }
}

//----------------------------------------------------------------------
// Find out whether a point is inside a polygon (2D)
//----------------------------------------------------------------------
bool geom::PtInPoly(const Vector3Vec& vertex, const long ptX,  const long ptY)
{
   if (vertex.size() < 2 || ( (vertex.back().x  - ptX) * ( vertex.front().y - ptY) 
			- (vertex.back().y - ptY) * (vertex.front().x - ptX )  )  < 0   )     
   {
	   return false;
   }
   for (Vector3Vec::const_iterator iVec = vertex.begin(); iVec != vertex.end() - 1; iVec++)
   {
	   if (   (    (iVec->x - ptX) * ( (iVec+1)->y - ptY)   
			- (iVec->y - ptY) * ((iVec+1)->x - ptX )  )   <  0  )
	   {
			return false;
	   }
   }
    return true;
}

//----------------------------------------------------------------------
// Find out whether a point is inside a polygon (3D)
//----------------------------------------------------------------------
bool geom::PtInPoly3(const Vector3 * vertex, long numVerts,  long ptX,  long ptY, long ptZ)
{
	if (( (vertex[numVerts - 1].x  - ptX) * ( vertex[0].y - ptY) 
			- (vertex[numVerts-1].y - ptY) * (vertex[0].x - ptX )  )  < 0  ||
		( (vertex[numVerts - 1].x  - ptX) * ( vertex[0].z - ptZ) 
			- (vertex[numVerts-1].z - ptZ) * (vertex[0].x - ptX )  )  < 0)     
	{
	   return false;
   }
   for (long i = 0; i < (numVerts - 1); i++)
   {
	   if (   (    (vertex[i].x - ptX) * ( vertex[i+1].y - ptY)   
			- (vertex[i].y - ptY) * (vertex[i+1].x - ptX )  )   <  0  ||
			(    (vertex[i].x - ptX) * ( vertex[i+1].z - ptZ)   
			- (vertex[i].z - ptZ) * (vertex[i+1].x - ptX )  )   <  0)
	   {
			return false;
	   }
}
	return true;
}

//----------------------------------------------------------------------
// Rotate a given point about X axis when the angle of rotation and the origin is given
//----------------------------------------------------------------------
Vector3 geom::TransformPointAngle(double sintheta, double costheta, Vector3 point, Vector3 Origin)
{
	Vector3 pt;

	float z1 = point.z - Origin.z;
	float y1 = point.y - Origin.y;
	pt.x = point.x;

	pt.y = Origin.y + z1 * sintheta + y1 * costheta;
	pt.z = Origin.z + z1 * costheta - y1 * sintheta;
	return(pt);
}

//----------------------------------------------------------------------
// Reposition  a given point when the scale and the origin is given
//----------------------------------------------------------------------
Vector3 geom::TransformPointScale(Vector3 scale, Vector3 point1, Vector3 Origin)
{
	Vector3 pt;

	pt = (point1 - Origin);	
	pt.x *= scale.x;
	pt.y *= scale.y;
	pt.z *= scale.z;
	pt += Origin;
	return(pt);
}

//----------------------------------------------------------------------
// Perpendicular distance from point to a line (v1,v2)
//----------------------------------------------------------------------
float geom::distance(const Vector3 point, const Vector3 v1, const Vector3 v2)
{
	float fvalue = (v2-v1)%(v2-v1);
	if (fabs(fvalue) < 0.00001)
		fvalue = 0.00001;
	return float(sqrt(fabs((point-v1)%(point-v1) - ((v2-v1)%(point-v1)/fvalue))));
}

//----------------------------------------------------------------------
// Smallest perpendicular distance from point to an edge in poly 
//----------------------------------------------------------------------
float geom::distance(const Vector3 point, const Vector3Vec& poly)
{
	if (poly.empty())
		return FLT_MAX;
	else if (poly.size() == 1)
		return ~(point-poly[0]);

	float min_dist = distance(point, *poly.begin(), *poly.end());
	for(Vector3Vec::const_iterator i = poly.begin()+1; i != poly.end(); i++)
	{
		float dist = distance(point, *(i-1), *i);
		if (dist < min_dist)
			min_dist = dist;
	}

	return min_dist;
}

