#ifndef gui_h
#define gui_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\kstring.h>

// ---------------------
// Destroy window guard
// ---------------------
class TDestroyWindowGuard
{
private:
	HWND& m_hWnd;

public:
	TDestroyWindowGuard(HWND& hWnd) : m_hWnd(hWnd) {}

	~TDestroyWindowGuard()
		{ if(m_hWnd) DestroyWindow(m_hWnd), m_hWnd = NULL; }
};

// -------
// Region
// -------
class TRegion
{
private:
	HRGN m_hRegion;

private:
	TRegion(const TRegion&);

	TRegion& operator = (const TRegion&);

public:
	TRegion();

	~TRegion()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hRegion; }

	void Release();

	void Allocate(HRGN hSRegion);

	void AssignToWindow(HWND hWnd, bool bRedraw = true);

	HRGN GetRegion() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hRegion; }

	operator HRGN () const
		{ return GetRegion(); }
};

// ------
// Brush
// ------
class TBrush
{
private:
	HBRUSH m_hBrush;

private:
	TBrush(const TBrush&);

	TBrush& operator = (const TBrush&);

public:
	TBrush();

	TBrush(COLORREF crColor);

	TBrush(HBITMAP hBitmap);

	TBrush(HBRUSH hSBhrush);

	~TBrush()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hBrush; }

	void Release();

	void Allocate(COLORREF crColor);

	void Allocate(HBITMAP hBitmap);
	
	void Allocate(HBRUSH hSBrush);

	HBRUSH GetBrush() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hBrush; }

	operator HBRUSH () const
		{ return GetBrush(); }
};

// ----
// Pen
// ----
class TPen
{
private:
	HPEN m_hPen;

private:
	TPen(const TPen&);

	TPen& operator = (const TPen&);
	
public:
	TPen();

	TPen(	COLORREF	crColor,
			size_t		szWidth = 1,
			kmode_t		mdStyle	= PS_SOLID);

	TPen(HPEN hSPen);

	~TPen()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hPen; }

	void Release();
	
	void Allocate(	COLORREF	crColor,
					size_t		szWidth = 1,
					kmode_t		mdStyle	= PS_SOLID);

	void Allocate(HPEN hSPen);

	HPEN GetPen() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hPen; }

	operator HPEN() const
		{ return GetPen(); }
};

// -------
// Bitmap
// -------
class TBitmap
{
private:
	HBITMAP m_hBitmap;
	
private:
	TBitmap(const TBitmap&);

	TBitmap& operator = (const TBitmap&);

public:
	TBitmap();

	TBitmap(HDC hDC, const SZSIZE& Size);

	TBitmap(HBITMAP hSBitmap);

	~TBitmap()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hBitmap; }

	void Release();

	void Allocate(HDC hDC, const SZSIZE& Size);

	void Allocate(HBITMAP hSBitmap);

	HBITMAP GetBitmap() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hBitmap; }

	operator HBITMAP () const
		{ return GetBitmap(); }
};

// ---
// DC
// ---
class TDC
{
private:
	HDC		m_hDC;
	HWND	m_hWnd;

	bool m_bWindowDC;

private:
	TDC(const TDC&);

	TDC& operator = (const TDC&);

public:
	TDC();
	TDC(HWND	hSWnd);
	TDC(HDC		hSDC);
	
	~TDC()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hDC; }

	void Release();

	void Allocate(HWND hSWnd);

	void Allocate(HDC hSDC);

	HDC GetDC() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hDC; }

	operator HDC () const
		{ return GetDC(); };

	HWND GetWnd() const
		{ return m_hWnd; }

	bool IsWindowDC() const
		{ return m_bWindowDC; }
};

// ---------
// Paint DC
// ---------
class TPaintDC
{
private:
	HWND m_hWnd;
	
	PAINTSTRUCT m_PaintStruct;

private:
	TPaintDC(const TPaintDC&);

	TPaintDC& operator = (const TPaintDC&);
	
public:
	TPaintDC(HWND hSWnd)
	{
		DEBUG_VERIFY(hSWnd);

		BeginPaint(m_hWnd = hSWnd, &m_PaintStruct);
	}

	~TPaintDC()
	{
		EndPaint(m_hWnd, &m_PaintStruct);
	}

	HDC GetDC() const
		{ return m_PaintStruct.hdc; }

	operator HDC () const
		{ return GetDC(); }

	HWND GetWnd() const
		{ return m_hWnd; }
	
	const PAINTSTRUCT& GetPaintStruct() const
		{ return m_PaintStruct; }
};

// --------------------
// GDI object selector
// --------------------
class TGDIObjectSelector
{
private:
	HDC m_hDC;

	HGDIOBJ m_hOldObject;

public:
	TGDIObjectSelector(HDC hSDC, HGDIOBJ hNewObject)
	{
		DEBUG_VERIFY(hSDC);

		DEBUG_VERIFY(hNewObject);

		m_hOldObject = SelectObject(m_hDC = hSDC, hNewObject);
	}

	~TGDIObjectSelector()
	{
		if(m_hOldObject)
			SelectObject(m_hDC, m_hOldObject);
	}
};

// --------------------------
// Stretch blt mode selector
// --------------------------
class TStretchBltModeSelector
{
private:
	HDC m_hDC;

	int m_iOldStretchMode;

public:
	TStretchBltModeSelector(HDC hSDC, int iNewStretchMode)
	{
		DEBUG_VERIFY(hSDC);

		m_iOldStretchMode = SetStretchBltMode(m_hDC = hSDC, iNewStretchMode);
	}

	~TStretchBltModeSelector()
	{
		if(m_iOldStretchMode)
			SetStretchBltMode(m_hDC, m_iOldStretchMode);
	}
};

// ------------------------
// Poly fill mode selector
// ------------------------
class TPolyFillModeSelector
{
private:
	HDC m_hDC;

	int m_iOldPolyFillMode;

public:
	TPolyFillModeSelector(HDC hSDC, int iNewPolyFillMode)
	{
		DEBUG_VERIFY(hSDC);

		DEBUG_VERIFY(iNewPolyFillMode);

		m_iOldPolyFillMode = SetPolyFillMode(m_hDC = hSDC, iNewPolyFillMode);
	}

	~TPolyFillModeSelector()
	{
		if(m_iOldPolyFillMode)
			SetPolyFillMode(m_hDC, m_iOldPolyFillMode);
	}
};

// -----------------
// Bk mode selector
// -----------------
class TBkModeSelector
{
private:
	HDC m_hDC;

	int m_iOldMode;

public:
	TBkModeSelector(HDC hSDC, int iNewMode)
	{
		DEBUG_VERIFY(hSDC);

		DEBUG_VERIFY(iNewMode);

		m_iOldMode = SetBkMode(m_hDC = hSDC, iNewMode);
	}

	~TBkModeSelector()
	{
		if(m_iOldMode)
			SetBkMode(m_hDC, m_iOldMode);
	}
};

// ------------------
// Bk color selector
// ------------------
class TBkColorSelector
{
private:
	HDC m_hDC;

	COLORREF m_crOldColor;

public:
	TBkColorSelector(HDC hSDC, COLORREF crNewColor)
	{
		DEBUG_VERIFY(hSDC);

		DEBUG_VERIFY(crNewColor != CLR_INVALID);

		m_crOldColor = SetBkColor(m_hDC = hSDC, crNewColor);
	}

	~TBkColorSelector()
	{
		if(m_crOldColor != CLR_INVALID)
			SetBkColor(m_hDC, m_crOldColor);
	}
};

// --------------------
// Text color selector
// --------------------
class TTextColorSelector
{
private:
	HDC m_hDC;

	COLORREF m_crOldColor;

public:
	TTextColorSelector(HDC hSDC, COLORREF crNewColor)
	{
		DEBUG_VERIFY(hSDC);

		DEBUG_VERIFY(crNewColor != CLR_INVALID);

		m_crOldColor = SetTextColor(m_hDC = hSDC, crNewColor);
	}

	~TTextColorSelector()
	{
		if(m_crOldColor != CLR_INVALID)
			SetTextColor(m_hDC, m_crOldColor);
	}
};

// ------------------
// Map mode selector
// ------------------
class TMapModeSelector
{
private:
	HDC m_hDC;

	int m_iOldMode;

public:
	TMapModeSelector(HDC hSDC, int iNewMode)
	{
		DEBUG_VERIFY(hSDC);

		DEBUG_VERIFY(iNewMode);

		m_iOldMode = SetMapMode(m_hDC = hSDC, iNewMode);
	}

	~TMapModeSelector()
	{
		if(m_iOldMode)
			SetMapMode(m_hDC, m_iOldMode);
	}
};

// -----------------------
// World transform setter
// -----------------------
class TWorldTransformSetter
{
private:
	HDC m_hDC;

	XFORM m_OldWT;

public:
	TWorldTransformSetter(HDC hSDC, const XFORM& NewWT)
	{
		DEBUG_VERIFY(hSDC);

		m_hDC = hSDC;

		GetWorldTransform(m_hDC, &m_OldWT);

		SetWorldTransform(m_hDC, &NewWT);
	}

	~TWorldTransformSetter()
	{
		SetWorldTransform(m_hDC, &m_OldWT);
	}
};

// -----------
// DC clipper
// -----------
class T_DC_Clipper
{
private:
	HDC m_hDC;

	HRGN m_hOldRgn;

public:
	T_DC_Clipper(HDC hDC, const IRECT& Rect);

	~T_DC_Clipper();
};

// -----------------
// Window placement
// -----------------
struct TWindowPlacement
{
	IPOINT m_Coords;
	SZSIZE m_Size;
	
	TWindowPlacement() {}

	TWindowPlacement(const IPOINT& SCoords, const SZSIZE& SSize) :	m_Coords(SCoords),
																	m_Size(SSize) {}

	TWindowPlacement(const IRECT& Rect) :	m_Coords(	Rect.m_Left,
														Rect.m_Right),

											m_Size(	Rect.m_Right - Rect.m_Left,
													Rect.m_Bottom - Rect.m_Top) {}
};

// ---------------
// Focus restorer
// ---------------
class TFocusRestorer
{
private:
	HWND m_hWnd;

public:
	TFocusRestorer(HWND hWnd = ::GetFocus()) : m_hWnd(hWnd) {}

	~TFocusRestorer()
		{ SetFocus(m_hWnd); }
};

// ---------------
// Window enabler
// ---------------
class TWindowEnabler
{
private:
	HWND m_hWnd;
	bool m_bOldState;

public:
	TWindowEnabler(HWND hWnd, bool bState)
	{
		DEBUG_VERIFY(hWnd);

		m_hWnd = hWnd;

		m_bOldState = IsWindowEnabled(m_hWnd);
		EnableWindow(m_hWnd, bState);
	}

	~TWindowEnabler()
	{
		EnableWindow(m_hWnd, m_bOldState);
	}
};

// ----------------
// Global routines
// ----------------
KString GetKWindowTextSafe	(HWND hWnd);
KString GetKWindowText		(HWND hWnd);

void SetKWindowTextSafe	(HWND hWnd, LPCTSTR pText);
void SetKWindowText		(HWND hWnd, LPCTSTR pText);

KString GetKListBoxItemText	(HWND hWnd, size_t szIndex);
KString GetKComboBoxItemText(HWND hWnd, size_t szIndex);

size_t GetKListBoxSelection(HWND hWnd);

KString GetKListControlItemText(HWND hWnd, size_t szIndex, size_t szSubIndex = 0);

LPARAM GetKListControlItemParam(HWND hWnd, size_t szIndex);

IRECT GetKListControlItemRect(HWND hWnd, size_t szIndex);

size_t KListControlHitTest(HWND hWnd, const IPOINT& Coords);

size_t GetKListControlItemCount(HWND hWnd);

void ChangeKWindowLong(	HWND		hWnd,
						ktype_t		tpType,
						kflags_t	uiSetFlags,
						kflags_t	uiDropFlags);

bool LimitSizingRect(	IRECT&			Rect,
						const IRECT&	Limits,
						kmode_t			mdKind);

SZSIZE GetTextSize(HDC hDC, LPCTSTR pText);

SZSIZE GetMultiLineTextSize(HDC hDC, LPCTSTR pText);

IRECT GetWindowPlacementRect(HWND hWnd);

IPOINT& KClientToScreen(HWND hWnd, IPOINT&	Point);
IRECT&  KClientToScreen(HWND hWnd, IRECT&	Rect);

IPOINT&	KScreenToClient(HWND hWnd, IPOINT&	Point);
IRECT&	KScreenToClient(HWND hWnd, IRECT&	Rect);

IPOINT GetKCursorPos();

void SetKCursorPos(const IPOINT& Coords);

void KEllipse(HDC hDC, const IRECT& Rect);

void KDrawLine(HDC hDC, const IPOINT& p1, const IPOINT &p2);

IRECT GetKClientRect(HWND hWnd);
IRECT GetKWindowRect(HWND hWnd);

void SetKWindowRect(HWND hWnd, const IRECT& Rect, bool bRedraw = true);

IRECT GetKClientScreenRect(HWND hWnd);

void SetKClientScreenRect(HWND hWnd, const IRECT& Rect, bool bRedraw = true);

inline IRECT GetKScreenRect(HWND hWnd)
	{ return GetKWindowRect(hWnd); }

inline HWND KWindowFromPoint(const IPOINT& Coords)
	{ return WindowFromPoint(*(const POINT*)Coords); }

void SetWindowScrollInfo(	HWND			hWnd,
							const IRECT&	Rect,
							const ISIZE&	PageSize,
							const IPOINT*	pCoords = NULL); // NULL for top-left

bool HandleWindowScrolling(	HWND			hWnd,
							UINT			uiMsg,
							WPARAM			wParam,
							LPARAM			lParam,
							const SZSIZE&	Step,
							bool			bRedraw = true);

int GetScrollPos32(HWND hWnd, ktype_t tpType);

ISIZE GetScrollPos32(HWND hWnd);

size_t GetSelectionAfterDeletion(size_t szNewCount, size_t szOldIndex);

bool IsKeyDown(int iKey);

bool GetKCheck(HWND hWnd);

void SetKCheck(HWND hWnd, bool bCheck);

void ClearIPControlAddress(HWND hWnd);

DWORD GetIPControlAddressDirect(HWND hWnd);

void SetIPControlAddressDirect(HWND hWnd, DWORD dwAddress);

DWORD GetIPControlAddress(HWND hWnd);

void SetIPControlAddress(HWND hWnd, DWORD dwAddress);

void ShowCursorNoAcc(bool bShow);

inline bool IsWindowRestored(HWND hWnd)
	{ return GetWindowLong(hWnd, GWL_STYLE) & WS_MINIMIZE; }

inline bool IsWindowForeground(HWND hWnd, bool bAllowMinimizedActive = false)
	{ return GetForegroundWindow() == hWnd && (bAllowMinimizedActive || IsWindowRestored(hWnd)); }

inline HICON KLoadIcon(int iID)
	{ return LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(iID)); }

inline HCURSOR KLoadCursor(int iID)
	{ return LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(iID)); }

inline HBITMAP KLoadBitmap(int iID)
	{ return LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(iID)); }

void TrackMouseLeave(HWND hWnd);

void InvertCheckerRect(HDC hDC, const IRECT& Rect);

void ReleaseCapture(HWND hWnd);

SZSIZE GetBitmapSize(HBITMAP hBitmap);

bool AreAllParentsEnabled(HWND hWnd, bool bChildOnly = true);

// ------
// XFORM
// ------
typedef XFORM FXFORM;

struct DXFORM
{
	double eM11, eM12, eM21, eM22, eDx, eDy;
};

extern FXFORM g_FNullWorldTransform;
extern DXFORM g_DNullWorldTransform;

extern FXFORM g_FIdentityWorldTransform;
extern DXFORM g_DIdentityWorldTransform;

inline bool operator == (const FXFORM& wt1, const FXFORM& wt2)
{
	return	!Compare(wt1.eM11, wt2.eM11) &&
			!Compare(wt1.eM12, wt2.eM12) &&
			!Compare(wt1.eM21, wt2.eM21) &&
			!Compare(wt1.eM22, wt2.eM22) &&
			!Compare(wt1.eDx,  wt2.eDx)  &&
			!Compare(wt1.eDy,  wt2.eDy);
}

inline bool operator == (const DXFORM& wt1, const DXFORM& wt2)
{
	return	!Compare(wt1.eM11, wt2.eM11) &&
			!Compare(wt1.eM12, wt2.eM12) &&
			!Compare(wt1.eM21, wt2.eM21) &&
			!Compare(wt1.eM22, wt2.eM22) &&
			!Compare(wt1.eDx,  wt2.eDx)  &&
			!Compare(wt1.eDy,  wt2.eDy);
}

inline bool operator != (const FXFORM& wt1, const FXFORM& wt2)
	{ return !(wt1 == wt2); }

inline bool operator != (const DXFORM& wt1, const DXFORM& wt2)
	{ return !(wt1 == wt2); }

inline bool operator ! (const FXFORM& wt)
	{ return wt == g_FNullWorldTransform; }

inline bool operator ! (const DXFORM& wt)
	{ return wt == g_DNullWorldTransform; }

inline XFORM operator * (const FXFORM& wt1, const FXFORM& wt2)
{
	FXFORM wt = {	wt1.eM11 * wt2.eM11 + wt1.eM12 * wt2.eM21,
					wt1.eM11 * wt2.eM12 + wt1.eM12 * wt2.eM22,
					wt1.eM21 * wt2.eM11 + wt1.eM22 * wt2.eM21,
					wt1.eM21 * wt2.eM12 + wt1.eM22 * wt2.eM22,
					wt1.eDx  * wt2.eM11 + wt1.eDy  * wt2.eM21 + wt2.eDx,
					wt1.eDx  * wt2.eM12 + wt1.eDy  * wt2.eM22 + wt2.eDy};

	return wt;
}

inline DXFORM operator * (const DXFORM& wt1, const DXFORM& wt2)
{
	DXFORM wt = {	wt1.eM11 * wt2.eM11 + wt1.eM12 * wt2.eM21,
					wt1.eM11 * wt2.eM12 + wt1.eM12 * wt2.eM22,
					wt1.eM21 * wt2.eM11 + wt1.eM22 * wt2.eM21,
					wt1.eM21 * wt2.eM12 + wt1.eM22 * wt2.eM22,
					wt1.eDx  * wt2.eM11 + wt1.eDy  * wt2.eM21 + wt2.eDx,
					wt1.eDx  * wt2.eM12 + wt1.eDy  * wt2.eM22 + wt2.eDy};

	return wt;
}

inline XFORM& operator *= (FXFORM& wt1, const FXFORM& wt2)
	{ return wt1 = wt1 * wt2; }

inline DXFORM& operator *= (DXFORM& wt1, const DXFORM& wt2)
	{ return wt1 = wt1 * wt2; }

template <class t>
inline TPoint<t> operator * (const TPoint<t>& p, const FXFORM& wt)
	{ return TPoint<t>((t)(p.x * wt.eM11 + p.y * wt.eM21 + wt.eDx), (t)(p.x * wt.eM12 + p.y * wt.eM22 + wt.eDy)); }

template <class t>
inline TPoint<t> operator * (const TPoint<t>& p, const DXFORM& wt)
	{ return TPoint<t>((t)(p.x * wt.eM11 + p.y * wt.eM21 + wt.eDx), (t)(p.x * wt.eM12 + p.y * wt.eM22 + wt.eDy)); }

template <class t>
inline TPoint<t>& operator *= (TPoint<t>& p, const FXFORM& wt)
	{ return p = p * wt; }

template <class t>
inline TPoint<t>& operator *= (TPoint<t>& p, const DXFORM& wt)
	{ return p = p * wt; }

template <class t>
inline TSize<t> operator * (const TSize<t>& s, const FXFORM& wt)
	{ return TSize<t>((t)(s.cx * wt.eM11 + s.cy * wt.eM21), (t)(s.cx * wt.eM12 + s.cy * wt.eM22)); }

template <class t>
inline TSize<t> operator * (const TSize<t>& s, const DXFORM& wt)
	{ return TSize<t>((t)(s.cx * wt.eM11 + s.cy * wt.eM21), (t)(s.cx * wt.eM12 + s.cy * wt.eM22)); }

template <class t>
inline TSize<t>& operator *= (TSize<t>& s, const FXFORM& wt)
	{ return s = s * wt; }

template <class t>
inline TSize<t>& operator *= (TSize<t>& s, const DXFORM& wt)
	{ return s = s * wt; }

inline float GetWT_ScaleFactor(const FXFORM& wt)
	{ return sqrt((FSIZE(1, 1) * wt).GetLengthSquare() * 0.5f); }

inline double GetWT_ScaleFactor(const DXFORM& wt)
	{ return sqrt((DSIZE(1, 1) * wt).GetLengthSquare() * 0.5); }

FXFORM InvertWT(const FXFORM& wt);
DXFORM InvertWT(const DXFORM& wt);

#endif // gui_h
