#ifndef service_h
#define service_h

#ifdef _MSC_VER

// ----------------
// Service manager
// ----------------
class TServiceManager
{
private:
	SC_HANDLE m_hManager;
	
public:
	TServiceManager(kflags_t flAutoOpenAccess = 0); // 0 avoids auto-allocation

	~TServiceManager() { Release(); }

	bool IsAllocated() const
		{ return m_hManager; }

	void Release();

	void Allocate(kflags_t flAccess);

	SC_HANDLE CreateService(LPCTSTR pServiceName,
							LPCTSTR pDisplayName,
							LPCTSTR pFileName);

	SC_HANDLE OpenService(	LPCTSTR		pServiceName,
							kflags_t	flAccess);

	void DeleteService(SC_HANDLE hService, bool bSafe);

	SC_HANDLE GetManager() const;

	operator SC_HANDLE () const { return GetManager(); }
};

// --------
// Service
// --------
class TService
{
private:
	SC_HANDLE m_hService;
	
public:
	TService();

	TService(SC_HANDLE hSService);

	~TService() { Release(); }	

	bool IsAllocated() const
		{ return m_hService; }

	void Invalidate();

	void Release();

	void Allocate(SC_HANDLE hSService);

	void Start();

	void Stop(bool bSafe);

	SC_HANDLE GetService() const;

	operator SC_HANDLE () const { return GetService(); }
};

#endif // _MSC_VER

#endif // service_h
