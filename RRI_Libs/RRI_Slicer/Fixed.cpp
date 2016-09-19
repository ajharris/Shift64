//#include "stdafx.h"
#include "VisLib.h"
#include "Fixed.h"

Vector3F fxd::FtoFP(Vector3 v)
{
	return Vector3F(FtoFP(v.x), FtoFP(v.y), FtoFP(v.z) );
}
Vector3F fxd::FPint(Vector3 v)
{
	return Vector3F(FPint(v.x), FPint(v.y), FPint(v.z) );
}
Vector3F fxd::FPint(Vector3F vF)
{
	return Vector3F(FPint(vF.x), FPint(vF.y), FPint(vF.z) );
}
long fxd::FPint(Fixed n)
{
	return (n >> 16);
}
Fixed fxd::FPfrac(Fixed n)
{
	return (n & 0xFFFF);
}
long fxd::FPround(Fixed n)
{
	return FPint( n + FPhalf);
}
Fixed fxd::ItoFP(long i)
{
	return (i << 16);
}
Fixed fxd::FtoFP(float f)
{
	return static_cast<Fixed>(f * Fone + 0.5f);
}
float fxd::FPtoF(Fixed f)
{
	return ( static_cast<float>(f) / Fone );
}
Fixed fxd::FPmult(Fixed a, Fixed b)
{
	return static_cast<Fixed>( ( static_cast<Fixed32>(a) * b ) >> 16);
}
Fixed fxd::FPdiv(Fixed a, Fixed b)
{
	return static_cast<Fixed>( ( static_cast<Fixed32>(a) << 16) / b);
}
long fxd::FPint8(Fixed8 n)
{
	return (n >> 8);
}











