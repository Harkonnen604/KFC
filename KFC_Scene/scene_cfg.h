#ifndef scene_cfg_h
#define scene_cfg_h

#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// ----------
// Scene cfg
// ----------
class TSceneCfg :
    public TGlobals,
    public TAssignmentsList
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

    void LoadItems(KRegistryKey& Key);
    void SaveItems(KRegistryKey& Key) const;

public:
    TSceneCfg();

    void Load();
    void Save() const;
};

extern TSceneCfg g_SceneCfg;

#endif // scene_cfg_h
