#ifndef message_dialog_h
#define message_dialog_h

#include <KFC_KTL\critical_section.h>
#include "dialog.h"

// ---------------
// Message dialog
// ---------------
class TMessageDialog : public TDialog
{
private:
	static size_t s_szNMessageDialogsRunning;

	static const UINT s_uiTerminateMsg;

	HANDLE m_hStartEvent;

	mutable TCriticalSection m_MessageCriticalSection;

	KString m_Message;

	SZSIZE m_MessageSize;

	ISIZE m_OldSize;

	const bool m_bOnlyEnlarge;


	TDialog::DoModal;

	HINSTANCE GetInstance() const;

	int GetTemplateID() const;

	bool OnMessage(UINT uiMsg, WPARAM wParam, LPARAM lParam);

	bool OnDestroy();

	bool OnInitDialog();

	bool OnPaint();

	void SetSize();		

public:
	TMessageDialog(LPCTSTR pSMessaage, bool bOnlyEnlarge);

	int DoModal(HWND hParentWnd, HANDLE hSStartEvent);

	void Terminate(int iResult);

	// Thread-safe message access
	KString GetMessage() const;

	void SetMessage(LPCTSTR pSMessage, bool bForceRepaint = false);

	// ---------------- TRIVIALS ----------------
	static size_t IsMessageDialogRunning()
		{ return s_szNMessageDialogsRunning > 0; }
};

#endif // message_dialog_h
