#ifndef gui_cfg_h
#define gui_cfg_h

#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// --------
// GUI cfg
// --------
class T_GUI_Cfg :
    public TGlobals,
    public TAssignmentsList
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

    void LoadItems(KRegistryKey& Key);
    void SaveItems(KRegistryKey& Key) const;

public:
    T_GUI_Cfg();

    void Load();
    void Save() const;
};

extern T_GUI_Cfg g_GUI_Cfg;

#endif // gui_cfg_h
