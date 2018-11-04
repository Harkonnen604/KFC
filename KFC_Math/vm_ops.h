#ifndef vm_ops_h
#define vm_ops_h

#include "vector.h"
#include "matrix.h"

// ------------------
// Vector/matrix ops
// ------------------
template <class t>
inline TPoint<t> MulVec32(const TVector3<t>& v, const TMatrix44<t>& m)
{
	return TPoint<t>(	v.x * m._11 + v.y * m._21 + v.z * m._31,
						v.x * m._12 + v.y * m._22 + v.z * m._32);
}

template <class t>
inline TVector3<t> MulVec33(const TVector3<t>& v, const TMatrix44<t>& m)
{
	return TVector3<t>(	v.x * m._11 + v.y * m._21 + v.z * m._31,
						v.x * m._12 + v.y * m._22 + v.z * m._32,
						v.x * m._13 + v.y * m._23 + v.z * m._33);
}

template <class t>
inline TPoint<t> MulVec42(const TVector3<t>& v, const TMatrix44<t>& m)
{
	return TPoint<t>(	v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41,
						v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42);
}

template <class t>
inline TVector3<t> MulVec43(const TVector3<t>& v, const TMatrix44<t>& m)
{
	return TVector3<t>(	v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41,
						v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42,
						v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43);
}

template <class t>
inline TVector3<t> operator & (const TVector3<t>& v, const TMatrix44<t>& m)
{
	return MulVec33(v, m);
}

template <class t>
inline TVector3<t> operator * (const TVector3<t>& v, const TMatrix44<t>& m)
{
	return MulVec43(v, m);
}

template <class t>
inline TVector3<t> operator + (const TVector3<t>& v, const TMatrix44<t>& m)
	{ return TVector3<t>(v.x + m._41, v.y + m._42, v.z + m._43); }

template <class t>
inline TVector3<t> operator - (const TVector3<t>& v, const TMatrix44<t>& m)
	{ return TVector3<t>(v.x - m._41, v.y - m._42, v.z - m._43); }

template <class t>
inline TVector3<t>& operator *= (TVector3<t>& v, const TMatrix44<t>& m)
	{ return v = v * m; }

template <class t>
inline TVector3<t>& operator &= (TVector3<t>& v, const TMatrix44<t>& m)
	{ return v = v & m; }

template <class t>
inline TVector3<t>& operator += (TVector3<t>& v, const TMatrix44<t>& m)
	{ v.x += m._41, v.y += m._42, v.z += m._43; return v; }

template <class t>
inline TVector3<t>& operator -= (TVector3<t>& v, const TMatrix44<t>& m)
	{ v.x -= m._41, v.y -= m._42, v.z -= m._43; return v; }

template <class t>
inline double MixedProduct(	const TVector3<t>& v1,
							const TVector3<t>& v2,
							const TVector3<t>& v3)
{
	return	v1.x * v2.y * v3.z + v1.z * v2.x * v3.y + v1.y * v2.z * v3.x -
			v1.z * v2.y * v3.x - v1.x * v2.z * v3.y - v1.y * v2.x * v3.z;
}

#endif // vm_ops_h