#ifndef d3d_material_h
#define d3d_material_h

// -------------
// D3D material
// -------------
struct TD3DMaterial
{
    TD3DColor   m_DiffuseColor;
    TD3DColor   m_AmbientColor;
    TD3DColor   m_SpecularColor;
    TD3DColor   m_EmissiveColor;
    float       m_fPower;


    TD3DMaterial(bool bSetDefaults = true)
        { if(bSetDefaults) SetDefaults(); }

    TD3DMaterial& SetDefaults();

    operator D3DMATERIAL9* ()
        { return (D3DMATERIAL9*)this; }

    operator const D3DMATERIAL9* () const
        { return (const D3DMATERIAL9*)this; }

    void Install() const;
};

#endif // d3d_material_h
