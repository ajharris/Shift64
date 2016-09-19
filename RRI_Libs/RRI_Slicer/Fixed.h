#ifndef FIXED_H_INCLUDED
#define FIXED_H_INCLUDED


#ifdef _MSC_VER
typedef __int64 Fixed32;
typedef unsigned __int64 UFixed32;
#endif
typedef long Fixed;  
typedef unsigned long UFixed;
const Fixed FPone  = 0x10000;
const float Fone   = 65536.0f;
const Fixed FPhalf = 0x8000;
typedef short Fixed8;  
typedef unsigned short UFixed8;
struct Vector3F;
struct Vector3;
class fxd
{
public:									            
	static Vector3F FtoFP(Vector3 v);
	static Vector3F FPint(Vector3 v);
	static Vector3F FPint(Vector3F vF);
	static long   FPint( Fixed n);				
	static Fixed  FPfrac( Fixed n);			
	static long   FPround( Fixed n);		
	static Fixed  ItoFP( long i);			
	static Fixed  FtoFP( float f);			
	static float  FPtoF( Fixed f);            
	static Fixed  FPmult( Fixed a, Fixed b);  
	static Fixed  FPdiv( Fixed a, Fixed b);   
	static long   FPint8( Fixed8 n);
};
struct Vector3F
{
	Fixed x,y,z;
	Vector3F(Fixed X, Fixed Y, Fixed Z)
	{
		x = X; 
		y = Y;
		z = Z;
	}
	Vector3F(short X, short Y, short Z)
	{
		x = X; 
		y = Y;
		z = Z;
	}
	Vector3F(){}
};
inline void operator+=(Vector3F &vec1, const Vector3F &vec2)
{
	vec1.x += vec2.x;
	vec1.y += vec2.y;
	vec1.z += vec2.z;
}
inline void operator-=(Vector3F &vec1, const Vector3F &vec2)
{
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
	vec1.z -= vec2.z;
}
inline Vector3F operator +(const Vector3F &op1, const Vector3F &op2)
{
	return(Vector3F(op1.x+op2.x, op1.y+op2.y, op1.z+op2.z));
}
inline Vector3F operator -(const Vector3F &op1, const Vector3F &op2)
{
	return(Vector3F(op1.x-op2.x, op1.y-op2.y, op1.z-op2.z));
}
inline Vector3F operator*(const Vector3F &V, Fixed s)
{
	return Vector3F( fxd::FPmult(V.x , s), fxd::FPmult(V.y , s), fxd::FPmult(V.z , s)  );
}
inline void operator*=(Vector3F &vec, Fixed s)
{
	vec = vec * s;
}
inline Vector3F operator*(Fixed s, const Vector3F &V)
{
	return (V * s);
}
inline Vector3F operator/(const Vector3F &V, Fixed s)
{
	return Vector3F( fxd::FPdiv(V.x , s),  fxd::FPdiv(V.y , s),  fxd::FPdiv(V.z , s)  );
}
inline void operator/=(Vector3F &vec, Fixed s)
{
	vec = vec / s;
}
inline Vector3F operator*(const Vector3F &V, float s)
{
	return (fxd::FtoFP(s)) * V;
}
inline void operator*=(Vector3F &vec, float s)
{
	vec = vec * s;
}
inline Vector3F operator*(float s, const Vector3F &V)
{
	return fxd::FtoFP(s) * V;
}
inline Vector3F operator/(const Vector3F &V, float s)
{
	return V / (fxd::FtoFP(s));
}
inline void operator/=(Vector3F &vec, float s)
{
	vec =  vec / (fxd::FtoFP(s));
}

#endif
