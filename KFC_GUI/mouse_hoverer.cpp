#include "kfc_gui_pch.h"
#include "mouse_hoverer.h"

#include "gui.h"

// --------------
// Mouse hoverer
// --------------
TMouseHoverer::TMouseHoverer()
{
	m_hWnd = NULL;
}

void TMouseHoverer::Release()
{
	m_hWnd = NULL;
}

int TMouseHoverer::Allocate(HWND hWnd)
{
	Release();

	try
	{
		DEBUG_VERIFY(hWnd);

		m_hWnd = hWnd;

		if(m_bHovered = KWindowFromPoint(GetKCursorPos()) == m_hWnd)
			TrackMouseLeave(m_hWnd);

		return m_bHovered;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

int TMouseHoverer::ProcessMessage(UINT uiMsg)
{
	if(!IsAllocated())
		return 0;

	switch(uiMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		{
			if(HitsRect(GetKCursorPos(), GetKWindowRect(m_hWnd)))
			{
				if(!m_bHovered)
				{
					m_bHovered = true;
					TrackMouseLeave(m_hWnd);
					return +1;
				}
			}
			else
			{
				if(m_bHovered)
				{
					m_bHovered = false;
					return -1;
				}
			}

			return 0;
		}
	
	case WM_MOUSELEAVE:
		if(m_bHovered)
		{
			m_bHovered = false;
			return -1;
		}

		return 0;

	default:
		return 0;
	}
}
