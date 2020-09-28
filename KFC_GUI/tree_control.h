 #ifndef tree_control_h
#define tree_control_h

// -------------
// Tree control
// -------------
class TTreeControl
{
private:
    HWND m_hWnd;

    KString m_EmptyText;

    COLORREF m_crEmptyTextColor;

private:
    LRESULT TTreeControl::WindowProc
        (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, WNDPROC pOldWindowProc);

public:
    TTreeControl();

    ~TTreeControl()
        { Release(); }

    bool IsAllocated() const
        { return m_hWnd; }

    void Release();

    void Create(HWND            hParentWnd,
                const IRECT&    Rect,
                DWORD           dwSetStyle  = 0,
                DWORD           dwDropStyle = 0,
                DWORD           dwBaseStyle =
                    TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS);

    void SetCheckboxes();

    void SetEmptyText(LPCTSTR pText, COLORREF crColor = GetSysColor(COLOR_GRAYTEXT));

    void SetImageList(HIMAGELIST hImageList);

    void Clear();

    HTREEITEM AddItem(  HTREEITEM   hParentItem,
                        HTREEITEM   hInsertAfter,
                        LPCTSTR     pText,
                        int         iImage          = -1,  // means none
                        int         iSelectedImage  = -1,
            int     nChilndren = 0); // means same as normal

    KString GetItem(HTREEITEM   hItem,
                    int&        iRImage         = temp<int>(),
                    int&        iRSelectedImage = temp<int>());

    kflags_t GetItemState(HTREEITEM hItem);

    void SetItem(   HTREEITEM   hItem,
                    LPCTSTR     pText,
                    int         iImage          = -1,  // means none
                    int         iSelectedImage  = -1); // means same as normal

    void DeleteItem(HTREEITEM hItem, bool bSelectNew = false);

    HTREEITEM GetFirstRootItem() const
    {
        DEBUG_VERIFY_ALLOCATION;

        return TreeView_GetNextItem(*this, TVI_ROOT, TVGN_CHILD);
    }

    HTREEITEM GetParentItem(HTREEITEM hItem)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(hItem);

        return TreeView_GetParent(*this, hItem);
    }

    HTREEITEM GetTopmostParentItem(HTREEITEM hItem)
    {
        DEBUG_VERIFY_ALLOCATION;

        while(HTREEITEM hParentItem = TreeView_GetParent(*this, hItem))
            hItem = hParentItem;

        return hItem;
    }

    HTREEITEM GetFirstChildItem(HTREEITEM hItem)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(hItem);

        return TreeView_GetNextItem(*this, hItem, TVGN_CHILD);
    }

    HTREEITEM GetNextSiblingItem(HTREEITEM hItem)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(hItem);

        return TreeView_GetNextSibling(*this, hItem);
    }

    bool IsEmpty() const
        { return !GetFirstRootItem(); }

    HTREEITEM GetSelectedItem()
    {
        DEBUG_VERIFY_ALLOCATION;

        return TreeView_GetSelection(*this);
    }

    void SelectItem(HTREEITEM hItem)
    {
        DEBUG_VERIFY_ALLOCATION;

        TreeView_SelectItem(*this, hItem);
    }

    bool IsItemChecked(HTREEITEM hItem);

    void CheckItem(HTREEITEM hItem, bool bCheck);

    void SortChildren(  HTREEITEM       hParent,
                        PFNTVCOMPARE    pCompare,
                        void*           pParam,
                        bool            bRecursive  = false,
                        HTREEITEM&      hHoldItem   = temp<HTREEITEM>((HTREEITEM)NULL));

    void EnsureVisible(HTREEITEM hItem)
    {
        DEBUG_VERIFY_ALLOCATION;

        DEBUG_VERIFY(hItem);

        TreeView_EnsureVisible(*this, hItem);
    }

    void EditLabel(HTREEITEM hItem, LPCTSTR pInitialText = NULL);

    bool GetClientItemRect(HTREEITEM hItem, IRECT& RRect, bool bTextOnly = true);
    bool GetScreenItemRect(HTREEITEM hItem, IRECT& RRect, bool bTextOnly = true);

    void* GetItemVoidData(HTREEITEM hItem);

    void SetItemVoidData(HTREEITEM hItem, void* pData);

    HTREEITEM FindItemByVoidData(HTREEITEM hParent, void* pData);

    HTREEITEM GetItemByVoidData(HTREEITEM hParent, void* pData)
    {
        DEBUG_VERIFY_ALLOCATION;

        HTREEITEM hItem = FindItemByVoidData(hParent, pData);

        DEBUG_VERIFY(hItem);

        return hItem;
    }

    template <class t>
    t GetItemData(HTREEITEM hItem)
    {
        kfc_static_assert(sizeof(t) <= sizeof(LPARAM));
        return (t&)(void*&)temp<void*>(GetItemVoidData(hItem));
    }

    template <class t>
    void SetItemData(HTREEITEM hItem, t Data)
    {
        kfc_static_assert(sizeof(t) <= sizeof(LPARAM));
        return SetItemVoidData(hItem, (void*&)Data);
    }

    template <class t>
    HTREEITEM FindItemByData(HTREEITEM hParent, t Data)
    {
        kfc_static_assert(sizeof(t) <= sizeof(LPARAM));
        return FindItemByVoidData(hParent, (void*)Data);
    }

    template <class t>
    HTREEITEM GetItemByData(HTREEITEM hParent, t Data)
    {
        kfc_static_assert(sizeof(t) <= sizeof(LPARAM));
        return GetItemByVoidData(hParent, (void*)Data);
    }

    HTREEITEM GetItemFromClientCoords(const IPOINT& Coords, bool bAsLClick = true, kflags_t& flRFlags = temp<kflags_t>()());

    HTREEITEM GetItemFromScreenCoords(const IPOINT& Coords, bool bAsLClick = true, kflags_t& flRFlags = temp<kflags_t>()());

    bool IsRootItem(HTREEITEM hItem)
        { return !GetParentItem(hItem); }

    bool IsItemSelected(HTREEITEM hItem)
        { DEBUG_VERIFY(hItem); return GetItemState(hItem) & TVIS_SELECTED; }

    bool IsItemExpanded(HTREEITEM hItem)
        { DEBUG_VERIFY(hItem); return GetItemState(hItem) & TVIS_EXPANDED; }

    HWND GetEditControl();

    HWND GetWindow() const
        { DEBUG_VERIFY_ALLOCATION; return m_hWnd; }

    operator HWND () const
        { return GetWindow(); }
};

#endif // tree_control_h
