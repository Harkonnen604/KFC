#include "kfc_ktl_pch.h"
#include "synched_queue.h"

// --------------
// Synched queue
// --------------
TSynchedQueue::TSynchedQueue()
{
	m_bAllocated = false;
}

TSynchedQueue::TSynchedQueue(HANDLE hTerminator)
{
	m_bAllocated = false;

	Allocate(hTerminator);
}

void TSynchedQueue::Release()
{
	m_bAllocated = false;

	m_Items.Clear();

	m_DoneEvent.Release();

	m_AvailableEvent.Release();
}

void TSynchedQueue::Allocate(HANDLE hTerminator)
{
	Release();

	try
	{
		m_AvailableEvent.Allocate(false, false);

		m_DoneEvent.Allocate(true, true);

		m_lNRegisteredItems = 0;

		m_hTerminator = hTerminator;

		m_bAllocated = true;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TSynchedQueue::RegisterItem()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_EVERIFY(InterlockedIncrement(&m_lNRegisteredItems) > 0);

	m_DoneEvent.Reset();
}

void TSynchedQueue::UnregisterItem()
{
	if(!IsAllocated())
		return;

	const LONG r = InterlockedDecrement(&m_lNRegisteredItems);

	DEBUG_VERIFY(r >= 0);

	if(!r)
		m_DoneEvent.Set();
}

void TSynchedQueue::Enqueue(TItem* pItem)
{
	DEBUG_VERIFY_ALLOCATION;

	if(m_hTerminator && WaitForSingleObject(m_hTerminator, 0) == WAIT_OBJECT_0)
	{
		delete pItem;
		return;
	}

	{
		TCriticalSectionLocker Locker0(m_AccessCS);	

		*m_Items.AddLast() = pItem;

		m_AvailableEvent.Set();
	}
}

TSynchedQueue::TItem* TSynchedQueue::DequeueOnceAvailable(size_t szTimeout)
{
	DEBUG_VERIFY_ALLOCATION;

	for(;;)
	{
		HANDLE Handles[] = {m_AvailableEvent, m_hTerminator};

		size_t szNHandles = ARRAY_SIZE(Handles);

		if(!m_hTerminator)
			szNHandles--;

		DWORD r = WaitForMultipleObjects((DWORD)szNHandles, Handles, FALSE, (DWORD)szTimeout);

		CHECK_TERMINATION(m_hTerminator);

		if(r == WAIT_TIMEOUT)
			return NULL;

		if(r == WAIT_OBJECT_0 + 0)
		{
			TCriticalSectionLocker Locker0(m_AccessCS);

			if(m_Items.IsEmpty())
				continue;

			TItem* pItem = m_Items.GetFirst()->Extract();

			m_Items.DelFirst();

			if(!m_Items.IsEmpty())
				m_AvailableEvent.Set();

			return pItem;
		}
	}
}

TSynchedQueue::TItem* TSynchedQueue::DequeueUntilDone()
{
	DEBUG_VERIFY_ALLOCATION;

	for(;;)
	{
		HANDLE Handles[] = {m_AvailableEvent, m_DoneEvent, m_hTerminator};

		size_t szNHandles = ARRAY_SIZE(Handles);

		if(!m_hTerminator)
			szNHandles--;

		DWORD r = WaitForMultipleObjects((DWORD)szNHandles, Handles, FALSE, INFINITE);

		CHECK_TERMINATION(m_hTerminator);

		if(r == WAIT_OBJECT_0 + 0)
		{
			TCriticalSectionLocker Locker0(m_AccessCS);

			if(m_Items.IsEmpty())
				continue;

			TItem* pItem = m_Items.GetFirst()->Extract();

			m_Items.DelFirst();

			if(!m_Items.IsEmpty())
				m_AvailableEvent.Set();

			return pItem;
		}

		if(r == WAIT_OBJECT_0 + 1)
			return NULL;
	}
}
