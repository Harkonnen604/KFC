#include "kfc_graphics_pch.h"
#include "graphics_device_globals.h"

#include <KFC_KTL\kstring.h>
#include <KFC_KTL\globals_collector.h>
#include <KFC_Common\file.h>
#include <KFC_Common\common_globals.h>
#include <KFC_Common\startup_globals.h>
#include <KFC_GUI\gui.h>
#include <KFC_Windows\windows_device_globals.h>
#include "graphics_consts.h"
#include "graphics_cfg.h"
#include "graphics_initials.h"
#include "graphics_tokens.h"
#include "color_defs.h"

TGraphicsDeviceGlobals g_GraphicsDeviceGlobals;

// ------------------------
// Graphics device globals
// ------------------------
TGraphicsDeviceGlobals::TGraphicsDeviceGlobals() :
    TGlobals(TEXT("Graphics globals"))
{
    AddSubGlobals(g_GraphicsCfg);
    AddSubGlobals(g_GraphicsInitials);
    AddSubGlobals(g_GraphicsTokens);

    // Interfaces
    m_pD3D          = NULL;
    m_pD3DDevice    = NULL;

    // Formats
    m_ImageFormat = D3DFMT_UNKNOWN;

    m_16BPPTextureFormat    = D3DFMT_UNKNOWN;
    m_32BPPTextureFormat    = D3DFMT_UNKNOWN;
    m_DefaultTextureFormat  = D3DFMT_UNKNOWN;

    m_16BPPAlphaTextureFormat   = D3DFMT_UNKNOWN;
    m_32BPPAlphaTextureFormat   = D3DFMT_UNKNOWN;
    m_DefaultAlphaTextureFormat = D3DFMT_UNKNOWN;

    // Limits
    m_szMaxActiveLights = 0;

    m_bRecordingStateBlock = false;
}

void TGraphicsDeviceGlobals::OnUninitialize()
{
    // Screenshot surface
    m_ScreenshotSurface.Release();

    // Rects strip buffers
    m_RectsStripIndexBuffer.Release();

    m_RectsStripVertexBuffer.Release();

    // Back buffer
    m_BackBuffer.Release();

    // Limits
    m_szMaxActiveLights = 0;

    // Formats
    m_ImageFormat = D3DFMT_UNKNOWN;

    m_16BPPTextureFormat    = D3DFMT_UNKNOWN;
    m_32BPPTextureFormat    = D3DFMT_UNKNOWN;
    m_DefaultTextureFormat  = D3DFMT_UNKNOWN;

    m_16BPPAlphaTextureFormat   = D3DFMT_UNKNOWN;
    m_32BPPAlphaTextureFormat   = D3DFMT_UNKNOWN;
    m_DefaultAlphaTextureFormat = D3DFMT_UNKNOWN;

    // Interfaces
    if(m_pD3DDevice)
        m_pD3DDevice->Release(), m_pD3DDevice = NULL;

    if(m_pD3D)
        m_pD3D->Release(), m_pD3D = NULL;
}

void TGraphicsDeviceGlobals::OnInitialize()
{
    HRESULT r;

    // Window rect
    {
        IRECT ClientScreen;

        FitRect(IRECT(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)),
                TO_I(g_GraphicsCfg.m_Resolution),
                ClientScreen);

        SetKClientScreenRect(g_WindowsDeviceGlobals.GetWindow(), ClientScreen);
    }

    // Activation mode
    {
        g_WindowsDeviceGlobals.SetActivationMode(   g_GraphicsCfg.m_bFullScreen ?
                                                        TWindowsDeviceGlobals::AM_WINDOW_ACTIVE :
                                                        TWindowsDeviceGlobals::AM_WINDOW_RESTORED);
    }

    // Formats
    {
        if(g_GraphicsCfg.m_szColorDepth == 15)
            m_ImageFormat = D3DFMT_X1R5G5B5;
        else if(g_GraphicsCfg.m_szColorDepth == 16)
            m_ImageFormat = D3DFMT_R5G6B5;
        else if(g_GraphicsCfg.m_szColorDepth == 32)
            m_ImageFormat = D3DFMT_X8R8G8B8;
        else
            INITIATE_DEFINED_FAILURE(TEXT("Incorrect color depth specified."));

        m_16BPPTextureFormat = m_ImageFormat;
        m_32BPPTextureFormat = D3DFMT_X8R8G8B8;

        m_DefaultTextureFormat  = g_GraphicsCfg.m_bUse32BPPTextures ?
                                    m_32BPPTextureFormat :
                                    m_16BPPTextureFormat;

        m_16BPPAlphaTextureFormat = D3DFMT_A4R4G4B4;
        m_32BPPAlphaTextureFormat = D3DFMT_A8R8G8B8;

        m_DefaultAlphaTextureFormat = g_GraphicsCfg.m_bUse32BPPAlphaTextures ?
                                        m_32BPPAlphaTextureFormat :
                                        m_16BPPAlphaTextureFormat;
    }

    // D3D
    {
        m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
        if(m_pD3D==NULL)
            INITIATE_DEFINED_FAILURE(TEXT("Error initializing Direct3D."));
    }

    // Screen dimensions
    {
        m_ScreenRect.Set(   0, 0,
                            (float)g_GraphicsCfg.m_Resolution.cx,
                            (float)g_GraphicsCfg.m_Resolution.cy);

        m_ScreenSize = m_ScreenRect;

        m_ScreenCenter = m_ScreenRect.GetCenter();

        m_fXYAspectRatio = (float)g_GraphicsCfg.m_Resolution.cx / g_GraphicsCfg.m_Resolution.cy;
        m_fYXAspectRatio = (float)g_GraphicsCfg.m_Resolution.cy / g_GraphicsCfg.m_Resolution.cx;
    }

    // Presentation parameters
    {
        memset(&m_PresentationParameters, 0, sizeof(m_PresentationParameters));

        m_PresentationParameters.BackBufferWidth    = (size_t)m_ScreenSize.cx;
        m_PresentationParameters.BackBufferHeight   = (size_t)m_ScreenSize.cy;
        m_PresentationParameters.BackBufferFormat   = m_ImageFormat;
        m_PresentationParameters.BackBufferCount    = 1;
        m_PresentationParameters.MultiSampleType    = D3DMULTISAMPLE_NONE;
        m_PresentationParameters.SwapEffect         = g_GraphicsCfg.m_bFullScreen ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_COPY;
        m_PresentationParameters.hDeviceWindow      = g_WindowsDeviceGlobals.GetWindow();
        m_PresentationParameters.Windowed           = !g_GraphicsCfg.m_bFullScreen;

        try
        {
            if(g_GraphicsCfg.m_szZBufferDepth)
            {
                m_PresentationParameters.EnableAutoDepthStencil = TRUE;

                switch(g_GraphicsCfg.m_szZBufferDepth)
                {
                case 15:
                    switch(g_GraphicsCfg.m_szStencilBufferDepth)
                    {
                    case 1:
                        m_PresentationParameters.AutoDepthStencilFormat = D3DFMT_D15S1;
                        break;

                    default:
                        throw T_KFC_Exception();
                    }

                    break;

                case 16:
                    switch(g_GraphicsCfg.m_szStencilBufferDepth == 0)
                    {
                    case 0:
                        m_PresentationParameters.AutoDepthStencilFormat = D3DFMT_D16;
                        break;

                    default:
                        throw T_KFC_Exception();
                    }

                    break;

                case 24:
                    switch(g_GraphicsCfg.m_szStencilBufferDepth)
                    {
                    case 0:
                        m_PresentationParameters.AutoDepthStencilFormat = D3DFMT_D24X8;
                        break;

                    case 4:
                        m_PresentationParameters.AutoDepthStencilFormat = D3DFMT_D24X4S4;
                        break;

                    case 8:
                        m_PresentationParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
                        break;

                    default:
                        throw T_KFC_Exception();
                    }

                    break;

                default:
                    throw T_KFC_Exception();
                }
            }
            else
            {
                m_PresentationParameters.EnableAutoDepthStencil = FALSE;

                if(g_GraphicsCfg.m_szStencilBufferDepth)
                    throw T_KFC_Exception();
            }
        }

        catch(...)
        {
            INITIATE_DEFINED_FAILURE(TEXT("Incorrect Z buffer / stencil depths combination."));
        }

        m_PresentationParameters.Flags                      = 0;
        m_PresentationParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        m_PresentationParameters.PresentationInterval       = g_GraphicsCfg.m_bVSync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    // D3D device
    {
        if(r = m_pD3D->CreateDevice(g_GraphicsCfg.m_szGraphicsAdapter,
                                    D3DDEVTYPE_HAL,
                                    g_WindowsDeviceGlobals.GetWindow(),
                                    D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                    &m_PresentationParameters,
                                    &m_pD3DDevice))
        {
            m_pD3DDevice = NULL;
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error initializing Direct3D device."), r);
        }
    }

    // Retrieving default adapter caps
    {
        if(r = m_pD3DDevice->GetDeviceCaps(&m_D3DCaps))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error retrieving Direct3D device capabilities."), r);
    }

    // Retrieving texture caps
    {
        m_TextureSizeLimits.Set(Min(32u, (size_t)m_D3DCaps.MaxTextureWidth),
                                Min(32u, (size_t)m_D3DCaps.MaxTextureHeight),
                                m_D3DCaps.MaxTextureWidth,
                                m_D3DCaps.MaxTextureHeight);

        m_bPow2OnlyTextures     = (m_D3DCaps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0;
        m_bSquareOnlyTextures   = (m_D3DCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) != 0;

        if(m_bSquareOnlyTextures) // !!!
            INITIATE_DEFINED_FAILURE(TEXT("Square only textures restriction not supported."));
    }

    // Retrieving back buffer
    try
    {
        RetrieveBackBuffer();
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error retrieving Direct3D back buffer."));
    }

    // Limits
    m_szMaxActiveLights = Min((size_t)m_D3DCaps.MaxActiveLights, g_GraphicsCfg.m_szMaxActiveLights);

    // Preparing initial device states
    {
        m_pD3DDevice->SetRenderState(   D3DRS_DITHERENABLE,
                                        g_GraphicsCfg.m_szColorDepth == 32 ||
                                        g_GraphicsCfg.m_szColorDepth == 24 ?
                                            FALSE : TRUE);

        m_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

        m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
        m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        size_t i;

        for(i = 0 ; i < m_D3DCaps.MaxSimultaneousTextures ; i++)
        {
            m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

            m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        }

        for(i = 0 ; i < m_D3DCaps.MaxSimultaneousTextures ; i++)
        {
            m_pD3DDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            m_pD3DDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            m_pD3DDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, g_GraphicsCfg.m_bTrilinearFiltering ? D3DTEXF_LINEAR : D3DTEXF_POINT);
        }
    }

    // Setting viewport
    {
        D3DVIEWPORT9 Viewport;

        Viewport.X      = 0 ;
        Viewport.Y      = 0;
        Viewport.Width  = g_GraphicsCfg.m_Resolution.cx;
        Viewport.Height = g_GraphicsCfg.m_Resolution.cy;
        Viewport.MinZ   = 0.0f;
        Viewport.MaxZ   = 1.0f;

        if(r = m_pD3DDevice->SetViewport(&Viewport))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Eror setting viewport"), r);
    }

    // Rects strip buffers
    try
    {
        m_RectsStripVertexBuffer.Allocate(  g_GraphicsConsts.m_szMaxGraphicsStripRects << 2,
                                            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                                            D3DPOOL_SYSTEMMEM);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error creating rects strip vertex buffer."));
    }

    try
    {
        m_RectsStripIndexBuffer.Allocate(   g_GraphicsConsts.m_szMaxGraphicsStripRects * 6,
                                            D3DUSAGE_WRITEONLY);

        WORD* pDstData;

        TRectsStripIndexBufferLocker Locker1(m_RectsStripIndexBuffer, pDstData);

        for(size_t i = 0 ; i < g_GraphicsConsts.m_szMaxGraphicsStripRects ; i++)
        {
            size_t szBaseIndex = i << 2;

            *pDstData++ = szBaseIndex;
            *pDstData++ = szBaseIndex + 2;
            *pDstData++ = szBaseIndex + 1;

            *pDstData++ = szBaseIndex + 1;
            *pDstData++ = szBaseIndex + 2;
            *pDstData++ = szBaseIndex + 3;
        }
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error creating rects strip index buffer."));
    }

    try
    {
        // Screenshot surface
        m_ScreenshotSurface.Allocate(   g_GraphicsCfg.m_bFullScreen ?
                                            TO_SZ(m_ScreenSize) :
                                            SZSIZE( GetSystemMetrics(SM_CXSCREEN),
                                                    GetSystemMetrics(SM_CYSCREEN)),
                                        D3DFMT_A8R8G8B8,
                                        D3DPOOL_SYSTEMMEM);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error creating screenshot surface."));
    }
}

bool TGraphicsDeviceGlobals::OnPreUpdate()
{
    if(!TGlobals::OnPreUpdate())
        return false;

    m_bScreenShotInitiated = false;

    return true;
}

bool TGraphicsDeviceGlobals::OnPreRender() const
{
    if(!TGlobals::OnPreRender())
        return false;

    if(!StartFrame())
        return false;

    if(m_flClearingFlags)
    {
        if(m_pD3DDevice->Clear( 0,
                                NULL,
                                m_flClearingFlags,
                                m_ClearingColor,
                                m_fClearingDepth,
                                m_szClearingStencilValue))
        {
            return false;
        }
    }

    if(m_pD3DDevice->BeginScene())
        return false;

    return true;
}

void TGraphicsDeviceGlobals::OnPostRender(bool bFailureRollBack) const
{
    TGlobals::OnPostRender(bFailureRollBack);

    m_pD3DDevice->EndScene();

//  DEBUG_VERIFY(!m_RectsStripVertexBufferIncrementalLockManager.HasOverflowed());

    if(!bFailureRollBack)
    {
        if(m_bScreenShotInitiated)
        {
            RELEASE_SAFE_BLOCK_BEGIN
            {
                TSuspender Suspender0(g_GlobalsCollector);

                ScreenShot();
            }
            RELEASE_SAFE_BLOCK_END
        }

        m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    }
}

void TGraphicsDeviceGlobals::OnChangeMode()
{
    // Clearing states
    m_flClearingFlags           = D3DCLEAR_ZBUFFER;
    m_ClearingColor             = BlackColor();
    m_fClearingDepth            = 1.0f;
    m_szClearingStencilValue    = 0;
}

void TGraphicsDeviceGlobals::RetrieveBackBuffer() const
{
    LPDIRECT3DSURFACE9 pBackBufferSurface = NULL;

    int r;

    if(r = m_pD3DDevice->GetBackBuffer( 0,
                                        0,
                                        D3DBACKBUFFER_TYPE_MONO,
                                        &pBackBufferSurface))
    {
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error retrieving back buffer"), r);
    }

    m_BackBuffer.Allocate(pBackBufferSurface);
}

bool TGraphicsDeviceGlobals::StartFrame() const
{
    DEBUG_VERIFY_INITIALIZATION;

    HRESULT r = m_pD3DDevice->TestCooperativeLevel();

    if(r == D3D_OK)
        return true;

    if(r == D3DERR_DEVICENOTRESET)
    {
        m_BackBuffer.Release();

        if(m_pD3DDevice->Reset(&m_PresentationParameters) == D3D_OK)
        {
            RetrieveBackBuffer();
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

void TGraphicsDeviceGlobals::ScreenShot() const
{
    DEBUG_VERIFY_INITIALIZATION;

    static size_t szIndex = 0;

    KString FileName;

    while(FileExists(FileName.Format(   TEXT("%s%.4X%s"),
                                            (LPCTSTR)(g_StartUpGlobals.m_StartFolder +
                                                g_GraphicsConsts.m_ScreenShotsFolderName +
                                                g_GraphicsConsts.m_ScreenShotFileNamePrefix),
                                            szIndex++,
                                            (LPCTSTR)g_GraphicsConsts.m_ScreenShotFileNamePostfix)));

    if(!m_pD3DDevice->GetFrontBufferData(0, m_ScreenshotSurface))
    {
        CreateFoldersTree(GetFilePath(FileName));

        m_ScreenshotSurface.SaveContents(FileName);
    }
}

void TGraphicsDeviceGlobals::InitiateScreenShot()
{
    DEBUG_VERIFY_INITIALIZATION;

    m_bScreenShotInitiated = true;
}

void TGraphicsDeviceGlobals::BeginStateBlock()
{
    DEBUG_VERIFY(m_pD3DDevice);

    DEBUG_VERIFY(!m_bRecordingStateBlock);

    HRESULT r;

    if(r = m_pD3DDevice->BeginStateBlock())
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error recording Direct3D state block."), r);

    m_bRecordingStateBlock = true;
}

IDirect3DStateBlock9* TGraphicsDeviceGlobals::EndStateBlock()
{
    if(m_pD3DDevice == NULL)
        return NULL;

    if(!m_bRecordingStateBlock)
        return NULL;

    m_bRecordingStateBlock = false;

    IDirect3DStateBlock9* pStateBlock = NULL;

    if(m_pD3DDevice->EndStateBlock(&pStateBlock))
        return NULL;

    return pStateBlock;
}
