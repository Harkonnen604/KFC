#include "kfc_gui_pch.h"
#include "message_dialog.h"

#include "gui.h"

#include "resource.h"

// ---------------
// Message dialog
// ---------------
const UINT TMessageDialog::s_uiTerminateMsg = WM_USER + 0x10;

size_t TMessageDialog::s_szNMessageDialogsRunning = 0;

TMessageDialog::TMessageDialog(LPCTSTR pSMessage, bool bSOnlyEnlarge) :
	TDialog(false) ,
	m_bOnlyEnlarge(bSOnlyEnlarge)
{
	m_hStartEvent = NULL;

	m_OldSize.Set(0, 0);

	SetMessage(pSMessage, true);
}

HINSTANCE TMessageDialog::GetInstance() const
{
	return GetKModuleHandle();
}

int TMessageDialog::GetTemplateID() const
{
	return IDD_MESSAGE_DIALOG;
}

int TMessageDialog::DoModal(HWND hParentWnd, HANDLE hSStartEvent)
{
	DEBUG_VERIFY(hSStartEvent);

	m_hStartEvent = hSStartEvent;	

	return TDialog::DoModal(hParentWnd);
}

void TMessageDialog::Terminate(int iResult)
{
	PostMessage(*this, s_uiTerminateMsg, iResult, 0);
}

bool TMessageDialog::OnMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(TDialog::OnMessage(uiMsg, wParam, lParam))
		return true;

	if(uiMsg == WM_SETCURSOR)
	{
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		return true;
	}
	else if(uiMsg == s_uiTerminateMsg)
	{
		EndDialog(wParam);
		return true;
	}

	return false;
}

bool TMessageDialog::OnDestroy()
{
	s_szNMessageDialogsRunning--;

	return TDialog::OnDestroy();
}

bool TMessageDialog::OnInitDialog()
{
	if(TDialog::OnInitDialog())
		return true;

	s_szNMessageDialogsRunning++;

	SetSize();

	SetEvent(m_hStartEvent);

	return false;
}

bool TMessageDialog::OnPaint()
{
	if(TDialog::OnPaint())
		return true;

	TCriticalSectionLocker Locker0(m_MessageCriticalSection);

	TPaintDC DC(*this);

	TGDIObjectSelector	Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));
	TBkModeSelector		Selector1(DC, TRANSPARENT);
	TTextColorSelector	Selector2(DC, RGB(0, 0, 0));

	IRECT TextRect;
	FitRect(GetKClientRect(*this), ISIZE(m_MessageSize.cx, m_MessageSize.cy), TextRect);

	DrawText(DC, m_Message, m_Message.GetLength(), TextRect, DT_CENTER | DT_NOPREFIX);

	return true;
}

void TMessageDialog::SetSize()
{
	if(GetWndSafe() == NULL)
		return;

	const IRECT ScreenRect(	0,
							0,
							GetSystemMetrics(SM_CXSCREEN),
							GetSystemMetrics(SM_CYSCREEN));

	ISIZE Size(m_MessageSize.cx + 64, m_MessageSize.cy + 64);
	
	if(m_bOnlyEnlarge)
	{
		if(m_OldSize.cx > Size.cx)
			Size.cx = m_OldSize.cx;

		if(m_OldSize.cy > Size.cy)
			Size.cy = m_OldSize.cy;
	}

	IRECT WindowRect;
	FitRect(ScreenRect, Size, WindowRect);

	SetWindowPos(	GetWnd(),
					NULL,
					WindowRect.m_Left,
					WindowRect.m_Top,
					WindowRect.m_Right - WindowRect.m_Left,
					WindowRect.m_Bottom - WindowRect.m_Top,
					SWP_NOZORDER);

	InvalidateRect(*this, NULL, TRUE);

	m_OldSize = Size;
}

// Thread-safe message access
KString TMessageDialog::GetMessage() const
{
	TCriticalSectionLocker Locker0(m_MessageCriticalSection);

	return m_Message;
}

void TMessageDialog::SetMessage(LPCTSTR pSMessage, bool bForceRepaint)
{
	{
		TCriticalSectionLocker Locker0(m_MessageCriticalSection);

		if(!bForceRepaint && m_Message == pSMessage)
			return;

		{
			TDC DC((HWND)NULL);

			TGDIObjectSelector Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));

			m_MessageSize = GetMultiLineTextSize(DC, m_Message = pSMessage);
		}
	}

	if(GetWndSafe())
	{
		SetSize();

		InvalidateRect(*this, NULL, TRUE);
	}
}
