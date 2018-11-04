#ifndef module_resource_h
#define module_resource_h

#ifdef _MSC_VER

// ----------------
// Module resource
// ----------------
class TModuleResource
{
private:
	HMODULE m_hModule;

	HRSRC m_hResource;

public:
	TModuleResource();
	
	TModuleResource(HMODULE	hSModule,
					LPCTSTR	pType,
					LPCTSTR	pName,
					WORD	wLanguage = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));

	~TModuleResource()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hResource; }

	void Release();

	void Allocate(	HMODULE	hSModule,
					LPCTSTR	pType,
					LPCTSTR	pName,
					WORD	wLanguage = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));

	size_t GetSize() const;

	const void* GetDataPtr() const;

	HRSRC GetResource() const;

	operator HRSRC () const { return GetResource(); }
};

#endif // _MSC_VER

#endif // module_resource_h
