#ifndef basic_wintypes_h
#define basic_wintypes_h

#ifdef _MSC_VER
#ifndef __AFX_H__
    #include <mmreg.h>
#endif // __AFX_H
#endif // _MSC_VER

#ifdef _MSC_VER
    #include <tchar.h>
#endif // _MSC_VER

#include <ctype.h>

// Casts
#define LONG_REF_CAST(Value)    (*(LONG*)(&(Value)))

// -------------------------
// Basic wintypes streaming
// -------------------------
DECLARE_BASIC_STREAMING(FILETIME);
DECLARE_BASIC_STREAMING(SYSTEMTIME);

// --------------------------
// Basic wintypes comparison
// --------------------------
inline int Compare(HWND hWnd1, HWND hWnd2)
{
    return hWnd1 < hWnd2 ? -1 : hWnd1 > hWnd2 ? +1 : 0;
}

#endif // basic_wintypes_h
