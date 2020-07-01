#ifndef sound_storage_h
#define sound_storage_h

#include <KFC_KTL\globals.h>
#include <KFC_Common\storage.h>
#include "sound.h"
#include "sound_defs.h"

#define SOUNDS_FACTORY					(g_SoundStorage.m_Factory)
#define SOUNDS_REGISTRATION_MANAGER		(g_SoundStorage.m_RegistrationManager)

// --------------
// Sound storage
// --------------
class TSoundStorage :	public TGlobals,
						public TStorage<TSound>
{
private:
	TFactoryTypesRegisterer<TSound> m_SystemSoundTypesRegisterer;


	void OnInitialize	();
	void OnUninitialize	();

	void LoadByDirectValue(	const KString&			FileName,
							TObjectPointer<TSound>&	RObject,
							bool					bOmittable);

public:
	TSoundStorage();
};

extern TSoundStorage g_SoundStorage;

#endif // sound_storage_h
