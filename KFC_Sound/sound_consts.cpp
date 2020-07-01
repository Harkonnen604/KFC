#include "kfc_sound_pch.h"
#include "sound_consts.h"

TSoundConsts g_SoundConsts;

// -------------
// Sound consts
// -------------
TSoundConsts::TSoundConsts()
{
	// Registry
	m_RegistryKeyName = TEXT("Sound\\");

	// Filenames
	m_SoundsFolderName = TEXT("Sounds\\");

	// Storages
 	m_szNSoundsRegistrationManagerFixedEntries = 1024;
}
