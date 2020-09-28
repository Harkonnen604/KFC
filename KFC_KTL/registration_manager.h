#ifndef registration_manager_h
#define registration_manager_h

#include "basic_types.h"
#include "object_pointer.h"
#include "array.h"

// ---------------------
// Registration manager
// ---------------------
template <class ObjectType>
class TRegistrationManager
{
private:
	bool m_bAllocated;

	TArray< TObjectPointer<ObjectType> > m_Storage;

	size_t m_szNFixedEntries;

public:
	TRegistrationManager();

	~TRegistrationManager() { Release(); }
	
	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(size_t szSNFixedEntries);

	size_t Register(TObjectPointer<ObjectType>&	Object,
					size_t						szIndex = -1);

	void Unregister(size_t szIndex);

	ObjectType*			GetObject(size_t szIndex);
	const ObjectType*	GetObject(size_t szIndex) const;

	ObjectType*			operator [] (size_t szIndex)		{ return GetObject(szIndex); }
	const ObjectType*	operator [] (size_t szIndex) const	{ return GetObject(szIndex); }

	bool IsValidIndex(size_t szIndex) const;

	// ---------------- TRIVIALS ----------------
	size_t GetNFixedEntries() const { return m_szNFixedEntries; }
};

template <class ObjectType>
TRegistrationManager<ObjectType>::TRegistrationManager()
{
	m_bAllocated = false;
}

template <class ObjectType>
void TRegistrationManager<ObjectType>::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
#ifdef _DEBUG

		for(size_t i = 0 ; i < m_Storage.GetN() ; i++)
		{
			if(IsValidIndex(i))
				ShowErrorText((KString)TEXT("Storage item at index ") + i + TEXT(" was not unregistered."));
		}

#endif // _DEBUG

		m_bAllocated = false;

		m_Storage.Clear();
	}
}

template <class ObjectType>
void TRegistrationManager<ObjectType>::Allocate(size_t szSNFixedEntries)
{
	Release();

	try
	{
		m_szNFixedEntries = szSNFixedEntries;

		m_Storage.Add(m_szNFixedEntries);

		m_bAllocated = true;
	}
	
	catch(...)
	{
		Release(true);
		throw;
	}
}

template <class ObjectType>
size_t TRegistrationManager<ObjectType>::Register(	TObjectPointer<ObjectType>&	Object,
													size_t						szIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(Object.IsAllocated());

	DEBUG_VERIFY(szIndex == -1 || szIndex < m_szNFixedEntries);

	if(szIndex < m_szNFixedEntries) // fixed
	{
		DEBUG_VERIFY(!m_Storage[szIndex].IsAllocated());

		m_Storage[szIndex].ReOwn(Object);

		return szIndex;
	}
	else // custom
	{
		m_Storage.Add().ReOwn(Object);

		return m_Storage.GetN() - 1;		
	}
}

template <class ObjectType>
void TRegistrationManager<ObjectType>::Unregister(size_t szIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szIndex < m_szNFixedEntries || szIndex == m_Storage.GetLast());

	if(szIndex < m_szNFixedEntries) // fixed
	{		
		m_Storage[szIndex].Release();
	}
	else // custom
	{
		m_Storage.Del(szIndex);
	}
}

template <class ObjectType>
ObjectType* TRegistrationManager<ObjectType>::GetObject(size_t szIndex)
{
	DEBUG_VERIFY_ALLOCATION;

	return m_Storage[szIndex].GetDataPtr();
}

template <class ObjectType>
const ObjectType* TRegistrationManager<ObjectType>::GetObject(size_t szIndex) const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_Storage[szIndex].GetDataPtr();
}

template <class ObjectType>
bool TRegistrationManager<ObjectType>::IsValidIndex(size_t szIndex) const
{
	DEBUG_VERIFY_ALLOCATION;
	
	return szIndex < m_Storage.GetN() && m_Storage[szIndex].IsAllocated();
}

// ------------------
// Object registerer
// ------------------
template <class ObjectType>
class TObjectRegisterer
{
private:
	bool m_bAllocated;
	
	TRegistrationManager<ObjectType>* m_pRegistrationManager;

	size_t m_szIndex;

public:
	TObjectRegisterer();

	~TObjectRegisterer() { Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(	TRegistrationManager<ObjectType>&	SRegistrationManager,
					TObjectPointer<ObjectType>&			Object,
					size_t								szIndex = -1);

	size_t GetIndex() const;

	operator size_t () const { return GetIndex(); }

	// ---------------- TRIVIALS ----------------
	TRegistrationManager<ObjectType>* GetRegistrationManager() { return m_pRegistrationManager; }
};

template <class ObjectType>
TObjectRegisterer<ObjectType>::TObjectRegisterer()
{
	m_bAllocated = false;

	m_pRegistrationManager = NULL;
}

template <class ObjectType>
void TObjectRegisterer<ObjectType>::Release()
{
	if(IsAllocated())
	{
		m_bAllocated = false;

		SAFE_BLOCK_BEGIN m_pRegistrationManager->Unregister(m_szIndex); SAFE_BLOCK_END
	}

	m_pRegistrationManager = NULL;
}

template <class ObjectType>
void TObjectRegisterer<ObjectType>::Allocate(	TRegistrationManager<ObjectType>&	SRegistrationManager,
												TObjectPointer<ObjectType>&			Object,
												size_t								szIndex)
{
	Release();

	try
	{
		m_pRegistrationManager = &SRegistrationManager;

		m_szIndex = m_pRegistrationManager->Register(Object, szIndex);

		m_bAllocated = true;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

template <class ObjectType>
size_t TObjectRegisterer<ObjectType>::GetIndex() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_szIndex;
}

#endif // registration_manager_h
