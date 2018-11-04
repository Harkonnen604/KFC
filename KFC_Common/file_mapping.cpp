#include "kfc_common_pch.h"
#include "file_mapping.h"

#include <KFC_KTL/pow2.h>

// -------------
// File mapping
// -------------
TFileMapping::TFileMapping()
{
	m_bAllocated = false;

	#ifdef _MSC_VER
		m_hMapping = NULL;
	#endif // _MSC_VER

	m_pData = NULL;
}

TFileMapping::TFileMapping(LPCTSTR pFileName, bool bReadOnly, size_t szSize)
{
	m_bAllocated = false;

	#ifdef _MSC_VER
		m_hMapping = NULL;
	#endif // _MSC_VER

	m_pData = NULL;

	Allocate(pFileName, bReadOnly, szSize);
}

void TFileMapping::InternalCloseMapping()
{
	#ifdef _MSC_VER
	{
		if(m_pData)
			UnmapViewOfFile(m_pData), m_pData = NULL;

		if(m_hMapping)
			CloseHandle(m_hMapping), m_hMapping = NULL;
	}
	#else // _MSC_VER
	{
		if(m_pData)
		{
			assert(m_File.IsAllocated());
			munmap(m_pData, m_File.GetLength()), m_pData = NULL;
		}
	}
	#endif // _MSC_VER
}

void TFileMapping::Release()
{
	m_bAllocated = false;

	InternalCloseMapping();

	m_File.Release();
}

void TFileMapping::ResizeFileAndOpenMapping(size_t szSize)
{
	#ifdef _MSC_VER
		DEBUG_VERIFY(!m_hMapping);
	#endif // _MSC_VER

	if(szSize != UINT_MAX)
		m_File.SetLength(szSize, false);

	if(GetActualSize() > 0)
	{
		#ifdef _MSC_VER
		{
			m_hMapping = CreateFileMapping(	m_File.GetHandle(),
											NULL,
											m_bReadOnly ? PAGE_READONLY : PAGE_READWRITE,
											0,
											szSize == UINT_MAX ? 0 : szSize,
											NULL);

			if(m_hMapping == NULL)
			{
				INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error mapping file \"") +
													m_File.GetFileName() +
													TEXT("\""),
												GetLastError());
			}

			m_pData = MapViewOfFile(m_hMapping,
									m_bReadOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS,
									0,
									0,
									szSize == UINT_MAX ? 0 : szSize);

			if(m_pData == NULL)
			{
				INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error mapping view of file \"") +
													m_File.GetFileName() +
													TEXT("\""),
												GetLastError());
			}
		}
		#else // _MSC_VER
		{
			m_pData = mmap(	NULL,
					 		szSize == UINT_MAX ? m_File.GetLength() : szSize,
							m_bReadOnly ? PROT_READ : (PROT_READ | PROT_WRITE),
							MAP_SHARED,
							fileno(m_File.GetStream()),
							0);

			if(m_pData == NULL)
			{
				INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error mapping file \"") +
													m_File.GetFileName() +
													TEXT("\""),
							 					errno);
			}
		}
		#endif // _MSC_VER

		DEBUG_VERIFY(!((UINT)m_pData & (DATA_ALIGNMENT - 1)));
	}
	else
	{
		m_pData = NULL;
	}
}

void TFileMapping::Allocate(LPCTSTR pFileName, bool bReadOnly, size_t szSize)
{
	Release();

	try
	{
		DEBUG_VERIFY(pFileName);

		DEBUG_VERIFY(!(bReadOnly && szSize != UINT_MAX));

		m_bReadOnly = bReadOnly;

		m_File.Allocate(pFileName, GetOpenFlags(szSize));

		ResizeFileAndOpenMapping(szSize);

		m_bAllocated = true;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TFileMapping::Reopen(bool bReadOnly, size_t szSize)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!(bReadOnly && szSize != UINT_MAX));

	if(bReadOnly == m_bReadOnly)
	{
		if(szSize != UINT_MAX && szSize != GetActualSize())
		{
			InternalCloseMapping();

			ResizeFileAndOpenMapping(szSize);
		}
	}
	else
	{
		InternalCloseMapping();

		m_bReadOnly = bReadOnly;

		m_File.Open(GetOpenFlags(szSize));

		ResizeFileAndOpenMapping(szSize);
	}	
}

void TFileMapping::SetSizeAndRelease(size_t szSize)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsReadOnly());

	DEBUG_VERIFY(szSize != UINT_MAX);

	InternalCloseMapping();

	if(szSize != m_File.GetLength())
		m_File.SetLength(szSize, false);

	Release();
}

// --------------------------------
// File mapping growable allocator
// --------------------------------
TFileMappingGrowableAllocator::TFileMappingGrowableAllocator()
{
}

TFileMappingGrowableAllocator::TFileMappingGrowableAllocator(LPCTSTR pFileName, bool bReadOnly, size_t szSize)
{
	Allocate(pFileName, bReadOnly, szSize);
}

void TFileMappingGrowableAllocator::Release()
{
	if(IsAllocated() && !IsReadOnly())
		m_Mapping.SetSizeAndRelease(m_szN);
	else
		m_Mapping.Release();
}

void TFileMappingGrowableAllocator::Allocate(LPCTSTR pFileName, bool bReadOnly, size_t szSize)
{
	Release();

	try
	{
		if(bReadOnly)
		{
			DEBUG_VERIFY(szSize == UINT_MAX);

			m_Mapping.Allocate(pFileName, true);

			m_szN = m_Mapping.GetActualSize();
		}
		else
		{
			if(szSize == UINT_MAX)
			{
				DEBUG_VERIFY(FileExists(pFileName));

				m_szN = GetFileLength(pFileName);			
			}
			else
			{
				m_szN = szSize;
			}

			m_Mapping.Allocate(pFileName, false, MinPow2GreaterEq(m_szN));
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TFileMappingGrowableAllocator::SetN(size_t szN)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsReadOnly());

	m_szN = szN;

	m_Mapping.Reopen(false, MinPow2GreaterEq(szN));
}

size_t TFileMappingGrowableAllocator::Add(size_t szN)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsReadOnly());

	m_szN += szN;

	m_Mapping.Reopen(false, MinPow2GreaterEq(m_szN));

	return m_szN - szN;
}
