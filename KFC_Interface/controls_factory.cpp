#include "kfc_interface_pch.h"
#include "controls_factory.h"

#include "interface_consts.h"
#include "interface_cfg.h"
#include "interface_initials.h"
#include "interface_tokens.h"
#include "dummy_control.h"
#include "image_control.h"
#include "button_control.h"
#include "checkbox_control.h"
#include "scroll_control.h"

TControlsFactory g_ControlsFactory;

// ----------------------------------
// Controls factory types registerer
// ----------------------------------
TControlsFactoryTypesRegisterer::TControlsFactoryTypesRegisterer()
{
	m_bAllocated = false;

	m_pFactory = NULL;

	m_szN = 0;
}

TControlsFactoryTypesRegisterer::TControlsFactoryTypesRegisterer(TControlsFactory& SFactory)
{
	m_bAllocated = false;

	m_pFactory = NULL;

	m_szN = 0;

	Allocate(SFactory);
}

void TControlsFactoryTypesRegisterer::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		if(m_pFactory && m_pFactory->IsAllocated())
			m_pFactory->UnregisterControls(m_szN);

		m_pFactory = NULL;

		m_szN = 0;
	}
}

void TControlsFactoryTypesRegisterer::Allocate(TControlsFactory& SFactory)
{
	Release();

	try
	{
//		DEBUG_VERIFY(SFactory.IsAllocated());

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

void TControlsFactoryTypesRegisterer::Add(	TControlsFactoryEntry::TControlLoader*	pLoader,
											type_t									tpType,
											size_t									szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	m_pFactory->RegisterControl(pLoader, tpType, szAmt);

	m_szN++;
}

// -----------------
// Controls factory
// -----------------
TControlsFactory::TControlsFactory() : TGlobals(TEXT("Controls factory"))
{
	AddSubGlobals(g_InterfaceCfg);
	AddSubGlobals(g_InterfaceInitials);
	AddSubGlobals(g_InterfaceTokens);
}

void TControlsFactory::OnUninitialize()
{
	m_SystemControlTypesRegisterer.Release();

	m_ControlsEntries.Clear();

	TPersistenceFactory<TControl>::Release();
}

void TControlsFactory::OnInitialize()
{
	// Factory
	try
	{
		TPersistenceFactory<TControl>::Allocate(CONTROL_TYPE_TOKENS);
	}

	catch(...)
	{
		INITIATE_DEFINED_FAILURE(TEXT("Error allocating controls factory."));
	}

	// System control types
	try
	{
		m_SystemControlTypesRegisterer.Allocate(CONTROLS_FACTORY);

		m_SystemControlTypesRegisterer.Add(TDummyControl::			LoadControl, CONTROL_TYPE_DUMMY);
		m_SystemControlTypesRegisterer.Add(TImageControl::			LoadControl, CONTROL_TYPE_IMAGE);
		m_SystemControlTypesRegisterer.Add(TButtonControl::			LoadControl, CONTROL_TYPE_BUTTON);
		m_SystemControlTypesRegisterer.Add(TCheckBoxControl::		LoadControl, CONTROL_TYPE_CHECKBOX);
		m_SystemControlTypesRegisterer.Add(TScrollControl::			LoadControl, CONTROL_TYPE_SCROLL);
		m_SystemControlTypesRegisterer.Add(TEasyLabelControl::		LoadControl, CONTROL_TYPE_EASY_LABEL);
		m_SystemControlTypesRegisterer.Add(TEasyCheckBoxControl::	LoadControl, CONTROL_TYPE_EASY_CHECKBOX);
	}

	catch(...)
	{
		INITIATE_DEFINED_FAILURE(TEXT("Error registering system control types."));
	}
}

void TControlsFactory::UnregisterControls(size_t szN)
{
//	DEBUG_VERIFY_INITIALIZATION

	if(szN > m_ControlsEntries.GetN())
	{
		INITIATE_DEFINED_FAILURE(KString::Formatted(TEXT("Attemt to unregister %u controls factory entries while there exist only %u."),
														szN,
														m_ControlsEntries.GetN()));
	}

	m_ControlsEntries.Del(m_ControlsEntries.GetN() - szN, m_ControlsEntries.GetN());
}

void TControlsFactory::RegisterControl(	TControlsFactoryEntry::TControlLoader*	pLoader,
										type_t									tpType,
										size_t									szAmt)
{
//	DEBUG_VERIFY_INITIALIZATION;

	DEBUG_VERIFY(pLoader);

	m_ControlsEntries.Add().Set(TPSEGMENT(tpType, tpType + szAmt), pLoader);
}

TControl* TControlsFactory::LoadControl(TInfoNodeConstIterator	InfoNode,
										const TControl*			pParentControl,
										const FRECT&			Resolution)
{
	DEBUG_VERIFY_INITIALIZATION;

	DEBUG_VERIFY(InfoNode.IsValid());

	size_t i;

	const type_t tpType = ResolveType(InfoNode->GetParameterValue(TEXT("Type")));

	for(i = 0 ; i < m_ControlsEntries.GetN() ; i++)
	{
		if(HitsSegment(tpType, m_ControlsEntries[i].m_Types))
		{
			return m_ControlsEntries[i].m_pLoader(	tpType,
													InfoNode,
													pParentControl,
													Resolution);
		}
	}

	INITIATE_DEFINED_FAILURE(	(KString)TEXT("Control type ") +
									tpType +
									TEXT(" has no registered control loader in the controls factory."));
}