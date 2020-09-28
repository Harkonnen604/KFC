#ifndef scene_initials_h
#define scene_initials_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// ---------------
// Scene initials
// ---------------
class TSceneInitials :
    public TGlobals,
    public TAssignmentsList
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

    void LoadItems(KRegistryKey& Key);
    void SaveItems(KRegistryKey& Key) const;

public:
    TSceneInitials();

    void Load();
    void Save() const;
};

extern TSceneInitials g_SceneInitials;

#endif // scene_initials_h
