#ifndef graphics_cfg_h
#define graphics_cfg_h

#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\globals.h>
#include <KFC_Common\assignments.h>

// -------------
// Graphics cfg
// -------------
class TGraphicsCfg :
    public TGlobals,
    public TAssignmentsList
{
private:
    void OnUninitialize ();
    void OnInitialize   ();

    void LoadItems(KRegistryKey& Key);
    void SaveItems(KRegistryKey& Key) const;

public:
    size_t  m_szGraphicsAdapter;
    SZSIZE  m_Resolution;
    size_t  m_szColorDepth;
    bool    m_bUse32BPPTextures;
    bool    m_bUse32BPPAlphaTextures;
    size_t  m_szZBufferDepth;
    size_t  m_szStencilBufferDepth;
    bool    m_bVSync;
    bool    m_bFullScreen;
    bool    m_bTrilinearFiltering;
    size_t  m_szMaxActiveLights;


    TGraphicsCfg();

    void Load();
    void Save() const;
};

extern TGraphicsCfg g_GraphicsCfg;

#endif // graphics_cfg_h
