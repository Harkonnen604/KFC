#ifndef basic_bitypes_h
#define basic_bitypes_h

#include "basic_types.h"
#include "basic_wintypes.h"
#include "kstring.h"
#include "bounding.h"
#include "string_conv.h"
#include "debug.h"

// ------
// Point
// ------
template <class t>
struct TPoint
{
private:
	void WinCopy(const POINT& SPoint) { x = (t)SPoint.x, y = (t)SPoint.y; }
	
public:
	t x, y;

public:
	TPoint() {}

	TPoint(const TPoint<t>&	SPoint) : x(SPoint.x), y(SPoint.y) {}
	TPoint(const POINT& SPoint) { WinCopy(SPoint); }
	TPoint(t sx, t sy) : x(sx), y(sy) {}

	TPoint& SetZero()
		{ x = 0, y = 0; return *this; }

	TPoint& Set(t sx, t sy)
		{ x = sx, y = sy; return *this; }

	bool IsValid() const
		{ return true; }

	bool IsZero() const
		{ return ::IsZero(x) && ::IsZero(y); }

	TPoint& Transpose()
		{ Swap(x, y); return *this; }

	TPoint GetTransposed() const
		{ return TPoint(y, x); }
	
	TPoint<t>& operator = (const TPoint<t>& SPoint)
	{
		x = SPoint.x;
		y = SPoint.y;

		return *this;
	}

	TPoint<t>& operator = (const POINT& SPoint) { WinCopy(SPoint); return *this; }

	bool operator == (const TPoint<t>& SPoint) const
	{
		return	Compare(x, SPoint.x) == 0 &&
				Compare(y, SPoint.y) == 0;
	}
	
	bool operator != (const TPoint<t>& SPoint) const
	{
		return	Compare(x, SPoint.x) != 0 ||
				Compare(y, SPoint.y) != 0;
	}

	TPoint<t>& operator -= (const TPoint<t>& SPoint)
	{
		x -= SPoint.x;
		y -= SPoint.y;

		return *this;
	}

	TPoint<t>& operator += (const TPoint<t>& SPoint)
	{
		x += SPoint.x;
		y += SPoint.y;
		
		return *this;
	}

	template <class k>
	TPoint<t>& operator *= (k Value)
	{
		x *= Value;
		y *= Value;

		return *this;
	}

	template <class k>
	TPoint<t>& operator /= (k Value)
	{
		x /= Value;
		y /= Value;

		return *this;
	}

	TPoint<t> operator - () const
	{
		return TPoint<t>(-x, -y);
	}

	TPoint<t> operator - (const TPoint<t>& SPoint) const
	{
		return TPoint<t>(x - SPoint.x, y - SPoint.y);
	}

	TPoint<t> operator + (const TPoint<t>& SPoint) const
	{
		return TPoint<t>(x + SPoint.x, y + SPoint.y);
	}
	
	template <class k>
	TPoint<t> operator * (k Value) const
	{
		return TPoint<t>((t)(x * Value), (t)(y * Value));
	}
	
	template <class k>
	TPoint<t> operator / (k Value) const
	{
		return TPoint<t>((t)(x / Value), (t)(y / Value));
	}

	#ifdef _MSC_VER
		operator POINT* () { return (POINT*)this; }
		operator const POINT* () const { return (const POINT*)this; }	
	#endif // _MSC_VER

	bool IsNegative() const
	{
		return	Compare(x, (t)0) < 0 &&
				Compare(y, (t)0) < 0;
	}

	bool IsPositive() const
	{
		return	Compare(x, (t)0) > 0 &&
				Compare(y, (t)0) > 0;
	}

	operator KString () const
	{
		KString Ret;

		Ret += TEXT("(");
		Ret += x;
		Ret += TEXT(";");
		Ret += y;
		Ret += TEXT(")");

		return Ret;
	}
};

template <class t>
inline bool FromString(KString String, TPoint<t>& RPoint)
{
	String.Trim();

	if(	String.GetLength() < 2 ||
		String[0] != TEXT('(') ||
		String[String.GetLength() - 1] != TEXT(')'))
	{
		return false;
	}

	const size_t szPos = String.Find(TEXT(";"), 1);

	if(szPos == -1)
		return false;

	if(	!FromString(String.Mid(1, szPos - 1).Trim(), RPoint.x) ||
		!FromString(String.Mid(szPos + 1, String.GetLength() - szPos - 2).Trim(), RPoint.y))
	{
		return false;
	}

	return true;
}

// -----
// Rect
// -----
template <class t>
struct TRect
{
private:
	void WinCopy(const RECT& SRect)
	{
		m_Left		= (t)SRect.left;
		m_Top		= (t)SRect.top;
		m_Right		= (t)SRect.right;
		m_Bottom	= (t)SRect.bottom;
	}

public:
	t m_Left, m_Top, m_Right, m_Bottom;

public:
	TRect()
		{ Invalidate(); }

	TRect(const TRect<t>& SRect) :
		m_Left	(SRect.m_Left),
		m_Top	(SRect.m_Top),
		m_Right	(SRect.m_Right),
		m_Bottom(SRect.m_Bottom) {}
		
	TRect(const RECT& SRect)
		{ WinCopy(SRect); }

	TRect(t SLeft, t STop, t SRight, t SBottom) :
		m_Left(SLeft), m_Top(STop), m_Right(SRight), m_Bottom(SBottom) {}

	TRect(const TPoint<t>& Point1, const TPoint<t>& Point2) :
		m_Left(Point1.x), m_Top(Point1.y), m_Right(Point2.x), m_Bottom(Point2.y)
	{
		OrientateRect(*this);
	}

	bool IsZero() const
		{ return ::IsZero(m_Left) && ::IsZero(m_Top) && ::IsZero(m_Right) && ::IsZero(m_Bottom); }

	TRect& SetZero()
		{ m_Left = 0, m_Top = 0, m_Right = 0, m_Bottom = 0; return *this; }

	TRect& Set(t SLeft, t STop, t SRight, t SBottom)
	{
		m_Left		= SLeft;
		m_Top		= STop;
		m_Right		= SRight;
		m_Bottom	= SBottom;

		return *this;
	}

	TRect& Set(const TPoint<t>& p1, const TPoint<t>& p2)
	{
		m_Left		= p1.x;
		m_Top		= p1.y;
		m_Right		= p2.x;
		m_Bottom	= p2.y;

		return *this;
	}

	TRect& Transpose()
		{ Swap(m_Left, m_Top); Swap(m_Right, m_Bottom); return *this; }

	TRect GetTransposed() const
		{ return TRect(m_Top, m_Left, m_Bottom, m_Right); }

	TRect& Flip()
		{ Swap(m_Left, m_Right); Swap(m_Top, m_Bottom); return *this; }

	TRect GetFlipped() const
		{ return TRect(m_Right, m_Bottom, m_Left, m_Top); }

	bool IsFlat() const
	{
		return	Compare(m_Left,	m_Right)	== 0 ||
				Compare(m_Top,	m_Bottom)	== 0;
	}

	bool IsPoint() const
	{
		return	Compare(m_Left,	m_Right)	== 0 &&
				Compare(m_Top,	m_Bottom)	== 0;
	}

	bool IsValid() const
	{
		return	Compare(m_Left,	m_Right)	< 0 &&
				Compare(m_Top,	m_Bottom)	< 0;
	}

	bool IsFlatValid() const
	{
		return	Compare(m_Left,	m_Right)	<= 0 &&
				Compare(m_Top,	m_Bottom)	<= 0;
	}

	void Invalidate() { m_Left = 0, m_Right = 0, m_Top = 0, m_Bottom = 0; }

	TRect<t>& operator = (const TRect<t>&SRect)
	{
		m_Left		= SRect.m_Left;
		m_Top		= SRect.m_Top;
		m_Right		= SRect.m_Right;
		m_Bottom	= SRect.m_Bottom;

		return *this;
	}

	TRect<t>& operator = (const RECT& SRect) { WinCopy(SRect); return *this; }

	bool operator == (const TRect<t>& SRect) const
	{
		return	Compare(m_Left,		SRect.m_Left)	== 0 &&
				Compare(m_Top,		SRect.m_Top)	== 0 &&
				Compare(m_Right,	SRect.m_Right)	== 0 &&
				Compare(m_Bottom,	SRect.m_Bottom)	== 0;
	}

	bool operator != (const TRect<t>& SRect) const
	{
		return	Compare(m_Left,		SRect.m_Left)	!= 0 ||
				Compare(m_Top,		SRect.m_Top)	!= 0 ||
				Compare(m_Right,	SRect.m_Right)	!= 0 ||
				Compare(m_Bottom,	SRect.m_Bottom)	!= 0;
	}

	TRect<t>& operator -= (const TRect<t>& SRect)
	{
		m_Left		-= SRect.m_Left;
		m_Top		-= SRect.m_Top;
		m_Right		-= SRect.m_Right;
		m_Bottom	-= SRect.m_Bottom;

		return *this;
	}

	TRect<t>& operator += (const TRect<t>& SRect)
	{
		m_Left		+= SRect.m_Left;
		m_Top		+= SRect.m_Top;
		m_Right		+= SRect.m_Right;
		m_Bottom	+= SRect.m_Bottom;

		return *this;
	}

	template <class k>
	TRect<t>& operator *= (k Value)
	{
		m_Left		*= Value;
		m_Top		*= Value;
		m_Right		*= Value;
		m_Bottom	*= Value;

		return *this;
	}

	template <class k>
	TRect<t>& operator /= (k Value)
	{
		m_Left		/= Value;
		m_Top		/= Value;
		m_Right		/= Value;
		m_Bottom	/= Value;

		return *this;
	}

	TRect<t> operator - () const // deorients
	{
		return TRect<t>((t)-m_Left,
						(t)-m_Top,
						(t)-m_Right,
						(t)-m_Bottom);
	}

	TRect<t> operator - (const TRect<t>& SRect) const
	{
		return TRect<t>(m_Left		- SRect.m_Left,
						m_Top		- SRect.m_Top,
						m_Right		- SRect.m_Right,
						m_Bottom	- SRect.m_Bottom);
	}

	TRect<t> operator + (const TRect<t>& SRect) const
	{
		return TRect<t>(m_Left		+ SRect.m_Left,
						m_Top		+ SRect.m_Top,
						m_Right		+ SRect.m_Right,
						m_Bottom	+ SRect.m_Bottom);
	}

	template <class k>
	TRect<t> operator * (k Value) const
	{
		return TRect<t>((t)(m_Left		* Value),
						(t)(m_Top		* Value),
						(t)(m_Right		* Value),
						(t)(m_Bottom	* Value));
	}

	template <class k>
	TRect<t> operator / (k Value) const
	{
		return TRect<t>((t)(m_Left		/ Value),
						(t)(m_Top		/ Value),
						(t)(m_Right		/ Value),
						(t)(m_Bottom	/ Value));
	}

	#ifdef _MSC_VER
		operator RECT*			()			{ return (RECT*)		this; }
		operator const RECT*	() const	{ return (const RECT*)	this; }
	#endif // _MSC_VER

	bool IsNegative() const
	{
		return	Compare(m_Left,		(t)0) < 0 &&
				Compare(m_Top,		(t)0) < 0 &&
				Compare(m_Right,	(t)0) < 0 &&
				Compare(m_Bottom,	(t)0) < 0;
	}

	bool IsPositive() const
	{
		return	Compare(m_Left,		(t)0) > 0 &&
				Compare(m_Top,		(t)0) > 0 &&
				Compare(m_Right,	(t)0) > 0 &&
				Compare(m_Bottom,	(t)0) > 0;
	}

	t GetWidth	() const { return m_Right - m_Left; }
	t GetHeight	() const { return m_Bottom - m_Top; }

	t GetArea() const { return GetWidth() * GetHeight(); }

	t GetCenterX() const { return (m_Left + m_Right) / 2; }
	t GetCenterY() const { return (m_Top + m_Bottom) / 2; }

	TPoint<t> GetCenter() const { return TPoint<t>(GetCenterX(), GetCenterY()); }

	TPoint<t> GetTopLeft() const
		{ return TPoint<t>(m_Left, m_Top); }

	TPoint<t> GetTopRight() const
		{ return TPoint<t>(m_Right, m_Top); }

	TPoint<t> GetBottomLeft() const
		{ return TPoint<t>(m_Left, m_Bottom); }

	TPoint<t> GetBottomRight() const
		{ return TPoint<t>(m_Right, m_Bottom); }

	operator KString () const
	{
		KString Ret;

		Ret += TEXT("[");
		Ret += m_Left;
		Ret += TEXT(";");
		Ret += m_Top;
		Ret += TEXT(" - ");
		Ret += m_Right;
		Ret += TEXT(";");
		Ret += m_Bottom;
		Ret += TEXT(")");

		return Ret;
	}
};

template <class t>
inline bool FromString(KString String, TRect<t>& RRect)
{
	String.Trim();

	if(	String.GetLength() < 2 ||
		String[0] != TEXT('[') ||
		String[String.GetLength() - 1] != TEXT(')'))
	{
		return false;
	}

	size_t szPos;

	szPos = String.Find(TEXT(" - "), 1);

	if(szPos == -1)
		return false;

	KString Start	= String.Mid(1, szPos - 1).Trim();
	KString End		= String.Mid(szPos + 3, String.GetLength() - szPos - 4).Trim();

	// Start
	szPos = Start.Find(TEXT(";"));

	if(szPos == -1)
		return false;

	if(	!FromString(Start.Left(szPos).Trim(), RRect.m_Left) ||
		!FromString(Start.Right(Start.GetLength() - szPos - 1).Trim(), RRect.m_Top))
	{
		return false;
	}

	// End
	szPos = End.Find(TEXT(";"));

	if(szPos == -1)
		return false;

	if(	!FromString(End.Left(szPos).Trim(), RRect.m_Right) ||
		!FromString(End.Right(End.GetLength() - szPos - 1).Trim(), RRect.m_Bottom))
	{
		return false;
	}

	return true;
}

// -----
// Size
// -----
template <class t>
struct TSize
{
private:
	void WinCopy(const SIZE& SSize)
		{ cx = (t)SSize.cx, cy = (t)SSize.cy; }

	void WinCopy(const POINT& Point)
		{ cx = (t)Point.x, cy = (t)Point.y; }

public:
	t cx, cy;

public:
	TSize() {}
	TSize(const TSize<t>& SSize) : cx(SSize.cx), cy(SSize.cy) {}
	TSize(const SIZE& SSize) { WinCopy(SSize); }
	TSize(t scx, t scy) : cx(scx), cy(scy) {}
	TSize(const TPoint<t>& Point) : cx(Point.x), cy(Point.y) {}
	TSize(const POINT& Point) { WinCopy(Point); }
	TSize(const TPoint<t>& Point1, const TPoint<t>& Point2) : cx(Point2.x - Point1.x), cy(Point2.y - Point1.y) {}
	TSize(const TRect<t>& Rect) : cx(Rect.m_Right - Rect.m_Left), cy(Rect.m_Bottom - Rect.m_Top) {}
	
	TSize& SetZero()
		{ cx = 0, cy = 0; return *this; }

	TSize& Set(t scx, t scy)
		{ cx = scx, cy = scy; return *this; }

	bool IsValid() const
		{ return true; }

	bool IsZero() const
		{ return ::IsZero(cx) && ::IsZero(cy); }

	bool IsFlat() const
		{ return ::IsZero(cx) || ::IsZero(cy); }

	bool IsPoint() const
		{ return IsZero(); }

	t GetArea() const { return cx * cy; }

	TSize& Transpose()
		{ Swap(cx, cy); return *this; }

	TSize GetTransposed() const
		{ return TSize(cy, cx); }

	t GetAverage() const
		{ return (cx + cy) / 2; }

	t GetLengthSquare() const
		{ return cx*cx + cy*cy; }

	t GetLength() const
		{ return (t)sqrt((double)GetLengthSquare()); }

	TSize<t>& operator = (const TSize<t>& SSize)
	{
		cx = SSize.cx;
		cy = SSize.cy;

		return *this;
	}

	TSize<t>& operator = (const SIZE& SSize)
		{ WinCopy(SSize); return *this; }

	TSize<t>& operator = (const POINT& Point)
		{ WinCopy(Point); return *this; }

	TSize<t>& operator = (const TRect<t>& Rect)
	{
		cx = Rect.m_Right - Rect.m_Left;
		cy = Rect.m_Bottom - Rect.m_Top;

		return *this;
	}

	TSize<t>& operator -= (const TSize<t>& SSize)
	{
		cx -= SSize.cx;
		cy -= SSize.cy;

		return *this;
	}

	TSize<t>& operator += (const TSize<t>& SSize)
	{
		cx += SSize.cx;
		cy += SSize.cy;

		return *this;
	}

	template <class k>
	TSize<t>& operator *= (k Value)
	{
		cx *= Value;
		cy *= Value;

		return *this;
	}

	template <class k>
	TSize<t>& operator /= (k Value)
	{
		cx /= Value;
		cy /= Value;

		return *this;
	}

	TSize<t> operator - () const
	{
		return TSize<t>(-cx, -cy);
	}

	TSize<t> operator - (const TSize<t>& SSize) const
	{
		return TSize<t>(cx - SSize.cx, cy - SSize.cy);
	}

	TSize<t> operator + (const TSize<t>& SSize) const
	{
		return TSize<t>(cx + SSize.cx, cy + SSize.cy);
	}

	template <class k>
	TSize<t> operator * (k Value) const
	{
		return TSize<t>((t)(cx * Value), (t)(cy * Value));
	}

	template <class k>
	TSize<t> operator / (k Value) const
	{
		return TSize<t>((t)(cx / Value), (t)(cy / Value));
	}

	// Warning: non-transactive comparisons
	bool operator <		(const TSize<t>& SSize) const { return cx <		SSize.cx || cy <	SSize.cy; }
	bool operator >		(const TSize<t>& SSize) const { return cx >		SSize.cx || cy >	SSize.cy; }
	bool operator <=	(const TSize<t>& SSize) const { return cx <=	SSize.cx || cy <=	SSize.cy; }
	bool operator >=	(const TSize<t>& SSize) const { return cx >=	SSize.cx || cy >=	SSize.cy; }
	bool operator ==	(const TSize<t>& SSize) const { return cx ==	SSize.cx && cy ==	SSize.cy; }
	bool operator !=	(const TSize<t>& SSize) const { return cx !=	SSize.cx || cy !=	SSize.cy; }

	#ifdef _MSC_VER
		operator SIZE*			() { return (SIZE*)			this; }
		operator const SIZE*	() { return (const SIZE*)	this; }	
	#endif // _MSC_VER

	bool IsNegative() const
	{
		return	Compare(cx, (t)0) < 0 &&
				Compare(cy, (t)0) < 0;
	}

	bool IsFlatNegative() const
	{
		return	Compare(cx, (t)0) <= 0 &&
				Compare(cy, (t)0) <= 0;
	}

	bool IsPositive() const
	{
		return	Compare(cx, (t)0) > 0 &&
				Compare(cy, (t)0) > 0;
	}

	bool IsFlatPositive() const
	{
		return	Compare(cx, (t)0) >= 0 &&
				Compare(cy, (t)0) >= 0;
	}

	operator KString () const
	{
		KString Ret;

		Ret += TEXT("<");
		Ret += cx;
		Ret += TEXT(";");
		Ret += cy;
		Ret += TEXT(">");

		return Ret;
	}
};

template <class t>
inline bool FromString(KString String, TSize<t>& RSize)
{
	String.Trim();

	if(	String.GetLength() < 2 ||
		String[0] != TEXT('<') ||
		String[String.GetLength() - 1] != TEXT('>'))
	{
		return false;
	}

	const size_t szPos = String.Find(TEXT(";"), 1);

	if(szPos == -1)
		return false;

	if(	!FromString(String.Mid(1, szPos - 1).Trim(), RSize.cx) ||
		!FromString(String.Mid(szPos + 1, String.GetLength() - szPos - 2).Trim(), RSize.cy))
	{
		return false;
	}

	return true;
}

// --------
// Segment
// --------
template <class t>
struct TSegment
{
public:
	t m_First, m_Last;

public:
	TSegment()
		{ Invalidate(); }

	TSegment(const TSegment<t>& SSegment) :
		m_First	(SSegment.m_First),
		m_Last	(SSegment.m_Last) {}

	TSegment(t SFirst, t SLast) :
		m_First(SFirst), m_Last(SLast) {}

	TSegment& SetZero()
		{ m_First = 0, m_Last = 0; return *this; }

	TSegment& Set(t SFirst, t SLast)
		{ m_First = SFirst, m_Last = SLast; return *this; }

	t GetLength() const
		{ return m_Last - m_First; }

	t GetCenter() const
		{ return (t)((m_First + m_Last) / 2); }

	bool IsValid() const
		{ return Compare(m_First, m_Last) < 0; }

	bool IsFlatValid() const
		{ return Compare(m_First, m_Last) <= 0; }

	bool IsFlat() const
		{ return Compare(m_First, m_Last) == 0; }

	bool IsZero() const
		{ return ::IsZero(m_First) && ::IsZero(m_Last); }

	TSegment& Invalidate()
		{ m_First = (t)1, m_Last = 0; return *this; }

	TSegment& Flip()
		{ Swap(m_First, m_Last); return *this; }

	TSegment GetFlipped() const
		{ return TSegment(m_Last, m_First); }

	TSegment<t>& operator = (const TSegment<t>& SSegment)
	{
		m_First	= SSegment.m_First;
		m_Last	= SSegment.m_Last;
		
		return *this;
	}

	TSegment<t>& operator ++ ()
	{
		m_First++, m_Last++;
		return *this;
	}

	TSegment<t>& operator -- ()
	{
		m_First--, m_Last--;
		return *this;
	}

	TSegment<t> operator - () const // deorients
	{
		return TSegment<t>((t)-m_First, (t)-m_Last);
	}

	TSegment<t> operator + () const
		{ return *this; }

	TSegment<t> operator + (const TSegment<t>& SSegment) const
	{
		return TSegment<t>(	m_First	+ SSegment.m_First,
							m_Last	+ SSegment.m_Last);
	}

	TSegment<t> operator - (const TSegment<t>& SSegment) const
	{
		return TSegment<t>(	m_First	- SSegment.m_First,
							m_Last	- SSegment.m_Last);
	}	

	template <class k>
	TSegment<t> operator + (k Value) const
	{
		return TSegment<t>(	(t)(m_First + Value),
							(t)(m_Last  + Value));
	}

	template <class k>
	TSegment<t> operator - (k Value) const
	{
		return TSegment<t>(	(t)(m_First - Value),
							(t)(m_Last  - Value));
	}

	template <class k>
	TSegment<t> operator * (k Value) const
	{
		return TSegment<t>(	(t)(m_First * Value),
							(t)(m_Last  * Value));
	}

	template <class k>
	TSegment<t> operator / (k Value) const
	{
		return TSegment<t>(	(t)(m_First / Value),
							(t)(m_Last  / Value));
	}

	TSegment<t>& operator += (const TSegment<t>& SSegment)
	{
		m_First += SSegment.m_First;
		m_Last  += SSegment.m_Last;

		return *this;
	}

	TSegment<t>& operator -= (const TSegment<t>& SSegment)
	{
		m_First -= SSegment.m_First;
		m_Last  -= SSegment.m_Last;

		return *this;
	}	

	template <class k>
	TSegment<t>& operator += (k Value)
	{
		m_First = (t)(m_First + Value);
		m_Last  = (t)(m_Last  + Value);

		return *this;
	}

	template <class k>
	TSegment<t>& operator -= (k Value)
	{
		m_First = (t)(m_First - Value);
		m_Last  = (t)(m_Last  - Value);

		return *this;
	}

	template <class k>
	TSegment<t>& operator *= (k Value)
	{
		m_First = (t)(m_First * Value);
		m_Last  = (t)(m_Last  * Value);

		return *this;
	}

	template <class k>
	TSegment<t>& operator /= (k Value)
	{
		m_First = (t)(m_First / Value);
		m_Last  = (t)(m_Last  / Value);

		return *this;
	}

	bool operator == (const TSegment<t>& SSegment) const
	{
		return	Compare(m_First,	SSegment.m_First)	== 0 &&
				Compare(m_Last,		SSegment.m_Last)	== 0;
	}

	bool operator != (const TSegment<t>& SSegment) const
	{
		return	Compare(m_First,	SSegment.m_First)	!= 0 ||
				Compare(m_Last,		SSegment.m_Last)	!= 0;
	}

	bool IsNegative() const
	{
		return	Compare(m_First,	(t)0) < 0 &&
				Compare(m_Last,		(t)0) < 0;
	}

	bool IsPositive() const
	{
		return	Compare(m_First,	(t)0) > 0 &&
				Compare(m_Last,		(t)0) > 0;
	}

	operator KString () const
	{
		KString Ret;

		Ret += TEXT("[");
		Ret += (KString)m_First;
		Ret += TEXT(";");
		Ret += (KString)m_Last;
		Ret += TEXT(")");

		return Ret;
	}
};

template <class t>
inline bool FromString(KString String, TSegment<t>& RSegment)
{
	String.Trim();

	if(	String.GetLength() < 2 ||
		String[0] != TEXT('[') ||
		String[String.GetLength() - 1] != TEXT(')'))
	{
		return false;
	}

	const size_t szPos = String.Find(TEXT(";"), 1);

	if(szPos == -1)
		return false;

	if(	!FromString(String.Mid(1, szPos - 1).Trim(), RSegment.m_First) ||
		!FromString(String.Mid(szPos + 1, String.GetLength() - szPos - 2).Trim(), RSegment.m_Last))
	{
		return false;
	}

	return true;
}

// -------------------
// Integer 64 structs
// -------------------
typedef TPoint	<INT64> I64POINT;
typedef TRect	<INT64> I64RECT;
typedef TSize	<INT64> I64SIZE;
typedef TSegment<INT64> I64SEGMENT;

// -------------
// Long structs
// -------------
typedef TPoint	<long> LPOINT;
typedef TRect	<long> LRECT;
typedef TSize	<long> LSIZE;
typedef TSegment<long> LSEGMENT;

// ----------------
// Integer structs
// ----------------
typedef TPoint	<int> IPOINT;
typedef TRect	<int> IRECT;
typedef TSize	<int> ISIZE;
typedef TSegment<int> ISEGMENT;

// --------------
// Short structs
// --------------
typedef TPoint	<short> SPOINT;
typedef TRect	<short> SRECT;
typedef TSize	<short> SSIZE;
typedef TSegment<short> SSEGMENT;

// --------------
// QWORD structs
// --------------
typedef TPoint	<QWORD> QWPOINT;
typedef TRect	<QWORD> QWRECT;
typedef TSize	<QWORD> QWSIZE;
typedef TSegment<QWORD> QWSEGMENT;

// --------------
// QWORD structs
// --------------
typedef TPoint	<QWORD> QWPOINT;
typedef TRect	<QWORD> QWRECT;
typedef TSize	<QWORD> QWSIZE;
typedef TSegment<QWORD> QWSEGMENT;

// --------------
// DWORD structs
// --------------
typedef TPoint	<DWORD> DWPOINT;
typedef TRect	<DWORD> DWRECT;
typedef TSize	<DWORD> DWSIZE;
typedef TSegment<DWORD> DWSEGMENT;

// -------------
// UINT structs
// -------------
typedef TPoint	<UINT> UIPOINT;
typedef TRect	<UINT> UIRECT;
typedef TSize	<UINT> UISIZE;
typedef TSegment<UINT> UISEGMENT;

// ------------------
// Size type structs
// ------------------
typedef TPoint	<size_t> SZPOINT;
typedef TRect	<size_t> SZRECT;
typedef TSize	<size_t> SZSIZE;
typedef TSegment<size_t> SZSEGMENT;

// -------------
// WORD structs
// -------------
typedef TPoint	<WORD> WPOINT;
typedef TRect	<WORD> WRECT;
typedef TSize	<WORD> WSIZE;
typedef TSegment<WORD> WSEGMENT;

// --------------
// Float structs
// --------------
typedef TPoint	<float> FPOINT;
typedef TRect	<float> FRECT;
typedef TSize	<float> FSIZE;
typedef TSegment<float> FSEGMENT;

// ---------------
// Double structs
// ---------------
typedef TPoint	<double> DPOINT;
typedef TRect	<double> DRECT;
typedef TSize	<double> DSIZE;
typedef TSegment<double> DSEGMENT;

// ------------------
// Alignment structs
// ------------------
typedef TPoint	<TAlignment> ALPOINT;
typedef TRect	<TAlignment> ALRECT;
typedef TSize	<TAlignment> ALSIZE;
typedef TSegment<TAlignment> ALSEGMENT;

// -------------
// Type structs
// -------------
typedef TPoint	<ktype_t> TPPOINT;
typedef TRect	<ktype_t> TPRECT;
typedef TSize	<ktype_t> TPSIZE;
typedef TSegment<ktype_t> TPSEGMENT;

// -------------
// Mode structs
// -------------
typedef TPoint	<kmode_t> MDPOINT;
typedef TRect	<kmode_t> MDRECT;
typedef TSize	<kmode_t> MDSIZE;
typedef TSegment<kmode_t> MDSEGMENT;

// --------------
// Flags structs
// --------------
typedef TPoint	<kflags_t> FLPOINT;
typedef TRect	<kflags_t> FLRECT;
typedef TSize	<kflags_t> FLSIZE;
typedef TSegment<kflags_t> FLSEGMENT;

// ----------------
// Global routines
// ----------------
template <class t>
inline TPoint<t> MinPoint(const TPoint<t>& Point1, const TPoint<t>& Point2)
{
	return TPoint<t>(Min(Point1.x, Point2.x), Min(Point1.y, Point2.y));
}

template <class t>
inline TPoint<t> MaxPoint(const TPoint<t>& Point1, const TPoint<t>& Point2)
{
	return TPoint<t>(Max(Point1.x, Point2.x), Max(Point1.y, Point2.y));
}

template <class t>
inline TSize<t> MinSize(const TSize<t>& Size1, const TSize<t>& Size2)
{
	return TSize<t>(Min(Size1.cx, Size2.cx), Min(Size1.cy, Size2.cy));
}

template <class t>
inline TSize<t> MaxSize(const TSize<t>& Size1, const TSize<t>& Size2)
{
	return TSize<t>(Max(Size1.cx, Size2.cx), Max(Size1.cy, Size2.cy));
}

template <class t1, class t2>
inline TRect<t1> RectFromCS(const TPoint<t1>& Coords, const TSize<t2>& Size)
{
	return TRect<t1>(	Coords.x,
						Coords.y,
						(t1)(Coords.x + Size.cx),
						(t1)(Coords.y + Size.cy));
}

template <class t>
inline bool HitsRect(const TPoint<t>& Point, const TRect<t>& Rect)
{
	return	Compare(Point.x, Rect.m_Left)	>= 0	&&
			Compare(Point.x, Rect.m_Right)	< 0		&&
			Compare(Point.y, Rect.m_Top)	>= 0	&&
			Compare(Point.y, Rect.m_Bottom)	< 0;
}

template <class t>
inline bool HitsRectBounds(const TPoint<t>& Point, const TRect<t>& Rect)
{
	return	Compare(Point.x, Rect.m_Left)	>= 0 &&
			Compare(Point.x, Rect.m_Right)	<= 0 &&
			Compare(Point.y, Rect.m_Top)	>= 0 &&
			Compare(Point.y, Rect.m_Bottom)	<= 0;
}

template <class t>
inline bool HitsSegment(const t& Value, const TSegment<t>& Segment)
{
	return	Compare(Value, Segment.m_First) >= 0 &&
			Compare(Value, Segment.m_Last)	< 0;
}

template <class t>
inline bool HitsSegmentBounds(const t& Value, const TSegment<t>& Segment)
{
	return	Compare(Value, Segment.m_First) >= 0 &&
			Compare(Value, Segment.m_Last)	<= 0;
}

template <class t, class ot>
inline TPoint<t>& ShiftPoint(TPoint<t>& Point, const TSize<ot>& Offset)
{
	Point.x += Offset.cx;
	Point.y += Offset.cy;

	return Point;
}

template <class t, class ot>
inline TRect<t>& ShiftRect(TRect<t>& Rect, const TSize<ot>& Offset)
{
	Rect.m_Left		+= Offset.cx;
	Rect.m_Top		+= Offset.cy;
	Rect.m_Right	+= Offset.cx;
	Rect.m_Bottom	+= Offset.cy;

	return Rect;
}

template <class t>
inline t& LimitValue(t& Value, const TSegment<t>& Segment)
{
	if(Compare(Value, Segment.m_First) < 0)
		Value = Segment.m_First;
	else if(Compare(Value, Segment.m_Last) > 0)
		Value = Segment.m_Last;
	
	return Value;
}

template <class t>
inline TPoint<t>& LimitPoint(TPoint<t>& Point, const TRect<t>& Rect)
{
	LimitValue(Point.x, TSegment<t>(Rect.m_Left, Rect.m_Right));
	LimitValue(Point.y, TSegment<t>(Rect.m_Top, Rect.m_Bottom));

	return Point;
}

template <class t>
inline TRect<t>& LimitRect(TRect<t>& Rect, const TRect<t>& Bound)
{
	TSegment<t> XSegment(Bound.m_Left,	Bound.m_Right);
	TSegment<t> YSegment(Bound.m_Top,	Bound.m_Bottom);

	LimitValue(Rect.m_Left,		XSegment);
	LimitValue(Rect.m_Top,		YSegment);
	LimitValue(Rect.m_Right,	XSegment);
	LimitValue(Rect.m_Bottom,	YSegment);

	return Rect;
}

template <class t>
inline t& FlipValue(t& Value, const TSegment<t>& Segment)
{
	Value = Segment.m_First + Segment.m_Last - Value;

	return Value;
}

template <class t>
inline TPoint<t>& FlipPoint(TPoint<t>& Point, const TRect<t>& Rect)
{
	Point.x = Rect.m_Left + Rect.m_Right  - Point.x;
	Point.y = Rect.m_Top  + Rect.m_Bottom - Point.y;

	return Point;
}

template <class t>
inline t& ResizeValue(	const TSegment<t>&	SrcCoordsSegment,
						const TSegment<t>&	DstCoordsSegment,
						const t&			SrcValue,
						t&					RDstValue)
{
	t c = BOUNDC(SrcCoordsSegment.m_First, SrcCoordsSegment.m_Last, DstCoordsSegment.m_First, DstCoordsSegment.m_Last);

	RDstValue = BOUNDR(SrcValue, SrcCoordsSegment.m_First, DstCoordsSegment.m_First, c);

	return RDstValue;
}

template <class t>
inline t& ResizeValue(	const TSegment<t>&	SrcCoordsSegment,
						const TSegment<t>&	DstCoordsSegment,
						t&					Value)
{
	ResizeValue(SrcCoordsSegment, DstCoordsSegment, Value, Value);

	return Value;
}

template <class t>
inline TPoint<t>& ResizePoint(	const TRect<t>&		SrcCoordsRect,
								const TRect<t>&		DstCoordsRect,
								const TPoint<t>&	SrcPoint,
								TPoint<t>&			RDstPoint)
{
	if(SrcCoordsRect.IsValid() && DstCoordsRect.IsValid())
	{
		ResizeValue(TSegment<t>(SrcCoordsRect.m_Left, SrcCoordsRect.m_Right),
					TSegment<t>(DstCoordsRect.m_Left, DstCoordsRect.m_Right),
					SrcPoint.x,
					RDstPoint.x);

		ResizeValue(TSegment<t>(SrcCoordsRect.m_Top, SrcCoordsRect.m_Bottom),
					TSegment<t>(DstCoordsRect.m_Top, DstCoordsRect.m_Bottom),
					SrcPoint.y,
					RDstPoint.y);
	}
	else
	{
		RDstPoint.Set(0, 0);
	}

	return RDstPoint;
}

template <class t>
inline TPoint<t>& ResizePoint(	const TRect<t>&		SrcCoordsRect,
								const TRect<t>&		DstCoordsRect,
								TPoint<t>&			Point)
{
	ResizePoint(SrcCoordsRect, DstCoordsRect, Point, Point);

	return Point;
}

template <class t>
inline TSize<t>& ResizeSize(const TSize<t>& SrcCoordsSize,
							const TSize<t>& DstCoordsSize,
							const TSize<t>& SrcSize,
							TSize<t>&		RDstSize)
{
	if(!SrcCoordsSize.IsFlat())
	{
		RDstSize.cx = SrcSize.cx * DstCoordsSize.cx / SrcCoordsSize.cx;
		RDstSize.cy = SrcSize.cy * DstCoordsSize.cy / SrcCoordsSize.cy;
	}
	else
	{
		RDstSize.Set(0, 0);
	}

	return RDstSize;
}

template <class t>
inline TSize<t>& ResizeSize(const TSize<t>& SrcCoordsSize,
							const TSize<t>& DstCoordsSize,
							TSize<t>&		Size)
{
	ResizeSize(SrcCoordsSize, DstCoordsSize, Size, Size);

	return Size;
}

template <class t>
inline TRect<t>& ResizeRect(const TRect<t>&	SrcCoordsRect,
							const TRect<t>&	DstCoordsRect,
							const TRect<t>&	SrcRect,
							TRect<t>&		RDstRect)
{
	if(SrcCoordsRect.IsValid() && DstCoordsRect.IsValid() && SrcRect.IsValid())
	{
		t cx = BOUNDC(SrcCoordsRect.m_Left,	SrcCoordsRect.m_Right,	DstCoordsRect.m_Left,	DstCoordsRect.m_Right);
		t cy = BOUNDC(SrcCoordsRect.m_Top,	SrcCoordsRect.m_Bottom,	DstCoordsRect.m_Top,	DstCoordsRect.m_Bottom);

		RDstRect.m_Left		= BOUNDR(SrcRect.m_Left,		SrcCoordsRect.m_Left,	DstCoordsRect.m_Left,	cx);
		RDstRect.m_Top		= BOUNDR(SrcRect.m_Top,		SrcCoordsRect.m_Top,	DstCoordsRect.m_Top,	cy);
		RDstRect.m_Right	= BOUNDR(SrcRect.m_Right,	SrcCoordsRect.m_Left,	DstCoordsRect.m_Left,	cx);
		RDstRect.m_Bottom	= BOUNDR(SrcRect.m_Bottom,	SrcCoordsRect.m_Top,	DstCoordsRect.m_Top,	cy);
	}
	else
	{
		RDstRect.Invalidate();
	}

	return RDstRect;
}

template <class t>
inline TRect<t>& ResizeRect(const TRect<t>&	SrcCoordsRect,
							const TRect<t>&	DstCoordsRect,
							TRect<t>&		Rect)
{
	ResizeRect(SrcCoordsRect, DstCoordsRect, Rect, Rect);

	return Rect;
}

template <class t>
inline bool FitSize(const TSize<t>&	ParentSize,
					const TSize<t>&	DefaultClientSize,
					TSize<t>&		RClientSize,
					bool			bScaleIfSmaller = false)
{
	if(!bScaleIfSmaller && !(DefaultClientSize > ParentSize))
	{
		RClientSize = DefaultClientSize;
		return false;
	}

	if(ParentSize.cx * DefaultClientSize.cy <= ParentSize.cy * DefaultClientSize.cx)
	{
		 RClientSize.cx = ParentSize.cx;
		 RClientSize.cy = DefaultClientSize.cx ? RClientSize.cx * DefaultClientSize.cy / DefaultClientSize.cx : 0;
	}
	else
	{
		RClientSize.cy = ParentSize.cy;
		RClientSize.cx = DefaultClientSize.cy ? RClientSize.cy * DefaultClientSize.cx / DefaultClientSize.cy : 0;
	}

	return true;
}

template <class t>
inline bool FitRect(const TRect<t>&	ParentRect,
					const TSize<t>&	DefaultClientSize,
					TRect<t>&		RClientRect,
					bool			bScaleIfSmaller = false)
{
	TSize<t> ClientSize;

	bool bScale = FitSize(	TSize<t>(ParentRect),
							DefaultClientSize,
							ClientSize,
							bScaleIfSmaller);

	RClientRect.m_Left		= ParentRect.	m_Left	+ (ParentRect.m_Right	- ParentRect.m_Left	- ClientSize.cx) / 2;
	RClientRect.m_Top		= ParentRect.	m_Top	+ (ParentRect.m_Bottom	- ParentRect.m_Top	- ClientSize.cy) / 2;
	RClientRect.m_Right		= RClientRect.	m_Left	+ ClientSize.cx;
	RClientRect.m_Bottom	= RClientRect.	m_Top	+ ClientSize.cy;

	return bScale;
}

template <class t>
inline size_t SubtractRect(const TRect<t>& Rect, const TRect<t>& SubRect, TRect<t> ResRects[4])
{
	if(!Rect.IsValid())
		return 0;

	TRect<t> IntSubRect = SubRect;
	if(IntersectRect(IntSubRect, Rect) == 0)
	{
		ResRects[0] = Rect;
		return 1;
	}

	size_t szN = 0;

	ResRects[szN].m_Left	= Rect.m_Left;
	ResRects[szN].m_Top		= Rect.m_Top;
	ResRects[szN].m_Right	= Rect.m_Right;
	ResRects[szN].m_Bottom	= IntSubRect.m_Top;
	if(ResRects[szN].IsValid())
		szN++;

	ResRects[szN].m_Left	= Rect.m_Left;
	ResRects[szN].m_Top		= IntSubRect.m_Top;
	ResRects[szN].m_Right	= IntSubRect.m_Left;
	ResRects[szN].m_Bottom	= IntSubRect.m_Bottom;
	if(ResRects[szN].IsValid())
		szN++;

	ResRects[szN].m_Left	= IntSubRect.m_Right;
	ResRects[szN].m_Top		= IntSubRect.m_Top;
	ResRects[szN].m_Right	= Rect.m_Right;
	ResRects[szN].m_Bottom	= IntSubRect.m_Bottom;
	if(ResRects[szN].IsValid())
		szN++;

	ResRects[szN].m_Left	= Rect.m_Left;
	ResRects[szN].m_Top		= IntSubRect.m_Bottom;
	ResRects[szN].m_Right	= Rect.m_Right;
	ResRects[szN].m_Bottom	= Rect.m_Bottom;
	if(ResRects[szN].IsValid())
		szN++;

	return szN;
}

template <class t>
inline size_t IntersectRect(TRect<t>& Rect, const TRect<t>& Bound)
{
	if(!Rect.IsValid())
		return 0;
	
	if(!Bound.IsValid())
	{
		Rect.Invalidate();
		return 0;
	}

	if(Compare(Rect.m_Left, Bound.m_Left) < 0)
		Rect.m_Left = Bound.m_Left;

	if(Compare(Rect.m_Top, Bound.m_Top) < 0)
		Rect.m_Top = Bound.m_Top;

	if(Compare(Rect.m_Right, Bound.m_Right) > 0)
		Rect.m_Right = Bound.m_Right;

	if(Compare(Rect.m_Bottom, Bound.m_Bottom) > 0)
		Rect.m_Bottom = Bound.m_Bottom;

	return Rect.IsValid() ? 1 : 0;
}

template <class t>
inline TRect<t>& CoverRect(TRect<t>& Rect1, const TRect<t>& Rect2)
{
	if(Rect2.m_Left < Rect1.m_Left)
		Rect1.m_Left = Rect2.m_Left;

	if(Rect2.m_Top < Rect1.m_Top)
		Rect1.m_Top = Rect2.m_Top;

	if(Rect2.m_Right > Rect1.m_Right)
		Rect1.m_Right = Rect2.m_Right;

	if(Rect2.m_Bottom > Rect1.m_Bottom)
		Rect1.m_Bottom = Rect2.m_Bottom;

	return Rect1;
}

template <class t>
inline TRect<t>& LessenRect(TRect<t>& Rect, const TSize<t>& Offset)
{
	Rect.m_Left		+= Offset.cx;
	Rect.m_Top		+= Offset.cy;
	Rect.m_Right	-= Offset.cx;
	Rect.m_Bottom	-= Offset.cy;

	return Rect;
}

template <class t>
inline TRect<t>& LessenRect(TRect<t>& Rect, t offset)
{
	Rect.m_Left		+= offset;
	Rect.m_Top		+= offset;
	Rect.m_Right	-= offset;
	Rect.m_Bottom	-= offset;

	return Rect;
}

template <class t>
inline TRect<t>& OrientateRect(TRect<t>& Rect)
{
	t Temp;

	if(Compare(Rect.m_Left, Rect.m_Right) > 0)
		Temp = Rect.m_Left, Rect.m_Left = Rect.m_Right, Rect.m_Right = Temp;

	if(Compare(Rect.m_Top, Rect.m_Bottom) > 0)
		Temp = Rect.m_Top, Rect.m_Top = Rect.m_Bottom, Rect.m_Bottom = Temp;

	return Rect;
}

template <class t>
inline TRect<t>& DefaultizeRect(TRect<t>& Rect, const TSize<t>& MinSize)
{
	TSize<t> Size = Rect;

	if(GetAbs(Size.cx) < MinSize.cx)
		Rect.m_Right = Rect.m_Left + Sign1Dir(Size.cx, MinSize.cx);

	if(GetAbs(Size.cy) < MinSize.cy)
		Rect.m_Bottom = Rect.m_Top + Sign1Dir(Size.cy, MinSize.cy);

	return Rect;
}

template <class t>
inline bool DoRectsIntersect(const TRect<t>& Rect1, const TRect<t>& Rect2)
{
	return	Compare(Rect1.m_Top,	Rect2.m_Bottom)	< 0 &&
			Compare(Rect2.m_Top,	Rect1.m_Bottom)	< 0 &&
			Compare(Rect1.m_Left,	Rect2.m_Right)	< 0 &&
			Compare(Rect2.m_Left,	Rect1.m_Right)	< 0;
}

template <class t>
inline size_t WeldSegments(	TSegment<t>&		Segment,
							const TSegment<t>&	WeldSegment,
							TSegment<t>*		pSegment2 = NULL)
{
	if(!Segment.IsValid())
	{
		if(!WeldSegment.IsValid())
			return 0;

		Segment = WeldSegment;
		return 1;
	}
	
	if(!WeldSegment.IsValid())
		return 1;

	if(	Compare(Segment.m_First, WeldSegment.m_Last) > 0 ||
		Compare(Segment.m_Last, WeldSegment.m_First) < 0)
	{
		if(pSegment2)
			*pSegment2 = WeldSegment;

		return 2;
	}

	if(Compare(WeldSegment.m_First, Segment.m_First) < 0)
		Segment.m_First = WeldSegment.m_First;

	if(Compare(WeldSegment.m_Last, Segment.m_Last) > 0)
		Segment.m_Last = WeldSegment.m_Last;

	return 1;
}

template <class t>
inline size_t SubtractSegment(TSegment<t>& Segment, const TSegment<t>& SubSegment)
{
	if(!Segment.IsValid())
		return 0;

	if(	!SubSegment.IsValid() ||
		(	Compare(Segment.m_Last,  SubSegment.m_First) <= 0 ||
			Compare(Segment.m_First, SubSegment.m_Last)  >= 0))
	{
		return 1;
	}

	if(	Compare(Segment.m_First, SubSegment.m_First) < 0 &&
		Compare(Segment.m_Last,  SubSegment.m_Last)  > 0)
	{
		assert(false); // extra segment required
	}

	if(Compare(Segment.m_First, SubSegment.m_First) < 0)
	{
		Segment.m_Last = SubSegment.m_First;
		return 1;
	}

	if(Compare(Segment.m_Last, SubSegment.m_Last) > 0)
	{
		Segment.m_First = SubSegment.m_Last;
		return 1;
	}

	Segment.Invalidate();

	return 0;
}

template <class t>
inline size_t SubtractSegment(const TSegment<t>& Segment, const TSegment<t>& SubSegment, TSegment<t>* pRSegments)
{
	assert(pRSegments);

	if(!Segment.IsValid())
		return 0;

	if(	!SubSegment.IsValid() ||
		(	Compare(Segment.m_Last,  SubSegment.m_First) <= 0 ||
			Compare(Segment.m_First, SubSegment.m_Last)  >= 0))
	{
		pRSegments[0] = Segment;
		return 1;
	}

	if(	Compare(Segment.m_First, SubSegment.m_First) < 0 &&
		Compare(Segment.m_Last,  SubSegment.m_Last)  > 0)
	{
		pRSegments[0].Set(Segment.m_First, SubSegment.m_First);
		pRSegments[1].Set(SubSegment.m_Last, Segment.m_Last);		
		return 2;
	}

	if(Compare(Segment.m_First, SubSegment.m_First) < 0)
	{
		pRSegments[0].Set(Segment.m_First, SubSegment.m_First);
		return 1;
	}

	if(Compare(Segment.m_Last, SubSegment.m_Last) > 0)
	{
		pRSegments[0].Set(SubSegment.m_Last, Segment.m_Last);
		return 1;
	}

	return 0;
}

template <class t>
inline size_t IntersectSegment(	TSegment<t>&		Segment,
								const TSegment<t>&	IntSegment)
{
	if(!Segment.IsValid())
		return 0;

	if(	!IntSegment.IsValid() ||
		(	Compare(Segment.m_Last,  IntSegment.m_First) <= 0 ||
			Compare(Segment.m_First, IntSegment.m_Last ) >= 0))
	{
		Segment.Invalidate();
		return 0;
	}

	if(Compare(Segment.m_First, IntSegment.m_First) < 0)
		Segment.m_First = IntSegment.m_First;

	if(Compare(Segment.m_Last, IntSegment.m_Last) > 0)
		Segment.m_Last = IntSegment.m_Last;

	assert(Segment.IsValid());

	return 1;
}

template <class t>
inline bool DoSegmentsIntersect(const TSegment<t>& Segment1, const TSegment<t>& Segment2)
{
	return
		Segment1.IsValid() &&
		Segment2.IsValid() &&
		Compare(Segment1.m_Last, Segment2.m_First) > 0 &&
		Compare(Segment2.m_Last, Segment1.m_First) > 0;
}

template <class t1, class t2>
inline TSegment<t1>& ShiftSegment(TSegment<t1>& Segment, t2 Offset)
{
	Segment.m_First = (t1)(Segment.m_First + Offset);
	Segment.m_Last  = (t1)(Segment.m_Last  + Offset);

	return Segment;
}

// --------------------------
// Global alignment routines
// --------------------------
template <class t>
inline t Align(t Value, t Width, TAlignment Alignment)
{
	if(Alignment == ALIGNMENT_MIN)
		return Value;

	if(Alignment == ALIGNMENT_MID)
		return Width / 2;

	if(Alignment == ALIGNMENT_MAX)
		return Width - Value;

	INITIATE_FAILURE;
}

template <class t>
inline t Dealign(t Value, t Width, TAlignment Alignment)
{
	if(Alignment == ALIGNMENT_MIN)
		return Value;

	if(Alignment == ALIGNMENT_MID)
		return Value - Width / 2;

	if(Alignment == ALIGNMENT_MAX)
		return Value - Width;

	INITIATE_FAILURE;
}

template <class t>
inline t GetAlignedOrigin(	const TSegment<t>&	Segment,
							t					Width,
							TAlignment			Alignment)
{
	if(Alignment == ALIGNMENT_MIN)
		return Segment.m_First;

	if(Alignment == ALIGNMENT_MID)
		return (Segment.m_First + Segment.m_Last - Width) / 2;

	if(Alignment == ALIGNMENT_MAX)
		return Segment.m_Last - Width;

	INITIATE_FAILURE;
}

template <class t>
inline TPoint<t> GetAlignedOrigin(	const TRect<t>&	Rect,
									const TSize<t>	Size,
									const ALSIZE&	Alignment)
{
	return TPoint<t>(	GetAlignedOrigin(	TSegment<t>(Rect.m_Left, Rect.m_Right),
											Size.cx,
											Alignment.cx),
						GetAlignedOrigin(	TSegment<t>(Rect.m_Top, Rect.m_Bottom),
											Size.cy,
											Alignment.cy));
}

template <class t>
TSize<t> PointFromSize(const TSize<t>& Size)
	{ return TPoint<t>(Size.cx, Size.cy); }

template <class t>
TSize<t> SizeFromPoint(const TPoint<t>& Point)
	{ return TSize<t>(Point.x, Point.y); }

template <class t>
bool IsRectWithin(const TRect<t>& InnerRect, const TRect<t>& OuterRect)
{
	if(!InnerRect.IsValid())
		return true;

	if(!OuterRect.IsValid())
		return false;

	return	Compare(InnerRect.m_Left,   OuterRect.m_Left  ) >= 0 &&
			Compare(InnerRect.m_Top,    OuterRect.m_Top   ) >= 0 &&
			Compare(InnerRect.m_Right,  OuterRect.m_Right ) <= 0 &&
			Compare(InnerRect.m_Bottom, OuterRect.m_Bottom) <= 0;
}

// -------------------------
// Global make-abs routines
// -------------------------
template <class t>
inline TPoint<t>& MakeAbs(TPoint<t>& Point)
{
	MakeAbs(Point.x);
	MakeAbs(Point.y);

	return Point;
}

template <class t>
inline TSize<t>& MakeAbs(TSize<t>& Size)
{
	MakeAbs(Size.cx);
	MakeAbs(Size.cy);

	return Size;
}

template <class t>
inline TRect<t>& MakeAbs(TRect<t>& Rect)
{
	MakeAbs(Rect.m_Left);
	MakeAbs(Rect.m_Top);
	MakeAbs(Rect.m_Right);
	MakeAbs(Rect.m_Bottom);

	return Rect;
}

template <class t>
inline TSegment<t>& MakeAbs(TSegment<t>& Segment)
{
	MakeAbs(Segment.m_First);
	MakeAbs(Segment.m_Last);

	return Segment;
}

// -------------------------
// Global round-up routines
// -------------------------
template <class t>
inline TPoint<t> RoundUp(TPoint<t>& Point)
{
	RoundUp(Point.x);
	RoundUp(Point.y);

	return Point;
}

template <class t>
inline TSize<t>& RoundUp(TSize<t>& Size)
{
	RoundUp(Size.cx);
	RoundUp(Size.cy);

	return Size;
}

template <class t>
inline TRect<t>& RoundUp(TRect<t>& Rect)
{
	RoundUp(Rect.m_Left);
	RoundUp(Rect.m_Top);
	RoundUp(Rect.m_Right);
	RoundUp(Rect.m_Bottom);

	return Rect;
}

template <class t>
inline TSegment<t>& RoundUp(TSegment<t>& Segment)
{
	RoundUp(Segment.m_First);
	RoundUp(Segment.m_Last);

	return Segment;
}

// ---------------------------
// Global transition routines
// ---------------------------
template <class t>
inline t& GenerateLinearTransition(	const t&	State1,
									const t&	State2,
									float		fTransition,
									t&			RState)
{
	return (((RState = State2) -= State1) *= fTransition) += State1;
}

// ------------------------
// Basic bitypes streaming
// ------------------------
template <class t>
inline TStream& operator >> (TStream& Stream, TPoint<t>& RPoint)
{
	return Stream >> RPoint.x >> RPoint.y;
}

template <class t>
inline TStream& operator << (TStream& Stream, const TPoint<t>& Point)
{
	return Stream << Point.x << Point.y;
}

template <class t>
inline TStream& operator >> (TStream& Stream, TRect<t>& RRect)
{
	return Stream >> RRect.m_Left >> RRect.m_Top >> RRect.m_Right >> RRect.m_Bottom;
}

template <class t>
inline TStream& operator << (TStream& Stream, const TRect<t>& Rect)
{
	return Stream << Rect.m_Left << Rect.m_Top << Rect.m_Right << Rect.m_Bottom;
}

template <class t>
inline TStream& operator >> (TStream& Stream, TSize<t>& RSize)
{
	return Stream >> RSize.cx >> RSize.cy;
}

template <class t>
inline TStream& operator << (TStream& Stream, const TSize<t>& Size)
{
	return Stream << Size.cx << Size.cy;
}

template <class t>
inline TStream& operator >> (TStream& Stream, TSegment<t>& RSegment)
{
	return Stream >> RSegment.m_First >> RSegment.m_Last;
}

template <class t>
inline TStream& operator << (TStream& Stream, const TSegment<t>& Segment)
{
	return Stream << Segment.m_First << Segment.m_Last;
}

// -----------------------
// Cross-type conversions
// -----------------------

// POINT
template <class t>
inline TPoint<t> TO_POINT(const TSize<t>& Size)
	{ return TPoint<t>(Size.cx, Size.cy); }

template <class t>
inline TSize<t> TO_SIZE(const TPoint<t>& Point)
	{ return TSize<t>(Point.x, Point.y); }

// I64
template <class t>
inline I64POINT TO_I64(const TPoint<t>& Point)
{
	return I64POINT((INT64)Point.x, (INT64)Point.y);
}

template <class t>
inline I64RECT TO_I64(const TRect<t>& Rect)
{
	return I64RECT(	(INT64)Rect.m_Left,
					(INT64)Rect.m_Top,
					(INT64)Rect.m_Right,
					(INT64)Rect.m_Bottom);
}

template <class t>
inline I64SIZE TO_I64(const TSize<t>& Size)
{
	return I64SIZE((INT64)Size.cx, (INT64)Size.cy);
}

template <class t>
inline I64SEGMENT TO_I64(const TSegment<t>& Segment)
{
	return I64SEGMENT((INT64)Segment.m_First, (INT64)Segment.m_Last);
}

// L
template <class t>
inline IPOINT TO_L(const TPoint<t>& Point)
{
	return LPOINT((long)Point.x, (long)Point.y);
}

template <class t>
inline IRECT TO_L(const TRect<t>& Rect)
{
	return IRECT(	(long)Rect.m_Left,
					(long)Rect.m_Top,
					(long)Rect.m_Right,
					(long)Rect.m_Bottom);
}

template <class t>
inline ISIZE TO_L(const TSize<t>& Size)
{
	return ISIZE((long)Size.cx, (long)Size.cy);
}

template <class t>
inline ISEGMENT TO_L(const TSegment<t>& Segment)
{
	return ISEGMENT((long)Segment.m_First, (long)Segment.m_Last);
}

// I
template <class t>
inline IPOINT TO_I(const TPoint<t>& Point)
{
	return IPOINT((int)Point.x, (int)Point.y);
}

template <class t>
inline IRECT TO_I(const TRect<t>& Rect)
{
	return IRECT(	(int)Rect.m_Left,
					(int)Rect.m_Top,
					(int)Rect.m_Right,
					(int)Rect.m_Bottom);
}

template <class t>
inline ISIZE TO_I(const TSize<t>& Size)
{
	return ISIZE((int)Size.cx, (int)Size.cy);
}

template <class t>
inline ISEGMENT TO_I(const TSegment<t>& Segment)
{
	return ISEGMENT((int)Segment.m_First, (int)Segment.m_Last);
}

// S
template <class t>
inline SPOINT TO_S(const TPoint<t>& Point)
{
	return SPOINT((short)Point.x, (short)Point.y);
}

template <class t>
inline SRECT TO_S(const TRect<t>& Rect)
{
	return SRECT(	(short)Rect.m_Left,
					(short)Rect.m_Top,
					(short)Rect.m_Right,
					(short)Rect.m_Bottom);
}

template <class t>
inline SSIZE TO_S(const TSize<t>& Size)
{
	return SSIZE((short)Size.cx, (short)Size.cy);
}

template <class t>
inline SSEGMENT TO_S(const TSegment<t>& Segment)
{
	return SSEGMENT((short)Segment.m_First, (short)Segment.m_Last);
}

// QW
template <class t>
inline QWPOINT TO_QW(const TPoint<t>& Point)
{
	return QWPOINT((QWORD)Point.x, (QWORD)Point.y);
}

template <class t>
inline QWRECT TO_QW(const TRect<t>& Rect)
{
	return QWRECT(	(QWORD)Rect.m_Left,
					(QWORD)Rect.m_Top,
					(QWORD)Rect.m_Right,
					(QWORD)Rect.m_Bottom);
}

template <class t>
inline QWSIZE TO_QW(const TSize<t>& Size)
{
	return QWSIZE((QWORD)Size.cx, (QWORD)Size.cy);
}

template <class t>
inline QWSEGMENT TO_QW(const TSegment<t>& Segment)
{
	return QWSEGMENT((QWORD)Segment.m_First, (QWORD)Segment.m_Last);
}

// DW
template <class t>
inline DWPOINT TO_DW(const TPoint<t>& Point)
{
	return DWPOINT((DWORD)Point.x, (DWORD)Point.y);
}

template <class t>
inline DWRECT TO_DW(const TRect<t>& Rect)
{
	return DWRECT(	(DWORD)Rect.m_Left,
					(DWORD)Rect.m_Top,
					(DWORD)Rect.m_Right,
					(DWORD)Rect.m_Bottom);
}

template <class t>
inline DWSIZE TO_DW(const TSize<t>& Size)
{
	return DWSIZE((DWORD)Size.cx, (DWORD)Size.cy);
}

template <class t>
inline DWSEGMENT TO_DW(const TSegment<t>& Segment)
{
	return DWSEGMENT((DWORD)Segment.m_First, (DWORD)Segment.m_Last);
}

// UI
template <class t>
inline SZPOINT TO_UI(const TPoint<t>& Point)
{
	return SZPOINT((UINT)Point.x, (UINT)Point.y);
}

template <class t>
inline SZRECT TO_UI(const TRect<t>& Rect)
{
	return SZRECT(	(UINT)Rect.m_Left,
					(UINT)Rect.m_Top,
					(UINT)Rect.m_Right,
					(UINT)Rect.m_Bottom);
}

template <class t>
inline SZSIZE TO_UI(const TSize<t>& Size)
{
	return SZSIZE((UINT)Size.cx, (UINT)Size.cy);
}

template <class t>
inline SZSEGMENT TO_UI(const TSegment<t>& Segment)
{
	return SZSEGMENT((UINT)Segment.m_First, (UINT)Segment.m_Last);
}

// SZ
template <class t>
inline SZPOINT TO_SZ(const TPoint<t>& Point)
{
	return SZPOINT((size_t)Point.x, (size_t)Point.y);
}

template <class t>
inline SZRECT TO_SZ(const TRect<t>& Rect)
{
	return SZRECT(	(size_t)Rect.m_Left,
					(size_t)Rect.m_Top,
					(size_t)Rect.m_Right,
					(size_t)Rect.m_Bottom);
}

template <class t>
inline SZSIZE TO_SZ(const TSize<t>& Size)
{
	return SZSIZE((size_t)Size.cx, (size_t)Size.cy);
}

template <class t>
inline SZSEGMENT TO_SZ(const TSegment<t>& Segment)
{
	return SZSEGMENT((size_t)Segment.m_First, (size_t)Segment.m_Last);
}

// W
template <class t>
inline WPOINT TO_W(const TPoint<t>& Point)
{
	return WPOINT((WORD)Point.x, (WORD)Point.y);
}

template <class t>
inline WRECT TO_W(const TRect<t>& Rect)
{
	return WRECT(	(WORD)Rect.m_Left,
					(WORD)Rect.m_Top,
					(WORD)Rect.m_Right,
					(WORD)Rect.m_Bottom);
}

template <class t>
inline WSIZE TO_W(const TSize<t>& Size)
{
	return WSIZE((WORD)Size.cx, (WORD)Size.cy);
}

template <class t>
inline WSEGMENT TO_W(const TSegment<t>& Segment)
{
	return WSEGMENT((WORD)Segment.m_First, (WORD)Segment.m_Last);
}

// F
template <class t>
inline FPOINT TO_F(const TPoint<t>& Point)
{
	return FPOINT((float)Point.x, (float)Point.y);
}

template <class t>
inline FRECT TO_F(const TRect<t>& Rect)
{
	return FRECT(	(float)Rect.m_Left,
					(float)Rect.m_Top,
					(float)Rect.m_Right,
					(float)Rect.m_Bottom);
}

template <class t>
inline FSIZE TO_F(const TSize<t>& Size)
{
	return FSIZE((float)Size.cx, (float)Size.cy);
}

template <class t>
inline FSEGMENT TO_F(const TSegment<t>& Segment)
{
	return FSEGMENT((float)Segment.m_First, (float)Segment.m_Last);
}

// D
template <class t>
inline DPOINT TO_D(const TPoint<t>& Point)
{
	return DPOINT((double)Point.x, (double)Point.y);
}

template <class t>
inline DRECT TO_D(const TRect<t>& Rect)
{
	return DRECT(	(double)Rect.m_Left,
					(double)Rect.m_Top,
					(double)Rect.m_Right,
					(double)Rect.m_Bottom);
}

template <class t>
inline DSIZE TO_D(const TSize<t>& Size)
{
	return DSIZE((double)Size.cx, (double)Size.cy);
}

template <class t>
inline DSEGMENT TO_D(const TSegment<t>& Segment)
{
	return DSEGMENT((double)Segment.m_First, (double)Segment.m_Last);
}

// WIN
template <class t>
inline POINT TO_WIN(const TPoint<t>& Point)
{
	POINT  WinPoint = {(int)Point.x, (int)Point.y};
	return WinPoint;
}

template <class t>
inline SIZE TO_WIN(const TSize<t>& Size)
{
	SIZE   WinSize = {(int)Size.cx, (int)Size.cy};
	return WinSize;
}

template <class t>
inline RECT TO_WIN(const TRect<t>& Rect)
{
	RECT   WinRect = {(int)Rect.m_Left, (int)Rect.m_Top, (int)Rect.m_Right, (int)Rect.m_Bottom};
	return WinRect;
}

// Win streaming
DECLARE_BASIC_STREAMING(POINT);
DECLARE_BASIC_STREAMING(RECT);
DECLARE_BASIC_STREAMING(SIZE);

#endif // basic_bitypes_h
