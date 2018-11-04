#include "kfc_gui_pch.h"
#include "gui_device_globals.h"

#include "gui_consts.h"
#include "gui_cfg.h"
#include "gui_initials.h"
#include "gui_tokens.h"
#include "color_label_control.h"
#include "splitter_control.h"

#include "resource.h"

T_GUI_DeviceGlobals g_GUI_DeviceGlobals;

// -------------------
// GUI device globals
// -------------------
T_GUI_DeviceGlobals::T_GUI_DeviceGlobals() : TGlobals(TEXT("GUI device globals"))
{
	AddSubGlobals(g_GUI_Cfg);
	AddSubGlobals(g_GUI_Initials);
	AddSubGlobals(g_GUI_Tokens);

	memset(m_CheckIcons, 0, sizeof(m_CheckIcons));
}

void T_GUI_DeviceGlobals::OnUninitialize()
{
	// Custom controls
	TSplitterControl::Unregister();

	TColorLabelControl::UnregisterClass();

	// Internal GDI objects
	m_CheckerBrush.Release();

	m_CheckIcons[1][1].Release();
	m_CheckIcons[1][0].Release();
	m_CheckIcons[0][1].Release();
	m_CheckIcons[0][0].Release();
}

void T_GUI_DeviceGlobals::OnInitialize()
{
	const HINSTANCE hInstance = GetKModuleHandle();

	// Loading internal GDI objects
	{
		if(g_GUI_Consts.m_bWithCheckIcons)
		{
			m_CheckIcons[0][0].Allocate(hInstance, IDI_CHECK00_ICON);
			m_CheckIcons[0][1].Allocate(hInstance, IDI_CHECK01_ICON);
			m_CheckIcons[1][0].Allocate(hInstance, IDI_CHECK10_ICON);
			m_CheckIcons[1][1].Allocate(hInstance, IDI_CHECK11_ICON);
		}

		{
			WORD v[8];

			for(size_t i = 0 ; i < 8 ; i++)
				v[i] = 0x55 << (i & 1);

			HBITMAP hBitmap = CreateBitmap(8, 8, 1, 1, v);
			KFC_VERIFY(hBitmap);

			m_CheckerBrush.Allocate(hBitmap);

			DeleteObject(hBitmap);
		}
	}

	// Initializing common controls
	{
		INITCOMMONCONTROLSEX Init;
		memset(&Init, 0, sizeof(Init)), Init.dwSize = sizeof(Init);

		Init.dwICC =
			ICC_PROGRESS_CLASS		|
			ICC_LISTVIEW_CLASSES	|
			ICC_TREEVIEW_CLASSES	|
			ICC_TAB_CLASSES			|
			ICC_BAR_CLASSES			|
			ICC_WIN95_CLASSES		|
			ICC_INTERNET_CLASSES	|
			ICC_DATE_CLASSES		|
			ICC_COOL_CLASSES;

		if(!InitCommonControlsEx(&Init))
			INITIATE_DEFINED_FAILURE(TEXT("Error initializing common controls."));
	}

	// Custom controls
	TColorLabelControl::RegisterClass();

	TSplitterControl::Register();
}