#include "kfc_gui_pch.h"
#include "toolbar.h"

#include "window_subclasser.h"

// --------
// Toolbar
// --------
TToolbar::TToolbar()
{
	m_hWnd = NULL;
}

void TToolbar::Release()
{
	if(m_hWnd)
	{
		UnsubclassWindow(GetParent(m_hWnd), *this, &TToolbar::ParentWindowProc);

		DestroyWindow(m_hWnd), m_hWnd = NULL;
	}

	m_Buttons.Clear();
}

void TToolbar::Create(	HWND			hParentWnd,
						DWORD			dwStyle,
						size_t			szButtonWidth,
						size_t			szButtonHeight,
						const TButtons&	Buttons,
						HINSTANCE		hBitmapInstance,
						int				iBitmapResID,
						size_t			szBitmapButtonWidth,
						size_t			szBitmapButtonHeight,
						size_t			szNBitmapButtons,
						int				iID)
{
	Release();

	try
	{
		DEBUG_VERIFY(!Buttons.IsEmpty());

		m_Buttons = Buttons;

		TArray<TBBUTTON> RealButtons;

		{
			size_t j = 0;

			FOR_EACH_ARRAY(Buttons, i)
			{
				TBBUTTON& RealButton = RealButtons.Add();

				memset(&RealButton, 0, sizeof(RealButton));

				if(Buttons[i].m_iID >= 0)
				{
					RealButton.iBitmap		= j++;
					RealButton.idCommand	= Buttons[i].m_iID;
					RealButton.fsState		= TBSTATE_ENABLED;
					RealButton.dwData		= i;
				}
				else
				{
					RealButton.fsStyle = TBSTYLE_SEP;
				}
			}
		}

		if(szNBitmapButtons == UINT_MAX)
			szNBitmapButtons = RealButtons.GetN();

		m_hWnd = CreateToolbarEx(	hParentWnd,
									WS_VISIBLE | WS_CHILD | dwStyle,
									iID,
									szNBitmapButtons,
									hBitmapInstance,
									iBitmapResID,
									RealButtons.GetDataPtr(),
									RealButtons.GetN(),
									szButtonWidth,
									szButtonHeight,
									szBitmapButtonWidth,
									szBitmapButtonHeight,
									sizeof(TBBUTTON));

		if(!m_hWnd)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating toolbar"),
											GetLastError());
		}

		SubclassWindow(GetParent(m_hWnd), *this, &TToolbar::ParentWindowProc);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TToolbar::EnableButton(int iID, bool bEnable)
{
	DEBUG_VERIFY_ALLOCATION;

	TBBUTTONINFO Info;
	memset(&Info, 0, sizeof(Info)), Info.cbSize = sizeof(Info);

	Info.dwMask = TBIF_STATE;

	DEBUG_EVALUATE_VERIFY(SendMessage(*this, TB_GETBUTTONINFO, iID, (LPARAM)&Info) >= 0);

	if(bEnable)
		Info.fsState |= TBSTATE_ENABLED;
	else
		Info.fsState &= ~TBSTATE_ENABLED;

	DEBUG_EVALUATE_VERIFY(SendMessage(*this, TB_SETBUTTONINFO, iID, (LPARAM)&Info));
}

void TToolbar::CheckButton(int iID, bool bCheck)
{
	DEBUG_VERIFY_ALLOCATION;

	TBBUTTONINFO Info;
	memset(&Info, 0, sizeof(Info)), Info.cbSize = sizeof(Info);

	Info.dwMask = TBIF_STATE;

	DEBUG_EVALUATE_VERIFY(SendMessage(*this, TB_GETBUTTONINFO, iID, (LPARAM)&Info) >= 0);

	if(bCheck)
		Info.fsState |= TBSTATE_CHECKED;
	else
		Info.fsState &= ~TBSTATE_CHECKED;

	DEBUG_EVALUATE_VERIFY(SendMessage(*this, TB_SETBUTTONINFO, iID, (LPARAM)&Info));
}

void TToolbar::Resize()
{
	DEBUG_VERIFY_ALLOCATION;

	SendMessage(*this, TB_AUTOSIZE, 0, 0);
}

LRESULT TToolbar::ParentWindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, WNDPROC pOldWindowProc)
{
	if(!IsAllocated())
		return CallWindowProc(pOldWindowProc, hWnd, uiMsg, wParam, lParam);

	if(uiMsg == WM_NOTIFY)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		
		if(pNMHDR->hwndFrom == *this && pNMHDR->code == TBN_GETINFOTIP)
		{
			NMTBGETINFOTIP* pInfo = (NMTBGETINFOTIP*)pNMHDR;

			size_t i = pInfo->lParam;

			if(i < m_Buttons.GetN() && !m_Buttons[i].m_Text.IsEmpty())
			{
				_tcsncpy(pInfo->pszText, m_Buttons[i].m_Text, pInfo->cchTextMax - 1);

				pInfo->pszText[pInfo->cchTextMax - 1] = 0;
			}

			return 0;
		}
	}

	return CallWindowProc(pOldWindowProc, hWnd, uiMsg, wParam, lParam);
}
