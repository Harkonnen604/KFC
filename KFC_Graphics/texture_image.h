#ifndef texture_image_h
#define texture_image_h

#include "d3d_inc.h"
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\vmatrix.h>
#include "texture.h"
#include "color_defs.h"

// --------------
// Texture image
// --------------
class TTextureImage
{
private:
    bool m_bAllocated;

    SZSIZE  m_Size;
    SZRECT  m_BlockSizeLimits;
    bool    m_bSingleLowestSizeAllowed;
    SZSIZE  m_BlocksSize;

public:
    typedef TValueMatrix<TTexture> TBlocks;

private:
    TBlocks m_Blocks;

    D3DFORMAT m_Format;

public:
    TTextureImage();

    ~TTextureImage()
        { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  const SZSIZE&   SSize,
                    D3DFORMAT       SFormat,
                    const SZRECT&   SBlockSizeLimits            = g_GraphicsDeviceGlobals.m_TextureSizeLimits,
                    bool            bSSingleLowestSizeAllowed   = true,
                    D3DPOOL         Pool                        = D3DPOOL_MANAGED,
                    flags_t         flUsage                     = 0);

    void CreateFromImage(   TImage&         Image,
                            D3DFORMAT       SFormat,
                            const SZRECT&   SBlockSizeLimits            = g_GraphicsDeviceGlobals.m_TextureSizeLimits,
                            bool            bSSingleLowestSizeAllowed   = true,
                            D3DPOOL         Pool                        = D3DPOOL_MANAGED,
                            flags_t         flUsage                     = 0);

    void DrawRect(  const FRECT&    DstRect,
                    D3DCOLOR        Color = WhiteColor()) const;

    void DrawNonScaled( FPOINT      DstCoords, // altered inside
                        D3DCOLOR    Color = WhiteColor()) const;

    // ---------------- TRIVALS ----------------
    const SZSIZE&   GetSize                     () const { return m_Size;                       }
    const SZRECT&   GetBlockSizeLimits          () const { return m_BlockSizeLimits;            }
    bool            IsSingleLowestSizeAllowed   () const { return m_bSingleLowestSizeAllowed;   }
    const SZSIZE&   GetBlocksSize               () const { return m_BlocksSize;                 }

    TBlocks&        GetBlocks()         { return m_Blocks; }
    const TBlocks&  GetBlocks() const   { return m_Blocks; }

    D3DFORMAT GetFormat() const { return m_Format; }
};

#endif // texture_image_h
