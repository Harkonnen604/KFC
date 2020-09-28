#include "kfc_gui_pch.h"
#include "tray_icon.h"

#include "gui.h"

// ----------
// Tray icon
// ----------
TTrayIcon::TTrayIcon()
{
    m_bAllocated = false;

    m_hWnd  = NULL;
    m_hIcon = NULL;

#ifdef _DEBUG
    m_bFailSafe = false;
#else // _DEBUG
    m_bFailSafe = true;
#endif // _DEBUG
}

void TTrayIcon::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        if(m_hWnd && m_hIcon)
        {
            NOTIFYICONDATA Data;
            memset(&Data, 0, sizeof(Data)), Data.cbSize = sizeof(Data);

            Data.hWnd   = m_hWnd;
            Data.uID    = m_uiID;

            Shell_NotifyIcon(NIM_DELETE, &Data);

            m_hIcon = NULL, m_hWnd = NULL;
        }
    }
}

void TTrayIcon::Allocate(   HWND    hSWnd,
                            UINT    uiSID,
                            HICON   hSIcon,
                            UINT    uiMsg,
                            LPCTSTR pToolTip)
{
    Release();

    try
    {
        DEBUG_VERIFY(hSWnd);

        DEBUG_VERIFY(hSIcon);

        DEBUG_VERIFY(pToolTip == NULL || _tcslen(pToolTip) < 64);

        NOTIFYICONDATA Data;
        memset(&Data, 0, sizeof(Data)), Data.cbSize = sizeof(Data);

        Data.hWnd               = hSWnd;
        Data.uID                = uiSID;
        Data.hIcon              = hSIcon;
        Data.uCallbackMessage   = uiMsg;
        Data.uFlags             = NIF_ICON | NIF_MESSAGE;

        if(pToolTip)
            _tcscpy(Data.szTip, pToolTip), Data.uFlags |= NIF_TIP;

        if(!Shell_NotifyIcon(NIM_ADD, &Data) && !m_bFailSafe)
        {
            INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error creating tray icon"),
                                            GetLastError());
        }

        m_hWnd = hSWnd, m_uiID = uiSID, m_hIcon = hSIcon;

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void TTrayIcon::SetIcon(HICON hSIcon)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(hSIcon);

    if(m_hIcon == hSIcon)
        return;

    NOTIFYICONDATA Data;
    memset(&Data, 0, sizeof(Data)), Data.cbSize = sizeof(Data);

    Data.hWnd   = m_hWnd;
    Data.uID    = m_uiID;
    Data.hIcon  = hSIcon;
    Data.uFlags = NIF_ICON;

    if(!Shell_NotifyIcon(NIM_MODIFY, &Data) && !m_bFailSafe)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error changing tray icon"),
                                        GetLastError());
    }

    m_hIcon = hSIcon;
}

void TTrayIcon::SetToolTip(LPCTSTR pToolTip)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(pToolTip && _tcslen(pToolTip) < 64);

    NOTIFYICONDATA Data;
    memset(&Data, 0, sizeof(Data)), Data.cbSize = sizeof(Data);

    Data.hWnd   = m_hWnd;
    Data.uID    = m_uiID;

    _tcscpy(Data.szTip, pToolTip), Data.uFlags |= NIF_TIP;

    if(!Shell_NotifyIcon(NIM_MODIFY, &Data) && !m_bFailSafe)
    {
        INITIATE_DEFINED_CODE_FAILURE(  TEXT("Error changing tray icon tooltip"),
                                        GetLastError());
    }
}

void TTrayIcon::HideWindow(HWND hWnd, bool bSafe)
{
    if(bSafe && !IsWindow(hWnd))
        return;

    DEBUG_VERIFY(IsWindow(hWnd));

    ShowWindow(hWnd, SW_HIDE);
    ChangeKWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
}

void TTrayIcon::RestoreWindow(HWND hWnd, bool bSafe)
{
    if(bSafe && !IsWindow(hWnd))
        return;

    DEBUG_VERIFY(IsWindow(hWnd));

    if(!GetParent(hWnd))
        ChangeKWindowLong(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

    ShowWindow(hWnd, SW_RESTORE);

    SetForegroundWindow(hWnd);
}
