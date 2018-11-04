#include "kfc_gui_pch.h"
#include "page_control.h"

// -------------
// Page control
// -------------
TPageControl::TPageControl()
{
	m_bAllocated = false;

	m_hWnd = NULL;
}

void TPageControl::Release(bool bFromAllocatorException)
{
	size_t i;

	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		if(IsWindow(m_hWnd))
			DestroyWindow(m_hWnd);

		for(i = 0 ; i < m_Pages.GetN() ; i--)
			m_Pages[i]->Release();

		m_Pages.Clear();

		m_hWnd = NULL;
	}
}

BOOL CALLBACK TPageControl::StaticEnumProc(HWND hWnd, LPARAM lParam)
{
	const int iID = GetWindowLong(hWnd, GWL_ID);

	if(iID == IDOK || iID == IDCANCEL)
		ShowWindow(hWnd, SW_HIDE);

	return TRUE;
}

void TPageControl::Allocate(TDialog&		ParentDialog,
							const IPOINT&	Coords,
							TDialog* const*	ppSPages,
							size_t			szN,
							size_t			szStartPage)
{
	size_t i;

	Release();

	try
	{
		// Local data
		DEBUG_VERIFY(IsWindow(ParentDialog));

		DEBUG_VERIFY(ppSPages);

		DEBUG_VERIFY(szN >= 1);		

		DEBUG_VERIFY(szStartPage < szN);
		
		for(i = 0 ; i < szN ; i++)
			m_Pages.Add() = ppSPages[i];

		TArray<HPROPSHEETPAGE> PageHandles;

		for(i = 0 ; i < m_Pages.GetN() ; i++)
			PageHandles.Add() = m_Pages[i]->CreateAsPropertySheetPage(ParentDialog);

		PROPSHEETHEADER Header;
		memset(&Header, 0, sizeof(Header));

		Header.dwSize		= sizeof(Header);
		Header.dwFlags		= PSH_MODELESS | PSH_NOAPPLYNOW | PSH_USECALLBACK;
		Header.hwndParent	= ParentDialog;
		Header.pszCaption	= TEXT("");
		Header.nPages		= m_Pages.GetN();
		Header.phpage		= PageHandles.GetDataPtr();
		Header.nStartPage	= szStartPage;
		Header.pfnCallback	= StaticPropertySheetCallback;

		const int r = PropertySheet(&Header);
		if(r <= 0)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating property sheet."),
											GetLastError());
		}

		m_hWnd = (HWND)r;

		// Removing OK/Cancel buttons
		EnumChildWindows(m_hWnd, StaticEnumProc, 0);

		// Setting position
		SetWindowPos(	m_hWnd,
						NULL,
						Coords.x,
						Coords.y,
						0,
						0,
						SWP_NOZORDER | SWP_NOSIZE);		

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

int CALLBACK TPageControl::StaticPropertySheetCallback(HWND hWnd, UINT uiMsg, LPARAM lParam)
{
	if(uiMsg == PSCB_PRECREATE)
	{
		if(lParam)
		{
			DWORD* pStyle;
			DWORD* pExStyle;

			LPDLGTEMPLATEEX pEx = (LPDLGTEMPLATEEX)lParam;
			LPDLGTEMPLATE p = (LPDLGTEMPLATE)lParam;

			if(((LPDLGTEMPLATEEX)lParam)->signature == 0xFFFF) // DLGTEMPLATEEX
			{
				pStyle		= &((LPDLGTEMPLATEEX)lParam)->style;
				pExStyle	= &((LPDLGTEMPLATEEX)lParam)->exStyle;
			}
			else // DLGTEMPLATE
			{
				pStyle		= &((LPDLGTEMPLATE)lParam)->style;
				pExStyle	= &((LPDLGTEMPLATE)lParam)->dwExtendedStyle;
			}

			// Removing help button
			*pStyle &= ~(	WS_OVERLAPPED	|
							WS_POPUP		|
							WS_BORDER		|
							WS_CAPTION		|
							DS_CONTEXTHELP	|
							DS_MODALFRAME);

			*pStyle |=	WS_CHILD		|
						WS_VISIBLE		|
						WS_CLIPSIBLINGS	|
						WS_TABSTOP		|
						DS_CONTROL;
		}
	}

	return 0;
}

void TPageControl::SetActivePage(size_t szIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	PropSheet_SetCurSel(*this, NULL, szIndex);
}

// Getters
size_t TPageControl::GetN() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_Pages.GetN();
}

HWND TPageControl::GetWnd() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_hWnd;
}