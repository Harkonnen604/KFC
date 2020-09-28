#include "kfc_sound_pch.h"
#include "sound_storage.h"

#include "sound_consts.h"
#include "sound_cfg.h"
#include "sound_initials.h"
#include "sound_tokens.h"
#include "sound_device_globals.h"
#include "plain_sounds.h"

TSoundStorage g_SoundStorage;

// --------------
// Sound storage
// --------------
TSoundStorage::TSoundStorage() : TGlobals(TEXT("Sound storage"))
{
    AddSubGlobals(g_SoundCfg);
    AddSubGlobals(g_SoundInitials);
    AddSubGlobals(g_SoundTokens);
    AddSubGlobals(g_SoundDeviceGlobals);
}

void TSoundStorage::OnUninitialize()
{
    m_SystemSoundTypesRegisterer.Release();

    TStorage<TSound>::Release();
}

void TSoundStorage::OnInitialize()
{
    // Storage
    try
    {
        TStorage<TSound>::Allocate( SOUND_TYPE_TOKENS,
                                    g_SoundConsts.m_szNSoundsRegistrationManagerFixedEntries,
                                    SOUND_INDEX_TOKENS);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error allocating sounds storage."));
    }

    // System sound types
    try
    {
        m_SystemSoundTypesRegisterer.Allocate(SOUNDS_FACTORY);

        m_SystemSoundTypesRegisterer.Add(TMusicSegmentSound::Create, SOUND_TYPE_PLAIN);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error registering system sound types."));
    }

    // System sounds
    try
    {
        TStructuredInfo Info(FILENAME_TOKENS.
            Process(TEXT("[StartFolder][DefinitionsFolder]System.Definition")));

        TInfoNodeConstIterator InfoNode =
            Info.GetNode(Info.GetRootNode(), TEXT("Sounds"));

        TObjectPointer<TSound> Sound;
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error loading system sounds."));
    }
}

void TSoundStorage::LoadByDirectValue(  const KString&          FileName,
                                        TObjectPointer<TSound>& RObject,
                                        bool                    bOmittable)
{
    TMusicSegmentSoundCreationStruct CreationStruct;

    if(!CreationStruct.SetFileName(FileName))
    {
        if(!bOmittable)
        {
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("Invalid direct value sound filename.") +
                                            FileName +
                                            TEXT("\"."));
        }
    }
    else
    {
        ((TMusicSegmentSound*)RObject.
            Allocate(new TMusicSegmentSound, false))->
                Allocate(CreationStruct);
    }
}
