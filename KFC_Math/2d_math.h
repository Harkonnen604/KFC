#ifndef _2d_math_h
#define _2d_math_h

// ----------------
// Global routines
// ----------------
template <class t>
inline t GetPointsDist2(const TPoint<t>& p1, const TPoint<t>& p2)
{
	const t dx = p1.x - p2.x;
	const t dy = p1.y - p2.y;

	return dx * dx + dy * dy;
}

template <class t>
inline void GetLineCoefs(const TPoint<t>& p1, const TPoint<t>& p2, t& a, t& b, t& c)
{
	a = p1.y - p2.y;
	b = p2.x - p1.x;
	c = p1.x * a + p1.y * b;
}

template <class t>
inline void GetOrtoLineCoefs(t a, t b, t c, t& a2, t& b2, t& c2, const TPoint<t>& p)
{
	a2 = -b;
	b2 = a;
	c2 = a2 * p.x + b2 * p.y;
}

template <class t>
inline bool SolveLinearEquations22(	t a1, t b1, t c1,
									t a2, t b2, t c2,
									t& x, t& y)
{
	const t d = a1 * b2 - a2 * b1;

	if(IsZero(d))
		return false;

	const t d1 = c1 * b2 - c2 * b1;
	const t d2 = a1 * c2 - a2 * c1;

	x = d1 / d;
	y = d2 / d;

	return true;
}

double GetLineDist2(double a, double b, double c, const DPOINT& p);

double GetLineDist2(const DPOINT& lp1, const DPOINT& lp2, DPOINT& p);

double GetSegmentDist2(const DPOINT& sp1, const DPOINT& sp2, const DPOINT& p);

// Returns 0.0 radians for zero vector
inline double GetAngle(double dx, double dy)
{
	return	Sign(dx) ?
				Sign(dx) < 0 ?
					g_PI + atan(dy / dx) :
					Sign(dy) < 0 ?
						g_PI * 2 + atan(dy / dx) :
						atan(dy / dx) :
				Sign(dy) ?
					Sign(dy) < 0 ?
						g_PI * 1.5 :
						g_PI * 0.5 :
					0.0;
}

#endif // _2d_math_h