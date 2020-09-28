#include "kfc_gui_pch.h"
#include "gui_back_buffer.h"

// ----------------
// GUI back buffer
// ----------------
TGUIBackBuffer::TGUIBackBuffer()
{
    m_bAllocated = false;

    m_hOldBitmap = NULL;
}

void TGUIBackBuffer::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        if(m_hOldBitmap)
            SelectObject(m_DC, m_hOldBitmap), m_hOldBitmap = NULL;

        m_Bitmap.Release();

        m_DC.Release();
    }
}

void TGUIBackBuffer::Allocate()
{
    Release();

    try
    {
        TDC ScreenDC((HWND)NULL);

        m_DC.Allocate(CreateCompatibleDC(ScreenDC));

        m_LastSize.Set( GetSystemMetrics(SM_CXSCREEN),
                        GetSystemMetrics(SM_CYSCREEN));

        m_Bitmap.Allocate(ScreenDC, m_LastSize);

        m_hOldBitmap = (HBITMAP)SelectObject(m_DC, m_Bitmap);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

TDC& TGUIBackBuffer::GetDC()
{
    DEBUG_VERIFY_ALLOCATION;

    if( m_LastSize.cx != (size_t)GetSystemMetrics(SM_CXSCREEN) ||
        m_LastSize.cy != (size_t)GetSystemMetrics(SM_CYSCREEN))
    {
        Allocate();
    }

    return m_DC;
}
