#include "kfc_ktl_pch.h"
#include "event.h"

// ------
// Event
// ------
TEvent::TEvent()
{
	m_hEvent = NULL;
}

TEvent::TEvent(bool bInitialState, bool bManualReset)
{
	m_hEvent = NULL;

	Allocate(bInitialState, bManualReset);
}

TEvent::TEvent(LPCTSTR pName, bool bInitialState, bool bManualReset)
{
	m_hEvent = NULL;

	Create(pName, bInitialState, bManualReset);
}

#ifdef _MSC_VER

TEvent::TEvent(LPCTSTR pName)
{
	m_hEvent = NULL;

	Open(pName);
}

#endif // _MSC_VER

void TEvent::Release()
{
	if(m_hEvent)
		CloseHandle(m_hEvent), m_hEvent = NULL;
}

void TEvent::Allocate(bool bInitialState, bool bManualReset)
{
	Release();

	m_hEvent = CreateEvent(	NULL,
							bManualReset	? TRUE : FALSE,
							bInitialState	? TRUE : FALSE,
							NULL);

	if(m_hEvent == NULL)
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating event"),
										GetLastError());
	}
}

bool TEvent::Create(LPCTSTR pName, bool bInitialState, bool bManualReset)
{
	Release();

	DEBUG_VERIFY(pName);

	m_hEvent = CreateEvent(	NULL,
							bManualReset	? TRUE : FALSE,
							bInitialState	? TRUE : FALSE,
							pName);

	if(m_hEvent == NULL)
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error creating named event"),
										GetLastError());
	}

	return GetLastError() != ERROR_ALREADY_EXISTS;
}

#ifdef _MSC_VER

void TEvent::Open(LPCTSTR pName)
{
	Release();

	DEBUG_VERIFY(pName);

	m_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, pName);

	if(m_hEvent == NULL)
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error opening named event"),
										GetLastError());
	}
}

#endif // _MSC_VER

bool TEvent::WaitWithTermination(HANDLE hTerminator, size_t szTimeout)
{
	DEBUG_VERIFY_ALLOCATION;

	if(!hTerminator)
		return Wait(szTimeout);

	const HANDLE Handles[2] = {m_hEvent, hTerminator};

	DWORD r = WaitForMultipleObjects(ARRAY_SIZE(Handles), Handles, FALSE, (DWORD)szTimeout);

	if(r == WAIT_OBJECT_0 + 0)
		return true;

	if(r == WAIT_OBJECT_0 + 1)
		throw TTerminationException(__FILE__, __LINE__);

	return false;
}
