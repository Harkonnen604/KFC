#include "kfc_gui_pch.h"
#include "status_bar.h"

#include "gui.h"

// -----------
// Status bar
// -----------
TStatusBar::TStatusBar()
{
	m_hWnd = NULL;
}

void TStatusBar::Release()
{
	if(m_hWnd)
		DestroyWindow(m_hWnd), m_hWnd = NULL;
}

void TStatusBar::Create(HWND hParentWnd, DWORD dwStyle)
{
	try
	{
		m_hWnd = CreateWindow(	STATUSCLASSNAME,
								TEXT(""),
								WS_CHILD | WS_VISIBLE | dwStyle,
								0, 0,
								16, 16,
								hParentWnd,
								NULL,
								GetModuleHandle(NULL),
								NULL);

		if(!m_hWnd)
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating status bar"), GetLastError());
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TStatusBar::SetParts(const int* pWidths, size_t szN)
{
	DEBUG_VERIFY_ALLOCATION;

	SendMessage(*this, SB_SETPARTS, szN, (LPARAM)pWidths);
}

void TStatusBar::Resize()
{
	DEBUG_VERIFY_ALLOCATION;

	SendMessage(*this, WM_SIZE, 0, 0);
}

KString TStatusBar::GetText() const
{
	DEBUG_VERIFY_ALLOCATION;

	return GetKWindowText(*this);
}

void TStatusBar::SetText(LPCTSTR pText)
{
	DEBUG_VERIFY_ALLOCATION;

	SetKWindowText(*this, pText);
}

void TStatusBar::SetText(size_t i, LPCTSTR pText)
{
	DEBUG_VERIFY_ALLOCATION;

	SendMessage(*this, SB_SETTEXT, i, (LPARAM)pText);
}
