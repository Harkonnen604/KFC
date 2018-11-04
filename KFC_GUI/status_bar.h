#ifndef status_bar_h
#define status_bar_h

// -----------
// Status bar
// -----------
class TStatusBar
{
private:
	HWND m_hWnd;

public:
	TStatusBar();

	~TStatusBar()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hWnd; }

	void Release();

	void Create(HWND hParentWnd, DWORD dwStyle = SBARS_SIZEGRIP);

	void SetParts(const int* pWidths, size_t szN);

	void Resize();

	KString GetText() const;

	void SetText(LPCTSTR pText);

	void SetText(size_t i, LPCTSTR pText);

	HWND GetWindow() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hWnd; }

	operator HWND () const
		{ return GetWindow(); }
};

#endif // status_bar_h
