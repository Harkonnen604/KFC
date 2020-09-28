#ifndef clipboard_h
#define clipboard_h

#ifdef _MSC_VER

// -----------------
// Clipboard opener
// -----------------
class TClipboardOpener
{
public:
    TClipboardOpener(HWND hWnd);

    ~TClipboardOpener();
};

// ----------------
// Global routines
// ----------------
bool GetClipboardText(HWND hWnd, KString& RText);

void SetClipboardText(  const KString&  Text,
                        HWND            hWnd,
                        bool            bClearFirst = true);

#endif // _MSC_VER

#endif // clipboard_h
