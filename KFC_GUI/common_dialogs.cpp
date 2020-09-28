#include "kfc_gui_pch.h"
#include "common_dialogs.h"

#include <KFC_KTL\file_names.h>
#include <KFC_Common\file.h>
#include "gui.h"

// ----------------
// Global routines
// ----------------
bool GetDialogOpenFileName( HWND        hParentWnd,
                            LPCTSTR     pTitle,
                            LPCTSTR     pFilter,
                            LPCTSTR     pInitialFileName,
                            KString&    RFileName,
                            LPCTSTR     pDefExt,
                            size_t      szFilterIndex)
{
    DEBUG_VERIFY(pInitialFileName && pFilter);

    TCurrentDirectoryPreserver Preserver0;

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn)), ofn.lStructSize = sizeof(ofn);

    TCHAR Buffer[MAX_PATH];
    _tcscpy(Buffer, pInitialFileName);

    ofn.hwndOwner       = hParentWnd;
    ofn.lpstrFile       = Buffer;
    ofn.nMaxFile        = ARRAY_SIZE(Buffer);
    ofn.lpstrFilter     = pFilter;
    ofn.nFilterIndex    = szFilterIndex;
    ofn.lpstrTitle      = pTitle;
    ofn.lpstrDefExt     = pDefExt;
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;

    if(!GetOpenFileName(&ofn))
        return false;

    RFileName = Buffer;

    return true;
}

bool GetDialogOpenFileNames(HWND        hParentWnd,
                            LPCTSTR     pTitle,
                            LPCTSTR     pFilter,
                            KStrings&   RFileNames,
                            LPCTSTR     pDefExt,
                            size_t      szFilterIndex)
{
    DEBUG_VERIFY(pFilter);

    TCurrentDirectoryPreserver Preserver0;

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn)), ofn.lStructSize = sizeof(ofn);

    TArray<TCHAR, true> Buffer;
    Buffer.Add(65535) = 0;

    ofn.hwndOwner       = hParentWnd;
    ofn.lpstrFile       = Buffer.GetDataPtr();
    ofn.nMaxFile        = Buffer.GetN();
    ofn.lpstrFilter     = pFilter;
    ofn.nFilterIndex    = szFilterIndex;
    ofn.lpstrTitle      = pTitle;
    ofn.lpstrDefExt     = pDefExt;

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
                    OFN_ALLOWMULTISELECT |
                    OFN_EXPLORER | OFN_HIDEREADONLY;

    if(!GetOpenFileName(&ofn))
        return false;

    // Extracting files names
    {
        KStrings Tokens;

        KString cur;

        size_t i;
        for(i = 0 ; ; i++)
        {
            if(ofn.lpstrFile[i] == 0)
            {
                if(cur.IsEmpty())
                    break;

                *Tokens.AddLast() = cur, cur.Empty();
            }
            else
            {
                cur += ofn.lpstrFile[i];
            }
        }

        DEBUG_VERIFY(Tokens.GetN() > 0);

        RFileNames.Clear();

        if(Tokens.GetN() == 1)
        {
            *RFileNames.AddLast() = *Tokens.GetFirst();
        }
        else
        {
            const KString FolderName = SlashedFolderName(*Tokens.GetFirst());

            for(KStrings::TConstIterator Iter = Tokens.GetFirst().GetNext() ;
                Iter.IsValid() ;
                ++Iter)
            {
                *RFileNames.AddLast() = FolderName + *Iter;
            }
        }
    }

    return true;
}

bool GetDialogSaveFileName( HWND        hParentWnd,
                            LPCTSTR     pTitle,
                            LPCTSTR     pFilter,
                            LPCTSTR     pInitialFileName,
                            KString&    RFileName,
                            LPCTSTR     pDefExt,
                            size_t      szFilterIndex)
{
    DEBUG_VERIFY(pInitialFileName && pFilter);

    TCurrentDirectoryPreserver Preserver0;

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn)), ofn.lStructSize = sizeof(ofn);

    TCHAR Buffer[MAX_PATH];
    _tcscpy(Buffer, pInitialFileName);

    ofn.hwndOwner       = hParentWnd;
    ofn.lpstrFile       = Buffer;
    ofn.nMaxFile        = ARRAY_SIZE(Buffer);
    ofn.lpstrFilter     = pFilter;
    ofn.nFilterIndex    = szFilterIndex;
    ofn.lpstrTitle      = pTitle;
    ofn.lpstrDefExt     = pDefExt;
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_EXPLORER;

    if(!GetSaveFileName(&ofn))
        return false;

    RFileName = Buffer;

    return true;
}

bool ChooseFont(HWND        hParentWnd,
                bool        bUsePassed,
                LOGFONT&    lfRFont,
                bool        bAllowEffects)
{
    CHOOSEFONT cf;
    memset(&cf, 0, sizeof(cf)), cf.lStructSize = sizeof(cf);

    LOGFONT lfFont = lfRFont;

    cf.hwndOwner = hParentWnd;

    cf.lpLogFont = &lfFont;

    cf.Flags = CF_SCREENFONTS | (bAllowEffects ? CF_EFFECTS : 0) | (bUsePassed ? CF_INITTOLOGFONTSTRUCT : 0);

    cf.nFontType = SCREEN_FONTTYPE;

    if(!ChooseFont(&cf))
        return false;

    lfRFont = lfFont;

    return true;
}

bool ChooseColor(   HWND        hParentWnd,
                    COLORREF    CustomColors[16],
                    bool        bUsePassed,
                    COLORREF&   crRColor)
{
    CHOOSECOLOR cc;
    memset(&cc, 0, sizeof(cc)), cc.lStructSize = sizeof(cc);

    cc.hwndOwner    = hParentWnd;
    cc.rgbResult    = crRColor;
    cc.lpCustColors = CustomColors;
    cc.Flags        = CC_ANYCOLOR | CC_FULLOPEN | (bUsePassed ? CC_RGBINIT : 0);

    if(!ChooseColor(&cc))
        return false;

    crRColor = cc.rgbResult;

    return true;
}
