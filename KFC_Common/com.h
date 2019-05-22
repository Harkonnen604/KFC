#ifndef com_h
#define com_h

#include <KFC_KTL/date_time.h>

#ifdef _MSC_VER

extern GUID g_NullGUID;

// Failsafe constants
#define WRONG_GUID_TEXT		TEXT("{WRONG_GUID}")
#define WRONG_CLSID_TEXT	TEXT("{WRONG_CLSID}")
#define WRONG_IID_TEXT		TEXT("{WRONG_IID}")

// Limits
#define MAX_STACK_DISP_ARGS		(16)

// ----------
// Streaming
// ----------
DECLARE_BASIC_STREAMING(VARENUM);
DECLARE_BASIC_STREAMING(CY);
DECLARE_BASIC_STREAMING(DECIMAL);
DECLARE_BASIC_STREAMING(GUID);
DECLARE_BASIC_STREAMING(CONNECTDATA);

// --------
// VARENUM
// --------
DECLARE_ENUM_STRING_CONV(VARENUM);

// --------------------
// KFC COM outer block
// --------------------
#ifdef _DEBUG

	#define KFC_COM_OUTER_BLOCK_BEGIN	\
		KFC_FAST_OUTER_BLOCK_BEGIN 		\
		TEST_BLOCK_BEGIN				\
		{								\

	#define KFC_COM_OUTER_BLOCK_END			\
		}									\
		TEST_BLOCK_KFC_EXCEPTION_HANDLER	\
		{									\
			ShowErrorText();				\
		}									\
		TEST_BLOCK_END						\
		KFC_OUTER_BLOCK_END					\
											\
		return E_FAIL;						\

#else // _DEBUG

	#define KFC_COM_OUTER_BLOCK_BEGIN	\
		KFC_FAST_OUTER_BLOCK_BEGIN 		\
		SAFE_BLOCK_BEGIN				\
		{								\

	#define KFC_COM_OUTER_BLOCK_END			\
		}									\
		SAFE_BLOCK_END						\
		KFC_OUTER_BLOCK_END					\
											\
		return E_FAIL;						\

#endif // _DEBUG

// ----------------
// Global routines
// ----------------
GUID KCreateGUID();

CLSID KProgID_ToCLSID(LPCWSTR pProgID);
TWideString K_CLSID_ToProgID(REFCLSID rCLSID, bool bSafe = false);

KString KGUIDToString	(REFGUID	rGUID);
KString KCLSIDToString	(REFCLSID	rCLSID);
KString KIIDToString	(REFIID		rIID);

CLSID KStringToCLSID(LPCTSTR pText);

LPWSTR COM_strdup(LPCWSTR pText);

// ----------------
// COM DLL locking
// ----------------
extern volatile LONG g_lCOM_DLL_LockCount;

inline void LockCOM_DLL()
	{ InterlockedIncrement(&g_lCOM_DLL_LockCount); }

inline void UnlockCOM_DLL()
	{ InterlockedDecrement(&g_lCOM_DLL_LockCount); }

inline bool CanUnloadCOM_DLL()
	{ return InterlockedExchangeAdd(&g_lCOM_DLL_LockCount, 0) <= 0; }

class T_COM_DLL_Locker
{
public:
	T_COM_DLL_Locker()
		{ LockCOM_DLL(); }

	~T_COM_DLL_Locker()
		{ UnlockCOM_DLL(); }
};

// ----------------
// COM initializer
// ----------------
class T_COM_Initializer
{
private:
	bool m_bAllocated;

public:
	T_COM_Initializer();

	T_COM_Initializer(kflags_t flInit);

	~T_COM_Initializer()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(kflags_t flInit);
};

// ----------------
// OLE Initializer
// ----------------
class T_OLE_Initializer
{
private:
	bool m_bAllocated;

public:
	T_OLE_Initializer(bool bAllocate = false);

	~T_OLE_Initializer()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate();
};

// --------------------
// COM inteface traits
// --------------------
template <class InterfaceType>
class T_COM_InterfaceTraits
{
protected:
	InterfaceType* m_pInterface;
};

// --------------
// COM interface
// --------------
template <class InterfaceType>
class T_COM_Interface : public T_COM_InterfaceTraits<InterfaceType>
{
public:
	T_COM_Interface()
		{ m_pInterface = NULL; }

	T_COM_Interface(const T_COM_Interface& Interface)
		{ m_pInterface = NULL; Set(Interface); }

	T_COM_Interface(InterfaceType* pInterface, bool bAddRef = true)
		{ m_pInterface = NULL; Set(pInterface, bAddRef); }

	T_COM_Interface(LPCTSTR		pProgID,
					REFIID		rIID,
					DWORD		dwContext	= CLSCTX_INPROC_SERVER,
					IUnknown*	pUnkOuter	= NULL,
					bool		bCreate		= true)
	{
		m_pInterface = NULL;

		if(bCreate)
			CreateObject(KProgID_ToCLSID(pProgID), rIID, dwContext, pUnkOuter);
		else
			GetObject(KProgID_ToCLSID(pProgID), rIID, dwContext);
	}

	T_COM_Interface(REFCLSID	rCLSID,
					REFIID		rIID,
					DWORD		dwContext	= CLSCTX_INPROC_SERVER,
					IUnknown*	pUnkOuter	= NULL,
					bool		bCreate		= true)
	{
		m_pInterface = NULL;

		if(bCreate)
		{
			CreateObject(rCLSID, rIID, dwContext, pUnkOuter);
		}
		else
		{
			DEBUG_VERIFY(!pUnkOuter);

			GetObject(rCLSID, rIID, dwContext);
		}
	}

	T_COM_Interface(IUnknown*	pUnknown,
					REFIID		rIID)
	{
		m_pInterface = NULL;

		QueryInterface(pUnknown, rIID);
	}

	~T_COM_Interface()
		{ Release(); }

	void Invalidate()
		{ m_pInterface = NULL; }

	void Release()
	{
		if(m_pInterface)
			m_pInterface->Release(), m_pInterface = NULL;
	}

	InterfaceType* Extract()
	{
		InterfaceType* pInterface = m_pInterface;

		Invalidate();

		return pInterface;
	}

	T_COM_Interface& CreateObject(	LPCTSTR		pProgID,
									REFIID		rIID,
									DWORD		dwContext = CLSCTX_INPROC_SERVER,
									IUnknown*	pUnkOuter = NULL)
	{
		return CreateObject(KProgID_ToCLSID(pProgID),
							rIID,
							dwContext,
							pUnkOuter);
	}

	T_COM_Interface& CreateObject(	REFCLSID	rCLSID,
									REFIID		rIID,
									DWORD		dwContext = CLSCTX_INPROC_SERVER,
									IUnknown*	pUnkOuter = NULL);

	T_COM_Interface& GetObject(	LPCTSTR	pProgID,
								REFIID	rIID,
								DWORD	dwContext = CLSCTX_INPROC_SERVER)
	{
		return GetObject(KProgID_ToCLSID(pProgID), rIID, dwContext);
	}

	T_COM_Interface& GetObject(	REFCLSID	rCLSID,
								REFIID		rIID,
								DWORD		dwContext = CLSCTX_INPROC_SERVER);

	T_COM_Interface& QueryInterface(IUnknown*	pUnknown,
									REFIID		rIID);

	InterfaceType* GetInterface() const
		{ return m_pInterface; }

	operator InterfaceType* () const
		{ return m_pInterface; }

	InterfaceType* operator -> () const
		{ DEBUG_VERIFY(m_pInterface); return m_pInterface; }

	T_COM_Interface& Set(InterfaceType* pInterface, bool bAddRef = true)
	{
		if(m_pInterface == pInterface && bAddRef)
			return *this;

		Release();

		m_pInterface = pInterface;

		if(m_pInterface && bAddRef)
			m_pInterface->AddRef();

		return *this;
	}

	T_COM_Interface& operator = (InterfaceType* pInterface)
		{ return Set(pInterface); }

	T_COM_Interface& Set(const T_COM_Interface& Interface)
	{
		if(*this == Interface)
			return *this;

		Release();

		if(m_pInterface = Interface.m_pInterface)
			m_pInterface->AddRef();

		return *this;
	}

	T_COM_Interface& operator = (const T_COM_Interface& Interface)
		{ return Set(Interface); }

	InterfaceType*& GetPtr()
		{ Release(); return m_pInterface; }
};

template <class InterfaceType>
T_COM_Interface<InterfaceType>&
	T_COM_Interface<InterfaceType>::CreateObject(	REFCLSID	rCLSID,
													REFIID		rIID,
													DWORD		dwContext,
													IUnknown*	pUnkOuter)
{
	Release();

	HRESULT r;

	if(r = CoCreateInstance(rCLSID, pUnkOuter, dwContext, rIID, (void**)&m_pInterface))
	{
		INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error creating COM object instance,\r\n")				+
											TEXT("CLSID  = ") + KCLSIDToString	(rCLSID)		+ TEXT(",\r\n")	+
											TEXT("ProgID = ") + K_CLSID_ToProgID(rCLSID, true)	+ TEXT(",\r\n")	+
											TEXT("IID    = ") + KIIDToString	(rIID)			+ TEXT("\r\n"),
										r);
	}

	KFC_VERIFY(m_pInterface);

	return *this;
}

template <class InterfaceType>
T_COM_Interface<InterfaceType>&
	T_COM_Interface<InterfaceType>::GetObject(	REFCLSID	rCLSID,
												REFIID		rIID,
												DWORD		dwContext)
{
	Release();

	HRESULT r;

	if(r = CoGetClassObject(rCLSID, dwContext, NULL, rIID, (void**)&m_pInterface))
	{
		INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error getting COM object instance,\r\n")					+
											TEXT("CLSID  = ") + KCLSIDToString	(rCLSID)		+ TEXT(",\r\n")	+
											TEXT("ProgID = ") + K_CLSID_ToProgID(rCLSID, true)	+ TEXT(",\r\n")	+
											TEXT("IID    = ") + KIIDToString	(rIID)			+ TEXT("\r\n"),
										r);
	}

	KFC_VERIFY(m_pInterface);

	return *this;
}

template <class InterfaceType>
T_COM_Interface<InterfaceType>&
	T_COM_Interface<InterfaceType>::QueryInterface(IUnknown* pUnknown, REFIID rIID)
{
	Release();

	HRESULT r;

	DEBUG_VERIFY(pUnknown);

	if(r = pUnknown->QueryInterface(rIID, (void**)&m_pInterface))
	{
		INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error querying interface,\r\n")	+
											TEXT("IID = ") + KIIDToString(rIID) + TEXT("\r\n"),
										r);
	}

	KFC_VERIFY(m_pInterface);

	return *this;
}

// --------------------
// COM pointer deleter
// --------------------
template <class t>
class com_ptr_deleter
{
public:
	static void Delete(t* p)
		{ CoTaskMemFree(p); }
};

// ---------------------
// BSTR pointer deleter
// ---------------------
template <class t>
class bstr_ptr_deleter
{
public:
	static void Delete(t* p)
		{ SysFreeString(p); }
};

// ---------
// BSTR ptr
// ---------
typedef TPtrHolder<OLECHAR, bstr_ptr_deleter> T_BSTR_Ptr;

// ------------
// COM variant
// ------------
class T_COM_Variant : public VARIANT
{
public:
	T_COM_Variant()
		{ VariantInit(this); }

	T_COM_Variant(const T_COM_Variant& Variant)
		{ VariantInit(this); *this = Variant; }

	T_COM_Variant(const VARIANT& Variant)
		{ VariantInit(this); *this = Variant; }

	T_COM_Variant(const VARIANT& Variant, VARTYPE vt)
	{
		VariantInit(this);

		KFC_VERIFY(!VariantChangeType(this, const_cast<VARIANT*>(&Variant), 0, vt));
	}

	T_COM_Variant(const char* pValue)
		{ VariantInit(this), *this = pValue; }

	T_COM_Variant(const WCHAR* pValue)
		{ VariantInit(this), *this = pValue; }

	T_COM_Variant(INT64 iValue)
		{ VariantClear(this), *this = iValue; }

	T_COM_Variant(UINT64 uiValue)
		{ VariantClear(this), *this = uiValue; }

	T_COM_Variant(long lValue)
		{ VariantClear(this), *this = lValue; }

	T_COM_Variant(DWORD dwValue)
		{ VariantClear(this), *this = dwValue; }

	T_COM_Variant(int iValue)
		{ VariantInit(this), *this = iValue; }

	T_COM_Variant(UINT uiValue)
		{ VariantInit(this), *this = uiValue; }

	T_COM_Variant(short sValue)
		{ VariantClear(this), *this = sValue; }

	T_COM_Variant(WORD wValue)
		{ VariantClear(this), *this = wValue; }

	T_COM_Variant(char cValue)
		{ VariantClear(this), *this = cValue; }

	T_COM_Variant(BYTE bValue)
		{ VariantClear(this), *this = bValue; }

	T_COM_Variant(float fValue)
		{ VariantInit(this), *this = fValue; }

	T_COM_Variant(double dValue)
		{ VariantInit(this), *this = dValue; }

	T_COM_Variant(bool bValue)
		{ VariantInit(this), *this = bValue; }

	T_COM_Variant(const TDateTime& DT, size_t szMSec = 0)
		{ VariantInit(this), Set(DT, szMSec); }

	T_COM_Variant(IDispatch* pValue)
		{ VariantInit(this), *this = pValue; }

	~T_COM_Variant()
		{ VariantClear(this); }

	T_COM_Variant& ChangeType(VARTYPE vt)
	{
		DEBUG_EVALUATE_VERIFY(!VariantChangeType(this, this, 0, vt));

		return *this;
	}

	T_COM_Variant& operator = (const VARIANT& Variant)
		{ DEBUG_EVALUATE_VERIFY(!VariantCopy(this, const_cast<VARIANT*>(&Variant))); return *this; }

	T_COM_Variant& operator = (const T_COM_Variant& Variant)
		{ DEBUG_EVALUATE_VERIFY(!VariantCopy(this, const_cast<T_COM_Variant*>(&Variant))); return *this; }

	T_COM_Variant& operator = (const char* pValue)
		{ return *this = (const WCHAR*)TWideString(pValue); }

	T_COM_Variant& operator = (const WCHAR* pValue)
	{
		VariantClear(this);

		vt = VT_BSTR, bstrVal = SysAllocString(pValue);

		return *this;
	}

	T_COM_Variant& operator = (INT64 iValue)
	{
		VariantClear(this);

		vt = VT_I8, llVal = iValue;

		return *this;
	}

	T_COM_Variant& operator = (UINT64 uiValue)
	{
		VariantClear(this);

		vt = VT_UI8, ullVal = uiValue;

		return *this;
	}

	T_COM_Variant& operator = (long lValue)
	{
		VariantClear(this);

		vt = VT_I4, lVal = lValue;

		return *this;
	}

	T_COM_Variant& operator = (DWORD dwValue)
	{
		VariantClear(this);

		vt = VT_UI4, ulVal = dwValue;

		return *this;
	}

	T_COM_Variant& operator = (int iValue)
	{
		VariantClear(this);

		vt = VT_INT, intVal = iValue;

		return *this;
	}

	T_COM_Variant& operator = (UINT uiValue)
	{
		VariantClear(this);

		vt = VT_UINT, uintVal = uiValue;

		return *this;
	}

	T_COM_Variant& operator = (short sValue)
	{
		VariantClear(this);

		vt = VT_I2, iVal = sValue;

		return *this;
	}

	T_COM_Variant& operator = (WORD wValue)
	{
		VariantClear(this);

		vt = VT_UI2, uiVal = wValue;

		return *this;
	}

	T_COM_Variant& operator = (char cValue)
	{
		VariantClear(this);

		vt = VT_I1, cVal = cVal;

		return *this;
	}

	T_COM_Variant& operator = (BYTE bValue)
	{
		VariantClear(this);

		vt = VT_UI1, bVal = bVal;

		return *this;
	}

	T_COM_Variant& operator = (float fValue)
	{
		VariantClear(this);

		vt = VT_R4, this->fltVal = fValue;

		return *this;
	}

	T_COM_Variant& operator = (double dValue)
	{
		VariantClear(this);

		vt = VT_R8, this->dblVal = dValue;

		return *this;
	}

	T_COM_Variant& operator = (bool bValue)
	{
		VariantClear(this);

		vt = VT_BOOL, boolVal = bValue ? 0xFFFF : 0;

		return *this;
	}

	T_COM_Variant& Set(const TDateTime& DT, size_t szMSec = 0)
	{
		VariantClear(this);

		vt = VT_DATE, date = DT.GetCOM_DT(szMSec);

		return *this;
	}

	T_COM_Variant& operator = (const TDateTime& DT)
		{ return Set(DT); }

	T_COM_Variant& operator = (IDispatch* pValue)
	{
		VariantClear(this);

		vt = VT_DISPATCH;

		if(pdispVal = pValue)
			pdispVal->AddRef();

		return *this;
	}

	operator BSTR () const
		{ DEBUG_VERIFY(vt == VT_BSTR && bstrVal); return bstrVal; }

	operator INT64 () const
		{ return vt == VT_I8 ? llVal : (INT64)T_COM_Variant(*this, VT_I8); }

	operator UINT64 () const
		{ return vt == VT_UI8 ? ullVal : (UINT64)T_COM_Variant(*this, VT_UI8); }

	operator long () const
		{ return vt == VT_I4 ? lVal : (long)T_COM_Variant(*this, VT_I4); }

	operator DWORD () const
		{ return vt == VT_UI4 ? ulVal : (DWORD)T_COM_Variant(*this, VT_UI4); }

	operator int () const
		{ return vt == VT_INT ? intVal : (int)T_COM_Variant(*this, VT_INT); }

	operator UINT () const
		{ return vt == VT_UINT ? uintVal : (UINT)T_COM_Variant(*this, VT_UINT); }

	operator short () const
		{ return vt == VT_I2 ? iVal : (short)T_COM_Variant(*this, VT_I2); }

	operator WORD () const
		{ return vt == VT_UI2 ? uiVal : (WORD)T_COM_Variant(*this, VT_UI2); }

	operator char () const
		{ return vt == VT_I1 ? cVal : (char)T_COM_Variant(*this, VT_I1); }

	operator BYTE () const
		{ return vt == VT_UI1 ? bVal : (BYTE)T_COM_Variant(*this, VT_UI1); }

	operator float () const
		{ return vt == VT_R4 ? fltVal : (float)T_COM_Variant(*this, VT_R4); }

	operator double () const
		{ return vt == VT_R8 ? dblVal : (double)T_COM_Variant(*this, VT_R8); }

	operator bool () const
		{ return vt == VT_BOOL ? boolVal : (bool)T_COM_Variant(*this, VT_BOOL); }

	operator TDateTime () const
		{ return vt == VT_DATE ? TDateTime().SetCOM_DT(date) : T_COM_Variant(*this, VT_DATE).operator TDateTime(); }

	operator IDispatch* () const
		{ DEBUG_VERIFY(vt == VT_DISPATCH && pdispVal); return pdispVal; }

	operator KString () const
		{ return vt == VT_BSTR ? KString(TAnsiString(bstrVal)) : vt == VT_EMPTY ? TEXT("") : vt == VT_DISPATCH ? TEXT("<Dispatch>") : (KString)T_COM_Variant(*this, VT_BSTR); }
};

TStream& operator >> (TStream& Stream, VARIANT& RVariant);

TStream& operator << (TStream& Stream, const VARIANT& Variant);

// --------------
// COM disp args
// --------------
class T_COM_DispArgs : public TArray<T_COM_Variant, true>
{
public:
	T_COM_DispArgs& operator << (const T_COM_Variant& Variant)
		{ (TArray<T_COM_Variant>&)*this << Variant; return *this; }
};

// -------------
// COM dispatch
// -------------
template <>
class T_COM_InterfaceTraits<IDispatch>
{
public:
	// Indexed property
	class TIndexedProperty
	{
	private:
		IDispatch* m_pDispatch;

		DISPID m_DispID;

		T_COM_DispArgs m_Args;

	public:
		TIndexedProperty(	IDispatch*				pDispatch,
							DISPID					DispID,
							const T_COM_DispArgs&	Args = T_COM_DispArgs()) :

			m_pDispatch	(pDispatch),
			m_DispID	(DispID),
			m_Args		(Args) {}

		TIndexedProperty& operator = (const T_COM_Variant& Value);

		operator T_COM_Variant () const;
	};

	// Property
	class TProperty
	{
	private:
		IDispatch* m_pDispatch;

		DISPID m_DispID;

	public:
		TProperty(IDispatch* pDispatch, LPCTSTR pName);

		TIndexedProperty operator [] (const T_COM_DispArgs& Args)
			{ return TIndexedProperty(m_pDispatch, m_DispID, Args); }

		TIndexedProperty operator [] (const T_COM_Variant& Arg)
			{ return TIndexedProperty(m_pDispatch, m_DispID, T_COM_DispArgs() << Arg); }

		TProperty& operator = (const T_COM_Variant& Value)
			{ TIndexedProperty(m_pDispatch, m_DispID) = Value; return *this; }

		operator T_COM_Variant () const
			{ return TIndexedProperty(m_pDispatch, m_DispID); }
	};

protected:
	IDispatch* m_pInterface;

public:
	TProperty operator [] (LPCTSTR pName) const
		{ DEBUG_VERIFY(m_pInterface); return TProperty(m_pInterface, pName); }

	T_COM_Variant operator ()
		(LPCTSTR pName, T_COM_DispArgs& Args = T_COM_DispArgs()) const;

	T_COM_Variant operator () (	LPCTSTR					pName,
								const T_COM_Variant&	Arg) const
	{
		return (*this)(pName, T_COM_DispArgs() << Arg);
	}

	T_COM_Variant operator () (	LPCTSTR					pName,
								const T_COM_Variant&	Arg1,
								const T_COM_Variant&	Arg2) const
	{
		return (*this)(pName, T_COM_DispArgs() << Arg1 << Arg2);
	}

	T_COM_Variant operator () (	LPCTSTR					pName,
								const T_COM_Variant&	Arg1,
								const T_COM_Variant&	Arg2,
								const T_COM_Variant&	Arg3) const
	{
		return (*this)(pName, T_COM_DispArgs() << Arg1 << Arg2 << Arg3);
	}

	T_COM_Variant operator () (	LPCTSTR					pName,
								const T_COM_Variant&	Arg1,
								const T_COM_Variant&	Arg2,
								const T_COM_Variant&	Arg3,
								const T_COM_Variant&	Arg4) const
	{
		return (*this)(pName, T_COM_DispArgs() << Arg1 << Arg2 << Arg3 << Arg4);
	}

	// Collections support
	int GetN() const
		{ return (T_COM_Variant)(*this)[TEXT("Count")]; }

	T_COM_Variant operator [] (int iIndex) const
		{ return (*this)[TEXT("Item")][iIndex]; }
};

typedef T_COM_Interface<IDispatch> T_COM_Dispatch;

// ----
// GIT
// ----
extern T_COM_Interface<IGlobalInterfaceTable> g_pCOM_GIT;

// -------------
// COM GIT link
// -------------
class T_COM_GIT_Link
{
private:
	IUnknown*	m_pInt;
	const IID*	m_pIID;
	DWORD		m_dwCookie;

public:
	T_COM_GIT_Link()
	{
		m_pInt = NULL;
	}

	T_COM_GIT_Link(IUnknown* pInt, REFIID iid)
	{
		m_pInt = NULL;

		Allocate(pInt, iid);
	}

	~T_COM_GIT_Link()
		{ Release(); }

	bool IsAllocated() const
		{ return m_pInt; }

	void Release()
	{
		assert(g_pCOM_GIT);

		if(m_pInt)
		{
			g_pCOM_GIT->RevokeInterfaceFromGlobal(m_dwCookie);

			m_pInt->Release(), m_pInt = NULL;
		}
	}

	void Allocate(IUnknown* pInt, REFIID iid)
	{
		DEBUG_VERIFY(g_pCOM_GIT);

		if(m_pInt == pInt)
			return;

		Release();

		m_pInt = pInt, m_pIID = &iid;

		HRESULT r;

		if(FAILED(r = g_pCOM_GIT->RegisterInterfaceInGlobal(m_pInt, *m_pIID, &m_dwCookie)))
		{
			m_pInt = NULL;

			INITIATE_DEFINED_CODE_FAILURE("COM GIT registration failed", r);
		}

		m_pInt->AddRef();
	}

	void GetInterface(void** ppRObject) const
	{
		DEBUG_VERIFY_ALLOCATION;

		HRESULT r;

		if(FAILED(r = g_pCOM_GIT->GetInterfaceFromGlobal(m_dwCookie, *m_pIID, ppRObject)))
			INITIATE_DEFINED_CODE_FAILURE("COM GIT fetch failed", r);
	}
};

#endif // _MSC_VER

#endif // com_h
