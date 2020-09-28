#ifndef kfc_mem_h
#define kfc_mem_h

// #define _DEBUG_HEAP_PADDING      (256)

// ---------------------
// KFC memory exception
// ---------------------
struct T_KFC_MemoryException {};

// ----------------
// Helper routines
// ----------------

#ifdef _DEBUG_HEAP_PADDING

inline void kfc_mem_debug_fill(void* pData)
{
    assert(pData);

    pData = (BYTE*)pData - _DEBUG_HEAP_PADDING;

    int iSize = _msize(pData);
    assert(iSize >= _DEBUG_HEAP_PADDING * 2);

    memset(pData, 0xDC, _DEBUG_HEAP_PADDING);
    memset((BYTE*)pData + iSize - _DEBUG_HEAP_PADDING, 0xDE, _DEBUG_HEAP_PADDING);
}

inline void kfc_mem_debug_verify(void* pData)
{
    if(!pData)
        return;

    pData = (BYTE*)pData - _DEBUG_HEAP_PADDING;

    int iSize = _msize(pData);
    assert(iSize >= _DEBUG_HEAP_PADDING * 2);

    BYTE*  p;
    size_t i;

    for(p = (BYTE*)pData, i = _DEBUG_HEAP_PADDING ; i ; i--, p++)
        assert(*p == 0xDC);

    for(p = (BYTE*)pData + iSize - _DEBUG_HEAP_PADDING, i = _DEBUG_HEAP_PADDING ; i ; i--, p++)
        assert(*p == 0xDE);
}

#endif // _DEBUG_HEAP_PADDING

// ----------------
// Global routines
// ----------------
inline void* kfc_malloc(size_t szSize)
{
    #ifdef _DEBUG_HEAP_PADDING
    {
        void* pData = malloc(szSize + _DEBUG_HEAP_PADDING * 2);

        if(!pData)
            throw T_KFC_MemoryException();

        pData = (BYTE*)pData + _DEBUG_HEAP_PADDING;

        kfc_mem_debug_fill(pData);

        return pData;
    }
    #else // _DEBUG_HEAP_PADDING
    {
        void* pData = malloc(szSize);

        if(szSize && !pData)
            throw T_KFC_MemoryException();

        return pData;
    }
    #endif // _DEBUG_HEAP_PADDING
}

inline void* kfc_calloc(size_t szNum, size_t szSize)
{
    #ifdef _DEBUG_HEAP_PADDING
    {
        void* pData = kfc_malloc(szNum * szSize);

        memset(pData, 0, szNum * szSize);

        return pData;
    }
    #else // _DEBUG_HEAP_ADING
    {
        void* pData = calloc(szNum, szSize);

        if(szSize && !pData)
            throw T_KFC_MemoryException();

        return pData;
    }
    #endif // _DEBUG_HEAP_PADDING
}

template <class t>
inline t* kfc_realloc(t*& pData, size_t szSize)
{
    #ifdef _DEBUG_HEAP_PADDING
    {
        kfc_mem_debug_verify(pData);

        if(pData)
            pData = (t*)((BYTE*)pData - _DEBUG_HEAP_PADDING);

        pData = (t*)realloc(pData, szSize + _DEBUG_HEAP_PADDING * 2);

        if(!pData)
            throw T_KFC_MemoryException();

        pData = (t*)((BYTE*)pData + _DEBUG_HEAP_PADDING);

        kfc_mem_debug_fill(pData);

        return pData;
    }
    #else // _DEBUG_HEAP_PADDING
    {
        pData = (t*)realloc(pData, szSize);

        if(szSize && !pData)
            throw T_KFC_MemoryException();
    }
    #endif // _DEBUG_HEAP_PADDING

    return pData;
}

template <class t>
inline void kfc_free(t*& pData)
{
    #ifdef _DEBUG_HEAP_PADDING
    {
        if(!pData)
            return;

        kfc_mem_debug_verify(pData);

        pData = (t*)((BYTE*)pData - _DEBUG_HEAP_PADDING);

        free(pData), pData = NULL;
    }
    #else // _DEBUG_HEAP_PADDDING
    {
        free(pData), pData = NULL;
    }
    #endif // _DEBUG_HEAP_PADDING
}

#ifdef _DEBUG_HEAP_PADDING

inline void* operator new (size_t szSize)
    { return kfc_malloc(szSize); }

inline void operator delete (void* pData)
    { kfc_free(pData); }

#endif // _DEBUG_HEAP_PADDING

#endif // kfc_mem_h
