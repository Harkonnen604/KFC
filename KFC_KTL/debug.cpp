#include "kfc_ktl_pch.h"
#include "debug.h"

#include "ktl_tls_item.h"

// ------------
// Test assert
// ------------
void _tassert(LPCTSTR pExp, LPCTSTR pFile, int iLine)
{
	INITIATE_DEFINED_FAILURE(	(KString)TEXT("Test assertion failed: ") +
									pExp +
									TEXT(" , \"") +
									pFile +
									TEXT("\" , ") +
									iLine);
}

// -------
// Consts
// -------
#ifdef _DEBUG
	const bool g_bDebug = true;
#else //_DEBUG
	const bool g_bDebug = false;
#endif // _DEBUG

#ifdef _DEVELOP
	const bool g_bDevelop = true;
#else //_DEVELOP
	const bool g_bDevelop = false;
#endif // _DEVELOP

// -----------
// Error text
// -----------
KString& GetErrorText()
{
	return T_KTL_TLS_Item::Get().m_ErrorText;
}

void UpdateErrorText(LPCTSTR pText)
{
	KString& ErrorText = GetErrorText();

	if(!ErrorText.IsEmpty())
	{
		#ifdef _MSC_VER
			ErrorText += TEXT("\r\n\r\n");
		#else // _MSC_VER
			ErrorText += TEXT("\n");
		#endif // _MSC_VER
	}

	ErrorText += pText;
}

void ShowErrorText(LPCTSTR pText)
{
	#ifdef _DEBUG
	{
		TDebugFileLocker Locker0;

		g_DebugFile <<	DFCC_EOL <<
						TEXT("-------- FAILURE MESSAGE (ThreadID: ") <<
						(size_t)GetCurrentThreadId() <<
						TEXT(") --------") <<
						DFCC_EOL;

		g_DebugFile << ((KString)pText).RNToN() << DFCC_EOL;

		g_DebugFile << TEXT("-------- END OF FAILURE MESSAGE --------") << DFCC_EOL;
	}
	#endif // _DEBUG

	::DebugMessageBox(	(KString)TEXT("Error (ThreadID: ") +
							(size_t)GetCurrentThreadId() +
							TEXT(")"),
						pText);
}

KString GetErrorDescription(DWORD dwCode)
{
	#ifdef _MSC_VER
	{
		KString Text;
	
		HANDLE hResult = NULL;
	
		if(FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							dwCode,
							0,
							(LPTSTR)&hResult,
							0,
							NULL) && hResult)
		{
			if(LPCTSTR pText = (LPCTSTR)LocalLock(hResult))
			{
				Text = pText;
	
				LocalUnlock(hResult);
			}
	
			LocalFree(hResult);
		}
		
		return Text.Trim();
	}
	#else // _MSC_VER
	{
		return "";
	}
	#endif // _MSC_VER
}

// ------------------
// Outer block guard
// ------------------
TOuterBlockGuard::TOuterBlockGuard()
{
	T_KTL_TLS_Item::Get().m_szOuterBlockDepth++;
}

TOuterBlockGuard::~TOuterBlockGuard()
{
	if(!--T_KTL_TLS_Item::Get().m_szOuterBlockDepth)
		g_TLS_Storage.CleanThreadChain();
}

// -----------------------
// Fast outer block guard
// -----------------------
TFastOuterBlockGuard::TFastOuterBlockGuard()
{
	T_KTL_TLS_Item::Get().m_szOuterBlockDepth++;
}

TFastOuterBlockGuard::~TFastOuterBlockGuard()
{
	T_KTL_TLS_Item::Get().m_szOuterBlockDepth--;
}
