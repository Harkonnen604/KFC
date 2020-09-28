#ifndef placement_allocation_h
#define placement_allocation_h

// ----------------
// Global routines
// ----------------
#if !defined(KFC_NO_PLACEMENT_ALLOCATION) && !defined(__PLACEMENT_NEW_INLINE)

    #define __PLACEMENT_NEW_INLINE // #import

    inline void* operator new(size_t, void* pPtr)
        { return pPtr; }

    inline void operator delete(void*, void*) {}

#endif // KFC_NO_PLACEMENT_ALLOCATION

#endif // placement_allocation_h
