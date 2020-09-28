#ifndef mouse_hoverer_h
#define mouse_hoverer_h

// --------------
// Mouse hoverer
// --------------
class TMouseHoverer
{
private:
    HWND m_hWnd;

    bool m_bHovered;

public:
    TMouseHoverer();

    ~TMouseHoverer()
        { Release(); }

    bool IsAllocated() const
        { return m_hWnd; }

    void Release();

    int Allocate(HWND hWnd);

    int ProcessMessage(UINT uiMsg);

    bool IsHovered() const
        { DEBUG_VERIFY_ALLOCATION; return m_bHovered; }
};

#endif // mouse_hoverer_h
