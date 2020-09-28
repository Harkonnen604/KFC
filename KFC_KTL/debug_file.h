#ifndef debug_file_h
#define debug_file_h

#include "critical_section.h"

// -------------------------
// Debug file control chars
// -------------------------
enum TDebugFileControlChar
{
    DFCC_CLEAR      = 1,
    DFCC_EOL        = 2,
    DFCC_FORCE_UINT = UINT_MAX,
};

// -----------
// Debug file
// -----------
class TDebugFile
{
private:
    bool m_bAllocated;

    KString m_FileName;

    TCriticalSection m_AccessCS;

private:
    void Clear();

public:
    TDebugFile();

    ~TDebugFile();

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(const KString& SFileName, bool bClear = true);

    void Lock()
        { m_AccessCS.Lock(); }

    void Unlock()
        { m_AccessCS.Unlock(); }

    TDebugFile& operator << (LPCTSTR pString);

    TDebugFile& operator << (TCHAR cChar)
        { return *this << KString(cChar); }

    TDebugFile& operator << (LPTSTR pString)
        { return *this << (LPCTSTR)pString; }

    TDebugFile& operator << (const KString& String)
        { return *this << (LPCTSTR)String; }

    TDebugFile& operator << (INT64  iValue)     { return *this << KString(iValue);  }
    TDebugFile& operator << (int    iValue)     { return *this << KString(iValue);  }
    TDebugFile& operator << (QWORD  qwValue)    { return *this << KString(qwValue); }
    TDebugFile& operator << (UINT   uiValue)    { return *this << KString(uiValue); }

    TDebugFile& operator << (float          fValue)     { return *this << KString(fValue);      }
    TDebugFile& operator << (double         dValue)     { return *this << KString(dValue);      }
    TDebugFile& operator << (bool           bValue)     { return *this << KString(bValue);      }
    TDebugFile& operator << (const void*    pPtr)       { return *this << KString(pPtr);        }
    TDebugFile& operator << (TAlignment     Alignment)  { return *this << KString(Alignment);   }

    TDebugFile& operator << (TDebugFileControlChar ControlChar);

    // ---------------- TRIVIALS ----------------
    const KString& GetFileName() const { return m_FileName; }
};

extern TDebugFile g_DebugFile;

// ------------------
// Debug file locker
// ------------------
class TDebugFileLocker
{
public:
    TDebugFileLocker()
        { g_DebugFile.Lock(); }

    ~TDebugFileLocker()
        { g_DebugFile.Unlock(); }
};

// --------------------
// Basic bitypes debug
// --------------------
template <class t>
inline TDebugFile& operator << (TDebugFile& DebugFile, const TPoint<t>& Point)
{
    return DebugFile << (KString)Point;
}

template <class t>
inline TDebugFile& operator << (TDebugFile& DebugFile, const TRect<t>& Rect)
{
    return DebugFile << (KString)Rect;
}

template <class t>
inline TDebugFile& operator << (TDebugFile& DebugFile, const TSize<t>& Size)
{
    return DebugFile << (KString)Size;
}

template <class t>
inline TDebugFile& operator << (TDebugFile& DebugFile, const TSegment<t>& Segment)
{
    return DebugFile << (KString)Segment;
}

#endif // debug_file_h
