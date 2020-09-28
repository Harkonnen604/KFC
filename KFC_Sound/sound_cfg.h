#ifndef sound_cfg_h
#define sound_cfg_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// ----------
// Sound cfg
// ----------
class TSoundCfg :
    public TGlobals,
    public TAssignmentsList
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

    void LoadItems(KRegistryKey& Key);
    void SaveItems(KRegistryKey& Key) const;

public:
    size_t m_szNChannels;
    size_t m_szFrequency;
    size_t m_szBPS;


    TSoundCfg();

    void Load();
    void Save() const;
};

extern TSoundCfg g_SoundCfg;

#endif // sound_cfg_h
