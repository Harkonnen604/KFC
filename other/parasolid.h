#ifndef parasolid_h
#define parasolid_h

#include <assert.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <typeinfo>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <memory>

// Parasolid precision constants
#define PARASOLID_INV_PRECISION		((double)(1<<27)) // a bit more than 1e+8
#define PARASOLID_PRECISION			(1.0 / PARASOLID_INV_PRECISION)

// Parasolid undefined values
#define PARASOLID_UNDEF_INT		(-32764)
#define PARASOLID_UNDEF_REAL	(-3.14158e13)

namespace parasolid
{

// -------
// Consts
// -------
extern const double g_dPI;

// ----------------------------------------
// Precision-aware floating point routines
// ----------------------------------------
inline double PrecisionAlign(double v)
{
	if(v == -DBL_MAX || v == +DBL_MAX)
		return v;

	return floor(v * PARASOLID_INV_PRECISION + 0.5) * PARASOLID_PRECISION;
}

inline int Compare(int a, int b)
	{ return a < b ? -1 : a > b ? +1 : 0; }

inline int Compare(double a, double b)
{
	a = PrecisionAlign(a);
	b = PrecisionAlign(b);

	return a < b ? -1 : a > b ? +1 : 0;
}

inline int Compare(const void* a, const void* b)
	{ return a < b ? -1 : a > b ? +1 : 0; }

struct precision_less : std::binary_function<double, double, bool>
{
public:
	inline bool operator () (double v1, double v2) const
		{ return Compare(v1, v2) < 0; }
};

inline int Sign(double v)
	{ return Compare(v, 0.0); }

inline double Floor(double v)
	{ return floor(PrecisionAlign(v)); }

inline double Ceil(double v)
	{ return ceil(PrecisionAlign(v)); }

inline double Min(double a, double b)
	{ return a < b ? a : b; }

inline double Max(double a, double b)
	{ return a > b ? a : b; }

inline double Limit(double a, double a1, double a2)
	{ return a < a1 ? a1 : a > a2 ? a2 : a; }

inline bool IsFinite(double v)
	{ return v != -DBL_MAX && v != +DBL_MAX; }

inline double SafeDiv(double a, double b)
	{ return b ? a / b : 0.0; }

// ------------------
// Iteration helpers
// ------------------
template <class t>
inline void StartTwoIters(t& cont, typename t::iterator& it1, typename t::iterator& it2)
{
	if((it2 = cont.begin()) == cont.end())
		it1 = cont.end();
	else
		it1 = it2++;
}

template <class t>
inline void IncTwoIters(t& cont, typename t::iterator& it1, typename t::iterator& it2)
{
	if(it2 == cont.end())
		it1 = cont.end();
	else
		it1 = it2++;
}

template <class t>
inline void StartTwoIters(t& cont, typename t::const_iterator& it1, typename t::const_iterator& it2)
{
	if((it2 = cont.begin()) == cont.end())
		it1 = cont.end();
	else
		it1 = it2++;
}

template <class t>
inline void IncTwoIters(t& cont, typename t::const_iterator& it1, typename t::const_iterator& it2)
{
	if(it2 == cont.end())
		it1 = cont.end();
	else
		it1 = it2++;
}

// ---------
// Vector 3
// ---------
template <class t>
struct TVector3
{
	union
	{
		struct
		{
			t x, y, z;
		};

		t v[3];
	};

	TVector3() : x(0), y(0), z(0) {}

	TVector3(t sx, t sy, t sz) : x(sx), y(sy), z(sz) {}

	TVector3& SetZero()
	{
		x = 0, y = 0, z = 0;
		return *this;
	}

	TVector3& Set(t sx, t sy, t sz)
	{
		x = sx, y = sy, z = sz;
		return *this;
	}

	TVector3& SetXAxis(t sx = 1)
	{
		x = sx, y = 0, z = 0;
		return *this;
	}

	TVector3& SetYAxis(t sy = 1)
	{
		x = 0, y = sy, z = 0;
		return *this;
	}

	TVector3& SetZAxis(t sz = 1)
	{
		x = 0, y = 0, z = sz;
		return *this;
	}

	t GetLengthSquare() const
		{ return x*x + y*y + z*z; }

	t GetLength() const
		{ return sqrt(GetLengthSquare());	}

	TVector3& Normalize()
	{
		const t Length = GetLength();
		
		if(Sign(Length))
			*this /= Length;
		else
			SetZero();

		return *this;
	}

	TVector3 Normalized() const
		{ return TVector3(*this).Normalize(); }	

	TVector3& Flip()
		{ x = -x, y = -y, z = -z; return *this; }	
	
	TVector3 Flipped() const
		{ return TVector3(*this).Flip(); }
	
	// Const operators
	TVector3 operator + (const TVector3& SVector) const
		{ return TVector3(x + SVector.x, y + SVector.y, z + SVector.z); }

	TVector3 operator - (const TVector3& SVector) const
		{ return TVector3(x - SVector.x, y - SVector.y, z - SVector.z); }	

	t operator & (const TVector3& SVector) const
		{ return x * SVector.x + y * SVector.y + z * SVector.z; }
	
	TVector3 operator * (const TVector3& SVector) const
	{
		return TVector3(	y * SVector.z - z * SVector.y,
							z * SVector.x - x * SVector.z,
							x * SVector.y - y * SVector.x);
	}

	TVector3 operator * (t c) const
		{ return TVector3(x * c, y * c, z * c); }

	TVector3 operator / (t c) const
		{ return *this * (c ? (t)1 / c : (t)0); }

	// Non-const operators
	TVector3& operator += (const TVector3& SVector)
		{ x += SVector.x, y += SVector.y, z += SVector.z; return *this; }

	TVector3& operator -= (const TVector3& SVector)
		{ x -= SVector.x, y -= SVector.y, z -= SVector.z; return *this; }

	TVector3& operator *= (const TVector3& SVector)
		{ return *this = *this * SVector; }

	TVector3& operator *= (t c)
		{ x *= c, y *= c, z *= c; return *this; }

	TVector3& operator /= (t c)
		{ return *this *= c ? (t)1 / c : (t)0; }

	TVector3 operator + () const
		{ return *this; }

	TVector3 operator - () const
		{ return TVector3(-x, -y, -z); }

	// Sign states
	bool IsZero() const
		{ return !Sign(x) && !Sign(y) && !Sign(z); }

	bool IsNormalized() const
		{ return !Compare(GetLengthSquare(), 1.0); }

	// Comparison operators
	bool operator == (const TVector3& SVector) const
		{ return !Compare(x, SVector.x) && !Compare(y, SVector.y) && !Compare(z, SVector.z); }

	bool operator != (const TVector3& SVector) const
		{ return Compare(x, SVector.x) || Compare(y, SVector.y) || Compare(z, SVector.z); }
};

template <class t>
inline t MixedProduct(	const TVector3<t>& v1,
						const TVector3<t>& v2,
						const TVector3<t>& v3)
{
	return	v1.x * v2.y * v3.z + v1.z * v2.x * v3.y + v1.y * v2.z * v3.x -
			v1.z * v2.y * v3.x - v1.x * v2.z * v3.y - v1.y * v2.x * v3.z;
}

template <class t>
inline TVector3<t> OrthogonalVector(const TVector3<t>& v)
{
	if(!Sign(v.x))
		return TVector3<t>(1, 0, 0);

	if(!Sign(v.y))
		return TVector3<t>(0, 1, 0);

	if(!Sign(v.z))
		return TVector3<t>(0, 0, 1);
	
	return TVector3<t>(-v.x, +v.y, 0).Normalized();
}

typedef TVector3<float>  TFVector3;
typedef TVector3<double> TDVector3;

template <class t>
inline TFVector3 TO_F(const TVector3<t>& v)
	{ return TFVector3((float)v.x, (float)v.y, (float)v.z); }

template <class t>
inline TDVector3 TO_D(const TVector3<t>& v)
	{ return TDVector3((double)v.x, (double)v.y, (double)v.z); }

// -----------
// Matrix 3x3
// -----------
template <class t>
struct TMatrix33
{
	union
	{
		struct
		{
			t _11, _12, _13;
			t _21, _22, _23;
			t _31, _32, _33;
		};

		t v[3][3];
	};

	TMatrix33() {}

	TMatrix33(	t s_11, t s_12, t s_13,
				t s_21, t s_22, t s_23,
				t s_31, t s_32, t s_33) :

		_11(s_11), _12(s_12), _13(s_13),
		_21(s_21), _22(s_22), _23(s_23),
		_31(s_31), _32(s_32), _33(s_33) {}

	TMatrix33(const TVector3<t>& x, const TVector3<t>& y, const TVector3<t>& z)
		{ SetBasis(x, y, z); }

	TMatrix33& SetZero()
	{
		memset(v, 0, sizeof(v));
		return *this;
	}

	TMatrix33& SetIdentity()
	{
		_11 = 1, _12 = 0, _13 = 0;
		_21 = 0, _22 = 1, _23 = 0;
		_31 = 0, _32 = 0, _33 = 1;

		return *this;
	}

	TMatrix33& SetBasis(const TVector3<t>& x, const TVector3<t>& y, const TVector3<t>& z)
	{
		_11 = x.x, _12 = y.x, _13 = z.x;
		_21 = x.y, _22 = y.y, _23 = z.y;
		_31 = x.z, _32 = y.z, _33 = z.z;

		return *this;
	}

	TMatrix33& SetRotateX(double a)
	{
		return SetBasis(TVector3<t>(0, (t)+cos(a), (t)-sin(a)),
						TVector3<t>(0, (t)+sin(a), (t)+cos(a)),
						TVector3<t>(1,         0,          0));
	}

	TMatrix33& SetRotateY(double a)
	{
		return SetBasis(TVector3<t>((t)+cos(a), 0, (t)+sin(a)),
						TVector3<t>((t)-sin(a), 0, (t)+cos(a)),
						TVector3<t>(        0,  1,         0 ));
	}

	TMatrix33& SetRotateZ(double a)
	{
		return SetBasis(TVector3<t>((t)+cos(a), (t)-sin(a), 0),
						TVector3<t>((t)+sin(a), (t)+cos(a), 0),
						TVector3<t>(        0,          0,  1));
	}

	TMatrix33 operator + (const TMatrix33& SMatrix) const
	{
		return TMatrix33(	_11 + SMatrix._11, _12 + SMatrix._12, _13 + SMatrix._13,
							_21 + SMatrix._21, _22 + SMatrix._22, _23 + SMatrix._23,
							_31 + SMatrix._31, _32 + SMatrix._32, _33 + SMatrix._33);
	}


	TMatrix33 operator - (const TMatrix33& SMatrix) const
	{
		return TMatrix33(	_11 - SMatrix._11, _12 - SMatrix._12, _13 - SMatrix._13,
							_21 - SMatrix._21, _22 - SMatrix._22, _23 - SMatrix._23,
							_31 - SMatrix._31, _32 - SMatrix._32, _33 - SMatrix._33);
	}

	TMatrix33 operator * (const TMatrix33& SMatrix) const
	{
		return TMatrix33(	_11 * SMatrix._11 + _12 * SMatrix._21 + _13 * SMatrix._31,
							_11 * SMatrix._12 + _12 * SMatrix._22 + _13 * SMatrix._32,
							_11 * SMatrix._13 + _12 * SMatrix._23 + _13 * SMatrix._33,
							
							_21 * SMatrix._11 + _22 * SMatrix._21 + _23 * SMatrix._31,
							_21 * SMatrix._12 + _22 * SMatrix._22 + _23 * SMatrix._32,
							_21 * SMatrix._13 + _22 * SMatrix._23 + _23 * SMatrix._33,

							_31 * SMatrix._11 + _32 * SMatrix._21 + _33 * SMatrix._31,
							_31 * SMatrix._12 + _32 * SMatrix._22 + _33 * SMatrix._32,
							_31 * SMatrix._13 + _32 * SMatrix._23 + _33 * SMatrix._33);
	}

	TVector3<t> operator * (const TVector3<t>& Vector) const
	{
		return TVector3<t>(	_11 * Vector.x + _12 * Vector.y + _13 * Vector.z,
							_21 * Vector.x + _22 * Vector.y + _23 * Vector.z,
							_31 * Vector.x + _32 * Vector.y + _33 * Vector.z);
	}

	TMatrix33& operator += (const TMatrix33& SMatrix)
	{
		_11 += SMatrix._11, _12 += SMatrix._12, _13 += SMatrix._13;
		_21 += SMatrix._21, _22 += SMatrix._22, _23 += SMatrix._23;
		_31 += SMatrix._31, _32 += SMatrix._32, _33 += SMatrix._33;

		return *this;
	}

	TMatrix33& operator -= (const TMatrix33& SMatrix)
	{
		_11 -= SMatrix._11, _12 -= SMatrix._12, _13 -= SMatrix._13;
		_21 -= SMatrix._21, _22 -= SMatrix._22, _23 -= SMatrix._23;
		_31 -= SMatrix._31, _32 -= SMatrix._32, _33 -= SMatrix._33;

		return *this;
	}

	TMatrix33& operator *= (const TMatrix33<t>& SMatrix)
		{ return *this = *this * SMatrix; }

	TMatrix33 operator + () const
		{ return *this; }

	TMatrix33 operator - () const
	{
		return TMatrix33(	-_11, -_12, -_13,
							-_21, -_22, -_23,
							-_31, -_32, -_33);
	}

	bool operator == (const TMatrix33& SMatrix) const
	{
		return	!Compare(_11, SMatrix._11) && !Compare(_12, SMatrix._12) && !Compare(_13, SMatrix._13) &&
				!Compare(_21, SMatrix._21) && !Compare(_22, SMatrix._22) && !Compare(_13, SMatrix._23) &&
				!Compare(_31, SMatrix._31) && !Compare(_32, SMatrix._32) && !Compare(_13, SMatrix._33);
	}

	bool operator != (const TMatrix33& SMatrix) const
	{
		return	Compare(_11, SMatrix._11) || Compare(_12, SMatrix._12) || Compare(_13, SMatrix._13) &&
				Compare(_21, SMatrix._21) || Compare(_22, SMatrix._22) || Compare(_13, SMatrix._23) &&
				Compare(_31, SMatrix._31) || Compare(_32, SMatrix._32) || Compare(_13, SMatrix._33);
	}

	t GetDet() const
	{
		return _11*_22*_33 + _13*_21*_32 + _12*_23*_31 -
		       _13*_22*_31 - _11*_23*_32 - _12*_21*_33;
	}

	TMatrix33 Inversed() const
	{
		TMatrix33 Result;

		t Det = GetDet();

		if(!Det)
			return *this;

		Det = (t)1 / Det;

		Result._11 = (_22*_33 - _23*_32) * Det;
		Result._12 = (_13*_32 - _12*_33) * Det;
		Result._13 = (_12*_23 - _13*_22) * Det;

		Result._21 = (_31*_23 - _21*_33) * Det;
		Result._22 = (_11*_33 - _13*_31) * Det;
		Result._23 = (_13*_21 - _11*_23) * Det;

		Result._31 = (_21*_32 - _31*_22) * Det;
		Result._32 = (_31*_12 - _11*_32) * Det;
		Result._33 = (_11*_22 - _12*_21) * Det;

		return Result;
	}

	TMatrix33& Inverse()
		{ return *this = Inversed(); }

	bool IsZero() const
	{
		return	!Sign(_11) && !Sign(_12) && !Sign(_13) &&
				!Sign(_21) && !Sign(_22) && !Sign(_23) &&
				!Sign(_31) && !Sign(_32) && !Sign(_33);
	}
};

typedef TMatrix33<float>  TFMatrix33;
typedef TMatrix33<double> TDMatrix33;

template <class t>
inline TFMatrix33 TO_F(const TMatrix33<t>& m)
{
	return TFMatrix33(	(float)m._11, (float)m._12, (float)m._13,
						(float)m._21, (float)m._22, (float)m._23,
						(float)m._31, (float)m._32, (float)m._33);
}

template <class t>
inline TDMatrix33 TO_D(const TMatrix33<t>& m)
{
	return TDMatrix33(	(double)m._11, (double)m._12, (double)m._13,
						(double)m._21, (double)m._22, (double)m._23,
						(double)m._31, (double)m._32, (double)m._33);
}

// ----------------
// Helper routines
// ----------------
inline size_t GetPrevCyclicIdx(size_t i, size_t n)
	{ return i ? i - 1 : n - 1; }

inline size_t GetPrevCappedIdx(size_t i, size_t n)
	{ return i ? i - 1 : 0; }

inline size_t GetNextCyclicIdx(size_t i, size_t n)
	{ return i + 1 < n ? i + 1 : 0u; }

inline size_t GetNextCappedIdx(size_t i, size_t n)
	{ return i + 1 < n ? i + 1 : i; }

typedef std::set<double, precision_less> TParamValues;

typedef std::map<double, unsigned, precision_less> TApproxParamValues;

inline void GenerateMidPoints(std::vector<double>& RValues, double v1, double v2, const TParamValues& ParamValues)
{
	if(v1 < v2)
	{
		for(TParamValues::const_iterator iter = ParamValues.upper_bound(v1) ;
			iter != ParamValues.end() && Compare(*iter, v2) < 0 ;
			++iter)
		{
			RValues.push_back(*iter);
		}
	}
	else
	{
		size_t first = RValues.size();

		for(TParamValues::const_iterator iter = ParamValues.upper_bound(v2) ;
			iter != ParamValues.end() && Compare(*iter, v1) < 0 ;
			++iter)
		{
			RValues.push_back(*iter);
		}

		std::reverse(RValues.begin() + first, RValues.end());
	}
}

// Returns angle of (dx;dy) vector in radians within [0;PI*2) range
inline double Get2D_Angle(double dx, double dy)
{
	if(Sign(dx) < 0) // 2nd and 3rd quarter
		return g_dPI + atan(dy / dx);

	if(Sign(dx) > 0) // 1st and 4th quarter
	{
		if(Sign(dy) < 0) // 4th quarter
			return g_dPI * 2 + atan(dy / dx);
		
		if(Sign(dy) > 0) // 1st quarter
			return atan(dy / dx);

		return 0.0; // right direction
	}

	if(Sign(dy) < 0) // top direction
		return g_dPI * 1.5;
	
	if(Sign(dy) > 0) // bottom direction
		return g_dPI * 0.5;
	
	return 0.0; // center
}

inline double Get2D_Angle(const TDVector3& v)
	{ return Get2D_Angle(v.x, v.y); }

inline char MulSense(char s1, char s2)
	{ return s1 == '?' || s2 == '?' ? '?' : s1 == s2 ? '+' : '-'; }

// ---------------
// Pointer holder
// ---------------

/*
Acts like std::auto_ptr<t>, but has copy constructor which
works via 't* t::CreateCopy() const' method. This allows
TPtrHodler to be used as member of vector<> and list<>
storage classes. Altought CreateCopy is not used with parasolid
classes, this member must exists for comilability.
*/

template <class t>
class TPtrHolder
{
private:
	t* m_pData;

public:
	TPtrHolder() : m_pData(NULL) {}

	TPtrHolder(t* pData) : m_pData(pData) {}

	TPtrHolder(const TPtrHolder& SHolder) : m_pData(NULL)
		{ *this = SHolder; }	

	~TPtrHolder()
		{ *this = NULL; }

	// Nullifies own pointer, but does not destroy the object
	TPtrHolder& Invalidate()
		{ m_pData = NULL; return *this; }

	TPtrHolder& operator = (t* pSData)
	{
		if(m_pData != pSData)
			delete m_pData, m_pData = pSData;

		return *this;
	}

	TPtrHolder& operator = (const TPtrHolder& SHolder)
	{
		if(m_pData != SHolder.m_pData)
		{
			delete m_pData;

			m_pData = SHolder ? SHolder.m_pData->CreateCopy() : NULL;
		}

		return *this;
	}

	operator t* () const
		{ return m_pData; }

	t* operator -> () const
		{ assert(m_pData); return m_pData; }

	// Similar to auto_ptr<t>::release()
	t* Extract()
		{ t* pData = m_pData; Invalidate(); return pData; }
}; 

// ----------
// Exception
// ----------
struct TException
{
public:
	std::string m_Message;
	
public:
	TException(const char* pMessage) : m_Message(pMessage) {}

	TException(const std::string& Message) : m_Message(Message) {}
};

// ------------
// Basic types
// ------------

// byte
typedef unsigned char byte;

// logical
enum logical
{
	log_undef	= -1,
	log_false	= 0,
	log_true	= 1 
};

// interval
struct interval
{
public:
	double low, high; 

public:
	interval() {}

	interval(double slow, double shigh) : low(slow), high(shigh) {}
};

// vector
typedef TDVector3 vector;

// matrix
typedef TDMatrix33 matrix;

// hvec
typedef vector hvec;

// box
struct box
{
public:
	interval x, y, z;

public:
	box() {}

	box(const interval& sx, const interval& sy, const interval& sz) : x(sx), y(sy), z(sz) {}
};

// ---------
// UV point
// ---------
struct T_UV_Point
{
public:
	double u, v;

public:
	T_UV_Point() {}

	T_UV_Point(double su, double sv) : u(su), v(sv) {}

	T_UV_Point& operator += (const T_UV_Point& p)
		{ u += p.u, v += p.v; return *this; }

	T_UV_Point& operator -= (const T_UV_Point& p)
		{ u -= p.u, v -= p.v; return *this; }

	T_UV_Point& operator *= (double c)
		{ u *= v, v *= c; return *this; }

	T_UV_Point& operator /= (double c)
		{ return *this *= c ? 1.0 / c : 0.0; }

	double GetLengthSquare() const
		{ return u*u + v*v; }

	double GetLength() const
		{ return sqrt(GetLengthSquare()); }

	T_UV_Point& Normalize()
		{ return *this /= GetLength(); }

	inline T_UV_Point Normalized() const;
};

inline T_UV_Point operator + (const T_UV_Point& p1, const T_UV_Point& p2)
	{ return T_UV_Point(p1.u + p2.u, p1.v + p2.v); }

inline T_UV_Point operator - (const T_UV_Point& p1, const T_UV_Point& p2)
	{ return T_UV_Point(p1.u - p2.u, p1.v - p2.v); }

inline T_UV_Point operator * (const T_UV_Point& p, double c)
	{ return T_UV_Point(p.u * c, p.v * c); }

inline T_UV_Point operator / (const T_UV_Point& p, double c)
	{ return p * (c ? 1.0 / c : 0.0); }

inline T_UV_Point T_UV_Point::Normalized() const
	{ return *this / GetLength(); }

inline int Compare(const T_UV_Point& p1, const T_UV_Point& p2)
{
	int d;

	if(d = Compare(p1.v, p2.v))
		return d;

	if(d = Compare(p1.u, p2.u))
		return d;

	return 0;
}

inline bool operator < (const T_UV_Point& p1, const T_UV_Point& p2)
	{ return Compare(p1, p2) < 0; }

inline vector UVasXYZ(const T_UV_Point& p)
	{ return vector(p.u, p.v, 0); }

inline T_UV_Point XYZasUV(const vector& v)
	{ return T_UV_Point(v.x, v.y); }

inline T_UV_Point Sign(const T_UV_Point& p)
	{ return T_UV_Point(Sign(p.u), Sign(p.v)); }

inline bool IsFinite(const T_UV_Point& p)
	{ return IsFinite(p.u) && IsFinite(p.v); }

// ----------------
// Predeclarations
// ----------------
class T_XT_Reader;

struct NODE;
struct TRANSFORM;

// ---------------
// Node index map
// ---------------
typedef std::map<int, NODE*> TNodeIndexMap;

template <class t>
const t* FindNodeByIndex(const TNodeIndexMap& Map, int iIndex)
{
	TNodeIndexMap::const_iterator i = Map.find(iIndex);

	if(i == Map.end())
		return NULL;

	return dynamic_cast<const t*>(i->second);
}

// -------------
// Ptr resolver
// -------------

// Assumes 'p' to contain index, the looks for NODE* inside Map
// and verifyes that it has desired type 't' via dynamic_cast
template <class t>
inline void ResolvePtr(const TNodeIndexMap& Map, t*& p)
{
	int iIndex = (int)reinterpret_cast<intptr_t>(p);

	if(!iIndex) // Index 0 becomes NULL pointer
	{
		p = NULL;
		return;
	}

	// Getting node
	TNodeIndexMap::const_iterator i = Map.find(iIndex);

	if(i == Map.end()) // node not found
	{
		p = NULL;
		return;
	}

	// Checking node type
	p = dynamic_cast<t*>(i->second);
}

// -----
// NODE
// -----

/*
Base class of all parasolid records. Used as virtual base class because class-pointer
aggregators (such as CURVE_OWNER) also inherit it for being automatically castable to
NODE*.
*/

struct NODE
{
private:
	int index; // $d

	bool prepared;
	bool valid;

protected:
	// Loads all info from parasolid reader right after node type. Expected
	// to call 'LoadData' of base class before loading specific members.
	virtual void LoadData(T_XT_Reader& Reader);	

	virtual bool InternalPrepare(double dMaxAngle)
		{ return true; }

	virtual void InternalPostPrepare() {}

public:
	bool IsValid() const
	{
		assert(prepared);
		return valid;
	}

	// Returns node type string using RTTI
	std::string GetTypeString() const;

	// Returns expanded node type string using RTTI
	std::string GetExpandedTypeString() const;

	// Creates new NODE from its type 'iType'
	static NODE* CreateFromType(int iType);

	// Loads NODE from 'Reader' parasolid stream, reader is expected to point to node type.
	// If terminator node is read (1 0), NULL is returned.
	static NODE* Load(T_XT_Reader& Reader);	

public:
	NODE()
		{ prepared= false; valid = false; }

	virtual ~NODE() {}

	// Safe index getter to support NULL pointers
	int GetIndex() const
		{ return this ? index : 0; }

	// Dummy 'CreateCopy' for TPtrHolder consistency
	NODE* CreateCopy() const
		{ assert(false); return NULL; }

	// Initially pointers are read as integers into type* variables. This call
	// resolves all of them into real pointers using 'Map' and sets unresolved
	// references to NULL. This method is expected to call ResolvePtr of base class
	// before resolving specific pointers.
	virtual void ResolvePtrs(const TNodeIndexMap& Map) {}	

	// Prepares node for operations, e.g. precalulaion of transformation matrices
	bool Prepare(double dMaxAngle)
	{
		if(prepared)
			return valid;

		prepared = true;

		if(!InternalPrepare(dMaxAngle))
			return false;

		valid = true;

		InternalPostPrepare();		

		return true;
	}
};

// -------------
// NODE_WITH_ID
// -------------

// Intermediate class for nodes with ID

struct NODE_WITH_ID : virtual NODE
{
private:
	int m_iID; // $d

protected:
	void LoadData(T_XT_Reader& Reader);

public:
	int GetID() const
		{ return this ? m_iID : 0; }
};

// ---------
// Topology
// ---------
enum TTopology
{
	TPL_INFINITE,
	TPL_INFINITE_MIN,
	TPL_INFINITE_MAX,
	TPL_OPEN,
	TPL_CLOSED,
	TPL_PERIODIC
};

inline double NormalizeTplCoord(double v, double v1, double v2, TTopology tpl)
{
	if(tpl >= TPL_PERIODIC)
	{
		// Bringing 'v' to [v1;v2) range
		if((v = v1 + fmod(v - v1, v2 - v1)) < v1)
			v += v2 - v1;

		// Treating 'v2' epsilon vicinity as 'v1'
		if(!Compare(v, v2))
			v = v1;
	}
	else
	{
		v = Limit(v, v1, v2);
	}

	return v;
}

// -----------
// Renderable
// -----------
struct RENDERABLE : virtual NODE
{
public:
	virtual void Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform = NULL, size_t szDepth = 0) const;
};

// ------
// Nodes
// ------
typedef std::list<TPtrHolder<NODE> > NODES;

// ----------
// XT reader
// ----------

/*
Parasolid format reader. Currently works with stdin. Reads ASCII parasolid file
line after line, removes trailing spaces and tracks current character position.
*/

class T_XT_Reader
{
private:
	FILE* m_pFile; // mesh file

	char m_Line[256]; // current read line

	char* m_pCur; // pointer to current character within 'm_Line'

private:
	// Reads a single character, updaing 'm_Line' buffer if necessary
	char InternalReadChar(bool bAllowEOF)
	{
		if(!*m_pCur && !ReadLine())
		{
			if(!bAllowEOF)
				throw new TException("Unexpected end of file");

			return EOF;
		}

		return *m_pCur++;
	}

	// Resets line buffer
	void ResetLine();

	// Reads next line, skipping empty lines. If end of file is reached, 'false' is returned.
	bool ReadLine();

	// Reads sequence of characters up to space, EOF or '?' character
	void ReadNumericToken(char* s, size_t n);

	// Skips text header of parasolid file
	void ReadTextHeader();

	// Reads format header of parasolid file and veryfies schema version
	void ReadFormatHeader();

public:
	T_XT_Reader();	

	// Byref-readers
	void ReadChar(char& c) // $c
		{ c = InternalReadChar(false); }
	
	void ReadByte		(byte&			v); // $u
	void ReadShort		(short&			v); // $n, $w	
	void ReadInt		(int&			v); // $d
	void ReadReal		(double&		v); // $f
	void ReadLogical	(logical&		v); // $l
	void ReadString		(std::string&	s);
	void ReadVector		(vector&		v); // $v, $h
	void ReadInterval	(interval&		i); // $i
	void ReadBox		(box&			b); // $b

	template <class t>
	void ReadEnum(t& v) // $u
		{ v = (t)ReadByte(); }	

	template <class t>
	void ReadPtr(t*& p) // $p
		{ p = reinterpret_cast<t*>((intptr_t)ReadInt()); }

	// Retval-readers
	char ReadChar()
		{ char c; ReadChar(c); return c; }

	byte ReadByte() // $u
		{ byte v; ReadByte(v); return v; }

	short ReadShort() // $n, $w
		{ short v; ReadShort(v); return v; }

	int ReadInt() // $d
		{ int v; ReadInt(v); return v; }

	double ReadReal() // $f
		{ double v; ReadReal(v); return v; }

	logical ReadLogical() // $l
		{ logical v; ReadLogical(v); return v; }

	std::string ReadString()
		{ std::string s; ReadString(s); return s; }

	vector ReadVector()
		{ vector v; ReadVector(v); return v; }

	interval ReadInterval()
		{ interval i; ReadInterval(i); return i; }

	box ReadBox()
		{ box b; ReadBox(b); return b; }

	template <class t>
	t ReadEnum() // $u
		{ t v; ReadEnum(v); return v; }

	template <class t>
	t* ReadPtr() // $p
		{ t* v; ReadPtr(v); return v; }	

	// Complete reader. Loads file into 'RNodes' and fills in 'RNodesIndexMap'
	void ReadFile(const char* pFileName, NODES& RNodes, TNodeIndexMap& RNodeIndexMap, double dMaxAngle);
};

// --------------
// SCH knot type
// --------------
enum SCH_knot_type
{
	SCH_KT_unset			= 1,
	SCH_KT_non_uniform		= 2,
	SCH_KT_uniform			= 3,
	SCH_KT_quasi_uniform	= 4,
	SCH_KT_piecewise_bezier	= 5,
	SCH_KT_bezier_ends		= 6
};

// ---------------
// SCH curve form
// ---------------
enum SCH_curve_form
{
	SCH_CF_unset			= 1,
	SCH_CF_arbitrary		= 2,
	SCH_CF_polyline			= 3,
	SCH_CF_circular_arc		= 4,
	SCH_CF_elliptic_arc		= 5,
	SCH_CF_parabolic_arc	= 6,
	SCH_CF_hyperbolic_arc	= 7
};

// -------------
// SCH self int
// -------------
enum SCH_self_int
{
	SCH_SI_unset						= 1,
	SCH_SI_no_self_intersections		= 2,
	SCH_SI_self_intersects				= 3,
	SCH_SI_checked_ok_in_old_version	= 4
};

// -----------------
// SCH surface form
// -----------------
enum SCH_surface_form
{
	SCH_SF_unset				= 1,
	SCH_SF_arbitrary			= 2,
	SCH_SF_planar				= 3,
	SCH_SF_cylindrical			= 4,
	SCH_SF_conical				= 5,
	SCH_SF_spherical			= 6,
	SCH_SF_toroidal				= 7,
	SCH_SF_surf_of_revolution	= 8,
	SCH_SF_ruled				= 9,
	SCH_SF_quadric				= 10,
	SCH_SF_swept				= 11
};

// ------------------
// SCH_assembly_type
// ------------------
enum SCH_assembly_type
{
	SCH_AT_collective_assembly	= 1,
	SCH_AT_conjunctive_assembly	= 2,
	SCH_AT_disjunctive_assembly	= 3
};

// ---------------
// SCH_part_state
// ---------------
enum SCH_part_state
{
	SCH_PS_new_part			= 1,
	SCH_PS_stored_part		= 2,
	SCH_PS_modified_part	= 3,
	SCH_PS_anonymous_part	= 4,
	SCH_PS_unloaded_part	= 5
};

// ------------------
// SCH_instance_type
// ------------------
enum SCH_instance_type
{
	SCH_IT_positive_instance = 1,
	SCH_IT_negative_instance = 2
};

// --------------
// SCH_body_type
// --------------
enum SCH_body_type
{
	SCH_BT_solid_body	= 1,
	SCH_BT_wire_body	= 2,
	SCH_BT_sheet_body	= 3,
	SCH_BT_general_body	= 6
};

// -------------------
// SCH_nom_geom_state
// -------------------
enum SCH_nom_geom_state
{
	SCH_NGS_nom_geom_off	= 1,
	SCH_NGS_nom_geom_on		= 2
};

// ---------
// LIS_type
// ---------
enum LIS_type
{
	LIS_pointer = 4
};

// -----------------
// SCH_logged_event
// -----------------
enum SCH_logged_event
{
	SCH_LE_rotate		= 0,
	SCH_LE_scale		= 1,
	SCH_LE_translate	= 2,
	SCH_LE_reflect		= 3,
	SCH_LE_split		= 4,
	SCH_LE_merge		= 5,
	SCH_LE_transfer		= 6,
	SCH_LE_change		= 7,
	SCH_LE_max_logged_event
};

// ---------------------
// SCH_action_on_fields
// ---------------------
enum SCH_action_on_fields
{
	SCH_AOF_do_nothing			= 0,
	SCH_AOF_delete				= 1,
	SCH_AOF_transform			= 2,
	SCH_AOF_propagate			= 3,
	SCH_AOF_keep_sub_dominant	= 4,
	SCH_AOF_keep_if_equal		= 5,
	SCH_AOF_combine				= 6
};

// ------------------
// SCH_attrib_owners
// ------------------
enum SCH_attrib_owners
{
	SCH_AO_as_owner = 0,
	SCH_AO_in_owner = 1,
	SCH_AO_by_owner = 2,
	SCH_AO_sh_owner = 3,
	SCH_AO_fa_owner = 4,
	SCH_AO_lo_owner = 5,
	SCH_AO_ed_owner = 6,
	SCH_AO_vx_owner = 7,
	SCH_AO_fe_owner = 8,
	SCH_AO_sf_owner = 9,
	SCH_AO_cu_owner = 10,
	SCH_AO_pt_owner = 11,
	SCH_AO_rg_owner = 12,
	SCH_AO_fn_owner = 13,
	SCH_AO_max_owner
};

// ---------------
// SCH_field_type
// ---------------
enum SCH_field_type
{
	SCH_FT_int_field		= 1,
	SCH_FT_real_field		= 2,
	SCH_FT_char_field		= 3,
	SCH_FT_point_field		= 4,
	SCH_FT_vector_field		= 5,
	SCH_FT_direction_field	= 6,
	SCH_FT_axis_field		= 7,
	SCH_FT_tag_field		= 8,
	SCH_FT_pointer_field	= 9,
	SCH_FT_unicode_field	= 10
};

// ---------------
// SCH_group_type
// ---------------
enum SCH_group_type
{
	SCH_GT_instance_fe	= 1,
	SCH_GT_face_fe		= 2,
	SCH_GT_loop_fe		= 3,
	SCH_GT_edge_fe		= 4,
	SCH_GT_vertex_fe	= 5,
	SCH_GT_surface_fe	= 6,
	SCH_GT_curve_fe		= 7,
	SCH_GT_point_fe		= 8,
	SCH_GT_mixed_fe		= 9,
	SCH_GT_region_fe	= 10
};

// -------------
// Declarations
// -------------
struct ANY_CURVE;
struct LINE;
struct CIRCLE;
struct ELLIPSE;
struct NURBS_CURVE;
struct BSPLINE_VERTICES;
struct KNOT_SET;
struct KNOT_MULT;
struct CURVE_DATA;
struct HELIX_CU_FORM;
struct B_CURVE;
struct INTERSECTION;
struct TRIMMED_CURVE;
struct CHART;
struct LIMIT;
struct PE_CURVE;
struct EXT_PE_DATA;
struct INT_PE_DATA;
struct SP_CURVE;
struct ANY_SURF;
struct PLANE;
struct CYLINDER;
struct CONE;
struct SPHERE;
struct TORUS;
struct BLENDED_EDGE;
struct BLEND_BOUND;
struct OFFSET_SURF;
struct NURBS_SURF;
struct SURFACE_DATA;
struct HELIX_SU_FORM;
struct B_SURFACE;
struct SWEPT_SURF;
struct SPUN_SURF;
struct PE_SURF;
struct POINT;
struct TRANSFORM;
struct GEOMETRIC_OWNER;
struct WORLD;
struct ASSEMBLY;
struct KEY;
struct INSTANCE;
struct BODY;
struct REGION;
struct SHELL;
struct FACE;
struct LOOP;
struct FIN;
struct VERTEX;
struct EDGE;
struct LIST;
struct POINTER_LIS_BLOCK;
struct ATT_DEF_ID;
struct FIELD_NAMES;
struct ATTRIB_DEF;
struct ATTRIBUTE;
struct INT_VALUES;
struct REAL_VALUES;
struct CHAR_VALUES;
struct UNICODE_VALUES;
struct POINT_VALUES;
struct VECTOR_VALUES;
struct DIRECTION_VALUES;
struct AXIS_VALUES;
struct TAG_VALUES;
struct GROUP;
struct MEMBER_OF_GROUP;

// ----------------
// Pointer classes
// ----------------
typedef ANY_CURVE	CURVE;		// since CURVE   ptr class exactly matches ANY_CURVE descendants, this is typedefed
typedef ANY_SURF	SURFACE;	// since SURFACE ptr class exactly matches ANY_SURF  descendants, this is typedefed

struct CURVE_OWNER		: virtual NODE {};
struct SURFACE_OWNER	: virtual NODE {};
struct ATTRIB_GROUP		: virtual NODE {};
struct GEOMETRY			: virtual NODE {};
struct PE_DATA			: virtual NODE {};
struct PE_INT_GEOM		: virtual NODE {};
struct POINT_OWNER		: virtual NODE {};
struct TRANSFORM_OWNER	: virtual NODE {};
struct PART				: virtual NODE, virtual RENDERABLE {};
struct SHELL_OR_BODY	: virtual NODE {};
struct LIS_BLOCK		: virtual NODE {};
struct LIST_OWNER		: virtual NODE {};
struct FIELD_NAME		: virtual NODE {};
struct ATTRIBUTE_OWNER	: virtual NODE {};
struct FIELD_VALUES		: virtual NODE {};
struct GROUP_MEMBER		: virtual NODE {};

// ----------
// ANY_CURVE
// ----------
struct ANY_CURVE : virtual NODE_WITH_ID, GEOMETRY, PE_INT_GEOM, ATTRIBUTE_OWNER, GROUP_MEMBER
{
private:
	double mint, maxt;

protected:
	TParamValues t_values;	

protected:
	void LoadData(T_XT_Reader& Reader);

	virtual void InternalPostPrepare()
	{
		NODE_WITH_ID::InternalPostPrepare();

		mint = GetTopology() <= TPL_INFINITE || GetTopology() == TPL_INFINITE_MIN ? -DBL_MAX : *t_values.begin ();
		maxt = GetTopology() <= TPL_INFINITE || GetTopology() == TPL_INFINITE_MAX ? +DBL_MAX : *t_values.rbegin();
	}

public:
	ATTRIB_GROUP*		attributes_groups; // $p
	CURVE_OWNER*		owner; // $p
	CURVE*				next; // $p
	CURVE*				previous; // $p
	GEOMETRIC_OWNER*	geometric_owner; // $p
	char				sense; // $c

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	const TParamValues& GetTValues() const
		{ return t_values; }

	double GetMinT() const
		{ assert(IsValid()); return mint; }

	double GetMaxT() const
		{ assert(IsValid()); return maxt; }

	double GetTLength() const
		{ assert(IsValid()); assert(GetTopology() >= TPL_OPEN); return maxt - mint; }

	double NormalizeT(double t) const
		{ return NormalizeTplCoord(t, GetMinT(), GetMaxT(), GetTopology()); }

	vector GetRealTangent(double t, int t_bias = 0) const
		{ return GetRealNoSenseTangent(t, t_bias) * (sense == '-' ? -1 : +1); }

	vector GetApproxTangent(double t, int t_bias = 0) const
		{ return GetApproxNoSenseTangent(t, t_bias) * (sense == '-' ? -1 : +1); }

	virtual TTopology GetTopology() const = 0;
	
	virtual vector GetRealXYZ(double t) const = 0;

	virtual vector GetApproxXYZ(double t) const
		{ return GetRealXYZ(t); }

	virtual vector GetRealNoSenseTangent(double t, int t_bias = 0) const = 0;

	virtual vector GetApproxNoSenseTangent(double t, int t_bias = 0) const
		{ return GetRealNoSenseTangent(t, t_bias); }

	virtual double GetApproxT(const vector& xyz) const = 0;
};

// ---------
// ANY_SURF
// ---------
struct ANY_SURF : virtual NODE_WITH_ID, GEOMETRY, PE_INT_GEOM, ATTRIBUTE_OWNER, GROUP_MEMBER
{
private:
	double minu, maxu;
	double minv, maxv;

protected:
	TParamValues u_values;
	TParamValues v_values;

protected:
	void LoadData(T_XT_Reader& Reader);

	void InternalPostPrepare()
	{
		NODE_WITH_ID::InternalPostPrepare();

		minu = GetUTopology() <= TPL_INFINITE || GetUTopology() == TPL_INFINITE_MIN ? -DBL_MAX : *u_values.begin ();
		maxu = GetUTopology() <= TPL_INFINITE || GetUTopology() == TPL_INFINITE_MAX ? +DBL_MAX : *u_values.rbegin();

		minv = GetVTopology() <= TPL_INFINITE || GetVTopology() == TPL_INFINITE_MIN ? -DBL_MAX : *v_values.begin ();
		maxv = GetVTopology() <= TPL_INFINITE || GetVTopology() == TPL_INFINITE_MAX ? +DBL_MAX : *v_values.rbegin();
	}

public:
	ATTRIB_GROUP*		attributes_groups; // $p
	SURFACE_OWNER*		owner; // $p
	SURFACE*			next; // $p
	SURFACE*			previous; // $p
	GEOMETRIC_OWNER*	geometric_owner; // $p
	char				sense; // $c

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	const TParamValues& GetUValues() const
		{ return u_values; }

	const TParamValues& GetVValues() const
		{ return v_values; }

	double GetMinU() const
		{ assert(IsValid()); return minu; }

	double GetMaxU() const
		{ assert(IsValid()); return maxu; }

	double GetMinV() const
		{ assert(IsValid()); return minv; }

	double GetMaxV() const
		{ assert(IsValid()); return maxv; }

	double GetULength() const
		{ assert(IsValid()); assert(GetUTopology() >= TPL_OPEN); return maxu - minu; }

	double GetVLength() const
		{ assert(IsValid()); assert(GetVTopology() >= TPL_OPEN); return maxv - minv; }

	double NormalizeU(double u) const
		{ return NormalizeTplCoord(u, GetMinU(), GetMaxU(), GetUTopology()); }

	double NormalizeV(double v) const
		{ return NormalizeTplCoord(v, GetMinV(), GetMaxV(), GetVTopology()); }

	T_UV_Point NormalizeUV(T_UV_Point uv) const
	{
		uv.u = NormalizeU(uv.u);
		uv.v = NormalizeV(uv.v);

		if(IsPointCapped(uv.v))
		{
			assert(GetUTopology() >= TPL_OPEN);
			uv.u = (GetMinU() + GetMaxU()) * 0.5;
		}

		return uv;
	}

	bool IsPointCapped(double v) const
	{
		return	IsPointMinV() && !Compare(v, GetMinV()) ||
				IsPointMaxV() && !Compare(v, GetMaxV());
	}

	vector GetRealNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return GetRealNoSenseNormal(uv, u_bias, v_bias) * (sense == '-' ? -1 : +1); }

	vector GetApproxNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return GetApproxNoSenseNormal(uv, u_bias, v_bias) * (sense == '-' ? -1 : +1); }

	virtual TTopology GetUTopology() const = 0;
	
	virtual TTopology GetVTopology() const = 0;

	virtual bool IsPointMinV() const
		{ return false; }

	virtual bool IsPointMaxV() const
		{ return false; }

	virtual vector GetRealXYZ(const T_UV_Point& uv) const = 0;

	virtual vector GetApproxXYZ(const T_UV_Point& uv) const
		{ return GetRealXYZ(uv); }

	virtual vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const = 0;

	virtual vector GetApproxNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return GetRealNoSenseNormal(uv, u_bias, v_bias); }

	virtual T_UV_Point GetApproxUV(const vector& xyz) const = 0;
};

// --------------------
// Curve approximation
// --------------------
struct TCurveApproximation
{
public:
	TApproxParamValues m_t;
	std::vector<vector> m_p;
	std::vector<vector> m_tan[2];

public:
	void Clear()
	{
		m_t.clear();
		m_p.clear();
		m_tan[0].clear();
		m_tan[1].clear();
	}

	void AddPoint(double t, const vector& p, const vector& tan1, const vector& tan2)
	{
		assert(m_t.empty() || Compare(t, m_t.rbegin()->first) > 0);

		m_t.insert(TApproxParamValues::value_type(t, (unsigned)m_t.size()));
		m_p.push_back(p);
		m_tan[0].push_back(tan1);
		m_tan[1].push_back(tan2);
	}

	void Build(const ANY_CURVE* pCurve);

	vector GetXYZ(double t) const;

	vector GetTangent(double t, int t_bias = 0) const;

	double GetT(const vector& p) const;

	const TApproxParamValues& GetTValues() const
		{ return m_t; }
};

// ----------------------
// Surface approximation
// ----------------------
struct TSurfaceApproximation
{
public:
	TApproxParamValues m_u;
	TApproxParamValues m_v;
	std::vector<vector> m_p;
	std::vector<vector> m_norm[2][2];
	std::vector<matrix> m_trans1;
	std::vector<matrix> m_trans2;

public:
	void Build(const ANY_SURF* pSurface);

	vector GetXYZ(const T_UV_Point& uv) const;

	vector GetNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const;

	T_UV_Point GetUV(const vector& p) const;

	const TApproxParamValues& GetUValues() const
		{ return m_u; }

	const TApproxParamValues& GetVValues() const
		{ return m_v; }
};

// -----
// LINE
// -----
struct LINE : ANY_CURVE
{
protected:
	void LoadData(T_XT_Reader& Reader);	

public:
	vector pvec; // $v
	vector direction; // $v

public:
	TTopology GetTopology() const
		{ return TPL_INFINITE; }

	vector GetRealXYZ(double t) const
		{ return pvec + direction * t; }

	vector GetRealNoSenseTangent(double t, int t_bias = 0) const
		{ return direction; }

	double GetApproxT(const vector& xyz) const
		{ return direction & (xyz - pvec); }
};

// -------
// CIRCLE
// -------
struct CIRCLE : ANY_CURVE
{
protected:
	void LoadData(T_XT_Reader& Reader);	

	bool InternalPrepare(double dMaxAngle);

private:
	vector y_axis;
	matrix inv_transform;

public:
	vector centre; // $v
	vector normal; // $v
	vector x_axis; // $v
	double radius; // $f

public:
	TTopology GetTopology() const
		{ return TPL_PERIODIC; }

	vector GetRealXYZ(double t) const
		{ return centre + (x_axis * cos(t) + y_axis * sin(t)) * radius; }

	vector GetRealNoSenseTangent(double t, int t_bias = 0) const
		{ return x_axis * -sin(t) + y_axis * cos(t); }

	double GetApproxT(const vector& xyz) const
		{ return Get2D_Angle(inv_transform * (xyz - centre)); }
};

// --------
// ELLIPSE
// --------
struct ELLIPSE : ANY_CURVE
{
protected:
	void LoadData(T_XT_Reader& Reader);	

	bool InternalPrepare(double dMaxAngle);

private:
	vector y_axis;
	matrix inv_transform;

public:
	vector centre; // $v
	vector normal; // $v
	vector x_axis; // $v
	double major_radius; // $f
	double minor_radius; // $f

public:
	TTopology GetTopology() const
		{ return TPL_PERIODIC; }

	vector GetRealXYZ(double t) const
		{ return centre + x_axis * (major_radius * cos(t)) + y_axis * (minor_radius * sin(t)); }

	vector GetRealNoSenseTangent(double t, int t_bias = 0) const
		{ return (x_axis * (major_radius * -sin(t)) + y_axis * (minor_radius * cos(t))).Normalized(); }

	double GetApproxT(const vector& xyz) const
		{ return Get2D_Angle(inv_transform * (xyz - centre)); }
};

// ------------
// NURBS_CURVE
// ------------
struct NURBS_CURVE : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

public:
	short				degree; // $n
	int					n_vertices; // $d
	short				vertex_dim; // $n
	int					n_knots; // $d
	SCH_knot_type		knot_type; // $u
	logical				periodic; // $l
	logical				closed; // $l
	logical				rational; // $l
	SCH_curve_form		curve_form; // $u
	BSPLINE_VERTICES*	bspline_vertices; // $p
	KNOT_MULT*			knot_mult; // $p
	KNOT_SET*			knots; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -----------------
// BSPLINE_VERTICES
// -----------------
struct BSPLINE_VERTICES : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<double> vertices; // $f[]
};

// ---------
// KNOT_SET
// ---------
struct KNOT_SET : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<double> knots; // $f[]
};

// ----------
// KNOT_MULT
// ----------
struct KNOT_MULT : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<short> mult; // $n[]
};

// -----------
// CURVE_DATA
// -----------
struct CURVE_DATA : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	SCH_self_int	self_int; // $u
	HELIX_CU_FORM*	analytic_form; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// --------------
// HELIX_CU_FORM
// --------------
struct HELIX_CU_FORM : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	vector		axis_pt; // $v
	vector		axis_dir; // $v
	vector		point; // $v
	char		hand; // $c
	interval	turns; // $i
	double		pitch; // $f
	double		tol; // $f
};

// --------
// B_CURVE
// --------
struct B_CURVE : ANY_CURVE
{
private:
	void ParseVertex(unsigned i, vector& v, double& w) const;

protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

	void InternalPostPrepare();

private:
	vector GetXYZdt(double t, size_t bi) const;

private:	
	std::vector<double> exp_t_knots;

	TApproxParamValues valid_t_segs;

	TCurveApproximation approx;

public:
	NURBS_CURVE*	nurbs; // $p
	CURVE_DATA*		data; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	unsigned GetDimensions() const
		{ return nurbs->rational ? nurbs->vertex_dim - 1 : nurbs->vertex_dim; }

	TTopology GetTopology() const
		{ return nurbs->periodic ? TPL_PERIODIC : nurbs->closed ? TPL_CLOSED : TPL_OPEN; }

	vector GetRealXYZ(double t) const;

	vector GetApproxXYZ(double t) const
		{ return approx.GetXYZ(NormalizeT(t)); }

	vector GetRealNoSenseTangent(double t, int t_bias = 0) const;

	vector GetApproxNoSenseTangent(double t, int t_bias = 0) const
		{ return approx.GetTangent(NormalizeT(t), t_bias); }

	double GetApproxT(const vector& xyz) const
		{ return NormalizeT(approx.GetT(xyz)); }
};

// ------
// CHART
// ------
struct CHART : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

public:
	double				base_parameter; // $f
	double				base_scale; // $f
	int					chart_count; // $d
	double				chordal_error; // $f
	double				angular_error; // $f
	double				parameter_error[2]; // $f[2]
	std::vector<hvec>	vec; // $h[]
};

// ------
// LIMIT
// ------
struct LIMIT : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	char				type; // $c
	std::vector<hvec>	Hvec; // $h[]
};

// -------------
// INTERSECTION
// -------------
struct INTERSECTION : ANY_CURVE
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

private:
	TCurveApproximation approx;

public:
	SURFACE*	surface[2]; // $p[2]
	CHART*		chart; // $p
	LIMIT*		start; // $p
	LIMIT*		end; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetTopology() const
		{ return start->type == 'H' ? TPL_CLOSED : TPL_OPEN; }

	vector GetRealXYZ(double t) const
		{ return approx.GetXYZ(NormalizeT(t)); }

	vector GetRealNoSenseTangent(double t, int t_bias = 0) const
	{
		return (surface[0]->GetApproxNormal(surface[0]->GetApproxUV(GetRealXYZ(t))) *
				surface[1]->GetApproxNormal(surface[1]->GetApproxUV(GetRealXYZ(t)))).Normalized();
	}

	double GetApproxT(const vector& xyz) const
		{ return NormalizeT(approx.GetT(xyz)); }
};

// --------------
// TRIMMED_CURVE
// --------------
struct TRIMMED_CURVE : ANY_CURVE
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

public:
	CURVE* basis_curve; // $p
	vector point_1; // $v
	vector point_2; // $v
	double parm_1; // $f
	double parm_2; // $f

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

	TTopology GetTopology() const
	{
		return	basis_curve->GetTopology() >= TPL_PERIODIC &&
				!Compare(parm_2 - parm_1, basis_curve->GetTLength()) ?
					TPL_CLOSED : TPL_OPEN;
	}

	vector GetRealXYZ(double t) const
		{ return basis_curve->GetRealXYZ(t); }

	vector GetApproxXYZ(double t) const
		{ return basis_curve->GetApproxXYZ(t); }

	vector GetRealNoSenseTangent(double t, int t_bias = 0) const
		{ return basis_curve->GetRealNoSenseTangent(t, t_bias); }

	vector GetApproxNoSenseTangent(double t, int t_bias = 0) const
		{ return basis_curve->GetApproxNoSenseTangent(t, t_bias); }

	double GetApproxT(const vector& xyz) const;
};

// ---------
// PE_CURVE
// ---------
struct PE_CURVE : ANY_CURVE
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle)
		{ return false; }

public:
	char						type; // $c
	PE_DATA*					data; // $p
	TRANSFORM*					tf; // $p
	std::vector<PE_INT_GEOM*>	internal_geom; // $p[]

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetTopology() const
		{ return TPL_INFINITE; }

	vector GetRealXYZ(double t) const
		{ return vector(0, 0, 0); }

	vector GetRealNoSenseTangent(double t, int t_bias = 0) const
		{ return vector(0, 0, 1); }

	double GetApproxT(const vector& xyz) const
		{ return 0; }
};

// ------------
// EXT_PE_DATA
// ------------
struct EXT_PE_DATA : virtual NODE, PE_DATA
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	KEY*			key; // $p
	REAL_VALUES*	real_array; // $p
	INT_VALUES*		int_array; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ------------
// INT_PE_DATA
// ------------
struct INT_PE_DATA : virtual NODE, PE_DATA
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	int				geom_type; // $d
	REAL_VALUES*	real_array; // $p
	INT_VALUES*		int_array; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ---------
// SP_CURVE
// ---------
struct SP_CURVE : ANY_CURVE
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

	void InternalPostPrepare();

private:
	TTopology topology;

	TCurveApproximation approx;

public:
	SURFACE*	surface; // $p
	B_CURVE*	b_curve; // $p
	CURVE*		original; // $p
	double		tolerance_to_original; // $f

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetTopology() const
		{ return topology; }

	vector GetRealXYZ(double t) const
		{ return surface->GetRealXYZ(surface->NormalizeUV(XYZasUV(b_curve->GetRealXYZ(t)))); }

	vector GetApproxXYZ(double t) const
		{ return approx.GetXYZ(NormalizeT(t)); }

	vector GetRealNoSenseTangent(double t, int t_bias = 0) const; // approximate

	vector GetApproxNoSenseTangent(double t, int t_bias = 0) const
		{ return approx.GetTangent(NormalizeT(t), t_bias); }

	double GetApproxT(const vector& xyz) const
		{ return approx.GetT(xyz); }
};

// ------
// PLANE
// ------
struct PLANE : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);	

	bool InternalPrepare(double dMaxAngle);

private:
	vector y_axis;
	matrix inv_transform;

public:
	vector pvec; // $v
	vector normal; // $v
	vector x_axis; // $v

public:
	TTopology GetUTopology() const
		{ return TPL_INFINITE; }
	
	TTopology GetVTopology() const
		{ return TPL_INFINITE; }

	vector GetRealXYZ(const T_UV_Point& uv) const
		{ return pvec + x_axis * uv.u + y_axis * uv.v; }

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return normal; }

	T_UV_Point GetApproxUV(const vector& xyz) const
		{ return XYZasUV(inv_transform * (xyz - pvec)); }
};

// ---------
// CYLINDER
// ---------
struct CYLINDER : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

private:
	vector y_axis;
	matrix inv_transform;

public:
	vector pvec; // $v
	vector axis; // $v
	double radius; // $f
	vector x_axis; // $v

public:
	TTopology GetUTopology() const
		{ return TPL_PERIODIC; }

	TTopology GetVTopology() const
		{ return TPL_INFINITE; }

	vector GetRealXYZ(const T_UV_Point& uv) const
		{ return pvec + (x_axis * cos(uv.u) + y_axis * sin(uv.u)) * radius + axis * uv.v; }

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return x_axis * cos(uv.u) + y_axis * sin(uv.u); };

	T_UV_Point GetApproxUV(const vector& xyz) const
	{
		vector p = inv_transform * (xyz - pvec);

		return NormalizeUV(T_UV_Point(Get2D_Angle(p.x, p.y), p.z));
	}
};

// -----
// CONE
// -----
struct CONE : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

private:
	vector y_axis;
	matrix inv_transform;
	double tan_half_angle;

public:
	vector pvec; // $v
	vector axis; // $v
	double radius; // $f
	double sin_half_angle; // $f
	double cos_half_angle; // $f
	vector x_axis; // $v

public:
	TTopology GetUTopology() const
		{ return TPL_PERIODIC; }

	TTopology GetVTopology() const
		{ return TPL_INFINITE_MAX; }

	bool IsPointMinV() const
		{ return true; }

	vector GetRealXYZ(const T_UV_Point& uv) const
		{ return pvec - axis * uv.v + (x_axis * cos(uv.u) + y_axis * sin(uv.u)) * (radius + uv.v * tan_half_angle); }

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
	{
		if(!Compare(uv.v, GetMinV()))
			return vector(0, 0, 0);

		return -((x_axis * cos(uv.u) + y_axis * sin(uv.u)) * cos_half_angle + axis * sin_half_angle);
	}

	T_UV_Point GetApproxUV(const vector& xyz) const
	{
		vector p = inv_transform * (xyz - pvec);

		double l = sqrt(p.x * p.x + p.y * p.y);

		double v = GetMinV() + ((-p.z - GetMinV()) * cos_half_angle + l * sin_half_angle) * cos_half_angle;

		return NormalizeUV(T_UV_Point(Get2D_Angle(p.x, p.y), v));
	}
};

// -------
// SPHERE
// -------
struct SPHERE : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

private:
	vector y_axis;
	matrix inv_transform;

public:
	vector centre; // $v
	double radius; // $f
	vector axis; // $v
	vector x_axis; // $v

public:
	TTopology GetUTopology() const
		{ return TPL_PERIODIC; }

	TTopology GetVTopology() const
		{ return TPL_OPEN; }

	bool IsPointMinV() const
		{ return true; }

	bool IsPointMaxV() const
		{ return true; }

	vector GetRealXYZ(const T_UV_Point& uv) const
		{ return centre + ((x_axis * cos(uv.u) + y_axis * sin(uv.u)) * cos(uv.v) + axis * sin(uv.v)) * radius; }

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
	{
		if(!Compare(uv.v, GetMinV()))
			return -axis * (sense == '-' ? -1 : +1);

		if(!Compare(uv.v, GetMaxV()))
			return +axis * (sense == '-' ? -1 : +1);

		return (x_axis * cos(uv.u) + y_axis * sin(uv.u)) * cos(uv.v) + axis * sin(uv.v);
	}

	T_UV_Point GetApproxUV(const vector& xyz) const
	{
		vector p = inv_transform * (xyz - centre);

		return NormalizeUV(T_UV_Point(Get2D_Angle(p.x, p.y), asin(Limit(SafeDiv(p.z, p.GetLength()), -1.0, +1.0))));
	}
};

// ------
// TORUS
// ------
struct TORUS : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

private:
	vector y_axis;
	matrix inv_transform;

public:
	vector centre; // $v
	vector axis; // $v
	double major_radius; // $f
	double minor_radius; // $f
	vector x_axis; // $v

public:
	TTopology GetUTopology() const
		{ return TPL_PERIODIC; }

	TTopology GetVTopology() const
		{ return TPL_PERIODIC; }

	vector GetRealXYZ(const T_UV_Point& uv) const
		{ return centre + (x_axis * cos(uv.u) + y_axis * sin(uv.u)) * (major_radius + minor_radius * cos(uv.v)) + axis * (minor_radius * sin(uv.v)); }

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return (x_axis * cos(uv.u) + y_axis * sin(uv.u)) * cos(uv.v) + axis * sin(uv.v); }

	T_UV_Point GetApproxUV(const vector& xyz) const
	{
		vector p = (inv_transform * (xyz - centre));

		double l = sqrt(p.x * p.x + p.y * p.y);

		return NormalizeUV(T_UV_Point(Get2D_Angle(p.x, p.y), Get2D_Angle(l - major_radius, p.z)));
	}
};

// -------------
// BLENDED_EDGE
// -------------
struct BLENDED_EDGE : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

public:
	char		blend_type; // $c
	SURFACE*	surface[2]; // $p[2]
	CURVE*		spine; // $p
	double		range[2]; // $f[2]
	double		thumb_weight[2]; // $f[2]
	SURFACE*	boundary[2]; // $p[2]
	LIMIT*		start; // $p
	LIMIT*		end; // $p

public:
	struct TNodeInfo
	{
	public:
		vector spine_xyz;
		vector spine_tg;
		vector x_axis;
		vector y_axis;
		matrix inv_transform;
		T_UV_Point surf_uv	[2];
		vector surf_xyz		[2];
		vector surf_normal	[2];
		double surf_ang		[2];		

	public:
		double GetAngle(double v, char cSpineSense) const
		{
			return	cSpineSense == '-' ?
						surf_ang[1] + (surf_ang[0] - surf_ang[1]) * v :
						surf_ang[0] + (surf_ang[1] - surf_ang[0]) * v;
		}

		double GetV(double ang, char cSpineSense) const
		{
			if(Sign(ang -= surf_ang[0]) < 0)
				ang += g_dPI * 2;

			double v = SafeDiv(ang, surf_ang[1] - surf_ang[0]);

			if(cSpineSense == '-')
				v = 1 - v;

			return v;
		}
	};	

private:
	typedef std::map<double, TNodeInfo, precision_less> TNodes;

	mutable TNodes nodes;

public:
	void GetNodeInfo(double u, TNodeInfo& RInfo) const;

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetUTopology() const
		{ return spine->GetTopology(); }

	TTopology GetVTopology() const
		{ return TPL_OPEN; }

	vector GetRealXYZ(const T_UV_Point& uv) const
	{
		TNodeInfo Info;

		GetNodeInfo(uv.u, Info);

		double ang = Info.GetAngle(uv.v, spine->sense);

		return Info.spine_xyz + (Info.x_axis * cos(ang) + Info.y_axis * sin(ang)) * fabs(range[0]);
	}

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
	{
		TNodeInfo Info;

		GetNodeInfo(uv.u, Info);

		double ang = Info.GetAngle(uv.v, spine->sense);

		return Info.x_axis * cos(ang) + Info.y_axis * sin(ang);
	}

	T_UV_Point GetApproxUV(const vector& xyz) const
	{
		T_UV_Point uv;

		uv.u = spine->GetApproxT(xyz);

		TNodeInfo Info;

		GetNodeInfo(uv.u, Info);

		uv.v = Info.GetV(Get2D_Angle(Info.inv_transform * (xyz - Info.spine_xyz)), spine->sense);

		return NormalizeUV(uv);
	}
};

// ------------
// BLEND_BOUND
// ------------
struct BLEND_BOUND : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

public:
	short			boundary; // $n
	BLENDED_EDGE*	blend; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetUTopology() const
		{ return blend->spine->GetTopology(); }

	TTopology GetVTopology() const
		{ return TPL_INFINITE; }

	vector GetRealXYZ(const T_UV_Point& uv) const
	{
		BLENDED_EDGE::TNodeInfo Info;

		blend->GetNodeInfo(uv.u, Info);

		return Info.surf_xyz[1 - boundary];
	}

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
	{
		BLENDED_EDGE::TNodeInfo Info;

		blend->GetNodeInfo(uv.u, Info);

		return (Info.spine_tg * Info.surf_normal[1 - boundary]).Normalized();
	}

	T_UV_Point GetApproxUV(const vector& xyz) const
		{ return T_UV_Point(blend->spine->GetApproxT(xyz), 0); }
};

// ------------
// OFFSET_SURF
// ------------
struct OFFSET_SURF : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

public:
	char		check; // $c
	logical		true_offset; // $l
	SURFACE*	surface; // $p
	double		offset; // $f
	double		scale; // $f

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetUTopology() const
		{ return surface->GetUTopology(); }

	TTopology GetVTopology() const
		{ return surface->GetVTopology(); }

	vector GetRealXYZ(const T_UV_Point& uv) const
		{ return surface->GetRealXYZ(uv) + surface->GetRealNormal(uv) * offset; }

	vector GetApproxXYZ(const T_UV_Point& uv) const
		{ return surface->GetApproxXYZ(uv) + surface->GetApproxNormal(uv) * offset; }

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return surface->GetRealNoSenseNormal(uv, u_bias, v_bias); }

	vector GetApproxNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return surface->GetApproxNoSenseNormal(uv, u_bias, v_bias); }

	T_UV_Point GetApproxUV(const vector& xyz) const
		{ return surface->GetApproxUV(xyz); } // there are cases when this is wrong, but they are not expected to occur
};

// -----------
// NURBS_SURF
// -----------
struct NURBS_SURF : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

public:
	logical				u_periodic; // $l
	logical				v_periodic; // $l
	short				u_degree; // $n
	short				v_degree; // $n
	int					n_u_vertices; // $d
	int					n_v_vertices; // $d
	SCH_knot_type		u_knot_type; // $u
	SCH_knot_type		v_knot_type; // $u
	int					n_u_knots; // $d
	int					n_v_knots; // $d
	logical				rational; // $l
	logical				u_closed; // $l
	logical				v_closed; // $l
	SCH_surface_form	surface_form; // $u
	short				vertex_dim; // $n
	BSPLINE_VERTICES*	bspline_vertices; // $p
	KNOT_MULT*			u_knot_mult; // $p
	KNOT_MULT*			v_knot_mult; // $p
	KNOT_SET*			u_knots; // $p
	KNOT_SET*			v_knots; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -------------
// SURFACE_DATA
// -------------
struct SURFACE_DATA : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	interval		original_uint; // $i
	interval		original_vint; // $i
	interval		extended_uint; // $i
	interval		extended_vint; // $i
	SCH_self_int	self_int; // $u
	char			original_u_start; // $c
	char			original_u_end; // $c
	char			original_v_start; // $c
	char			original_v_end; // $c
	char			extended_u_start; // $c
	char			extended_u_end; // $c
	char			extended_v_start; // $c
	char			extended_v_end; // $c
	char			analytic_form_type; // $c
	char			swept_form_type; // $c
	char			spun_form_type; // $c
	char			blend_form_type; // $c
	NODE*			analytic_form; // $p
	NODE*			swept_form; // $p
	NODE*			spun_form; // $p
	NODE*			blend_form; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// --------------
// HELIX_SU_FORM
// --------------
struct HELIX_SU_FORM : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	vector		axis_pt; // $v
	vector		axis_dir; // $v
	char		hand; // $c
	interval	turns; // $i
	double		pitch; // $f
	double		gap; // $f
	double		tol; // $f
};

// ----------
// B_SURFACE
// ----------
struct B_SURFACE : ANY_SURF
{
private:
	void ParseVertex(unsigned i, unsigned j, vector& v, double& w) const;

protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

	void InternalPostPrepare();

private:
	vector GetXYZdu(const T_UV_Point& uv, size_t bi, size_t bj) const;
	vector GetXYZdv(const T_UV_Point& uv, size_t bi, size_t bj) const;

private:
	std::vector<double> exp_u_knots;
	std::vector<double> exp_v_knots;

	TApproxParamValues valid_u_segs;
	TApproxParamValues valid_v_segs;

	TSurfaceApproximation approx;

public:
	NURBS_SURF*		nurbs; // $p
	SURFACE_DATA*	data; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	unsigned GetDimensions() const
		{ return nurbs->rational ? nurbs->vertex_dim - 1 : nurbs->vertex_dim; }

	TTopology GetUTopology() const
		{ return nurbs->u_periodic ? TPL_PERIODIC : nurbs->u_closed ? TPL_CLOSED : TPL_OPEN; }

	TTopology GetVTopology() const
		{ return nurbs->v_periodic ? TPL_PERIODIC : nurbs->v_closed ? TPL_CLOSED : TPL_OPEN; }

	vector GetRealXYZ(const T_UV_Point& uv) const;

	vector GetApproxXYZ(const T_UV_Point& uv) const
		{ return approx.GetXYZ(NormalizeUV(uv)); }

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const;

	vector GetApproxNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return approx.GetNormal(uv, u_bias, v_bias); }

	T_UV_Point GetApproxUV(const vector& xyz) const // approximate
		{ return NormalizeUV(approx.GetUV(xyz)); }
};

// -----------
// SWEPT_SURF
// -----------
struct SWEPT_SURF : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

private:
	matrix fwd_transform;
	matrix inv_transform;
	TCurveApproximation approx_xy; // also holds 'approx_xy + approx_z' tangents
	TCurveApproximation approx_z;

public:
	CURVE* section; // $p
	vector sweep; // $v
	double scale; // $f

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetUTopology() const
		{ return section->GetTopology(); }

	TTopology GetVTopology() const
		{ return TPL_INFINITE; }

	vector GetRealXYZ(const T_UV_Point& uv) const
		{ return section->GetRealXYZ(uv.u) + sweep * uv.v; }

	vector GetApproxXYZ(const T_UV_Point& uv) const
	{
		double u = NormalizeU(uv.u);

		return fwd_transform * (approx_xy.GetXYZ(u) + approx_z.GetXYZ(u) + vector(0, 0, uv.v));
	}

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return (section->GetRealTangent(uv.u, u_bias) * sweep).Normalized(); }

	vector GetApproxNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
	{
		double u = NormalizeU(uv.u);

		return (approx_xy.GetTangent(u, u_bias) * sweep).Normalized();
	}

	T_UV_Point GetApproxUV(const vector& xyz) const // approximate
	{
		T_UV_Point uv;

		vector p = inv_transform * xyz;

		uv.u = approx_xy.GetT(vector(p.x, p.y, 0));
		uv.v = p.z - approx_z.GetXYZ(uv.u).z;

		return NormalizeUV(uv);
	}
};

// ----------
// SPUN_SURF
// ----------
struct SPUN_SURF : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle);

private:
	TTopology u_topology;

	matrix fwd_transform;
	matrix inv_transform;

	TSurfaceApproximation approx;

public:
	CURVE* profile; // $p
	vector base; // $v
	vector axis; // $v
	vector start; // $v
	vector end; // $v
	double start_param; // $f
	double end_param; // $f
	vector x_axis; // $v
	double scale; // $f

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetUTopology() const
		{ return u_topology; }

	TTopology GetVTopology() const
		{ return TPL_PERIODIC; }

	vector GetRealXYZ(const T_UV_Point& uv) const
	{
		vector v = profile->GetRealXYZ(NormalizeU(uv.u));

		double al = (v - base) & axis;

		vector p = base + axis * al;

		vector x_axis = v - p;
		vector y_axis = axis * x_axis;

		return p + x_axis * cos(uv.v) + y_axis * sin(uv.v);
	}

	vector GetApproxXYZ(const T_UV_Point& uv) const
	{
		// {{{ return from approx curve

		return approx.GetXYZ(NormalizeUV(uv));
	}

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
	{
		// Getting normal at v = 0
		vector v = profile->GetRealXYZ(uv.u);

		double al = (v - base) & axis;

		vector p = base + axis * al;

		vector Normal = (profile->GetRealNoSenseTangent(uv.u, u_bias) * (axis * (v - p))).Normalized();

		// Rotating normal 'v' radians in XY plane (-v radians around 'axis' counter-clockwise direction)
		matrix Rotation;
		Rotation.SetRotateZ(-uv.v);

		return fwd_transform * (Rotation * (inv_transform * Normal));
	}

	vector GetApproxNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0)
	{
		// {{{ return from curve approx
		return approx.GetNormal(NormalizeUV(uv), u_bias, v_bias);
	}

	T_UV_Point GetApproxUV(const vector& xyz) const // approximate
		{ return NormalizeUV(approx.GetUV(xyz)); }
};

// --------
// PE_SURF
// --------
struct PE_SURF : ANY_SURF
{
protected:
	void LoadData(T_XT_Reader& Reader);

	bool InternalPrepare(double dMaxAngle)
		{ return false; }

public:
	char						type; // $c
	PE_DATA*					data; // $p
	TRANSFORM*					tf; // $p
	std::vector<PE_INT_GEOM*>	internal_geom; // $p[]

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

public:
	TTopology GetUTopology() const
		{ return TPL_INFINITE; }

	TTopology GetVTopology() const
		{ return TPL_INFINITE; }

	vector GetRealXYZ(const T_UV_Point& uv) const
		{ return vector(0, 0, 0); }

	vector GetRealNoSenseNormal(const T_UV_Point& uv, int u_bias = 0, int v_bias = 0) const
		{ return vector(0, 0, 1); }

	T_UV_Point GetApproxUV(const vector& xyz) const
		{ return T_UV_Point(0, 0); }
};

// ------
// POINT
// ------
struct POINT : virtual NODE_WITH_ID, GEOMETRY, ATTRIBUTE_OWNER, GROUP_MEMBER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*	attributes_groups; // $p
	POINT_OWNER*	owner; // $p
	POINT*			next; // $p
	POINT*			previous; // $p
	vector			pvec; // $v

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ----------
// TRANSFORM
// ----------
struct TRANSFORM : virtual NODE_WITH_ID, GEOMETRY
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	TRANSFORM_OWNER*	owner; // $p
	TRANSFORM*			next; // $p
	TRANSFORM*			previous; // $p
	matrix				rotation_matrix; // $f[9]
	vector				translation_vector; // $v
	double				scale; // $f
	int					flag; // $d
	vector				perspective_vector; // $v

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

	TRANSFORM operator * (const TRANSFORM& Transform) const;
};

// ----------------
// GEOMETRIC_OWNER
// ----------------
struct GEOMETRIC_OWNER : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	GEOMETRY*			owner; // $p
	GEOMETRIC_OWNER*	next; // $p
	GEOMETRIC_OWNER*	previous; // $p
	GEOMETRY*			shared_geometry; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ------
// WORLD
// ------
struct WORLD : virtual NODE, virtual RENDERABLE, CURVE_OWNER, SURFACE_OWNER, POINT_OWNER, TRANSFORM_OWNER, LIST_OWNER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ASSEMBLY*	assembly; // $p
	ATTRIBUTE*	attribute; // $p
	BODY*		body; // $p
	TRANSFORM*	transform; // $p
	SURFACE*	surface; // $p
	CURVE*		curve; // $p
	POINT*		point; // $p
	logical		alive; // $l
	ATTRIB_DEF*	attrib_def; // $p
	int			highest_id; // $d
	int			current_id; // $d

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

	void Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth = 0) const;
};

// ---------
// ASSEMBLY
// ---------
struct ASSEMBLY : virtual NODE, virtual RENDERABLE, CURVE_OWNER, SURFACE_OWNER, POINT_OWNER, PART, LIST_OWNER, ATTRIBUTE_OWNER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	int					highest_node_id; // $d
	ATTRIB_GROUP*		attributes_groups; // $p
	LIST*				attribute_chains; // $p
	LIST*				list; // $p
	SURFACE*			surface; // $p
	CURVE*				curve; // $p
	POINT*				point; // $p
	KEY*				key; // $p
	double				res_size; // $f
	double				res_linear; // $f
	INSTANCE*			ref_instance; // $p
	ASSEMBLY*			next; // $p
	ASSEMBLY*			previous; // $p
	SCH_part_state		state; // $u
	WORLD*				owner; // $p
	SCH_assembly_type	type; // $u
	INSTANCE*			sub_instance; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

	void Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth = 0) const;
};

// ----
// KEY
// ----
struct KEY : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<char> string; // $c[]
};

// ---------
// INSTANCE
// ---------
struct INSTANCE : virtual NODE_WITH_ID, TRANSFORM_OWNER, ATTRIBUTE_OWNER, GROUP_MEMBER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*		attributes_groups; // $p
	SCH_instance_type	type; // $u
	PART*				part; // $p
	TRANSFORM*			transform; // $p
	ASSEMBLY*			assembly; // $p
	INSTANCE*			next_in_part; // $p
	INSTANCE*			prev_in_part; // $p
	INSTANCE*			next_of_part; // $p
	INSTANCE*			prev_of_part; // $p	

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -----
// BODY
// -----
struct BODY : virtual NODE, virtual RENDERABLE, CURVE_OWNER, SURFACE_OWNER, POINT_OWNER, PART, SHELL_OR_BODY, LIST_OWNER, ATTRIBUTE_OWNER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	int					highest_node_id; // $d
	ATTRIB_GROUP*		attributes_groups; // $p
	LIST*				attribute_chains; // $p
	SURFACE*			surface; // $p
	CURVE*				curve; // $p
	POINT*				point; // $p
	KEY*				key; // $p
	double				res_size; // $f
	double				res_linear; // $f
	INSTANCE*			ref_instance; // $p
	BODY*				next; // $p
	BODY*				previous; // $p
	SCH_part_state		state; // $u
	WORLD*				owner; // $p
	SCH_body_type		body_type; // $u
	SCH_nom_geom_state	nom_geom_state; // $u
	SHELL*				shell; // $p
	SURFACE*			boundary_surface; // $p
	CURVE*				boundary_curve; // $p
	POINT*				boundary_point; // $p
	REGION*				region; // $p
	EDGE*				edge; // $p
	VERTEX*				vertex; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

	void Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth = 0) const;
};

// -------
// REGION
// -------
struct REGION : virtual NODE_WITH_ID, ATTRIBUTE_OWNER, GROUP_MEMBER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*	attributes_groups; // $p
	BODY*			body; // $p
	REGION*			next; // $p
	REGION*			previous; // $p
	SHELL*			shell; // $p
	char			type; // $c

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ------
// SHELL
// ------
struct SHELL : virtual NODE_WITH_ID, SHELL_OR_BODY, ATTRIBUTE_OWNER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*	attributes_groups; // $p
	BODY*			body; // $p
	SHELL*			next; // $p
	FACE*			face; // $p
	EDGE*			edge; // $p
	VERTEX*			vertex; // $p
	REGION*			region; // $p
	FACE*			front_face; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -----
// FACE
// -----
struct FACE : virtual NODE_WITH_ID, SURFACE_OWNER, ATTRIBUTE_OWNER, GROUP_MEMBER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*	attributes_groups; // $p
	double			tolerance; // $f
	FACE*			next; // $p
	FACE*			previous; // $p
	LOOP*			loop; // $p
	SHELL*			shell; // $p
	SURFACE*		surface; // $p
	char			sense; // $c
	FACE*			next_on_surface; // $p
	FACE*			previous_on_surface; // $p
	FACE*			next_front; // $p
	FACE*			previous_front; // $p
	SHELL*			front_shell; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -----
// LOOP
// -----
struct LOOP : virtual NODE_WITH_ID, ATTRIBUTE_OWNER, GROUP_MEMBER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*	attributes_groups; // $p
	FIN*			fin; // $p
	FACE*			face; // $p
	LOOP*			next; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ----
// FIN
// ----
struct FIN : virtual NODE, CURVE_OWNER, ATTRIBUTE_OWNER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*	attributes_groups; // $p
	LOOP*			loop; // $p
	FIN*			forward; // $p
	FIN*			backward; // $p
	VERTEX*			vertex; // $p
	FIN*			other; // $p
	EDGE*			edge; // $p
	CURVE*			curve; // $p
	FIN*			next_at_vx; // $p
	char			sense; // $c

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -------
// VERTEX
// -------
struct VERTEX : virtual NODE_WITH_ID, POINT_OWNER, ATTRIBUTE_OWNER, GROUP_MEMBER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*	attributes_groups; // $p
	FIN*			fin; // $p
	VERTEX*			previous; // $p
	VERTEX*			next; // $p
	POINT*			point; // $p
	double			tolerance; // $f
	SHELL_OR_BODY*	owner; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -----
// EDGE
// -----
struct EDGE : virtual NODE_WITH_ID, CURVE_OWNER, ATTRIBUTE_OWNER, GROUP_MEMBER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*	attributes_groups; // $p
	double			tolerance; // $f
	FIN*			fin; // $p
	EDGE*			previous; // $p
	EDGE*			next; // $p
	CURVE*			curve; // $p
	EDGE*			next_on_curve; // $p
	EDGE*			previous_on_curve; // $p
	SHELL_OR_BODY*	owner; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -----
// LIST
// -----
struct LIST : virtual NODE_WITH_ID
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	LIST_OWNER*	owner; // $p
	LIST*		next; // $p
	LIST*		previous; // $p
	LIS_type	list_type; // $d
	int			list_length; // $d
	int			block_length; // $d
	int			size_of_entry; // $d
	LIS_BLOCK*	list_block; // $p
	LIS_BLOCK*	finger_block; // $p
	int			finger_index; // $d
	logical		notransmit; // $l

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ------------------
// POINTER_LIS_BLOCK
// ------------------
struct POINTER_LIS_BLOCK : virtual NODE, virtual RENDERABLE, LIS_BLOCK
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	int					n_entries; // $d
	POINTER_LIS_BLOCK*	next_block; // $p
	std::vector<NODE*>	entries; // $p[]

public:
	void ResolvePtrs(const TNodeIndexMap& Map);

	void Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth = 0) const;
};

// -----------
// ATT_DEF_ID
// -----------
struct ATT_DEF_ID : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<char> String; // $c[]
};

// ------------
// FIELD_NAMES
// ------------
struct FIELD_NAMES : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<FIELD_NAME*> names; // $p[]

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -----------
// ATTRIB_DEF
// -----------
struct ATTRIB_DEF : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_DEF*					next; // $p
	ATT_DEF_ID*					identifier; // $p
	int							type_id; // $d
	SCH_action_on_fields		actions[(int)SCH_LE_max_logged_event];	// $u[8]
	std::vector<SCH_field_type>	fields; // $u[]
	logical						legal_owners[(int)SCH_AO_max_owner]; // $l[14]

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ----------
// ATTRIBUTE
// ----------
struct ATTRIBUTE : virtual NODE_WITH_ID, ATTRIB_GROUP
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_DEF*					definition; // $p
	ATTRIBUTE_OWNER*			owner; // $p
	ATTRIB_GROUP*				next; // $p
	ATTRIB_GROUP*				previous; // $p
	ATTRIBUTE*					next_of_type; // $p
	ATTRIBUTE*					previous_of_type; // $p
	std::vector<FIELD_VALUES*>	fields; // $p[]

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// -----------
// INT_VALUES
// -----------
struct INT_VALUES : virtual NODE, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<int> values; // $d[]
};

// ------------
// REAL_VALUES
// ------------
struct REAL_VALUES : virtual NODE, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<double> values; // $f[]
};

// ------------
// CHAR_VALUES
// ------------
struct CHAR_VALUES : virtual NODE, FIELD_NAME, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<char> values; // $c[]
};

// ---------------
// UNICODE_VALUES
// ---------------
struct UNICODE_VALUES : virtual NODE, FIELD_NAME, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<short> values; // $w[]
};

// -------------
// SHORT_VALUES
// -------------
struct SHORT_VALUES : virtual NODE
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<short> values; // $n[]
};

// -------------
// POINT_VALUES
// -------------
struct POINT_VALUES : virtual NODE, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<vector> values; // $v[]
};

// -------------
// VECTOR_VALUES
// -------------
struct VECTOR_VALUES : virtual NODE, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<vector> values; // $v[]
};

// -----------------
// DIRECTION_VALUES
// -----------------
struct DIRECTION_VALUES : virtual NODE, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<vector> values; // $v[]
};

// ------------
// AXIS_VALUES
// ------------
struct AXIS_VALUES : virtual NODE, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<vector> values; // $v[]
};

// -----------
// TAG_VALUES
// -----------
struct TAG_VALUES : virtual NODE, FIELD_VALUES
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	std::vector<int> values; // $d[]
};

// ------
// GROUP
// ------
struct GROUP : virtual NODE_WITH_ID, ATTRIB_GROUP, ATTRIBUTE_OWNER
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	ATTRIB_GROUP*		attributes_groups; // $p
	PART*				owner; // $p
	ATTRIB_GROUP*		next; // $p
	ATTRIB_GROUP*		previous; // $p
	SCH_group_type		type; // $u
	MEMBER_OF_GROUP*	first_member; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ----------------
// MEMBER_OF_GROUP
// ----------------
struct MEMBER_OF_GROUP : virtual NODE, ATTRIB_GROUP
{
protected:
	void LoadData(T_XT_Reader& Reader);

public:
	int					dummy_node_id; // $d
	GROUP*				owning_group; // $p
	GROUP_MEMBER*		owner; // $p
	ATTRIB_GROUP*		next; // $p
	ATTRIB_GROUP*		previous; // $p
	MEMBER_OF_GROUP*	next_member; // $p
	MEMBER_OF_GROUP*	previous_member; // $p

public:
	void ResolvePtrs(const TNodeIndexMap& Map);
};

// ----------------
// Global routines
// ----------------
void RefineCollinearEdges(std::vector<vector>& Faces);

// !!!
void SaveSTL_File(LPCTSTR pFileName, const std::vector<vector>& Faces);

}

#endif // parasolid_h
