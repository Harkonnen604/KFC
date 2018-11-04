#include "kfc_common_pch.h"
#include "message_loop.h"

#include "common_tls_item.h"

#ifdef _MSC_VER

// ----------------
// Helper routines
// ----------------
static bool ProcessAuxMessages()
{
	TAuxMessages* pAuxMessages = TCommonTLS_Item::Get().m_pAuxMessages;

	DEBUG_VERIFY(pAuxMessages);

	while(!pAuxMessages->IsEmpty())
	{
		size_t szN = pAuxMessages->GetN(); // locking current boundary

		for(size_t i = 0 ; i < szN ; i++)
		{
			const CWPSTRUCT& Msg = (*pAuxMessages)[i];

			if(Msg.message == WM_QUIT)
				return false;

			SendMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
		}

		pAuxMessages->Del(0, szN); // avoiding repeatable aux queue overflow
	}

	return true;
}

// ----------------
// Global routines
// ----------------
void RunMessageLoop(HWND hAccelWnd, HACCEL hAccel)
{
	TAuxMessages AuxMessages;

	TAuxMessagesSetter Setter0(AuxMessages);

	for(;;)
	{
		MSG Msg;

		BOOL r = GetMessage(&Msg, NULL, 0, 0);

		if(!r)
			break;

		if(r == -1)
			INITIATE_DEFINED_CODE_FAILURE(TEXT("GetMessage() failure."), GetLastError());

		if(hAccelWnd && hAccel)
		{
			if(!TranslateAccelerator(hAccelWnd, hAccel, &Msg))
				TranslateMessage(&Msg);
		}
		else
		{
			TranslateMessage(&Msg);
		}

		DispatchMessage(&Msg);

		if(!ProcessAuxMessages())
			break;
	}
}

void PostAuxMessage(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	DEBUG_VERIFY(hWnd);

	DEBUG_VERIFY(IsWindow(hWnd));

	DEBUG_VERIFY(GetWindowThreadProcessId(hWnd, NULL) == GetCurrentThreadId());

	TAuxMessages* pAuxMessages = TCommonTLS_Item::Get().m_pAuxMessages;

	DEBUG_VERIFY(pAuxMessages);

	CWPSTRUCT& Msg = pAuxMessages->Add();

	Msg.hwnd	= hWnd;
	Msg.message	= uiMsg;
	Msg.wParam	= wParam;
	Msg.lParam	= lParam;
}

#endif // _MSC_VER
