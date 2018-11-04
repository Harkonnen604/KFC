#ifndef termination_h
#define termination_h

// Delays
#define DEFAULT_POLL_DELAY	(250)

// Speed defs
#define CHECK_TERMINATION(hTerminator) \
	CheckTermination(hTerminator, __FILE__, __LINE__)

#define CHECK_CUSTOM_TERMINATION(hTerminator, szDelay) \
	CheckTermination(hTerminator, __FILE__, __LINE__, szDelay)

#define CHECK_POLLED_TERMINATION(hTerminator) \
	CHECK_CUSTOM_TERMINATION(hTerminator, DEFAULT_POLL_DELAY)
	
#define TERMINATE \
	throw TTerminationException(__FILE__, __LINE__)

// ----------------------
// Termination exception
// ----------------------
struct TTerminationException
{
public:
	LPCTSTR	m_pFile;
	int		m_iLine;
	
public:
	TTerminationException(LPCTSTR pSFile, int iSLine) :
		m_pFile(pSFile), m_iLine(iSLine) {}
};

// ----------------
// Global routines
// ----------------
bool IsTerminated(HANDLE hTerminator, size_t szTimeout = 0);

inline void CheckTermination(HANDLE hTerminator, LPCTSTR pFile, int iLine, size_t szTimeout = 0)
{
	if(IsTerminated(hTerminator, szTimeout))
		throw TTerminationException(pFile, iLine);
}

#endif // termination_h
