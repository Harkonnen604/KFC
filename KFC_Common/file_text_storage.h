#ifndef file_text_storage_h
#define file_text_storage_h

#include <KFC_KTL/flexible_item_heap.h>
#include <KFC_Common/file_mapping.h>

#ifdef _MSC_VER

// ------------------
// File text storage
// ------------------
class TFileTextStorage
{
private:
    // Storage
    typedef TFlexibleItemHeap
        <TFileMappingGrowableAllocator, TFileMappingGrowableAllocator>
            TStorage;

public:
    // Iterator
    typedef TStorage::TIterator TIterator;

    // Const iterator
    typedef TStorage::TConstIterator TConstIterator;

private:
    TStorage m_Storage;

private:
    TIterator AddText(LPCTSTR pText, size_t szLength);

    TIterator ReplaceText(TIterator Iter, LPCTSTR pText, size_t szLength)
        { return DelText(Iter), AddText(pText, szLength); }

public:
    TFileTextStorage() {}

    TFileTextStorage(LPCTSTR pFileNamePrefix)
        { Allocate(pFileNamePrefix); }

    ~TFileTextStorage()
        { Release(); }

    bool IsAllocated() const
        { return m_Storage.IsAllocated(); }

    void Release();

    void Allocate(LPCTSTR pFileNamePrefix);

    void Clear();

    LPCTSTR GetText(const TConstIterator Iter) const
        { DEBUG_VERIFY_ALLOCATION; return (LPCTSTR)m_Storage[Iter]; }

    LPCTSTR operator [] (const TConstIterator Iter) const
        { return GetText(Iter); }

    void DelText(TIterator Iter)
        { DEBUG_VERIFY_ALLOCATION; m_Storage.Free(Iter); }

    TIterator AddText(LPCTSTR pText)
        { DEBUG_VERIFY(pText); return AddText(pText, _tcslen(pText)); }

    TIterator AddText(const KString& Text)
        { return AddText(Text.GetDataPtr(), Text.GetLength()); }

    TIterator ReplaceText(TIterator Iter, LPCTSTR pText)
        { return ReplaceText(Iter, pText, _tcslen(pText)); }

    TIterator ReplaceText(TIterator Iter, const KString& Text)
        { return ReplaceText(Iter, Text.GetDataPtr(), Text.GetLength()); }
};

#endif // _MSC_VER

#endif // file_text_storage_h
