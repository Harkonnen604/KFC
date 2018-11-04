#ifndef dde_h
#define dde_h

#ifdef _MSC_VER

// Server return results
#define KFC_DDE_SERVER_OK							(0)
#define KFC_DDE_SERVER_ERROR_INVALID_HANDLE			(-1)
#define KFC_DDE_SERVER_ERROR_UNSUPPORTED_FORMAT		(1)
#define KFC_DDE_SERVER_ERROR_ITEM_NOT_FOUND			(2)
#define KFC_DDE_SERVER_ERROR_INVALID_LENGTH			(3)
#define KFC_DDE_SERVER_ERROR_NOT_ADVISED			(4)
#define KFC_DDE_SERVER_ERROR_REJECTED				(5)

// -----------
// DDE server
// -----------
class T_DDE_Server
{
private:
	static ATOM ms_aListenerAtom;
	static ATOM ms_aConnectionAtom;

	static ATOM ms_aConnectionTypePropName;
	static ATOM ms_aClientWndPropName;
	static ATOM ms_aTerminatingPropName;

public:
	static void UnregisterWindowClasses();

	static void RegisterWindowClasses();

private:
	// Windows
	typedef T_AVL_Storage<HWND> TWindows;	

	// Item
	struct TItem
	{
	public:
		KString	m_Name;

		KString	m_Value;

	public:
		TItem(LPCTSTR pName) : m_Name(pName) {}
	};

	friend inline int Compare(const TItem& Item, const LPCTSTR pName);

	// Items
	typedef T_AVL_Storage<TItem> TItems;

	// Advise link
	struct TAdviseLink
	{
	public:
		HWND m_hServerWnd;
		HWND m_hClientWnd;
		bool m_bHot;
		bool m_bAckReq;
	};

	// Advise links
	typedef TList<TAdviseLink> TAdviseLinks;

	// Advise link WI key
	struct TAdviseLinkWI_Key
	{
	public:
		HWND				m_hServerWnd;
		TItems::TIterator	m_IIter;

		TAdviseLinks::TIterator m_LIter;

	public:
		TAdviseLinkWI_Key(	HWND				hServerWnd,
							TItems::TIterator	IIter) :

			m_hServerWnd(hServerWnd),
			m_IIter		(IIter) {}
	};

	friend inline int Compare(const TAdviseLinkWI_Key& Key, HWND hServerWnd);

	friend inline int Compare(	const TAdviseLinkWI_Key& Key1,
								const TAdviseLinkWI_Key& Key2);

	// Advise link WI keys
	typedef T_AVL_Storage<TAdviseLinkWI_Key> TAdviseLinkWI_Keys;

	// Advise link IW key
	struct TAdviseLinkIW_Key
	{
	public:
		TItems::TIterator	m_IIter;
		HWND				m_hServerWnd;

		TAdviseLinks::TIterator m_LIter;

	public:
		TAdviseLinkIW_Key(	TItems::TIterator	IIter,
							HWND				hServerWnd) :

			m_IIter		(IIter),
			m_hServerWnd(hServerWnd) {}
	};

	friend inline int Compare(	const TAdviseLinkIW_Key&	Key,
								TItems::TIterator			IIter);

	friend inline int Compare(	const TAdviseLinkIW_Key& Key1,
								const TAdviseLinkIW_Key& Key2);

	// Advise link IW keys
	typedef T_AVL_Storage<TAdviseLinkIW_Key> TAdviseLinkIW_Keys;

	// Connection type
	struct TConnectionType
	{
	public:
		// Key
		struct TKey
		{
		public:
			KString m_Application;
			KString m_Topic;

		public:
			TKey(LPCTSTR pApplication, LPCTSTR pTopic) :
				m_Application	(pApplication),
				m_Topic			(pTopic) {}
		};

	public:
		KString m_Application;
		KString m_Topic;

		TItems m_Items;

		TWindows m_Servers;

		TAdviseLinks		m_AdviseLinks;
		TAdviseLinkWI_Keys	m_AdviseLinkWI_Keys;
		TAdviseLinkIW_Keys	m_AdviseLinkIW_Keys;

	public:
		TConnectionType(const TKey& Key) :
			m_Application	(Key.m_Application),
			m_Topic			(Key.m_Topic) {}
	};

	friend inline int Compare(	const TConnectionType&			CT,
								const TConnectionType::TKey&	Key);

	friend inline int Compare(	const TConnectionType&	CT,
								LPCTSTR					pApplication);

	// Connection types
	typedef T_AVL_Storage<TConnectionType> TConnectionTypes;	

private:
	void DisconnectAll(TConnectionTypes::TIterator CTIter);

	HWND Connect(TConnectionTypes::TIterator CTIter, HWND hClientWnd);

	static bool UnadviseAll(TConnectionTypes::TIterator CTIter, HWND hServerWnd);

	static bool UnadviseAll(TConnectionTypes::TIterator CTIter, TItems::TIterator IIter);

	static bool SendAck(HWND hServerWnd,
						HWND hClientWnd,
						ATOM aName,
						WORD wRetCode);

	static bool SendData(	HWND	hServerWnd,
							HWND	hClientWnd,
							ATOM	aName,
							LPCTSTR	pString,
							bool	bResponse,
							bool	bAckReq);

private:
	static LRESULT CALLBACK StaticListenerWindowProc
		(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	LRESULT ListenerWindowProc
		(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK StaticConnectionWindowProc
		(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	LRESULT ConnectionWindowProc
		(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hWnd;

	TConnectionTypes m_ConnectionTypes;

public:
	// Returned value affects reply only.
	// Item's stored value should be set explicitly if needed.
	bool (*m_pOnSetItem)(	LPCTSTR	pApplication,
							LPCTSTR	pTopic,
							LPCTSTR	pName,
							LPCTSTR	pValue,
							void*	pParam);

	void* m_pCallbacksParam;

public:
	T_DDE_Server(bool bAllocate = false);

	~T_DDE_Server()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hWnd; }

	void Release();

	void Allocate();

	// Returns 'true' if value has changed or notification was forced
	bool SetItem(	LPCTSTR pApplication,
					LPCTSTR pTopic,
					LPCTSTR pName,
					LPCTSTR pValue,
					bool	bForceNotify = false);

	// Returns 'true' if value existed
	bool RemoveItem(LPCTSTR	pApplication,
					LPCTSTR	pTopic,
					LPCTSTR	pName,
					bool	bDisconnectIfEmpty = true);

	bool HasItem(	LPCTSTR pApplication,
					LPCTSTR pTopic,
					LPCTSTR pName) const;

	void TestPeers();
};

// Advise link WI key
inline int Compare(const T_DDE_Server::TAdviseLinkWI_Key& Key, HWND hServerWnd)
{
	return Compare(Key.m_hServerWnd, hServerWnd);
}

inline int Compare(	const T_DDE_Server::TAdviseLinkWI_Key& Key1,
					const T_DDE_Server::TAdviseLinkWI_Key& Key2)
{
	int d;

	if(d = Compare(Key1.m_hServerWnd, Key2.m_hServerWnd))
		return d;

	if(d = Compare(Key1.m_IIter, Key2.m_IIter))
		return d;

	return 0;
}

// Advise link IW key
inline int Compare(	const T_DDE_Server::TAdviseLinkIW_Key&	Key,
					T_DDE_Server::TItems::TIterator			IIter)
{
	return Compare(Key.m_IIter, IIter);
}

inline int Compare(	const T_DDE_Server::TAdviseLinkIW_Key& Key1,
					const T_DDE_Server::TAdviseLinkIW_Key& Key2)
{
	int d;	

	if(d = Compare(Key1.m_IIter, Key2.m_IIter))
		return d;

	if(d = Compare(Key1.m_hServerWnd, Key2.m_hServerWnd))
		return d;

	return 0;
}

// Item
inline int Compare(const T_DDE_Server::TItem& Item, LPCTSTR pName)
{
	return CompareNoCase(Item.m_Name, pName);
}

// Connection type
inline int Compare(	const T_DDE_Server::TConnectionType&	CT,
					LPCTSTR									pApplication)
{
	return CompareNoCase(CT.m_Application, pApplication);
}

inline int Compare(	const T_DDE_Server::TConnectionType&		CT,
					const T_DDE_Server::TConnectionType::TKey&	Key)
{
	int d;
	
	if(d = CompareNoCase(CT.m_Application, Key.m_Application))
		return d;

	if(d = CompareNoCase(CT.m_Topic, Key.m_Topic))
		return d;

	return 0;
}

// -----------
// DDE client
// -----------
class T_DDE_Client
{
private:
	static ATOM ms_aAtom;

public:
static void UnregisterWindowClass();

	static void RegisterWindowClass();

private:
	KString m_Application;
	KString m_Topic;

	HWND m_hWnd;

	HWND m_hPeerWnd;

	bool m_bTerminating;

	KString	m_WaitItem;
	bool	m_bSuccess;
	KString	m_WaitData;	

	KString m_Data;

	HWND m_hConnectPeerWnd;

	WORD m_wResult;

private:
	struct TValue
	{
	public:
		KString m_Item;
		KString m_Data;

	public:
		TValue& Set(LPCTSTR pItem, LPCTSTR pData)
		{
			m_Item = pItem, m_Data = pData;

			return *this;
		}
	};

	TList<TValue> m_Values;

private:
	static LRESULT CALLBACK StaticWindowProc
		(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	LRESULT WindowProc
		(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

public:
	typedef void TOnTerminate(	LPCTSTR	pApplication,
								LPCTSTR	pTopic,
								void*	pParam);

	typedef void TOnData(	LPCTSTR	pApplication,
							LPCTSTR	pTopic,
							LPCTSTR	pItem,
							LPCTSTR	pData,
							void*	pParam);

public:
	TOnTerminate* m_pOnTerminate;
	void* m_pOnTerminateParam;

	TOnData* m_pOnData;
	void* m_pOnDataParam;

private:
	void Init();

public:
	T_DDE_Client();

	T_DDE_Client(LPCTSTR pApplication, LPCTSTR pTopic, bool bConnect = false);

	~T_DDE_Client()
		{ Release(); }

	bool IsAllocated() const
		{ return !m_Application.IsEmpty(); }

	void Release();

	void Allocate(LPCTSTR pApplication, LPCTSTR pTopic, bool bConnect = false);

	bool IsConnected() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hWnd; }

	void Disconnect();

	void Connect();

	bool TestPeer();

	KString RequestData(LPCTSTR pItem);

	WORD SetData(LPCTSTR pItem, LPCTSTR pValue);

	void Unadvise(LPCTSTR pItem);

	void Advise(LPCTSTR pItem);

	bool IsPeerAlive() const;

	LPCTSTR GetApplication() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Application; }

	LPCTSTR GetTopic() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Topic; }
};

#endif // _MSC_VER

#endif // dde_h
