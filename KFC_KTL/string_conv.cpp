#include "kfc_ktl_pch.h"
#include "string_conv.h"

// ----------
// Alignment
// ----------
IMPLEMENT_ENUM_STRING_CONV(TAlignment)
    (ALIGNMENT_MIN, TEXT("min"))
    (ALIGNMENT_MID, TEXT("mid"))
    (ALIGNMENT_MAX, TEXT("max"));

// ----------------
// Helper routines
// ----------------
template<class t, size_t n>
bool ReadHexValue(const KString& s, t& rv)
{
    if(s.GetLength() > n + 2 || s[0] != TEXT('0') || s[1] != TEXT('x'))
        return false;

    t v = 0;

    for(size_t i = 2 ; i < s.GetLength() ; i++)
    {
        if(!IsHexChar(s[i]))
            return false;

        v <<= 4, v |= CharToHex(s[i]);
    }

    rv = v;

    return true;
}

// -------------------------------
// Basic types string conversions
// -------------------------------
bool FromString(KString String, QWORD& qwRValue)
{
    String.Trim();

    if(String.IsEmpty())
        return false;

    if( String[0] == '-' &&
        String[1] >= TEXT('1') &&
        String[1] <= TEXT('8') &&
        !String[2])
    {
        qwRValue = (QWORD)(INT64)_ttoi(String);

        return true;
    }

    if(ReadHexValue<QWORD, 16>(String, qwRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if(!_istdigit(String[i]))
            return false;
    }

    if(_stscanf(String, UINT64_FMT, &qwRValue) != 1)
        return false;

    return true;
}

bool FromString(KString String, INT64& iRValue)
{
    String.Trim();

    if(ReadHexValue<INT64, 16>(String, iRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if( !_istdigit(String[i]) &&
            (i > 0 ||
                String[0] != TEXT('+') &&
                String[0] != TEXT('-')))
        {
            return false;
        }
    }

    if(_stscanf(String, INT64_FMT, &iRValue) != 1)
        return false;

    return true;
}

bool FromString(KString String, DWORD& dwRValue)
{
    String.Trim();

    if(String.IsEmpty())
        return false;

    if( String[0] == '-' &&
        String[1] >= TEXT('1') &&
        String[1] <= TEXT('8') &&
        !String[2])
    {
        dwRValue = (UINT)_ttoi(String);

        return true;
    }

    if(ReadHexValue<DWORD, 8>(String, dwRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if(!_istdigit(String[i]))
            return false;
    }

    if(_stscanf(String, TEXT("%lu"), &dwRValue) != 1)
        return false;

    return true;
}

bool FromString(KString String, long& lRValue)
{
    String.Trim();

    if(String.IsEmpty())
        return false;

    if(ReadHexValue<long, 8>(String, lRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if( !_istdigit(String[i]) &&
            (i > 0 ||
                String[0] != TEXT('+') &&
                String[0] != TEXT('-')))
        {
            return false;
        }
    }

    if(_stscanf(String, TEXT("%ld"), &lRValue) != 1)
        return false;

    return true;
}

bool FromString(KString String, UINT& uiRValue)
{
    String.Trim();

    if(String.IsEmpty())
        return false;

    if( String[0] == '-' &&
        String[1] >= TEXT('1') &&
        String[1] <= TEXT('8') &&
        !String[2])
    {
        uiRValue = (UINT)_ttoi(String);

        return true;
    }

    if(ReadHexValue<UINT, 8>(String, uiRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if(!_istdigit(String[i]))
            return false;
    }

    if(_stscanf(String, TEXT("%u"), &uiRValue) != 1)
        return false;

    return true;
}

bool FromString(KString String, int& iRValue)
{
    String.Trim();

    if(String.IsEmpty())
        return false;

    if(ReadHexValue<int, 8>(String, iRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if( !_istdigit(String[i]) &&
            (i > 0 ||
                String[0] != TEXT('+') &&
                String[0] != TEXT('-')))
        {
            return false;
        }
    }

    if(_stscanf(String, TEXT("%d"), &iRValue) != 1)
        return false;

    return true;
}

bool FromString(KString String, WORD& wRValue)
{
    String.Trim();

    if(String.IsEmpty())
        return false;

    if( String[0] == '-' &&
        String[1] >= TEXT('1') &&
        String[1] <= TEXT('8') &&
        !String[2])
    {
        wRValue = (WORD)(UINT)_ttoi(String);

        return true;
    }

    if(ReadHexValue<WORD, 4>(String, wRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if(!_istdigit(String[i]))
            return false;
    }

    UINT uiValue;

    if( _stscanf(String, TEXT("%u"), &uiValue) != 1 ||
        uiValue > USHRT_MAX)
    {
        return false;
    }

    wRValue = (WORD)uiValue;

    return true;
}

bool FromString(KString String, short& sRValue)
{
    String.Trim();

    if(String.IsEmpty())
        return false;

    if(ReadHexValue<short, 4>(String, sRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if( !_istdigit(String[i]) &&
            (i > 0 ||
                String[0] != TEXT('+') &&
                String[0] != TEXT('-')))
        {
            return false;
        }
    }

    int iValue;

    if( _stscanf(String, TEXT("%d"), &iValue) != 1 ||
        iValue < SHRT_MIN || iValue > SHRT_MAX)
    {
        return false;
    }

    sRValue = (short)iValue;

    return true;
}

bool FromString(KString String, BYTE& bRValue)
{
    String.Trim();

    if(String.IsEmpty())
        return false;

    if( String[0] == '-' &&
        String[1] >= TEXT('1') &&
        String[1] <= TEXT('8') &&
        !String[2])
    {
        bRValue = (BYTE)(UINT)_ttoi(String);

        return true;
    }

    if(ReadHexValue<BYTE, 2>(String, bRValue))
        return true;

    for(size_t i = 0 ; i < String.GetLength() ; i++)
    {
        if(!_istdigit(String[i]))
            return false;
    }

    UINT uiValue;

    if( _stscanf(String, TEXT("%u"), &uiValue) != 1 ||
        uiValue > UCHAR_MAX)
    {
        return false;
    }

    bRValue = (BYTE)uiValue;

    return true;
}

bool FromString(LPCTSTR pString, float& fRValue)
{
    if(_stscanf(pString, TEXT("%f"), &fRValue) != 1)
        return false;

    return true;
}

bool FromString(LPCTSTR pString, double& dRValue)
{
    if(_stscanf(pString, TEXT("%lf"), &dRValue) != 1)
        return false;

    return true;
}

bool FromString(KString String, bool& bRValue)
{
    String.Trim();

    if(String == TEXT("false") || String == TEXT("No") || _istdigit(String[0]) && !_ttoi(String))
        return bRValue = false, true;

    if(String == TEXT("true") || String == TEXT("Yes") || _istdigit(String[0]) && _ttoi(String))
        return bRValue = true, true;

    return false;
}
