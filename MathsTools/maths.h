#ifndef MATHS_H
#define MATHS_H

#include <math.h>

#define PI		3.14159265358979323846f
#define PI_2		1.57079632679489661923f
#define PI_4		0.78539816339744830962f

inline int NextMultipleM(int n, int m)
{
	return ((n + (m - 1)) / m) * m;
}

inline int NextMultiple8(int n)
{
	return ((n + 7) / 8) * 8;
}

inline int NextMultiple4(int n)
{
	return ((n + 3) / 4) * 4;
}

inline int NextMultiple2(int n)
{
	return n + (n & 1);
}

#endif // MATHS_H
