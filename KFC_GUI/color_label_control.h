#ifndef color_label_control_h
#define color_label_control_h

// --------------------
// Color label control
// --------------------
class TColorLabelControl
{
private:
    static ATOM s_aClass;

    bool m_bAllocated;

    HWND m_hWnd;

    COLORREF m_crBGColor;
    COLORREF m_crFGColor;

    TAlignment m_alTextAlignment;


    static void UnregisterClass();

    static void RegisterClass();

    static LRESULT CALLBACK StaticWindowProc(   HWND    hWnd,
                                                UINT    uiMsg,
                                                WPARAM  wParam,
                                                LPARAM  lParam);

    LRESULT WindowProc( HWND    hWnd,
                        UINT    uiMsg,
                        WPARAM  wParam,
                        LPARAM  lParam);

    friend class T_GUI_DeviceGlobals;

public:
    TColorLabelControl();

    ~TColorLabelControl() { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  HWND            hParentWnd,
                    kflags_t        flStyle,
                    kflags_t        flExStyle,
                    const IRECT&    Rect,
                    LPCTSTR         pSText          = TEXT(""),
                    TAlignment      STextAlignment  = ALIGNMENT_MID,
                    COLORREF        crSBGColor      = ::GetSysColor(COLOR_BTNFACE),
                    COLORREF        crSFGColor      = ::GetSysColor(COLOR_BTNTEXT));

    COLORREF GetBGColor() const;
    COLORREF GetFGColor() const;

    void SetBGColor(COLORREF crSBGColor);
    void SetFGColor(COLORREF crSFGColor);

    void Move(  const IPOINT&   Coords,
                const ISIZE&    Size,
                const ALSIZE&   Alignment,
                HWND            hInsertAfter = NULL);

    HWND GetWnd() const;

    operator HWND () const { return GetWnd(); }
};

#endif // color_label_control_h
