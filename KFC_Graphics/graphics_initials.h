#ifndef graphics_initials_h
#define graphics_initials_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// ------------------
// Graphics initials
// ------------------
class TGraphicsInitials :
    public TGlobals,
    public TAssignmentsList
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

    void LoadItems(KRegistryKey& Key);
    void SaveItems(KRegistryKey& Key) const;

public:
    TGraphicsInitials();

    void Load();
    void Save() const;
};

extern TGraphicsInitials g_GraphicsInitials;

#endif // graphics_initials_h
