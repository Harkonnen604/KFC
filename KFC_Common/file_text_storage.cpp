#include "kfc_common_pch.h"
#include "file_text_storage.h"

#include <KFC_Common/file.h>

#ifdef _MSC_VER

// ------------------
// File text storage
// ------------------
void TFileTextStorage::Release()
{
	m_Storage.Release();
}

void TFileTextStorage::Allocate(LPCTSTR pFileNamePrefix)
{
	Release();

	try
	{
		const KString HeadersFileName	= (KString)pFileNamePrefix + TEXT(".hdr");
		const KString DataFileName		= (KString)pFileNamePrefix + TEXT(".dat");

		bool bClear = false;

		if(	!FileExists(HeadersFileName) ||
			!FileExists(DataFileName))
		{
			CreateEmptyFile(HeadersFileName);
			CreateEmptyFile(DataFileName);

			bClear = true;
		}

		m_Storage.GetHeadersAllocator().Allocate(HeadersFileName,	false);
		m_Storage.GetDataAllocator   ().Allocate(DataFileName,		false);
		m_Storage.Allocate();

		if(bClear)
			Clear();
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TFileTextStorage::Clear()
{
	DEBUG_VERIFY_ALLOCATION;

	m_Storage.Clear();
}

TFileTextStorage::TIterator TFileTextStorage::AddText(LPCTSTR pText, size_t szLength)
{
	DEBUG_VERIFY_ALLOCATION;

	const size_t szSize = (szLength + 1) * sizeof(TCHAR);

	const TIterator Iter = m_Storage.Reserve(szSize);

	memcpy(m_Storage[Iter], pText, szSize);

	return Iter;
}

#endif // _MSC_VER
