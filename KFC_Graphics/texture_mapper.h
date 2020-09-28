#ifndef texture_mapper_h
#define texture_mapper_h

// ---------------
// Texture mapper
// ---------------
class TTextureMapper
{
public:
    virtual FPOINT GetTextureCoords(const TD3DVector& Coords) const = 0;

    virtual TPoint<bool> GetWrapTextureCoords() const = 0;
};

// ----------------------
// Planar texture mapper
// ----------------------
class TPlanarTextuerMapper : public TTextureMapper
{
private:
    TD3DVector m_Corner1;

    TD3DMatrix m_Conversion;


    TPlanarTextuerMapper();

public:
    TPlanarTextuerMapper(   const TD3DVector& Corner1,
                            const TD3DVector& Corner2,
                            const TD3DVector& Corner3);

    FPOINT GetTextureCoords(const TD3DVector& Coords) const;

    TPoint<bool> GetWrapTextureCoords() const
        { return TPoint<bool>(false, false); }
};

// ---------------------------
// Cylindrical texture mapper
// ---------------------------
class TCylindricalTextureMapper : public TTextureMapper
{
private:
    TD3DVector m_FirstBaseCenter;

    TD3DMatrix m_Conversion;


    TCylindricalTextureMapper();

public:
    // 'V' spans from 'BaseCenter.m_First' till 'BaseCenter.m_Last',
    // 'U' wraps CCW using 'U0Direction' as X, and 'U1_4Direction' as Y'
    // 'U0Direction' and 'U1_4Direction' must be perpendicular
    TCylindricalTextureMapper
        (   const TD3DVectorSegment&    BaseCenters,
            const TD3DVector&           U0Direction,
            const TD3DVector&           U1_4Direction);

    FPOINT GetTextureCoords(const TD3DVector& Coords) const;

    TPoint<bool> GetWrapTextureCoords() const
        { return TPoint<bool>(true, false); }
};

// -------------------------
// Spherical texture mapper
// -------------------------
class TSphericalTextureMapper : public TTextureMapper
{
private:
    TD3DVector m_Center;

    TD3DMatrix m_Conversion;


    TSphericalTextureMapper();

public:
    // 'U0Direction', 'U1_4Direction' and 'VDirection' must be all perpendicular
    TSphericalTextureMapper(const TD3DVector& Center,
                            const TD3DVector& U0Direction,
                            const TD3DVector& U1_4Direction,
                            const TD3DVector& VDirection);

    FPOINT GetTextureCoords(const TD3DVector& Coords) const;

    TPoint<bool> GetWrapTextureCoords() const
        { return TPoint<bool>(true, false); }
};

#endif // texture_mapper_h
