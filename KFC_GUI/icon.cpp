#include "kfc_gui_pch.h"
#include "icon.h"

// -----
// Icon
// -----
TIcon::TIcon()
{
	m_hIcon = NULL;
}

TIcon::TIcon(HICON hSIcon)
{
	m_hIcon = NULL;

	Allocate(hSIcon);
}

TIcon::TIcon(HINSTANCE hInstance, UINT uiID)
{
	m_hIcon = NULL;

	Allocate(hInstance, uiID);
}

TIcon::TIcon(UINT uiID)
{
	m_hIcon = NULL;

	Allocate(uiID);
}

void TIcon::Release()
{
	if(m_hIcon)
		DestroyIcon(m_hIcon), m_hIcon = NULL;
}

void TIcon::Allocate(HICON hSIcon)
{
	Release();

	try
	{
		DEBUG_VERIFY(hSIcon);

		m_hIcon = hSIcon;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

HICON TIcon::GetIcon() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_hIcon;
}
