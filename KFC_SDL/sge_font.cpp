#include "kfc_sdl_pch.h"
#include "sge_font.h"

#include <KFC_Common/file.h>

// ---------
// SGE font
// ---------
T_SGE_Font::T_SGE_Font()
{
    m_pFont = NULL;
}

T_SGE_Font::T_SGE_Font(LPCTSTR pFileName, size_t szSize)
{
    m_pFont = NULL;

    Load(pFileName, szSize);
}

void T_SGE_Font::Release()
{
    if(m_pFont)
        sge_TTF_CloseFont(m_pFont), m_pFont = NULL;
}

void T_SGE_Font::Load(LPCTSTR pFileName, size_t szSize)
{
    Release();

    if(!FileExists(pFileName)) // SGE SIGSEGV protection
        INITIATE_DEFINED_FAILURE((KString)"SGE font file not found: \"" + pFileName + "\".");

    if(!(m_pFont = sge_TTF_OpenFont(pFileName, szSize)))
        INITIATE_DEFINED_FAILURE((KString)"Error opening SGE TTF font. File: \"" + pFileName + "\", size: " + szSize + "pt.");
}
