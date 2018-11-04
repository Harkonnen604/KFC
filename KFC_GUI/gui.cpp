#include "kfc_gui_pch.h"
#include "gui.h"

#include "gui_device_globals.h"

// Gradiented progress resolution
#define GPROGRESS_HORIZONTAL_SEGMENTS_NUM	(3)
#define GPROGRESS_VERTICAL_SEGMENTS_NUM		(3)
#define GPROGRESS_FADE_LENGTH				(16)
#define GPROGRESS_FADE_CENTER_COEF			(0.75)

// --------
// Statics
// --------
static volatile LONG s_CursorVisible = true;

// -------
// Region
// -------
TRegion::TRegion()
{
	m_hRegion = NULL;
}

void TRegion::Release()
{
	if(m_hRegion)
		DeleteObject(m_hRegion);
}

void TRegion::Allocate(HRGN hSRegion)
{
	Release();

	try
	{
		DEBUG_VERIFY(hSRegion);

		m_hRegion = hSRegion;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TRegion::AssignToWindow(HWND hWnd, bool bRedraw)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsWindow(hWnd));

	if(!SetWindowRgn(hWnd, *this, bRedraw ? TRUE : FALSE))
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error setting window region"),
										GetLastError());
	}
	
	m_hRegion = NULL;
}

// ------
// Brush
// ------
TBrush::TBrush()
{
	m_hBrush = NULL;
}

TBrush::TBrush(COLORREF crColor)
{
	m_hBrush = NULL;

	Allocate(crColor);
}

TBrush::TBrush(HBITMAP hBitmap)
{
	m_hBrush = NULL;

	Allocate(hBitmap);
}

TBrush::TBrush(HBRUSH hSBrush)
{
	m_hBrush = NULL;

	Allocate(hSBrush);
}

void TBrush::Release()
{
	if(m_hBrush)
		DeleteObject(m_hBrush), m_hBrush = NULL;
}

void TBrush::Allocate(COLORREF crColor)
{
	Release();

	try
	{
		m_hBrush = CreateSolidBrush(crColor);

		if(!m_hBrush)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating solid brush"),
											GetLastError());
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TBrush::Allocate(HBITMAP hBitmap)
{
	Release();

	try
	{
		DEBUG_VERIFY(hBitmap);

		m_hBrush = CreatePatternBrush(hBitmap);

		if(!m_hBrush)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating pattern brush"),
											GetLastError());
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TBrush::Allocate(HBRUSH hSBrush)
{
	Release();

	try
	{
		DEBUG_VERIFY(hSBrush);

		m_hBrush = hSBrush;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

// ----
// Pen
// ----
TPen::TPen()
{
	m_hPen = NULL;
}

TPen::TPen(	COLORREF	crColor,
			size_t		szWidth,
			kmode_t		mdStyle)
{
	m_hPen = NULL;

	Allocate(crColor, szWidth, mdStyle);
}

TPen::TPen(HPEN hSPen)
{
	m_hPen = NULL;

	Allocate(hSPen);
}

void TPen::Release()
{
	if(m_hPen)
		DeleteObject(m_hPen), m_hPen = NULL;
}

void TPen::Allocate(COLORREF	crColor,
					size_t		szWidth,
					kmode_t		mdStyle)
{
	Release();

	try
	{
		m_hPen = CreatePen(mdStyle, szWidth, crColor);
		if(m_hPen == NULL)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating pen"),
											GetLastError());
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TPen::Allocate(HPEN hSPen)
{
	Release();

	try
	{
		DEBUG_VERIFY(hSPen);

		m_hPen = hSPen;
	}
	
	catch(...)
	{
		Release();
		throw;
	}
}

// -------
// Bitmap
// -------
TBitmap::TBitmap()
{
	m_hBitmap = NULL;
}

TBitmap::TBitmap(HDC hDC, const SZSIZE& Size)
{
	m_hBitmap = NULL;

	Allocate(hDC, Size);
}

TBitmap::TBitmap(HBITMAP hSBitmap)
{
	m_hBitmap = NULL;

	Allocate(hSBitmap);
}

void TBitmap::Release()
{
	if(m_hBitmap)
		DeleteObject(m_hBitmap), m_hBitmap = NULL;
}

void TBitmap::Allocate(HDC hDC, const SZSIZE& Size)
{
	Release();

	DEBUG_VERIFY(hDC);

	DEBUG_VERIFY(Size.IsValid());

	m_hBitmap = CreateCompatibleBitmap(hDC, Size.cx, Size.cy);

	if(!m_hBitmap)
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating compatible bitmap."),
										GetLastError());
	}
}

void TBitmap::Allocate(HBITMAP hSBitmap)
{
	Release();

	DEBUG_VERIFY(hSBitmap);

	m_hBitmap = hSBitmap;

}

// ---
// DC
// ---
TDC::TDC()
{
	m_hDC = NULL;
}

TDC::TDC(HWND hSWnd)
{
	m_hDC = NULL;

	Allocate(hSWnd);
}

TDC::TDC(HDC hSDC)
{
	m_hDC = NULL;

	Allocate(hSDC);
}

void TDC::Release()
{
	if(m_hDC)
	{
		if(m_bWindowDC)
			ReleaseDC(m_hWnd, m_hDC);
		else
			DeleteDC(m_hDC);
		
		m_hDC = NULL;
	}
}

void TDC::Allocate(HWND hSWnd)
{
	Release();

	try
	{
		m_hWnd = hSWnd, m_bWindowDC = true;

		m_hDC = ::GetDC(m_hWnd);

		if(m_hDC == NULL)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error getting window device context"),
											GetLastError());
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TDC::Allocate(HDC hSDC)
{
	Release();

	try
	{
		DEBUG_VERIFY(hSDC != NULL);

		m_hDC = hSDC, m_bWindowDC = false;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

// -----------
// DC clipper
// -----------
T_DC_Clipper::T_DC_Clipper(HDC hDC, const IRECT& Rect)
{
	DEBUG_VERIFY(hDC);

	m_hDC = hDC;

	m_hOldRgn = CreateRectRgn(0, 0, 1, 1);
	DEBUG_VERIFY(m_hOldRgn);

	int r = GetClipRgn(hDC, m_hOldRgn);

	if(r <= 0)
	{
		DeleteObject(m_hOldRgn), m_hOldRgn = NULL;

		if(r < 0)
		{
			m_hDC = NULL;
			return;
		}
	}	

	if(IntersectClipRect(	m_hDC,
							Rect.m_Left,
							Rect.m_Top,
							Rect.m_Right,
							Rect.m_Bottom) == ERROR)
	{
		DeleteObject(m_hOldRgn), m_hOldRgn = NULL;
		m_hDC = NULL;
	}
}

T_DC_Clipper::~T_DC_Clipper()
{
	if(!m_hDC)
		return;

	SelectClipRgn(m_hDC, m_hOldRgn);

	if(m_hOldRgn)
		DeleteObject(m_hOldRgn);
}

// ----------------
// Global routines
// ----------------
KString GetKWindowTextSafe(HWND hWnd)
{
	if(!hWnd || !IsWindow(hWnd))
		return TEXT("");

	const size_t szLength = GetWindowTextLength(hWnd);
	
	KString Text;
	Text.Allocate(szLength);

	GetWindowText(hWnd, Text.GetDataPtr(), szLength + 1);

	Text.SetChar(szLength, 0), Text.TrimByZero();

	return Text;
}

KString GetKWindowText(HWND hWnd)
{
	DEBUG_VERIFY(hWnd && IsWindow(hWnd));

	return GetKWindowTextSafe(hWnd);
}

void SetKWindowTextSafe(HWND hWnd, LPCTSTR pText)
{
	if(!IsWindow(hWnd))
		return;

	SetWindowText(hWnd, pText);
}

IRECT GetKClientScreenRect(HWND hWnd)
{
	return KClientToScreen(hWnd, GetKClientRect(hWnd));
}

void SetKClientScreenRect(HWND hWnd, const IRECT& Rect, bool bRedraw)
{
	SetKWindowRect(hWnd, Rect + (GetKWindowRect(hWnd) - GetKClientScreenRect(hWnd)), bRedraw);
}

void SetKWindowText(HWND hWnd, LPCTSTR pText)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	SetKWindowTextSafe(hWnd, pText);
}

static KString GetKCustomListBoxItemText(	HWND	hWnd,
											size_t	szIndex,
											ktype_t	tpDataMessage,
											ktype_t	tpLengthMessage)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	const size_t szLength = (size_t)SendMessage(hWnd, tpLengthMessage, szIndex, 0);

	if(szLength == (size_t)LB_ERR)
	{
		INITIATE_DEFINED_CODE_FAILURE(	"Error retrieving list box item text length",
										GetLastError());
	}

	KString Text;
	Text.Allocate(szLength);
	
	if(	SendMessage(hWnd, tpDataMessage, (WPARAM)szIndex, (LPARAM)Text.GetDataPtr()) ==
		LB_ERR)
	{
		INITIATE_DEFINED_CODE_FAILURE(	"Error retrieving list box item text",
										GetLastError());
	}

	return Text;
}

KString GetKListBoxItemText(HWND hWnd, size_t szIndex)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	return GetKCustomListBoxItemText(hWnd, szIndex, LB_GETTEXT, LB_GETTEXTLEN);
}

KString GetKComboBoxItemText(HWND hWnd, size_t szIndex)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	return GetKCustomListBoxItemText(hWnd, szIndex, CB_GETLBTEXT, CB_GETLBTEXTLEN);
}

size_t GetKListBoxSelection(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	return SendMessage(hWnd, LB_GETCURSEL, 0, 0);
}

KString GetKListControlItemText(HWND hWnd, size_t szIndex, size_t szSubIndex)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	TCHAR Buffer[8192];
	ListView_GetItemText(hWnd, szIndex, szSubIndex, Buffer, sizeof(Buffer) - 1);

	return Buffer;
}

LPARAM GetKListControlItemParam(HWND hWnd, size_t szIndex)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	Item.mask	= LVIF_PARAM;
	Item.iItem	= szIndex;

	ListView_GetItem(hWnd, &Item);

	return Item.lParam;
}

IRECT GetKListControlItemRect(HWND hWnd, size_t szIndex)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	RECT Rect;
	ListView_GetItemRect(hWnd, szIndex, &Rect, LVIR_BOUNDS);

	return Rect;
}

size_t KListControlHitTest(HWND hWnd, const IPOINT& Coords)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	LVHITTESTINFO Info;
	memset(&Info, 0, sizeof(Info));

	Info.pt = *Coords;

	return SendMessage(hWnd, LVM_HITTEST, 0, (LPARAM)&Info);
}

size_t GetKListControlItemCount(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	return ListView_GetItemCount(hWnd);
}

void ChangeKWindowLong(	HWND		hWnd,
						ktype_t		tpType,
						kflags_t	uiSetFlags,
						kflags_t	uiDropFlags)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	SetWindowLong(	hWnd,
					tpType,
					GetWindowLong(hWnd, tpType) & ~uiDropFlags | uiSetFlags);
					
}

bool LimitSizingRect(	IRECT&			Rect,
						const IRECT&	Limits,
						kmode_t			mdKind)
{
	bool bHadEffect = false;

	ISIZE Size(Rect);

	// Y
	if(Size.cy < Limits.m_Top || Size.cy > Limits.m_Bottom)
	{
		const size_t szDesired =	Size.cy < Limits.m_Top ?
										Limits.m_Top :
										Limits.m_Bottom;

		if(	mdKind == WMSZ_TOPLEFT	||
			mdKind == WMSZ_TOP		||
			mdKind == WMSZ_TOPRIGHT)
		{
			Rect.m_Top = Rect.m_Bottom - szDesired;
		}
		else
		{
			Rect.m_Bottom = Rect.m_Top + szDesired;
		}

		bHadEffect = true;
	}

	// X
	if(Size.cx < Limits.m_Left || Size.cx > Limits.m_Right)
	{
		const size_t szDesired =	Size.cx < Limits.m_Left ?
										Limits.m_Left :
										Limits.m_Right;

		if(	mdKind == WMSZ_TOPLEFT	||
			mdKind == WMSZ_LEFT		||
			mdKind == WMSZ_BOTTOMLEFT)
		{
			Rect.m_Left = Rect.m_Right - szDesired;
		}
		else
		{
			Rect.m_Right = Rect.m_Left + szDesired;
		}

		bHadEffect = true;
	}

	return bHadEffect;
}

SZSIZE GetTextSize(HDC hDC, LPCTSTR pText)
{
	DEBUG_VERIFY(hDC);
	DEBUG_VERIFY(pText);

	SZSIZE Size;
	GetTextExtentPoint32(hDC, pText, _tcslen(pText), Size);

	return Size;
}

SZSIZE GetMultiLineTextSize(HDC hDC, LPCTSTR pText)
{
	DEBUG_VERIFY(hDC);
	DEBUG_VERIFY(pText);

	KStrings Rows;
	Rows.SplitString(pText, TEXT("\n"), SSEM_ALL);

	SZSIZE Size(0, 0);	

	for(KStrings::TConstIterator Iter = Rows.GetFirst() ; Iter.IsValid() ; ++Iter)
	{
		const SZSIZE RowSize = GetTextSize(hDC, *Iter);

		if(RowSize.cx > Size.cx)
			Size.cx = RowSize.cx;

		Size.cy += RowSize.cy;
	}

	return Size;
}

IRECT GetWindowPlacementRect(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	WINDOWPLACEMENT wp;
	memset(&wp, 0, sizeof(wp)), wp.length = sizeof(wp);

	GetWindowPlacement(hWnd, &wp);

	return wp.rcNormalPosition;
}

IPOINT& KClientToScreen(HWND hWnd, IPOINT& Point)
{
	if(hWnd)
		ClientToScreen(hWnd, Point);

	return Point;
}

IRECT& KClientToScreen(HWND hWnd, IRECT& Rect)
{	
	if(hWnd)
	{
		Rect = IRECT(	KClientToScreen(hWnd, IPOINT(Rect.m_Left,	Rect.m_Top)),
						KClientToScreen(hWnd, IPOINT(Rect.m_Right,	Rect.m_Bottom)));
	}

	return Rect;
}

IPOINT& KScreenToClient(HWND hWnd, IPOINT& Point)
{
	if(hWnd)
		ScreenToClient(hWnd, Point);
	return Point;
}

IRECT& KScreenToClient(HWND hWnd, IRECT& Rect)
{
	if(hWnd)
	{
		Rect = IRECT(	KScreenToClient(hWnd, IPOINT(Rect.m_Left,	Rect.m_Top)),
						KScreenToClient(hWnd, IPOINT(Rect.m_Right,	Rect.m_Bottom)));
	}

	return Rect;
}

ISIZE& KScreenToClient(HWND hWnd, ISIZE& Size)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	return Size -= ISIZE(GetKWindowRect(hWnd)) - ISIZE(GetKClientRect(hWnd));
}

IPOINT GetKCursorPos()
{
	IPOINT Coords;
	GetCursorPos(Coords);

	return Coords;
}

void SetKCursorPos(const IPOINT& Coords)
{
	SetCursorPos(Coords.x, Coords.y);
}

void KEllipse(HDC hDC, const IRECT& Rect)
{
	DEBUG_VERIFY(hDC);

	Ellipse(hDC, Rect.m_Left, Rect.m_Top, Rect.m_Right, Rect.m_Bottom);
}

void KDrawLine(HDC hDC, const IPOINT& p1, const IPOINT &p2)
{
	DEBUG_VERIFY(hDC);

	MoveToEx(hDC, p1.x, p1.y, NULL), LineTo(hDC, p2.x, p2.y);
}

IRECT GetKClientRect(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	IRECT Rect;
	GetClientRect(hWnd, Rect);
	
	return Rect;
}

IRECT GetKWindowRect(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	IRECT Rect;
	GetWindowRect(hWnd, Rect);
	
	return Rect;
}

void SetKWindowRect(HWND hWnd, const IRECT& Rect, bool bRedraw)
{
	SetWindowPos(	hWnd,
					NULL,
					Rect.m_Left,
					Rect.m_Top,
					Rect.GetWidth (),
					Rect.GetHeight(),
					SWP_NOZORDER | (bRedraw ? 0 : SWP_NOREDRAW));
}

void SetWindowScrollInfo(	HWND			hWnd,
							const IRECT&	Rect,
							const ISIZE&	PageSize,
							const IPOINT*	pCoords)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	DEBUG_VERIFY(Rect.IsFlatValid());

	DEBUG_VERIFY(PageSize.IsFlatPositive());

	// Horizontal
	{
		SCROLLINFO ScrollInfo;
		memset(&ScrollInfo, 0, sizeof(ScrollInfo)), ScrollInfo.cbSize = sizeof(ScrollInfo);

		ScrollInfo.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_HORZ, &ScrollInfo);

		ScrollInfo.nMin		= Rect.m_Left;
		ScrollInfo.nMax		= Rect.m_Right;
		ScrollInfo.nPage	= PageSize.cx;
		ScrollInfo.nPos		= pCoords ? pCoords->x : Rect.m_Left;

		LimitValue(ScrollInfo.nPos, ISEGMENT(ScrollInfo.nMin, ScrollInfo.nMax));

		SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
	}

	// Vertical
	{
		SCROLLINFO ScrollInfo;
		memset(&ScrollInfo, 0, sizeof(ScrollInfo)), ScrollInfo.cbSize = sizeof(ScrollInfo);

		ScrollInfo.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &ScrollInfo);

		ScrollInfo.nMin		= Rect.m_Top;
		ScrollInfo.nMax		= Rect.m_Bottom;
		ScrollInfo.nPage	= PageSize.cy;
		ScrollInfo.nPos		= pCoords ? pCoords->y : Rect.m_Top;

		LimitValue(ScrollInfo.nPos, ISEGMENT(ScrollInfo.nMin, ScrollInfo.nMax));

		SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
	}
}

bool HandleWindowScrolling(	HWND			hWnd,
							UINT			uiMsg,
							WPARAM			wParam,
							LPARAM			lParam,
							const SZSIZE&	Step,
							bool			bRedraw)
{
	DEBUG_VERIFY(IsWindow(hWnd));
	
	// Getting the bar
	ktype_t tpBar;

	size_t szStep;
	
	if(uiMsg == WM_HSCROLL)
	{
		tpBar = SB_HORZ;

		szStep = Step.cx;
	}
	else if(uiMsg == WM_VSCROLL)
	{
		tpBar = SB_VERT;

		szStep = Step.cy;
	}
	else
	{
		return false;
	}
	
	// Getting scroll info
	SCROLLINFO ScrollInfo;
	memset(&ScrollInfo, 0, sizeof(ScrollInfo)), ScrollInfo.cbSize = sizeof(ScrollInfo);

	ScrollInfo.fMask = SIF_ALL;
	GetScrollInfo(hWnd, tpBar, &ScrollInfo);

	// Mode processing
	const kmode_t mdMode = LOWORD(wParam);

	if(mdMode == SB_THUMBPOSITION || mdMode == SB_THUMBTRACK)
		ScrollInfo.nPos = HIWORD(wParam);
	else if(mdMode == SB_LEFT || mdMode == SB_LINELEFT)
		ScrollInfo.nPos -= szStep;
	else if(mdMode == SB_RIGHT || mdMode == SB_LINERIGHT)
		ScrollInfo.nPos += szStep;
	else if(mdMode == SB_PAGELEFT)
		ScrollInfo.nPos -= ScrollInfo.nPage;
	else if(mdMode == SB_PAGERIGHT)
		ScrollInfo.nPos += ScrollInfo.nPage;

	// Bounding scroll pos
	LimitValue(ScrollInfo.nPos, ISEGMENT(ScrollInfo.nMin, ScrollInfo.nMax));

	// New scroll info
	ScrollInfo.fMask = SIF_POS;

	SetScrollInfo(hWnd, tpBar, &ScrollInfo, bRedraw ? TRUE : FALSE);

	return true;
}

int GetScrollPos32(HWND hWnd, ktype_t tpType)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	SCROLLINFO ScrollInfo;
	memset(&ScrollInfo, 0, sizeof(ScrollInfo)), ScrollInfo.cbSize = sizeof(ScrollInfo);

	ScrollInfo.fMask = SIF_POS;
	GetScrollInfo(hWnd, tpType, &ScrollInfo);

	return ScrollInfo.nPos;
}

ISIZE GetScrollPos32(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	return ISIZE(	GetScrollPos32(hWnd, SB_HORZ),
					GetScrollPos32(hWnd, SB_VERT));
}

size_t GetSelectionAfterDeletion(size_t szNewCount, size_t szOldIndex)
{
	return szOldIndex < szNewCount ? szOldIndex : szOldIndex - 1;
}

bool IsKeyDown(int iKey)
{
	return (GetKeyState(iKey) & 0x8000) ? true : false;
}

bool GetKCheck(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));
	
	return SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void SetKCheck(HWND hWnd, bool bCheck)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	SendMessage(hWnd, BM_SETCHECK, bCheck ? BST_CHECKED : BST_UNCHECKED, 0);
}

void ClearIPControlAddress(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	SendMessage(hWnd, IPM_CLEARADDRESS, 0, 0);
}

DWORD GetIPControlAddressDirect(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	DWORD dwAddress;
	SendMessage(hWnd, IPM_GETADDRESS, 0, (LPARAM)&dwAddress);

	return dwAddress;
}

void SetIPControlAddressDirect(HWND hWnd, DWORD dwAddress)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	SendMessage(hWnd, IPM_SETADDRESS, 0, dwAddress);
}

DWORD GetIPControlAddress(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	DWORD dwAddress = GetIPControlAddressDirect(hWnd);	

	dwAddress = (((dwAddress)		& 0xFF)	<< 24)	|
				(((dwAddress >> 8)	& 0xFF)	<< 16)	|
				(((dwAddress >> 16)	& 0xFF)	<< 8)	|
				(((dwAddress >> 24)	& 0xFF));

	return dwAddress;
}

void SetIPControlAddress(HWND hWnd, DWORD dwAddress)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	dwAddress = (((dwAddress)		& 0xFF)	<< 24)	|
				(((dwAddress >> 8)	& 0xFF)	<< 16)	|
				(((dwAddress >> 16)	& 0xFF)	<< 8)	|
				(((dwAddress >> 24)	& 0xFF));

	SetIPControlAddressDirect(hWnd, dwAddress);
}

void ShowCursorNoAcc(bool bShow)
{
	if((bool)InterlockedExchange(&s_CursorVisible, bShow) != bShow)
		ShowCursor(bShow);
}

void TrackMouseLeave(HWND hWnd)
{
	DEBUG_VERIFY(IsWindow(hWnd));

	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(tme)), tme.cbSize = sizeof(tme);

	tme.hwndTrack	= hWnd;
	tme.dwFlags		= TME_LEAVE;

	DEBUG_EVALUATE_VERIFY(_TrackMouseEvent(&tme));
}

void InvertCheckerRect(HDC hDC, const IRECT& Rect)
{
	DEBUG_VERIFY(hDC);

	if(!Rect.IsValid())
		return;

	TGDIObjectSelector Selector0(hDC, g_GUI_DeviceGlobals.m_CheckerBrush);

	PatBlt(	hDC,
			Rect.m_Left,
			Rect.m_Top,
			Rect.GetWidth (),
			Rect.GetHeight(),
			PATINVERT);
}

void ReleaseCapture(HWND hWnd)
{
	if(GetCapture() == hWnd)
		ReleaseCapture();
}

SZSIZE GetBitmapSize(HBITMAP hBitmap)
{
	DEBUG_VERIFY(hBitmap);

	BITMAPINFO bi;
	memset(&bi, 0, sizeof(bi)), bi.bmiHeader.biSize = sizeof(bi.bmiHeader);

	TDC DC((HWND)NULL);

	KFC_VERIFY(GetDIBits(DC, hBitmap, 0, 0, NULL, &bi, DIB_RGB_COLORS));

	return SZSIZE(abs(bi.bmiHeader.biWidth), abs(bi.bmiHeader.biHeight));
}

bool AreAllParentsEnabled(HWND hWnd, bool bChildOnly)
{
	DEBUG_VERIFY(hWnd);

	for(;;)
	{
		if(bChildOnly && !(GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD))
			break;

		if(!(hWnd = GetParent(hWnd)))
			break;

		if(!IsWindowEnabled(hWnd))
			return false;
	}

	return true;
}

// ------
// XFORM
// ------
FXFORM g_FNullWorldTransform = {};
DXFORM g_DNullWorldTransform = {};

FXFORM g_FIdentityWorldTransform = {1, 0, 0, 1, 0, 0};
DXFORM g_DIdentityWorldTransform = {1, 0, 0, 1, 0, 0};

FXFORM InvertWT(const FXFORM& wt)
{
	float d = wt.eM11 * wt.eM22 - wt.eM12 * wt.eM21;

	if(IsZero(d))
	{
		FXFORM rwt = {};
		return rwt;
	}

	d = 1.0f / d;

	FXFORM rwt =
	{	+wt.eM22 * d, -wt.eM12 * d,
		-wt.eM21 * d, +wt.eM11 * d,
		(+wt.eM21 * wt.eDy - wt.eM22 * wt.eDx) * d,
		(-wt.eM11 * wt.eDy + wt.eM12 * wt.eDx) * d};

	return rwt;
}

DXFORM InvertWT(const DXFORM& wt)
{
	double d = wt.eM11 * wt.eM22 - wt.eM12 * wt.eM21;

	if(IsZero(d))
	{
		DXFORM rwt = {};
		return rwt;
	}

	d = 1.0 / d;

	DXFORM rwt =
	{	+wt.eM22 * d, -wt.eM12 * d,
		-wt.eM21 * d, +wt.eM11 * d,
		+(wt.eM21 * wt.eDy - wt.eM22 * wt.eDx) * d,
		-(wt.eM11 * wt.eDy - wt.eM12 * wt.eDx) * d};

	return rwt;
}
