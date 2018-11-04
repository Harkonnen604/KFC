#ifndef win_inc_h
#define win_inc_h

#ifdef _MSC_VER

	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <windows.h>
	#include <mstcpip.h>

#else // _MSC_VER

	#include <pthread.h>
	
	#define TRUE	((DWORD)1)
	#define FALSE	((DWORD)0)

	typedef char		CHAR;
	typedef short		SHORT;
	typedef int			INT;
	typedef long		LONG;

	typedef unsigned char		BYTE;
	typedef unsigned short		WORD;
	typedef unsigned long		DWORD;
	typedef unsigned int		UINT;

	typedef signed char			INT8;
	typedef short				INT16;
	typedef int					INT32;
	typedef long long			INT64;

	typedef unsigned char		UINT8;
	typedef unsigned short		UINT16;
	typedef unsigned int		UINT32;
	typedef unsigned long long	UINT64;	

	#define _I64_MIN	(-9223372036854775807ll - 1)
	#define _I64_MAX	(9223372036854775807ll)

	#define _UI64_MAX	(0xffffffffffffffffull)

	#ifdef _UNICODE
		typedef wchar_t		TCHAR;
		#define TEXT(t)		L##t
	#else // _UNICODE
		typedef char TCHAR;
		#define TEXT(t)		t
	#endif // _UNICODE

	typedef wchar_t WCHAR;

	typedef char*	LPSTR;
	typedef WCHAR*	LPWSTR;
	typedef TCHAR*	LPTSTR;

	typedef const char*		LPCSTR;
	typedef const WCHAR*	LPCWSTR;
	typedef const TCHAR*	LPCTSTR;
	
	typedef void*		LPVOID;
	typedef const void*	LPCVOID;
	
	typedef DWORD	BOOL;

	#define CP_ACP 1251

	#define RGB(r, g, b)	((r) | ((g) << 8) | ((b) << 16))

	#define GetRValue(v)	((BYTE)(((v)      ) & 0xFF))
	#define GetGValue(v)	((BYTE)(((v) >> 8 ) & 0xFF))
	#define GetBValue(v)	((BYTE)(((v) >> 16)	& 0xFF))

	struct FILETIME
	{
		DWORD dwLowDateTime;
		DWORD dwHighDateTime;
	};
	
	struct SYSTEMTIME
	{
		WORD wYear; 
		WORD wMonth; 
		WORD wDayOfWeek; 
		WORD wDay; 
		WORD wHour; 
		WORD wMinute; 
		WORD wSecond; 
		WORD wMilliseconds; 
	};
	
	struct POINT
	{
		int x, y;
	};
	
	struct SIZE
	{
		int cx, cy;
	};
	
	struct RECT
	{
		int left, top, right, bottom;
	};

	#define BI_RGB			(0)
	#define BI_RLE8			(1)
	#define BI_RLE4			(2)
	#define BI_BITFIELDS	(3)
	#define BI_JPEG			(4)
	#define BI_PNG			(5)

	struct RGBQUAD
	{
		BYTE rgbBlue;
		BYTE rgbGreen;
		BYTE rgbRed;
		BYTE rgbReserved;
	};
	
	#pragma pack(2)

	struct BITMAPFILEHEADER
	{ 
		WORD	bfType;
		DWORD	bfSize;
		WORD	bfReserved1;
		WORD	bfReserved2;
		DWORD	bfOffBits;
	};
	
	#pragma pack()

	struct BITMAPINFOHEADER
	{
		DWORD	biSize;
		LONG	biWidth;
		LONG	biHeight;
		WORD	biPlanes;
		WORD	biBitCount;
		DWORD	biCompression;
		DWORD	biSizeImage;
		LONG	biXPelsPerMeter;
		LONG	biYPelsPerMeter;
		DWORD	biClrUsed;
		DWORD	biClrImportant;
	};

	struct BITMAPINFO
	{
		BITMAPINFOHEADER	bmiHeader;
		RGBQUAD				bmiColors[1];
	};

	typedef void* HANDLE;
	typedef void* HINSTANCE;
	typedef void* HMODULE;

	struct HWND__ {};
	typedef HWND__* HWND;

	struct HFILE__ {};
	typedef HFILE__* HFILE;

	struct HEVENT__ {};
	typedef HEVENT__* HEVENT;
	
	inline int stricmp(const char* a, const char* b)
	{
		for( ; *a || *b ; a++, b++)
		{
			if(int d = toupper(*a) - toupper(*b))
				return d;
		}
		
		return 0;
	}

	inline int strnicmp(const char* a, const char* b, size_t n)
	{
		for( ; n && (*a || *b) ; a++, b++, n--)
		{
			if(int d = toupper(*a) - toupper(*b))
				return d;
		}

		return 0;
	}
	
	inline char* strlwr(char* s)
	{
		for( ; *s ; s++)
			*s = tolower(*s);

		return s;
	}
	
	inline char* strupr(char* s)
	{
		for( ; *s ; s++)
			*s = toupper(*s);

		return s;
	}

	inline size_t strnlen(const char* s, size_t n)
	{
		size_t i;

		for(i = 0 ; i < n && s[i] ; i++);

		return i;
	}

	#define _tcscpy			strcpy
	#define	_tcsstr			strstr
	#define _tcschr			strchr
	#define _tcspbrk		strpbrk
	#define _ttoi			atoi
	#define _ttof			atof
	#define _tcsftime		strftime
	#define _tcstok			strtok_r
	#define _tcslen			strlen
	#define _tcscmp			strcmp
	#define _tcsicmp		stricmp
	#define _tcsncmp		strncmp
	#define _tcsnicmp		strnicmp
	#define _tcscoll		strcmp
	#define _tcsicoll		stricmp
	#define _tcsncoll		strncmp
	#define _tcsnicoll		strnicmp
	#define _tfopen			fopen
	#define _trename		rename
	#define _tgetcwd		getcwd
	#define _tmkdir			mkdir
	#define _trmdir			rmdir
	#define _tchdir			chdir
	#define _tprintf		printf
	#define _stprintf		sprintf
	#define _stscanf		sscanf
	#define _ftprintf		fprintf
	#define _ftscanf		fscanf
	#define _vsntprintf		vsnprintf
	#define _putts			puts
	#define _totlower 		tolower
	#define _totupper		toupper
	#define _istupper		isupper
	#define _istlower		islower
	#define _istalpha		isalpha
	#define _istdigit		isdigit
	#define _istalnum		isalnum
	#define _istspace		isspace
	#define _tgetenv		getenv
	#define _tcslwr			strlwr
	#define _tcsupr			strupr
	#define _fgetts			fgets
	#define _fputtc			fputc

	inline int putch(char c)
		{ return putc(c, stdout); }

	inline int CompareFileTime(const FILETIME* pTime1, const FILETIME* pTime2)
	{
		if(pTime1->dwHighDateTime < pTime2->dwHighDateTime)
			return -1;
			
		if(pTime1->dwHighDateTime > pTime2->dwHighDateTime)
			return +1;
			
		if(pTime1->dwLowDateTime < pTime2->dwLowDateTime)
			return -1;
			
		if(pTime1->dwLowDateTime > pTime2->dwLowDateTime)
			return +1;
			
		return 0;
	}
	
	#include "critical_section.h"

	extern TCriticalSection g_InterlockedCS;
	
	extern TConditionVariable g_SyncCV;

	inline LONG InterlockedDecrement(volatile LONG* v)
		{ TCriticalSectionLocker Locker0(g_InterlockedCS); return --*v; }

	inline LONG InterlockedIncrement(volatile LONG* v)
		{ TCriticalSectionLocker Locker0(g_InterlockedCS); return ++*v; }

	inline LONG InterlockedExchange(volatile LONG* v, LONG x)
		{ LONG ret; TCriticalSectionLocker Locker0(g_InterlockedCS); ret = *v, *v = x; return ret; }

	inline LONG InterlockedExchangeAdd(volatile LONG* v, LONG a)
		{ LONG ret; TCriticalSectionLocker Locker0(g_InterlockedCS); ret = *v; *v += a; return ret; }
		
	inline DWORD GetCurrentThreadId()
		{ return 0;/*pthread_getthreadid_np();*/ }
		
	#define INFINITE	((DWORD)-1)
	
	#define WAIT_ABANDONED	(0x00000080L)
	#define WAIT_OBJECT_0	(0X00000000L)
	#define WAIT_TIMEOUT	(0x00000102L)
	#define WAIT_FAILED		(0xFFFFFFFFL)
	
	// Sync object
	class TSyncObject
	{
	public:	
		virtual ~TSyncObject() {}

		virtual bool Acquire() = 0;

		virtual void Revert() = 0;
	};
	
	// Mutex sync object
	class TMutexSyncObject : public TSyncObject
	{
	private:
		TCriticalSection m_CS;
	
	public:
		TMutexSyncObject(bool bCreateOwned = false)
		{
			if(bCreateOwned)
				m_CS.Lock();
		}

		bool Acquire()
			{ return m_CS.TryLock(); }

		void Revert()
			{ m_CS.Unlock(); }

		bool Release()
		{
			TConditionVariableLocker Locker0(g_SyncCV);

			if(!m_CS.Unlock())
				return false;
				
			g_SyncCV.Broadcast();
			
			return true;
		}
	};

	// Event sync object
	class TEventSyncObject : public TSyncObject
	{
	private:
		const bool m_bManualReset;

		bool m_bSignaled;
	
	public:
		TEventSyncObject(bool bManualReset, bool bCreateSignaled) : m_bManualReset(bManualReset)
		{
			m_bSignaled = bCreateSignaled;
		}
			
		bool Acquire()
		{
			if(!m_bSignaled)
				return false;
			
			if(!m_bManualReset)
				m_bSignaled = false;
				
			return true;
		}
		
		void Revert()
		{
			if(m_bManualReset)
			{
				assert(m_bSignaled);
				return;
			}

			assert(!m_bSignaled);
			m_bSignaled = true;
		}
		
		void Set()
		{
			TConditionVariableLocker Locker0(g_SyncCV);
			
			m_bSignaled = true;
			
			g_SyncCV.Broadcast();
		}

		void Reset()
		{
			TConditionVariableLocker Locker0(g_SyncCV);
			
			m_bSignaled = false;
		}
	};

	// Semaphore sync object
	class TSemaphoreSyncObject : public TSyncObject
	{
	private:
		const size_t m_szMaxN;

		size_t m_szN;
		
	public:
		TSemaphoreSyncObject(size_t szMaxN, size_t szN) : m_szMaxN(szMaxN)
		{
			assert(szN <= m_szMaxN);
			
			m_szN = szN;
		}

		bool Acquire()
		{
			if(!m_szN)
				return false;
				
			m_szN--;
			
			return true;
		}
		
		void Revert()
		{
			assert(m_szN < m_szMaxN);
			
			m_szN++;
		}
		
		bool Release(size_t szDelta, size_t* pROldN)
		{
			TConditionVariableLocker Locker0(g_SyncCV);

			if(m_szN + szDelta > m_szMaxN)
				return false;

			if(pROldN)
				*pROldN = m_szN;

			m_szN += szDelta;
			
			g_SyncCV.Broadcast();
			
			return true;
		}
	};
	
	// Waitable timer sync object
/*	class TWaitableTimerSyncObject : public TSyncObject
	{
	private:
		UINT64 m_qwTimeDue; // msec
		
		UINT64 m_qwOldTimeDue; // msec
		
		size_t m_szPeriod; // msec
		
		size_t m_szTimesSignaled;
		
		size_t m_szOldTimesSignaled;

		bool m_bSignaled;
		
		bool m_bOldSignaled;
		
		const bool m_bManualReset;

	public:
		TWaitableTimerSyncObject(bool bManualReset);

		bool Acquire();
		
		void Revert();

		void Set(UINT64 qwTimeDue, size_t szPeriod); // all msec

		void Cancel();
		
		UINT64 GetTimeDue() const
		{
			TConditionVariableLocker Locker0(g_SyncCV);

			return m_qwTimeDue;
		}
			
		bool UpdateTimeDue(UINT64 qwTime)
		{
			TConditionVariableLocker Locker0(g_SyncCV);
		
			if(m_qwTimeDue > qwTime)
				return false;
				
			m_szTimesSignaled += (qwTime - m_qwTimeDue) / m_szPeriod + 1;
			
			m_qwTimeDue += (UINT64)m_szTimesSignaled * m_szPeriod;
				
			g_SyncCV.Broadcast();
			
			return true;
		}
	};*/
	
	// Thread sync object
	class TThreadSyncObject : public TSyncObject
	{
	private:
		pthread_t m_Thread;
		
		#ifdef _DEBUG
			volatile LONG m_lJoinCount;
		#endif // _DEBUG
		
	public:
		TThreadSyncObject(pthread_t Thread) : m_Thread(Thread)
		{
			#ifdef _DEBUG
				m_lJoinCount = 0;
			#endif // _DEBUG
		}
		
		~TThreadSyncObject()
		{
			if(m_Thread)
				pthread_detach(m_Thread), m_Thread = 0;
		}

		bool Acquire()
			{ return true; }
			
		void Revert() {}
			
		void Join()
		{
			#ifdef _DEBUG
				assert(InterlockedIncrement(&m_lJoinCount) == 1);
			#endif // _DEBUG
		
			assert(m_Thread);
			
			pthread_join(m_Thread, NULL), m_Thread = 0;
		}
	};

	// simple mkdir
	inline int mkdir(const char* pName)
		{ return mkdir(pName, 0775); }

	// msec_time
	inline unsigned long long msec_time()
	{
		timeval tv;
		gettimeofday(&tv, NULL);

		return (unsigned long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}

	// CloseHandle
	inline void CloseHandle(HANDLE hObject)
		{ if(hObject) delete (TSyncObject*)hObject; }
		
	// SECURITY_ATTRIBUTES
	struct SECURITY_ATTRIBUTES
	{
		DWORD	nLength;
		LPVOID	lpSecurityDescriptor;
  		BOOL	bInheritHandle;
  	};
  	
  	// LARGE_INTEGER
  	union LARGE_INTEGER
  	{
/* 		struct
  		{
  			DWORD LowPart;
  			DWORD HighPart;
  		};*/

  		struct
  		{
  			DWORD LowPart;
  			DWORD HighPart;
  		}u;
 
  		INT64 QuadPart;
  	};
  	
  	// Mutex API
  	inline HANDLE CreateMutex(SECURITY_ATTRIBUTES* pSecurity, BOOL bInitialOwner, LPCTSTR pName)
  	{
  		return (HANDLE)new TMutexSyncObject(bInitialOwner);
  	}
  	
  	inline BOOL ReleaseMutex(HANDLE hMutex)
  	{
  		if(!hMutex)
  			return FALSE;
  			
  		assert(dynamic_cast<TMutexSyncObject*>((TSyncObject*)hMutex));
  
  		return ((TMutexSyncObject*)hMutex)->Release();
  	}
  	
  	// Event API
  	inline HANDLE CreateEvent(SECURITY_ATTRIBUTES* pSecurity, BOOL bManualReset, BOOL bInitialState, LPCTSTR pName)
  	{
  		return (HANDLE)new TEventSyncObject(bManualReset, bInitialState);
  	}
 
  	inline BOOL SetEvent(HANDLE hEvent)
  	{
  		if(!hEvent)
  			return FALSE;
  			
  		assert(dynamic_cast<TEventSyncObject*>((TSyncObject*)hEvent));
  		
  		((TEventSyncObject*)hEvent)->Set();
  		
  		return TRUE;
  	}
  	
  	inline BOOL ResetEvent(HANDLE hEvent)
  	{
  		if(!hEvent)
  			return FALSE;
  			
  		assert(dynamic_cast<TEventSyncObject*>((TSyncObject*)hEvent));
  		
  		((TEventSyncObject*)hEvent)->Reset();
  		
  		return TRUE;
  	}
  	
  	// Semaphore API
  	inline HANDLE CreateSemaphore(SECURITY_ATTRIBUTES* pSecurity, LONG lInitialCount, LONG lMaximumCount, LPCTSTR pName)
  	{
  		if((DWORD)lInitialCount > (DWORD)lMaximumCount)
  			return NULL;
  	
  		return (HANDLE)new TSemaphoreSyncObject(lMaximumCount, lInitialCount);
  	}
  	
  	inline BOOL ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LONG* pPreviousCount)
  	{
  		if(!hSemaphore)
  			return FALSE;
  			
  		assert(dynamic_cast<TSemaphoreSyncObject*>((TSyncObject*)hSemaphore));

  		return ((TSemaphoreSyncObject*)hSemaphore)->Release(lReleaseCount, (size_t*)pPreviousCount);
  	}
  
  	// Waitable timer API
/*  HANDLE CreateWaitableTimer(SECURITY_ATTRIBUTES* pSecurity, BOOL bManualReset, LPCTSTR pName);
  	
  	BOOL SetWaitableTimer(	HANDLE					hTimer,
							const LARGE_INTEGER*	pDueTime,
							LONG					lPeriod,
							void*					pCompletionRoutine,
							void*					pCompletionArg,
							BOOL					fResume);
  	
  	BOOL CancelWaitableTimer(HANDLE hTimer);*/

	// Waiters
	void Sleep(DWORD dwTime);

	DWORD WaitForSingleObject(HANDLE hObject, DWORD dwTimeout);

	DWORD WaitForMultipleObjects(DWORD dwN, const HANDLE* pObjects, BOOL bWaitAll, DWORD dwTimeout);
	
	// TLS API
	#define TLS_OUT_OF_INDEXES	((DWORD)0xFFFFFFFF)
	
	inline DWORD TlsAlloc()
	{	
		pthread_key_t key;
		
		if(pthread_key_create(&key, NULL))
			return TLS_OUT_OF_INDEXES;
			
		return (const DWORD&)key;
	}
	
	inline BOOL TlsFree(DWORD dwIndex)
	{
		if(pthread_key_delete((const pthread_key_t&)dwIndex))
			return FALSE;
		
		return TRUE;
	}

	inline LPVOID TlsGetValue(DWORD dwIndex)
	{
		return pthread_getspecific((const pthread_key_t&)dwIndex);
	}
	
	inline BOOL TlsSetValue(DWORD dwIndex, LPVOID pValue)
	{
		pthread_setspecific((const pthread_key_t&)dwIndex, pValue);
		
		return TRUE;
	}

	// Last error
	#define S_OK					((DWORD)0)
	#define S_FAIL					((DWORD)1)
	#define ERROR_ALREADY_EXISTS	((DWORD)183)
	
	inline DWORD GetLastError()
		{ return 0; }

#endif // _MSC_VER

#define R_RGB(v)	GetRValue(v)
#define G_RGB(v)	GetGValue(v)
#define B_RGB(v)	GetBValue(v)

#endif // win_inc_h
