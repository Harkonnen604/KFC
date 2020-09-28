#ifndef dll_h
#define dll_h

#ifdef _MSC_VER

// ----
// DLL
// ----
class T_DLL
{
private:
    bool m_bAllocated;

    HMODULE m_hModule;

public:
    T_DLL();

    T_DLL(LPCTSTR pFileName, kflags_t flFlags = 0);

    T_DLL(HMODULE hSModule);

    ~T_DLL() { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(LPCTSTR pFileName, kflags_t flFlags = 0);

    void Allocate(HMODULE hSModule);

    void ReOwn(T_DLL& DLL);

    void Invalidate();

    void* FindFunction(LPCSTR pFunctionName) const;

    void* GetFunction(LPCSTR pFunctionName) const;

    void* operator [] (LPCSTR pFunctionName) const
        { return GetFunction(pFunctionName); }

    HMODULE GetModule() const;

    operator HMODULE () const { return m_hModule; }
};

#endif // _MSC_VER

#endif // dll_h
