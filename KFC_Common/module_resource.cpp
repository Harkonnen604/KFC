#include "kfc_common_pch.h"
#include "module_resource.h"

#ifdef _MSC_VER

// ----------------
// Module resource
// ----------------
TModuleResource::TModuleResource()
{
	m_hResource = NULL;
}

TModuleResource::TModuleResource(	HMODULE	hSModule,
									LPCTSTR	pType,
									LPCTSTR	pName,
									WORD	wLanguage)
{
	m_hResource = NULL;

	Allocate(hSModule, pType, pName, wLanguage);
}

void TModuleResource::Release()
{
	m_hResource = NULL;		
}

void TModuleResource::Allocate(	HMODULE	hSModule,
								LPCTSTR	pType,
								LPCTSTR	pName,
								WORD	wLanguage)
{
	Release();

	try
	{
		DEBUG_VERIFY(pType);

		DEBUG_VERIFY(pName);		

		m_hResource = FindResourceEx(m_hModule = hSModule, pType, pName, wLanguage);

		if(m_hResource == NULL)
		{
			INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error finding resource"),
											GetLastError());
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

size_t TModuleResource::GetSize() const
{
	DEBUG_VERIFY_ALLOCATION;

	const size_t szSize = SizeofResource(m_hModule, m_hResource);

	if(szSize == 0)
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error getting resource size"),
										GetLastError());
	}

	return szSize;
}

const void* TModuleResource::GetDataPtr() const
{
	DEBUG_VERIFY_ALLOCATION;

	const HGLOBAL hMemory = LoadResource(m_hModule, m_hResource);

	if(hMemory == NULL)
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error loading resource"),
										GetLastError());
	}

	const void* const pData = LockResource(hMemory);

	if(pData == NULL)
		INITIATE_DEFINED_FAILURE(TEXT("Error locking resource."));

	return pData;
}

HRSRC TModuleResource::GetResource() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_hResource;
}

#endif // _MSC_VER
