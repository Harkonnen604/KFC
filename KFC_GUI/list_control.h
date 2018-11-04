#ifndef list_control_h
#define list_control_h

// -------------
// List control
// -------------
class TListControl
{
public:
	// Header
	struct THeader
	{
	public:
		KString		m_Title;
		TAlignment	m_Alignment;
		size_t		m_szWidth;

	public:
		THeader()
		{
			m_Alignment = ALIGNMENT_MIN;
		}

		THeader(LPCTSTR pTitle, TAlignment Alignment, size_t szWidth = UINT_MAX)
		{
			m_Title		= pTitle;
			m_Alignment	= Alignment;
			m_szWidth	= szWidth;
		}
	};

	// Headers
	typedef TArray<THeader> THeaders;

private:
	HWND m_hWnd;

	KString m_EmptyText;

	COLORREF m_crEmptyTextColor;

private:
	LRESULT WindowProc
		(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, WNDPROC pOldWindowProc);

public:
	TListControl();

	~TListControl()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hWnd; }

	void Release();

	void Create(HWND			hParentWnd,
				const IRECT&	Rect,
				bool			bSingleSelection,
				const THeaders&	Headers,
				DWORD			dwSetStyle	= 0,
				DWORD			dwDropStyle	= 0,
				DWORD			dwBaseStyle = LVS_SHOWSELALWAYS);

	void SetEmptyText(LPCTSTR pText, COLORREF crColor = ::GetSysColor(COLOR_GRAYTEXT));

	void FixLastColumn();

	void SetImageList(HIMAGELIST hImageList, ktype_t tpType = LVSIL_SMALL);	

	void Clear();

	int AddItem(LPCTSTR	pText	= NULL,
				int		iImage	= -1, // means do not set
				size_t	szPos	= UINT_MAX); // means end of the list

	KString GetItem(int		iItem,
					int		iSubItem,
					int&	iRImage = temp<int>());

	kflags_t GetItemState(int iItem, int iSubItem = 0);

	void SetItem(	int			iItem,
					int			iSubItem,
					LPCTSTR		pText,			// NULL means 'don't chane'
					int			iImage = -1);	// means retain old

	void DeleteItem(int iItem, bool bSelectNew = false);

	size_t GetNColumns() const;

	size_t GetN() const
		{ DEBUG_VERIFY_ALLOCATION; return ListView_GetItemCount(*this); }

	bool IsEmpty() const
		{ return !GetN(); }

	int GetFocusedItem()
	{
		DEBUG_VERIFY_ALLOCATION;

		return ListView_GetNextItem(*this, -1, LVNI_FOCUSED);
	}

	int GetSelectedItem()
	{
		DEBUG_VERIFY_ALLOCATION;

		return ListView_GetNextItem(*this, -1, LVNI_SELECTED);
	}

	void FocusItem(int iItem);

	void SelectItem(int iItem, bool bFocus = true);

	void SortItems(	PFNLVCOMPARE	pCompare,
					void*			pParam,
					int&			iHoldItem = temp<int>(-1));

	void EnsureVisible(int iItem)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(iItem >= 0);

		ListView_EnsureVisible(*this, iItem, FALSE);
	}

	void EditLabel(int iItem, LPCTSTR pInitialText = NULL);

	bool GetClientItemRect(int iItem, IRECT& RRect, bool bTextOnly = true);
	bool GetScreenItemRect(int iItem, IRECT& RRect, bool bTextOnly = true);

	void* GetItemVoidData(int iItem);

	void SetItemVoidData(int iItem, void* pData);

	int FindItemByVoidData(void* pData);

	int GetItemByVoidData(void* pData)
	{
		DEBUG_VERIFY_ALLOCATION;

		int iItem = FindItemByVoidData(pData);

		DEBUG_VERIFY(iItem >= 0);

		return iItem;
	}

	template <class t>
	t GetItemData(int iItem)
	{
		kfc_static_assert(sizeof(t) <= sizeof(LPARAM));
		return (t&)(void*&)temp<void*>(GetItemVoidData(iItem));
	}	

	template <class t>
	void SetItemData(int iItem, t Data)
	{
		kfc_static_assert(sizeof(t) <= sizeof(LPARAM));
		return SetItemVoidData(iItem, (void*&)Data);
	}

	template <class t>
	int FindItemByData(t Data)
	{
		kfc_static_assert(sizeof(t) <= sizeof(LPARAM));
		return FindItemByVoidData((void*)Data);
	}

	template <class t>
	int GetItemByData(t Data)
	{
		kfc_static_assert(sizeof(t) <= sizeof(LPARAM));
		return GetItemByVoidData((void*)Data);
	}

	IPOINT GetSubItemFromClientCoords(const IPOINT& Coords, bool bAsLClick = true);

	IPOINT GetSubItemFromScreenCoords(const IPOINT& Coords, bool bAsLClick = true);

	int GetItemFromClientCoords(const IPOINT& Coords, bool bAsLClick = true)
		{ return GetSubItemFromClientCoords(Coords, bAsLClick).y; }

	int GetItemFromScreenCoords(const IPOINT& Coords, bool bAsLClick = true)
		{ return GetSubItemFromScreenCoords(Coords, bAsLClick).y; }

	bool IsItemSelected(int iItem)
		{ return GetItemState(iItem) & LVIS_SELECTED; }

	HWND GetEditControl();

	HWND GetWindow() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hWnd; }

	operator HWND () const
		{ return GetWindow(); }
};

#endif // list_control_h
