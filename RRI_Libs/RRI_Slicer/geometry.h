#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED



#include "VisLib.h"
#include <math.h>


struct Vector3;
struct Matrix4x4;
struct Quaternion;

const float  PI = 3.14159265358979f;
const float MIN_DELTA_THETA  = PI/180.0f;	
const float INV_MIN_DELTA_THETA = 180.0f / PI;

inline void operator+=(Vector3 &vec1, const Vector3 &vec2)
{
	vec1.x += vec2.x;
	vec1.y += vec2.y;
	vec1.z += vec2.z;
}
inline void operator-=(Vector3 &vec1, const Vector3 &vec2)
{

	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
	vec1.z -= vec2.z;
}
inline Vector3 operator +(const Vector3 &op1, const Vector3 &op2)
{
	return(Vector3(op1.x+op2.x, op1.y+op2.y, op1.z+op2.z));
}
inline Vector3 operator -(const Vector3 &op1, const Vector3 &op2)
{
	return(Vector3(op1.x-op2.x, op1.y-op2.y, op1.z-op2.z));
}
inline Vector3 operator*(const Vector3 &V, float s)
{
	return Vector3(V.x * s, V.y * s, V.z * s);
}
inline void operator*=(Vector3 &vec, float s)
{
	vec = vec * s;
}
inline Vector3 operator*(float s, const Vector3 &V)
{
	return (V * s);
}
inline Vector3 operator/(const Vector3 &V, float s)
{
	return Vector3(V.x / s, V.y / s, V.z / s);
}
inline void operator/=(Vector3 &vec, float s)
{
	vec = vec / s;
}
inline float operator%(const Vector3 &vec1, const Vector3 &vec2)
{
	return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}
inline Vector3 operator*(const Vector3 &vec1, const Vector3 &vec2)
{

	return Vector3(vec1.y * vec2.z - vec1.z * vec2.y,
				   vec1.z * vec2.x - vec1.x * vec2.z,
				   vec1.x * vec2.y - vec1.y * vec2.x);
}
inline float operator~(const Vector3 &vec)
{
	return sqrtf( vec % vec);
}
const Matrix4x4 identity4x4(	  1., 0., 0., 0.,
								  0., 1., 0., 0.,
								  0., 0., 1., 0.,
								  0., 0., 0., 1.);
inline Vector3 operator*(const Vector3 &V, const Matrix4x4 &M)
{


	float scale = V.x * M.d + V.y * M.h + V.z * M.l + M.p;

	return Vector3( (V.x * M.a + V.y * M.e + V.z * M.i + M.m) / scale,
					(V.x * M.b + V.y * M.f + V.z * M.j + M.n) / scale,
					 (V.x * M.c + V.y * M.g + V.z * M.k + M.o) / scale);

}
inline Vector3 operator%(const Vector3 &V, const Matrix4x4 &M)
{
	return Vector3( V.x * M.a + V.y * M.e + V.z * M.i,
					V.x * M.b + V.y * M.f + V.z * M.j,
					V.x * M.c + V.y * M.g + V.z * M.k);
}
inline Matrix4x4 operator*(const Matrix4x4 M1, const Matrix4x4 &M2)
{

	return Matrix4x4(

				M1.a * M2.a + M1.b * M2.e + M1.c * M2.i + M1.d * M2.m,
				M1.a * M2.b + M1.b * M2.f + M1.c * M2.j + M1.d * M2.n,
				M1.a * M2.c + M1.b * M2.g + M1.c * M2.k + M1.d * M2.o,
				M1.a * M2.d + M1.b * M2.h + M1.c * M2.l + M1.d * M2.p,
				M1.e * M2.a + M1.f * M2.e + M1.g * M2.i + M1.h * M2.m,
				M1.e * M2.b + M1.f * M2.f + M1.g * M2.j + M1.h * M2.n,
				M1.e * M2.c + M1.f * M2.g + M1.g * M2.k + M1.h * M2.o,
				M1.e * M2.d + M1.f * M2.h + M1.g * M2.l + M1.h * M2.p,
				M1.i * M2.a + M1.j * M2.e + M1.k * M2.i + M1.l * M2.m,
				M1.i * M2.b + M1.j * M2.f + M1.k * M2.j + M1.l * M2.n,
				M1.i * M2.c + M1.j * M2.g + M1.k * M2.k + M1.l * M2.o,
				M1.i * M2.d + M1.j * M2.h + M1.k * M2.l + M1.l * M2.p,
				M1.m * M2.a + M1.n * M2.e + M1.o * M2.i + M1.p * M2.m,
				M1.m * M2.b + M1.n * M2.f + M1.o * M2.j + M1.p * M2.n,
				M1.m * M2.c + M1.n * M2.g + M1.o * M2.k + M1.p * M2.o,
				M1.m * M2.d + M1.n * M2.h + M1.o * M2.l + M1.p * M2.p);
}
inline Matrix4x4 operator~(const Matrix4x4 M)
{
	return Matrix4x4(
					M.a, M.e, M.i,  M.m,
					M.b, M.f, M.j,  M.n,
					M.c, M.g, M.k,  M.o,
					M.d, M.h, M.l,  M.p);
}
// :DANGEROUS ASSUMPTION: elements d, h and l are zero, element p is one
// since (AB)inverse = (Binv)(Ainv), we could decompose H into components R & T
// consisting of Identity4x4 plus the 3x3 rotation,
// and Identity4x4 with the xyz translation elements.
// Since the inverse of a pure rotation matrix is its transpose, and
// the inverse of a pure translation is its negative, YOU MIGHT THINK
// we could generate both Binv and Ainv easily...
// Since the rotation matrix is "hit" first in the multiplication p*H,
// we decompose H into the product RT (not TR) and its inverse will be TinvRinv.

// HOWEVER: R is no longer a pure rotation matrix! Coming from the tracker
//          it was, but it was since multiplied by screen offset/flip matrices.
// THEREFORE we do a straight determinant-based inversion...

inline Matrix4x4 InvertHomogeneous(const Matrix4x4 M)
{
    Matrix4x4 MOut;
	float D =								/* Determinant */
				-(M.c * M.f * M.i) + M.b * M.g * M.i + M.c * M.e * M.j -
				M.a * M.g * M.j - M.b * M.e * M.k + M.a * M.f * M.k;
				
	MOut.a = (-(M.g * M.j) + M.f * M.k) / D;
	MOut.b = (M.c * M.j - M.b * M.k) / D;
	MOut.c = (-(M.c * M.f) + M.b * M.g) / D;
	MOut.d = 0.;
	
	MOut.e = (M.g * M.i - M.e * M.k) / D;
	MOut.f = (-(M.c * M.i) + M.a * M.k) / D;
	MOut.g = (M.c * M.e - M.a * M.g) / D;
	MOut.h = 0.;
	
	MOut.i = (-(M.f * M.i) + M.e * M.j) / D;
	MOut.j = (M.b * M.i - M.a * M.j) / D;
	MOut.k = (-(M.b * M.e) + M.a * M.f) / D;
	MOut.l = 0.;
	
	MOut.m = (M.o * M.f * M.i - M.n * M.g * M.i - M.o * M.e * M.j +
				M.m * M.g * M.j + M.n * M.e * M.k - M.m * M.f * M.k) / D;
	MOut.n = (-(M.o * M.b * M.i) + M.n * M.c * M.i + M.o * M.a * M.j -
				M.m * M.c * M.j - M.n * M.a * M.k + M.m * M.b * M.k) / D;
	MOut.o = (M.o * M.b * M.e - M.n * M.c * M.e - M.o * M.a * M.f +
				M.m * M.c * M.f + M.n * M.a * M.g - M.m * M.b * M.g) / D;
	MOut.p = 1.;

    return MOut;
}

struct Quaternion
{
	float s;
	Vector3 V;
	Quaternion(float S, float X, float Y, float Z)
	{
		s = S;
		V.x = X;
		V.y = Y; 
		V.z = Z;
	}
	Quaternion(float S, Vector3 Vec)
	{
		s = S;
		V = Vec;
	}
	Quaternion(float rx, float ry, float rz)
	{
		float psi   = MIN_DELTA_THETA * rx /2.0f; 
		float theta = MIN_DELTA_THETA * ry /2.0f;
		float phi   = MIN_DELTA_THETA * rz /2.0f;
	
		s   = cosf(psi) * cosf(theta) * cosf(phi) + sinf(psi) * sinf(theta) * sinf(phi);
		V.x = sinf(psi) * cosf(theta) * cosf(phi) - cosf(psi) * sinf(theta) * sinf(phi);
		V.y = cosf(psi) * sinf(theta) * cosf(phi) + sinf(psi) * cosf(theta) * sinf(phi);
		V.z = cosf(psi) * cosf(theta) * sinf(phi) - sinf(psi) * sinf(theta) * cosf(phi);
	}
	Quaternion(){}

};
const Quaternion qid(1,0,0,0);
inline Quaternion operator*(const Quaternion &q1, const Quaternion &q2)
{
	return Quaternion( (q1.s * q2.s - q1.V % q2.V),  (q2.V * q1.s + q1.V * q2.s + q1.V * q2.V) );
}
inline void operator*=(Quaternion &q1, const Quaternion &q2)
{
	q1 = q1 * q2;
}
class SLICER_EXPORT geom
{
public:
	static long sgn(float num);
	static float FastInvSqrt(float x);
	static float DegToRad(float d);
	static float RadToDeg(float r);
	static unsigned long GET_EXP(unsigned long a) ;
	static unsigned long SET_EXP(unsigned long a) ;
	static unsigned long GET_EMANT( unsigned long a);
	static unsigned long SET_MANTSEED(unsigned long a);
	static bool inside(Vector3* point, Plane* plane);
	static long detside(Vector3* point, const Plane* plane);
	static void intersect(Vector3* s, Vector3* p, const Plane* plane, Vector3* i);
	static void SortVertices(Vector3Vec& intersections, Face* face);
	static void TransformPlane(const viewORTHO& view, Vector3 *plane_normal, float *plane_D, long plane_no);
	static bool PtInPoly3(const Vector3 * vertex, long numVerts,  long ptX,  long ptY, long ptZ);
	static bool PtInPoly(const Vector3Vec& vertex, const long ptX,  const long ptY);
	static Vector3 TransformPointScale(Vector3 scale, Vector3 point, Vector3 Origin);
	static Vector3 TransformPointAngle(double sintheta, double costheta, Vector3 point, Vector3 Origin);

	static float distance(Vector3 point, Vector3 v1, Vector3 v2);
	static float distance(Vector3 point, const Vector3Vec& poly);

};
inline Vector3 operator!(const Vector3 &vec)
{
	return (vec * geom::FastInvSqrt( vec % vec));
}
const long LOOKUP_BITS =   6;
const long EXP_POS    =   23;
const long EXP_BIAS  =   127;
const long LOOKUP_POS  = (EXP_POS-LOOKUP_BITS);
const long SEED_POS   =  (EXP_POS-8);
const long TABLE_SIZE  = (2 << LOOKUP_BITS);
const long LOOKUP_MASK = (TABLE_SIZE - 1);
const unsigned char iSqt[] = {
	106, 103, 100, 98, 95, 92, 90, 87, 85, 83, 80, 78, 76, 74, 72, 70, 68, 
	66, 64, 62, 60, 58, 56, 54, 52, 51, 49, 47, 46, 44, 42, 41, 39, 
	38, 36, 35, 33, 32, 31, 29, 28, 26, 25, 24, 22, 21, 20, 19, 17, 
	16, 15, 14, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 255, 
	252, 248, 244, 240, 237, 233, 230, 226, 223, 220, 217, 214, 211, 208, 205, 202, 
	199, 196, 193, 191, 188, 185, 183, 180, 178, 176, 173, 171, 168, 166, 164, 162, 
	160, 158, 155, 153, 151, 149, 147, 145, 143, 142, 140, 138, 136, 134, 133, 131, 
	129, 127, 126, 124, 122, 121, 119, 118, 116, 115, 113, 112, 110, 109, 107
};
union _flint {
	unsigned long    i;
	float            f;
};




#endif