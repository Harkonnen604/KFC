#ifndef ptr_holder_h
#define ptr_holder_h

#include "kfc_mem.h"

// ------------------
// C pointer deleter
// ------------------
template <class t>
class c_ptr_deleter
{
public:
    static void Delete(t* p)
        { kfc_free(p); }
};

// --------------------
// CPP pointer deleter
// --------------------
template <class t>
class cpp_ptr_deleter
{
public:
    static void Delete(const t* p)
        { delete p; }
};

// --------------------------
// CPP array pointer deleter
// --------------------------
template <class t>
class cpp_array_ptr_deleter
{
public:
    static void Delete(const t* p)
        { delete[] p; }
};

// ---------------
// Pointer holder
// ---------------
template <class t, template <class> class ptr_deleter = cpp_ptr_deleter>
class TPtrHolder
{
private:
    t* m_pData;

public:
    TPtrHolder() : m_pData(NULL) {}

    TPtrHolder(t* pData) : m_pData(pData) {}

    TPtrHolder(const TPtrHolder& SHolder) : m_pData(NULL)
        { *this = SHolder; }

    ~TPtrHolder()
        { *this = NULL; }

    // Nullifies own pointer, but does not destroy the object
    TPtrHolder& Invalidate()
        { m_pData = NULL; return *this; }

    TPtrHolder& operator = (t* pSData)
    {
        if(m_pData != pSData)
            ptr_deleter<t>::Delete(m_pData), m_pData = pSData;

        return *this;
    }

    TPtrHolder& operator = (const TPtrHolder& SHolder)
    {
        if(m_pData != SHolder.m_pData)
        {
            ptr_deleter<t>::Delete(m_pData);

            m_pData = SHolder ? SHolder.m_pData->CreateCopy() : NULL;
        }

        return *this;
    }

    operator t* () const
        { return m_pData; }

    t* operator -> () const
        { assert(m_pData); return m_pData; }

    t* Extract()
        { t* pData = m_pData; Invalidate(); return pData; }

    t*& GetPtr()
        { *this = NULL; return m_pData; }
};

#endif // ptr_holder_h
