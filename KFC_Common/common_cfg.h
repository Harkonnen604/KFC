#ifndef common_cfg_h
#define common_cfg_h

#include <KFC_KTL/globals.h>
#include "assignments.h"

#ifdef _MSC_VER

// -----------
// Common cfg
// -----------
class TCommonCfg :
    public TGlobals,
    public TAssignmentsList
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

    void LoadItems(KRegistryKey& Key);
    void SaveItems(KRegistryKey& Key) const;

public:
    TCommonCfg();

    void Load();
    void Save() const;
};

extern TCommonCfg g_CommonCfg;

#endif // _MSC_VER

#endif // common_cfg_h
