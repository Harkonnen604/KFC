#include "kfc_common_pch.h"
#include "dde.h"

#include "atom.h"
#include "timer.h"

#ifdef _MSC_VER

// Delays
#define DDE_TIMEOUT				(2000)
#define DDE_POLL_DELAY			(50)

// Timers
#define TEST_PEER_TIMER_ID	(1)

// Delays
#ifdef _DEBUG
	#define TEST_PEER_TIMER_DELAY	(30000)
#else // _DEBUG
	#define TEST_PEER_TIMER_DELAY	(5000)
#endif // _DEBUG

// Client messages
#define WM_PROCESS_DDE_VALUES	(WM_USER + 0x40)
#define WM_FIRE_ON_TERMINATE	(WM_USER + 0x41)

// -----------
// DDE server
// -----------
ATOM T_DDE_Server::ms_aListenerAtom				= 0;
ATOM T_DDE_Server::ms_aConnectionAtom			= 0;

ATOM T_DDE_Server::ms_aConnectionTypePropName	= 0;
ATOM T_DDE_Server::ms_aClientWndPropName		= 0;
ATOM T_DDE_Server::ms_aTerminatingPropName		= 0;

void T_DDE_Server::UnregisterWindowClasses()
{
	if(ms_aTerminatingPropName)
		DeleteAtom(ms_aTerminatingPropName), ms_aTerminatingPropName = 0;

	if(ms_aClientWndPropName)
		DeleteAtom(ms_aClientWndPropName), ms_aClientWndPropName = 0;

	if(ms_aConnectionTypePropName)
		DeleteAtom(ms_aConnectionTypePropName), ms_aConnectionTypePropName = 0;

	if(ms_aConnectionAtom)
	{
		UnregisterClass((LPCTSTR)ms_aConnectionAtom, GetModuleHandle(NULL));
		ms_aConnectionAtom = 0;
	}

	if(ms_aListenerAtom)
	{
		UnregisterClass((LPCTSTR)ms_aListenerAtom, GetModuleHandle(NULL));
		ms_aListenerAtom = 0;
	}
}

void T_DDE_Server::RegisterWindowClasses()
{
	{
		WNDCLASS wc;
		memset(&wc, 0, sizeof(wc));

		wc.lpszClassName	= TEXT("KFC_DDE_ServerListenerClass");
		wc.hInstance		= GetModuleHandle(NULL);
		wc.lpfnWndProc		= StaticListenerWindowProc;

		ms_aListenerAtom = RegisterClass(&wc);
		KFC_VERIFY(ms_aListenerAtom);
	}

	{
		WNDCLASS wc;
		memset(&wc, 0, sizeof(wc));

		wc.lpszClassName	= TEXT("KFC_DDE_ServerConnectionClass");
		wc.hInstance		= GetModuleHandle(NULL);
		wc.lpfnWndProc		= StaticConnectionWindowProc;

		ms_aConnectionAtom = RegisterClass(&wc);
		KFC_VERIFY(ms_aConnectionAtom);
	}

	ms_aConnectionTypePropName = AddAtom("ConnectionType");
	KFC_VERIFY(ms_aConnectionTypePropName);

	ms_aClientWndPropName = AddAtom("ClientWnd");
	KFC_VERIFY(ms_aClientWndPropName);

	ms_aTerminatingPropName = AddAtom("Terminating");
	KFC_VERIFY(ms_aTerminatingPropName);
}

T_DDE_Server::T_DDE_Server(bool bAllocate)
{
	m_hWnd = NULL;

	m_pCallbacksParam = NULL;

	m_pOnSetItem = NULL;	

	if(bAllocate)
		Allocate();
}

void T_DDE_Server::Release()
{
	if(m_hWnd)
	{
		KillTimer(m_hWnd, TEST_PEER_TIMER_ID);

		while(!m_ConnectionTypes.IsEmpty())
			DisconnectAll(m_ConnectionTypes.GetFirst());

		DestroyWindow(m_hWnd), m_hWnd = NULL;
	}

	// m_ConnectionTypes.Clear();
}

void T_DDE_Server::Allocate()
{
	Release();

	try
	{
		m_hWnd = CreateWindow(	(LPCTSTR)ms_aListenerAtom,
								TEXT("KFC_DDE_ServerListenerWindow"),
								WS_POPUP,
								0, 0,
								16, 16,
								NULL,
								NULL,
								GetModuleHandle(NULL),
								0);

		KFC_VERIFY(m_hWnd);

		SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

		SetTimer(m_hWnd, TEST_PEER_TIMER_ID, TEST_PEER_TIMER_DELAY, NULL);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void T_DDE_Server::DisconnectAll(TConnectionTypes::TIterator CTIter)
{
	FOR_EACH_AVL_STORAGE(CTIter->m_Servers, TWindows::TIterator, WIter)
	{
		if(!PostMessage((HWND)GetProp(*WIter, (LPCTSTR)ms_aClientWndPropName),
						WM_DDE_TERMINATE,
						(WPARAM)*WIter,
						0))
		{
			DestroyWindow(*WIter);

			CTIter->m_Servers.Del(WIter);

			continue;
		}

		SetProp(*WIter, (LPCTSTR)ms_aTerminatingPropName, (HANDLE)TRUE);
	}

	TTimer Timer(true);

	FOR_EACH_AVL_STORAGE(CTIter->m_Servers, TWindows::TIterator, WIter)
	{
		for(;;)
		{
			bool bDestroyed = false;

			for(;;)
			{
				MSG Msg;

				if(!PeekMessage(&Msg, *WIter, WM_DDE_FIRST, WM_DDE_LAST, PM_REMOVE))
					break;

				TranslateMessage(&Msg), DispatchMessage(&Msg);

				if(Msg.message == WM_DDE_TERMINATE)
				{
					bDestroyed = true;
					break;
				}
			}

			if(bDestroyed)
				break;

			if((QWORD)Timer >= DDE_TIMEOUT)
			{
				DestroyWindow(*WIter);

				// CTIter->m_Servers.Del(WIter);				

				break;
			}

			Sleep(DDE_POLL_DELAY);
		}		
	}

	m_ConnectionTypes.Del(CTIter);
}

HWND T_DDE_Server::Connect(TConnectionTypes::TIterator CTIter, HWND hClientWnd)
{
	RELEASE_SAFE_BLOCK_BEGIN
	{
		TGlobalAtom aApplication(CTIter->m_Application);
		TGlobalAtom aTopic		(CTIter->m_Topic);

		HWND hServerWnd =
			CreateWindow(	(LPCTSTR)ms_aConnectionAtom,
							"KFC_DDE_ServerConnectionWindow",
							WS_POPUP,
							0, 0,
							16, 16,
							NULL,
							NULL,
							GetModuleHandle(NULL),
							NULL);

		KFC_VERIFY(hServerWnd);

		SetWindowLong(hServerWnd, GWL_USERDATA, (LONG)this);

		SetProp(hServerWnd,
				(LPCTSTR)ms_aClientWndPropName,
				(HANDLE)hClientWnd);

		SetProp(hServerWnd,
				(LPCTSTR)ms_aConnectionTypePropName,
				(HANDLE)CTIter.x);

		SendMessage(hClientWnd,
					WM_DDE_ACK,
					(WPARAM)hServerWnd,
					MAKELPARAM((ATOM)aApplication, (ATOM)aTopic));

		CTIter->m_Servers.Add(hServerWnd);

		return hServerWnd;
	}
	RELEASE_SAFE_BLOCK_END

	return NULL;
}

bool T_DDE_Server::UnadviseAll(TConnectionTypes::TIterator CTIter, HWND hServerWnd)
{
	bool bHadOne = false;

	for(TAdviseLinkWI_Keys::TIterator WI_K_Iter =
			CTIter->m_AdviseLinkWI_Keys.FindFirst(hServerWnd), N_WI_K_Iter ;
		WI_K_Iter.IsValid() && !Compare(*WI_K_Iter, hServerWnd) &&
			(N_WI_K_Iter = WI_K_Iter.GetNext(), true) ;
		WI_K_Iter = N_WI_K_Iter)
	{
		TAdviseLinkIW_Keys::TIterator IW_K_Iter =
			CTIter->m_AdviseLinkIW_Keys.
				Find(TAdviseLinkIW_Key(WI_K_Iter->m_IIter, hServerWnd));

		DEBUG_VERIFY(IW_K_Iter.IsValid());

		DEBUG_VERIFY(WI_K_Iter->m_LIter == IW_K_Iter->m_LIter);

		CTIter->m_AdviseLinks.Del(WI_K_Iter->m_LIter);
		
		CTIter->m_AdviseLinkWI_Keys.Del(WI_K_Iter);
		CTIter->m_AdviseLinkIW_Keys.Del(IW_K_Iter);

		bHadOne = true;
	}

	return bHadOne;
}

bool T_DDE_Server::UnadviseAll(TConnectionTypes::TIterator CTIter, TItems::TIterator IIter)
{
	bool bHadOne = false;

	for(TAdviseLinkIW_Keys::TIterator IW_K_Iter =
			CTIter->m_AdviseLinkIW_Keys.FindFirst(IIter), N_IW_K_Iter ;
		IW_K_Iter.IsValid() && !Compare(*IW_K_Iter, IIter) &&
			(N_IW_K_Iter = IW_K_Iter.GetNext(), true) ;
		IW_K_Iter = N_IW_K_Iter)
	{
		TAdviseLinkWI_Keys::TIterator WI_K_Iter =
			CTIter->m_AdviseLinkWI_Keys.
				Find(TAdviseLinkWI_Key(IW_K_Iter->m_hServerWnd, IW_K_Iter->m_IIter));

		DEBUG_VERIFY(WI_K_Iter->m_LIter == IW_K_Iter->m_LIter);

		CTIter->m_AdviseLinks.Del(WI_K_Iter->m_LIter);

		CTIter->m_AdviseLinkWI_Keys.Del(WI_K_Iter);
		CTIter->m_AdviseLinkIW_Keys.Del(IW_K_Iter);

		bHadOne = true;
	}

	return bHadOne;
}

bool T_DDE_Server::SendAck(	HWND hServerWnd,
							HWND hClientWnd,
							ATOM aName,
							WORD wRetCode)
{
	DDEACK Ack;
	memset(&Ack, 0, sizeof(Ack));

	Ack.fAck = !wRetCode;

	Ack.bAppReturnCode = wRetCode;

	LPARAM DDE_lParam;

	if(!PostMessage(hClientWnd,
					WM_DDE_ACK,
					(WPARAM)hServerWnd,
					DDE_lParam = PackDDElParam(WM_DDE_ACK, (WORD&)Ack, aName)))
	{
		FreeDDElParam(WM_DDE_ACK, DDE_lParam);

		GlobalDeleteAtom(aName);

		return false;
	}

	return true;
}

bool T_DDE_Server::SendData(HWND	hServerWnd,
							HWND	hClientWnd,
							ATOM	aName,
							LPCTSTR	pString,
							bool	bResponse,
							bool	bAckReq)
{
	HANDLE hData = NULL;

	if(pString)
	{
		size_t szLength = strlen(pString);

		hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DDEDATA) + szLength);

		if(!hData)
		{
			GlobalDeleteAtom(aName);
			return false;
		}

		DDEDATA* pData = (DDEDATA*)GlobalLock(hData);

		if(!pData)
		{
			if(hData) GlobalFree(hData);
			GlobalDeleteAtom(aName);
			return false;
		}

		pData->fResponse	= bResponse;
		pData->fRelease		= true;
		pData->fAckReq		= bAckReq;
		pData->cfFormat		= CF_TEXT;

		memcpy(pData->Value, pString, szLength + 1);

		GlobalUnlock(hData);
	}

	LPARAM DDE_lParam;

	if(!PostMessage(hClientWnd,
					WM_DDE_DATA,
					(WPARAM)hServerWnd,
					DDE_lParam = PackDDElParam(WM_DDE_DATA, (UINT)hData, aName)))
	{
		FreeDDElParam(WM_DDE_DATA, DDE_lParam);
		if(hData) GlobalFree(hData);
		GlobalDeleteAtom(aName);
		return false;
	}

	return true;
}

bool T_DDE_Server::SetItem(	LPCTSTR pApplication,
							LPCTSTR pTopic,
							LPCTSTR pName,
							LPCTSTR pValue,
							bool	bForceNotify)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(*pValue);

	TConnectionTypes::TIterator CTIter =
		m_ConnectionTypes.Find(TConnectionType::TKey(pApplication, pTopic));

	if(!CTIter.IsValid())
		CTIter = m_ConnectionTypes.Add(TConnectionType::TKey(pApplication, pTopic));

	TItems::TIterator IIter = CTIter->m_Items.Find(pName);

	if(!IIter.IsValid())
		IIter = CTIter->m_Items.Add(pName);

	if(!bForceNotify && IIter->m_Value == pValue)
		return false;

	IIter->m_Value = pValue;

	for(TAdviseLinkIW_Keys::TConstIterator IW_K_Iter =
			CTIter->m_AdviseLinkIW_Keys.FindFirst(IIter) ;
		IW_K_Iter.IsValid() && !Compare(*IW_K_Iter, IIter) ;
		++IW_K_Iter)
	{
		const TAdviseLink& AdviseLink = *IW_K_Iter->m_LIter;

		ATOM aName = GlobalAddAtom(IIter->m_Name);

		if(!aName)
			continue;

		SendData(	AdviseLink.m_hServerWnd,
					AdviseLink.m_hClientWnd,
					aName,
					AdviseLink.m_bHot ? IIter->m_Value : NULL,
					false,
					AdviseLink.m_bAckReq);
	}

	return true;
}

bool T_DDE_Server::RemoveItem(	LPCTSTR	pApplication,
								LPCTSTR	pTopic,
								LPCTSTR	pName,
								bool	bDisconnectIfEmpty)
{
	DEBUG_VERIFY_ALLOCATION;

	TConnectionTypes::TIterator CTIter =
		m_ConnectionTypes.Find(TConnectionType::TKey(pApplication, pTopic));

	if(!CTIter.IsValid())
		return false;

	TItems::TIterator IIter = CTIter->m_Items.Find(pName);

	if(!IIter.IsValid())
		return false;

	UnadviseAll(CTIter, IIter);

	CTIter->m_Items.Del(IIter);

	if(bDisconnectIfEmpty && CTIter->m_Items.IsEmpty())
		DisconnectAll(CTIter);

	return true;
}

bool T_DDE_Server::HasItem(	LPCTSTR pApplication,
							LPCTSTR pTopic,
							LPCTSTR pName) const
{
	DEBUG_VERIFY_ALLOCATION;

	TConnectionTypes::TConstIterator CTIter =
		m_ConnectionTypes.Find(TConnectionType::TKey(pApplication, pTopic));

	return CTIter.IsValid() && CTIter->m_Items.Has(pName);
}

void T_DDE_Server::TestPeers()
{
	DEBUG_VERIFY_ALLOCATION;

	FOR_EACH_AVL_STORAGE(m_ConnectionTypes, TConnectionTypes::TIterator, CTIter)
	{
		FOR_EACH_AVL_STORAGE(CTIter->m_Servers, TWindows::TIterator, WIter)
		{
			if(!IsWindow((HWND)GetProp(*WIter, (LPCTSTR)ms_aClientWndPropName)))
			{
				CTIter->m_Servers.Del(WIter);

				DestroyWindow(*WIter);
			}
		}
	}
}

LRESULT CALLBACK T_DDE_Server::StaticListenerWindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(!hWnd)
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);

	T_DDE_Server* pServer = (T_DDE_Server*)GetWindowLong(hWnd, GWL_USERDATA);

	if(!pServer)
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);

	return pServer->ListenerWindowProc(hWnd, uiMsg, wParam, lParam);	
}

LRESULT T_DDE_Server::ListenerWindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
	case WM_TIMER:
		if(wParam == TEST_PEER_TIMER_ID)
		{
			if(IsAllocated())
				TestPeers();
		}

		return 0;

	case WM_DDE_INITIATE:
		{
			HWND hClientWnd = (HWND)wParam;

			if(!IsWindow(hClientWnd))
				return 0;
			
			KString Application;
			KString Topic;

			ATOM aApplication	= LOWORD(lParam);
			ATOM aTopic			= HIWORD(lParam);

			if(aApplication)
			{
				if(!KGlobalGetAtomName(aApplication, Application))
					return 0;
			}

			if(aTopic)
			{
				if(!KGlobalGetAtomName(aTopic, Topic))
					return 0;
			}
			
			if(aApplication && aTopic)
			{
				TConnectionTypes::TIterator CTIter =
					m_ConnectionTypes.Find(TConnectionType::TKey(Application, Topic));

				if(CTIter.IsValid())
					Connect(CTIter, hClientWnd);
			}
			else if(aApplication)
			{
				for(TConnectionTypes::TIterator CTIter =
						m_ConnectionTypes.FindFirst(Application) ;
					CTIter.IsValid() && !Compare(*CTIter, Application) ;
					++CTIter)
				{
					Connect(CTIter, hClientWnd);
				}
			}
			else if(aTopic)
			{
				FOR_EACH_AVL_STORAGE(m_ConnectionTypes, TConnectionTypes::TIterator, CTIter)
				{
					if(!CompareNoCase(CTIter->m_Topic, Topic))
						Connect(CTIter, hClientWnd);
				}
			}
			else
			{
				FOR_EACH_AVL_STORAGE(m_ConnectionTypes, TConnectionTypes::TIterator, CTIter)
					Connect(CTIter, hClientWnd);
			}
		}

		return 0;
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

LRESULT CALLBACK T_DDE_Server::StaticConnectionWindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(!hWnd)
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);

	T_DDE_Server* pServer = (T_DDE_Server*)GetWindowLong(hWnd, GWL_USERDATA);

	if(!pServer)
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);

	return pServer->ConnectionWindowProc(hWnd, uiMsg, wParam, lParam);	
}

LRESULT T_DDE_Server::ConnectionWindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
	case WM_DDE_REQUEST:
		{
			HWND hClientWnd = (HWND)wParam;

			union
			{
				WORD wFormat;
				UINT _1;
			};

			union
			{
				ATOM aName;
				UINT _2;
			};

			if(!UnpackDDElParam(WM_DDE_REQUEST, lParam, &_1, &_2))
				return 0;

			FreeDDElParam(WM_DDE_REQUEST, lParam);

			KString Name;

			if(!KGlobalGetAtomName(aName, Name))
				return 0;

			if((BOOL)GetProp(hWnd, (LPCTSTR)ms_aTerminatingPropName))
			{
				GlobalDeleteAtom(aName);
				return 0;
			}

			if(wFormat != CF_TEXT)
			{
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_UNSUPPORTED_FORMAT);
				return 0;
			}			

			TConnectionTypes::TIterator CTIter =
				(void*)GetProp(hWnd, (LPCTSTR)ms_aConnectionTypePropName);

			DEBUG_VERIFY(CTIter.IsValid());

			TItems::TIterator IIter = CTIter->m_Items.Find(Name);

			if(!IIter.IsValid())
			{
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_ITEM_NOT_FOUND);
				return 0;
			}

			if(!SendData(hWnd, hClientWnd, aName, IIter->m_Value, true, false))
				return 0;
		}

		return 0;

	case WM_DDE_POKE:
		{
			HWND hClientWnd = (HWND)wParam;

			union
			{
				HANDLE hPoke;
				UINT    _1;
			};

			union
			{
				ATOM aName;
				UINT _2;
			};

			if(!UnpackDDElParam(WM_DDE_POKE, lParam, &_1, &_2))
				return 0;

			FreeDDElParam(WM_DDE_POKE, lParam);

			KString Name;

			if(!KGlobalGetAtomName(aName, Name))
				return 0;

			DDEPOKE* pPoke = (DDEPOKE*)GlobalLock(hPoke);

			if(!pPoke)
			{
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_INVALID_HANDLE);
				return 0;
			}

			bool bRelease = pPoke->fRelease;

			if((BOOL)GetProp(hWnd, (LPCTSTR)ms_aTerminatingPropName))
			{
				GlobalDeleteAtom(aName);
				GlobalUnlock(hPoke);
				if(bRelease) GlobalFree(hPoke);
				return 0;
			}

			if(pPoke->cfFormat != CF_TEXT)
			{
				GlobalUnlock(hPoke);
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_UNSUPPORTED_FORMAT);
				return 0;
			}			

			int iLength = GlobalSize(hPoke) - sizeof(DDEPOKE);

			if(iLength < 0 || iLength > 1023)
			{
				GlobalUnlock(hPoke);
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_INVALID_LENGTH);
				return 0;
			}

			TConnectionTypes::TIterator CTIter =
				(void*)GetProp(hWnd, (LPCTSTR)ms_aConnectionTypePropName);

			DEBUG_VERIFY(CTIter.IsValid());

			KString Value((char*)pPoke->Value, iLength);
			Value.TrimByZero();

			GlobalUnlock(hPoke), pPoke = NULL;

			if(bRelease)
				GlobalFree(hPoke), hPoke = NULL;

			WORD wRetCode = KFC_DDE_SERVER_ERROR_REJECTED;

			if(	m_pOnSetItem &&
				m_pOnSetItem(	CTIter->m_Application,
								CTIter->m_Topic,
								Name,
								Value,
								m_pCallbacksParam))
			{
				wRetCode = KFC_DDE_SERVER_OK;
			}

			if(!SendAck(hWnd, hClientWnd, aName, wRetCode))
				return 0;
		}

		return 0;

	case WM_DDE_ADVISE:
		{
			HWND hClientWnd = (HWND)wParam;

			union
			{
				HANDLE hAdvise;
				UINT   _1;
			};

			union
			{
				ATOM aName;
				UINT _2;
			};

			if(!UnpackDDElParam(WM_DDE_ADVISE, lParam, &_1, &_2))
				return 0;

			FreeDDElParam(WM_DDE_ADVISE, lParam);

			KString Name;

			if(!KGlobalGetAtomName(aName, Name))
				return 0;

			DDEADVISE* pAdvise = (DDEADVISE*)GlobalLock(hAdvise);

			if(!pAdvise)
			{
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_INVALID_HANDLE);
				return 0;
			}

			if((BOOL)GetProp(hWnd, (LPCTSTR)ms_aTerminatingPropName))
			{
				GlobalDeleteAtom(aName);
				GlobalUnlock(hAdvise);
				GlobalFree(hAdvise);
				return 0;
			}

			if(pAdvise->cfFormat != CF_TEXT)
			{
				GlobalUnlock(hAdvise);
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_UNSUPPORTED_FORMAT);
				return 0;
			}			

			if(GlobalSize(hAdvise) < sizeof(DDEADVISE))
			{
				GlobalUnlock(hAdvise);
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_INVALID_HANDLE);
				return 0;
			}

			TConnectionTypes::TIterator CTIter =
				(void*)GetProp(hWnd, (LPCTSTR)ms_aConnectionTypePropName);

			DEBUG_VERIFY(CTIter.IsValid());

			TItems::TIterator IIter = CTIter->m_Items.Find(Name);

			if(!IIter.IsValid())
			{
				GlobalUnlock(hAdvise);
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_ITEM_NOT_FOUND);
				return 0;
			}

			bool bAckReq = pAdvise->fAckReq;

			bool bHot = !pAdvise->fDeferUpd;

			GlobalUnlock(hAdvise), pAdvise = NULL;

			GlobalFree(hAdvise), hAdvise = NULL;
			
			TAdviseLinks::TIterator LIter;

			if(!CTIter->m_AdviseLinkWI_Keys.Has(TAdviseLinkWI_Key(hWnd, IIter)))
			{
				DEBUG_VERIFY(	!CTIter->m_AdviseLinkIW_Keys.
									Has(TAdviseLinkIW_Key(IIter, hWnd)));

				LIter = CTIter->m_AdviseLinks.AddLast();

				LIter->m_hServerWnd = hWnd;
				LIter->m_hClientWnd	= hClientWnd;
				LIter->m_bHot		= bHot;
				LIter->m_bAckReq	= bAckReq;

				CTIter->m_AdviseLinkWI_Keys.
					Add(TAdviseLinkWI_Key(hWnd, IIter))->
						m_LIter = LIter;

				CTIter->m_AdviseLinkIW_Keys.
					Add(TAdviseLinkIW_Key(IIter, hWnd))->
						m_LIter = LIter;
			}

			if(!SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_OK))
				return 0;

			if(LIter.IsValid())
			{
				SendData(	LIter->m_hServerWnd,
							LIter->m_hClientWnd,
							aName,
							LIter->m_bHot ? IIter->m_Value : NULL,
							false,
							LIter->m_bAckReq);
			}
		}

		return 0;

	case WM_DDE_UNADVISE:
		{
			HWND hClientWnd = (HWND)wParam;

			union
			{
				WORD wFormat;
				UINT _1;
			};

			union
			{
				ATOM aName;
				UINT _2;
			};

			if(!UnpackDDElParam(WM_DDE_UNADVISE, lParam, &_1, &_2))
				return 0;

			FreeDDElParam(WM_DDE_UNADVISE, lParam);

			KString Name;

			if(!KGlobalGetAtomName(aName, Name))
				return 0;

			if((BOOL)GetProp(hWnd, (LPCTSTR)ms_aTerminatingPropName))
			{
				GlobalDeleteAtom(aName);
				return 0;
			}

			if(aName && wFormat != CF_TEXT)
			{
				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_UNSUPPORTED_FORMAT);
				return 0;
			}

			TConnectionTypes::TIterator CTIter =
				(void*)GetProp(hWnd, (LPCTSTR)ms_aConnectionTypePropName);

			DEBUG_VERIFY(CTIter.IsValid());

			if(aName)
			{
				TItems::TIterator IIter = CTIter->m_Items.Find(Name);

				if(!IIter.IsValid())
				{
					SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_ITEM_NOT_FOUND);
					return 0;
				}

				TAdviseLinkWI_Keys::TIterator WI_K_Iter =
					CTIter->m_AdviseLinkWI_Keys.Find(TAdviseLinkWI_Key(hWnd, IIter));

				if(!WI_K_Iter.IsValid())
				{
					DEBUG_VERIFY(	!CTIter->m_AdviseLinkIW_Keys.
										Has(TAdviseLinkIW_Key(IIter, hWnd)));

					SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_ERROR_NOT_ADVISED);

					return 0;
				}

				TAdviseLinkIW_Keys::TIterator IW_K_Iter =
					CTIter->m_AdviseLinkIW_Keys.Find(TAdviseLinkIW_Key(IIter, hWnd));

				DEBUG_VERIFY(IW_K_Iter.IsValid());

				DEBUG_VERIFY(WI_K_Iter->m_LIter == IW_K_Iter->m_LIter);

				CTIter->m_AdviseLinks.Del(WI_K_Iter->m_LIter);

				CTIter->m_AdviseLinkWI_Keys.Del(WI_K_Iter);
				CTIter->m_AdviseLinkIW_Keys.Del(IW_K_Iter);

				SendAck(hWnd, hClientWnd, aName, KFC_DDE_SERVER_OK);
			}
			else
			{
				bool bHadOne = UnadviseAll(CTIter, hWnd);				

				SendAck(hWnd,
						hClientWnd,
						aName,
						bHadOne ? KFC_DDE_SERVER_OK : KFC_DDE_SERVER_ERROR_ITEM_NOT_FOUND);
			}			
		}

		return 0;

	case WM_DDE_ACK:
		{
			union
			{
				DDEACK Ack;
				UINT   _1;
			};

			union
			{
				ATOM aName;
				UINT _2;
			};

			if(!UnpackDDElParam(WM_DDE_ACK, lParam, &_1, &_2))
				return 0;

			FreeDDElParam(WM_DDE_ACK, lParam);

			GlobalDeleteAtom(aName);
		}

		return 0;

	case WM_DDE_TERMINATE:
		{
			HWND hClientWnd = (HWND)wParam;

			if(!(BOOL)GetProp(hWnd, (LPCTSTR)ms_aTerminatingPropName))
			{
				PostMessage(hClientWnd,
							WM_DDE_TERMINATE,
							(WPARAM)hWnd,
							0);
			}

			TConnectionTypes::TIterator CTIter =
				(void*)GetProp(hWnd, (LPCTSTR)ms_aConnectionTypePropName);

			DEBUG_VERIFY(CTIter.IsValid());

			UnadviseAll(CTIter, hWnd);

			TWindows::TIterator WIter = CTIter->m_Servers.Find(hWnd);

			DEBUG_VERIFY(WIter.IsValid());

			CTIter->m_Servers.Del(WIter);

			DestroyWindow(hWnd);			
		}

		return 0;	
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

// -----------
// DDE client
// -----------
ATOM T_DDE_Client::ms_aAtom = 0;

void T_DDE_Client::UnregisterWindowClass()
{
	if(ms_aAtom)
		UnregisterClass((LPCTSTR)ms_aAtom, GetModuleHandle(NULL)), ms_aAtom = 0;
}

void T_DDE_Client::RegisterWindowClass()
{
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));

	wc.lpszClassName	= TEXT("KFC_DDE_ClientClass");
	wc.hInstance		= GetModuleHandle(NULL);
	wc.lpfnWndProc		= StaticWindowProc;

	ms_aAtom = RegisterClass(&wc);
	KFC_VERIFY(ms_aAtom);
}

T_DDE_Client::T_DDE_Client()
{
	Init();
}

T_DDE_Client::T_DDE_Client(LPCTSTR pApplication, LPCTSTR pTopic, bool bConnect)
{
	Init();
	
	Allocate(pApplication, pTopic, bConnect);
}

void T_DDE_Client::Init()
{
	m_pOnTerminate = NULL;
	m_pOnTerminateParam = NULL;

	m_pOnData = NULL;
	m_pOnDataParam = NULL;

	m_hWnd = NULL;	
}

void T_DDE_Client::Release()
{
	if(IsAllocated())
		Disconnect();

	m_Application.Empty(), m_Topic.Empty();	
}

void T_DDE_Client::Disconnect()
{
	DEBUG_VERIFY_ALLOCATION;

	if(m_hWnd)
	{
		KillTimer(m_hWnd, TEST_PEER_TIMER_DELAY);

		if(m_hPeerWnd && IsWindow(m_hPeerWnd))
		{
			m_bTerminating = true;

			PostMessage(m_hPeerWnd, WM_DDE_TERMINATE, (WPARAM)m_hWnd, 0);			

			for(TTimer Timer(true) ; m_hPeerWnd && (QWORD)Timer < DDE_TIMEOUT ; )
			{
				MSG Msg;

				if(!PeekMessage(&Msg, m_hWnd, WM_DDE_FIRST, WM_DDE_LAST, PM_REMOVE))
				{
					Sleep(DDE_POLL_DELAY);
					continue;
				}

				TranslateMessage(&Msg), DispatchMessage(&Msg);
			}
		}

		DestroyWindow(m_hWnd), m_hWnd = NULL;
	}

	m_Values.Clear();
}

void T_DDE_Client::Connect()
{
	DEBUG_VERIFY_ALLOCATION;
	
	Disconnect();

	try
	{
		m_hPeerWnd = NULL;

		m_bTerminating = false;
		
		m_WaitItem.Empty();

		// Creating window
		{
			m_hWnd = CreateWindow(	(LPCTSTR)ms_aAtom,
									TEXT("KFC_DDE_ClientWindow"),
									WS_POPUP,
									0, 0,
									16, 16,
									NULL,
									NULL,
									GetModuleHandle(NULL),
									0);

			KFC_VERIFY(m_hWnd);

			SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

			SetTimer(m_hWnd, TEST_PEER_TIMER_ID, TEST_PEER_TIMER_DELAY, NULL);
		}

		// Connecting
		{
			m_hConnectPeerWnd = NULL;

			ATOM aApplication = GlobalAddAtom(m_Application);
			KFC_VERIFY(aApplication);

			ATOM aTopic = GlobalAddAtom(m_Topic);
			KFC_VERIFY(aTopic);

			SendMessage(HWND_BROADCAST,
						WM_DDE_INITIATE,
						(WPARAM)m_hWnd,
						MAKELONG(aApplication, aTopic));

			GlobalDeleteAtom(aTopic);

			GlobalDeleteAtom(aApplication);
		}

		if(!m_hConnectPeerWnd)
		{
			INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error connecting DDE server \"") +
											m_Application + TEXT('|') + m_Topic + TEXT("\"."));
		}

		m_hPeerWnd = m_hConnectPeerWnd;
	}

	catch(...)
	{
		Disconnect();
		throw;
	}
}

bool T_DDE_Client::TestPeer()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsConnected());

	if(IsWindow(m_hPeerWnd))
		return true;

	Disconnect();

	if(m_pOnTerminate)
		m_pOnTerminate(m_Application, m_Topic, m_pOnTerminateParam);

	return false;
}

void T_DDE_Client::Allocate(LPCTSTR pApplication, LPCTSTR pTopic, bool bConnect)
{
	Release();

	try
	{
		DEBUG_VERIFY(*pApplication && *pTopic);

		m_Application = pApplication, m_Topic = pTopic;

		if(bConnect)
			Connect();
	}

	catch(...)
	{
		Release();
		throw;
	}
}

KString T_DDE_Client::RequestData(LPCTSTR pItem)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(*pItem);

	ATOM aItem = GlobalAddAtom(pItem);
	KFC_VERIFY(aItem);

	m_WaitItem = pItem;

	LPARAM DDE_lParam;

	if(!PostMessage(m_hPeerWnd,
					WM_DDE_REQUEST,
					(WPARAM)m_hWnd,
					DDE_lParam = PackDDElParam(WM_DDE_REQUEST, CF_TEXT, aItem)))
	{
		m_WaitItem.Empty();

		FreeDDElParam(WM_DDE_REQUEST, DDE_lParam);

		GlobalDeleteAtom(aItem);

		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error requesting DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}

	for(TTimer Timer(true) ; !m_WaitItem.IsEmpty() && (QWORD)Timer < DDE_TIMEOUT ; )
	{
		MSG Msg;

		if(!PeekMessage(&Msg, m_hWnd, WM_DDE_FIRST, WM_DDE_LAST, PM_REMOVE))
		{
			Sleep(DDE_POLL_DELAY);
			continue;
		}

		TranslateMessage(&Msg), DispatchMessage(&Msg);
	}

	if(!IsAllocated() || !IsConnected()) // termination during ack cycle
	{
		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error requesting DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}

	if(!m_Values.IsEmpty())
		PostMessage(m_hWnd, WM_PROCESS_DDE_VALUES, 0, 0);

	if(!m_WaitItem.IsEmpty() || !m_bSuccess)
	{
		m_WaitItem.Empty();

		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error requesting DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}

	return m_WaitData;
}

WORD T_DDE_Client::SetData(LPCTSTR pItem, LPCTSTR pValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(*pItem);

	HANDLE hOptions = GlobalAlloc(GMEM_MOVEABLE, sizeof(DDEPOKE) + (_tcslen(pValue) + 1) * sizeof(TCHAR) - 1);
	KFC_VERIFY(hOptions);

	{
		DDEPOKE* pOptions = (DDEPOKE*)GlobalLock(hOptions);
		KFC_VERIFY(pOptions);

		pOptions->fRelease = TRUE;
		pOptions->cfFormat = CF_TEXT;
		_tcscpy((LPTSTR)pOptions->Value, pValue);

		GlobalUnlock(hOptions);
	}

	ATOM aItem = GlobalAddAtom(pItem);

	if(!aItem)
	{
		GlobalFree(hOptions);
		KFC_VERIFY(aItem);
	}

	m_WaitItem = pItem;

	LPARAM DDE_lParam;

	if(!PostMessage(m_hPeerWnd,
					WM_DDE_POKE,
					(WPARAM)m_hWnd,
					DDE_lParam = PackDDElParam(WM_DDE_POKE, (UINT)hOptions, aItem)))
	{
		m_WaitItem.Empty();

		FreeDDElParam(WM_DDE_POKE, DDE_lParam);

		GlobalDeleteAtom(aItem);

		GlobalFree(hOptions);

		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error poking DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}

	for(TTimer Timer(true) ; !m_WaitItem.IsEmpty() && (QWORD)Timer < DDE_TIMEOUT ; )
	{
		MSG Msg;

		if(!PeekMessage(&Msg, m_hWnd, WM_DDE_FIRST, WM_DDE_LAST, PM_REMOVE))
		{
			Sleep(DDE_POLL_DELAY);
			continue;
		}

		TranslateMessage(&Msg), DispatchMessage(&Msg);
	}

	if(!IsAllocated() || !IsConnected()) // termination during ack cycle
	{
		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error advising DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}

	if(!m_Values.IsEmpty())
		PostMessage(m_hWnd, WM_PROCESS_DDE_VALUES, 0, 0);

	if(!m_WaitItem.IsEmpty() || !m_bSuccess)
	{
		m_WaitItem.Empty();

		GlobalFree(hOptions);

		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error poking DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}

	return m_wResult;
}

void T_DDE_Client::Unadvise(LPCTSTR pItem)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(*pItem);

	ATOM aItem = GlobalAddAtom(pItem);
	KFC_VERIFY(aItem);

	LPARAM DDE_lParam;

	if(!PostMessage(m_hPeerWnd,
					WM_DDE_UNADVISE,
					(WPARAM)m_hWnd,
					DDE_lParam = PackDDElParam(WM_DDE_UNADVISE, 0, aItem)))
	{
		FreeDDElParam(WM_DDE_UNADVISE, DDE_lParam);

		GlobalDeleteAtom(aItem);
	}
}

void T_DDE_Client::Advise(LPCTSTR pItem)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(*pItem);

	HANDLE hOptions = GlobalAlloc(GMEM_MOVEABLE, sizeof(DDEADVISE));
	KFC_VERIFY(hOptions);

	{
		DDEADVISE* pOptions = (DDEADVISE*)GlobalLock(hOptions);
		KFC_VERIFY(pOptions);

		pOptions->cfFormat	= CF_TEXT;
		pOptions->fAckReq	= TRUE;
		pOptions->fDeferUpd	= FALSE;

		GlobalUnlock(hOptions);
	}

	ATOM aItem = GlobalAddAtom(pItem);

	if(!aItem)
	{
		GlobalFree(hOptions);
		KFC_VERIFY(aItem);
	}

	m_WaitItem = pItem;

	LPARAM DDE_lParam;

	if(!PostMessage(m_hPeerWnd,
					WM_DDE_ADVISE,
					(WPARAM)m_hWnd,
					DDE_lParam = PackDDElParam(WM_DDE_ADVISE, (UINT)hOptions, aItem)))
	{
		m_WaitItem.Empty();

		FreeDDElParam(WM_DDE_ADVISE, DDE_lParam);

		GlobalDeleteAtom(aItem);

		GlobalFree(hOptions);

		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error advising DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}

	for(TTimer Timer(true) ; !m_WaitItem.IsEmpty() && (QWORD)Timer < DDE_TIMEOUT ; )
	{
		MSG Msg;

		if(!PeekMessage(&Msg, m_hWnd, WM_DDE_FIRST, WM_DDE_LAST, PM_REMOVE))
		{
			Sleep(DDE_POLL_DELAY);
			continue;
		}

		TranslateMessage(&Msg), DispatchMessage(&Msg);
	}

	if(!IsAllocated() || !IsConnected()) // termination during ack cycle
	{
		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error advising DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}

	if(!m_Values.IsEmpty())
		PostMessage(m_hWnd, WM_PROCESS_DDE_VALUES, 0, 0);

	if(!m_WaitItem.IsEmpty() || !m_bSuccess)
	{
		m_WaitItem.Empty();

		GlobalFree(hOptions);

		INITIATE_DEFINED_FAILURE(	(KString)TEXT("Error advising DDE item \"") +
										m_Application + TEXT('|') + m_Topic + TEXT('!') + pItem +
										TEXT("\"."));
	}
}

bool T_DDE_Client::IsPeerAlive() const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_hPeerWnd);

	return IsWindow(m_hPeerWnd);
}

LRESULT CALLBACK T_DDE_Client::StaticWindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(!hWnd)
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);

	T_DDE_Client* pClient = (T_DDE_Client*)GetWindowLong(hWnd, GWL_USERDATA);

	if(!pClient)
		return DefWindowProc(hWnd, uiMsg, wParam, lParam);

	return pClient->WindowProc(hWnd, uiMsg, wParam, lParam);	
}

LRESULT T_DDE_Client::WindowProc
	(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
	case WM_TIMER:
		if(wParam == TEST_PEER_TIMER_ID)
		{
			if(IsAllocated() && IsConnected())
				TestPeer();
		}

		return 0;
 
	case WM_DDE_TERMINATE:
		{
			if(!m_hPeerWnd)
				return 0;

			if((HWND)wParam == m_hPeerWnd)
			{
				if(!m_bTerminating)
					PostMessage(m_hPeerWnd, WM_DDE_TERMINATE, (WPARAM)m_hWnd, 0);

				if(!m_WaitItem.IsEmpty())
					m_WaitItem.Empty(), m_bSuccess = false;

				m_hPeerWnd = NULL;

				if(!m_bTerminating)
					PostMessage(hWnd, WM_FIRE_ON_TERMINATE, 0, 0);
				else
					Disconnect();
			}

			return 0;
		}

	case WM_FIRE_ON_TERMINATE:
		{
			Disconnect();

			if(m_pOnTerminate)
				m_pOnTerminate(m_Application, m_Topic, m_pOnTerminateParam);

			return 0;
		}

	case WM_DDE_ACK:
		{
			if(m_hPeerWnd)
			{
				if((HWND)wParam == m_hPeerWnd)
				{
					union
					{
						DDEACK Ack;
						UINT _1;
					};

					union
					{
						ATOM aItem;
						UINT _2;
					};

					UnpackDDElParam(WM_DDE_ACK, lParam, &_1, &_2);

					FreeDDElParam(WM_DDE_ACK, lParam);

					KString Item;

					if(!KGlobalGetAtomName(aItem, Item))
						return 0;

					GlobalDeleteAtom(aItem);

					if(!m_WaitItem.IsEmpty() && m_WaitItem == Item)
					{
						m_WaitItem.Empty();
						m_wResult	= Ack.bAppReturnCode;
						m_bSuccess	= Ack.fAck;
					}
				}
			}
			else
			{
				HWND hPeerWnd = (HWND)wParam;			

				if(hPeerWnd)
				{
					ATOM aApplication	= LOWORD(lParam);
					ATOM aTopic			= HIWORD(lParam);

					KString Application;
					KString Topic;

					if(	KGlobalGetAtomName(aApplication,  Application) &&
							!CompareNoCase(m_Application, Application) &&
						KGlobalGetAtomName(aTopic, Topic) &&
							!CompareNoCase(m_Topic, Topic))
					{
						m_hConnectPeerWnd = hPeerWnd;
					}
				}
			}

			return 0;
		}

	case WM_DDE_DATA:
		{
			if(!m_hPeerWnd)
				return 0;

			if((HWND)wParam != m_hPeerWnd)
				return 0;

			union
			{
				HANDLE hData;
				UINT _1;
			};

			union
			{
				ATOM aItem;
				UINT _2;
			};

			UnpackDDElParam(WM_DDE_DATA, lParam, &_1, &_2);

			FreeDDElParam(WM_DDE_DATA, lParam);

			KString Item;

			if(!KGlobalGetAtomName(aItem, Item))
				return 0;

			KString Data;

			bool bRelease = false;

			int iDataLength = GlobalSize(hData) - offsetof(DDEDATA, Value[0]);

			DDEDATA* pDDE_Data = (DDEDATA*)GlobalLock(hData);

			if(!pDDE_Data || iDataLength < 0 || pDDE_Data->cfFormat != CF_TEXT)
			{
				DDEACK Ack = {0, 0, 0, 0};

				LPARAM DDE_lParam;

				if(!PostMessage(m_hPeerWnd,
								WM_DDE_ACK,
								(WPARAM)m_hWnd,
								DDE_lParam = PackDDElParam(WM_DDE_ACK, (WORD&)Ack, aItem)))
				{
					FreeDDElParam(WM_DDE_ACK, DDE_lParam);

					GlobalDeleteAtom(aItem);
				}
			}
			else
			{
				Data.Allocate((LPCSTR)pDDE_Data->Value, iDataLength);
				Data.TrimByZero();

				if(pDDE_Data->fAckReq)
				{
					DDEACK Ack = {0, 0, 0, 1};

					LPARAM DDE_lParam;

					if(!PostMessage(m_hPeerWnd,
									WM_DDE_ACK,
									(WPARAM)m_hWnd,
									DDE_lParam = PackDDElParam(WM_DDE_ACK, (WORD)(uintptr_t)&Ack, aItem)))
					{
						FreeDDElParam(WM_DDE_ACK, DDE_lParam);

						GlobalDeleteAtom(aItem);
					}
				}
				else
				{
					GlobalDeleteAtom(aItem);
				}

				if(pDDE_Data->fRelease)
					bRelease = true;

				if(m_WaitItem.IsEmpty())
				{
					if(m_pOnData)
						m_pOnData(m_Application, m_Topic, Item, Data, m_pOnDataParam);
				}
				else
				{
					if(m_WaitItem == Item)
						m_WaitItem.Empty(), m_bSuccess = true, m_WaitData = Data;
					else
						m_Values.AddLast()->Set(Item, Data);
				}
			}

			if(pDDE_Data)
				GlobalUnlock(hData);

			if(bRelease)
				GlobalFree(hData);			

			return 0;
		}

		case WM_PROCESS_DDE_VALUES:
			{
				if(m_pOnData)
				{
					while(!m_Values.IsEmpty())
					{
						const TValue Value = *m_Values.GetFirst();

						m_Values.DelFirst();

						m_pOnData(	m_Application,
									m_Topic,
									Value.m_Item,
									Value.m_Data,
									m_pOnDataParam);
					}
				}

				return 0;
			}
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

#endif // _MSC_VER
