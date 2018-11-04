#include "kfc_gui_pch.h"
#include "message_box.h"

#include "gui.h"

#include "resource.h"

// Sizes
#define MBS_SPACING			(8)
#define MBS_ICON_SPACING	(16)
#define MBS_BUTTON_SPACING	(16)
#define MBS_ICON_CX			(32)
#define MBS_ICON_CY			(32)
#define MBS_BUTTON_CX		(80)
#define MBS_BUTTON_CY		(24)
#define MBS_TIMEOUT_CY		(16)

// Strings
#define MBS_TIMEOUT_MESSAGE			TEXT("This window will close in %u second%s...")
#define MBS_MAX_TIMEOUT_MESSAGE		TEXT("This window will close in 9999 seconds...")

// Timer IDs
#define MBS_TIMEOUT_TIMER_ID	(1)

// ------------
// Message box
// ------------
TMessageBox::TMessageBox(	LPCTSTR pSTitle,
							LPCTSTR	pSText,							
							HICON	hSIcon,
							int		iSFocusButtonID,
							int		iSTimeoutButtonID,
							size_t	szSTimeout) : TDialog(true)
{
	DEBUG_VERIFY(pSTitle);
	m_Title = pSTitle;

	DEBUG_VERIFY(pSText);
	m_Text = pSText;

	DEBUG_VERIFY(hSIcon);
	m_hIcon = hSIcon;

	DEBUG_VERIFY(iSFocusButtonID == IDYES || iSTimeoutButtonID == IDNO);
	m_iFocusButtonID = iSFocusButtonID;

	DEBUG_VERIFY(iSTimeoutButtonID == IDYES || iSTimeoutButtonID == IDNO);
	m_iTimeoutButtonID = iSTimeoutButtonID;

	DEBUG_VERIFY(szSTimeout > 0);
	m_szTimeout = szSTimeout;

	m_bTimedOut = false;
}

HINSTANCE TMessageBox::GetInstance() const
{
	return GetKModuleHandle();
}

int TMessageBox::GetTemplateID() const
{
	return IDD_YES_NO_MESSAGE_BOX_DIALOG;
}

bool TMessageBox::OnInitDialog()
{
	if(TDialog::OnInitDialog())
		return true;

	// Title
	SetKWindowText(*this, m_Title);

	// Sizing
	{
		ISIZE TimeoutSize;

		{
			TDC DC(*this);

			TGDIObjectSelector Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));

			m_TextSize = TO_I(GetMultiLineTextSize(DC, m_Text));

			TimeoutSize = TO_I(GetTextSize(DC, MBS_MAX_TIMEOUT_MESSAGE));
		}

		m_IconTextSize.Set(	MBS_ICON_CX + MBS_ICON_SPACING + m_TextSize.cx,
							Max(MBS_ICON_CY, m_TextSize.cy));

		m_DialogSize.Set(	MBS_ICON_SPACING +
								Max(m_IconTextSize.cx,
									MBS_BUTTON_CX + MBS_BUTTON_SPACING + MBS_BUTTON_CX,
									TimeoutSize.cx) +
								MBS_ICON_SPACING,
							MBS_ICON_SPACING + m_IconTextSize.cy + MBS_ICON_SPACING + MBS_BUTTON_CY + MBS_SPACING + MBS_TIMEOUT_CY + MBS_SPACING);

		m_DialogScreenSize =
			m_DialogSize + ISIZE(GetKScreenRect(*this)) - ISIZE(GetKClientRect(*this));

		// Dialog
		SetWindowPos(	*this,
						NULL,
						0,
						0,
						m_DialogScreenSize.cx,
						m_DialogScreenSize.cy,
						SWP_NOZORDER | SWP_NOMOVE);

		// Yes button
		SetWindowPos(	GetDlgItem(IDOK),
						NULL,
						(m_DialogSize.cx - MBS_BUTTON_SPACING) / 2 - MBS_BUTTON_CX,
						m_DialogSize.cy - (MBS_BUTTON_CY + MBS_SPACING + MBS_TIMEOUT_CY + MBS_SPACING),
						MBS_BUTTON_CX,
						MBS_BUTTON_CY,
						SWP_NOZORDER);

		// No button
		SetWindowPos(	GetDlgItem(IDCANCEL),
						NULL,
						(m_DialogSize.cx + MBS_BUTTON_SPACING) / 2,
						m_DialogSize.cy - (MBS_BUTTON_CY + MBS_SPACING + MBS_TIMEOUT_CY + MBS_SPACING),
						MBS_BUTTON_CX,
						MBS_BUTTON_CY,
						SWP_NOZORDER);
	}

	// Timer
	{
		if(m_szTimeout)
			m_TimeoutTimer.Allocate(*this, 1000, MBS_TIMEOUT_TIMER_ID);
	}

	// Focus
	{
		int iRealFocusID;
		
		switch(m_iFocusButtonID)
		{
		case IDYES:
			iRealFocusID = IDOK;
			break;

		case IDNO:
			iRealFocusID = IDCANCEL;
			break;

		default:
			INITIATE_FAILURE;
		}

		SetFocus(GetDlgItem(iRealFocusID));
	}

	return false;
}

bool TMessageBox::OnPaint()
{
	if(TDialog::OnPaint())
		return true;

	TPaintDC DC(*this);

	TGDIObjectSelector	Selector0(DC, GetStockObject(DEFAULT_GUI_FONT));
	TBkModeSelector		Selector1(DC, TRANSPARENT);
	TTextColorSelector	Selector2(DC, GetSysColor(COLOR_WINDOWTEXT));

	// Text
	{
		IRECT TextRect;
		TextRect.m_Left		= MBS_ICON_SPACING + MBS_ICON_CX + MBS_ICON_SPACING;
		TextRect.m_Top		= MBS_ICON_SPACING;
		TextRect.m_Right	= TextRect.m_Left	+ m_TextSize.cx;
		TextRect.m_Bottom	= TextRect.m_Top	+ m_IconTextSize.cy;

		FitRect(IRECT(TextRect),
				m_TextSize,
				TextRect);

		DrawText(DC, m_Text, -1, TextRect, DT_NOPREFIX);
	}

	// Icon
	{
		IRECT IconRect;
		IconRect.m_Left		= MBS_ICON_SPACING;
		IconRect.m_Top		= MBS_ICON_SPACING;
		IconRect.m_Right	= IconRect.m_Left	+ MBS_ICON_CX;
		IconRect.m_Bottom	= IconRect.m_Top	+ m_IconTextSize.cy;

		FitRect(IRECT(IconRect),
				ISIZE(MBS_ICON_CX, MBS_ICON_CY),
				IconRect);

		DrawIcon(DC, IconRect.m_Left, IconRect.m_Top, m_hIcon);
	}

	// Timeout
	{
		IRECT TimeoutRect;
		TimeoutRect.m_Left		= MBS_SPACING;
		TimeoutRect.m_Top		= m_DialogSize.cy - MBS_SPACING - MBS_TIMEOUT_CY;
		TimeoutRect.m_Right		= m_DialogSize.cx - MBS_SPACING;
		TimeoutRect.m_Bottom	= m_DialogSize.cy - MBS_SPACING;

		DrawText(	DC,
					KString::Formatted(	MBS_TIMEOUT_MESSAGE,
											m_szTimeout,
											m_szTimeout == 1 ? "" : "s"),
					-1,
					TimeoutRect,
					DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
	}

	return true;
}

bool TMessageBox::OnTimer(size_t szID)
{
	if(TDialog::OnTimer(szID))
		return true;

	switch(szID)
	{
	case MBS_TIMEOUT_TIMER_ID:
		if(m_szTimeout == 0 || --m_szTimeout == 0)
		{
			m_bTimedOut = true;

			EndDialog(m_iTimeoutButtonID);

			return true;
		}

		InvalidateRect(*this, NULL, TRUE);

		return true;
	}

	return false;
}

void TMessageBox::OnOK()
{
	EndDialog(IDYES);
}

void TMessageBox::OnCancel()
{
	EndDialog(IDNO);
}