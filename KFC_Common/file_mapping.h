#ifndef file_mapping_h
#define file_mapping_h

#include "file.h"
#include <KFC_KTL/growable_allocator.h>

// -------------
// File mapping
// -------------
class TFileMapping
{
private:
	void* m_pData;

	bool m_bReadOnly;

	TFile m_File;

	#ifdef _MSC_VER
		HANDLE m_hMapping;
	#endif // _MSC_VER

	bool m_bAllocated;

private:
	inline kflags_t GetOpenFlags(size_t szSize) const
	{
		return	m_bReadOnly ?
					FOF_BINARY | FOF_READ :
					szSize == -1 ?
						FOF_BINARY | FOF_READ | FOF_WRITE:
						FOF_BINARY | FOF_READ | FOF_WRITE | FOF_CREATE;
	}

	void InternalCloseMapping();

	void ResizeFileAndOpenMapping(size_t szSize);

public:
	TFileMapping();

	TFileMapping(LPCTSTR pFileName, bool bReadOnly, size_t szSize = -1);

	~TFileMapping()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(LPCTSTR pFileName, bool bReadOnly, size_t szSize = -1);

	void Reopen(bool bReadOnly, size_t szSize = -1);
	
	void SetSizeAndRelease(size_t szSize);

	void* GetDataPtr()
		{ DEBUG_VERIFY_ALLOCATION; return m_pData; }

	const void* GetDataPtr() const
		{ DEBUG_VERIFY_ALLOCATION; return m_pData; }

	const KString& GetFileName() const
		{ DEBUG_VERIFY_ALLOCATION; return m_File.GetFileName(); }

	bool IsReadOnly() const
		{ DEBUG_VERIFY_ALLOCATION; return m_bReadOnly; }

	size_t GetActualSize() const
		{ return m_File.GetLength(); }
};

// --------------------------------
// File mapping growable allocator
// --------------------------------
class TFileMappingGrowableAllocator : public TGrowableAllocator
{
private:
	TFileMapping m_Mapping;

	size_t m_szN;

public:
	TFileMappingGrowableAllocator();

	TFileMappingGrowableAllocator(LPCTSTR pFileName, bool bReadOnly, size_t szSize = -1);

	~TFileMappingGrowableAllocator()
		{ Release(); }

	bool IsAllocated() const
		{ return m_Mapping.IsAllocated(); }

	void Release();
	
	void Allocate(LPCTSTR pFileName, bool bReadOnly, size_t szSize = -1);

	BYTE* GetDataPtr()
		{ DEBUG_VERIFY_ALLOCATION; return (BYTE*)m_Mapping.GetDataPtr(); }

	const BYTE* GetDataPtr() const
		{ DEBUG_VERIFY_ALLOCATION; return (const BYTE*)m_Mapping.GetDataPtr(); }

	void SetN(size_t szN);

	size_t Add(size_t szN);

	size_t GetN() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szN; }

	const KString& GetFileName() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Mapping.GetFileName(); }

	bool IsReadOnly() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Mapping.IsReadOnly(); }
};

// ----------------
// File mapped var
// ----------------
template <class t>
class TFileMappedVar
{
private:
	TFileMapping m_Mapping;

public:
	TFileMappedVar() {}

	TFileMappedVar(LPCTSTR pFileName)
		{ Allocate(pFileName); }

	~TFileMappedVar()
		{ Release(); }

	bool IsAllocated() const
		{ return m_Mapping.IsAllocated(); }

	void Release()
		{ m_Mapping.Release(); }

	void Allocate(LPCTSTR pFileName)
	{
		Release();

		const bool bReset = !FileExists(pFileName);

		m_Mapping.Allocate(pFileName, false, sizeof(t));

		if(bReset)
			Reset();
	}

	void Reset()
		{ DEBUG_VERIFY_ALLOCATION; new(GetDataPtr()) t(); }

	t* GetDataPtr()
		{ DEBUG_VERIFY_ALLOCATION; return (t*)m_Mapping.GetDataPtr(); }

	const t* GetDataPtr() const
		{ DEBUG_VERIFY_ALLOCATION; return (const t*)m_Mapping.GetDataPtr(); }

	t& GetDataRef()
		{ DEBUG_VERIFY_ALLOCATION; return *(t*)m_Mapping.GetDataPtr(); }

	const t& GetDataRef() const
		{ DEBUG_VERIFY_ALLOCATION; return *(const t*)m_Mapping.GetDataPtr(); }

	t* operator -> ()
		{ return GetDataPtr(); }

	const t* operator -> () const
		{ return GetDataPtr(); }

	t& operator * ()
		{ return GetDataRef(); }

	const t& operator * () const
		{ return GetDataRef(); }
};

// ----------------
// Global routines
// ----------------
template <class t>
inline void AllocateFileMappedContainer(t& Container, LPCTSTR pFileName)
{
	if(FileExists(pFileName))
	{
		Container.m_Accessor.GetHeapAllocator().Allocate(pFileName, false);

		Container.m_Accessor.AllocateHeap();
	}
	else
	{
		Container.m_Accessor.GetHeapAllocator().Allocate(pFileName, false, 0);

		Container.m_Accessor.AllocateHeap();

		Container.Clear();
	}
}

#endif // file_mapping_h
