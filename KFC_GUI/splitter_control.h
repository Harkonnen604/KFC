#ifndef splitter_control_h
#define splitter_control_h

// -----------------
// Splitter control
// -----------------
class TSplitterControl
{
public:
    // Type
    enum TType
    {
        TYPE_HORIZONTAL,
        TYPE_VERTICAL
    };

private:
    HWND m_hWnd;

    TType m_Type;

    IRECT m_AreaRect;

    ISEGMENT m_Range;

    int m_iThickness;

private:
    bool m_bDragged;

    ISIZE m_DragOffset;

    IPOINT m_DragCoords;

    HWND m_hOldFocus;

private:
    static LRESULT CALLBACK StaticWindowProc
        (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

    LRESULT WindowProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

private:
    void OnPaint();

private:
    static ATOM ms_aHClass;
    static ATOM ms_aVClass;

    static UINT ms_uiResizeMessage;
public:
    static void Unregister();

    static void Register();

    static bool IsRegistered()
        { return ms_aHClass && ms_aVClass; }

    static UINT GetResizeMessage()
        { DEBUG_VERIFY(ms_uiResizeMessage); return ms_uiResizeMessage; }

private:
    void Canonize();

    IRECT GetDragRect();

    void NotifyResize();

private:
    void StartDrag  ();
    void StopDrag   (bool bAccept);
    void Drag       ();
    void DrawDrag   (bool bUpdateCoords);

public:
    TSplitterControl();

    ~TSplitterControl()
        { Release(); }

    bool IsAllocated() const
        { return m_hWnd; }

    void Release();

    void Create(HWND            hParentWnd,
                TType           Type,
                const IRECT&    AreaRect,
                const ISEGMENT& Range,
                int             iCoords,
                int             iThickness = 3);

    int GetThickness() const
        { return m_iThickness; }

    const IRECT& GetAreaRect() const
        { DEBUG_VERIFY_ALLOCATION; return m_AreaRect; }

    void Adjust(const IRECT& AreaRect, const ISEGMENT& Range);

    HWND GetWindow() const
        { DEBUG_VERIFY_ALLOCATION; return m_hWnd; }

    TType GetType() const
        { DEBUG_VERIFY_ALLOCATION; return m_Type; }

    operator HWND () const
        { return GetWindow(); }
};

#endif // splitter_control_h
