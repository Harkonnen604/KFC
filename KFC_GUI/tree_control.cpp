#include "kfc_gui_pch.h"
#include "tree_control.h"

#include "gui.h"
#include "window_subclasser.h"

// -------------
// Tree control
// -------------
TTreeControl::TTreeControl()
{
    m_hWnd = NULL;
}

void TTreeControl::Release()
{
    if(m_hWnd)
    {
        if(GetWindowLong(m_hWnd, GWL_STYLE) & TVS_CHECKBOXES)
        {
            HIMAGELIST hStateIL = TreeView_GetImageList(m_hWnd, TVSIL_STATE);

            if(hStateIL)
                ImageList_Destroy(hStateIL);
        }

        DestroyWindow(m_hWnd), m_hWnd = NULL;
    }
}

void TTreeControl::Create(  HWND            hParentWnd,
                            const IRECT&    Rect,
                            DWORD           dwSetStyle,
                            DWORD           dwDropStype,
                            DWORD           dwBaseStyle)
{
    Release();

    try
    {
        (dwBaseStyle |= dwSetStyle) &= ~dwDropStype;

        m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                                WC_TREEVIEW,
                                TEXT(""),
                                WS_VISIBLE | (hParentWnd ? (WS_CHILD | WS_TABSTOP) : WS_POPUP) |
                                    dwBaseStyle,
                                Rect.m_Left,
                                Rect.m_Top,
                                Rect.GetWidth   (),
                                Rect.GetHeight  (),
                                hParentWnd,
                                NULL,
                                GetModuleHandle(NULL),
                                NULL);

        if(!m_hWnd)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error creating tree view control"),
                                            GetLastError());
        }

        SubclassWindow(m_hWnd, *this, &TTreeControl::WindowProc);

        SetEmptyText(NULL);
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TTreeControl::SetCheckboxes()
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsEmpty());

    SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | TVS_CHECKBOXES);
}

void TTreeControl::SetEmptyText(LPCTSTR pText, COLORREF crColor)
{
    DEBUG_VERIFY_ALLOCATION;

    m_EmptyText = pText ? pText : TEXT(""), m_crEmptyTextColor = crColor;

    if(IsEmpty())
        InvalidateRect(*this, NULL, TRUE);
}

void TTreeControl::SetImageList(HIMAGELIST hImageList)
{
    DEBUG_VERIFY_ALLOCATION;

    TreeView_SetImageList(*this, hImageList, TVSIL_NORMAL);
}

void TTreeControl::Clear()
{
    DEBUG_VERIFY_ALLOCATION;

    if(!TreeView_DeleteAllItems(*this))
        INITIATE_DEFINED_FAILURE(TEXT("Error clearing tree view."));

    DEBUG_VERIFY(IsEmpty());

    if(!m_EmptyText.IsEmpty())
        InvalidateRect(*this, NULL, TRUE);
}

HTREEITEM TTreeControl::AddItem(HTREEITEM   hParentItem,
                                HTREEITEM   hInsertAfter,
                                LPCTSTR     pText,
                                int         iImage,
                                int         iSelectedImage,
                int     nChilndren)
{
    DEBUG_VERIFY_ALLOCATION;

    if(iSelectedImage < 0)
        iSelectedImage = iImage;

    bool bWasEmpty = IsEmpty();

    TVINSERTSTRUCT is;

    is.hParent      = hParentItem;
    is.hInsertAfter = hInsertAfter;

    is.item.mask = 0;

    if(pText)
        is.item.mask |= TVIF_TEXT, is.item.pszText = (LPTSTR)pText;

    if(iImage >= 0)
        is.item.mask |= TVIF_IMAGE, is.item.iImage = iImage;

    if(iSelectedImage >= 0)
        is.item.mask |= TVIF_SELECTEDIMAGE, is.item.iSelectedImage = iSelectedImage;

  if(nChilndren > 0)
    is.item.mask = TVIF_CHILDREN, is.item.cChildren = nChilndren;

    HTREEITEM hItem = TreeView_InsertItem(*this, &is);

    if(!hItem)
        INITIATE_DEFINED_FAILURE(TEXT("Error adding tree view item."));

    if(!m_EmptyText.IsEmpty() && bWasEmpty)
        InvalidateRect(*this, NULL, TRUE);

    return hItem;
}

KString TTreeControl::GetItem(  HTREEITEM   hItem,
                                int&        iRImage,
                                int&        iRSelectedImage)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    TVITEM Item;
    memset(&Item, 0, sizeof(Item));

    Item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    Item.hItem = hItem;

    TCHAR buf[1024];
    buf[ARRAY_SIZE(buf) - 1] = 0;

    Item.pszText = buf;
    Item.cchTextMax = ARRAY_SIZE(buf) - 1;

    if(!TreeView_GetItem(*this, &Item))
        INITIATE_DEFINED_FAILURE(TEXT("Error getting tree view item."));

    iRImage = Item.iImage;
    iRSelectedImage = Item.iSelectedImage;

    return buf;
}

kflags_t TTreeControl::GetItemState(HTREEITEM hItem)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    TVITEM Item;
    memset(&Item, 0, sizeof(Item));

    Item.mask |= TVIF_HANDLE, Item.hItem = hItem;

    Item.mask |= TVIF_STATE, Item.stateMask = UINT_MAX;

    if(!TreeView_GetItem(*this, &Item))
        INITIATE_DEFINED_FAILURE(TEXT("Error getting tree view item state."));

    return Item.state;
}

void TTreeControl::SetItem( HTREEITEM   hItem,
                            LPCTSTR     pText,
                            int         iImage,
                            int         iSelectedImage)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    if(iSelectedImage < 0)
        iSelectedImage = iImage;

    TVITEM Item;
    memset(&Item, 0, sizeof(Item));

    Item.mask |= TVIF_HANDLE, Item.hItem = hItem;

    if(pText)
        Item.mask |= TVIF_TEXT, Item.pszText = (LPTSTR)pText;

    if(iImage >= 0)
        Item.mask |= TVIF_IMAGE, Item.iImage = iImage;

    if(iSelectedImage >= 0)
        Item.mask |= TVIF_SELECTEDIMAGE, Item.iSelectedImage = iSelectedImage;

    if(!TreeView_SetItem(*this, &Item))
        INITIATE_DEFINED_FAILURE(TEXT("Error setting tree view item."));
}

void TTreeControl::DeleteItem(HTREEITEM hItem, bool bSelectNew)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    HTREEITEM hSelItem = NULL;

    if(bSelectNew && GetSelectedItem() == hItem)
    {
        !(hSelItem = TreeView_GetNextItem(*this, hItem, TVGN_NEXT))     &&
        !(hSelItem = TreeView_GetNextItem(*this, hItem, TVGN_PREVIOUS)) &&
        !(hSelItem = TreeView_GetNextItem(*this, hItem, TVGN_PARENT));
    }

    if(!TreeView_DeleteItem(*this, hItem))
        INITIATE_DEFINED_FAILURE(TEXT("Error deleting tree view item."));

    if(bSelectNew)
    {
        SetFocus(*this);

        if(hSelItem)
            TreeView_SelectItem(*this, hSelItem);
    }

    if(!m_EmptyText.IsEmpty() && IsEmpty())
        InvalidateRect(*this, NULL, TRUE);
}

bool TTreeControl::IsItemChecked(HTREEITEM hItem)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    return ((GetItemState(hItem) & TVIS_STATEIMAGEMASK) >> 12) - 1;
}

void TTreeControl::CheckItem(HTREEITEM hItem, bool bCheck)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    TVITEM Item;
    memset(&Item, 0, sizeof(Item));

    Item.mask |= TVIF_HANDLE, Item.hItem = hItem;

    Item.mask |= TVIF_STATE, Item.stateMask = TVIS_STATEIMAGEMASK;

    Item.state = ((size_t)bCheck + 1u) << 12;

    if(!TreeView_SetItem(*this, &Item))
        INITIATE_DEFINED_FAILURE(TEXT("Error (un)checking tree view item."));
}

void TTreeControl::SortChildren(HTREEITEM       hParent,
                                PFNTVCOMPARE    pCompare,
                                void*           pParam,
                                bool            bRecursive,
                                HTREEITEM&      hHoldItem)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hParent);

    void* pHoldParam;

    if(hHoldItem)
    {
        DEBUG_VERIFY(GetParentItem(hHoldItem) == (hParent == TVI_ROOT ? NULL : hParent));

        pHoldParam = GetItemVoidData(hHoldItem);
    }

    TVSORTCB SortCB;

    SortCB.hParent      = hParent;
    SortCB.lpfnCompare  = pCompare;
    SortCB.lParam       = (LPARAM)pParam;

    TreeView_SortChildrenCB(*this, &SortCB, bRecursive);

    if(hHoldItem)
        hHoldItem = GetItemByVoidData(hParent, pHoldParam);
}

void TTreeControl::EditLabel(HTREEITEM hItem, LPCTSTR pInitialText)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    if(pInitialText)
        SetItem(hItem, pInitialText);

    SetFocus(*this);

    TreeView_EditLabel(*this, hItem);
}

bool TTreeControl::GetClientItemRect(HTREEITEM hItem, IRECT& RRect, bool bTextOnly)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    return TreeView_GetItemRect(*this, hItem, (RECT*)RRect, bTextOnly);
}
bool TTreeControl::GetScreenItemRect(HTREEITEM hItem, IRECT& RRect, bool bTextOnly)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    if(!GetClientItemRect(hItem, RRect, bTextOnly))
        return false;

    KClientToScreen(*this, RRect);

    return true;
}

void* TTreeControl::GetItemVoidData(HTREEITEM hItem)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    TVITEM Item;
    memset(&Item, 0, sizeof(Item));

    Item.mask = TVIF_HANDLE | TVIF_PARAM;

    Item.hItem = hItem;

    if(!TreeView_GetItem(*this, &Item))
        INITIATE_DEFINED_FAILURE(TEXT("Error getting tree view item param."));

    return (void*)Item.lParam;
}

void TTreeControl::SetItemVoidData(HTREEITEM hItem, void* pData)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hItem);

    TVITEM Item;
    memset(&Item, 0, sizeof(Item));

    Item.mask = TVIF_HANDLE | TVIF_PARAM, Item.lParam = (LPARAM)pData;

    Item.hItem = hItem;

    if(!TreeView_SetItem(*this, &Item))
        INITIATE_DEFINED_FAILURE(TEXT("Error setting tree view item param."));
}

HTREEITEM TTreeControl::FindItemByVoidData(HTREEITEM hParent, void* pData)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hParent);

    for(HTREEITEM hItem = GetFirstChildItem(hParent) ;
        hItem ;
        hItem = GetNextSiblingItem(hItem))
    {
        if(GetItemVoidData(hItem) == pData)
            return hItem;
    }

    return NULL;
}

HTREEITEM TTreeControl::GetItemFromClientCoords
    (const IPOINT& Coords, bool bAsLClick, kflags_t& flRFlags)
{
    DEBUG_VERIFY_ALLOCATION;

    TVHITTESTINFO hti;
    memset(&hti, 0, sizeof(hti));

    hti.pt = *Coords;

    TreeView_HitTest(*this, &hti);

    flRFlags = hti.flags;

    return bAsLClick && !(flRFlags & (TVHT_ONITEM | TVHT_ONITEMSTATEICON)) ? NULL : hti.hItem;
}

HTREEITEM TTreeControl::GetItemFromScreenCoords
    (const IPOINT& Coords, bool bAsLClick, kflags_t& flRFlags)
{
    DEBUG_VERIFY_ALLOCATION;

    return GetItemFromClientCoords(KScreenToClient(*this, IPOINT(Coords)), bAsLClick, flRFlags);
}

HWND TTreeControl::GetEditControl()
{
    DEBUG_VERIFY_ALLOCATION;

    return TreeView_GetEditControl(*this);
}

LRESULT TTreeControl::WindowProc
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

            TGDIObjectSelector  Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));
            TTextColorSelector  Selector1(DC, m_crEmptyTextColor);
            TBkModeSelector     Selector2(DC, TRANSPARENT);

            DrawText(   DC,
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
