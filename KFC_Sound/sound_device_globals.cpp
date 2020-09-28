#include "kfc_sound_pch.h"
#include "sound_device_globals.h"

#include <KFC_KTL\ktl_globals.h>
#include <KFC_Common\common_globals.h>
#include <KFC_Windows\windows_device_globals.h>
#include "sound_cfg.h"
#include "sound_initials.h"
#include "sound_tokens.h"

TSoundDeviceGlobals g_SoundDeviceGlobals;

// ---------------------
// Sound device globals
// ---------------------
TSoundDeviceGlobals::TSoundDeviceGlobals() : TGlobals(TEXT("Sound device globals"))
{
    AddSubGlobals(g_SoundCfg);
    AddSubGlobals(g_SoundInitials);
    AddSubGlobals(g_SoundTokens);

    // Devices
    m_pDMusic               = NULL;
    m_pDSound               = NULL;
    m_pDMusicLoader         = NULL;
    m_pDMusicPerformance    = NULL;
    m_pDefaultAudioPath     = NULL;
}

void TSoundDeviceGlobals::OnUninitialize()
{
    // Buffers
    m_PrimarySoundBuffer.Release();

    // Devices
    if(m_pDefaultAudioPath)
        m_pDefaultAudioPath->Release(), m_pDefaultAudioPath = NULL;

    if(m_pDMusicPerformance)
    {
        m_pDMusicPerformance->Stop(NULL, NULL, 0, 0);
        m_pDMusicPerformance->CloseDown();

        m_pDMusicPerformance->Release();

        m_pDMusicPerformance = NULL;
    }

    if(m_pDMusicLoader)
        m_pDMusicLoader->Release(), m_pDMusicLoader = NULL;

    // COM
    m_COMInitializer.Release();
}

void TSoundDeviceGlobals::OnInitialize()
{
    HRESULT r;

    // COM
    m_COMInitializer.Allocate();

    // Loader
    if(r = CoCreateInstance(CLSID_DirectMusicLoader,
                            NULL,
                            CLSCTX_INPROC,
                            IID_IDirectMusicLoader8,
                            (void**)&m_pDMusicLoader))
    {
        m_pDMusicLoader = NULL;
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error acquring DirectMusic loader interface"), r);
    }

    // Performance
    if(r = CoCreateInstance(CLSID_DirectMusicPerformance,
                            NULL,
                            CLSCTX_INPROC,
                            IID_IDirectMusicPerformance8,
                            (void**)&m_pDMusicPerformance))
    {
        m_pDMusicPerformance = NULL;
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error acquring DirectMusic performance interface"), r);
    }

    // Initializing
    {
        IDirectMusic* pDM = NULL;
        IDirectSound* pDS = NULL;

        try
        {
            if(r = m_pDMusicPerformance->InitAudio( &pDM,
                                                    &pDS,
                                                    g_WindowsDeviceGlobals.GetWindow(),
                                                    DMUS_APATH_DYNAMIC_STEREO,
                                                    64,
                                                    DMUS_AUDIOF_ALL,
                                                    NULL))
            {
                INITIATE_DEFINED_FAILURE(TEXT("Error initializing DirectMusic performance audio."));
            }

            if(r = pDM->QueryInterface(IID_IDirectMusic8, (void**)&m_pDMusic))
            {
                m_pDMusic = NULL;
                INITIATE_DEFINED_FAILURE(TEXT("Error acquiring IDirectMusic8 interface."));
            }

            if(r = pDS->QueryInterface(IID_IDirectSound8, (void**)&m_pDSound))
            {
                m_pDSound = NULL;
                INITIATE_DEFINED_FAILURE(TEXT("Error acquiring IDirectSound8 interface."));
            }
        }

        catch(...)
        {
            if(pDS)
                pDS->Release(), pDS = NULL;

            if(pDM)
                pDM->Release(), pDM = NULL;

            throw;
        }

        pDS->Release(), pDS = NULL;
        pDM->Release(), pDM = NULL;
    }

    // Setting cooperative level
    if(r = m_pDSound->SetCooperativeLevel(g_WindowsDeviceGlobals.GetWindow(), DSSCL_PRIORITY))
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error setting DirectSound cooperative level."), r);

    // Allocating primary sound buffer
    m_PrimarySoundBuffer.AllocatePrimary();

    // Setting primary sound buffer format
    {
        TWaveFormat Format;

        Format.wFormatTag       = WAVE_FORMAT_PCM;
        Format.nChannels        = g_SoundCfg.m_szNChannels;
        Format.nSamplesPerSec   = g_SoundCfg.m_szFrequency;
        Format.wBitsPerSample   = (WORD)g_SoundCfg.m_szBPS;
        Format.nBlockAlign      = (WORD)((Format.wBitsPerSample * Format.nChannels) >> 3);
        Format.nAvgBytesPerSec  = Format.nBlockAlign * Format.nSamplesPerSec;

        m_PrimarySoundBuffer.SetFormat(Format);
    }

    // Getting default audiopath
    if(r = m_pDMusicPerformance->GetDefaultAudioPath(&m_pDefaultAudioPath))
    {
        m_pDefaultAudioPath = NULL;
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error retrieving default audio path"), r);
    }
}

bool TSoundDeviceGlobals::OnSuspend()
{
    if(!TGlobals::OnSuspend())
        return false;

    // {{{

    return true;
}

bool TSoundDeviceGlobals::OnResume()
{
    if(!TGlobals::OnResume())
        return false;

    // {{{

    return true;
}
