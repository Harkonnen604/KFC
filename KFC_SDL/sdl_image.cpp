#include "kfc_sdl_pch.h"
#include "sdl_image.h"

#include <KFC_Image/image.h>
#include "sdl_common.h"

// ----------
// SDL image
// ----------
T_SDL_Image::T_SDL_Image()
{
    m_pSurface = NULL;
}

T_SDL_Image::T_SDL_Image(const T_SDL_Image& Image)
{
    m_pSurface = NULL;

    *this = Image;
}

T_SDL_Image::T_SDL_Image(LPCTSTR pFileName)
{
    m_pSurface = NULL;

    Load(pFileName);
}

T_SDL_Image::T_SDL_Image(const SZSIZE& Size)
{
    m_pSurface = NULL;

    Create(Size);
}

void T_SDL_Image::Release()
{
    if(m_pSurface)
        SDL_FreeSurface(m_pSurface), m_pSurface = NULL;
}

void T_SDL_Image::Create(const SZSIZE& Size)
{
    Release();

    DEBUG_VERIFY(Size.IsPositive());

    if(!(m_pSurface = SDL_CreateRGBSurface( SDL_SWSURFACE,
                                            Size.cx,
                                            Size.cy,
                                            g_pSDL_FB->format->BitsPerPixel,
                                            g_pSDL_FB->format->Rmask,
                                            g_pSDL_FB->format->Gmask,
                                            g_pSDL_FB->format->Bmask,
                                            g_pSDL_FB->format->Amask)))
    {
        INITIATE_DEFINED_FAILURE("Error creating SDL image.");
    }

    if(g_pSDL_FB->format->palette)
        SDL_SetPalette(m_pSurface, SDL_LOGPAL, g_pSDL_FB->format->palette->colors, 0, g_pSDL_FB->format->palette->ncolors);
}

template <class t>
void ReformatNoLockBPP(const TImage& Image, SDL_Surface* pSurface)
{
    DEBUG_VERIFY(Image.IsAllocated());
    DEBUG_VERIFY(pSurface);
    DEBUG_VERIFY(!pSurface->locked);
    DEBUG_VERIFY(Image.GetWidth() == pSurface->w && Image.GetHeight() == pSurface->h);

    T_SDL_SurfaceLocker Locker0(pSurface);

    const DWORD* pSrc = Image.GetDataPtr();

    t* pDst = (t*)pSurface->pixels;

    size_t szDstDlt = pSurface->pitch - pSurface->w * sizeof(t);

    for(size_t y = Image.GetHeight() ; y ; y--, (BYTE*&)pDst += szDstDlt)
    {
        for(size_t x = Image.GetWidth() ; x ; x--, pSrc++, pDst++)
            *pDst = GetSDL_Color(pSurface, RGB((*pSrc >> 16) & 0xFF, (*pSrc >> 8) & 0xFF, *pSrc & 0xFF));
    }
}

void T_SDL_Image::Load(LPCTSTR pFileName)
{
    Release();

    try
    {
        TImage Image(pFileName);

        Create(Image.GetSize());

        if(m_pSurface->format->BytesPerPixel == 1)
            ReformatNoLockBPP<BYTE>(Image, *this);
        else if(m_pSurface->format->BytesPerPixel == 2)
            ReformatNoLockBPP<WORD>(Image, *this);
        else if(m_pSurface->format->BytesPerPixel == 4)
            ReformatNoLockBPP<DWORD>(Image, *this);
        else
            INITIATE_DEFINED_FAILURE("Unsupported color depth for SDL surface loading.");

        m_uiColorKey = UINT_MAX;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void T_SDL_Image::SetColorKey(UINT32 uiColorKey)
{
    DEBUG_VERIFY_ALLOCATION;

    m_uiColorKey = uiColorKey;

    if(m_uiColorKey == UINT_MAX)
    {
        if(SDL_SetColorKey(m_pSurface, 0, 0))
            INITIATE_DEFINED_FAILURE("Error removing SDL image color key.");
    }
    else
    {
        if(SDL_SetColorKey(m_pSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, GetSDL_Color(m_pSurface, m_uiColorKey)))
            INITIATE_DEFINED_FAILURE("Error setting SDL image color key.");
    }
}

T_SDL_Image& T_SDL_Image::operator = (const T_SDL_Image& Image)
{
    if(&Image == this)
        return *this;

    Release();

    try
    {
        if(!(m_pSurface = SDL_CreateRGBSurface( (Image.m_pSurface->flags & SDL_HWSURFACE) ? SDL_HWSURFACE : SDL_SWSURFACE,
                                                Image.m_pSurface->w,
                                                Image.m_pSurface->h,
                                                Image.m_pSurface->format->BitsPerPixel,
                                                Image.m_pSurface->format->Rmask,
                                                Image.m_pSurface->format->Gmask,
                                                Image.m_pSurface->format->Bmask,
                                                Image.m_pSurface->format->Amask)))
        {
            INITIATE_DEFINED_FAILURE("Error creating SDL image.");
        }

        if(Image.m_pSurface->format->palette)
            SDL_SetPalette(m_pSurface, SDL_LOGPAL, Image.m_pSurface->format->palette->colors, 0, Image.m_pSurface->format->palette->ncolors);

        {
            T_SDL_SurfaceLocker Locker0(Image);
            T_SDL_SurfaceLocker Locker1(m_pSurface);

            const BYTE* pSrc = (const BYTE*)Image.m_pSurface->pixels;
            BYTE*       pDst = (      BYTE*)      m_pSurface->pixels;

            size_t szStride = m_pSurface->w * (m_pSurface->format->BitsPerPixel >> 3);

            size_t szSrcPitch = Image.m_pSurface->pitch;
            size_t szDstPitch =       m_pSurface->pitch;

            for(size_t i = m_pSurface->h ; i ; i--, pSrc += szSrcPitch, pDst += szDstPitch)
                memcpy(pDst, pSrc, szStride);
        }

        if(Image.HasColorKey())
        {
            int c1 = GetSDL_Color(Image.m_pSurface, Image.GetColorKey());
            int c2 = GetSDL_Color(m_pSurface, Image.GetColorKey());

            SetColorKey(Image.GetColorKey());
        }

        SetColorKey(Image.GetColorKey());
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

// ----------------
// SDL multi-image
// ----------------
T_SDL_MultiImage::T_SDL_MultiImage()
{
}

T_SDL_MultiImage::T_SDL_MultiImage(LPCTSTR pFileName, bool bSingle)
{
    Load(pFileName, bSingle);
}

void T_SDL_MultiImage::Release()
{
    for(size_t i = 2 ; i != UINT_MAX ; i--)
    {
        for(size_t j = 2 ; j != UINT_MAX ; j--)
            m_Images[i][j].Release();
    }
}

void T_SDL_MultiImage::Load(LPCTSTR pFileName, bool bSingle)
{
    Release();

    try
    {
        if(bSingle)
        {
            m_Images[1][1].Load(pFileName);
        }
        else
        {
            m_Images[0][0].Load(TTokens("[place]", "tl")(pFileName));
            m_Images[0][1].Load(TTokens("[place]", "tc")(pFileName));
            m_Images[0][2].Load(TTokens("[place]", "tr")(pFileName));

            m_Images[1][0].Load(TTokens("[place]", "cl")(pFileName));
            m_Images[1][1].Load(TTokens("[place]", "cc")(pFileName));
            m_Images[1][2].Load(TTokens("[place]", "cr")(pFileName));

            m_Images[2][0].Load(TTokens("[place]", "bl")(pFileName));
            m_Images[2][1].Load(TTokens("[place]", "bc")(pFileName));
            m_Images[2][2].Load(TTokens("[place]", "br")(pFileName));

            if( m_Images[0][0].GetHeight() != m_Images[0][1].GetHeight() ||
                m_Images[0][2].GetHeight() != m_Images[0][1].GetHeight() ||

                m_Images[2][0].GetHeight() != m_Images[2][1].GetHeight() ||
                m_Images[2][2].GetHeight() != m_Images[2][1].GetHeight() ||

                m_Images[0][0].GetWidth() != m_Images[1][0].GetWidth() ||
                m_Images[2][0].GetWidth() != m_Images[1][0].GetWidth() ||

                m_Images[0][2].GetWidth() != m_Images[1][2].GetWidth() ||
                m_Images[2][2].GetWidth() != m_Images[1][2].GetWidth())
            {
                INITIATE_DEFINED_FAILURE((KString)"SDL multi-image set \"" + pFileName + "\" has mismatching component dimensions.");
            }
        }
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void T_SDL_MultiImage::Draw(SDL_Surface* pSurface, const IRECT& Rect, bool bUpdate) const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(pSurface);
    DEBUG_VERIFY(!pSurface->locked);

    if(!Rect.IsValid())
        return;

    if(IsSingle())
    {
        StretchBlitSDL_Image(m_Images[1][1], pSurface, Rect, false);
    }
    else
    {
        int lw = m_Images[0][0].GetWidth();
        int rw = m_Images[0][2].GetWidth();

        int th = m_Images[0][0].GetHeight();
        int bh = m_Images[2][0].GetHeight();

        if(Rect.GetWidth() < lw + rw)
        {
            lw = lw * Rect.GetWidth() / (lw + rw);
            rw = Rect.GetWidth() - lw;
        }

        if(Rect.GetHeight() < th + bh)
        {
            th = th * Rect.GetHeight() / (th + bh);
            bh = Rect.GetHeight() - th;
        }

        StretchBlitSDL_Image(m_Images[0][0], pSurface, IRECT(Rect.m_Left,       Rect.m_Top, Rect.m_Left  + lw,  Rect.m_Top + th), false);
        StretchBlitSDL_Image(m_Images[0][1], pSurface, IRECT(Rect.m_Left  + lw, Rect.m_Top, Rect.m_Right - rw,  Rect.m_Top + th), false);
        StretchBlitSDL_Image(m_Images[0][2], pSurface, IRECT(Rect.m_Right - rw, Rect.m_Top, Rect.m_Right,       Rect.m_Top + th), false);

        StretchBlitSDL_Image(m_Images[1][0], pSurface, IRECT(Rect.m_Left,       Rect.m_Top + th, Rect.m_Left  + lw, Rect.m_Bottom - bh), false);
        StretchBlitSDL_Image(m_Images[1][1], pSurface, IRECT(Rect.m_Left + lw,  Rect.m_Top + th, Rect.m_Right - rw, Rect.m_Bottom - bh), false);
        StretchBlitSDL_Image(m_Images[1][2], pSurface, IRECT(Rect.m_Right - rw, Rect.m_Top + th, Rect.m_Right,      Rect.m_Bottom - bh), false);

        StretchBlitSDL_Image(m_Images[2][0], pSurface, IRECT(Rect.m_Left,       Rect.m_Bottom - bh, Rect.m_Left  + lw,  Rect.m_Bottom), false);
        StretchBlitSDL_Image(m_Images[2][1], pSurface, IRECT(Rect.m_Left  + lw, Rect.m_Bottom - bh, Rect.m_Right - rw,  Rect.m_Bottom), false);
        StretchBlitSDL_Image(m_Images[2][2], pSurface, IRECT(Rect.m_Right - rw, Rect.m_Bottom - bh, Rect.m_Right,       Rect.m_Bottom), false);
    }

    if(bUpdate)
        UpdateSDL_Image(pSurface, Rect);
}
