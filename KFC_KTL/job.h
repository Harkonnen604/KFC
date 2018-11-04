#ifndef job_h
#define job_h

#include "critical_section.h"
#include "event.h"

// Job
class TJob;

// Jobs
typedef TList<TPtrHolder<TJob> > TJobs;

// Special JOB pointers (aligned, past 2Gb boundary for 32-bit)
#define DONE_JOB			((TJob*)(0xFFFF0000 + sizeof(TJob) * (0)))
#define TIMEOUT_JOB			((TJob*)(0xFFFF0000 + sizeof(TJob) * (1)))
#define EXTRA_EVENT_JOB(n)	((TJob*)(0xFFFF0000 + sizeof(TJob) * (2 + (n))))

// -------------
// Jobs manager
// -------------
class TJobsManager
{
private:
	bool m_bAllocated;	

	volatile long m_lNJobs;

	mutable TCriticalSection m_AccessCS;

	TEvent m_AvailableEvent;

	TEvent m_DoneEvent;		

	TJobs m_Jobs;

	HANDLE m_hTerminator;

public:
	TJobsManager(bool bAllocate = false, HANDLE hTerminator = NULL);

	~TJobsManager()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(HANDLE hTerminator = NULL);

	void RegisterJob();

	void UnregisterJob();

	bool Enqueue(TJob* pJob, bool bDelIfTerminated);

	TJob* Dequeue(	bool	bStopOnDone,
					size_t	szTimeout		= INFINITE,
					HANDLE*	pExtraEvents	= NULL,
					size_t	szNExtraEvents	= 0);

	size_t GetN() const
	{
		DEBUG_VERIFY_ALLOCATION;

		TCriticalSectionLocker Locker0(m_AccessCS);

		return m_Jobs.GetN();
	}

	bool IsEmpty() const
		{ return !GetN(); }

	HANDLE GetTerminator() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hTerminator; }

	void SetTerminator(HANDLE hTerminator)
		{ DEBUG_VERIFY_ALLOCATION; m_hTerminator = hTerminator; }

	HANDLE GetAvailableEvent() const
		{ DEBUG_VERIFY_ALLOCATION; return m_AvailableEvent; }

	HANDLE GetDoneEvent() const
		{ DEBUG_VERIFY_ALLOCATION; return m_DoneEvent; }
};

//-----
// Job
//-----
class TJob
{
public:
	// Constructor arg
	struct TConstructorArg
	{
	public:
		TJobsManager& m_JobsManager;

	public:
		TConstructorArg(TJobsManager& JobsManager) : m_JobsManager(JobsManager) {}
	};

private:
	TJob(const TJob&);

	TJob& operator = (const TJob&);

protected:
	TJobsManager& m_JobsManager;

public:
	TJob(TJobsManager& JobsManager) : m_JobsManager(JobsManager)
		{ m_JobsManager.RegisterJob(); }

	TJob(const TConstructorArg& Arg) : m_JobsManager(Arg.m_JobsManager)
		{ m_JobsManager.RegisterJob(); }

	virtual ~TJob()
		{ m_JobsManager.UnregisterJob(); }

	TJobsManager& GetJobsManager() const
		{ return m_JobsManager; }

	bool EnqueueSelf(bool bDelIfTermianted)
		{ return m_JobsManager.Enqueue(this, bDelIfTermianted); }
};

#endif // job_h
