#ifndef physics_initials_h
#define physics_initials_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// -----------------
// Physics initials
// -----------------
class TPhysicsInitials :
    public TGlobals,
    public TAssignmentsList
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

    void LoadItems(KRegistryKey& Key);
    void SaveItems(KRegistryKey& Key) const;

public:
    TPhysicsInitials();

    void Load();
    void Save() const;
};

extern TPhysicsInitials g_PhysicsInitials;

#endif // physics_initials_h
