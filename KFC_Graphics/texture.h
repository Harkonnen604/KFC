#ifndef texture_h
#define texture_h

#include "d3d_inc.h"
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_Common\image.h>
#include "graphics_device_globals.h"
#include "color_defs.h"

// --------
// Texture
// --------
class TTexture
{
private:
    LPDIRECT3DTEXTURE9 m_pTexture;

    SZSIZE      m_Size;
    size_t      m_szLevels;
    D3DFORMAT   m_Format;

    FSIZE m_InvSize;

public:
    TTexture();

    TTexture(LPDIRECT3DTEXTURE9 pSTexture);

    ~TTexture()
        { Release(); }

    bool IsAllocated() const
        { return m_pTexture; }

    void Release();

    void Allocate(  const SZSIZE&   SSize,
                    D3DFORMAT       SFormat,
                    D3DPOOL         Pool        = D3DPOOL_MANAGED,
                    flags_t         flUsage     = D3DUSAGE_AUTOGENMIPMAP,
                    size_t          szSLevels   = 0);

    void Allocate(LPDIRECT3DTEXTURE9 pSTexture);

    void CreateFromImage(   const TImage&   Image,
                            D3DFORMAT       SFormat,
                            const SZRECT*   pSrcRect        = NULL,
                            bool            bClampOutside   = true,
                            D3DPOOL         Pool            = D3DPOOL_MANAGED,
                            flags_t         flUsage         = D3DUSAGE_AUTOGENMIPMAP,
                            size_t          szSLevels       = 0);

    void FillFromImage( const TImage&   Image,
                        const SZRECT*   pSrcRect        = NULL,
                        bool            bClampOutside   = true);

    void Lock(  void*&          pRData,
                size_t&         szRPitch,
                size_t          szLevel = 0,
                const SZRECT*   pRect   = NULL,
                flags_t         flFlags = 0);

    void Lock(  BYTE*&          pRData,
                size_t&         szRPitch,
                size_t          szLevel = 0,
                const SZRECT*   pRect   = NULL,
                flags_t         flFlags = 0);

    void Lock(  WORD*&          wRData,
                size_t&         szRPitch,
                size_t          szLevel = 0,
                const SZRECT*   pRect   = NULL,
                flags_t         flFlags = 0);

    void Lock(  DWORD*&         pRData,
                size_t&         szRPitch,
                size_t          szLevel = 0,
                const SZRECT*   pRect   = NULL,
                flags_t         flFlags = 0);

    void Unlock(size_t szLevel);

    void Install(size_t szStage = 0) const;

    float GetTextureCoordX(float fX) const
        { return fX * m_InvSize.cx; }

    float GetTextureCoordY(float fY) const
        { return fY * m_InvSize.cy; }

    void DrawRect(  const FPOINT&   DstCoords,
                    D3DCOLOR        Color       = WhiteColor(),
                    const FSIZE*    pDstSize    = NULL,
                    const FRECT*    pSrcRect    = NULL) const;

    void DrawNonScaledRect( const FRECT&    DstRect,
                            D3DCOLOR        Color = WhiteColor()) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DrawRect(   FPOINT( DstRect.m_Left,
                            DstRect.m_Top),
                    Color,
                    &FSIZE(DstRect),
                    &FRECT( 0.0f,
                            0.0f,
                            GetTextureCoordX(DstRect.GetWidth ()),
                            GetTextureCoordY(DstRect.GetHeight())));
    }

    void DrawNonScaled( const FPOINT&   DstCoords,
                        D3DCOLOR        Color) const
    {
        DEBUG_VERIFY_ALLOCATION;

        DrawRect(DstCoords, Color);
    }

    operator LPDIRECT3DTEXTURE9 ();

    LPDIRECT3DSURFACE9 GetSurface(size_t szLevel = 0);

    // ---------------- TRIVIALS ----------------
    const SZSIZE&   GetSize     () const { return m_Size;       }
    size_t          GetLevels   () const { return m_szLevels;   }
    D3DFORMAT       GetFormat   () const { return m_Format;     }
};

// ---------------
// Texture locker
// ---------------
class TTextureLocker
{
private:
    TTexture&   m_Texture;
    size_t      m_szLevel;

public:
    TTextureLocker( TTexture&       STexture,
                    void*&          pRData,
                    size_t&         szRPitch,
                    size_t          szSLevel    = 0,
                    const SZRECT*   pRect       = NULL,
                    flags_t         flFlags     = 0);

    TTextureLocker( TTexture&       STexture,
                    BYTE*&          pRData,
                    size_t&         szRPitch,
                    size_t          szSLevel    = 0,
                    const SZRECT*   pRect       = NULL,
                    flags_t         flFlags     = 0);

    TTextureLocker( TTexture&       STexture,
                    WORD*&          pRData,
                    size_t&         szRPitch,
                    size_t          szSLevel    = 0,
                    const SZRECT*   pRect       = NULL,
                    flags_t         flFlags     = 0);

    TTextureLocker( TTexture&       STexture,
                    DWORD*&         pRData,
                    size_t&         szRPitch,
                    size_t          szSLevel    = 0,
                    const SZRECT*   pRect       = NULL,
                    flags_t         flFlags     = 0);

    ~TTextureLocker();

    // ---------------- TRIVIALS ----------------
    TTexture& GetTexture() { return m_Texture; }

    size_t GetLevel() const { return m_szLevel; }
};

#endif // texture_h
