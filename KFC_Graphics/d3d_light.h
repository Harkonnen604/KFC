#ifndef d3d_light_h
#define d3d_light_h

// ----------
// D3D light
// ----------
struct TD3DLight
{
    D3DLIGHTTYPE    m_Type;
    TD3DColor       m_DiffuseColor;
    TD3DColor       m_SpecularColor;
    TD3DColor       m_AmbientColor;
    TD3DVector      m_Coords;
    TD3DVector      m_Direction;
    float           m_fRange;
    float           m_fFalloff;
    float           m_fAttenuation0;
    float           m_fAttenuation1;
    float           m_fAttenuation2;
    float           m_fHotSpotTheta;
    float           m_fHotSpotPhi;


    TD3DLight(  bool            bSetDefaults    = true,
                D3DLIGHTTYPE    SType           = D3DLIGHT_DIRECTIONAL)
        { if(bSetDefaults) SetDefaults(SType); }

    TD3DLight& SetDefaults(D3DLIGHTTYPE SType);

    operator D3DLIGHT9* ()
        { return (D3DLIGHT9*)this; }

    operator const D3DLIGHT9* () const
        { return (const D3DLIGHT9*)this; }

    void Install(size_t szIndex, bool bSetEnable = true, bool bEnable = true) const;

    static void Enable(size_t szIndex, bool bEnable = true);
};

#endif // d3d_light_h
