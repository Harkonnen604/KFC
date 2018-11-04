#ifndef check_list_control_h
#define check_list_control_h

#include "notification.h"

// -------------------
// Check list control
// -------------------
class TCheckListControl
{
private:
	// Item
	struct TItem
	{
		size_t	m_szListIndex;
		bool	m_bChecked;
		void*	m_pData;


		TItem() : m_szListIndex(UINT_MAX), m_bChecked(false), m_pData(NULL) {}

		TItem& Set(size_t szSListIndex, bool bSChecked, void* pSData = NULL)
		{
			m_szListIndex	= szSListIndex;
			m_bChecked		= bSChecked;
			m_pData			= pSData;

			return *this;
		}
	};

	// Items
	typedef TList<TItem> TItems;

public:
	// Column
	struct TColumn
	{
		KString		m_Title;
		size_t		m_szWidth;
		TAlignment	m_Alignment;


		TColumn() : m_szWidth(32), m_Alignment(ALIGNMENT_MIN) {}

		TColumn(LPCTSTR		pSTitle,
				size_t		szSWidth	= 32,
				TAlignment	SAlignment	= ALIGNMENT_MIN) :

			m_Title		(pSTitle),
			m_szWidth	(szSWidth),
			m_Alignment	(SAlignment) {}

		TColumn& Set(	LPCTSTR		pSTitle,
						size_t		szSWidth	= 32,
						TAlignment	SAlignment	= ALIGNMENT_MIN)
		{
			m_Title		= pSTitle;
			m_szWidth	= szSWidth;
			m_Alignment	= SAlignment;

			return *this;
		}
	};

private:
	// Columns
	typedef TArray<TColumn> TColumns;


	// Check list control
	bool m_bAllocated;

	size_t m_iID;

	HWND m_hWnd;

	WNDPROC m_pOldWindowProc;	

	TColumns m_Columns;

	TItems m_Items;

	KString m_EmptyText;

	bool m_bDrawChecks;	


	void OnInitDialog(HWND hWnd);

	static void OnMeasureItem(bool bDrawChecks, LPMEASUREITEMSTRUCT pMeasureItemStruct);

	void OnDrawItem(LPDRAWITEMSTRUCT pDrawItemStruct);

	bool OnNotify(const TNotification* pNotification);

	static LRESULT CALLBACK StaticSubWindowProc(HWND	hWnd,
												UINT	uiMsg,
												WPARAM	wParam,
												LPARAM	lParam);

	LRESULT SubWindowProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	void FixLastColumn() const;
	
	TItem& GetItem(size_t szListIndex);

	const TItem& GetItem(size_t szListIndex) const;	

public:
	// Item check notification, return TRUE to forbid check state transition.
	struct TItemCheckNotification : public TNotification
	{
		static const UINT m_uiCode;

		size_t	m_szListIndex;
		bool	m_bOldState;
		bool	m_bNewState;


		TItemCheckNotification(	HWND	hSWnd,
								int		iSID,
								size_t	szSListIndex,
								bool	bSOldState,
								bool	bSNewState) :

			TNotification(hSWnd, iSID, m_uiCode),

			m_szListIndex	(szSListIndex),
			m_bOldState		(bSOldState),
			m_bNewState		(bSNewState) {}
	};

	// Left button click notification, return value is ignored. Not sent or checkbox area.
	struct TLeftButtonClickNotification : public TNotification
	{
		static const UINT m_uiCode;

		size_t m_szListIndex;


		TLeftButtonClickNotification(	HWND	hSWnd,
										int		iSID,
										size_t	szSListIndex) :

			TNotification(hSWnd, iSID, m_uiCode),

			m_szListIndex(szSListIndex) {}
	};

	// Right button click notification, return value is ignored. Not sent or checkbox area.
	struct TRightButtonClickNotification : public TNotification
	{
		static const UINT m_uiCode;

		size_t m_szListIndex;


		TRightButtonClickNotification(	HWND	hSWnd,
										int		iSID,
										size_t	szSListIndex) :

			TNotification(hSWnd, iSID, m_uiCode),

			m_szListIndex(szSListIndex) {}
	};

	// Item double-click notification, return value is ignored. Not sent for checkbox area.
	struct TItemDoubleClickNotification : public TNotification
	{
		static const UINT m_uiCode;

		size_t m_szListIndex;


		TItemDoubleClickNotification(	HWND	hSWnd,
										int		iSID,
										size_t	szSListIndex) :

			TNotification(hSWnd, iSID, m_uiCode),

			m_szListIndex(szSListIndex) {}
	};

	// Selection change notification, return value is ignored.
	struct TSelectionChangeNotification : public TNotification
	{
		static const UINT m_uiCode;

		size_t m_szListIndex;


		TSelectionChangeNotification(	HWND	hSWnd,
										int		iSID,
										size_t	szSListIndex) :

			TNotification(hSWnd, iSID, m_uiCode),

			m_szListIndex(szSListIndex) {}
	};

	// Item color notification, return TRUE to set up referenced colors, return value is ignored
	struct TItemColorNotification : public TNotification
	{
		static const UINT m_uiCode;
		
		size_t		m_szColumnIndex;
		size_t		m_szListIndex;
		COLORREF	m_crTextBGColor;
		COLORREF	m_crTextFGColor;
		ISIZE		m_BorderSpacing; // negative - from text size, otherwise - from cell size


		TItemColorNotification(	HWND			hSWnd,
								int				iSID,
								size_t			szSColunmIndex,
								size_t			szSListIndex,
								COLORREF		crSTextBGColor = CLR_INVALID,
								COLORREF		crSTextFGColor = CLR_INVALID,
								const ISIZE&	SBorderSpacing = ISIZE(0, 0)) :

			TNotification(hSWnd, iSID, m_uiCode),

			m_szColumnIndex	(szSColunmIndex),
			m_szListIndex	(szSListIndex),
			m_crTextBGColor	(crSTextBGColor),
			m_crTextFGColor	(crSTextFGColor),
			m_BorderSpacing	(SBorderSpacing) {}
	};


	// Check list cotrol
	bool m_bAccelerateCheckNone;
	bool m_bAccelerateCheckAll;


	TCheckListControl();
	
	~TCheckListControl() { Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(	int				iSID,
					const TColumn*	pSColumns	= NULL,
					size_t			szNColumns	= 0);

	static bool ProcessStaticParentMessage(int iID, bool bDrawChecks, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	bool ProcessParentMessage(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	void Clear();

	size_t AddItem(LPCTSTR pText, bool bChecked, void* pData = NULL); // returns list index

	void DeleteItem(size_t szListIndex);

	KString GetSubItemText(size_t szListIndex, size_t szColIndex) const;

	void SetSubItemText(size_t szListIndex, size_t szColIndex, LPCTSTR pText);

	void* GetItemData(size_t szListIndex) const;

	size_t GetCurrentSelection() const; // returns list index

	void SetCurrentSelection(size_t szListIndex) const;

	void SetInitialFocus();

	size_t GetColumnWidth(size_t szIndex) const;

	bool GetCheck(size_t szListIndex) const;

	bool SetCheck(size_t szListIndex, bool bCheck);	

	bool ToggleCheck(size_t szListIndex);

	size_t FindCheck(size_t szListAfter = UINT_MAX) const;

	void CheckRange(size_t szStart, size_t szAmt, bool bCheck = true);

	void CheckAll(bool bCheck);

	void InvalidateItemRect(size_t szListIndex);

	const KString& GetEmptyText() const;

	void SetEmptyText(LPCTSTR pSEmptyText);

	bool GetDrawChecks() const;

	void SetDrawChecks(bool bSDrawChecks);

	// Getters
	size_t GetN() const;

	HWND GetWnd() const;

	operator HWND () const { return m_hWnd; }

	// ---------------- TRIVIALS ----------------
	int GetID() const { return m_iID; }
};

#endif // check_list_control_h