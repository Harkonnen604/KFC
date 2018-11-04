#include "kfc_gui_pch.h"
#include "menu.h"

// -----
// Menu
// -----
TMenu::TMenu()
{
	m_hMenu = NULL;

	m_bSubMenu = false;
}

TMenu::TMenu(HINSTANCE hInstance, LPCTSTR pName)
{
	m_hMenu = NULL;

	m_bSubMenu = false;

	Allocate(hInstance, pName);
}

TMenu::TMenu(int iID)
{
	m_hMenu = NULL;

	m_bSubMenu = false;

	Allocate(iID);
}

TMenu::TMenu(HMENU hSMenu)
{
	m_hMenu = NULL;

	Allocate(hSMenu);
}

void TMenu::Release()
{
	if(!m_bSubMenu)
	{
		if(m_hMenu)
			DestroyMenu(m_hMenu), m_hMenu = NULL;
	}

	m_hMenu = NULL;

	m_bSubMenu = false;
}

void TMenu::Allocate(HINSTANCE hInstance, LPCTSTR pName)
{
	Release();

	try
	{
		DEBUG_EVALUATE_VERIFY(m_hMenu = LoadMenu(hInstance, pName));

		m_bSubMenu = false;
	}
	
	catch(...)
	{
		Release();
		throw;
	}
}

void TMenu::Allocate(HMENU hSMenu)
{
	Release();

	try
	{
		DEBUG_VERIFY(IsMenu(hSMenu));

		m_hMenu = hSMenu;

		m_bSubMenu = true;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

HMENU TMenu::GetSubMenu(size_t szPos) const
{
	DEBUG_VERIFY_ALLOCATION;

	const HMENU hMenu = ::GetSubMenu(*this, szPos);

	DEBUG_VERIFY(hMenu);

	return hMenu;
}

void TMenu::EnableItem(int iID, bool bEnable)
{
	DEBUG_VERIFY_ALLOCATION;

	EnableMenuItem(m_hMenu, iID, bEnable ? MF_ENABLED : MF_GRAYED);
}

void TMenu::CheckItem(int iID, bool bCheck)
{
	DEBUG_VERIFY_ALLOCATION;

	CheckMenuItem(m_hMenu, iID, bCheck ? MF_CHECKED : MF_UNCHECKED);
}

void TMenu::TrackPopup(HWND hWnd, const IPOINT& Coords, const ISIZE& Offset, kflags_t flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	::TrackPopupMenu(*this, flFlags, Coords.x + Offset.cx, Coords.y + Offset.cy, 0, hWnd, NULL);
}

HMENU TMenu::GetMenu() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_hMenu;
}

// --------
// Submenu
// --------
TSubMenu::TSubMenu()
{
}

TSubMenu::TSubMenu(HINSTANCE hInstance, LPCTSTR pName, size_t szPos)
{
	Allocate(hInstance, pName, szPos);
}

TSubMenu::TSubMenu(int iID, size_t szPos)
{
	Allocate(iID, szPos);
}

void TSubMenu::Release()
{
	TMenu::Release();

	m_Menu.Release();
}

void TSubMenu::Allocate(HINSTANCE hInstance, LPCTSTR pName, size_t szPos)
{
	Release();

	try
	{
		m_Menu.Allocate(hInstance, pName);

		TMenu::Allocate(m_Menu.GetSubMenu(szPos));
	}

	catch(...)
	{
		Release();
		throw;
	}
}
