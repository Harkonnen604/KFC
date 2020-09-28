#ifndef rgb_h
#define rgb_h

// ----
// RGB
// ----
class TRGB
{
public:
    int r, g, b;

public:
    TRGB() {}

    TRGB(int sr, int sg, int sb) : r(sr), g(sg), b(sb) {}

    bool IsBlack() const
        { return !r && !g && !b; }

    void SetBlack()
        { r = 0, g = 0, b = 0; }

    TRGB& Set(int sr, int sg, int sb)
        { r = sr, g = sg, b = sb; return *this; }

    bool IsNormalized() const
    {
        return  r >= 0 && r <= 0xFF &&
                g >= 0 && g <= 0xFF &&
                b >= 0 && b <= 0xFF;
    }

    TRGB& Normalize()
    {
        UpdateMinMax(r, 0, 0xFF);
        UpdateMinMax(g, 0, 0xFF);
        UpdateMinMax(b, 0, 0xFF);

        return *this;
    }

    TRGB Normalized() const
        { return make_temp(*this)().Normalize(); }

    // RGB converters
    TRGB& Set15bpp(WORD c)
    {
        r = (c & 0x7C00) >> 7;
        g = (c & 0x03E0) >> 2;
        b = (c & 0x001F) << 3;

        return *this;
    }

    TRGB& Set16bpp(WORD c)
    {
        r = (c & 0xFC00) >> 8;
        g = (c & 0x07E0) >> 3;
        b = (c & 0x001F) << 3;

        return *this;
    }

    TRGB& SetWORD_BPP(WORD c, size_t szBPP)
        { return szBPP == 15 ? Set15bpp(c) : Set16bpp(c); }

    TRGB& Set32bpp(DWORD c)
    {
        r = (c & 0x00FF0000) >> 16;
        g = (c & 0x0000FF00) >> 8;
        b = (c & 0x000000FF);

        return *this;
    }

    TRGB& SetWinColor(DWORD c)
    {
        r = (c)         & 0xFF;
        g = (c >> 8)    & 0xFF;
        b = (c >> 16)   & 0xFF;

        return *this;
    }

    // BPP converters, components must be in range 0-255
    WORD Get15bpp() const
    {
        DEBUG_VERIFY(IsNormalized());

        return (WORD)((b >> 3) | ((g >> 3) << 5) | (r >> 3) << 10);
    }

    WORD Get16bpp() const
    {
        DEBUG_VERIFY(IsNormalized());

        return (WORD)((b >> 3) | ((g >> 2) << 5) | ((r >> 3) << 11));
    }

    WORD GetWORD_BPP(size_t szBPP) const
        { return szBPP == 15 ? Get15bpp() : Get16bpp(); }

    DWORD Get32bpp(BYTE bAlpha = 0xFF) const
    {
        DEBUG_VERIFY(IsNormalized());

        return b | (g << 8) | (r << 16) | (bAlpha << 24);
    }

    DWORD GetWinColor() const
        { return (b << 16) | (g << 8) | r; }

    // BPP via 15 converters, components must be in range 0-255
    WORD Get15bpp_via15 () const
    {
        DEBUG_VERIFY(IsNormalized());

        return (WORD)((b >> 3) | ((g >> 3) << 5) | ((r >> 3) << 10));
    }

    WORD Get16bpp_via15() const
    {
        DEBUG_VERIFY(IsNormalized());

        return (WORD)((b >> 3) | ((g >> 3) << 6) | ((r >> 3) << 11));
    }

    WORD GetWORD_BPP_via15(size_t szBPP) const
        { return szBPP == 15 ? Get15bpp_via15() : Get16bpp_via15(); }

    DWORD Get32bpp_via15(BYTE bAlpha = 0xFF) const
    {
        DEBUG_VERIFY(IsNormalized());

        return (b >> 3) | ((g >> 3) << 11) | ((r >> 3) << 19) | (bAlpha << 24);
    }

    DWORD GetWinColor_via15() const
    {
        DEBUG_VERIFY(IsNormalized());

        return ((b >> 3) << 19) | ((g >> 3) << 11) | (r >> 3);
    }

    // ------------------ OPERATORS -----------------------
    TRGB operator + (const TRGB &srgb) const
        { return TRGB(r + srgb.r, g + srgb.g, b + srgb.b); }

    TRGB operator - (const TRGB &srgb) const
        { return TRGB(r - srgb.r, g - srgb.g, b - srgb.b); }

    TRGB operator ~ () const
        { DEBUG_VERIFY(IsNormalized()); return TRGB((BYTE)~r, (BYTE)~g, (BYTE)~b); }

    TRGB operator * (double v) const
        { return TRGB((int)(r * v), (int)(g * v), (int)(b * v)); }

    TRGB operator / (double v) const
        { return *this * (1.0 / v); }

    TRGB& operator += (const TRGB &srgb)
        { r += srgb.r, g += srgb.g, b += srgb.b; return *this; }

    TRGB& operator -= (const TRGB &srgb)
        { r -= srgb.r, g -= srgb.g, b -= srgb.b; return *this; }

    TRGB& operator *= (double v)
        { r = (int)(r * v), g = (int)(g * v), b = (int)(b * v); return *this; }

    TRGB& operator /= (double v)
        { return *this *= 1.0 / v; }

    bool operator == (const TRGB& srgb) const
        { return r == srgb.r && g == srgb.g && b == srgb.b; }

    bool operator != (const TRGB& srgb) const
        { return r != srgb.r || g != srgb.g || b != srgb.b; }

    operator KString() const
        { return KString::Formatted("%d %d %d", r, g, b); }
};

inline bool FromString(const KString& String, TRGB& RRGB)
    { return _stscanf(String, TEXT("%d %d %d"), &RRGB.r, &RRGB.g, &RRGB.b) == 3; }

#endif // rgb_h
