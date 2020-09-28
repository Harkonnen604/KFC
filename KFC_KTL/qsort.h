#ifndef qsort_h
#define qsort_h

// ---------------
// QSort function
// ---------------
typedef int TQSortFunction(const void* p1, const void* p2);

template <class t>
inline int CompareQSortFunction(const void* p1, const void* p2)
{
    return Compare(*(const t*)p1, *(const t*)p2);
}

#endif // qsort_h
