#ifndef matrix_h
#define matrix_h

#include "vector.h"

// -----------
// Matrix 4x4
// -----------
template <class t>
struct TMatrix44
{
    union
    {
        struct
        {
            t _11, _12, _13, _14;
            t _21, _22, _23, _24;
            t _31, _32, _33, _34;
            t _41, _42, _43, _44;
        };

        t v[4][4];
    };

    TMatrix44() {}

    TMatrix44(const TMatrix44& SMatrix)
        { memcpy(v, SMatrix.v, sizeof(v)); }

    TMatrix44(  t s_11, t s_12, t s_13, t s_14,
                t s_21, t s_22, t s_23, t s_24,
                t s_31, t s_32, t s_33, t s_34,
                t s_41, t s_42, t s_43, t s_44) :

        _11(s_11), _12(s_12), _13(s_13), _14(s_14),
        _21(s_21), _22(s_22), _23(s_23), _24(s_24),
        _31(s_31), _32(s_32), _33(s_33), _34(s_34),
        _41(s_41), _42(s_42), _43(s_43), _44(s_44) {}

    TMatrix44(  const TVector3<t>& BassisX,
                const TVector3<t>& BassisY,
                const TVector3<t>& BassisZ,
                const TVector3<t>& PostOffset = TVector3<t>(0, 0, 0))
    {
        SetBassis(BassisX, BassisY, BassisZ, PostOffset);
    }

    TMatrix44& SetZero()
    {
        memset(v, 0, sizeof(v));
        return *this;
    }

    TMatrix44& SetIdentity()
    {
        _11 = 1, _12 = 0, _13 = 0, _14 = 0;
        _21 = 0, _22 = 1, _23 = 0, _24 = 0;
        _31 = 0, _32 = 0, _33 = 1, _34 = 0;
        _41 = 0, _42 = 0, _43 = 0, _44 = 1;

        return *this;
    }

    TMatrix44& SetTranslation(const TVector3<t>& v)
    {
        _11 = 1,   _12 = 0,    _13 = 0,    _14 = 0;
        _21 = 0,   _22 = 1,    _23 = 0,    _24 = 0;
        _31 = 0,   _32 = 0,    _33 = 1,    _34 = 0;
        _41 = v.x, _42 = v.y,  _43 = v.z,  _44 = 1;

        return *this;
    }

    TMatrix44& SetScale(const TVector3<t>& v)
    {
        _11 = v.x, _12 = 0,   _13 = 0,   _14 = 0;
        _21 = 0,   _22 = v.y, _23 = 0,   _24 = 0;
        _31 = 0,   _32 = 0,   _33 = v.z, _34 = 0;
        _41 = 0,   _42 = 0,   _43 = 0,   _44 = 1;

        return *this;
    }

    TMatrix44& SetRotateX(double ang)
    {
        const t cs = (t)cos(ang);
        const t sn = (t)sin(ang);

        _11 = 1, _12 = 0,   _13 =   0, _14 = 0;
        _21 = 0, _22 = +cs, _23 = +sn, _24 = 0;
        _31 = 0, _32 = -sn, _33 = +cs, _34 = 0;
        _41 = 0, _42 = 0,   _43 =   0, _44 = 1;

        return *this;
    }

    TMatrix44& SetRotateY(double ang)
    {
        const t cs = (t)cos(ang);
        const t sn = (t)sin(ang);

        _11 = +cs, _12 = 0, _13 = -sn, _14 = 0;
        _21 =  0,  _22 = 1, _23 =   0, _24 = 0;
        _31 = +sn, _32 = 0, _33 = +cs, _34 = 0;
        _41 =   0, _42 = 0, _43 =   0, _44 = 1;

        return *this;
    }

    TMatrix44& SetRotateZ(double ang)
    {
        const t cs = (t)cos(ang);
        const t sn = (t)sin(ang);

        _11 = +cs, _12 = +sn, _13 = 0, _14 = 0;
        _21 = -sn, _22 = +cs, _23 = 0, _24 = 0;
        _31 =   0, _32 =   0, _33 = 1, _34 = 0;
        _41 =   0, _42 =   0, _43 = 0, _44 = 1;

        return *this;
    }

    TMatrix44& SetBassis(   const TVector3<t>& BassisX,
                            const TVector3<t>& BassisY,
                            const TVector3<t>& BassisZ,
                            const TVector3<t>& PostOffset = TVector3<t>(0, 0, 0))
    {
        _11 = BassisX.x, _12 = BassisX.y, _13 = BassisX.z, _14 = 0;
        _21 = BassisY.x, _22 = BassisY.y, _23 = BassisY.z, _24 = 0;
        _31 = BassisZ.x, _32 = BassisZ.y, _33 = BassisZ.z, _34 = 0;
        _41 = PostOffset.x, _42 = PostOffset.y, _43 = PostOffset.z, _44 = 1;

        return *this;
    }

    TMatrix44& Flip()
    {
        _11 = -_11, _12 = -_12, _13 = -_13, _14 = -_14;
        _21 = -_21, _22 = -_22, _23 = -_23, _24 = -_24;
        _31 = -_31, _32 = -_32, _33 = -_33, _34 = -_34;
        _41 = -_41, _42 = -_42, _43 = -_43, _44 = -_44;

        return *this;
    }

    TMatrix44 Flipped() const
        { return TMatrix44(*this).Flip(); }

    // Const operators
    TMatrix44 operator + (const TMatrix44& SMatrix) const
        { return TMatrix44( _11 + SMatrix._11, _12 + SMatrix._12, _13 + SMatrix._13, _14 + SMatrix._14,
                            _21 + SMatrix._21, _22 + SMatrix._22, _23 + SMatrix._23, _24 + SMatrix._24,
                            _31 + SMatrix._31, _32 + SMatrix._32, _33 + SMatrix._33, _34 + SMatrix._34,
                            _41 + SMatrix._41, _42 + SMatrix._42, _43 + SMatrix._43, _44 + SMatrix._44); }

    TMatrix44 operator - (const TMatrix44& SMatrix) const
        { return TMatrix44( _11 - SMatrix._11, _12 - SMatrix._12, _13 - SMatrix._13, _14 - SMatrix._14,
                            _21 - SMatrix._21, _22 - SMatrix._22, _23 - SMatrix._23, _24 - SMatrix._24,
                            _31 - SMatrix._31, _32 - SMatrix._32, _33 - SMatrix._33, _34 - SMatrix._34,
                            _41 - SMatrix._41, _42 - SMatrix._42, _43 - SMatrix._43, _44 - SMatrix._44); }

    TMatrix44 operator + (const TVector3<t>& Vector) const
    {
        return TMatrix44(   _11, _12, _13, _14,
                            _21, _22, _23, _24,
                            _31, _32, _33, _34,
                            _41 + Vector.x, _42 + Vector.y, _43 + Vector.z, _44);
    }

    TMatrix44 operator - (const TVector3<t>& Vector) const
    {
        return TMatrix44(   _11, _12, _13, _14,
                            _21, _22, _23, _24,
                            _31, _32, _33, _34,
                            _41 - Vector.x, _42 - Vector.y, _43 - Vector.z, _44);
    }

    TMatrix44 operator * (const TMatrix44& SMatrix) const
    {
        return TMatrix44(   _11 * SMatrix._11 + _12 * SMatrix._21 + _13 * SMatrix._31 + _14 * SMatrix._41,
                            _11 * SMatrix._12 + _12 * SMatrix._22 + _13 * SMatrix._32 + _14 * SMatrix._42,
                            _11 * SMatrix._13 + _12 * SMatrix._23 + _13 * SMatrix._33 + _14 * SMatrix._43,
                            _11 * SMatrix._14 + _12 * SMatrix._24 + _13 * SMatrix._34 + _14 * SMatrix._44,

                            _21 * SMatrix._11 + _22 * SMatrix._21 + _23 * SMatrix._31 + _24 * SMatrix._41,
                            _21 * SMatrix._12 + _22 * SMatrix._22 + _23 * SMatrix._32 + _24 * SMatrix._42,
                            _21 * SMatrix._13 + _22 * SMatrix._23 + _23 * SMatrix._33 + _24 * SMatrix._43,
                            _21 * SMatrix._14 + _22 * SMatrix._24 + _23 * SMatrix._34 + _24 * SMatrix._44,

                            _31 * SMatrix._11 + _32 * SMatrix._21 + _33 * SMatrix._31 + _34 * SMatrix._41,
                            _31 * SMatrix._12 + _32 * SMatrix._22 + _33 * SMatrix._32 + _34 * SMatrix._42,
                            _31 * SMatrix._13 + _32 * SMatrix._23 + _33 * SMatrix._33 + _34 * SMatrix._43,
                            _31 * SMatrix._14 + _32 * SMatrix._24 + _33 * SMatrix._34 + _34 * SMatrix._44,

                            _41 * SMatrix._11 + _42 * SMatrix._21 + _43 * SMatrix._31 + _44 * SMatrix._41,
                            _41 * SMatrix._12 + _42 * SMatrix._22 + _43 * SMatrix._32 + _44 * SMatrix._42,
                            _41 * SMatrix._13 + _42 * SMatrix._23 + _43 * SMatrix._33 + _44 * SMatrix._43,
                            _41 * SMatrix._14 + _42 * SMatrix._24 + _43 * SMatrix._34 + _44 * SMatrix._44);
    }

    TMatrix44 operator & (const TMatrix44& SMatrix) const
    {
        return TMatrix44(   _11 * SMatrix._11 + _12 * SMatrix._21 + _13 * SMatrix._31,
                            _11 * SMatrix._12 + _12 * SMatrix._22 + _13 * SMatrix._32,
                            _11 * SMatrix._13 + _12 * SMatrix._23 + _13 * SMatrix._33,
                            _14,

                            _21 * SMatrix._11 + _22 * SMatrix._21 + _23 * SMatrix._31,
                            _21 * SMatrix._12 + _22 * SMatrix._22 + _23 * SMatrix._32,
                            _21 * SMatrix._13 + _22 * SMatrix._23 + _23 * SMatrix._33,
                            _24,

                            _31 * SMatrix._11 + _32 * SMatrix._21 + _33 * SMatrix._31,
                            _31 * SMatrix._12 + _32 * SMatrix._22 + _33 * SMatrix._32,
                            _31 * SMatrix._13 + _32 * SMatrix._23 + _33 * SMatrix._33,
                            _34,

                            _41, _42, _43, _44);
    }

    TMatrix44 operator * (const TVector3<t>& Vector) const
    {
        return TMatrix44(   _11 * Vector.x, _12, _13, _14,
                            _21, _22 * Vector.y, _23, _24,
                            _31, _32, _33 * Vector.z, _34,
                            _41, _42, _43, _44);
    }

    TMatrix44 operator * (t c) const
    {
        return TMatrix44(   _11 * c, _12 * c, _13 * c, _14 * c,
                            _21 * c, _22 * c, _23 * c, _24 * c,
                            _31 * c, _32 * c, _33 * c, _34 * c,
                            _41 * c, _42 * c, _43 * c, _44 * c);
    }

    TMatrix44 operator / (t c) const
        { return *this * ((t)1 / c); }

    // Non-const operators
    TMatrix44& operator += (const TMatrix44& SMatrix)
    {
        _11 += SMatrix._11, _12 += SMatrix._12, _13 += SMatrix._13, _14 += SMatrix._14;
        _21 += SMatrix._21, _22 += SMatrix._22, _23 += SMatrix._23, _24 += SMatrix._24;
        _31 += SMatrix._31, _32 += SMatrix._32, _33 += SMatrix._33, _34 += SMatrix._34;
        _41 += SMatrix._41, _42 += SMatrix._42, _43 += SMatrix._43, _44 += SMatrix._44;

        return *this;
    }

    TMatrix44& operator -= (const TMatrix44& SMatrix)
    {
        _11 -= SMatrix._11, _12 -= SMatrix._12, _13 -= SMatrix._13, _14 -= SMatrix._14;
        _21 -= SMatrix._21, _22 -= SMatrix._22, _23 -= SMatrix._23, _24 -= SMatrix._24;
        _31 -= SMatrix._31, _32 -= SMatrix._32, _33 -= SMatrix._33, _34 -= SMatrix._34;
        _41 -= SMatrix._41, _42 -= SMatrix._42, _43 -= SMatrix._43, _44 -= SMatrix._44;

        return *this;
    }

    TMatrix44& operator += (const TVector3<t>& Vector)
        { _41 += Vector.x, _42 += Vector.y, _43 += Vector.z; return *this; }

    TMatrix44& operator -= (const TVector3<t>& Vector)
        { _41 -= Vector.x, _42 -= Vector.y, _43 -= Vector.z; return *this; }

    TMatrix44& operator *= (const TMatrix44<t>& SMatrix)
        { return *this = *this * SMatrix; }

    TMatrix44& operator &= (const TMatrix44<t>& SMatrix)
        { return *this = *this & SMatrix; }

    TMatrix44& operator *= (const TVector3<t>& Vector)
        { _11 *= Vector.x, _22 *= Vector.y, _33 *= Vector.z; return *this; }

    TMatrix44& operator *= (t c)
        { return *this = *this * c; }

    TMatrix44& operator /= (t c)
        { return *this = *this / c; }

    TMatrix44 operator + () const
        { return *this; }

    TMatrix44 operator - () const
        { return Flipped(); }

    // Comparison operators
    bool operator == (const TMatrix44& SMatrix) const
    {
        return  Compare(_11, SMatrix._11) == 0 &&
                Compare(_12, SMatrix._12) == 0 &&
                Compare(_13, SMatrix._13) == 0 &&
                Compare(_14, SMatrix._14) == 0 &&

                Compare(_21, SMatrix._21) == 0 &&
                Compare(_22, SMatrix._22) == 0 &&
                Compare(_23, SMatrix._23) == 0 &&
                Compare(_24, SMatrix._24) == 0 &&

                Compare(_31, SMatrix._31) == 0 &&
                Compare(_32, SMatrix._32) == 0 &&
                Compare(_33, SMatrix._33) == 0 &&
                Compare(_34, SMatrix._34) == 0 &&

                Compare(_41, SMatrix._41) == 0 &&
                Compare(_42, SMatrix._42) == 0 &&
                Compare(_43, SMatrix._43) == 0 &&
                Compare(_44, SMatrix._44) == 0;
    }

    bool operator != (const TMatrix44& SMatrix) const
    {
        return  Compare(_11, SMatrix._11) ||
                Compare(_12, SMatrix._12) ||
                Compare(_13, SMatrix._13) ||
                Compare(_14, SMatrix._14) ||

                Compare(_21, SMatrix._21) ||
                Compare(_22, SMatrix._22) ||
                Compare(_23, SMatrix._23) ||
                Compare(_24, SMatrix._24) ||

                Compare(_31, SMatrix._31) ||
                Compare(_32, SMatrix._32) ||
                Compare(_33, SMatrix._33) ||
                Compare(_34, SMatrix._34) ||

                Compare(_41, SMatrix._41) ||
                Compare(_42, SMatrix._42) ||
                Compare(_43, SMatrix._43) ||
                Compare(_44, SMatrix._44);
    }

    // Inversion
    t GetDet33() const
    {
        return _11*_22*_33 + _13*_21*_32 + _12*_23*_31 -
               _13*_22*_31 - _11*_23*_32 - _12*_21*_33;
    }

    TMatrix44 Inversed32Denorm() const
    {
        TMatrix44 Result;

        Result._11 = _22*_33 - _23*_32;
        Result._12 = _13*_32 - _12*_33;
        Result._13 = 0;
        Result._14 = 0;

        Result._21 = _31*_23 - _21*_33;
        Result._22 = _11*_33 - _13*_31;
        Result._23 = 0;
        Result._24 = 0;

        Result._31 = _21*_32 - _31*_22;
        Result._32 = _31*_12 - _11*_32;
        Result._33 = 1
        Result._34 = 0;

        Result._41 = 0;
        Result._42 = 0;
        Result._43 = 0;
        Result._44 = 1;

        return Result;
    }

    TMatrix44 Inversed32() const
    {
        TMatrix44 Result;

        t Det = GetDet33();

        DEBUG_VERIFY(Sign(Det));

        Det = (t)1 / Det;

        Result._11 = (_22*_33 - _23*_32) * Det;
        Result._12 = (_13*_32 - _12*_33) * Det;
        Result._13 = 0;
        Result._14 = 0;

        Result._21 = (_31*_23 - _21*_33) * Det;
        Result._22 = (_11*_33 - _13*_31) * Det;
        Result._23 = 0;
        Result._24 = 0;

        Result._31 = (_21*_32 - _31*_22) * Det;
        Result._32 = (_31*_12 - _11*_32) * Det;
        Result._33 = 1;
        Result._34 = 0;

        Result._41 = 0;
        Result._42 = 0;
        Result._43 = 0;
        Result._44 = 1;

        return Result;
    }

    TMatrix44 Inversed33Denorm() const
    {
        TMatrix44 Result;

        Result._11 = _22*_33 - _23*_32;
        Result._12 = _13*_32 - _12*_33;
        Result._13 = _12*_23 - _13*_22;
        Result._14 = 0;

        Result._21 = _31*_23 - _21*_33;
        Result._22 = _11*_33 - _13*_31;
        Result._23 = _13*_21 - _11*_23;
        Result._24 = 0;

        Result._31 = _21*_32 - _31*_22;
        Result._32 = _31*_12 - _11*_32;
        Result._33 = _11*_22 - _12*_21;
        Result._34 = 0;

        Result._41 = 0;
        Result._42 = 0;
        Result._43 = 0;
        Result._44 = 1;

        return Result;
    }

    TMatrix44 Inversed33() const
    {
        TMatrix44 Result;

        t Det = GetDet33();

        DEBUG_VERIFY(Sign(Det));

        Det = (t)1 / Det;

        Result._11 = (_22*_33 - _23*_32) * Det;
        Result._12 = (_13*_32 - _12*_33) * Det;
        Result._13 = (_12*_23 - _13*_22) * Det;
        Result._14 = 0;

        Result._21 = (_31*_23 - _21*_33) * Det;
        Result._22 = (_11*_33 - _13*_31) * Det;
        Result._23 = (_13*_21 - _11*_23) * Det;
        Result._24 = 0;

        Result._31 = (_21*_32 - _31*_22) * Det;
        Result._32 = (_31*_12 - _11*_32) * Det;
        Result._33 = (_11*_22 - _12*_21) * Det;
        Result._34 = 0;

        Result._41 = 0;
        Result._42 = 0;
        Result._43 = 0;
        Result._44 = 1;

        return Result;
    }

    t GetDet44() const
    {
        t Det = (t)0;

        for(size_t i = 0 ; i < 4 ; i++)
        {
            TMatrix44 tmp;

            size_t j2 = 0;

            for(size_t j = 0 ; j < 4 ; j++)
            {
                if(j == i)
                    continue;

                for(size_t k = 0 ; k < 3 ; k++)
                    tmp.v[k][j2] = v[k+1][j];

                j2++;
            }

            Det += v[0][i] * tmp.GetDet33() * (t)((i&1) ? -1 : +1);
        }

        return Det;
    }

    TMatrix44 Inversed44() const
    {
        t Det = GetDet44();

        DEBUG_VERIFY(Sign(Det));

        Det = (t)1 / Det;

        TMatrix44 res;

        for(size_t i = 0 ; i < 4 ; i++)
        {
            for(size_t j = 0 ; j < 4 ; j++)
            {
                TMatrix44 tmp;

                size_t k2 = 0;

                for(size_t k = 0 ; k < 4 ; k++)
                {
                    if(k == i)
                        continue;

                    size_t l2 = 0;

                    for(size_t l = 0 ; l < 4 ; l++)
                    {
                        if(l == j)
                            continue;

                        tmp.v[k2][l2] = v[k][l];

                        l2++;
                    }

                    k2++;
                }

                res.v[j][i] = tmp.GetDet33() * (t)(((i^j)&1) ? -1 : +1) * Det;
            }
        }

        return res;
    }

    TMatrix44& Inverse32Denorm()
        { return *this = Inversed32Denorm(); }

    TMatrix44& Inverse32()
        { return *this = Inversed32(); }

    TMatrix44& Inverse33Denorm()
        { return *this = Inversed33Denorm(); }

    TMatrix44& Inverse33()
        { return *this = Inversed33(); }

    TMatrix44& Inverse44()
        { return *this = Inversed44(); }
};

template <class t>
inline TDebugFile& operator << (TDebugFile& DebugFile, const TMatrix44<t>& Matrix)
{
    DebugFile << Matrix._11 << TEXT(" ") << Matrix._12 << TEXT(" ") << Matrix._13 << TEXT(" ") << Matrix._14 << DFCC_EOL;
    DebugFile << Matrix._21 << TEXT(" ") << Matrix._22 << TEXT(" ") << Matrix._23 << TEXT(" ") << Matrix._24 << DFCC_EOL;
    DebugFile << Matrix._31 << TEXT(" ") << Matrix._32 << TEXT(" ") << Matrix._33 << TEXT(" ") << Matrix._34 << DFCC_EOL;
    DebugFile << Matrix._41 << TEXT(" ") << Matrix._42 << TEXT(" ") << Matrix._43 << TEXT(" ") << Matrix._44 << DFCC_EOL;

    return DebugFile;
}

typedef TMatrix44<float>  TFMatrix44;
typedef TMatrix44<double> TDMatrix44;

template <class t>
inline TFMatrix44 TO_F(const TMatrix44<t>& m)
{
    return TFMatrix44(  (float)m._11, (float)m._12, (float)m._13, (float)m._14,
                        (float)m._21, (float)m._22, (float)m._23, (float)m._24,
                        (float)m._31, (float)m._32, (float)m._33, (float)m._34,
                        (float)m._41, (float)m._42, (float)m._43, (float)m._44);
}

template <class t>
inline TDMatrix44 TO_D(const TMatrix44<t>& m)
{
    return TDMatrix44(  (double)m._11, (double)m._12, (double)m._13, (double)m._14,
                        (double)m._21, (double)m._22, (double)m._23, (double)m._24,
                        (double)m._31, (double)m._32, (double)m._33, (double)m._34,
                        (double)m._41, (double)m._42, (double)m._43, (double)m._44);
}

#endif // matrix_h
