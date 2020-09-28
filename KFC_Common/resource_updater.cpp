#include "kfc_common_pch.h"
#include "resource_updater.h"

#include <KFC_Common/file.h>

#ifdef _MSC_VER

// -----------------
// Resource updater
// -----------------
TResourceUpdater::TResourceUpdater()
{
	m_hHandle = NULL;
}

TResourceUpdater::TResourceUpdater(	LPCTSTR	pFileName,
									bool	bDeleteExisting)
{
	m_hHandle = NULL;

	Allocate(pFileName, bDeleteExisting);
}

void TResourceUpdater::Release(bool bCommit)
{
	if(m_hHandle)
		EndUpdateResource(m_hHandle, bCommit ? FALSE : TRUE), m_hHandle = NULL;
}

void TResourceUpdater::Allocate(LPCTSTR	pFileName,
								bool	bDeleteExisting)
{
	Release();

	try
	{
		m_hHandle = BeginUpdateResource(pFileName, bDeleteExisting);
		if(m_hHandle == NULL)
		{
			INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error begining updating file \"") +
												pFileName +
												TEXT("\" resources"),
											GetLastError());
		}
	}

	catch(...)
	{
		Release(false);
		throw;
	}
}

void TResourceUpdater::Update(	LPCTSTR	pType,
								size_t	szID,
								void*	pData,
								size_t	szLength,
								WORD	wLanguage)
{
	DEBUG_VERIFY_ALLOCATION;
	
	DEBUG_VERIFY(pType);

	DEBUG_VERIFY(pData && szLength > 0);

	if(!UpdateResource(*this, pType, MAKEINTRESOURCE(szID), wLanguage, pData, (DWORD)szLength))
	{
		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error updating resource"),
										GetLastError());
	}
}

void TResourceUpdater::UpdateFile(	LPCTSTR	pType,
									size_t	szID,
									LPCTSTR	pFileName,
									WORD	wLanguage,
									bool	bPrependWithLength,
									BYTE	bXORValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(pType);

	DEBUG_VERIFY(pFileName);

	TFile File(pFileName, FOF_BINARYREAD);

	TArray<BYTE, true> Buffer;

	if(bPrependWithLength)
	{
		(size_t&)Buffer.Add(4) = File.GetLength();

		File.Read(&Buffer.Add(File.GetLength()), File.GetLength());
	}
	else
	{
		File.Read(&Buffer.Add(File.GetLength()), File.GetLength());
	}

	size_t i;
	for(i = 0 ; i < Buffer.GetN() ; i++)
		Buffer[i] ^= bXORValue;

	Update(pType, szID, Buffer.GetDataPtr(), Buffer.GetN(), wLanguage);
}

HANDLE TResourceUpdater::GetHandle() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_hHandle;
}

#endif // _MSC_VER
