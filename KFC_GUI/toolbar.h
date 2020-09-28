#ifndef toolbar_h
#define toolbar_h

// --------
// Toolbar
// --------
class TToolbar
{
public:
    // Button
    struct TButton
    {
    public:
        int     m_iID;
        KString m_Text;

    public:
        TButton() : m_iID(-1) {}

        TButton(int iID, LPCTSTR pText) : m_iID(iID), m_Text(pText) {}
    };

    // Buttons
    typedef TArray<TButton> TButtons;

private:
    LRESULT ParentWindowProc
        (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, WNDPROC pOldWindowProc);

private:
    HWND m_hWnd;

    TButtons m_Buttons;

public:
    TToolbar();

    ~TToolbar()
        { Release(); }

    bool IsAllocated() const
        { return m_hWnd; }

    void Release();

    void Create(HWND            hParentWnd,
                DWORD           dwStyle,
                size_t          szButtonWidth,
                size_t          szButtonHeight,
                const TButtons& Buttons,
                HINSTANCE       hBitmapInstance,
                int             iBitmapResID,
                size_t          szBitmapButtonWidth,
                size_t          szBitmapButtonHeight,
                size_t          szNBitmapButtons    = UINT_MAX,
                int             iID                 = -1);

    void EnableButton(int iID, bool bEnable);

    void CheckButton(int iID, bool bCheck);

    void Resize();

    HWND GetWindow() const
        { DEBUG_VERIFY_ALLOCATION; return m_hWnd; }

    operator HWND () const
        { return GetWindow(); }
};

#endif //toolbar_h
