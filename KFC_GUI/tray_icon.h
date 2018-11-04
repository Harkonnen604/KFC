#ifndef tray_icon_h
#define tray_icon_h

// ----------
// Tray icon
// ----------
class TTrayIcon
{
private:
	bool m_bAllocated;

	HWND m_hWnd;

	HICON m_hIcon;

	UINT m_uiID;

public:
	bool m_bFailSafe; // prevents exceptions for dead tray under non-NT

public:
	TTrayIcon();

	~TTrayIcon()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(	HWND	hSWnd,
					UINT	uiSID,
					HICON	hSIcon,
					UINT	uiMsg,
					LPCTSTR	pToolTip = NULL);

	void SetIcon(HICON hSIcon);

	void SetToolTip(LPCTSTR pToolTip);

	static void HideWindow		(HWND hWnd, bool bSafe = true);
	static void RestoreWindow	(HWND hWnd, bool bSafe = true);

	// ---------------- TRIVIALS ----------------
	HWND GetWnd() const { return m_hWnd; }

	UINT GetID() const { return m_uiID; }

	HICON GetIcon() const { return m_hIcon; }
};

#endif // tray_icon_h