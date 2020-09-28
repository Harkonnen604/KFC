#ifndef surface_h
#define surface_h

#include "d3d_inc.h"
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\kstring.h>
#include <KFC_Common\image.h>

// --------
// Surface
// --------
class TSurface
{
private:
    IDirect3DSurface9* m_pSurface;

    SZSIZE      m_Size;
    D3DFORMAT   m_Format;
    D3DPOOL     m_Pool;

public:
    TSurface();

    TSurface(IDirect3DSurface9* pSSurface);

    ~TSurface()
        { Release(); }

    bool IsAllocated() const
        { return m_pSurface; }

    void Release();

    void Allocate(const SZSIZE& SSize, D3DFORMAT SFormat, D3DPOOL Pool = D3DPOOL_MANAGED);

    void Allocate(IDirect3DSurface9* pSSurface);

    void CreateFromImage(   const TImage&   Image,
                            D3DFORMAT       SFormat,
                            const SZRECT*   pSrcRect        = NULL,
                            bool            bClampOutside   = true);

    void FillFromImage( const TImage&   Image,
                        const SZRECT*   pSrcRect        = NULL,
                        bool            bClampOutside   = true);

    void Lock(  void*&          pRData,
                size_t&         szRPitch,
                const SZRECT*   pRect   = NULL,
                flags_t         flFlags = 0);

    void Lock(  BYTE*&          pRData,
                size_t&         szRPitch,
                const SZRECT*   pRect   = NULL,
                flags_t         flFlags = 0);

    void Lock(  WORD*&          pRData,
                size_t&         szRPitch,
                const SZRECT*   pRect   = NULL,
                flags_t         flFlags = 0);

    void Lock(  DWORD*&         pRData,
                size_t&         szRPitch,
                const SZRECT*   pRect   = NULL,
                flags_t         flFlags = 0);

    void Unlock();

    void Copy(  TSurface&               DstSurface,
                const IPOINT&           DstCoords,
                const IRECT*            pSrcRect, // can be NULL
                D3DTEXTUREFILTERTYPE    Filter) const;

    void CopyClipped(   TSurface&               DstSurface,
                        IPOINT&                 DstCoords,
                        IRECT*                  pSrcRect,     // can be NULL
                        const IRECT*            pDstClipRect, // can be NULL
                        D3DTEXTUREFILTERTYPE    Filter) const;

    void CopyRects( TSurface&               DstSurface,
                    const IRECT*            pSrcRects,
                    const IPOINT*           pDstCoords,
                    size_t                  szN,
                    D3DTEXTUREFILTERTYPE    Filter) const;

    void CopyRectsClipped(  TSurface&               DstSurface,
                            const IRECT*            pSrcRects,  // can be NULL
                            const IPOINT*           pDstCoords, // can be NULL
                            size_t                  szN,
                            const IRECT*            pDstClipRect, // can be NULL
                            D3DTEXTUREFILTERTYPE    Filter) const;

    void SaveContents(const KString& FileName) const;

    IDirect3DSurface9* GetSurface() const
        { DEBUG_VERIFY_ALLOCATION; return m_pSurface; }

    operator IDirect3DSurface9* () const
        { return GetSurface(); }

    // ---------------- TRIVIALS ----------------
    const SZSIZE&   GetSize     () const { return m_Size;       }
    D3DFORMAT       GetFormat   () const { return m_Format;     }
};

// ---------------
// Surface locker
// ---------------
class TSurfaceLocker
{
private:
    TSurface& m_Surface;

public:
    TSurfaceLocker( TSurface&       SSurface,
                    void*&          pRData,
                    size_t&         szRPitch,
                    const SZRECT*   pRect   = NULL,
                    flags_t         flFlags = 0);

    TSurfaceLocker( TSurface&       SSurface,
                    BYTE*&          pRData,
                    size_t&         szRPitch,
                    const SZRECT*   pRect   = NULL,
                    flags_t         flFlags = 0);

    TSurfaceLocker( TSurface&       SSurface,
                    WORD*&          pRData,
                    size_t&         szRPitch,
                    const SZRECT*   pRect   = NULL,
                    flags_t         flFlags = 0);

    TSurfaceLocker( TSurface&       SSurface,
                    DWORD*&         pRData,
                    size_t&         szRPitch,
                    const SZRECT*   pRect   = NULL,
                    flags_t         flFlags = 0);

    ~TSurfaceLocker();

    // ---------------- TRIVIALS ----------------
    TSurface& GetSurface() { return m_Surface; }
};

#endif // surface_h
