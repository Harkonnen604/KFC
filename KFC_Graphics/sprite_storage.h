#ifndef sprite_storage_h
#define sprite_storage_h

#include <KFC_KTL\globals.h>
#include <KFC_KTL\registration_manager.h>
#include <KFC_Common\storage.h>
#include "sprite.h"

#define SPRITES_FACTORY					(g_SpriteStorage.m_Factory)
#define SPRITES_REGISTRATION_MANAGER	(g_SpriteStorage.m_RegistrationManager)

// ---------------
// Sprite storage
// ---------------
class TSpriteStorage :	public TGlobals,
						public TStorage<TSprite>
{
private:
	TFactoryTypesRegisterer<TSprite> m_SystemSpriteTypesRegisterer;


	void OnUninitialize	();
	void OnInitialize	();

	void LoadByDirectValue(	const KString&				FileName,
							TObjectPointer<TSprite>&	RObject,
							bool						bOmittable);

public:
	TSpriteStorage();
};

extern TSpriteStorage g_SpriteStorage;

#endif // sprite_storage_h
