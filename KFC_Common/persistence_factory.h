#ifndef persistence_factory_h
#define persistence_factory_h

#include <KFC_KTL/factory.h>
#include <KFC_KTL/tokens.h>
#include "structured_info.h"

// --------------------------------
// Persistence factory direct item
// --------------------------------
template <class ObjectType>
struct TPersistenceFactoryDirectEntry
{
	typedef ObjectType* TDirectLoader(ktype_t tpType, TInfoNodeConstIterator InfoNode);


	TPSEGMENT		m_Types;
	TDirectLoader*	m_pDirectLoader;


	void Set(	const TPSEGMENT&	STypes,
				TDirectLoader*		pSDirectLoader)
	{
		m_Types			= STypes;
		m_pDirectLoader	= pSDirectLoader;
	}
};

// --------------------
// Persistence factory
// --------------------
template <class ObjectType>
class TPersistenceFactory : public TFactory<ObjectType>
{
public:
	typedef TPersistenceFactoryDirectEntry<ObjectType> TDirectEntry;

private:
	typedef TArray<TDirectEntry, true> TDirectEntries;

		
	bool m_bAllocated;		

	TDirectEntries m_DirectEntries;

	const TTokens* m_pTypeTokens;

public:
	TPersistenceFactory();

	~TPersistenceFactory() { Release(); }

	bool IsAllocated() const
		{ return TFactory<ObjectType>::IsAllocated() && m_bAllocated; }

	void Release(bool bFromAllocatorException = false);
	
	void Allocate(const TTokens& STypeTokens);

	void UnregisterDirect(size_t szN);

	void RegisterDirect(	typename TDirectEntry::TDirectLoader*	pDirectLoader,
							ktype_t									tpType,
							size_t									szAmt = 1);

	ktype_t ResolveType(const KString& Type) const;

	ObjectType* Load(TInfoNodeConstIterator InfoNode);

	ObjectType* LoadDirect(TInfoNodeConstIterator InfoNode);

	// ---------------- TRIVIALS ----------------
	const TTokens& GetTypeTokens() const { return *m_pTypeTokens; }
};

template <class ObjectType>
TPersistenceFactory<ObjectType>::TPersistenceFactory()
{
	m_bAllocated = false;

	m_pTypeTokens = NULL;
}

template <class ObjectType>
void TPersistenceFactory<ObjectType>::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		m_pTypeTokens = NULL;

		m_DirectEntries.Clear();

		TFactory<ObjectType>::Release();
	}
}

template <class ObjectType>
void TPersistenceFactory<ObjectType>::Allocate(const TTokens& STypeTokens)
{
	Release();

	try
	{
		TFactory<ObjectType>::Allocate();

		m_pTypeTokens = &STypeTokens;

		m_bAllocated = true;
	}
	
	catch(...)
	{
		Release(true);
		throw;
	}
}

template <class ObjectType>
void TPersistenceFactory<ObjectType>::UnregisterDirect(size_t szN)
{
	DEBUG_VERIFY_ALLOCATION;

	if(szN > m_DirectEntries.GetN())
	{
		INITIATE_DEFINED_FAILURE(KString::Formatted(TEXT("Attempt to unregister %u persistence factory direct loaders while there exist only %u."),
														szN,
														m_DirectEntries.GetN()));
	}

	m_DirectEntries.Del(m_DirectEntries.GetN() - szN, m_DirectEntries.GetN());
}

template <class ObjectType>
void TPersistenceFactory<ObjectType>::RegisterDirect(	typename TDirectEntry::TDirectLoader*	pDirectLodader,
														ktype_t									tpType,
														size_t									szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(pDirectLodader);

	m_DirectEntries.Add().Set(TPSEGMENT(tpType, tpType + szAmt), pDirectLodader);
}

template <class ObjectType>
ktype_t TPersistenceFactory<ObjectType>::ResolveType(const KString& Type) const
{
	DEBUG_VERIFY_ALLOCATION;

	ktype_t tpType;
	ReadUINT(m_pTypeTokens->Process(Type), tpType, TEXT("object type"));
	
	return tpType;
}

template <class ObjectType>
ObjectType* TPersistenceFactory<ObjectType>::Load(TInfoNodeConstIterator InfoNode)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(InfoNode.IsValid());

	TObjectPointer<ObjectType> Object(	Create(ResolveType(InfoNode->GetParameterValue(TEXT("Type")))),
										false);

	Object->Load(InfoNode);

	return Object.MakeExternal();
}

template <class ObjectType>
ObjectType* TPersistenceFactory<ObjectType>::LoadDirect(TInfoNodeConstIterator InfoNode)
{
	DEBUG_VERIFY_ALLOCATION;

	size_t i;

	DEBUG_VERIFY(InfoNode.IsValid());

	const ktype_t tpType = ResolveType(InfoNode->GetParameterValue(TEXT("Type")));

	for(i = 0 ; i < m_DirectEntries.GetN() ; i++)
	{
		if(HitsSegmentBounds(tpType, m_DirectEntries[i].m_Types))
			return m_DirectEntries[i].m_pDirectLoader(tpType, InfoNode);
	}

	INITIATE_DEFINED_FAILURE(	(KString)TEXT("No direct loader entry found for type ") +
									tpType +
									TEXT("."));
}

// --------------------------------------------
// Persistence factory direct types registerer
// --------------------------------------------
template <class ObjectType>
class TPersistenceFactoryDirectTypesRegisterer
{
private:
	bool m_bAllocated;

	TPersistenceFactory<ObjectType>* m_pFactory;

	size_t m_szN;

public:
	TPersistenceFactoryDirectTypesRegisterer();

	TPersistenceFactoryDirectTypesRegisterer(TPersistenceFactory<ObjectType>& SFactory);

	~TPersistenceFactoryDirectTypesRegisterer() { Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(TPersistenceFactory<ObjectType>& SFactory);

	void Add(	typename TPersistenceFactory<ObjectType>::TDirectEntry::TDirectLoader*	pDirectLoader,
				ktype_t																	tpType,
				size_t																	szAmt = 1);

	// ---------------- TRIVIALS ----------------
	TPersistenceFactory<ObjectType>& GetFactory() { return *m_pFactory; }

	const TPersistenceFactory<ObjectType>& GetFactory() const { return *m_pFactory; }

	size_t GetN() const { return m_szN; }
};

template <class ObjectType>
TPersistenceFactoryDirectTypesRegisterer<ObjectType>::TPersistenceFactoryDirectTypesRegisterer()
{
	m_bAllocated = false;

	m_pFactory = NULL;

	m_szN = 0;
}

template <class ObjectType>
TPersistenceFactoryDirectTypesRegisterer<ObjectType>::TPersistenceFactoryDirectTypesRegisterer(TPersistenceFactory<ObjectType>& SFactory)
{
	m_bAllocated = false;

	m_pFactory = NULL;

	m_szN = 0;

	Allocate(SFactory);
}

template <class ObjectType>
void TPersistenceFactoryDirectTypesRegisterer<ObjectType>::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;
		
		if(m_pFactory && m_pFactory->IsAllocated())
			m_pFactory->UnregisterDirect(m_szN);
		
		m_pFactory = NULL;

		m_szN = 0;
	}
}

template <class ObjectType>
void TPersistenceFactoryDirectTypesRegisterer<ObjectType>::Allocate(TPersistenceFactory<ObjectType>& SFactory)
{
	Release();

	try
	{
		DEBUG_VERIFY(SFactory.IsAllocated());

		m_pFactory = &SFactory;

		m_szN = 0;

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

template <class ObjectType>
void TPersistenceFactoryDirectTypesRegisterer<ObjectType>::Add(	typename TPersistenceFactory<ObjectType>::TDirectEntry::TDirectLoader*	pDirectLoader,
																ktype_t																	tpType,
																size_t																	szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	m_pFactory->RegisterDirect(pDirectLoader, tpType, szAmt);

	m_szN++;
}

#endif // persistence_factory_h
