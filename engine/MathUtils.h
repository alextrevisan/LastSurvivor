#ifndef _MATH_UTILS_H_
#define _MATH_UTILS_H_

#include <psxgte.h>

class MathUtils
{
public:
    static SVECTOR cross(const SVECTOR& left, const SVECTOR& right)
    {
        SVECTOR output;
        VECTOR input{(((left.vy*right.vz)-(left.vz*right.vy))>>12),
                (((left.vz*right.vx)-(left.vx*right.vz))>>12),
                (((left.vx*right.vy)-(left.vy*right.vx))>>12)
        };

        VectorNormalS(&input, &output);
        return output;
    }

    static SVECTOR Normalize(SVECTOR& vector)
    {
        VECTOR input = {vector.vx, vector.vy, vector.vz};
        SVECTOR output;
        VectorNormalS(&input, &output);
        return output;
    }

    static const short Magnitude(const SVECTOR& vector)
    {
        VECTOR input = {vector.vx, vector.vy, vector.vz};
        VECTOR output;
        Square0(&input, &output);
        return output.vx+output.vy+output.vz;
    }

    static const short Magnitude(VECTOR& vector)
    {
        VECTOR output;
        Square0(&vector, &output);
        return output.vx+output.vy+output.vz;
    }

    static const inline short DotProduct(const SVECTOR& left, const SVECTOR& right)
	{
		return ((left.vx * right.vx)>>12) + ((left.vy * right.vy)>>12) + ((left.vz * right.vz)>>12);
	}
};


#endif //_MATH_UTILS_H_