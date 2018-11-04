#ifndef vector_h
#define vector_h

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

	TVector3() {}

	TVector3(const TVector3& SVector) : x(SVector.x), y(SVector.y), z(SVector.z) {}

	TVector3(t sx, t sy, t sz) : x(sx), y(sy), z(sz) {}

	TVector3(t v) : x(v), y(v), z(v) {}

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

	t GetLengthSquare	() const { return x*x + y*y + z*z;			}
	t GetLength			() const { return Sqrt(GetLengthSquare());	}

	TVector3& Normalize()
	{
		const t Length = GetLength();
		
		if(Compare(Length, (t)0))
			*this /= Length;
		
		return *this;
	}

	TVector3 Normalized() const { return TVector3(*this).Normalize(); }	

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
		{ return *this * (1.0f / c); }

	TVector3 operator / (const TVector3& SVector)
		{ return TVector3(x / SVector.x, y / SVector.y, z / SVector.z); }

	TVector3& operator = (const TVector3& SVector)
		{ x=SVector.x, y=SVector.y, z=SVector.z; return *this; }

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
		{ return *this *= (t)1 / c; }

	TVector3& operator /= (const TVector3& SVector)
		{ return *this = *this / SVector; }

	TVector3 operator + () const
		{ return *this; }

	TVector3 operator - () const
		{ return TVector3(-x, -y, -z); }

	// Sign states
	bool IsZero() const
		{ return Sign(x) == 0 && Sign(y) == 0 && Sign(z) == 0; }

	bool IsPositive() const
		{ return Sign(x) > 0 && Sign(y) > 0 && Sign(z) > 0; }

	bool IsNegative() const
		{ return Sign(x) < 0 && Sign(y) < 0 && Sign(z) < 0; }

	// Comparison operators
	bool operator == (const TVector3& SVector) const
	{
		return	Compare(x, SVector.x) == 0 &&
				Compare(y, SVector.y) == 0 &&
				Compare(z, SVector.z) == 0;
	}

	bool operator != (const TVector3& SVector) const
	{
		return	Compare(x, SVector.x) ||
				Compare(y, SVector.y) ||
				Compare(z, SVector.z);
	}

	inline operator KString () const
		{ return (KString)x + " "  + y + " " + z; }
};

template <class t>
inline bool FromString(KString s, TVector3<t>& RVector)
{
	FOR_EACH_STRING(s, i)
	{
		if(!isdigit(s[i]) && !strchr(".eE+-", s[i]))
			s.SetChar(i, ' ');
	}

	KStrings Elts(s, " ");

	if(Elts.GetN() < 3)
		return false;

	return	FromString(Elts[0], RVector.x) &&
			FromString(Elts[1], RVector.y) &&
			FromString(Elts[2], RVector.z);
}

// Debug operators
template <class t>
inline TDebugFile& operator << (TDebugFile& DebugFile, const TVector3<t>& Vector)
{
	return	DebugFile	<< TEXT("(") <<
			Vector.x	<< TEXT(";") <<
			Vector.y	<< TEXT(";") <<
			Vector.z	<< TEXT(")");
}

typedef TVector3<float>  TFVector3;
typedef TVector3<double> TDVector3;

typedef TSegment<TFVector3> TFVector3Segment;
typedef TSegment<TDVector3> TDVector3Segment;

template <class t>
inline TFVector3 TO_F(const TVector3<t>& v)
	{ return TFVector3((float)v.x, (float)v.y, (float)v.z); }

template <class t>
inline TDVector3 TO_D(const TVector3<t>& v)
	{ return TDVector3((double)v.x, (double)v.y, (double)v.z); }

template <class t>
inline TFVector3Segment TO_F(const TSegment< TVector3<t> >& s)
	{ return TFVector3Segment(TO_F(s.m_First), TO_F(s.m_Last)); }

template <class t>
inline TDVector3Segment TO_D(const TSegment< TVector3<t> >& s)
	{ return TDVector3Segment(TO_D(s.m_First), TO_D(s.m_Last)); }

#endif // vector_h
