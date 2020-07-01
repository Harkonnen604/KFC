#ifndef scene_globals_h
#define scene_globals_h

#include <KFC_KTL\module_globals.h>

// --------------
// Scene globals
// --------------
class TSceneGlobals : public TModuleGlobals
{
public:
	TSceneGlobals();
};

extern TSceneGlobals g_SceneGlobals;

#endif // scene_globals_h