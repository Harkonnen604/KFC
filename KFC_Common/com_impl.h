#ifndef com_impl_h
#define com_impl_h

#include <KFC_KTL/critical_section.h>
#include "com.h"

#ifdef _MSC_VER

// ------------------------
// IUnknown implementation
// ------------------------
class T_IUnknownImpl : public IUnknown
{
private:
	volatile LONG m_lRefCount;

public:
	T_IUnknownImpl() : m_lRefCount(0) {}

	virtual ~T_IUnknownImpl() {}

	STDMETHOD_(ULONG, AddRef)()
		{ return InterlockedIncrement(&m_lRefCount); }

	STDMETHOD_(ULONG, Release)()
	{
		LONG lRet = InterlockedDecrement(&m_lRefCount);

		if(!lRet)
			delete this;

		return lRet;
	}
};

// -------------------------------
// Static IUnknown implementation
// -------------------------------
class TStaticIUnknownImpl : public IUnknown
{
public:
	STDMETHOD_(ULONG, AddRef)()
		{ return 1; }

	STDMETHOD_(ULONG, Release)()
		{ return 1; }
};

// ------------------------------
// IUnknown-based implementation
// ------------------------------
template <class t>
class T_IUnknownBasedImpl : public t
{
public:
	STDMETHOD_(ULONG, AddRef)()
		{ return dynamic_cast<T_IUnknownImpl&>(*this).AddRef(); }

	STDMETHOD_(ULONG, Release)()
		{ return dynamic_cast<T_IUnknownImpl&>(*this).Release(); }
};

#define DECLARE_IUNKNOWN_BASED_IMPL(Interface) \
	typedef T_IUnknownBasedImpl<Interface> T_##Interface##Impl

// -------------------------------------
// Static IUnknown-based implementation
// -------------------------------------
template <class t>
class TStaticIUnknownBasedImpl : public t
{
public:
	STDMETHOD_(ULONG, AddRef)()
		{ return 1; }

	STDMETHOD_(ULONG, Release)()
		{ return 1; }
};

#define DECLARE_STATIC_IUNKNOWN_BASED_IMPL(Interface) \
	typedef TStaticIUnknownBasedImpl<Interface> TStatic##Interface##Impl

// ------------------
// COM class factory
// ------------------
DECLARE_STATIC_IUNKNOWN_BASED_IMPL(IClassFactory);

template <class ot>
class T_COM_ClassFactory :
	public TStaticIUnknownImpl,
	public TStaticIClassFactoryImpl
{
public:
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID qi_iid, void** ppRObject)
	{
		if(!ppRObject)
			return E_INVALIDARG;

		*ppRObject = NULL;

		if(qi_iid == IID_IUnknown)
			return (*(IUnknown**)ppRObject = (TStaticIUnknownImpl*)this)->AddRef(), S_OK;

		if(qi_iid == IID_IClassFactory)
			return (*(IClassFactory**)ppRObject = (TStaticIClassFactoryImpl*)this)->AddRef(), S_OK;

		return E_NOINTERFACE;
	}

	// IClassFactory
	STDMETHOD(CreateInstance)(IUnknown* pUnkOuter, REFIID qi_iid, void** ppRObject)
	{
		KFC_COM_OUTER_BLOCK_BEGIN
		{
			HRESULT r;

			if(!ppRObject)
				return E_INVALIDARG;

			*ppRObject = NULL;

			if(pUnkOuter)
				return CLASS_E_NOAGGREGATION;

			TPtrHolder<ot> pObject = new ot;

			if(!SUCCEEDED(r = pObject->QueryInterface(qi_iid, ppRObject)))
				return r;

			pObject.Extract();

			return S_OK;
		}
		KFC_COM_OUTER_BLOCK_END
	}

	STDMETHOD(LockServer)(BOOL bLock)
	{
		if(bLock)
			LockCOM_DLL();
		else
			UnlockCOM_DLL();

		return S_OK;
	}
};

// ---------------
// COM enumerator
// ---------------

// Thread-unsafe in terms of possible "eaten" resets

template <class it, REFIID iid, class ot, class traits>
class T_COM_Enumerator :
	public T_IUnknownImpl,
	public T_IUnknownBasedImpl<it>
{
public:
	TArray<ot, true> m_Data;

	volatile LONG m_lPos;

private:
	T_COM_Enumerator(T_COM_Enumerator&);

	T_COM_Enumerator& operator = (const T_COM_Enumerator&);

public:
	T_COM_Enumerator() : m_lPos(0) {}

	~T_COM_Enumerator()
		{ traits::Clean(m_Data.GetDataPtr(), m_Data.GetN()); }

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID qi_iid, void** ppRObject)
	{
		if(!ppRObject)
			return E_INVALIDARG;

		*ppRObject = NULL;

		if(qi_iid == IID_IUnknown)
			return (*(IUnknown**)ppRObject = (T_IUnknownImpl*)this)->AddRef(), S_OK;

		if(qi_iid == iid)
			return (*(IUnknown**)ppRObject = (T_IUnknownBasedImpl<it>*)this)->AddRef(), S_OK;

		return E_NOINTERFACE;
	}

	// it
	STDMETHOD(Next)(ULONG	celt,
					ot*		rgelt,
					ULONG*	pceltFetched)
	{
		if(!celt)
			return S_OK;

		if(!rgelt)
			return E_POINTER;

		if(celt > 1 && !pceltFetched)
			return E_INVALIDARG;

		if(pceltFetched)
			*pceltFetched = 0;

		size_t szPos = InterlockedExchangeAdd(&m_lPos, celt);

		if(szPos >= m_Data.GetN())
		{
			InterlockedExchange(&m_lPos, m_Data.GetN());
			return S_FALSE;
		}

		size_t szAmt = Min((size_t)celt, m_Data.GetN() - szPos);

		traits::Fill(m_Data.GetDataPtr() + szPos, rgelt, szAmt);

		if(pceltFetched)
			*pceltFetched = szAmt;

		return szAmt == celt ? S_OK : S_FALSE;
	}

	STDMETHOD(Skip)(ULONG celt)
	{
		if(!celt)
			return S_OK;

		size_t szPos = InterlockedExchangeAdd(&m_lPos, celt);

		if(szPos >= m_Data.GetN())
		{
			InterlockedExchange(&m_lPos, m_Data.GetN());
			return S_FALSE;
		}

		size_t szAmt = Min((size_t)celt, m_Data.GetN() - szPos);

		return szAmt == celt ? S_OK : S_FALSE;
	}

	STDMETHOD(Reset)()
	{
		InterlockedExchange(&m_lPos, 0);

		return S_OK;
	}

	STDMETHOD(Clone)(it** ppEnum)
	{
		if(!ppEnum)
			return E_POINTER;

		T_COM_Enumerator* pEnum = new T_COM_Enumerator;

		pEnum->m_Data.SetN(m_Data.GetN());

		traits::Clone(m_Data.GetDataPtr(), pEnum->m_Data.GetDataPtr(), m_Data.GetN());

		pEnum->m_lPos = InterlockedExchangeAdd(&m_lPos, 0);

		DEBUG_EVERIFY(SUCCEEDED(pEnum->QueryInterface(iid, (void**)ppEnum)));

		return S_OK;
	}
};

// ----------------------
// COM string enumerator
// ----------------------
class T_COM_StringEnumeratorTraits
{
public:
	static void Fill(const LPOLESTR* pSrc, LPOLESTR* pDst, size_t szN)
	{
		for(size_t i = 0 ; i < szN ; i++)
			pDst[i] = COM_strdup(pSrc[i]);
	}

	static void Clean(LPOLESTR* pData, size_t szN)
	{
		for(size_t i = szN - 1 ; i != UINT_MAX ; i--)
			CoTaskMemFree(pData[i]), pData[i] = NULL;
	}

	static void Clone(const LPOLESTR* pSrc, LPOLESTR* pDst, size_t szN)
		{ Fill(pSrc, pDst, szN); }
};

class T_COM_StringEnumerator : public
	T_COM_Enumerator<	IEnumString,
						IID_IEnumString,
						LPOLESTR,
						T_COM_StringEnumeratorTraits>
{
public:
	T_COM_StringEnumerator& Add(LPCWSTR pString)
	{
		DEBUG_VERIFY(pString);

		m_Data.Add() = COM_strdup(pString);

		return *this;
	}

	T_COM_StringEnumerator& operator << (LPCWSTR pString)
		{ return Add(pString); }
};

// ------------------------
// COM IUnknown enumerator
// ------------------------
class T_COM_IUnknownEnumeratorTraits
{
public:
	static void Fill(IUnknown* const* pSrc, IUnknown** pDst, size_t szN)
	{
		memcpy(pDst, pSrc, szN * sizeof(IUnknown));

		for(size_t i = 0 ; i < szN ; i++)
			pDst[i]->AddRef();
	}

	static void Clean(IUnknown** pData, size_t szN)
	{
		for(size_t i = szN - 1 ; i != UINT_MAX ; i--)
			pData[i]->Release(), pData[i] = NULL;
	}

	static void Clone(IUnknown* const* pSrc, IUnknown** pDst, size_t szN)
		{ Fill(pSrc, pDst, szN); }
};

class T_COM_IUnknownEnumerator : public
	T_COM_Enumerator<	IEnumUnknown,
						IID_IEnumUnknown,
						IUnknown*,
						T_COM_IUnknownEnumeratorTraits>
{
public:
	T_COM_IUnknownEnumerator& Add(IUnknown* pUnk)
	{
		DEBUG_VERIFY(pUnk);

		(m_Data.Add() = pUnk)->AddRef();

		return *this;
	}

	T_COM_IUnknownEnumerator& operator << (IUnknown* pUnk)
		{ return Add(pUnk); }
};

// -----------------
// Connection point
// -----------------
DECLARE_STATIC_IUNKNOWN_BASED_IMPL(IConnectionPoint);

DECLARE_IUNKNOWN_BASED_IMPL(IEnumConnections);

class TConnectionPoint :
	public TStaticIUnknownImpl,
	public TStaticIConnectionPointImpl
{
private:
	// Connection
	struct TConnection
	{
		T_COM_Interface<IUnknown>	m_pUnk;
		T_COM_GIT_Link				m_IntGIT_Link;
	};

	// Connections
	typedef TList<TConnection> TConnections;

	// Enum connections
	class TEnumConnectionsTraits
	{
	public:
		static void Fill(const CONNECTDATA* pSrc, CONNECTDATA* pDst, size_t szN)
		{
			memcpy(pDst, pSrc, szN * sizeof(CONNECTDATA));

			for(size_t i = 0 ; i < szN ; i++)
				pDst[i].pUnk->AddRef();
		}

		static void Clean(CONNECTDATA* pData, size_t szN) {}

		static void Clone(const CONNECTDATA* pSrc, CONNECTDATA* pDst, size_t szN)
			{ memcpy(pDst, pSrc, szN * sizeof(CONNECTDATA)); }
	};

	typedef T_COM_Enumerator<	IEnumConnections,
								IID_IEnumConnections,
								CONNECTDATA,
								TEnumConnectionsTraits>
		TEnumConnections;

private:
	IConnectionPointContainer* m_pContainer;

	const IID* m_pIID;

	TConnections m_Connections;

	mutable TCriticalSection m_CS;

public:
	TConnectionPoint() :
		m_pContainer(NULL), m_pIID(NULL) {}

	TConnectionPoint& Prepare(IConnectionPointContainer* pContainer, REFIID iid)
	{
		DEBUG_VERIFY(!m_pContainer && !m_pIID);

		m_pContainer = pContainer;

		m_pIID = &iid;

		return *this;
	}

	bool HasConnections() const
	{
		TCriticalSectionLocker Locker0(m_CS);

		return !m_Connections.IsEmpty();
	}

	REFIID GetIID() const
	{
		DEBUG_VERIFY(m_pContainer && m_pIID);

		return *m_pIID;
	}

	template <class it>
	size_t GetConnections(TArray<T_COM_Interface<it> >& RConnections, bool bClearFirst = true) const
	{
		DEBUG_VERIFY(m_pContainer && m_pIID);

		if(bClearFirst)
			RConnections.Clear();

		size_t szRet = RConnections.GetN();

		{
			TCriticalSectionLocker Locker0(m_CS);

			FOR_EACH_LIST(m_Connections, TConnections::TConstIterator, Iter)
				Iter->m_IntGIT_Link.GetInterface((void**)&RConnections.Add().GetPtr());
		}

		return szRet;
	}

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID qi_iid, void** ppRObject)
	{
		if(!ppRObject)
			return E_INVALIDARG;

		*ppRObject = NULL;

		if(qi_iid == IID_IUnknown)
			return (*(IUnknown**)ppRObject = (TStaticIUnknownImpl*)this)->AddRef(), S_OK;

		if(qi_iid == IID_IConnectionPoint)
			return (*(IUnknown**)ppRObject = (TStaticIConnectionPointImpl*)this)->AddRef(), S_OK;

		return E_NOINTERFACE;
	}

	// IConnectionPoint
	STDMETHOD(GetConnectionInterface)(IID* pR_IID)
	{
		DEBUG_VERIFY(m_pContainer && m_pIID);

		if(!pR_IID)
			return E_POINTER;

		*pR_IID = *m_pIID;

		return S_OK;
	}

	STDMETHOD(GetConnectionPointContainer)(IConnectionPointContainer** ppCPC)
	{
		DEBUG_VERIFY(m_pContainer && m_pIID);

		if(!ppCPC)
			return E_POINTER;

		(*ppCPC = m_pContainer)->AddRef();

		return S_OK;
	}

	STDMETHOD(Advise)(IUnknown* pUnk, DWORD* pdwCookie)
	{
		DEBUG_VERIFY(m_pContainer && m_pIID);

		if(pdwCookie)
			*pdwCookie = 0;

		if(!pUnk || !pdwCookie)
			return E_POINTER;

		T_COM_Interface<IUnknown> pInt;

		if(FAILED(pUnk->QueryInterface(*m_pIID, (void**)&pInt.GetPtr())))
			return CONNECT_E_CANNOTCONNECT;

		TCriticalSectionLocker Locker(m_CS);

		TConnections::TIterator Iter = m_Connections.AddLast();

		Iter->m_pUnk = pUnk;

		Iter->m_IntGIT_Link.Allocate(pInt, *m_pIID);

		*pdwCookie = (DWORD)(uintptr_t)Iter.AsPVoid();

		return S_OK;
	}

	STDMETHOD(Unadvise)(DWORD dwCookie)
	{
		DEBUG_VERIFY(m_pContainer && m_pIID);

		TCriticalSectionLocker Locker0(m_CS);

		bool bValidCookie = false;

		FOR_EACH_LIST(m_Connections, TConnections::TConstIterator, CIter)
		{
			if((uintptr_t)CIter.AsPVoid() == dwCookie)
			{
				bValidCookie = true;
				break;
			}
		}

		if(!bValidCookie)
			return CONNECT_E_NOCONNECTION;

		m_Connections.Del(TConnections::TIterator().FromPVoid((void*)(uintptr_t)dwCookie));

		return S_OK;
	}

	STDMETHOD(EnumConnections)(IEnumConnections** ppEnum)
	{
		DEBUG_VERIFY(m_pContainer && m_pIID);

		if(!ppEnum)
			return E_POINTER;

		TEnumConnections* pEnum = new TEnumConnections;

		TCriticalSectionLocker Locker0(m_CS);

		FOR_EACH_LIST(m_Connections, TConnections::TConstIterator, Iter)
		{
			CONNECTDATA& Data = pEnum->m_Data.Add();

			Data.pUnk		= Iter->m_pUnk;
			Data.dwCookie	= (DWORD)(uintptr_t)Iter.AsPVoid();
		}

		DEBUG_EVERIFY(SUCCEEDED(pEnum->QueryInterface(IID_IEnumConnections, (void**)ppEnum)));

		return S_OK;
	}
};

// -----------------------------------------
// IConnectionPointContainer implementation
// -----------------------------------------
DECLARE_IUNKNOWN_BASED_IMPL(IEnumConnectionPoints);

class T_IConnectionPointContainerImpl :
	public T_IUnknownBasedImpl<IConnectionPointContainer>
{
private:
	// Enum connection points
	class TEnumConnetionPointTraits
	{
	public:
		static void Fill(IConnectionPoint* const* pSrc, IConnectionPoint** pDst, size_t szN)
		{
			memcpy(pDst, pSrc, szN * sizeof(IConnectionPoint*));

			for(size_t i = 0 ; i < szN ; i++)
				pDst[i]->AddRef();
		}

		static void Clean(IConnectionPoint** pData, size_t szN) {}

		static void Clone(IConnectionPoint* const* pSrc, IConnectionPoint** pDst, size_t szN)
			{ memcpy(pDst, pSrc, szN * sizeof(IConnectionPoint*)); }
	};

	typedef T_COM_Enumerator<	IEnumConnectionPoints,
								IID_IEnumConnectionPoints,
								IConnectionPoint*,
								TEnumConnetionPointTraits>
		TEnumConnectionPoints;

private:
	TArray<TConnectionPoint> m_CPs;

public:
	T_IConnectionPointContainerImpl(const IID* pIIDs, size_t szN)
	{
		for(size_t i = 0 ; i < szN ; i++)
			m_CPs.Add().Prepare(this, pIIDs[i]);
	}

	TConnectionPoint& GetCP(REFIID iid)
	{
		for(size_t i = 0 ; i < m_CPs.GetN() ; i++)
		{
			if(m_CPs[i].GetIID() == iid)
				return m_CPs[i];
		}

		INITIATE_FAILURE;
	}

	// IConnectionPointContainer
	STDMETHOD(EnumConnectionPoints)(IEnumConnectionPoints** ppEnum)
	{
		if(!ppEnum)
			return E_POINTER;

		TEnumConnectionPoints* pEnum = new TEnumConnectionPoints;

		FOR_EACH_ARRAY(m_CPs, i)
			pEnum->m_Data.Add() = m_CPs + i;

		DEBUG_EVERIFY(SUCCEEDED(pEnum->QueryInterface(IID_IEnumConnectionPoints, (void**)ppEnum)));

		return S_OK;
	}

	STDMETHOD(FindConnectionPoint)(REFIID cp_iid, IConnectionPoint** ppCP)
	{
		if(!ppCP)
			return E_POINTER;

		*ppCP = NULL;

		FOR_EACH_ARRAY(m_CPs, i)
		{
			if(m_CPs[i].GetIID() == cp_iid)
				return (*ppCP = m_CPs + i)->AddRef(), S_OK;
		}

		return CONNECT_E_NOCONNECTION;
	}
};

#endif // _MSC_VER

#endif // com_impl_h
