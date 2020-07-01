#ifndef scene_device_globals_h
#define scene_device_globals_h

#include <KFC_KTL\globals.h>

// ---------------------
// Scene device globals
// ---------------------
class TSceneDeviceGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();	

public:
	TSceneDeviceGlobals();
};

extern TSceneDeviceGlobals g_SceneDeviceGlobals;

#endif // scene_device_globals_h