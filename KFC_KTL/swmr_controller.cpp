#include "kfc_ktl_pch.h"
#include "swmr_controller.h"

// ----------------
// SWMR controller
// ----------------

// Item
T_SWMR_Controller::TItem::TItem()
{
	m_GrantedEvent.Allocate(false, false);
	m_FreedEvent.  Allocate(true,  false);
}

// SWMR controller
T_SWMR_Controller::T_SWMR_Controller(size_t szMaxThreads)
{
	if(szMaxThreads)
		Allocate(szMaxThreads);
}

void T_SWMR_Controller::Release()
{
	#ifdef _DEBUG
		if(IsAllocated())
		{
			TCriticalSectionLocker Locker0(m_AccessCS);

			assert(!m_szTimesReadLocked);
			assert(!m_bWriteLocked);
			assert(!m_szN);
		}
	#endif // _DEBUG

	m_Items.Clear();
}

void T_SWMR_Controller::Allocate(size_t szMaxThreads)
{
	Release();

	DEBUG_VERIFY(szMaxThreads);

	m_Items.SetN(szMaxThreads);

	m_szHead = m_szTail = 0;
	m_szN = 0;

	m_szTimesReadLocked = 0;
	m_bWriteLocked = false;
}

bool T_SWMR_Controller::TryLockRead()
{
	DEBUG_VERIFY_ALLOCATION;

	TCriticalSectionLocker Locker0(m_AccessCS);

	DEBUG_VERIFY(m_szN < m_Items.GetN());

	if(!m_bWriteLocked && !m_szN)
	{
		m_szTimesReadLocked++;
		return true;
	}

	return false;
}

void T_SWMR_Controller::LockRead()
{
	DEBUG_VERIFY_ALLOCATION;

	size_t szWaitIndex;

	{
		TCriticalSectionLocker Locker0(m_AccessCS);

		DEBUG_VERIFY(m_szN < m_Items.GetN());

		if(!m_bWriteLocked && !m_szN)
		{
			m_szTimesReadLocked++;
			return;
		}

		szWaitIndex = m_szTail;

		m_Items[szWaitIndex].m_FreedEvent.Wait();

		m_Items[szWaitIndex].m_bWriter = false;

		if(++m_szTail == m_Items.GetN())
			m_szTail = 0;

		m_szN++;
	}

	m_Items[szWaitIndex].m_GrantedEvent.Wait();

	DEBUG_VERIFY(!m_Items[szWaitIndex].m_FreedEvent.Wait(0));
	m_Items[szWaitIndex].m_FreedEvent.Set();
}

void T_SWMR_Controller::UnlockRead()
{
	DEBUG_VERIFY_ALLOCATION;

	TCriticalSectionLocker Locker0(m_AccessCS);

	DEBUG_VERIFY(m_szN < m_Items.GetN());

	DEBUG_VERIFY(m_szTimesReadLocked);
	DEBUG_VERIFY(!m_bWriteLocked);

	m_szTimesReadLocked--;

	if(m_szTimesReadLocked || !m_szN)
		return;

	DEBUG_VERIFY(m_Items[m_szHead].m_bWriter);

	DEBUG_VERIFY(!m_Items[m_szHead].m_GrantedEvent.Wait(0));
	m_Items[m_szHead].m_GrantedEvent.Set();

	m_bWriteLocked = true;

	if(++m_szHead == m_Items.GetN())
		m_szHead = 0;

	m_szN--;
}

bool T_SWMR_Controller::TryLockWrite()
{
	DEBUG_VERIFY_ALLOCATION;

	TCriticalSectionLocker Locker0(m_AccessCS);

	DEBUG_VERIFY(m_szN < m_Items.GetN());

	if(!m_szTimesReadLocked && !m_bWriteLocked && !m_szN)
	{
		m_bWriteLocked = true;
		return true;
	}

	return false;
}

void T_SWMR_Controller::LockWrite()
{
	DEBUG_VERIFY_ALLOCATION;

	size_t szWaitIndex;

	{
		TCriticalSectionLocker Locker0(m_AccessCS);

		DEBUG_VERIFY(m_szN < m_Items.GetN());

		if(!m_szTimesReadLocked && !m_bWriteLocked && !m_szN)
		{
			m_bWriteLocked = true;
			return;
		}

		szWaitIndex = m_szTail;

		m_Items[szWaitIndex].m_FreedEvent.Wait();

		m_Items[szWaitIndex].m_bWriter = true;

		if(++m_szTail == m_Items.GetN())
			m_szTail = 0;

		m_szN++;
	}

	m_Items[szWaitIndex].m_GrantedEvent.Wait();

	DEBUG_VERIFY(!m_Items[szWaitIndex].m_FreedEvent.Wait(0));
	m_Items[szWaitIndex].m_FreedEvent.Set();
}

void T_SWMR_Controller::UnlockWrite()
{
	DEBUG_VERIFY_ALLOCATION;

	TCriticalSectionLocker Locker0(m_AccessCS);

	DEBUG_VERIFY(!m_szTimesReadLocked);
	DEBUG_VERIFY(m_bWriteLocked);

	m_bWriteLocked = false;

	if(!m_szN)
		return;

	if(m_Items[m_szHead].m_bWriter)
	{
		DEBUG_VERIFY(!m_Items[m_szHead].m_GrantedEvent.Wait(0));
		m_Items[m_szHead].m_GrantedEvent.Set();

		m_bWriteLocked = true;

		if(++m_szHead == m_Items.GetN())
			m_szHead = 0;

		m_szN--;
	}
	else
	{
		do
		{
			DEBUG_VERIFY(!m_Items[m_szHead].m_GrantedEvent.Wait(0));
			m_Items[m_szHead].m_GrantedEvent.Set();

			m_szTimesReadLocked++;

			if(++m_szHead == m_Items.GetN())
				m_szHead = 0;

			m_szN--;

		}while(m_szN && !m_Items[m_szHead].m_bWriter);
	}
}
