#ifndef basic_types_h
#define basic_types_h

#include "basic_macros.h"

// Math constants
extern const double g_PI;
extern const double g_E;

// Signed char
typedef signed char schar;

#include "basic_wintypes.h"
#include "stream.h"

// Own limits
#define BYTE_MIN    ((BYTE)0)
#define BYTE_MAX    ((BYTE)UCHAR_MAX)

#define SHORT_MIN   ((short)SHRT_MIN)
#define SHORT_MAX   ((short)SHRT_MAX)

#define WORD_MIN    ((WORD)0)
#define WORD_MAX    (USHRT_MAX)

#define UINT_MIN    ((UINT)0)

#define DWORD_MIN   ((DWORD)0)
#define DWORD_MAX   (ULONG_MAX)

// #define INT64_MIN    (_I64_MIN)
// #define INT64_MAX    (_I64_MAX)

#define QWORD_MIN   ((QWORD)0)
#define QWORD_MAX   (_UI64_MAX)

// 64-bit formats
#ifdef _MSC_VER
    #define INT64_FMT   TEXT("%I64d")
    #define UINT64_FMT  TEXT("%I64u")
    #define XINT64_FMT  TEXT("%I64X")
    #define FXINT64_FMT TEXT("%.16I64X")
#else // _MSC_VER
    #define INT64_FMT   TEXT("%lld")
    #define UINT64_FMT  TEXT("%llu")
    #define XINT64_FMT  TEXT("%llX")
    #define FXINT64_FMT TEXT("%.16llX")
#endif // _MSC_VER

// Thresholds
#define FLOAT_THRESHOLD     (1e-4f)
#define DOUBLE_THRESHOLD    (1e-8)

// Equality operator speed defs
#define IMPLEMENT_INEQUALITY(Type)          \
    bool operator != (const Type& Value)    \
    {                                       \
        return !(*this == Value);           \
    }                                       \

// MakeAbs speed-defs
#define DECLARE_SIGNED_MAKEABS(Type)    \
    inline Type& MakeAbs(Type& Value)   \
    {                                   \
        if(Value < 0)                   \
            Value = -Value;             \
                                        \
        return Value;                   \
    }                                   \

#define DECLARE_UNSIGNED_MAKEABS(Type)  \
    inline Type& MakeAbs(Type& Value)   \
    {                                   \
        return Value;                   \
    }                                   \

// Round-up speed-defs
#define DECLARE_INTEGER_ROUNDUP(Type)   \
    inline Type& RoundUp(Type& Value)   \
    {                                   \
        return Value;                   \
    }                                   \

// -----
// Temp
// -----
template <class t>
class temp
{
private:
    mutable t v;

public:
    temp() {}

    template <class dt>
    temp(const dt& d) : v(d) {}

    template <class dt1, class dt2>
    temp(const dt1& d1, const dt2& d2) : v(d1, d2) {}

    template <class dt1, class dt2, class dt3>
    temp(const dt1& d1, const dt2& d2, const dt3& d3) : v(d1, d2, d3) {}

    template <class dt1, class dt2, class dt3, class dt4>
    temp(const dt1& d1, const dt2& d2, const dt3& d3, const dt4& d4) : v(d1, d2, d3, d4) {}

    t& GetData() const
        { return v; }

    operator t& () const
        { return v; }

    t& operator () () const
        { return v; }
};

template <class t>
temp<t> make_temp(const t& v)
    { return temp<t>(v); }

// ---------
// Fake ref
// ---------
extern void* const fake_ref;

// --------------
// Release guard
// --------------
template <class ObjectType>
class TReleaseGuard
{
private:
    ObjectType& m_Object;

public:
    TReleaseGuard(ObjectType& Object) : m_Object(Object) {}

    ~TReleaseGuard()
        { m_Object.Release(); }
};

// -----------
// QWORD type
// -----------
typedef UINT64 QWORD;

// -----------------
// FILETIME wrapper
// -----------------
struct TFileTime : public FILETIME
    { TFileTime() { memset(this, 0, sizeof(*this)); } };

// ----------
// Size type
// ----------
#define SIZE_T_PTR_IN(Size,     Type)   ((Type*)&(Size))
#define SIZE_T_PTR_OUT(Size,    Type)   ((Size) = 0, (Type*)&(Size))
#define SIZE_T_PTR_INOUT(Size,  Type)   ((Type*)&(Size))

// -----------
// Flags type
// -----------
typedef UINT kflags_t;

#define KFLAGS_T_PTR_IN(Flags,      Type)   ((Type*)&(Flags))
#define KFLAGS_T_PTR_OUT(Flags,     Type)   ((Flags) = 0, (Type*)&(Flags))
#define KFLAGS_T_PTR_INOUT(Flags,   Type)   ((Type*)&(Flags))

// ----------
// Type type
// ----------
typedef UINT ktype_t;

#define KTYPE_T_PTR_IN(Type,        TType)  ((TType*)&(Type))
#define KTYPE_T_PTR_OUT(Type,   TType)  ((Type) = 0, (TType*)&(Type))
#define KTYPE_T_PTR_INOUT(Type, TType)  ((TType*)&(Type))

// ----------
// Mode type
// ----------
typedef UINT kmode_t;

#define KMODE_T_PTR_IN(Mode,        TType)  ((TType*)&(Mode))
#define KMODE_T_PTR_OUT(Mode,   TType)  ((Mode) = 0, (TType*)&(Mode))
#define KMODE_T_PTR_INOUT(Mode, TType)  ((TType*)&(Mode))

// ----------
// Alignment
// ----------
enum TAlignment
{
    ALIGNMENT_MIN           = 1,
    ALIGNMENT_MAX           = 2,
    ALIGNMENT_MID           = 3,
    ALIGNMENT_FORCE_UINT    = UINT_MAX,
};

// ------------------------
// Global service routines
// ------------------------
inline bool& ToggleBool(bool& bValue)
{
    return bValue = !bValue;
}

// ---------------------------
// Global comparison routines
// ---------------------------
inline int Compare(INT64 iValue1, INT64 iValue2)
    { return iValue1 == iValue2 ? 0 : (iValue1 < iValue2 ? -1 : 1); }

inline int Compare(long lValue1, long lValue2)
    { return lValue1 == lValue2 ? 0 : (lValue1 < lValue2 ? -1 : 1); }

inline int Compare(int iValue1, int iValue2)
    { return iValue1 == iValue2 ? 0 : (iValue1 < iValue2 ? -1 : 1); }

inline int Compare(short sValue1, short sValue2)
    { return sValue1 == sValue2 ? 0 : (sValue1 < sValue2 ? -1 : 1); }

inline int Compare(QWORD qwValue1, QWORD qwValue2)
    { return qwValue1 == qwValue2 ? 0 : (qwValue1 < qwValue2 ? -1 : 1); }

inline int Compare(DWORD dwValue1, DWORD dwValue2)
    { return dwValue1 == dwValue2 ? 0 : (dwValue1 < dwValue2 ? -1 : 1); }

inline int Compare(UINT uiValue1, UINT uiValue2)
    { return uiValue1 == uiValue2 ? 0 : (uiValue1 < uiValue2 ? -1 : 1); }

inline int Compare(WORD wValue1, WORD wValue2)
    { return wValue1 == wValue2 ? 0 : (wValue1 < wValue2 ? -1 : 1); }

inline int Compare(float fValue1, float fValue2)
    { return fabsf(fValue1 - fValue2) < FLOAT_THRESHOLD ? 0 : (fValue1 < fValue2 ? -1 : 1); }

inline int Compare(double dValue1, double dValue2)
    { return fabs(dValue1 - dValue2) < DOUBLE_THRESHOLD ? 0 : (dValue1 < dValue2 ? -1 : 1); }

inline int Compare(TCHAR cChar1, TCHAR cChar2)
    { return cChar1 == cChar2 ? 0 : (cChar1 < cChar2 ? -1 : 1); }

inline int Compare(bool bValue1, bool bValue2)
    { return bValue1 ? (bValue2 ? 0 : +1) : (bValue2 ? -1 : 0); }

inline int Compare(BYTE bValue1, BYTE bValue2)
    { return bValue1 == bValue2 ? 0 : (bValue1 < bValue2 ? -1 : 1); }

inline int Compare(const void* p1, const void* p2)
    { return p1 == p2 ? 0 : p1 < p2 ? -1 : +1; }

inline int Compare(const FILETIME& Time1, const FILETIME& Time2)
    { return CompareFileTime(&Time1, &Time2); }

// ------------
// sqr routine
// ------------
template <class t>
inline t sqr(t v)
    { return v*v; }

// ---------------------
// Global sign routines
// ---------------------
template <class t>
inline int Sign(const t& Value)
{
    int d = Compare(Value, (t)0);
    return d < 0 ? -1 : d > 0 ? +1 : 0;
}

template <class t>
inline int Sign1(const t& Value)
{
    const int iCmp = Compare(Value, (t)0);
    return iCmp ? iCmp : 1;
}

template <class t>
inline t SignDir(const t& Value, const t& Direction)
{
    return Value * (t)Sign(Direction);
}

template <class t>
inline t Sign1Dir(const t& Value, const t& Direction)
{
    return Value * (t)Sign1(Direction);
}

// ------------------------
// Global is-zero routines
// ------------------------
template <class t>
inline bool IsZero(const t& Value)
{
    return !Compare(Value, (t)0);
}

// ----------------------------
// Global comparison operators
// ----------------------------
#define DECLARE_COMPARISON_OPERATORS_EXCEPT_EQUALITY(Type1, Type2)  \
                                                                    \
    inline bool operator < (Type1 v1, Type2 v2)                     \
        { return Compare(v1, v2) < 0; }                             \
                                                                    \
    inline bool operator <= (Type1 v1, Type2 v2)                    \
        { return Compare(v1, v2) <= 0; }                            \
                                                                    \
    inline bool operator > (Type1 v1, Type2 v2)                     \
        { return Compare(v1, v2) > 0; }                             \
                                                                    \
    inline bool operator >= (Type1 v1, Type2 v2)                    \
        { return Compare(v1, v2) >= 0; }                            \

#define DECLARE_COMPARISON_OPERATORS(Type1, Type2)  \
                                                    \
    inline bool operator < (Type1 v1, Type2 v2)     \
        { return Compare(v1, v2) < 0; }             \
                                                    \
    inline bool operator <= (Type1 v1, Type2 v2)    \
        { return Compare(v1, v2) <= 0; }            \
                                                    \
    inline bool operator > (Type1 v1, Type2 v2)     \
        { return Compare(v1, v2) > 0; }             \
                                                    \
    inline bool operator >= (Type1 v1, Type2 v2)    \
        { return Compare(v1, v2) >= 0; }            \
                                                    \
    inline bool operator == (Type1 v1, Type2 v2)    \
        { return Compare(v1, v2) == 0; }            \
                                                    \
    inline bool operator != (Type1 v1, Type2 v2)    \
        { return Compare(v1, v2) != 0; }            \

#define DECLARE_TEMPLATE_COMPARISON_OPERATORS_EXCEPT_EQUALITY(Template, Type1, Type2)   \
                                                                                        \
    template < Template >                                                               \
    inline bool operator < (Type1 v1, Type2 v2)                                         \
        { return Compare(v1, v2) < 0; }                                                 \
                                                                                        \
    template < Template >                                                               \
    inline bool operator <= (Type1 v1, Type2 v2)                                        \
        { return Compare(v1, v2) <= 0; }                                                \
                                                                                        \
    template < Template >                                                               \
    inline bool operator > (Type1 v1, Type2 v2)                                         \
        { return Compare(v1, v2) > 0; }                                                 \
                                                                                        \
    template < Template >                                                               \
    inline bool operator >= (Type1 v1, Type2 v2)                                        \
        { return Compare(v1, v2) >= 0; }                                                \

#define DECLARE_TEMPLATE_COMPARISON_OPERATORS(Template, Type1, Type2)   \
                                                                        \
    template < Template >                                               \
    inline bool operator < (Type1 v1, Type2 v2)                         \
        { return Compare(v1, v2) < 0; }                                 \
                                                                        \
    template < Template >                                               \
    inline bool operator <= (Type1 v1, Type2 v2)                        \
        { return Compare(v1, v2) <= 0; }                                \
                                                                        \
    template < Template >                                               \
    inline bool operator > (Type1 v1, Type2 v2)                         \
        { return Compare(v1, v2) > 0; }                                 \
                                                                        \
    template < Template >                                               \
    inline bool operator >= (Type1 v1, Type2 v2)                        \
        { return Compare(v1, v2) >= 0; }                                \
                                                                        \
    template < Template >                                               \
    inline bool operator == (Type1 v1, Type2 v2)                        \
        { return Compare(v1, v2) == 0; }                                \
                                                                        \
    template < Template >                                               \
    inline bool operator != (Type1 v1, Type2 v2)                        \
        { return Compare(v1, v2) != 0; }                                \

// ---------------------
// Global swap routines
// ---------------------
template <class t>
inline void Swap(t& v1, t& v2)
{
    t x;

    x = v1, v1 = v2, v2 = x;
}

// --------------------
// Global abs routines
// --------------------
DECLARE_SIGNED_MAKEABS(INT64);
DECLARE_SIGNED_MAKEABS(long);
DECLARE_SIGNED_MAKEABS(int);
DECLARE_SIGNED_MAKEABS(short);
DECLARE_SIGNED_MAKEABS(signed char);

DECLARE_UNSIGNED_MAKEABS(QWORD);
DECLARE_UNSIGNED_MAKEABS(DWORD);
DECLARE_UNSIGNED_MAKEABS(UINT);
DECLARE_UNSIGNED_MAKEABS(WORD);
DECLARE_UNSIGNED_MAKEABS(BYTE);

DECLARE_SIGNED_MAKEABS(float);
DECLARE_SIGNED_MAKEABS(double);

template <class t>
inline t GetAbs(const t& Value)
{
    return MakeAbs(make_temp(Value));
}

// ------------------
// Flooring routines
// ------------------
inline float Floor(float v)
    { return floorf(v + FLOAT_THRESHOLD); }

inline double Floor(double v)
    { return floor(v + DOUBLE_THRESHOLD); }

inline float AbsFloor(float v)
    { return Sign(v) ? Sign(v) < 0 ? -Floor(-v) : Floor(v) : 0.0f; }

inline double AbsFloor(double v)
    { return Sign(v) ? Sign(v) < 0 ? -Floor(-v) : Floor(v) : 0.0; }

// -----------------
// Ceiling routines
// -----------------
inline float Ceil(float v)
    { return ceilf(v - FLOAT_THRESHOLD); }

inline double Ceil(double v)
    { return ceil(v - DOUBLE_THRESHOLD); }

inline float AbsCeil(float v)
    { return Sign(v) ? Sign(v) < 0 ? -Ceil(-v) : Ceil(v) : 0.0f; }

inline double AbsCeil(double v)
    { return Sign(v) ? Sign(v) < 0 ? -Ceil(-v) : Ceil(v) : 0.0; }

// ------------------
// Rounding routines
// ------------------
inline float Round(float v)
    { return Floor(v + 0.5f); }

inline double Round(double v)
    { return Floor(v + 0.5); }

inline float AbsRound(float v)
    { return Sign(v) ? Sign(v) < 0 ? -Round(-v) : Round(v) : 0.0f; }

inline double AbsRound(double v)
    { return Sign(v) ? Sign(v) < 0 ? -Round(-v) : Round(v) : 0.0; }

// -------------------------
// Global round-up routines
// -------------------------
DECLARE_INTEGER_ROUNDUP(INT64);
DECLARE_INTEGER_ROUNDUP(long);
DECLARE_INTEGER_ROUNDUP(int);
DECLARE_INTEGER_ROUNDUP(short);
DECLARE_INTEGER_ROUNDUP(char);

DECLARE_INTEGER_ROUNDUP(QWORD);
DECLARE_INTEGER_ROUNDUP(DWORD);
DECLARE_INTEGER_ROUNDUP(UINT);
DECLARE_INTEGER_ROUNDUP(WORD);
DECLARE_INTEGER_ROUNDUP(BYTE);

inline float& RoundUp(float& fValue)
{
    return fValue = floorf(fValue + 0.5f);
}

inline double& RoundUp(double& dValue)
{
    return dValue = floor(dValue + 0.5);
}

template <class t>
inline t GetRounded(const t& Value)
{
    return RoundUp(t(Value));
}

// ----------------------------
// Global bind-to-int routines
// ----------------------------
template <class t>
t& BindToInt(t& Value)
{
    const t Rounded = GetRounded(Value);
    if(Compare(Value, Rounded) == 0)
        Value = Rounded;

    return Value;
}

template <class t>
t GetBindedToInt(const t& Value)
{
    return BindToInt(t(Value));
}

// ---------------------------------------------
// Global min/max functions (up to 4 arguments)
// ---------------------------------------------
template <class t>
inline t Min(t a, t b)
{
    return Compare(a, b) <= 0 ? a : b;
}

template <class t>
inline t Min(t a, t b, t c)
{
    return Min(Min(a, b), c);
}

template <class t>
inline t Min(t a, t b, t c, t d)
{
    return Min(Min(Min(a, b), c), d);
}

template <class t>
inline t Max(t a, t b)
{
    return Compare(a, b) >= 0 ? a : b;
}

template <class t>
inline t Max(t a, t b, t c)
{
    return Max(Max(a, b), c);
}

template <class t>
inline t Max(t a, t b, t c, t d)
{
    return Max(Max(Max(a, b), c), d);
}

// --------------------------
// MinMax function (bounder)
// --------------------------
template <class t>
inline t MinMax(t a, t b, t c)
    { return Compare(a, b) < 0 ? b : Compare(a, c) > 0 ? c : a; }

// -------------------------
// Update min/max functions
// -------------------------
template <class t>
inline t& UpdateMin(t& a, const t& b)
    { if(a > b) a = b; return a; }

template <class t>
inline t& UpdateMax(t& a, const t& b)
    { if(a < b) a = b; return a; }

template <class t>
inline t& UpdateMinMax(t& a, const t& b, const t& c)
    { if(a < b) a = b; else if(a > c) a = c; return a; }

// -----------------
// QWORD converions
// -----------------
inline float TO_F(QWORD v)
    { return (float)(INT64)v; }

inline double TO_D(QWORD v)
    { return (double)(INT64)v; }

// ----------------------
// Basic types streaming
// ----------------------
DECLARE_BASIC_STREAMING(INT64);
DECLARE_BASIC_STREAMING(long);
DECLARE_BASIC_STREAMING(int);
DECLARE_BASIC_STREAMING(short)
DECLARE_BASIC_STREAMING(char);

DECLARE_BASIC_STREAMING(QWORD);
DECLARE_BASIC_STREAMING(DWORD);
DECLARE_BASIC_STREAMING(UINT);
DECLARE_BASIC_STREAMING(WORD)
DECLARE_BASIC_STREAMING(BYTE);

DECLARE_BASIC_STREAMING(double);
DECLARE_BASIC_STREAMING(float);

DECLARE_BASIC_STREAMING(bool);

#endif // basic_types_h
