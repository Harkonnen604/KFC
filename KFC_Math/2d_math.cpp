#include "kfc_math_pch.h"
#include "2d_math.h"

// ----------------
// Global routines
// ----------------
double GetLineDist2(double a, double b, double c, const DPOINT& p)
{
	const double d = a * p.x + b * p.y - c;

	DEBUG_VERIFY(Compare(d, 0.0));

	return (d * d) / (a * a + b * b);
}

double GetLineDist2(const DPOINT& lp1, const DPOINT& lp2, DPOINT& p)
{
	double a, b, c;
	GetLineCoefs(lp1, lp2, a, b, c);

	return GetLineDist2(a, b, c, p);
}

double GetSegmentDist2(const DPOINT& sp1, const DPOINT& sp2, const DPOINT& p)
{
	// End-points distances
	double d = Min(GetPointsDist2(sp1, p), GetPointsDist2(sp2, p));

	// Middle-poitn distance
	double a1, b1, c1;
	GetLineCoefs(sp1, sp2, a1, b1, c1);

	double a2, b2, c2;
	GetOrtoLineCoefs(a1, b1, c1, a2, b2, c2, p);

	double x, y;
	DEBUG_EVALUATE_VERIFY(SolveLinearEquations22(a1, b1, c1, a2, b2, c2, x, y));	

	if(	Compare(x, Min(sp1.x, sp2.x)) >= 0 &&
		Compare(x, Max(sp1.x, sp2.x)) <= 0 &&
		Compare(y, Min(sp1.y, sp2.y)) >= 0 &&
		Compare(y, Max(sp1.y, sp2.y)) <= 0) // on the segment
	{
		const double dd = GetPointsDist2(p, DPOINT(x, y));
		if(dd < d)
			d = dd;
	}

	return d;
}