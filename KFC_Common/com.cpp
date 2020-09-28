#include "kfc_common_pch.h"
#include "com.h"

#ifdef _MSC_VER

GUID g_NullGUID;

// --------
// VARENUM
// --------
IMPLEMENT_ENUM_STRING_CONV(VARENUM)
    (VT_EMPTY,              TEXT("None"))
    (VT_EMPTY,              TEXT("Empty"))
    (VT_NULL,               TEXT("SQL NULL"))
    (VT_I1,                 TEXT("Char"))
    (VT_I1,                 TEXT("Int1"))
    (VT_I2,                 TEXT("Int2"))
    (VT_I4,                 TEXT("Int4"))
    (VT_I8,                 TEXT("Int8"))
    (VT_INT,                TEXT("Int"))
    (VT_UI1,                TEXT("BYTE"))
    (VT_UI2,                TEXT("WORD"))
    (VT_UI4,                TEXT("DWORD"))
    (VT_UI8,                TEXT("QWORD"))
    (VT_UINT,               TEXT("UINT"))
    (VT_R4,                 TEXT("Real4"))
    (VT_R4,                 TEXT("Float"))
    (VT_R4,                 TEXT("Real8"))
    (VT_R8,                 TEXT("Double"))
    (VT_CY,                 TEXT("Currency"))
    (VT_DATE,               TEXT("Date"))
    (VT_BSTR,               TEXT("String"))
    (VT_BSTR,               TEXT("BSTR"))
    (VT_DISPATCH,           TEXT("IDispatch"))
    (VT_ERROR,              TEXT("SCODE"))
    (VT_BOOL,               TEXT("Bool"))
    (VT_VARIANT,            TEXT("VARIANT ptr"))
    (VT_DECIMAL,            TEXT("Decimal"))
    (VT_RECORD,             TEXT("Record"))
    (VT_UNKNOWN,            TEXT("IUnknown"))
    (VT_VOID,               TEXT("void"))
    (VT_HRESULT,            TEXT("HRESULT"))
    (VT_PTR,                TEXT("ptr"))
    (VT_SAFEARRAY,          TEXT("SAFEARRAY"))
    (VT_CARRAY,             TEXT("C array"))
    (VT_USERDEFINED,        TEXT("User-defined"))
    (VT_LPSTR,              TEXT("LPSTR"))
    (VT_LPWSTR,             TEXT("LPWSTR"))
    (VT_FILETIME,           TEXT("FILETIME"))
    (VT_BLOB,               TEXT("BLOB"))
    (VT_STREAM,             TEXT("Stream"))
    (VT_STORAGE,            TEXT("Storage"))
    (VT_STREAMED_OBJECT,    TEXT("Streamed object"))
    (VT_STORED_OBJECT,      TEXT("Stored object"))
    (VT_BLOB_OBJECT,        TEXT("BLOB object"))
    (VT_CF,                 TEXT("Clipboard format"))
    (VT_CLSID,              TEXT("CLSID"))
    (VT_VECTOR,             TEXT("Vector"))
    (VT_ARRAY,              TEXT("Array"))
    (VT_BYREF,              TEXT("ByRef"))
    (VT_RESERVED,           TEXT("(reserved)"));

// ----------------
// COM initializer
// ----------------
T_COM_Initializer::T_COM_Initializer()
{
    m_bAllocated = false;
}

T_COM_Initializer::T_COM_Initializer(kflags_t flInit)
{
    m_bAllocated = false;

    Allocate(flInit);
}

void T_COM_Initializer::Release()
{
    if(m_bAllocated)
    {
        m_bAllocated = false;

        CoUninitialize();
    }
}

void T_COM_Initializer::Allocate(kflags_t flInit)
{
    Release();

    try
    {
        HRESULT r = CoInitializeEx(NULL, flInit);

        if(FAILED(r))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error initializing COM"), r);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

// ----------------
// OLE Initializer
// ----------------
T_OLE_Initializer::T_OLE_Initializer(bool bAllocate)
{
    m_bAllocated = false;

    if(bAllocate)
        Allocate();
}

void T_OLE_Initializer::Release()
{
    if(m_bAllocated)
    {
        m_bAllocated = false;

        OleUninitialize();
    }
}

void T_OLE_Initializer::Allocate()
{
    Release();

    try
    {
        HRESULT r = OleInitialize(NULL);

        if(FAILED(r))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error initializing OLE"), r);

        m_bAllocated = true;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

// ------------
// COM variant
// ------------
TStream& operator >> (TStream& Stream, VARIANT& RVariant)
{
    VariantClear(&RVariant);

    try
    {
        Stream >> RVariant.vt;

        switch(RVariant.vt)
        {
        case VT_EMPTY:
        case VT_NULL:
        case VT_VOID:
            return Stream;

        case VT_I1:
            return Stream >> RVariant.cVal;

        case VT_I2:
            return Stream >> RVariant.iVal;

        case VT_I4:
            return Stream >> RVariant.lVal;

        case VT_I8:
            return Stream >> RVariant.llVal;

        case VT_INT:
            return Stream >> RVariant.intVal;

        case VT_UI1:
            return Stream >> RVariant.bVal;

        case VT_UI2:
            return Stream >> RVariant.uiVal;

        case VT_UI4:
            return Stream >> RVariant.ulVal;

        case VT_UI8:
            return Stream >> RVariant.ullVal;

        case VT_UINT:
            return Stream >> RVariant.uintVal;

        case VT_R4:
            return Stream >> RVariant.fltVal;

        case VT_R8:
            return Stream >> RVariant.dblVal;

        case VT_CY:
            return Stream >> RVariant.cyVal;

        case VT_DATE:
            return Stream >> RVariant.date;

        case VT_ERROR:
            return Stream >> RVariant.scode;

        case VT_BOOL:
            return Stream >> RVariant.boolVal;

        case VT_DECIMAL:
            return Stream >> RVariant.decVal;

        case VT_HRESULT:
            return Stream >> RVariant.scode;

        case VT_BSTR:
        {
            size_t szLength;
            Stream >> szLength;

            RVariant.bstrVal = SysAllocStringLen(NULL, (UINT)szLength);

            if(!RVariant.bstrVal)
                INITIATE_DEFINED_FAILURE(TEXT("Error allocating COM string."));

            try
            {
                STREAM_READ_ARR(Stream, RVariant.bstrVal, szLength, WCHAR);
            }

            catch(...)
            {
                SysFreeString(RVariant.bstrVal), RVariant.bstrVal = NULL;
                throw;
            }

            RVariant.bstrVal[szLength] = 0;

            return Stream;
        }

        default:
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("Unsuported VARTYPE in input stream: ") +
                                            RVariant.vt + TEXT("."));
        }
    }

    catch(...)
    {
        memset(&RVariant, 0, sizeof(RVariant)), RVariant.vt = VT_EMPTY;
        throw;
    }
}

TStream& operator << (TStream& Stream, const VARIANT& Variant)
{
    switch(Variant.vt)
    {
    case VT_EMPTY:
    case VT_NULL:
    case VT_VOID:
        return Stream << Variant.vt;

    case VT_I1:
        return Stream << Variant.vt << Variant.cVal;

    case VT_I2:
        return Stream << Variant.vt << Variant.iVal;

    case VT_I4:
        return Stream << Variant.vt << Variant.lVal;

    case VT_I8:
        return Stream << Variant.vt << Variant.llVal;

    case VT_INT:
        return Stream << Variant.vt << Variant.intVal;

    case VT_UI1:
        return Stream << Variant.vt << Variant.bVal;

    case VT_UI2:
        return Stream << Variant.vt << Variant.uiVal;

    case VT_UI4:
        return Stream << Variant.vt << Variant.ulVal;

    case VT_UI8:
        return Stream << Variant.vt << Variant.ullVal;

    case VT_UINT:
        return Stream << Variant.vt << Variant.uintVal;

    case VT_R4:
        return Stream << Variant.vt << Variant.fltVal;

    case VT_R8:
        return Stream << Variant.vt << Variant.dblVal;

    case VT_CY:
        return Stream << Variant.vt << Variant.cyVal;

    case VT_DATE:
        return Stream << Variant.vt << Variant.date;

    case VT_ERROR:
        return Stream << Variant.vt << Variant.scode;

    case VT_BOOL:
        return Stream << Variant.vt << Variant.boolVal;

    case VT_DECIMAL:
        return Stream << Variant.vt << Variant.decVal;

    case VT_HRESULT:
        return Stream << Variant.vt << Variant.scode;

    case VT_BSTR:
    {
        DEBUG_VERIFY(Variant.bstrVal);

        Stream << Variant.vt;

        size_t szLength = wcslen(Variant.bstrVal);

        Stream << szLength;

        STREAM_WRITE_ARR(Stream, Variant.bstrVal, szLength, WCHAR);

        return Stream;
    }

    default:
        INITIATE_DEFINED_FAILURE(   (KString)TEXT("Unsuported VARTYPE for streaming: ") +
                                        Variant.vt + TEXT("."));
    }
}

// -------------
// COM dispatch
// -------------
T_COM_InterfaceTraits<IDispatch>::TProperty::TProperty
    (IDispatch* pDispatch, LPCTSTR pName) : m_pDispatch(pDispatch)
{
    DEBUG_VERIFY(m_pDispatch);

    TWideString Name(pName);

    LPWSTR pWideName = Name.GetDataPtr();

    HRESULT r;

    if( r = m_pDispatch->GetIDsOfNames
            (IID_NULL, &pWideName, 1, LOCALE_SYSTEM_DEFAULT, &m_DispID))
    {
        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error getting DISPID of COM dispatch property \"") +
                                            pName + "\"",
                                        r);
    }
}

T_COM_InterfaceTraits<IDispatch>::TIndexedProperty&
    T_COM_InterfaceTraits<IDispatch>::TIndexedProperty::operator = (const T_COM_Variant& Value)
{
    DISPPARAMS Params;

    VARIANT StackTempArgs[MAX_STACK_DISP_ARGS];

    TArray<VARIANT, true> TempArgs;

    const VARIANT* pArgs;

    if(m_Args.IsEmpty())
    {
        pArgs = &Value;
    }
    else if(m_Args.GetN() < MAX_STACK_DISP_ARGS) // n + 1 <= max
    {
        memcpy(StackTempArgs, m_Args.GetDataPtr(), m_Args.GetN() * sizeof(VARIANT));

        memcpy(StackTempArgs + m_Args.GetN(), &Value, sizeof(VARIANT));

        pArgs = StackTempArgs;
    }
    else
    {
        TempArgs.SetN(m_Args.GetN() + 1);

        memcpy(TempArgs.GetDataPtr(), m_Args.GetDataPtr(), m_Args.GetN());

        TempArgs[m_Args.GetN()] = Value;

        pArgs = TempArgs.GetDataPtr();
    }

    Params.rgvarg               = const_cast<VARIANT*>(pArgs);
    Params.rgdispidNamedArgs    = NULL;
    Params.cArgs                = (UINT)m_Args.GetN() + 1;
    Params.cNamedArgs           = 0;

    UINT uiArgError = 0;

    HRESULT r;

    if(r = m_pDispatch->Invoke( m_DispID,
                                IID_NULL,
                                LOCALE_SYSTEM_DEFAULT,
                                DISPATCH_PROPERTYPUT,
                                &Params,
                                NULL,
                                NULL,
                                &uiArgError))
    {
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error setting COM dispatch property value"), r);
    }

    return *this;
}

T_COM_InterfaceTraits<IDispatch>::TIndexedProperty::operator T_COM_Variant () const
{
    DISPPARAMS Params;

    Params.rgvarg               = const_cast<T_COM_Variant*>(m_Args.GetDataPtr());
    Params.rgdispidNamedArgs    = NULL;
    Params.cArgs                = (UINT)m_Args.GetN();
    Params.cNamedArgs           = 0;

    UINT uiArgError;

    T_COM_Variant Result;

    HRESULT r;

    if(r = m_pDispatch->Invoke( m_DispID,
                                IID_NULL,
                                LOCALE_SYSTEM_DEFAULT,
                                DISPATCH_PROPERTYGET,
                                &Params,
                                &Result,
                                NULL,
                                &uiArgError))
    {
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error setting COM dispatch property value"), r);
    }

    return Result;
}

T_COM_Variant T_COM_InterfaceTraits<IDispatch>::operator ()
    (LPCTSTR pName, T_COM_DispArgs& Args) const
{
    DEBUG_VERIFY(m_pInterface);

    DISPID DispID;

    {
        TWideString Name(pName);

        LPWSTR pWideName = Name.GetDataPtr();

        HRESULT r;

        if( r = m_pInterface->GetIDsOfNames
                (IID_NULL, &pWideName, 1, LOCALE_SYSTEM_DEFAULT, &DispID))
        {
            INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error getting DISPID of COM dispatch method \"") +
                                                pName + "\"",
                                            r);
        }
    }

    DISPPARAMS Params;

    Params.rgvarg               = const_cast<T_COM_Variant*>(Args.GetDataPtr());
    Params.rgdispidNamedArgs    = NULL;
    Params.cArgs                = (UINT)Args.GetN();
    Params.cNamedArgs           = 0;

    UINT uiArgError;

    T_COM_Variant Result;

    HRESULT r;

    if(r = m_pInterface->Invoke(DispID,
                                IID_NULL,
                                LOCALE_SYSTEM_DEFAULT,
                                DISPATCH_METHOD,
                                &Params,
                                &Result,
                                NULL,
                                &uiArgError))
    {
        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error invoking COM dispatch method \"") +
                                            pName + TEXT("\""),
                                        r);
    }

    return Result;
}

// ----------------
// Global routines
// ----------------
GUID KCreateGUID()
{
    GUID guid;

    CoCreateGuid(&guid);

    return guid;
}

CLSID KProgID_ToCLSID(LPCWSTR pProgID)
{
    CLSID clsid;

    DEBUG_VERIFY(pProgID);

    HRESULT r;

    if(r = CLSIDFromProgID(pProgID, &clsid))
        INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error getting CLSID of \"") + pProgID + TEXT('\"'), r);

    return clsid;
}

TWideString K_CLSID_ToProgID(REFCLSID rCLSID, bool bSafe)
{
    WCHAR* pProgID;

    HRESULT r;

    if(r = ProgIDFromCLSID(rCLSID, &pProgID))
    {
        if(!bSafe)
            INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error getting ProgID of \"") + KCLSIDToString(rCLSID) + TEXT("\""), r);

        return "";
    }

    TWideString ProgID = pProgID;

    CoTaskMemFree(pProgID);

    return ProgID;
}

KString KGUIDToString(REFCLSID rGUID)
{
    OLECHAR Buf[2048];

    Buf[StringFromGUID2(rGUID, Buf, sizeof(Buf) - 1)] = 0;

    if(Buf[0] == 0)
        return WRONG_GUID_TEXT;

    return (LPCTSTR)TDefaultString(Buf);
}

KString KCLSIDToString(REFCLSID rCLSID)
{
    LPOLESTR pBuf = NULL;

    if(StringFromCLSID(rCLSID, &pBuf))
        return WRONG_CLSID_TEXT;

    TDefaultString Ret = pBuf;

    CoTaskMemFree(pBuf);

    return (LPCTSTR)Ret;
}

KString KIIDToString(REFIID rIID)
{
    LPOLESTR pBuf = NULL;

    if(StringFromIID(rIID, &pBuf))
        return WRONG_IID_TEXT;

    TDefaultString Ret = pBuf;

    CoTaskMemFree(pBuf);

    return (LPCTSTR)Ret;
}

CLSID KStringToCLSID(LPCTSTR pText)
{
    CLSID clsid;

    if(FAILED(CLSIDFromString(TWideString(pText), &clsid)))
        INITIATE_DEFINED_FAILURE((KString)"Error parsing CLSID: \"" + pText + TEXT("\"."));

    return clsid;
}

LPWSTR COM_strdup(LPCWSTR pText)
{
    DEBUG_VERIFY(pText);

    size_t szLength = wcslen(pText);

    LPWSTR pResult = (LPWSTR)CoTaskMemAlloc((szLength + 1) * sizeof(WCHAR));
    KFC_VERIFY(pResult);

    memcpy(pResult, pText, (szLength + 1) * sizeof(WCHAR));

    return pResult;
}

// ----------------
// COM DLL locking
// ----------------
volatile LONG g_lCOM_DLL_LockCount = 0;

// ----
// GIT
// ----
T_COM_Interface<IGlobalInterfaceTable> g_pCOM_GIT;

#endif // _MSC_VER
