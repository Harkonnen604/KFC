#include "kfc_common_pch.h"
#include "common_device_globals.h"

#include "common_tls_item.h"
#include "common_consts.h"
#include "file_table.h"
#include "dde.h"

TCommonDeviceGlobals g_CommonDeviceGlobals;

// ----------------------
// Common device globals
// ----------------------
TCommonDeviceGlobals::TCommonDeviceGlobals() : TGlobals(TEXT("Common device globals"))
{
}

void TCommonDeviceGlobals::OnUninitialize()
{
	#ifdef _MSC_VER
	{
		T_DDE_Client::UnregisterWindowClass  ();
		T_DDE_Server::UnregisterWindowClasses();
	}
	#endif // _MSC_VER

	#ifdef _MSC_VER
	{
		g_pCOM_GIT.Release();

		m_COM_Initializer.Release();
		
		m_COM_Initializer.Release();
	}
	#endif // _MSC_VER
	
	#ifdef _MSC_VER
	{
		m_MsgBoxesEvent.Release();
	}
	#endif // _MSC_VER
	
	TFileTableTLS_Item::Free();

	TCommonTLS_Item::FreeItemType();
}

void TCommonDeviceGlobals::OnInitialize()
{
	TCommonTLS_Item::ReserveItemType();

	TFileTableTLS_Item::Reserve();

	#ifdef _MSC_VER
	{
		KFC_VERIFY(!(g_CommonConsts.m_bInitOLE && g_CommonConsts.m_bInitCOM));

		if(g_CommonConsts.m_bInitOLE)
			m_OLE_Initializer.Allocate();

		if(g_CommonConsts.m_bInitCOM)
			m_COM_Initializer.Allocate(g_CommonConsts.m_flCOM_Init);

		if(g_CommonConsts.m_bInitCOM_GIT)
			g_pCOM_GIT.CreateObject(CLSID_StdGlobalInterfaceTable, IID_IGlobalInterfaceTable);
	}
	#endif // _MSC_VER

	m_MsgBoxesEvent.Allocate(true, true);

	#ifdef _MSC_VER
	{
		if(g_CommonConsts.m_bInitDDE)
		{
			T_DDE_Server::RegisterWindowClasses();
			T_DDE_Client::RegisterWindowClass  ();
		}
	}
	#endif // _MSC_VER
}
