#ifndef sdl_consts_h
#define sdl_consts_h

#include <KFC_KTL/consts.h>
#include <KFC_Common/structured_info.h>

// -----------
// SDL consts
// -----------
class T_SDL_Consts : public TConsts
{
public:
    size_t m_szMaxResourceID;

    SZSIZE m_Resolution;
    size_t m_szBPP;

    bool m_bSGE_TTF_AlphaAA;

    SZSIZE m_DefaultInterfaceExtFocusSize;

    UINT32 m_uiDefaultInterfaceBG_Color;
    UINT32 m_uiDefaultInterfaceExtFocusColor;
    UINT32 m_uiHeaderBG_CN_Color;
    UINT32 m_uiHeaderBG_SC_Color;
    UINT32 m_uiHeaderBG_LT_Color;
    UINT32 m_uiHeaderBG_DK_Color;
    UINT32 m_uiLabelSelectionBG_Color;
    UINT32 m_uiListSelectionBG_Color;

public:
    T_SDL_Consts();

    void Load(TInfoNodeConstIterator Node);
};

extern T_SDL_Consts g_SDL_Consts;

#endif // sdl_consts_h
