#ifndef d3d_color_h
#define d3d_color_h

// ----------
// D3D color
// ----------
struct TD3DColor
{
private:
	void Set(D3DCOLOR SColor)
	{
		m_fBlue		= (float)((SColor)			& 0xFF)	/ 0xFF;
		m_fGreen	= (float)((SColor >> 8)		& 0xFF)	/ 0xFF;
		m_fRed		= (float)((SColor >> 16)	& 0xFF)	/ 0xFF;
		m_fAlpha	= (float)((SColor >> 24))			/ 0xFF;
	}
	
public:
	float m_fRed;
	float m_fGreen;
	float m_fBlue;
	float m_fAlpha;

	
	TD3DColor() {}

	TD3DColor(float fSRed, float fSGreen, float fSBlue, float fSAlpha = 1.0f) :
		m_fRed(fSRed), m_fGreen(fSGreen), m_fBlue(fSBlue), m_fAlpha(fSAlpha) {}

	TD3DColor(D3DCOLOR SColor)
		{ Set(SColor); }

	void SetBlack(float fSAlpha = 1.0f)
		{ m_fRed = m_fGreen = m_fBlue = 0.0f, m_fAlpha = fSAlpha; }

	void SetWhite(float fSAlpha = 1.0f)
		{ m_fRed = m_fGreen = m_fBlue = 1.0f, m_fAlpha = fSAlpha; }

	void Set(float fSRed, float fSGreen, float fSBlue, float fSAlpha = 1.0f)
		{ m_fRed = fSRed, m_fGreen = fSGreen, m_fBlue = fSBlue, m_fAlpha = fSAlpha; }

	TD3DColor& Normalize()
	{
		return *this /= 255.0f;
	}

	TD3DColor Normalized() const
	{
		return TD3DColor(*this).Normalize();
	}

	TD3DColor& Truncate(bool bProportional = false);

	TD3DColor Truncated(bool bProportional = false) const
	{
		return TD3DColor(*this).Truncate(bProportional);
	}

	bool IsValid() const
	{
		const FSEGMENT ValidSegment(0.0f, 1.0f);

		return	HitsSegmentBounds(m_fRed,	ValidSegment) &&
				HitsSegmentBounds(m_fGreen,	ValidSegment) &&
				HitsSegmentBounds(m_fBlue,	ValidSegment) &&
				HitsSegmentBounds(m_fAlpha,	ValidSegment);
	}

	// Non-Const color operators
	TD3DColor& operator -= (const TD3DColor& SColor)
	{
		m_fRed		-= SColor.m_fRed;
		m_fGreen	-= SColor.m_fGreen;
		m_fBlue		-= SColor.m_fBlue;
		m_fAlpha	-= SColor.m_fAlpha;

		return *this;
	}

	TD3DColor& operator += (const TD3DColor& SColor)
	{
		m_fRed		+= SColor.m_fRed;
		m_fGreen	+= SColor.m_fGreen;
		m_fBlue		+= SColor.m_fBlue;
		m_fAlpha	+= SColor.m_fAlpha;

		return *this;
	}

	TD3DColor& operator &= (const TD3DColor& SColor)
	{
		m_fRed		*= SColor.m_fRed;
		m_fGreen	*= SColor.m_fGreen;
		m_fBlue		*= SColor.m_fBlue;

		return *this;
	}

	TD3DColor& operator *= (const TD3DColor& SColor)
	{
		m_fRed		*= SColor.m_fRed;
		m_fGreen	*= SColor.m_fGreen;
		m_fBlue		*= SColor.m_fBlue;
		m_fAlpha	*= SColor.m_fAlpha;

		return *this;
	}

	// Const color operators
	TD3DColor operator - (const TD3DColor& SColor) const
	{
		return TD3DColor(	m_fRed		- SColor.m_fRed,
							m_fGreen	- SColor.m_fGreen,
							m_fBlue		- SColor.m_fBlue,
							m_fAlpha	- SColor.m_fAlpha);
	}

	TD3DColor operator + (const TD3DColor& SColor) const
	{
		return TD3DColor(	m_fRed		+ SColor.m_fRed,
							m_fGreen	+ SColor.m_fGreen,
							m_fBlue		+ SColor.m_fBlue,
							m_fAlpha	+ SColor.m_fAlpha);
	}

	TD3DColor operator & (const TD3DColor& SColor) const
	{
		return TD3DColor(	m_fRed		* SColor.m_fRed,
							m_fGreen	* SColor.m_fGreen,
							m_fBlue		* SColor.m_fBlue,
							m_fAlpha);
	}	

	TD3DColor operator * (const TD3DColor& SColor) const
	{
		return TD3DColor(	m_fRed		* SColor.m_fRed,
							m_fGreen	* SColor.m_fGreen,
							m_fBlue		* SColor.m_fBlue,
							m_fAlpha	* SColor.m_fAlpha);
	}

	// Non-const float operators
	TD3DColor& operator &= (float fValue)
	{
		m_fRed		*= fValue;
		m_fGreen	*= fValue;
		m_fBlue		*= fValue;

		return *this;
	}

	TD3DColor& operator *= (float fValue)
	{
		m_fRed		*= fValue;
		m_fGreen	*= fValue;
		m_fBlue		*= fValue;
		m_fAlpha	*= fValue;

		return *this;
	}

	TD3DColor& operator /= (float fValue)
	{
		return *this *= (1.0f / fValue);
	}

	// Const float operators
	TD3DColor operator & (float fValue) const
	{
		return TD3DColor(	m_fRed		* fValue,
							m_fGreen	* fValue,
							m_fBlue		* fValue,
							m_fAlpha);
	}

	TD3DColor operator * (float fValue) const
	{
		return TD3DColor(	m_fRed		* fValue,
							m_fGreen	* fValue,
							m_fBlue		* fValue,
							m_fAlpha	* fValue);
	}

	TD3DColor operator / (float fValue) const
	{
		return *this * (1.0f / fValue);
	}

	// Comparisons
	bool operator == (const TD3DColor& SColor) const
	{
		return	m_fRed		== SColor.m_fRed	&&
				m_fGreen	== SColor.m_fGreen	&&
				m_fBlue		== SColor.m_fBlue	&&
				m_fAlpha	== SColor.m_fAlpha;
	}

	bool operator != (const TD3DColor& SColor) const
	{
		return	m_fRed		!= SColor.m_fRed	||
				m_fGreen	!= SColor.m_fGreen	||
				m_fBlue		!= SColor.m_fBlue	||
				m_fAlpha	!= SColor.m_fAlpha;
	}

	TD3DColor& operator = (D3DCOLOR SColor)
		{ Set(SColor); return *this; }

	operator D3DCOLOR () const;

	operator D3DCOLORVALUE* ()
		{ return (D3DCOLORVALUE*)this; }

	operator const D3DCOLORVALUE* () const
		{ return (const D3DCOLORVALUE*)this; }

	operator KString () const
	{
		KString Ret;

		Ret += (int)(m_fRed * 255.0f);
		Ret += TEXT(" ");
		Ret += (int)(m_fGreen * 255.0f);
		Ret += TEXT(" ");
		Ret += (int)(m_fBlue * 255.0f);

		if(Compare(m_fAlpha, 1.0f) < 0)
		{
			Ret += TEXT(" ");
			Ret += (int)(m_fAlpha * 255.0f);
		}
		
		return Ret;
	}
};

inline bool FromString(const KString& String, TD3DColor& RColor)
{
	const KString TempString = String.Trimmed();

	size_t szCount = _stscanf(	TempString,
								TEXT("%f %f %f %f"),
								&RColor.m_fRed,
								&RColor.m_fGreen,
								&RColor.m_fBlue,
								&RColor.m_fAlpha);

	if(szCount == 3)
	{
		RColor.m_fAlpha = 255.0f;
	}
	else if(szCount != 4)
	{
		return false;
	}

	RColor.Normalize();

	return true;
}

void ReadColor(	const KString&	String,
				TD3DColor&		RColor,
				LPCTSTR			pValueName,
				bool			bValidate = true);

#endif // d3d_color_h
