#ifndef edit_filter_h
#define edit_filter_h

#include <KFC_KTL\globals.h>

// --------------------
// Edit filter globals
// --------------------
class TEditFilterGlobals : public TGlobals
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

public:
    ATOM m_aAllowPropName;
    ATOM m_aDenyPropName;

public:
    TEditFilterGlobals();
};

extern TEditFilterGlobals g_EditFilterGlobals;

// ----------------
// Global routines
// ----------------
void FilterEditControl(HWND hWnd, LPCTSTR pAllow, LPCTSTR pDeny = NULL);

inline void UnfilterEditControl(HWND hWnd)
    { FilterEditControl(hWnd, NULL); }

#endif // edit_filter_h
