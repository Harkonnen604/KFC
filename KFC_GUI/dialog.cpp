#include "kfc_gui_pch.h"
#include "dialog.h"

#include "gui.h"
#include "edit_filter.h"

// -------
// Dialog
// -------
TDialog::TDialog(bool bInputDialog)
{
	m_hWnd = NULL;

	m_hPropertySheetPage = NULL;

	m_Mode = MD_NONE;

	m_iButtonOKID		= -1;
	m_iButtonCancelID	= -1;

	m_bAccelerateOK		= bInputDialog;
	m_bAccelerateCancel	= bInputDialog;
	m_bCancelOnClose	= true;	

	m_bDestroying = false;
}

void TDialog::Release()
{
	switch(m_Mode)
	{
	case MD_MODAL:
		if(IsWindow(m_hWnd))
			::EndDialog(m_hWnd, IDABORT);

		break;

	case MD_NONMODAL:
		if(IsWindow(m_hWnd))
			::DestroyWindow(m_hWnd);

		break;

	case MD_PROPERTY_SHEET_PAGE:
		break;
	}

	m_hWnd = NULL;

	m_hPropertySheetPage = NULL;

	m_Mode = MD_NONE;
}

BOOL CALLBACK TDialog::StaticDialogProc(HWND	hWnd,
										UINT	uiMsg,
										WPARAM	wParam,
										LPARAM	lParam)
{
	BOOL bResult = FALSE;

	if(uiMsg == WM_INITDIALOG)
		SetWindowLong(hWnd, GWL_USERDATA, lParam);

	TDialog* const pDialog = (TDialog*)GetWindowLong(hWnd, GWL_USERDATA);	

	if(pDialog)
	{
		if(uiMsg == WM_INITDIALOG)
			pDialog->m_hWnd = hWnd;

		bResult = pDialog->DialogProc(hWnd, uiMsg, wParam, lParam);	

		if(uiMsg == WM_DESTROY)
			pDialog->m_hWnd = NULL;
	}

	if(uiMsg == WM_DESTROY)
		SetWindowLong(hWnd, GWL_USERDATA, 0);

	return bResult;
}

BOOL CALLBACK TDialog::StaticPropertySheetPageProc(	HWND	hWnd,
													UINT	uiMsg,
													WPARAM	wParam,
													LPARAM	lParam)
{
	if(uiMsg == WM_INITDIALOG)
	{
		if(lParam)
			SetWindowLong(hWnd, GWL_USERDATA, ((LPCPROPSHEETPAGE)lParam)->lParam);
	}
	else if(uiMsg == WM_DESTROY)
	{
		SetWindowLong(hWnd, GWL_USERDATA, 0);
	}

	TDialog* const pDialog = (TDialog*)GetWindowLong(hWnd, GWL_USERDATA);

	if(pDialog)
	{
		if(uiMsg == WM_INITDIALOG)
			pDialog->m_hWnd = hWnd;

		return pDialog->DialogProc(hWnd, uiMsg, wParam, lParam);

		if(uiMsg == WM_DESTROY)
			pDialog->m_hWnd = NULL;
	}

	return FALSE;
}

BOOL TDialog::DialogProc(	HWND	hWnd,
							UINT	uiMsg,
							WPARAM	wParam,
							LPARAM	lParam)
{
	if(hWnd != m_hWnd)
		return FALSE;

	m_bUseForcedResult = false;
	
	const bool bRes = OnMessage(uiMsg, wParam, lParam);

	return m_bUseForcedResult ? m_bForcedResult : (bRes ? TRUE : FALSE);
}

void TDialog::EndDialog(int iResult)
{
	if(IsWindow(m_hWnd))
	{
		switch(m_Mode)
		{
		case MD_MODAL:
			::EndDialog(m_hWnd, iResult), m_Mode = MD_NONE;
			break;

		case MD_PROPERTY_SHEET_PAGE:
			m_pOwnerDialog->EndDialog(iResult);
			break;
		}
	}
}

int TDialog::DoModal(HWND hParentWnd)
{
	Release();

	try
	{
		m_Mode = MD_MODAL;

		const int iResult = DialogBoxParam(	GetInstance(),
											MAKEINTRESOURCE(GetTemplateID()),
											hParentWnd,
											GetStaticDialogProc(),
											(LPARAM)this);

		m_hWnd = NULL;

		return iResult;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TDialog::Destroy()
{
	if(IsWindow(m_hWnd))
	{
		switch(m_Mode)
		{
		case MD_NONMODAL:
			::DestroyWindow(m_hWnd), m_hWnd = NULL, m_Mode == MD_NONE;
			break;

		case MD_PROPERTY_SHEET_PAGE:
			m_pOwnerDialog->Destroy();
			break;
		}
	}		
}

TDialog& TDialog::Create(HWND hParentWnd)
{
	Release();

	m_Mode = MD_NONMODAL;

	try
	{
		m_hWnd = CreateDialogParam(	GetInstance(),
									MAKEINTRESOURCE(GetTemplateID()),
									hParentWnd,
									GetStaticDialogProc(),
									(LPARAM)this);

		if(m_hWnd == NULL)
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating non-modal dialog"), GetLastError());

		return *this;
	}
	catch(...)
	{
		Release();
		throw;
	}
}

TDialog& TDialog::CreateAsPropertySheetPage(TDialog& SOwnerDialog)
{
	Release();

	m_Mode = MD_PROPERTY_SHEET_PAGE;

	try
	{
		m_bAccelerateOK		= false;
		m_bAccelerateCancel	= false;
		m_bCancelOnClose	= false;

		m_pOwnerDialog = &SOwnerDialog;

		PROPSHEETPAGE Page;
		memset(&Page, 0, sizeof(Page));

		Page.dwSize			= sizeof(Page);
		Page.dwFlags		= PSP_DEFAULT;
		Page.hInstance		= GetInstance();
		Page.pszTemplate	= MAKEINTRESOURCE(GetTemplateID());
		Page.pfnDlgProc		= GetStaticPropertySheetPageProc();
		Page.lParam			= (LPARAM)this;

		m_hPropertySheetPage = CreatePropertySheetPage(&Page);

		if(m_hPropertySheetPage == NULL)
			INITIATE_DEFINED_FAILURE(TEXT("Error creating property sheet page."));
		
		return *this;
	}
	catch(...)
	{
		Release();
		throw;
	}
}

HWND TDialog::GetDlgItemSafe(int iID) const
{
	DEBUG_VERIFY_ALLOCATION;

	return ::GetDlgItem(*this, iID);
}

HWND TDialog::GetDlgItem(int iID) const
{
	DEBUG_VERIFY_ALLOCATION;

	const HWND hWnd = GetDlgItemSafe(iID);

	DEBUG_VERIFY(hWnd);

	return hWnd;
}

KString TDialog::GetDlgItemTextSafe(int iID) const
{
	DEBUG_VERIFY_ALLOCATION;

	return GetKWindowTextSafe(GetDlgItemSafe(iID));
}

KString TDialog::GetDlgItemText(int iID) const
{
	DEBUG_VERIFY_ALLOCATION;

	return GetKWindowText(GetDlgItem(iID));
}

void TDialog::SetDlgItemTextSafe(int iID, const KString& Text)
{
	DEBUG_VERIFY_ALLOCATION;

	SetKWindowTextSafe(GetDlgItemSafe(iID), Text);
}

void TDialog::SetDlgItemText(int iID, const KString& Text)
{
	DEBUG_VERIFY_ALLOCATION;

	SetWindowText(GetDlgItem(iID), Text);
}

bool TDialog::SetDlgItemTextNoFlickerSafe(int iID, const KString& Text)
{
	DEBUG_VERIFY_ALLOCATION;

	if(GetDlgItemTextSafe(iID) == Text)
		return false;
	
	SetDlgItemTextSafe(iID, Text);

	return true;
}

bool TDialog::SetDlgItemTextNoFlicker(int iID, const KString& Text)
{
	DEBUG_VERIFY_ALLOCATION;

	if(GetDlgItemText(iID) == Text)
		return false;

	SetDlgItemText(iID, Text);

	return true;
}

void TDialog::PostDlgItemMessage(int iID, UINT uiMsg, WPARAM wParam, LPARAM lParam) const
{
	DEBUG_VERIFY_ALLOCATION;

	PostMessage(GetDlgItem(iID), uiMsg, wParam, lParam);
}

LRESULT TDialog::SendDlgItemMessage(int iID, UINT uiMsg, WPARAM wParam, LPARAM lParam) const
{
	DEBUG_VERIFY_ALLOCATION;

	return ::SendDlgItemMessage(*this, iID, uiMsg, wParam, lParam);
}

HWND TDialog::SetDlgItemFocus(int iID)
{
	DEBUG_VERIFY_ALLOCATION;

	return SetFocus(iID == -1 ? NULL : GetDlgItem(iID));
}

bool TDialog::IsDlgButtonChecked(int iID) const
{
	DEBUG_VERIFY_ALLOCATION;

	return ::IsDlgButtonChecked(*this, iID) == BST_CHECKED;
}

void TDialog::CheckDlgButton(int iID, bool bCheck)
{
	DEBUG_VERIFY_ALLOCATION;

	::CheckDlgButton(*this, iID, bCheck ? BST_CHECKED : BST_UNCHECKED);
}

bool TDialog::IsDlgItemVisible(int iID) const
{
	DEBUG_VERIFY_ALLOCATION;

	return IsWindowVisible(GetDlgItem(iID)) ? true : false;
}

void TDialog::ShowDlgItem(int iID, bool bShow)
{
	DEBUG_VERIFY_ALLOCATION;

	ShowWindow(GetDlgItem(iID), bShow ? SW_SHOW : SW_HIDE);
}

bool TDialog::IsDlgItemEnabled(int iID) const
{
	DEBUG_VERIFY_ALLOCATION;

	return IsWindowEnabled(GetDlgItem(iID)) ? true : false;
}

void TDialog::EnableDlgItem(int iID, bool bEnable)
{
	DEBUG_VERIFY_ALLOCATION;

	EnableWindow(GetDlgItem(iID), bEnable ? TRUE : FALSE);
}

void TDialog::FilterEditControl(int iID,  LPCTSTR pAllow, LPCTSTR pDeny)
{
	DEBUG_VERIFY_ALLOCATION;

	::FilterEditControl(GetDlgItem(iID), pAllow, pDeny);
}

int TDialog::MessageBox(LPCTSTR pCaption, LPCTSTR pText, kflags_t flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	return ::MessageBox(*this, pText, pCaption, flFlags);
}

void TDialog::InvalidateWindow()
{
	DEBUG_VERIFY_ALLOCATION;

	::InvalidateRect(*this, NULL, TRUE);
}

void TDialog::UpdateWindow()
{
	DEBUG_VERIFY_ALLOCATION;

	::UpdateWindow(*this);
}

// Message handlers
bool TDialog::OnMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT NotifyResult;

	switch(uiMsg)
	{
	case WM_DESTROY:
		return OnDestroy();

	case WM_INITDIALOG:
		return OnInitDialog();

	case WM_CLOSE:
		return OnClose();

	case WM_COMMAND:
		return OnCommand(LOWORD(wParam), (HWND)lParam, HIWORD(wParam));

	case WM_MEASUREITEM:
		return OnMeasureItem(wParam, (LPMEASUREITEMSTRUCT)lParam);

	case WM_DRAWITEM:
		return OnDrawItem(wParam, (LPDRAWITEMSTRUCT)lParam);

	case WM_NOTIFY:
		NotifyResult = 0;

		if(OnNotify(wParam, (const TNotification*)lParam, NotifyResult))
		{
			SetWindowLong(*this, DWL_MSGRESULT, NotifyResult);
			return true;
		}

		return false;

	case WM_ERASEBKGND:
		return OnEraseBKGND();

	case WM_PAINT:
		return OnPaint();

	case WM_TIMER:
		return OnTimer(wParam);

	case WM_SIZE:
		return OnSize(wParam);
	}

	return false;
}

bool TDialog::OnDestroy()
{
	m_bDestroying = true;

	return false;
}

bool TDialog::OnInitDialog()
{
	ShowWindow(*this, SW_SHOW);

	return false;
}

bool TDialog::OnCommand(int iID, HWND hWnd, int iCode)
{
	if(m_iButtonOKID >= 0 && iID == m_iButtonOKID)
	{
		OnOK();
		return true;
	}

	if(m_iButtonCancelID >= 0 && iID == m_iButtonCancelID)
	{
		OnCancel();
		return true;
	}

	switch(iID)
	{
	case IDOK:
		if(m_bAccelerateOK)
		{
			OnOK();
			return true;
		}

		break;

	case IDCANCEL:
		if(m_bAccelerateCancel)
		{
			OnCancel();
			return true;
		}

		break;
	}

	return false;
}

bool TDialog::OnClose()
{
	if(m_bCancelOnClose)
	{
		OnCancel();
		return true;
	}

	return false;
}

// Special handlers
void TDialog::OnOK()
{
	switch(GetMode())
	{
		case MD_MODAL:
			EndDialog(IDOK);
			break;

		case MD_NONMODAL:
			Destroy();
			break;

		case MD_PROPERTY_SHEET_PAGE:
			break;
	}
}

void TDialog::OnCancel()
{
	switch(GetMode())
	{
		case MD_MODAL:
			EndDialog(IDCANCEL);
			break;

		case MD_NONMODAL:
			Destroy();
			break;

		case MD_PROPERTY_SHEET_PAGE:
			break;
	}
}

void TDialog::SetIcon(HICON hIcon, bool bBig)
{
	DEBUG_VERIFY_ALLOCATION;

	SendMessage(m_hWnd, WM_SETICON, bBig ? ICON_BIG : ICON_SMALL, (LPARAM)hIcon);
}

// Getters
HWND TDialog::GetWnd() const
{
	DEBUG_VERIFY_ALLOCATION; // blocking pre-WM_INIT_DIAOG calls

	return m_hWnd;
}

HPROPSHEETPAGE TDialog::GetPropertySheetPage() const
{
	DEBUG_VERIFY(m_hPropertySheetPage); // blocking pre-WM_INIT_DIAOG calls

	return m_hPropertySheetPage;
}

// ----------------------
// Dialog data exception
// ----------------------
bool TDialogDataException::Apply(TDialog& Dialog, kflags_t flMessageBoxFlags, int iFocusRetCode)
{
	const int iRet =
		Dialog.MessageBox(	TEXT("Error"),
							m_ErrorText,
							flMessageBoxFlags);

	if(iRet == iFocusRetCode)
	{
		Dialog.SetDlgItemFocus(m_iFocusID);
		return true;
	}

	return false;
}
