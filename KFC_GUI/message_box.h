#ifndef message_box_h
#define message_box_h

#include <KFC_Common\window_timer.h>
#include "dialog.h"

// ------------
// Message box
// ------------
class TMessageBox : public TDialog
{
private:
	KString m_Text;
	KString m_Title;
	
	HICON m_hIcon;

	int m_iFocusButtonID;
	int m_iTimeoutButtonID;

	size_t m_szTimeout;

	ISIZE m_TextSize;
	ISIZE m_IconTextSize;
	ISIZE m_DialogSize;
	ISIZE m_DialogScreenSize;

	TWindowTimer m_TimeoutTimer;

	bool m_bTimedOut;


	HINSTANCE GetInstance() const;

	int GetTemplateID() const;

	bool OnInitDialog();

	bool OnPaint();

	bool OnTimer(size_t szID);

	void OnOK();

	void OnCancel();

public:
	TMessageBox(LPCTSTR	pSTitle,
				LPCTSTR	pSText,
				HICON	hSIcon,
				int		iSFocusButtonID,
				int		iSTimeoutButtonID,
				size_t	szSTimeout);

	bool HasTimedOut() const { return m_bTimedOut; }
};

#endif // message_box_h