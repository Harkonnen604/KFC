#include "kfc_sdl_pch.h"
#include "sdl_consts.h"

#include "sdl_common.h"

T_SDL_Consts g_SDL_Consts;

// -----------
// SDL consts
// -----------
T_SDL_Consts::T_SDL_Consts()
{
	m_szMaxResourceID = 16384 - 1;

	m_Resolution.Set(640, 480), m_szBPP = 16;

	m_bSGE_TTF_AlphaAA = true;

	m_DefaultInterfaceExtFocusSize.Set(8, 8);

	m_uiDefaultInterfaceBG_Color		= RGB(  0,   0,   0);
	m_uiDefaultInterfaceExtFocusColor	= RGB(255, 255, 255);
	m_uiHeaderBG_CN_Color				= RGB(160, 160, 160);
	m_uiHeaderBG_SC_Color				= RGB(150, 150, 150);
	m_uiHeaderBG_LT_Color				= RGB(204, 204, 204);
	m_uiHeaderBG_DK_Color				= RGB( 32,  32,  32);
	m_uiLabelSelectionBG_Color			= RGB(128, 128, 128);
	m_uiListSelectionBG_Color			= RGB(255, 255, 255);
}

void T_SDL_Consts::Load(TInfoNodeConstIterator Node)
{
	m_Resolution = ReadFromString<SZSIZE>(Node->GetParameterValue("Resolution"));

	m_szBPP = ReadFromString<size_t>(Node->GetParameterValue("BPP"));

	m_bSGE_TTF_AlphaAA = ReadFromString<bool>(Node->GetParameterValue("FontAntiAliasing"));

	m_uiDefaultInterfaceBG_Color = ReadRGB(Node->GetParameterValue("DefaultInterfaceBG_Color"));

	m_uiDefaultInterfaceExtFocusColor = ReadRGB(Node->GetParameterValue("DefaultInterfaceExtFocusColor"));

	m_DefaultInterfaceExtFocusSize = ReadFromString<SZSIZE>(Node->GetParameterValue("DefaultInterfaceExtFocusSize"));

	m_uiHeaderBG_CN_Color		= ReadRGB(Node->GetParameterValue("HeaderBG_CenterColor"));
	m_uiHeaderBG_SC_Color		= ReadRGB(Node->GetParameterValue("HeaderBG_ScrollColor"));
	m_uiHeaderBG_LT_Color		= ReadRGB(Node->GetParameterValue("HeaderBG_LightColor"));
	m_uiHeaderBG_DK_Color		= ReadRGB(Node->GetParameterValue("HeaderBG_DarkColor"));
	m_uiLabelSelectionBG_Color	= ReadRGB(Node->GetParameterValue("LabelSelectionBG_Color"));
	m_uiListSelectionBG_Color	= ReadRGB(Node->GetParameterValue("ListSelectionBG_Color"));
}
