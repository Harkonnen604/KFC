#ifndef string_conv_h
#define string_conv_h

#include "kstring.h"
#include "basic_types.h"
#include "array.h"
#include "debug.h"

// ----------------
// Enum conversion
// ----------------
template <class t>
struct TEnumStringConvItem
{
public:
    t m_Value;

    KString m_Text;

public:
    void Set(t Value, LPCTSTR pText)
    {
        m_Value = Value;

        m_Text = pText;

        assert(m_Text.IsTrimmed());
    }
};

template <class t>
class TEnumStringConv : public TArray<TEnumStringConvItem<t> >
{
private:
    const bool m_bSafe;

public:
    TEnumStringConv(bool bSafe = false) : m_bSafe(bSafe) {}

    TEnumStringConv& operator () (t Value, LPCTSTR pText)
    {
        #ifdef _DEBUG
        {
            for(size_t i = 0 ; i <  TArray<TEnumStringConvItem<t> >::GetN() ; i++)
                assert((*this)[i].m_Text != pText);
        }
        #endif // _DEBUG

        TArray<TEnumStringConvItem<t> >::Add().Set(Value, pText);

        return *this;
    }

    bool Read(KString String, t& RValue) const;

    KString Write(t Value) const;
};

template <class t>
bool TEnumStringConv<t>::Read(KString String, t& RValue) const
{
    String.Trim();

    for(size_t i = 0 ; i < TArray<TEnumStringConvItem<t> >::GetN() ; i++)
    {
        if((*this)[i].m_Text == String)
            return RValue = (*this)[i].m_Value, true;
    }

    unsigned int v;

    if( m_bSafe &&
        String.DoesStart("0x") &&
        _stscanf(String.GetDataPtr() + 2, TEXT("%X"), &v) == 1)
    {
        RValue = (t)v;
        return true;
    }

    return false;
}

template <class t>
KString TEnumStringConv<t>::Write(t Value) const
{
    for(size_t i = 0 ; i < TArray<TEnumStringConvItem<t> >::GetN() ; i++)
    {
        if((*this)[i].m_Value == Value)
            return (*this)[i].m_Text;
    }

    if(m_bSafe)
        return KString::Formatted(TEXT("0x%.8X"), (size_t)Value);

    INITIATE_FAILURE;
}

#define _DECLARE_ENUM_STRING_CONV(Type, RawName)                    \
    extern const TEnumStringConv<Type> g_##RawName##_StringConv;    \
                                                                    \
inline bool FromString(const KString& String, Type& RValue)         \
    { return g_##RawName##_StringConv.Read(String, RValue); }       \
                                                                    \
inline KString ToString(Type Value)                                 \
    { return g_##RawName##_StringConv.Write(Value); }               \

#define DECLARE_ENUM_STRING_CONV(Type)  _DECLARE_ENUM_STRING_CONV(Type, Type)

#define _IMPLEMENT_ENUM_STRING_CONV(Type, RawName)          \
    const TEnumStringConv<Type> g_##RawName##_StringConv =  \
        TEnumStringConv<Type>()                             \

#define IMPLEMENT_ENUM_STRING_CONV(Type)    _IMPLEMENT_ENUM_STRING_CONV(Type, Type)

#define _IMPLEMENT_SAFE_ENUM_STRING_CONV(Type, RawName)     \
    const TEnumStringConv<Type> g_##RawName##_StringConv =  \
        TEnumStringConv<Type>(true)                         \

#define IMPLEMENT_SAFE_ENUM_STRING_CONV(Type)   _IMPLEMENT_SAFE_ENUM_STRING_CONV(Type, Type)

// ------------
// Enum tester
// ------------
template <class t>
inline bool IsValidEnum(t v)
{
    TEST_BLOCK_BEGIN
    {
        ToString(v);
    }
    TEST_BLOCK_KFC_EXCEPTION_HANDLER
    {
        return false;
    }
    TEST_BLOCK_END

    return true;
}

// ----------
// Alignment
// ----------
DECLARE_ENUM_STRING_CONV(TAlignment);

// ------------------
// String conversion
// ------------------
inline bool FromString(const KString& String, KString& RString)
{
    RString = String;

    return true;
}

// -------------------------------
// Basic types string conversions
// -------------------------------
bool FromString(KString String,     QWORD&  qwRValue);
bool FromString(KString String,     INT64&  iRValue);
bool FromString(KString String,     DWORD&  dwRValue);
bool FromString(KString String,     long&   lRValue);
bool FromString(KString String,     UINT&   uiRValue);
bool FromString(KString String,     int&    iRValue);
bool FromString(KString String,     WORD&   wRValue);
bool FromString(KString String,     short&  sRValue);
bool FromString(KString String,     BYTE&   bRValue);
bool FromString(LPCTSTR pString,    float&  fRValue);
bool FromString(LPCTSTR pString,    double& dRValue);
bool FromString(KString String,     bool&   bRValue);

// -------------
// Forced macro
// -------------
template <class t>
t ReadFromString(LPCTSTR s)
{
    t v;

    if(!FromString(s, v))
        INITIATE_DEFINED_FAILURE((KString)"Invalid format: \"" + s + TEXT("\"."));

    return v;
}

#endif // string_conv_h
