#include "kfc_gui_pch.h"
#include "list_control.h"

#include "gui.h"
#include "window_subclasser.h"

// -------------
// List control
// -------------
TListControl::TListControl()
{
	m_hWnd = NULL;
}

void TListControl::Release()
{
	if(m_hWnd)
		DestroyWindow(m_hWnd), m_hWnd = NULL;
}

void TListControl::Create(	HWND			hParentWnd,
							const IRECT&	Rect,
							bool			bSingleSelection,
							const THeaders&	Headers,
							DWORD			dwSetStyle,
							DWORD			dwDropStyle,
							DWORD			dwBaseStyle)
{
	Release();

	try
	{
		DEBUG_VERIFY(!Headers.IsEmpty());

		(dwBaseStyle |= dwSetStyle) &= ~dwDropStyle;

		m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
								WC_LISTVIEW,
								TEXT(""),
								WS_VISIBLE | (hParentWnd ? WS_CHILD : WS_POPUP) |
									LVS_REPORT | LVS_SHAREIMAGELISTS |
									(bSingleSelection ? LVS_SINGLESEL : 0) |
									dwBaseStyle,
								Rect.m_Left,
								Rect.m_Top,
								Rect.GetWidth	(),
								Rect.GetHeight	(),
								hParentWnd,
								NULL,
								GetModuleHandle(NULL),
								NULL);

		if(!m_hWnd)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating list view control"),
											GetLastError());
		}

		SubclassWindow(m_hWnd, *this, &TListControl::WindowProc);

		ListView_SetExtendedListViewStyle
			(*this, ListView_GetExtendedListViewStyle(*this) | LVS_EX_FULLROWSELECT);

		SetEmptyText(NULL);

		{
			size_t szTotalWidth = 0;

			for(size_t i = 0 ; i < Headers.GetN() ; i++)
			{
				LVCOLUMN Column;
				memset(&Column, 0, sizeof(Column));

				KString Title = Headers[i].m_Title;

				Column.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT;

				switch(Headers[i].m_Alignment)
				{
				case ALIGNMENT_MIN:
					Column.fmt |= LVCFMT_LEFT;
					break;

				case ALIGNMENT_MAX:
					Column.fmt |= LVCFMT_RIGHT;
					break;

				case ALIGNMENT_MID:
					Column.fmt |= LVCFMT_CENTER;
					break;

				default:
					INITIATE_FAILURE;
				}

				Column.cx =
					i == Headers.GetLast() && Headers[i].m_szWidth == UINT_MAX ?
						GetKClientRect(m_hWnd).GetWidth() - szTotalWidth :
						Headers[i].m_szWidth;

				Column.pszText = Title.GetDataPtr();

				ListView_InsertColumn(m_hWnd, i, &Column);

				szTotalWidth += Headers[i].m_szWidth;
			}

			if(!Headers.IsEmpty() && Headers.GetLastItem().m_szWidth == UINT_MAX)
				FixLastColumn();
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TListControl::SetEmptyText(LPCTSTR pText, COLORREF crColor)
{
	DEBUG_VERIFY_ALLOCATION;

	m_EmptyText = pText ? pText : "", m_crEmptyTextColor = crColor;

	if(IsEmpty())
		InvalidateRect(*this, NULL, TRUE);
}

void TListControl::FixLastColumn()
{
	size_t i;

	DEBUG_VERIFY_ALLOCATION;

	int iWidth = 0;

	size_t szNColumns = GetNColumns();
	DEBUG_VERIFY(szNColumns);

	LVCOLUMN HeaderColumn;
	memset(&HeaderColumn, 0, sizeof(HeaderColumn));

	HeaderColumn.mask = LVCF_WIDTH;

	// Getting total width of all columns except for the last
	for(i = 0 ; i < szNColumns - 1 ; i++)
	{
		DEBUG_EVALUATE_VERIFY(ListView_GetColumn(*this, i, &HeaderColumn));

		iWidth += HeaderColumn.cx;
	}

	// Adjusting last column width
	iWidth = ISIZE(GetKClientRect(*this)).cx - iWidth;

	if(iWidth > 0)
	{	
		LVCOLUMN HeaderColumn;
		memset(&HeaderColumn, 0, sizeof(HeaderColumn));

		HeaderColumn.mask	= LVCF_WIDTH;
		HeaderColumn.cx		= iWidth;

		ListView_SetColumn(*this, szNColumns - 1, &HeaderColumn);
	}
}

void TListControl::SetImageList(HIMAGELIST hImageList, ktype_t tpType)
{
	DEBUG_VERIFY_ALLOCATION;

	ListView_SetImageList(*this, hImageList, tpType);
}

void TListControl::Clear()
{
	DEBUG_VERIFY_ALLOCATION;

	ListView_DeleteAllItems(*this);

	if(!m_EmptyText.IsEmpty())
	{
		DEBUG_VERIFY(IsEmpty());
		InvalidateRect(*this, NULL, TRUE);
	}
}

int TListControl::AddItem(	LPCTSTR	pText,
							int		iImage,
							size_t	szPos)
{
	DEBUG_VERIFY_ALLOCATION;

	if(szPos == UINT_MAX)
		szPos = GetN();

	bool bWasEmpty = IsEmpty();

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));	

	DEBUG_VERIFY(szPos <= GetN());

	Item.iItem = szPos;

	if(pText)
		Item.mask |= LVIF_TEXT, Item.pszText = (LPSTR)pText;

	if(iImage >= 0)
		Item.mask |= LVIF_IMAGE, Item.iImage = iImage;

	int iItem = ListView_InsertItem(*this, &Item);

	if(iItem < 0)
		INITIATE_DEFINED_FAILURE(TEXT("Error adding list view item."));

	if(!m_EmptyText.IsEmpty() && bWasEmpty)
		InvalidateRect(*this, NULL, TRUE);

	return iItem;
}

KString TListControl::GetItem(	int		iItem,
								int		iSubItem,
								int&	iRImage)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(iItem		>= 0);
	DEBUG_VERIFY(iSubItem	>= 0);

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	Item.mask = LVIF_TEXT | LVIF_IMAGE;

	Item.iItem = iItem, Item.iSubItem = iSubItem;

	TCHAR buf[1024];
	buf[ARRAY_SIZE(buf) - 1] = 0;

	Item.pszText = buf;
	Item.cchTextMax = ARRAY_SIZE(buf) - 1;

	if(!ListView_GetItem(*this, &Item))
		INITIATE_DEFINED_FAILURE(TEXT("Error getting list view item."));

	iRImage = Item.iImage;

	return buf;
}

kflags_t TListControl::GetItemState(int iItem, int iSubItem)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(iItem		>= 0);
	DEBUG_VERIFY(iSubItem	>= 0);

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	Item.mask = LVIF_STATE;

	Item.iItem = iItem, Item.iSubItem = iSubItem;

	Item.stateMask = UINT_MAX;

	if(!ListView_GetItem(*this, &Item))
		INITIATE_DEFINED_FAILURE(TEXT("Error getting list view item state."));

	return Item.state;
}

void TListControl::SetItem(	int			iItem,
							int			iSubItem,
							LPCTSTR		pText,
							int			iImage)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(iItem		>= 0);
	DEBUG_VERIFY(iSubItem	>= 0);

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	Item.iItem = iItem, Item.iSubItem = iSubItem;

	if(pText)
		Item.mask |= LVIF_TEXT, Item.pszText = (LPSTR)pText;

	if(iImage >= 0)
		Item.mask |= LVIF_IMAGE, Item.iImage = iImage;
	
	if(!ListView_SetItem(*this, &Item))
		INITIATE_DEFINED_FAILURE(TEXT("Error setting list view item."));
}

void TListControl::DeleteItem(int iItem, bool bSelectNew)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(iItem >= 0);

	DEBUG_VERIFY(!(bSelectNew && (GetWindowLong(*this, GWL_STYLE) & LVS_SINGLESEL)));

	int iSelItem = -1;

	if(bSelectNew && GetSelectedItem() == iItem)
	{
		if(iItem + 1 < (int)GetN())
			iSelItem = iItem;
		else if(iItem > 0)
			iSelItem = iItem - 1;
	}

	if(!ListView_DeleteItem(*this, iItem))
		INITIATE_DEFINED_FAILURE(TEXT("Error deleting list view item."));

	if(bSelectNew)
	{
		SetFocus(*this);

		if(iSelItem >= 0)
			SelectItem(iSelItem);
	}

	if(!m_EmptyText.IsEmpty() && IsEmpty())
		InvalidateRect(*this, NULL, TRUE);
}

size_t TListControl::GetNColumns() const
{
	DEBUG_VERIFY_ALLOCATION;

	size_t szNHeaders;

	HWND hHeader = ListView_GetHeader(*this);

	DEBUG_VERIFY(hHeader);

	szNHeaders = Header_GetItemCount(hHeader);

	DEBUG_VERIFY(szNHeaders != UINT_MAX);

	return szNHeaders;
}

void TListControl::FocusItem(int iItem)
{
	DEBUG_VERIFY_ALLOCATION;

	if(iItem >= 0)
	{
		LVITEM Item;
		memset(&Item, 0, sizeof(Item));

		Item.mask = LVIF_STATE;

		Item.iItem = iItem;

		Item.state = LVIS_FOCUSED, Item.stateMask = LVIS_FOCUSED;

		ListView_SetItem(*this, &Item);

		EnsureVisible(iItem);
	}
	else
	{
		if((iItem = GetFocusedItem()) >= 0)
		{
			LVITEM Item;
			memset(&Item, 0, sizeof(Item));

			Item.mask = LVIF_STATE;

			Item.iItem = iItem;

			Item.state = 0, Item.stateMask = LVIS_FOCUSED;

			ListView_SetItem(*this, &Item);
		}
	}
}

void TListControl::SelectItem(int iItem, bool bFocus)
{
	DEBUG_VERIFY_ALLOCATION;

	if(iItem >= 0)
	{
		LVITEM Item;
		memset(&Item, 0, sizeof(Item));

		Item.mask = LVIF_STATE;

		Item.iItem = iItem;

		Item.state = LVIS_SELECTED, Item.stateMask = LVIS_SELECTED;

		if(bFocus && (GetWindowLong(*this, GWL_STYLE) & LVS_SINGLESEL))
			Item.state |= LVIS_FOCUSED, Item.stateMask |= LVIS_FOCUSED;

		ListView_SetItem(*this, &Item);

		EnsureVisible(iItem);
	}
	else
	{
		while((iItem = GetSelectedItem()) >= 0)
		{
			LVITEM Item;
			memset(&Item, 0, sizeof(Item));

			Item.mask = LVIF_STATE;

			Item.iItem = iItem;

			Item.state = 0, Item.stateMask = LVIS_SELECTED;

			if(!ListView_SetItem(*this, &Item)) // safety
				break;
		}
	}
}

void TListControl::SortItems(	PFNLVCOMPARE	pCompare,
								void*			pParam,
								int&			iHoldItem)
{
	DEBUG_VERIFY_ALLOCATION;

	void* pHoldParam;

	if(iHoldItem >= 0)
		pHoldParam = GetItemVoidData(iHoldItem);

	ListView_SortItems(*this, pCompare, (LPARAM)pParam);

	if(iHoldItem >= 0)
		iHoldItem = GetItemByVoidData(pHoldParam);
}

void TListControl::EditLabel(int iItem, LPCTSTR pInitialText)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(iItem >= 0);

	if(pInitialText)
		SetItem(iItem, 0, pInitialText);

	SetFocus(*this);

	ListView_EditLabel(*this, iItem);
}

bool TListControl::GetClientItemRect(int iItem, IRECT& RRect, bool bTextOnly)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(iItem >= 0);

	return	ListView_GetItemRect
				(*this, iItem, (RECT*)RRect, bTextOnly ? LVIR_LABEL : LVIR_BOUNDS);
}

bool TListControl::GetScreenItemRect(int iItem, IRECT& RRect, bool bTextOnly)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(iItem >= 0);

	if(!GetClientItemRect(iItem, RRect, bTextOnly))
		return false;

	KClientToScreen(*this, RRect);

	return true;
}

void* TListControl::GetItemVoidData(int iItem)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(iItem >= 0);

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	Item.mask = LVIF_PARAM;

	Item.iItem = iItem;

	if(!ListView_GetItem(*this, &Item))
		INITIATE_DEFINED_FAILURE(TEXT("Error getting list view item param."));

	return (void*)Item.lParam;
}

void TListControl::SetItemVoidData(int iItem, void* pData)
{
	DEBUG_VERIFY_ALLOCATION;
	
	DEBUG_VERIFY(iItem >= 0);

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	Item.mask = LVIF_PARAM;

	Item.iItem = iItem;

	Item.lParam = (LPARAM)pData;

	if(!ListView_SetItem(*this, &Item))
		INITIATE_DEFINED_FAILURE(TEXT("Error setting list view item param."));
}

int TListControl::FindItemByVoidData(void* pData)
{
	DEBUG_VERIFY_ALLOCATION;

	size_t szN = GetN();

	for(size_t i = 0 ; i < szN ; i++)
	{
		if(GetItemVoidData(i) == pData)
			return i;
	}

	return -1;
}

IPOINT TListControl::GetSubItemFromClientCoords(const IPOINT& Coords, bool bAsLClick)
{
	DEBUG_VERIFY_ALLOCATION;

	LVHITTESTINFO hti;
	memset(&hti, 0, sizeof(hti));

	hti.pt = *Coords;

	ListView_HitTest(*this, &hti);

	return	bAsLClick && (!(hti.flags & LVHT_ONITEM) || hti.iSubItem < 0) ? 
				IPOINT(-1, -1) :
				IPOINT(hti.iSubItem, hti.iItem);
}

IPOINT TListControl::GetSubItemFromScreenCoords(const IPOINT& Coords, bool bAsLClick)
{
	DEBUG_VERIFY_ALLOCATION;

	return GetSubItemFromClientCoords(KScreenToClient(*this, IPOINT(Coords)), bAsLClick);
}

HWND TListControl::GetEditControl()
{
	DEBUG_VERIFY_ALLOCATION;

	return ListView_GetEditControl(*this);
}

LRESULT TListControl::WindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, WNDPROC pOldWindowProc)
{
	if(!IsAllocated())
		return CallWindowProc(pOldWindowProc, hWnd, uiMsg, wParam, lParam);

	switch(uiMsg)
	{
	case WM_SIZE:
		if(!m_EmptyText.IsEmpty() && IsEmpty())
			InvalidateRect(*this, NULL, TRUE);

		break;

	case WM_ERASEBKGND:
		if(!m_EmptyText.IsEmpty() && IsEmpty())
			return TRUE;

		break;

	case WM_PAINT:
		if(!m_EmptyText.IsEmpty() && IsEmpty())
		{
			TPaintDC DC(*this);

			IRECT Rect = GetKClientRect(*this);

			FillRect(DC, Rect, GetSysColorBrush(COLOR_WINDOW));

			TGDIObjectSelector	Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));
			TTextColorSelector	Selector1(DC, m_crEmptyTextColor);
			TBkModeSelector		Selector2(DC, TRANSPARENT);

			DrawText(	DC,
						m_EmptyText,
						-1,
						Rect,
						DT_SINGLELINE | DT_NOPREFIX | DT_CENTER | DT_VCENTER);

			return 0;
		}

		break;
	}

	return CallWindowProc(pOldWindowProc, hWnd, uiMsg, wParam, lParam);
}
