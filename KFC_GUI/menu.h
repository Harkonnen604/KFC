#ifndef menu_h
#define menu_h

// -----
// Menu
// -----
class TMenu
{
private:
    HMENU m_hMenu;

    bool m_bSubMenu;

public:
    TMenu();

    TMenu(HINSTANCE hInstance, LPCTSTR pName);

    TMenu(int iID);

    TMenu(HMENU hSMenu);

    ~TMenu()
        { Release(); }

    bool IsAllocated() const
        { return m_hMenu; }

    void Release();

    void Allocate(HINSTANCE hInstance, LPCTSTR pName);

    void Allocate(int iID)
        { Allocate(GetModuleHandle(NULL), MAKEINTRESOURCE(iID)); }

    void Allocate(HMENU hSMenu);

    HMENU GetSubMenu(size_t szPos) const;

    void EnableItem(int iID, bool bEnable);

    void CheckItem(int iID, bool bCheck);

    void TrackPopup(HWND            hWnd,
                    const IPOINT&   Coords,
                    const ISIZE&    Offset  = ISIZE(0, 0),
                    kflags_t        flFlags = TPM_TOPALIGN | TPM_LEFTALIGN);

    HMENU GetMenu() const;

    operator HMENU () const { return GetMenu(); }

    // ---------------- TRIVIALS ----------------
    bool IsSubMenu() const { return m_bSubMenu; }
};

// --------
// Submenu
// --------
class TSubMenu : public TMenu
{
private:
    TMenu m_Menu;

public:
    TSubMenu();

    TSubMenu(HINSTANCE hInstance, LPCTSTR pName, size_t szPos = 0);

    TSubMenu(int iID, size_t szPos = 0);

    ~TSubMenu()
        { Release(); }

    bool IsAllocated()
        { return TMenu::IsAllocated(); }

    void Release();

    void Allocate(HINSTANCE hInstance, LPCTSTR pName, size_t szPos = 0);

    void Allocate(int iID, size_t szPos = 0)
        { Allocate(GetModuleHandle(NULL), MAKEINTRESOURCE(iID), szPos); }
};

#endif // menu_h
