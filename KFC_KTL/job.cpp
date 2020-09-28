#include "kfc_ktl_pch.h"
#include "job.h"

// -------------
// Jobs manager
// -------------
TJobsManager::TJobsManager(bool bAllocate, HANDLE hTerminator)
{
	m_bAllocated = 0;

	if(bAllocate)
		Allocate(hTerminator);
}

void TJobsManager::Release()
{
	m_bAllocated = false;

	m_Jobs.Clear();

	m_DoneEvent.Release();

	m_AvailableEvent.Release();
}

void TJobsManager::Allocate(HANDLE hTerminator)
{
	Release();

	try
	{
		m_AvailableEvent.Allocate(false, false);

		m_DoneEvent.Allocate(true, true);

		m_lNJobs = 0;

		m_hTerminator = NULL;

		m_bAllocated = true;

		if(hTerminator)
			SetTerminator(hTerminator);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TJobsManager::RegisterJob()
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_EVERIFY(InterlockedIncrement(&m_lNJobs) > 0);

	m_DoneEvent.Reset();
}

void TJobsManager::UnregisterJob()
{
	if(!IsAllocated())
		return;

	const LONG r = InterlockedDecrement(&m_lNJobs);

	DEBUG_VERIFY(r >= 0);

	if(!r)
		m_DoneEvent.Set();
}

bool TJobsManager::Enqueue(TJob* pJob, bool bDelIfTerminated)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(pJob);

	DEBUG_VERIFY(&pJob->GetJobsManager() == this);

	if(	bDelIfTerminated &&
		m_hTerminator && WaitForSingleObject(m_hTerminator, 0) == WAIT_OBJECT_0)
	{
		delete pJob;
		return false;
	}

	{
		TCriticalSectionLocker Locker0(m_AccessCS);	

		*m_Jobs.AddLast() = pJob;

		m_AvailableEvent.Set();
	}

	return true;
}

TJob* TJobsManager::Dequeue(bool	bStopOnDone,
							size_t	szTimeout,
							HANDLE*	pExtraEvents,
							size_t	szNExtraEvents)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!(szNExtraEvents && !pExtraEvents));

	HANDLE FewHandles[8];

	TArray<HANDLE, true> ManyHandles;
	
	HANDLE*	pHandles;
	size_t	szN;

	size_t szDoneIndex			= UINT_MAX;
	size_t szTerminatorIndex	= UINT_MAX;
	size_t szExtraIndexBase;

	// Considering 'bStopOnDone' and 'm_hTerminator' would be more accurate, but slower
	if(szNExtraEvents <= ARRAY_SIZE(FewHandles) - 3)
	{
		szN = 0;

		FewHandles[szN++] = m_AvailableEvent;

		if(bStopOnDone)
			FewHandles[szDoneIndex = szN++] = m_DoneEvent;

		if(m_hTerminator)
			FewHandles[szTerminatorIndex = szN++] = m_hTerminator;

		szExtraIndexBase = szN;
		memcpy(FewHandles + szN, pExtraEvents, szNExtraEvents * sizeof(HANDLE));
		szN += szNExtraEvents;

		pHandles = FewHandles;
	}
	else
	{
		ManyHandles.Add() = m_AvailableEvent;

		if(bStopOnDone)
			szDoneIndex = ManyHandles.GetN(), ManyHandles.Add() = m_DoneEvent;

		if(m_hTerminator)
			szTerminatorIndex = ManyHandles.GetN(), ManyHandles.Add() = m_hTerminator;

		szExtraIndexBase = ManyHandles.GetN();
		memcpy(&ManyHandles.Add(szNExtraEvents), pExtraEvents, szNExtraEvents * sizeof(HANDLE));

		pHandles = ManyHandles.GetDataPtr(), szN = ManyHandles.GetN();
	}

	for(;;)
	{
		DWORD r = WaitForMultipleObjects((DWORD)szN, pHandles, FALSE, (DWORD)szTimeout);

		CHECK_TERMINATION(m_hTerminator);

		if(r == WAIT_TIMEOUT)
			return TIMEOUT_JOB;

		if(r == WAIT_OBJECT_0 + 0) // available
		{
			TCriticalSectionLocker Locker0(m_AccessCS);

			if(m_Jobs.IsEmpty())
				continue;

			TJob* pJob = m_Jobs.GetFirst()->Extract();

			m_Jobs.DelFirst();

			if(!m_Jobs.IsEmpty())
				m_AvailableEvent.Set();

			return pJob;
		}
		else if(r == WAIT_OBJECT_0 + szDoneIndex)
		{
			return DONE_JOB;
		}
		else if(r == WAIT_OBJECT_0 + szTerminatorIndex)
		{
			INITIATE_FAILURE;
		}
		else
		{
			return EXTRA_EVENT_JOB(r - WAIT_OBJECT_0 - szExtraIndexBase);
		}
	}
}
