#ifndef _3d_math_h
#define _3d_math_h

#include "2d_math.h"

// ----------------
// Global routines
// ----------------
inline void LatLonToCoords(double dLattitude, double dLongitude, TDVector3& RCoords)
{
	const double lcs = Cos(dLattitude);

	RCoords.Set(Cos(dLongitude) * lcs, Sin(dLattitude), Sin(dLongitude) * lcs);
}

// Returns 0/0 for zero vector, returns zero longitude for poles
inline void CoordsToLatLon(const TDVector3& Coords, double& dRLattitude, double& dRLongitude)
{
	if(Coords.IsZero())
	{
		dRLattitude = dRLongitude = 0.0;
		return;
	}

	dRLattitude = ASinSafe(Coords.y / Coords.GetLength());

	dRLongitude = GetAngle(Coords.x, Coords.z); // scaled by same 'cos(dRLattitude)' value
}

#endif // _3d_math_h