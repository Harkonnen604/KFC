#include "kfc_common_pch.h"
#include "hooks.h"

#ifdef _MSC_VER

// ---------------
// Hook installer
// ---------------
THookInstaller::THookInstaller()
{
	m_bAllocated = false;

	m_hHook = NULL;
}

void THookInstaller::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		if(m_hHook)
			UnhookWindowsHookEx(m_hHook), m_hHook = NULL;
	}
}
	
void THookInstaller::Allocate(	int				iType,
								const T_DLL&	DLL,
								LPCSTR			pFunctionName,
								DWORD			dwThreadID)
{
	Release();

	try
	{
		DEBUG_VERIFY(DLL.IsAllocated());

		m_hHook = SetWindowsHookEx(	iType,
									(HOOKPROC)DLL.GetFunction(pFunctionName),
									DLL,
									dwThreadID);

		if(m_hHook == NULL)
		{
			INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error installing hook type ") + iType,
											GetLastError());
		}

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}
	
HHOOK THookInstaller::GetHook() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_hHook;
}

#endif // _MSC_VER
