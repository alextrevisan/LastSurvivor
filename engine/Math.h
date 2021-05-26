#ifndef _MATH_H_
#define _MATH_H_
#include <PS1FixedPoint/FixedPoint/FixedPoint.h>
#include <PS1FixedPoint/FixedPoint/Vector.h>

typedef ps1::FixedPoint<12, int, long long, true> Float;
typedef ps1::FixedPoint<12, short, long long, true> SFloat;

typedef ps1::Vector3F<VECTOR> Vector3D;
typedef ps1::Vector3F<SVECTOR> SVector3D;

#endif //_MATH_H_