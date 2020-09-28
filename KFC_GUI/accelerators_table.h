#ifndef accelerators_table_h
#define accelerators_table_h

// -------------------
// Accelerators table
// -------------------
class TAcceleratorsTable
{
private:
    HACCEL m_hAccel;

public:
    TAcceleratorsTable();

    TAcceleratorsTable(HACCEL hAccel);

    TAcceleratorsTable(HINSTANCE hInstance, LPCTSTR pName);

    TAcceleratorsTable(int iID);

    ~TAcceleratorsTable()
        { Release(); }

    bool IsAllocated() const
        { return m_hAccel; }

    void Release();

    void Allocate(HACCEL hAccel);

    void Allocate(HINSTANCE hInstance, LPCTSTR pName);

    void Allocate(int iID)
        { Allocate(GetModuleHandle(NULL), MAKEINTRESOURCE(iID)); }

    bool Translate(HWND hWnd, LPMSG pMsg) const;

    HACCEL GetAccel() const;

    operator HACCEL () const { return GetAccel(); }
};

#endif // accelerators_table_h
