#ifndef graphics_device_globals_h
#define graphics_device_globals_h

#include "d3d_inc.h"
#include <KFC_KTL\globals.h>
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include "surface.h"
#include "vertex_buffer.h"
#include "vertex_formats.h"
#include "index_buffer.h"

// -----------------------------------
// Rects strip vertex buffer typedefs
// -----------------------------------
typedef TFVFVertexBuffer        < TTLVertex<1> > TRectsStripVertexBuffer;
typedef TFVFVertexBufferLocker  < TTLVertex<1> > TRectsStripVertexBufferLocker;

// ----------------------------------
// Rects strip index buffer typedefs
// ----------------------------------
typedef TIndexBuffer        <WORD> TRectsStripIndexBuffer;
typedef TIndexBufferLocker  <WORD> TRectsStripIndexBufferLocker;

// ------------------------
// Graphics device globals
// ------------------------
class TGraphicsDeviceGlobals : public TGlobals
{
private:
    bool m_bScreenShotInitiated;

    bool m_bRecordingStateBlock;

    TSurface m_ScreenshotSurface;


    void OnUninitialize ();
    void OnInitialize   ();

    bool OnPreUpdate();

    bool OnPreRender() const;

    void OnPostRender(bool bFailureRollBack) const;

    void OnChangeMode();

    void RetrieveBackBuffer() const;

    bool StartFrame() const;

    void ScreenShot() const;

public:
    // Interfaces
    LPDIRECT3D9         m_pD3D;
    LPDIRECT3DDEVICE9   m_pD3DDevice;

    // D3D caps
    D3DCAPS9 m_D3DCaps;

    // Presentation parameters
    mutable D3DPRESENT_PARAMETERS m_PresentationParameters;

    // Screen dimensions
    FRECT   m_ScreenRect;
    FSIZE   m_ScreenSize;
    FPOINT  m_ScreenCenter;

    float m_fXYAspectRatio;
    float m_fYXAspectRatio;

    // Formats
    D3DFORMAT m_ImageFormat;

    D3DFORMAT m_16BPPTextureFormat;
    D3DFORMAT m_32BPPTextureFormat;
    D3DFORMAT m_DefaultTextureFormat;

    D3DFORMAT m_16BPPAlphaTextureFormat;
    D3DFORMAT m_32BPPAlphaTextureFormat;
    D3DFORMAT m_DefaultAlphaTextureFormat;

    // Limits
    size_t m_szMaxActiveLights;

    // Texture caps
    SZRECT  m_TextureSizeLimits;
    bool    m_bPow2OnlyTextures;
    bool    m_bSquareOnlyTextures;

    // Back buffer
    mutable TSurface m_BackBuffer;

    // Rects strip buffers
    mutable TRectsStripVertexBuffer m_RectsStripVertexBuffer;
    mutable TRectsStripIndexBuffer  m_RectsStripIndexBuffer;

    // Clearing states
    flags_t     m_flClearingFlags;
    D3DCOLOR    m_ClearingColor;
    float       m_fClearingDepth;
    size_t      m_szClearingStencilValue;


    TGraphicsDeviceGlobals();

    void InitiateScreenShot();

    void BeginStateBlock();

    // Returns NULL in case of failure (including initialization/recursion failures)
    IDirect3DStateBlock9* EndStateBlock();

    // ---------------- TRIVIALS ----------------
    bool IsScreenshotInitiated() const { return m_bScreenShotInitiated; }
};

extern TGraphicsDeviceGlobals g_GraphicsDeviceGlobals;

#endif // graphics_device_globals_h
