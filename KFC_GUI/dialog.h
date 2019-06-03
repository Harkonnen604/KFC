#ifndef dialog_h
#define dialog_h

#include <KFC_Common/module.h>
#include "notification.h"
#include "gui.h"

// -------
// Dialog
// -------
class TDialog
{
public:
	// Mode
	enum TMode
	{
		MD_NONE					= 0x00,
		MD_MODAL				= 0x01,
		MD_NONMODAL				= 0x02,
		MD_PROPERTY_SHEET_PAGE	= 0x03,
		MD_FORCE_UINT			= UINT_MAX,
	};

private:
	HWND m_hWnd;

	HPROPSHEETPAGE m_hPropertySheetPage;

	TMode m_Mode;

	TDialog* m_pOwnerDialog; // master dialog for property sheet pages

	BOOL m_bForcedResult;
	bool m_bUseForcedResult;

	bool m_bDestroying;

protected:
	static BOOL CALLBACK StaticDialogProc(	HWND	hWnd,
											UINT	uiMsg,
											WPARAM	wParam,
											LPARAM	lParam);

	static BOOL CALLBACK StaticPropertySheetPageProc(	HWND	hWnd,
														UINT	uiMsg,
														WPARAM	wParam,
														LPARAM	lParam);

private:
	BOOL DialogProc(HWND	hWnd,
					UINT	uiMsg,
					WPARAM	wParam,
					LPARAM	lParam);

protected:
	int m_iButtonOKID;
	int m_iButtonCancelID;

	bool m_bAccelerateOK;
	bool m_bAccelerateCancel;
	bool m_bCancelOnClose;

protected:
	virtual HINSTANCE GetInstance() const
		{ return GetKModuleHandle(NULL); }

	virtual int GetTemplateID() const = 0;

	virtual DLGPROC GetStaticDialogProc() const
		{ return StaticDialogProc; }

	virtual DLGPROC GetStaticPropertySheetPageProc() const
		{ return StaticPropertySheetPageProc; }

	// Message handlers
	virtual bool OnMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam);

	virtual bool OnDestroy();

	virtual bool OnInitDialog();

	virtual bool OnCommand(int iID, HWND hWnd, int iCode);

	virtual bool OnClose();

	virtual bool OnMeasureItem(int iID, LPMEASUREITEMSTRUCT pMIS)
		{ return false; }

	virtual bool OnDrawItem(int iID, LPDRAWITEMSTRUCT pDIS)
		{ return false; }

	virtual bool OnNotify(	int						iID,
							const TNotification*	pNotification,
							LRESULT&				RResult)
	{
		return false;
	}

	virtual bool OnEraseBKGND()
		{ return false; }

	virtual bool OnPaint()
		{ return false; }

	virtual bool OnTimer(size_t szID)
		{ return false; }

	virtual bool OnSize(kmode_t mdMode)
		{ return false; }

	// Special handlers
	virtual void OnOK();

	virtual void OnCancel();

	void SetIcon(HICON hIcon, bool bBig);

	void SetForcedResult(BOOL bSResult)
		{ m_bForcedResult = bSResult, m_bUseForcedResult = true; }

public:
	TDialog(bool bInputDialog); // sets accelerators appropriately

	virtual ~TDialog()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hWnd; }

	void Release();

	void EndDialog(int iResult);

	int DoModal(HWND hParentWnd);

	void Destroy();

	TDialog& Create(HWND hParentWnd = NULL);

	TDialog& CreateAsPropertySheetPage(TDialog& SOwnerDialog);

	KString GetTitle() const
		{ DEBUG_VERIFY_ALLOCATION; return GetKWindowText(m_hWnd); }

	void SetTitle(LPCTSTR pText)
		{ DEBUG_VERIFY_ALLOCATION; SetKWindowText(m_hWnd, pText); }

	HWND GetDlgItemSafe	(int iID) const;
	HWND GetDlgItem		(int iID) const;

	KString GetDlgItemTextSafe	(int iID) const;
	KString GetDlgItemText		(int iID) const;

	void SetDlgItemTextSafe	(int iID, const KString& Text);
	void SetDlgItemText		(int iID, const KString& Text);

	bool SetDlgItemTextNoFlickerSafe(int iID, const KString& Text);
	bool SetDlgItemTextNoFlicker	(int iID, const KString& Text);

	void PostDlgItemMessage(int iID, UINT uiMsg, WPARAM wParam, LPARAM lParam) const;

	LRESULT SendDlgItemMessage(int iID, UINT uiMsg, WPARAM wParam, LPARAM lParam) const;

	HWND SetDlgItemFocus(int iID);

	bool IsDlgButtonChecked	(int iID) const;
	void CheckDlgButton		(int iID, bool bCheck);

	bool IsDlgItemVisible	(int iID) const;
	void ShowDlgItem		(int iID, bool bShow);

	bool IsDlgItemEnabled	(int iID) const;
	void EnableDlgItem		(int iID, bool bEnable);

	void FilterEditControl(int iID,  LPCTSTR pAllow, LPCTSTR pDeny = NULL);

	int MessageBox(LPCTSTR pCaption, LPCTSTR pText, kflags_t flFlags);

	void InvalidateWindow();

	void UpdateWindow();

	bool IsDestroying() const
		{ return m_bDestroying; }

	IRECT GetPlaceholderRect(int iID)
		{ DEBUG_VERIFY_ALLOCATION; return GetWindowPlacementRect(GetDlgItem(iID)); }

	// Getters
	HWND GetWnd() const;

	operator HWND () const { return GetWnd(); }

	HPROPSHEETPAGE GetPropertySheetPage() const;

	operator HPROPSHEETPAGE () const { return GetPropertySheetPage(); }

	// ---------------- TRIVIALS ----------------
	TMode GetMode() const { return m_Mode; }

	HWND GetWndSafe() const { return m_hWnd; }

	HPROPSHEETPAGE GetPropertySheetPageSafe() const { return m_hPropertySheetPage; }
};

// ----------------------
// Dialog data exception
// ----------------------
struct TDialogDataException
{
public:
	KString m_ErrorText;
	int		m_iFocusID;

public:
	TDialogDataException(LPCTSTR pErrorText, int iFocusID = UINT_MAX) :
		m_ErrorText(pErrorText), m_iFocusID(iFocusID) {}

	// 'true' - stop, 'false' - continue
	bool Apply(	TDialog&	Dialog,
				kflags_t	flMessageBoxFlags	= MB_OK | MB_ICONSTOP,
				int			iStopRetCode		= IDOK);
};

#endif // dialog_h