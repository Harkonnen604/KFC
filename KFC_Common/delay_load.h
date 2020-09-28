#ifndef delay_load_h
#define delay_load_h

#include "dll.h"

#ifdef _MSC_VER

// ------------
// Fast access
// ------------
template <class t>
t GetDelayLoadFunction(LPCTSTR pDllName, LPCTSTR pProcName)
{
    T_DLL DLL(pDllName);

    t pProc = (t)DLL[pProcName];

    DLL.Invalidate();

    return pProc;
}

// --------------------
// Delay load function
// --------------------
template <class FunctionType>
class TDelayLoadFunction
{
private:
    KString m_DLLFileName;
    KString m_FunctionName;

    mutable T_DLL m_DLL;

    mutable FunctionType* m_pFunction;

public:
    TDelayLoadFunction();

    TDelayLoadFunction(LPCTSTR pSDLLFileName, LPCTSTR pSFunctionName);

    ~TDelayLoadFunction() { Release(); }

    bool IsAllocated() const
        { return !m_DLLFileName.IsEmpty() || !m_FunctionName.IsEmpty(); }

    void Release();

    void Allocate(LPCTSTR pSDLLFileName, LPCTSTR pSFunctionName);

    FunctionType* GetFunction() const;

    FunctionType* operator * () const
        { return GetFunction(); }

    FunctionType* operator () () const
        { return GetFunction(); }
};

template <class FunctionType>
TDelayLoadFunction<FunctionType>::TDelayLoadFunction()
{
    m_pFunction = NULL;
}

template <class FunctionType>
TDelayLoadFunction<FunctionType>::TDelayLoadFunction(LPCTSTR pSDLLFileName, LPCTSTR pSFunctionName)
{
    m_pFunction = NULL;

    Allocate(pSDLLFileName, pSFunctionName);
}

template <class FunctionType>
void TDelayLoadFunction<FunctionType>::Release()
{
    m_pFunction = NULL;

    m_DLL.Release();

    m_FunctionName. Empty();
    m_DLLFileName.  Empty();
}

template <class FunctionType>
void TDelayLoadFunction<FunctionType>::Allocate(LPCTSTR pSDLLFileName, LPCTSTR pSFunctionName)
{
    Release();

    try
    {
        DEBUG_VERIFY(pSDLLFileName && pSDLLFileName[0]);

        DEBUG_VERIFY(pSFunctionName && pSFunctionName[0]);

        m_DLLFileName   = pSDLLFileName;
        m_FunctionName  = pSFunctionName;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

template <class FunctionType>
FunctionType* TDelayLoadFunction<FunctionType>::GetFunction() const
{
    DEBUG_VERIFY_ALLOCATION;

    if(m_DLL.IsAllocated())
    {
        DEBUG_VERIFY(m_pFunction);
    }
    else
    {
        m_DLL.Allocate(m_DLLFileName);

        m_pFunction = (FunctionType*)m_DLL.GetFunction(TAnsiString(m_FunctionName));

        DEBUG_VERIFY(m_pFunction);
    }

    return m_pFunction;
}

#endif // _MSC_VER

#endif // delay_load_h
