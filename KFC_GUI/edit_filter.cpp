#include "kfc_gui_pch.h"
#include "edit_filter.h"

#include <KFC_Common\clipboard.h>
#include "window_subclasser.h"

TEditFilterGlobals g_EditFilterGlobals;

// --------------------
// Edit filter globals
// --------------------
TEditFilterGlobals::TEditFilterGlobals() : TGlobals(TEXT("Edit filter globals"))
{
	AddSubGlobals(g_WindowSubclasserGlobals);

	m_aAllowPropName = 0;
	m_aDenyPropName  = 0;
}

void TEditFilterGlobals::OnUninitialize()
{
	if(m_aDenyPropName)
		DeleteAtom(m_aDenyPropName), m_aDenyPropName = 0;

	if(m_aAllowPropName)
		DeleteAtom(m_aAllowPropName), m_aAllowPropName = 0;
}

void TEditFilterGlobals::OnInitialize()
{
	DEBUG_EVALUATE_VERIFY(m_aAllowPropName = AddAtom(TEXT("KFC_EditFilter_Allow")));
	DEBUG_EVALUATE_VERIFY(m_aDenyPropName  = AddAtom(TEXT("KFC_EditFilter_Deny")));
}

// ----------------
// Helper routines
// ----------------
static void FreeData(HWND hWnd)
{
	LPTSTR pAllow = (LPTSTR)GetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aAllowPropName);
	LPTSTR pDeny  = (LPTSTR)GetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aDenyPropName);

	SetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aAllowPropName, NULL);
	SetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aDenyPropName,  NULL);

	kfc_free(pAllow), kfc_free(pDeny);
}

// ------------------------
// Edit filter window proc
// ------------------------
static LRESULT EditFilterWindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, WNDPROC pOldWndProc)
{
	if(uiMsg == WM_NCDESTROY)
	{
		FreeData(hWnd);

		return CallWindowProc(pOldWndProc, hWnd, uiMsg, wParam, lParam);
	}

	if(uiMsg == WM_CHAR)
	{
		TCHAR cChar = (TCHAR)wParam;

		if(/*!(GetKeyState(VK_CONTROL)	& 0x8000) &&
			!(GetKeyState(VK_MENU)		& 0x8000) &&*/
			cChar != VK_BACK)
		{
			LPTSTR pAllow = (LPTSTR)GetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aAllowPropName);
			LPTSTR pDeny  = (LPTSTR)GetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aDenyPropName);

			if(	pAllow && !_tcschr(pAllow, cChar) ||
				pDeny  &&  _tcschr(pDeny,  cChar))
			{
				return 0;
			}
		}
	}
	else if(uiMsg == WM_PASTE)
	{
		LPTSTR pAllow = (LPTSTR)GetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aAllowPropName);
		LPTSTR pDeny  = (LPTSTR)GetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aDenyPropName);

		KString Text;

		if(	GetClipboardText(hWnd, Text) &&
			(	pAllow && _tcsspn (Text, pAllow) < Text.GetLength() ||
				pDeny  && _tcspbrk(Text, pDeny)))
		{
			return 0;
		}
	}
	else if(uiMsg == WM_SETTEXT)
	{
		LPTSTR pAllow = (LPTSTR)GetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aAllowPropName);
		LPTSTR pDeny  = (LPTSTR)GetProp(hWnd, (LPCTSTR)g_EditFilterGlobals.m_aDenyPropName);

		LPCTSTR pText = (LPCTSTR)lParam;

		if(	pAllow && pText[_tcsspn(pText, pAllow)] ||
			pDeny  && _tcspbrk(pText, pDeny))
		{
			return FALSE;
		}
	}

	return CallWindowProc(pOldWndProc, hWnd, uiMsg, wParam, lParam);
}

// ----------------
// Global routines
// ----------------
void FilterEditControl(HWND hWnd, LPCTSTR pAllow, LPCTSTR pDeny)
{
	FreeData(hWnd);

	if(pAllow || pDeny)
	{
		SubclassWindow(hWnd, EditFilterWindowProc, true);

		if(pAllow)
		{
			SetProp(hWnd,
					(LPCTSTR)g_EditFilterGlobals.m_aAllowPropName,
					(HANDLE)_tcsdup(pAllow));
		}

		if(pDeny)
		{
			SetProp(hWnd,
					(LPCTSTR)g_EditFilterGlobals.m_aDenyPropName,
					(HANDLE)_tcsdup(pDeny));
		}
	}
	else
	{
		UnsubclassWindow(hWnd, EditFilterWindowProc);
	}
}
