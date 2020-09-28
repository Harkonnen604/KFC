#include "kfc_sound_pch.h"
#include "plain_sounds.h"

#include <KFC_KTL\file_names.h>
#include "dmusic_inc.h"
#include "sound_device_globals.h"

// ------------------------------------
// Music segment sound creation struct
// ------------------------------------
TMusicSegmentSoundCreationStruct::TMusicSegmentSoundCreationStruct()
{
    m_szNRepeats            = 0;
    m_bAllowOnlyOneInstance = false;
    m_bPrimary              = false;
    m_bFailOnNoPort         = false;
}

void TMusicSegmentSoundCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
    TSoundCreationStruct::Load(InfoNode);

    TInfoParameterConstIterator PIter;

    // Getting filename
    if(!SetFileName(InfoNode->GetParameterValue(TEXT("FileName"))))
    {
        INITIATE_DEFINED_FAILURE((KString)TEXT("Invalid sound filename: \"") + m_FileName + TEXT("\"."));
    }

    // Getting nrepeats
    if((PIter = InfoNode->FindParameter(TEXT("NRepeats"))).IsValid())
    {
        ReadUINT(   PIter->m_Value,
                    m_szNRepeats,
                    TEXT("number of repeats"));
    }

    // Getting only one instance allowance
    if((PIter = InfoNode->FindParameter(TEXT("AllowOnlyOneInstance"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    m_bAllowOnlyOneInstance,
                    TEXT("only one instance allowance"));
    }

    // Getting primality
    if((PIter = InfoNode->FindParameter(TEXT("Primary"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    m_bPrimary,
                    TEXT("primality"));
    }

    // Getting no port failure
    if((PIter = InfoNode->FindParameter(TEXT("FailOnNoPort"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    m_bFailOnNoPort,
                    TEXT("no port failure"));
    }
}

bool TMusicSegmentSoundCreationStruct::SetFileName(const KString& SFileName)
{
    return FileExists(m_FileName = FILENAME_TOKENS.Process(SFileName));
}

// --------------------
// Music segment sound
// --------------------
TSound* TMusicSegmentSound::Create(type_t tpType)
{
    DEBUG_VERIFY(tpType == SOUND_TYPE_PLAIN);

    return new TMusicSegmentSound;
}

TMusicSegmentSound::TMusicSegmentSound()
{
    m_bAllocated = false;

    m_pSegment = NULL;
}

void TMusicSegmentSound::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        if(m_bAllocated)
            SAFE_BLOCK_BEGIN TMusicSegmentSound::Stop(); SAFE_BLOCK_END

        m_bAllocated = false;

        if(m_pSegment)
            m_pSegment->Release(), m_pSegment = NULL;

        TSound::Release();
    }
}

void TMusicSegmentSound::Allocate(const TMusicSegmentSoundCreationStruct& CreationStruct)
{
    Release();

    try
    {
        HRESULT r;

        TSound::Allocate(CreationStruct);

        // Fetching synthezied property
        KString FileExtension = GetFileExtension(CreationStruct.m_FileName);

        if( FileExtension.CollateNoCase(TEXT("mid"))    == 0 ||
            FileExtension.CollateNoCase(TEXT("midi"))   == 0 ||
            FileExtension.CollateNoCase(TEXT("sgt"))    == 0)
        {
            m_bSynthezied = true;
        }
        else
        {
            m_bSynthezied = false;
        }

        // Fetching other parameters
        m_bAllowOnlyOneInstance = CreationStruct.m_bAllowOnlyOneInstance;
        m_bPrimary              = CreationStruct.m_bPrimary;
        m_bFailOnNoPort         = CreationStruct.m_bFailOnNoPort;

        // Loading the segment
        TWideString WideFileName(CreationStruct.m_FileName);

        if(r =  g_SoundDeviceGlobals.m_pDMusicLoader->
                    LoadObjectFromFile( CLSID_DirectMusicSegment,
                                        IID_IDirectMusicSegment8,
                                        WideFileName,
                                        (void**)&m_pSegment))
        {
            m_pSegment = NULL;
            INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error loading plain sound: \"") + CreationStruct.m_FileName + TEXT("\""), r);
        }

        if(r = m_pSegment->Download(g_SoundDeviceGlobals.m_pDMusicPerformance))
            INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error downloading plain sound dependents: \"") + CreationStruct.m_FileName + TEXT("\""), r);

        m_bPlaying = false;

        m_bAllocated = true;

        // Setting nrepeats
        SetNRepeats(CreationStruct.m_szNRepeats);
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void TMusicSegmentSound::Load(TInfoNodeConstIterator InfoNode)
{
    Release();

    DEBUG_VERIFY(InfoNode.IsValid());

    TMusicSegmentSoundCreationStruct CreationStruct;
    CreationStruct.Load(InfoNode);

    Allocate(CreationStruct);
}

void TMusicSegmentSound::Play() const
{
    DEBUG_VERIFY_ALLOCATION;

    if(m_bAllowOnlyOneInstance && m_bPlaying)
        return;

    HRESULT r = g_SoundDeviceGlobals.m_pDMusicPerformance->
                        PlaySegmentEx(  m_pSegment,
                                        NULL,
                                        NULL,
                                        m_bPrimary ? 0 : DMUS_SEGF_SECONDARY,
                                        0,
                                        NULL,
                                        NULL,
                                        NULL);

    if(r)
    {
        if(r == DMUS_E_AUDIOPATH_INACTIVE)
        {
        }
        else if(r == DMUS_E_AUDIOPATH_NOPORT)
        {
            if(m_bFailOnNoPort)
                INITIATE_FAILURE;
        }
        else
        {
            INITIATE_FAILURE;
        }
    }

    m_bPlaying = true;
}

void TMusicSegmentSound::Stop() const
{
    DEBUG_VERIFY_ALLOCATION;

    if(!m_bPlaying)
        return;

    g_SoundDeviceGlobals.m_pDMusicPerformance->StopEx(m_pSegment, 0, 0);
    m_bPlaying = false;

    if(m_bSynthezied)
    {
        Play();
        g_SoundDeviceGlobals.m_pDMusicPerformance->StopEx(m_pSegment, 0, 0);
        m_bPlaying = false;
    }
}

bool TMusicSegmentSound::IsPlaying() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_bPlaying;
}

size_t TMusicSegmentSound::GetNRepeats() const
{
    DEBUG_VERIFY_ALLOCATION;

    return m_szNRepeats;
}

void TMusicSegmentSound::SetNRepeats(size_t szSNRepeats)
{
    DEBUG_VERIFY_ALLOCATION;

    m_szNRepeats = szSNRepeats;

    if(m_pSegment->SetRepeats(  m_szNRepeats == UINT_MAX ?
                                    DMUS_SEG_REPEAT_INFINITE :
                                    m_szNRepeats))
    {
        INITIATE_FAILURE;
    }
}

IDirectMusicSegment8* TMusicSegmentSound::GetSegment()
{
    DEBUG_VERIFY_ALLOCATION;

    return m_pSegment;
}
