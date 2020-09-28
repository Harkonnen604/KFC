#ifndef crt_math_ext_h
#define crt_math_ext_h

#include "math_consts.h"

// ---------------------
// Global math routines
// ---------------------
template <class t>
inline t Sqr(t v)
    { return (t)(v * v); }

#define DECLARE_MATH_WRAPPER_P1(OwnFunc, FloatFunc, DoubleFunc) \
    inline float OwnFunc(float v)                               \
        { return FloatFunc(v); }                                \
                                                                \
    inline double OwnFunc(double v)                             \
        { return DoubleFunc(v); }                               \

#define DECLARE_MATH_WRAPPER_P2(OwnFunc, FloatFunc, DoubleFunc) \
    inline float OwnFunc(float a, float b)                      \
        { return FloatFunc(a, b); }                             \
                                                                \
    inline double OwnFunc(double a, double b)                   \
        { return DoubleFunc(a, b); }                            \

DECLARE_MATH_WRAPPER_P1(Sqrt, sqrtf, sqrt)
DECLARE_MATH_WRAPPER_P1(Sin,  sinf,  sin)
DECLARE_MATH_WRAPPER_P1(Cos,  cosf,  cos)
DECLARE_MATH_WRAPPER_P1(Tan,  tanf,  tan)
DECLARE_MATH_WRAPPER_P1(ASin, asinf, asin)
DECLARE_MATH_WRAPPER_P1(ACos, acosf, acos)
DECLARE_MATH_WRAPPER_P1(ATan, atanf, atan)
DECLARE_MATH_WRAPPER_P1(Exp,  expf,  exp)
DECLARE_MATH_WRAPPER_P1(Log,  logf,  log)
DECLARE_MATH_WRAPPER_P2(Pow,  powf,  pow)

template <class t>
inline t SqrtSafe(t v)
    { return Sign(v) <= 0 ? (t)0 : Sqrt(v); }

template <class t>
inline t ASinSafe(t v)
{
    return  Compare(v, (t)-1) <= 0 ?    (t)(-g_PI * 0.5) :
            Compare(v, (t)+1) >= 0 ?    (t)(+g_PI * 0.5) :
                                        ASin(v);
}

template <class t>
inline t ACosSafe(t v)
{
    return  Compare(v, (t)-1) <= 0 ?    (t)g_PI :
            Compare(v, (t)+1) >= 0 ?    (t)0    :
                                        ACos(v);
}

// Hypot2
template <class t>
inline t Hypot2(t dx, t dy)
{
    return Sqr(dx) + Sqr(dy);
}

template <class t>
inline t Hypot2(t dx, t dy, t dz)
{
    return Sqr(dx) + Sqr(dy) + Sqr(dz);
}

template <class t>
inline t Hypot2(t dx, t dy, t dz, t dw)
{
    return Sqr(dx) + Sqr(dy) + Sqr(sz) + Sqr(dw);
}

// Hypot
template <class t>
inline t Hypot(t dx, t dy)
{
    return Sqrt(Hypot2(dx, dy));
}

template <class t>
inline t Hypot(t dx, t dy, t dz)
{
    return Sqrt(Hypot2(dx, dy, dz));
}

template <class t>
inline t Hypot(t dx, t dy, t dz, t dw)
{
    return Sqrt(Hypot2(dx, dy, dz, dw));
}

#endif // crt_math_ext_h
