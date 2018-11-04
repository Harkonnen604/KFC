#ifndef process_h
#define process_h

// ------------
// Thread proc
// ------------
#ifdef _MSC_VER
	#define DECLARE_THREAD_PROC(ProcName, ParamName)					UINT __stdcall ProcName(void* ParamName)
	#define DECLARE_MEMBER_THREAD_PROC(ClassName, ProcName, ParamName)	UINT __stdcall ClassName::ProcName(void* ParamName)
#else // _MSC_VER
	#define DECLARE_THREAD_PROC(ProcName, ParamName)					void* ProcName(void* ParamName)
	#define DECLARE_MEMBER_THREAD_PROC(ClassName, ProcName, ParamName)	void* ClassName::ProcName(void* ParamName)
#endif // _MSC_VER

typedef DECLARE_THREAD_PROC(TThreadProc, pParam);

// -------------
// Thread guard
// -------------
class TThreadGuard
{
private:
	volatile LONG& m_lNThreads;

public:
	TThreadGuard(volatile LONG& lNThreads) : m_lNThreads(lNThreads) {}

	~TThreadGuard()
		{ InterlockedDecrement(&m_lNThreads); }
};

// ----------------
// Global routines
// ----------------
HANDLE StartThread(	TThreadProc*	pThreadProc,
					void*			pParam,
					bool			bSuspended	= false,
					UINT*			pRID		= NULL,
					DWORD*			pRError		= NULL);

inline void RunThread(	TThreadProc*	pThreadProc,
						void*			pParam)
{
	DEBUG_VERIFY(pThreadProc);

	pThreadProc(pParam);
}

void StartGuardedThread(volatile LONG&	lNThreads,
						TThreadProc*	pThreadProc,
						void*			pParam,
						bool			bSuspended	= false,
						UINT*			pRID		= NULL,
						DWORD*			pRError		= NULL);

inline void RunGuardedThread(	volatile LONG&	lNThreads,
								TThreadProc*	pThreadProc,
								void*			pParam)
{
	DEBUG_VERIFY(pThreadProc);

	InterlockedIncrement(&lNThreads);

	pThreadProc(pParam);
}

void WaitGuardedThreads(volatile LONG& lNThreads, size_t szPollDelay = DEFAULT_POLL_DELAY);

#ifdef _MSC_VER

struct TStartupInfo : public STARTUPINFO
{
public:
	TStartupInfo()
		{ memset(this, 0, sizeof(*this)), cb = sizeof(*this); }
};

DWORD StartProcess(	const KString&	FileName,
					const KString&	Params,
					const KString&	FolderName		= ".",
					bool			bInheritHandles	= false,
					DWORD			dwFlags			= 0,
					STARTUPINFO&	StartupInfo		= temp<TStartupInfo>()(),
					HANDLE*			pRProcess		= NULL,
					HANDLE*			pRThread		= NULL,
					DWORD*			pRError			= NULL);

DWORD StartProcessAtUser(const KString&	FileName,
						 const KString&	Params,
						 const KString&	FolderName,
						 LPCWSTR		pUser,
						 LPCWSTR		pDomain,
						 LPCWSTR		pPassword,
						 HANDLE*		pRProcess	= NULL,
						 HANDLE*		pRThread	= NULL,
						 DWORD*			pRError		= NULL);

void OpenDocument(LPCTSTR pName);

#endif // _MSC_VER

// -----------------------
// Guarded threads waiter
// -----------------------
class TGuardedThreadsWaiter
{
private:
	volatile LONG& m_lNThreads;

public:
	TGuardedThreadsWaiter(volatile LONG& lNThreads) : m_lNThreads(lNThreads) {}

	~TGuardedThreadsWaiter()
		{ WaitGuardedThreads(m_lNThreads); }
};

#endif // process_h
