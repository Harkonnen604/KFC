#include "kfc_common_pch.h"
#include "clipboard.h"

#include "global_mem.h"

#ifdef _MSC_VER

// -----------------
// Clipboard opener
// -----------------
TClipboardOpener::TClipboardOpener(HWND hWnd)
{
    if(!OpenClipboard(hWnd))
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error opening clipboard"),
                                        GetLastError());
    }
}

TClipboardOpener::~TClipboardOpener()
{
    CloseClipboard();
}

// ----------------
// Global routines
// ----------------
bool GetClipboardText(HWND hWnd, KString& RText)
{
    TClipboardOpener Opener0(hWnd);

    HANDLE hData = GetClipboardData(CF_TEXT);

    if(!hData)
        return false;

    LPCTSTR pText = (LPCTSTR)GlobalLock(hData);

    if(!pText || pText[GlobalSize(hData) - 1])
        return false;

    RText = pText;

    GlobalUnlock(hData);

    return true;
}

void SetClipboardText(  const KString&  Text,
                        HWND            hWnd,
                        bool            bClearFirst)
{
    // Allocating global memory
    TGlobalMem Mem(Text.GetStreamTermCharsLength(), GMEM_MOVEABLE);

    // Copying text into the global memory
    {
        void* pData;
        TGlobalMemLocker Locker0(Mem, pData);

        memcpy(pData, (LPCTSTR)Text, Text.GetStreamTermCharsLength());
    }

    // Accessing the clipboard
    {
        // Opening the clipboard
        TClipboardOpener Opener0(hWnd);

        // Clearing the clipboard
        if(bClearFirst)
        {
            if(!EmptyClipboard())
            {
                INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error emptying clipboard."),
                                                GetLastError());
            }
        }

        // Setting clipboard data
        HANDLE hData = SetClipboardData(CF_TEXT, Mem);

        if(hData == NULL)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error setting clipboard text data."),
                                            GetLastError());
        }

        Mem.Invalidate();
    }
}

#endif // _MSC_VER
