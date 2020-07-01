#ifndef controls_factory_h
#define controls_factory_h

#include <KFC_KTL\globals.h>
#include <KFC_Common\persistence_factory.h>
#include "control.h"

// Speed defs
#define CONTROLS_FACTORY	(g_ControlsFactory)

// Declarations
class TControlsFactory;

// ------------------------
// Controls factory entry
// ------------------------
struct TControlsFactoryEntry
{
	typedef TControl* TControlLoader(	type_t					tpType,
										TInfoNodeConstIterator	InfoNode,
										const TControl*			pParentControl,
										const FRECT&			Resolution);


	TPSEGMENT		m_Types;
	TControlLoader*	m_pLoader;

	void Set(	const TPSEGMENT&	STypes,
				TControlLoader*		pSLoader)
	{
		m_Types		= STypes;
		m_pLoader	= pSLoader;
	}
};

// ----------------------------------
// Controls factory types registerer
// ----------------------------------
class TControlsFactoryTypesRegisterer
{
private:
	bool m_bAllocated;

	TControlsFactory* m_pFactory;

	size_t m_szN;

public:
	TControlsFactoryTypesRegisterer();

	TControlsFactoryTypesRegisterer(TControlsFactory& SFactory);

	~TControlsFactoryTypesRegisterer() { Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(TControlsFactory& SFactory);

	void Add(	TControlsFactoryEntry::TControlLoader*	pLoader,
				type_t									tpType,
				size_t									szAmt = 1);

	// ---------------- TRIVIALS ----------------
	TControlsFactory& GetFactory() { return *m_pFactory; }

	const TControlsFactory& GetFactory() const { return *m_pFactory; }

	size_t GetN() const { return m_szN; }
};

// -----------------
// Controls factory
// -----------------
class TControlsFactory :	public TGlobals,
							public TPersistenceFactory<TControl>
{
private:
	typedef TArray<TControlsFactoryEntry, true> TControlsFactoryEntries;

	
	TControlsFactoryEntries m_ControlsEntries;

	TControlsFactoryTypesRegisterer m_SystemControlTypesRegisterer;


	void OnUninitialize	();
	void OnInitialize	();

public:
	TControlsFactory();

	void UnregisterControls(size_t szN);
	
	void RegisterControl(	TControlsFactoryEntry::TControlLoader*	pLoader,
							type_t									tpType,
							size_t									szAmt = 1);

	TControl* LoadControl(	TInfoNodeConstIterator	InfoNode,
							const TControl*			pParentControl,
							const FRECT&			Resolution);
};

extern TControlsFactory g_ControlsFactory;

#endif // controls_factory_h