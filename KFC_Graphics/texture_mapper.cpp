#include "kfc_graphics_pch.h"
#include "texture_mapper.h"

#include <KFC_Math\2d_math.h>
#include <KFC_Math\3d_math.h>

// ----------------------
// Planar texture mapper
// ----------------------
TPlanarTextuerMapper::TPlanarTextuerMapper(	const TD3DVector& Corner1,
											const TD3DVector& Corner2,
											const TD3DVector& Corner3)
{
	m_Corner1 = Corner1;

	const TD3DVector d2 = Corner2 - m_Corner1;
	const TD3DVector d3 = Corner3 - m_Corner1;

	m_Conversion = TD3DMatrix().SetBassis(d2, d3, d2 * d3).Inversed32();
}

FPOINT TPlanarTextuerMapper::GetTextureCoords(const TD3DVector& Coords) const
{
	return MulVec32(Coords - m_Corner1, m_Conversion);
}

// ---------------------------
// Cylindrical texture mapper
// ---------------------------
TCylindricalTextureMapper::TCylindricalTextureMapper
	(	const TD3DVectorSegment&	BaseCenters,
		const TD3DVector&			U0Direction,
		const TD3DVector&			U1_4Direction)
{
	DEBUG_VERIFY(!Sign(U0Direction & U1_4Direction));

	m_FirstBaseCenter = BaseCenters.m_First;

	m_Conversion =
		TD3DMatrix().SetBassis(	U0Direction,
								U1_4Direction,
								BaseCenters.GetLength()).Inversed33();
}

FPOINT TCylindricalTextureMapper::GetTextureCoords(const TD3DVector& Coords) const
{
	const TD3DVector TextureCoords = (Coords - m_FirstBaseCenter) & m_Conversion;

	return FPOINT(	(float)(GetAngle(	TextureCoords.x,
										TextureCoords.y) * g_1_PI * 0.5),
					TextureCoords.z);
}

// -------------------------
// Spherical texture mapper
// -------------------------
TSphericalTextureMapper::TSphericalTextureMapper(	const TD3DVector& Center,
													const TD3DVector& U0Direction,
													const TD3DVector& U1_4Direction,
													const TD3DVector& VDirection)
{
	DEBUG_VERIFY(!Sign(U0Direction   & U1_4Direction));
	DEBUG_VERIFY(!Sign(U1_4Direction & VDirection));
	DEBUG_VERIFY(!Sign(VDirection    & U0Direction));

	m_Center = Center;

	m_Conversion =
		TD3DMatrix().SetBassis(U0Direction, VDirection, U1_4Direction).Inversed33();
}

FPOINT TSphericalTextureMapper::GetTextureCoords(const TD3DVector& Coords) const
{
	double dLattitude, dLongitude;

	CoordsToLatLon(TO_D((Coords - m_Center) & m_Conversion), dLattitude, dLongitude);

	return FPOINT((float)(dLongitude * g_1_PI * 0.5), (float)(dLattitude * g_1_PI + 0.5));
}