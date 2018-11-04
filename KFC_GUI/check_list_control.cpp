#include "kfc_gui_pch.h"
#include "check_list_control.h"

#include "gui.h"
#include "gui_elements.h"

// -------------------
// Check list control
// -------------------

// Item check notification
const UINT TCheckListControl::TItemCheckNotification::m_uiCode = LVN_LAST - 0x10;

// Left button click notification
const UINT TCheckListControl::TLeftButtonClickNotification::m_uiCode = LVN_LAST - 0x11;

// Right button click notification
const UINT TCheckListControl::TRightButtonClickNotification::m_uiCode = LVN_LAST - 0x12;

// Item double-click notification
const UINT TCheckListControl::TItemDoubleClickNotification::m_uiCode = LVN_LAST - 0x13;

// Selection change notification
const UINT TCheckListControl::TSelectionChangeNotification::m_uiCode = LVN_LAST - 0x14;

// Item color notification
const UINT TCheckListControl::TItemColorNotification::m_uiCode = LVN_LAST - 0x15;

// Check list control
TCheckListControl::TCheckListControl()
{
	m_bAllocated = false;

	m_hWnd = NULL;

	m_pOldWindowProc = NULL;

	m_bAccelerateCheckNone	= true;
	m_bAccelerateCheckAll	= true;
	
	m_EmptyText = TEXT("(no items)");
}

void TCheckListControl::Release()
{
	m_bAllocated = false;

	if(IsWindow(m_hWnd))
	{
		SetWindowLong(m_hWnd, GWL_USERDATA, 0);

		if(m_pOldWindowProc)
			SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)m_pOldWindowProc);

		m_Items.Clear();

		m_Columns.Clear();
	}

	m_hWnd = NULL;

	m_pOldWindowProc = NULL;
}

void TCheckListControl::Allocate(	int				iSID,
									const TColumn*	pSColumns,
									size_t			szNColumns)
{
	Release();

	try
	{
		size_t i;

		m_iID = iSID;

		DEBUG_VERIFY(szNColumns == 0 || pSColumns);

		if(szNColumns == 0)
		{			
			m_Columns.Add(1).Set(TEXT(""));
		}
		else
		{
			for(i = 0 ; i < szNColumns ; i++)
				m_Columns.Add() = pSColumns[i];
		}

		m_bDrawChecks = true;

		m_bAllocated = true;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TCheckListControl::OnInitDialog(HWND hWnd)
{
	DEBUG_EVALUATE_VERIFY(m_hWnd = GetDlgItem(hWnd, m_iID));

	size_t i;

	// Subclassing
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	m_pOldWindowProc = (WNDPROC)GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)StaticSubWindowProc);

	// Columns
	{
		size_t szTotalWidth = 0;

		for(i = 0 ; i < m_Columns.GetN() ; i++)
		{
			LVCOLUMN Column;
			memset(&Column, 0, sizeof(Column));

			KString Title = m_Columns[i].m_Title;

			Column.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT;

			if(m_Columns[i].m_Alignment == ALIGNMENT_MIN)
				Column.fmt |= LVCFMT_LEFT;
			else if(m_Columns[i].m_Alignment == ALIGNMENT_MAX)
				Column.fmt |= LVCFMT_RIGHT;
			else if(m_Columns[i].m_Alignment == ALIGNMENT_MID)
				Column.fmt |= LVCFMT_CENTER;
			else
				INITIATE_FAILURE;

			Column.cx =	i == m_Columns.GetLast() ?
							GetKClientRect(m_hWnd).GetWidth() - szTotalWidth :
							m_Columns[i].m_szWidth;

			Column.pszText	= Title.GetDataPtr();

			ListView_InsertColumn(m_hWnd, i, &Column);

			szTotalWidth += m_Columns[i].m_szWidth;
		}

		FixLastColumn();
	}
}

void TCheckListControl::OnMeasureItem(bool bDrawChecks, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	pMeasureItemStruct->itemHeight =
		bDrawChecks ?
			MeasureCheckText(TDC((HWND)NULL)) :
			MeasureNoCheckText(TDC((HWND)NULL));
}

void TCheckListControl::OnDrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
	size_t i;

	const HWND hWnd = pDrawItemStruct->hwndItem;

	const size_t szListIndex = pDrawItemStruct->itemID;

	const HWND hParentWnd = GetParent(*this);

	if(szListIndex < m_Items.GetN())
	{
		const bool bChecked		= GetItem(szListIndex).m_bChecked;
		const bool bSelected	= pDrawItemStruct->itemState & ODS_SELECTED ? true : false;
		const bool bFocused		= GetFocus() == m_hWnd;

		IRECT Rect = pDrawItemStruct->rcItem;

		for(i = 0 ; i < m_Columns.GetN() ; i++)
		{
			// Updating rect
			Rect.m_Right = Rect.m_Left + GetColumnWidth(i);

			// Requesting parent for colors (if has one)
			TItemColorNotification Notification(*this, m_iID, i, szListIndex);

			if(hParentWnd)
				SendMessage(hParentWnd, WM_NOTIFY, m_iID, (LPARAM)&Notification);

			// Drawing sub-item
			if(i == 0 && m_bDrawChecks)
			{
				DrawCheckText(	pDrawItemStruct->hDC,
								GetSubItemText(szListIndex, i),
								Rect,
								m_Columns[i].m_Alignment,
								bChecked,
								bSelected,
								bFocused,
								Notification.m_BorderSpacing,
								Notification.m_crTextBGColor,
								Notification.m_crTextFGColor);
			}
			else
			{
				DrawNoCheckText(pDrawItemStruct->hDC,
								GetSubItemText(szListIndex, i),
								Rect,
								m_Columns[i].m_Alignment,
								bSelected,
								bFocused,
								Notification.m_BorderSpacing,
								Notification.m_crTextBGColor,
								Notification.m_crTextFGColor);
			}

			// Shifting rect
			Rect.m_Left = Rect.m_Right;
		}
	}
}

bool TCheckListControl::OnNotify(const TNotification* pNotification)
{
	if(pNotification->code == LVN_ITEMCHANGED)
	{
		const NM_LISTVIEW* pNMListView = (const NM_LISTVIEW*)pNotification;

		if(	(pNMListView->uOldState & LVIS_SELECTED) !=
			(pNMListView->uNewState & LVIS_SELECTED))
		{
			// Notifying parent (if has one)
			const HWND hParentWnd = GetParent(*this);

			if(hParentWnd)
			{
				SendMessage(hParentWnd,
							WM_NOTIFY,
							m_iID,
							(LPARAM)&TSelectionChangeNotification(	*this,
																	m_iID,
																	GetCurrentSelection()));				
			}

			return true;
		}
	}

	return false;
}

LRESULT CALLBACK TCheckListControl::StaticSubWindowProc(HWND	hWnd,
														UINT	uiMsg,
														WPARAM	wParam,
														LPARAM	lParam)
{
	TCheckListControl* const pControl = (TCheckListControl*)GetWindowLong(hWnd, GWL_USERDATA);

	if(pControl)
		return pControl->SubWindowProc(hWnd, uiMsg, wParam, lParam);

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

LRESULT TCheckListControl::SubWindowProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	DEBUG_VERIFY(m_pOldWindowProc);

	DEBUG_VERIFY(m_hWnd == hWnd);

	if(	uiMsg == WM_LBUTTONDOWN		|| uiMsg == WM_RBUTTONDOWN ||
		uiMsg == WM_LBUTTONDBLCLK	|| uiMsg == WM_RBUTTONDBLCLK)
	{
		const IPOINT Coords = KScreenToClient(hWnd, GetKCursorPos());

		const size_t szListIndex = KListControlHitTest(hWnd, Coords);

		if(uiMsg == WM_LBUTTONDOWN)
		{
			// Notifying parent (if has one)
			const HWND hParentWnd = GetParent(*this);

			if(hParentWnd)
			{
				SendMessage(hParentWnd,
							WM_NOTIFY,
							m_iID,
							(LPARAM)&TLeftButtonClickNotification(	*this,
																	m_iID,
																	szListIndex));
			}
		}
		else if(uiMsg == WM_RBUTTONDOWN)
		{
			// Notifying parent (if has one)
			const HWND hParentWnd = GetParent(*this);

			if(hParentWnd)
			{
				SendMessage(hParentWnd,
							WM_NOTIFY,
							m_iID,
							(LPARAM)&TRightButtonClickNotification(	*this,
																	m_iID,
																	szListIndex));
			}
		}

		if(szListIndex != UINT_MAX)
		{
			if(m_bDrawChecks && TogglesCheckText(GetKListControlItemRect(hWnd, szListIndex), Coords))
			{
				ToggleCheck(szListIndex);

				return 0;
			}
			else
			{				
				if(uiMsg == WM_LBUTTONDBLCLK)
				{
					// Notifying parent (if has one)
					const HWND hParentWnd = GetParent(*this);

					if(hParentWnd)
					{
						SendMessage(hParentWnd,
									WM_NOTIFY,
									m_iID,
									(LPARAM)&TItemDoubleClickNotification(	*this,
																			m_iID,
																			szListIndex));

						return 0;
					}
				}
			}
		}
	}
	else if(uiMsg == WM_KEYDOWN)
	{
		// Hijacking up/down keys since first up/down do not work inside original WNDPROC
		if(wParam == VK_UP)
		{
			if(GetN() > 0)
			{
				const size_t szListIndex = GetCurrentSelection();
				if(szListIndex != 0)
					SetCurrentSelection(szListIndex - 1);
			}

			return 0;
		}
		else if(wParam == VK_DOWN)
		{
			if(GetN() > 0)
			{
				const size_t szListIndex = GetCurrentSelection();
				if(szListIndex != GetN() - 1)
					SetCurrentSelection(szListIndex + 1);
			}

			return 0;
		}
		else if(wParam == VK_SPACE) // toggles a check
		{
			ToggleCheck(GetCurrentSelection());
			return 0;
		}
		else if(wParam == 'N' && IsKeyDown(VK_CONTROL)) // selects none
		{
			if(m_bAccelerateCheckNone)
			{
				CheckAll(false);
				return 0;
			}
		}
		else if(wParam == 'A' && IsKeyDown(VK_CONTROL)) // selects all
		{
			if(m_bAccelerateCheckAll)
			{
				CheckAll(true);
				return 0;
			}
		}
	}
	else if(uiMsg == WM_PAINT)
	{
		CallWindowProc(m_pOldWindowProc, hWnd, uiMsg, wParam, lParam);

		if(GetN() == 0 && !m_EmptyText.IsEmpty())
		{
			TDC DC(hWnd);

			TGDIObjectSelector	Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));
			TBkColorSelector	Selector1(DC, GetSysColor(COLOR_WINDOW));
			TTextColorSelector	Selector2(DC, GetSysColor(COLOR_GRAYTEXT));
			TBkModeSelector		Selector3(DC, OPAQUE);

			IRECT Rect = GetKClientRect(hWnd);

			// Cutting header control (if exists)
			if(!(GetWindowLong(*this, GWL_STYLE) & LVS_NOCOLUMNHEADER)) // has header
				Rect.m_Top += GetTextSize(DC, TEXT("MW")).cy + 8;

			DrawText(	DC,
						m_EmptyText,
						-1,
						Rect,
						DT_SINGLELINE | DT_NOPREFIX | DT_CENTER | DT_VCENTER);
		}

		return 0;
	}

	return CallWindowProc(m_pOldWindowProc, hWnd, uiMsg, wParam, lParam);
}

void TCheckListControl::FixLastColumn() const
{
	size_t i;

	DEBUG_VERIFY(m_hWnd);

	int iWidth = 0;

	DEBUG_VERIFY(m_Columns.GetN() > 0);

	// Getting total width of all columns except for the last
	for(i = 0 ; i < m_Columns.GetN() - 1 ; i++)
	{
		LVCOLUMN HeaderColumn;
		memset(&HeaderColumn, 0, sizeof(HeaderColumn));

		HeaderColumn.mask = LVCF_WIDTH;

		ListView_GetColumn(*this, i, &HeaderColumn);

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

		ListView_SetColumn(*this, m_Columns.GetLast(), &HeaderColumn);
	}
}

TCheckListControl::TItem& TCheckListControl::GetItem(size_t szListIndex)
{
	DEBUG_VERIFY(m_hWnd);

	DEBUG_VERIFY(szListIndex < m_Items.GetN());

	return *TItems::TIterator().FromPVoid((void*)GetKListControlItemParam(m_hWnd, szListIndex));
}

const TCheckListControl::TItem& TCheckListControl::GetItem(size_t szListIndex) const
{
	DEBUG_VERIFY(m_hWnd);

	DEBUG_VERIFY(szListIndex < m_Items.GetN());

	return *TItems::TConstIterator().FromPVoid((void*)GetKListControlItemParam(m_hWnd, szListIndex));
}

bool TCheckListControl::ProcessStaticParentMessage(int iID, bool bDrawChecks, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
	case WM_MEASUREITEM:
		if((int)wParam == iID)
		{
			OnMeasureItem(bDrawChecks, (LPMEASUREITEMSTRUCT)lParam);
			return true;
		}

		return false;
	}

	return false;
}

bool TCheckListControl::ProcessParentMessage(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	DEBUG_VERIFY_ALLOCATION;

	switch(uiMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hWnd);
		return false;

	case WM_DRAWITEM:
		if(wParam == m_iID)
		{
			OnDrawItem((LPDRAWITEMSTRUCT)lParam);
			return true;
		}

	case WM_NOTIFY:
		if(wParam == m_iID)
			return OnNotify((const TNotification*)lParam);

		return false;
	}

	return false;
}

void TCheckListControl::Clear()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);	

	ListView_DeleteAllItems(*this);

	FixLastColumn();

	// Ensuring that area with 'none' text is also redrawn in case list control
	// invalidates only item rects upon clearing
	InvalidateRect(*this, NULL, TRUE);

	m_Items.Clear();
}

size_t TCheckListControl::AddItem(LPCTSTR pText, bool bChecked, void* pData)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	const TItems::TIterator Iter = m_Items.AddLast();	

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	KString Text = pText;

	Item.mask		= LVIF_TEXT | LVIF_PARAM;
	Item.iItem		= m_Items.GetN();
	Item.pszText	= Text.GetDataPtr();
	Item.cchTextMax	= _tcslen(Item.pszText);
	Item.lParam		= (LPARAM)Iter.AsPVoid();	

	const size_t szIndex = ListView_InsertItem(*this, &Item);
	DEBUG_VERIFY(szIndex != UINT_MAX);

	Iter->Set(szIndex, bChecked, pData);

	FixLastColumn();

	return szIndex;
}

void TCheckListControl::DeleteItem(size_t szListIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	DEBUG_VERIFY(szListIndex < m_Items.GetN());

	m_Items.Del(TItems::TIterator().
					FromPVoid((void*)GetKListControlItemParam(m_hWnd, szListIndex)));

	ListView_DeleteItem(*this, szListIndex);
}

KString TCheckListControl::GetSubItemText(size_t szListIndex, size_t szColIndex) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	DEBUG_VERIFY(szListIndex < m_Items.GetN());

	DEBUG_VERIFY(szColIndex < m_Columns.GetN());

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	TCHAR Buffer[16384] = TEXT("");

	Item.mask		= LVIF_TEXT;
	Item.iItem		= szListIndex;
	Item.iSubItem	= szColIndex;
	Item.pszText	= Buffer;
	Item.cchTextMax	= sizeof(Buffer) - 1;

	ListView_GetItem(*this, &Item);

	return Buffer;
}

void TCheckListControl::SetSubItemText(size_t szListIndex, size_t szColIndex, LPCTSTR pText)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	DEBUG_VERIFY(szListIndex < m_Items.GetN());

	DEBUG_VERIFY(szColIndex < m_Columns.GetN());

	LVITEM Item;
	memset(&Item, 0, sizeof(Item));

	KString Text = pText;

	Item.mask		= LVIF_TEXT;
	Item.iItem		= szListIndex;
	Item.iSubItem	= szColIndex;
	Item.pszText	= Text.GetDataPtr();

	FixLastColumn();

	ListView_SetItem(*this, &Item);
}

void* TCheckListControl::GetItemData(size_t szListIndex) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	DEBUG_VERIFY(szListIndex < m_Items.GetN());

	return GetItem(szListIndex).m_pData;
}

size_t TCheckListControl::GetCurrentSelection() const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	return ListView_GetNextItem(m_hWnd, -1, LVNI_SELECTED);
}

void TCheckListControl::SetCurrentSelection(size_t szListIndex) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	if(szListIndex == UINT_MAX)
	{
		const size_t szIndex = GetCurrentSelection();

		if(szIndex != UINT_MAX)
		{
			LVITEM Item;
			memset(&Item, 0, sizeof(Item));

			Item.mask		= LVIF_STATE;
			Item.iItem		= szIndex;
			Item.state		= 0;
			Item.stateMask	= LVIS_SELECTED | LVIS_FOCUSED;

			ListView_SetItem(m_hWnd, &Item);
		}
	}
	else
	{
		DEBUG_VERIFY(szListIndex < m_Items.GetN());

		LVITEM Item;
		memset(&Item, 0, sizeof(Item));

		Item.mask		= LVIF_STATE;
		Item.iItem		= szListIndex;
		Item.state		= LVIS_SELECTED | LVIS_FOCUSED;
		Item.stateMask	= LVIS_SELECTED | LVIS_FOCUSED;

		ListView_SetItem(m_hWnd, &Item);

		ListView_EnsureVisible(m_hWnd, szListIndex, FALSE);
	}

	FixLastColumn();
}

void TCheckListControl::SetInitialFocus()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	SetFocus(m_hWnd);

	if(!m_Items.IsEmpty())
		SetCurrentSelection(0);

	FixLastColumn();
}

size_t TCheckListControl::GetColumnWidth(size_t szIndex) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	DEBUG_VERIFY(szIndex < m_Columns.GetN());

	LVCOLUMN Column;
	memset(&Column, 0, sizeof(Column));

	Column.mask = LVCF_WIDTH;

	ListView_GetColumn(*this, szIndex, &Column);

	return Column.cx;
}

bool TCheckListControl::GetCheck(size_t szListIndex) const
{
	DEBUG_VERIFY_ALLOCATION;

	return GetItem(szListIndex).m_bChecked;
}

bool TCheckListControl::SetCheck(size_t szListIndex, bool bCheck)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	TItem& Item = GetItem(szListIndex);

	// Checking if state has changed
 	if(Item.m_bChecked == bCheck)
		return Item.m_bChecked;

	// Notifying parent (if has one)
	const HWND hParentWnd = GetParent(*this);

	if(hParentWnd)
	{
		if(SendMessage(	hParentWnd,
						WM_NOTIFY,
						m_iID,
						(LPARAM)&TItemCheckNotification(*this,
														m_iID,
														szListIndex,
														Item.m_bChecked,
														bCheck)))
		{
			return Item.m_bChecked;
		}
	}

	// Setting new state
	Item.m_bChecked = bCheck;

	// Invalidating item rect
	InvalidateItemRect(szListIndex);

	return Item.m_bChecked;
}

bool TCheckListControl::ToggleCheck(size_t szListIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	return SetCheck(szListIndex, !GetCheck(szListIndex));
}

size_t TCheckListControl::FindCheck(size_t szListAfter) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	size_t i;

	DEBUG_VERIFY(szListAfter == UINT_MAX || szListAfter < GetN());

	for(i = szListAfter == UINT_MAX ? 0 : szListAfter + 1 ; i < GetN() ; i++)
	{
		if(GetCheck(i))
			return i;
	}

	return UINT_MAX;
}

void TCheckListControl::CheckRange(size_t szStart, size_t szAmt, bool bCheck)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	DEBUG_VERIFY(szStart + szAmt <= GetN());

	size_t i;

	for(i = 0 ; i < szAmt ; i++)
		SetCheck(szStart + i, bCheck);
}

void TCheckListControl::CheckAll(bool bCheck)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	CheckRange(0, GetN(), bCheck);
	
}

void TCheckListControl::InvalidateItemRect(size_t szListIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	InvalidateRect(m_hWnd, GetKListControlItemRect(m_hWnd, szListIndex), FALSE);
}

const KString& TCheckListControl::GetEmptyText() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_EmptyText;
}

void TCheckListControl::SetEmptyText(LPCTSTR pSEmptyText)
{
	DEBUG_VERIFY_ALLOCATION;

	m_EmptyText = pSEmptyText;

	if(GetN() == 0)
		InvalidateRect(*this, NULL, TRUE);
}

bool TCheckListControl::GetDrawChecks() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_bDrawChecks;
}

void TCheckListControl::SetDrawChecks(bool bSDrawChecks)
{
	DEBUG_VERIFY_ALLOCATION;

	if(m_bDrawChecks != bSDrawChecks)
		m_bDrawChecks = bSDrawChecks, InvalidateRect(*this, NULL, TRUE);
}

// Getters
size_t TCheckListControl::GetN() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_Items.GetN();
}

HWND TCheckListControl::GetWnd() const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hWnd);

	return m_hWnd;
}