#include "kfc_graphics_pch.h"
#include "texture_image.h"

#include <KFC_Common\pow2.h>
#include "pixel_formats.h"

// #define REPORT_TEXTURE_IMAGE_BLOCK_SIZES

// --------------
// Texture image
// --------------
TTextureImage::TTextureImage()
{
    m_bAllocated = false;
}

void TTextureImage::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        m_Blocks.Release();
    }
}

void TTextureImage::Allocate(   const SZSIZE&   SSize,
                                D3DFORMAT       SFormat,
                                const SZRECT&   SBlockSizeLimits,
                                bool            bSSingleLowestSizeAllowed,
                                D3DPOOL         Pool,
                                flags_t         flUsage)
{
    Release();

    try
    {
        DEBUG_VERIFY(!SSize.IsFlat());

        DEBUG_VERIFY(SFormat != D3DFMT_UNKNOWN);

        // Setting size
        m_Size = SSize;

        // Settting single lowest size allowance
        m_bSingleLowestSizeAllowed = bSSingleLowestSizeAllowed;

        // Setting format
        m_Format = SFormat;

        // Evaluating block sizes
        SZSIZE BlockSizes[128];
        m_BlocksSize.Set(0, 0);

        size_t i;

        // {{{ check for different restrictions here
        {
            // Setting block size limits
            m_BlockSizeLimits.m_Left    = MinPow2GreaterEq  (SBlockSizeLimits.m_Left);
            m_BlockSizeLimits.m_Top     = MinPow2GreaterEq  (SBlockSizeLimits.m_Top);
            m_BlockSizeLimits.m_Right   = MaxPow2LessEq     (SBlockSizeLimits.m_Right);
            m_BlockSizeLimits.m_Bottom  = MaxPow2LessEq     (SBlockSizeLimits.m_Bottom);

            DEBUG_VERIFY(m_BlockSizeLimits.IsFlatValid());

            // Setting sizes clipped to high limits
            for(size_t szCurSize = 1 << ((sizeof(UINT) << 3) - 1) ;
                szCurSize ;
                szCurSize >>= 1)
            {
                if(m_Size.cx & szCurSize)
                {
                    if(szCurSize > m_BlockSizeLimits.m_Right)
                    {
                        for(i = szCurSize / m_BlockSizeLimits.m_Right ; i ; i--)
                            BlockSizes[m_BlocksSize.cx++].cx = m_BlockSizeLimits.m_Right;
                    }
                    else
                        BlockSizes[m_BlocksSize.cx++].cx = szCurSize;
                }

                if(m_Size.cy & szCurSize)
                {
                    if(szCurSize > m_BlockSizeLimits.m_Bottom)
                    {
                        for(i = szCurSize / m_BlockSizeLimits.m_Bottom ; i ; i--)
                            BlockSizes[m_BlocksSize.cy++].cy = m_BlockSizeLimits.m_Bottom;
                    }
                    else
                        BlockSizes[m_BlocksSize.cy++].cy = szCurSize;
                }
            }

            // Clipping sizes to low limits
            size_t szTotal;

            if(BlockSizes[m_BlocksSize.cx - 1].cx < m_BlockSizeLimits.m_Left)
            {
                if(!m_bSingleLowestSizeAllowed ||
                    m_BlocksSize.cx >= 2 &&
                    BlockSizes[m_BlocksSize.cx - 2].cx < m_BlockSizeLimits.m_Left)
                {
                    for(szTotal = 0 ;
                        m_BlocksSize.cx &&
                                BlockSizes[m_BlocksSize.cx - 1].cx <
                                    m_BlockSizeLimits.m_Left ;
                        szTotal += BlockSizes[--m_BlocksSize.cx].cx);

                    BlockSizes[m_BlocksSize.cx++].cx =  m_bSingleLowestSizeAllowed ?
                                                            MinPow2GreaterEq(szTotal) :
                                                            m_BlockSizeLimits.m_Left;

                    while(  m_BlocksSize.cx >= 2 &&
                            BlockSizes[m_BlocksSize.cx - 2].cx == BlockSizes[m_BlocksSize.cx - 1].cx &&
                            BlockSizes[m_BlocksSize.cx - 1].cx < m_BlockSizeLimits.m_Right)
                    {
                        BlockSizes[--m_BlocksSize.cx - 1].cx <<= 1;
                    }
                }
            }

            if(BlockSizes[m_BlocksSize.cy - 1].cy < m_BlockSizeLimits.m_Top)
            {
                if(!m_bSingleLowestSizeAllowed ||
                    m_BlocksSize.cy >= 2 &&
                    BlockSizes[m_BlocksSize.cy - 2].cy < m_BlockSizeLimits.m_Top)
                {
                    for(szTotal = 0 ;
                        m_BlocksSize.cy &&
                                BlockSizes[m_BlocksSize.cy - 1].cy <
                                    m_BlockSizeLimits.m_Top ;
                        szTotal += BlockSizes[--m_BlocksSize.cy].cy);

                    BlockSizes[m_BlocksSize.cy++].cy =  m_bSingleLowestSizeAllowed ?
                                                            MinPow2GreaterEq(szTotal) :
                                                            m_BlockSizeLimits.m_Top;

                    while(  m_BlocksSize.cy >= 2 &&
                            BlockSizes[m_BlocksSize.cy - 2].cy == BlockSizes[m_BlocksSize.cy - 1].cy &&
                            BlockSizes[m_BlocksSize.cy - 1].cy < m_BlockSizeLimits.m_Bottom)
                    {
                        BlockSizes[--m_BlocksSize.cy - 1].cy <<= 1;
                    }
                }
            }
        }

        // Creating the blocks
        m_Blocks.Allocate(m_BlocksSize);

        TTexture* pBlock = m_Blocks.GetDataPtr();
        for(size_t y = 0 ; y < m_BlocksSize.cy ; y++)
            for(size_t x = 0 ; x < m_BlocksSize.cx ; x++, pBlock++)
            {
                pBlock->Allocate(   SZSIZE(BlockSizes[x].cx, BlockSizes[y].cy),
                                    m_Format,
                                    Pool,
                                    flUsage);
            }

#ifdef _DEBUG
#ifdef REPORT_TEXTURE_IMAGE_BLOCK_SIZES

        g_DebugFile << "Size = " << SSize << DFCC_EOL;
        for(size_t q = 0 ; q < m_BlocksSize.cy ; q++)
        {
            for(size_t w = 0 ; w < m_BlocksSize.cx ; w++)
                g_DebugFile << m_Blocks(q, w).GetSize() << " ";

            g_DebugFile << DFCC_EOL;
        }

        g_DebugFile << DFCC_EOL;

#endif // REPORT_TEXTURE_IMAGE_BLOCK_SIZES
#endif // _DEBUG

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void TTextureImage::CreateFromImage(TImage&         Image,
                                    D3DFORMAT       SFormat,
                                    const SZRECT&   SBlockSizeLimits,
                                    bool            bSSingleLowestSizeAllowed,
                                    D3DPOOL         Pool,
                                    flags_t         flUsage)
{
    Release();

    try
    {
        DEBUG_VERIFY(Image.IsAllocated());

        // Allocating
        Allocate(   Image.GetSize(),
                    SFormat,
                    SBlockSizeLimits,
                    bSSingleLowestSizeAllowed,
                    Pool,
                    flUsage);

        // Filling the blocks
        TTexture* pBlock = m_Blocks.GetDataPtr();

        SZRECT BlockSrcRect;

        BlockSrcRect.m_Top = 0;
        for(size_t y = m_BlocksSize.cy ; y ; y--)
        {
            BlockSrcRect.m_Bottom = BlockSrcRect.m_Top + pBlock->GetSize().cy;
            if(BlockSrcRect.m_Bottom > m_Size.cy)
                BlockSrcRect.m_Bottom = m_Size.cy;

            BlockSrcRect.m_Left = 0;
            for(size_t x = m_BlocksSize.cx ; x ; x--, pBlock++)
            {
                BlockSrcRect.m_Right = BlockSrcRect.m_Left + pBlock->GetSize().cx;
                if(BlockSrcRect.m_Right > m_Size.cx)
                    BlockSrcRect.m_Right = m_Size.cx;

                pBlock->FillFromImage(Image, &BlockSrcRect, true);

                BlockSrcRect.m_Left = BlockSrcRect.m_Right;
            }

            BlockSrcRect.m_Top = BlockSrcRect.m_Bottom;
        }
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TTextureImage::DrawRect(   const FRECT&    DstRect,
                                D3DCOLOR        Color) const
{
    DEBUG_VERIFY_ALLOCATION;

    const TTexture* pBlock = m_Blocks.GetDataPtr();

    FRECT   CurDstRect;
    FSIZE   CurDstSize;
    FPOINT  CurDstScaledCoords;
    FSIZE   CurDstScaledSize;

    const FPOINT Limits(DstRect.m_Left  + m_Size.cx,
                        DstRect.m_Top   + m_Size.cy);

    const FSIZE DstSize = DstRect;

    const FSIZE Scale(  DstSize.cx / (float)m_Size.cx,
                        DstSize.cy / (float)m_Size.cy);

    CurDstRect.m_Top = DstRect.m_Top;
    for(size_t y = m_BlocksSize.cy ; y ; y--)
    {
        CurDstRect.m_Bottom = CurDstRect.m_Top + pBlock->GetSize().cy;
        if(CurDstRect.m_Bottom > Limits.y)
            CurDstRect.m_Bottom = Limits.y;

        CurDstSize.cy = CurDstRect.m_Bottom - CurDstRect.m_Top;

        CurDstScaledCoords.y    = DstRect.m_Top + (CurDstRect.m_Top - DstRect.m_Top) * Scale.cy;
        CurDstScaledSize.cy     = CurDstSize.cy * Scale.cy;

        CurDstRect.m_Left = DstRect.m_Left;
        for(size_t x = m_BlocksSize.cx ; x ; x--, pBlock++)
        {
            CurDstRect.m_Right = CurDstRect.m_Left + pBlock->GetSize().cx;
            if(CurDstRect.m_Right > Limits.x)
                CurDstRect.m_Right = Limits.x;

            CurDstSize.cx = CurDstRect.m_Bottom - CurDstRect.m_Top;

            const FSIZE CurDstSize = CurDstRect;

            CurDstScaledCoords.x    = DstRect.m_Left + (CurDstRect.m_Left - DstRect.m_Left) * Scale.cx;
            CurDstScaledSize.cx     = CurDstSize.cx * Scale.cx;

            const FRECT* pCurSrcRect = NULL;
            FRECT TempCurSrcRect;
            if(x==1 || y==1)
            {
                TempCurSrcRect.Set( 0.0f,
                                    0.0f,
                                    pBlock->GetTextureCoordX(CurDstSize.cx),
                                    pBlock->GetTextureCoordY(CurDstSize.cy));

                pCurSrcRect = &TempCurSrcRect;
            }

            pBlock->DrawRect(   CurDstScaledCoords,
                                Color,
                                &CurDstScaledSize,
                                pCurSrcRect);

            CurDstRect.m_Left = CurDstRect.m_Right;
        }

        CurDstRect.m_Top = CurDstRect.m_Bottom;
    }
}

void TTextureImage::DrawNonScaled(  FPOINT      DstCoords,
                                    D3DCOLOR    Color) const
{
    DEBUG_VERIFY_ALLOCATION;

    const DWORD dwColorAlpha = Color >> 24;

    if(dwColorAlpha == 0)
        return;

    RoundUp(DstCoords);

    const TTexture* pBlock = m_Blocks.GetDataPtr();

    FRECT CurDstRect;

    const FPOINT Limits(DstCoords.x + m_Size.cx,
                        DstCoords.y + m_Size.cy);

    CurDstRect.m_Top = DstCoords.y;
    for(size_t y = m_BlocksSize.cy ; y ; y--)
    {
        CurDstRect.m_Bottom = CurDstRect.m_Top + pBlock->GetSize().cy;
        if(CurDstRect.m_Bottom > Limits.y)
            CurDstRect.m_Bottom = Limits.y;

        CurDstRect.m_Left = DstCoords.x;
        for(size_t x = m_BlocksSize.cx ; x ; x--, pBlock++)
        {
            CurDstRect.m_Right = CurDstRect.m_Left + pBlock->GetSize().cx;
            if(CurDstRect.m_Right > Limits.x)
                CurDstRect.m_Right = Limits.x;

            pBlock->DrawNonScaledRect(CurDstRect, Color);

            CurDstRect.m_Left = CurDstRect.m_Right;
        }

        CurDstRect.m_Top = CurDstRect.m_Bottom;
    }
}
