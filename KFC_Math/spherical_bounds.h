#ifndef spherical_bounds_h
#define spherical_bounds_h

// -----------------
// Spherical bounds
// -----------------
struct TSphericalBounds
{
private:
	TDVector3 m_Center;

	double m_dRadius;
	double m_dRadius2;

public:
	TSphericalBounds()
		{ Reset(TDVector3().SetZero()); }

	TSphericalBounds& Set(const TDVector3& SCenter, double dSRadius)
		{ m_Center = SCenter, m_dRadius2 = Sqr(m_dRadius = dSRadius); return *this; };

	void Reset(const TDVector3& SCenter)
		{ m_Center.SetZero(), m_dRadius = 0.0, m_dRadius2 = 0.0; }

	TSphericalBounds& Update(const TDVector3& Coords)
	{
		const double dNewRadius2 =
			(Coords - m_Center).GetLengthSquare();

		if(Compare(dNewRadius2, m_dRadius2) > 0)
			m_dRadius = Sqrt(m_dRadius2 = dNewRadius2);

		return *this;
	}

	// <0 - inside, 0 - boundary, >0 - outside
	int GetDisposition(const TDVector3& Coords) const
		{ return Compare((Coords - m_Center).GetLengthSquare(), m_dRadius2); }

	TSphericalBounds& Merge(const TSphericalBounds& RBounds);
/*	{ // {{{ bad tech here, use smarter algo
		const double dNewRadius =
			(RBounds.m_Center - m_Center).GetLength() + RBounds.m_dRadius;

		if(Compare(dNewRadius, m_dRadius) > 0)
			m_dRadius2 = Sqr(m_dRadius = dNewRadius);

		return *this;
	}*/

	const TDVector3& GetCenter() const
		{ return m_Center; }

	double GetRadius() const
		{ return m_dRadius; }

	double GetRadius2() const
		{ return m_dRadius2; }
};

#endif // spherical_bounds_h