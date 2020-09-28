#include "kfc_ktl_pch.h"
#include "kstring.h"

#include "ktl_consts.h"
#include "file_names.h"

const KString g_EmptyString;

// ----------------
// Helper routines
// ----------------
inline LPTSTR ShortenFloatBuf(LPTSTR s)
{
	size_t i;

	for(i = _tcslen(s) - 1 ; i >= 0 && s[i] == TEXT('0') ; i--)
		s[i] = 0;

	if(i >= 0 && s[i] == TEXT('.'))
		s[i] = 0;

	if(!_tcscmp(s, TEXT("-0")))
		s++;

	return s;
}

// -------
// String
// -------
KString::KString(TCHAR c, size_t szNRepeats)
{
	Allocate(szNRepeats);

	if(sizeof(c) == 1)
	{
		memset(GetDataPtr(), c, szNRepeats * sizeof(c));
	}
	else
	{
		for(size_t i = 0 ; i < szNRepeats ; i++)
			m_Chars[i] = c;
	}
}

KString::KString(LPCTSTR pStr, size_t szLength)
{
	Allocate(szLength);

	memcpy(GetDataPtr(), pStr, GetStreamCharsLength(szLength));
}

KString::KString(LPCTSTR pStr1, size_t szLength1, LPCTSTR pStr2, size_t szLength2)
{
	Allocate(szLength1 + szLength2);

	memcpy(GetDataPtr(), pStr1, GetStreamCharsLength(szLength1));
	memcpy(GetDataPtr() + szLength1, pStr2, GetStreamCharsLength(szLength2));
}

KString::KString(TAlignment Value)
{
	*this = Value;
}

LPTSTR KString::Allocate(size_t szLength)
{
	m_Chars.SetN(szLength + 1);

	Terminate();

	return GetDataPtr();
}

bool KString::IsSpaceChar(TCHAR cChar)
{
	return	cChar == 0			||
			cChar == TEXT('\r') ||
			cChar == TEXT('\n') ||
			cChar == TEXT('\t');
}

TCHAR KString::ValueToAlphabet(size_t szValue)
{
	if(szValue >= MAX_ALPHABET_RADIX)
		szValue = MAX_ALPHABET_RADIX - 1;

	if(szValue < 10)
		return (TCHAR)(TEXT('0') + szValue);

	if(szValue < 10 + 26)
		return (TCHAR)(TEXT('A') + (szValue - 10));

	return (TCHAR)(TEXT('a') + (szValue - (10 + 26)));
}

size_t KString::AlphabetToValue(TCHAR cChar)
{
	if(_istdigit(cChar))
		return cChar - TEXT('0');

	if(_istupper(cChar))
		return cChar - TEXT('A') + 10;

	if(_istlower(cChar))
		return cChar - TEXT('a') + (10 + 26);

	return -1;
}

size_t KString::GetNoSpaceCharactersLength() const
{
	size_t szLength = 0;

	for(size_t i = 0 ; i < GetLength() ; i++)
	{
		if(!IsSpaceChar(m_Chars[i]))
			szLength++;
	}

	return szLength;
}

LPTSTR KString::Extend(size_t l)
{
	m_Chars.Add(l);

	Terminate();

	return GetDataPtr() + (GetLength() - l);
}

LPTSTR KString::Extend(LPCTSTR s, size_t l)
{
	size_t szOldLength = GetLength();

	Extend(l);

	memcpy(GetDataPtr() + szOldLength, s, GetStreamCharsLength(l));

	return GetDataPtr() + (GetLength() - l);
}

KString& KString::WriteInt64(INT64 iValue, bool bSeparate)
{
	TCHAR Buf[128];

	if(!bSeparate)
	{
		_stprintf(Buf, INT64_FMT, iValue);
		return *this = Buf;
	}

	// Preparing
	bool bNeg = iValue < 0;

	size_t l = 0;

	// Writing digits
	if(bSeparate)
	{
		size_t nd = 0;

		do
		{
			Buf[l++] = (TCHAR)(TEXT('0') + (bNeg ? -(iValue % 10) : iValue % 10)), iValue /= 10;

			if(++nd==3 && iValue)
				Buf[l++] = TEXT('\''), nd=0;

		}while(iValue);
	}
	else
	{
		do
		{
			Buf[l++] = (TCHAR)(TEXT('0') + (bNeg ? -(iValue % 10) : iValue % 10)), iValue /= 10;

		}while(iValue);
	}

	// Sign
	if(bNeg)
		Buf[l++] = TEXT('-');

	// Finishing
	Buf[l] = 0;

	// Reversing
	for(size_t i = (l>>1) - 1 ; i != -1 ; i--)
	{
		TCHAR t;
		t=Buf[i], Buf[i]=Buf[l-i-1], Buf[l-i-1]=t;
	}

	return *this = Buf;
}

KString& KString::WriteQWORD(QWORD qwValue, bool bSeparate)
{
	TCHAR Buf[128];

	if(!bSeparate)
	{
		_stprintf(Buf, UINT64_FMT, qwValue);

		return *this = Buf;
	}

	// Preparing
	size_t l = 0;

	// Writing digits
	if(bSeparate)
	{
		size_t nd = 0;

		do
		{
			Buf[l++] = (TCHAR)(TEXT('0') + qwValue % 10), qwValue /= 10;

			if(++nd == 3 && qwValue)
				Buf[l++] = TEXT('\''), nd=0;

		}while(qwValue);
	}
	else
	{
		do
		{
			Buf[l++] = (TCHAR)(TEXT('0') + qwValue % 10), qwValue /= 10;

		}while(qwValue);
	}

	// Finishing
	Buf[l] = 0;

	// Reversing
	for(size_t i = (l >> 1) - 1 ; i != -1 ; i--)
	{
		TCHAR t;
		t=Buf[i], Buf[i]=Buf[l-i-1], Buf[l-i-1]=t;
	}

	return *this = Buf; // +++ use internal known length allocator
}

KString& KString::Format(LPCTSTR pFormat, ...)
{
	va_list ArgList;
	va_start(ArgList, pFormat);

	TArray<TCHAR, true> Buffer(g_KTL_Consts.m_szStringBufferSize);

	_vsntprintf(Buffer.GetDataPtr(), Buffer.GetN() - 1, pFormat, ArgList);

	Buffer.GetLastItem() = 0;

	Allocate(Buffer.GetDataPtr(), _tcslen(Buffer.GetDataPtr()));

	va_end(ArgList);

	return *this;
}

KString& KString::FormatV(LPCTSTR pFormat, va_list ArgList)
{
	TArray<TCHAR, true> Buffer(g_KTL_Consts.m_szStringBufferSize);

	_vsntprintf(Buffer.GetDataPtr(), Buffer.GetN() - 1, pFormat, ArgList);

	Buffer.GetLastItem() = 0;

	Allocate(Buffer.GetDataPtr(), _tcslen(Buffer.GetDataPtr()));

	return *this;
}

KString KString::Formatted(LPCTSTR pFormat, ...)
{
	va_list ArgList;
	va_start(ArgList, pFormat);

	KString String;
	String.FormatV(pFormat, ArgList);

	va_end(ArgList);

	return String;
}

KString KString::FormattedV(LPCTSTR pFormat, va_list ArgList)
{
	KString String;
	String.FormatV(pFormat, ArgList);

	return String;
}

KString& KString::operator = (const KString& SString)
{
	if(&SString == this)
		return *this;

	m_Chars = SString.m_Chars;

	return *this;
}

KString& KString::operator = (LPCTSTR pString)
{
	if(!m_Chars.IsEmpty() && GetDataPtr() == pString)
		return *this;

	Allocate(_tcslen(pString));

	memcpy(GetDataPtr(), pString, GetStreamCharsLength());

	return *this;
}

KString& KString::operator = (TCHAR cChar)
{
	*Allocate(1) = cChar;

	return *this;
}

KString& KString::operator = (float fValue)
{
	TCHAR Buf[4096];
	_stprintf(Buf, TEXT("%.4f"), fValue);

	return *this = ShortenFloatBuf(Buf);
}

KString& KString::operator = (double dValue)
{
	TCHAR Buf[4096];
	_stprintf(Buf, TEXT("%.8lf"), dValue);

	return *this = ShortenFloatBuf(Buf);
}

KString& KString::operator = (bool bValue)
{
	return *this = (bValue ? TEXT("true") : TEXT("false"));
}

KString& KString::operator = (TAlignment Value)
{
	return *this = ToString(Value);
}

KString KString::operator + (const KString& SString) const
{
	return KString(GetDataPtr(), GetLength(), SString, SString.GetLength());
}

KString KString::operator + (LPCTSTR pString) const
{
	return KString(GetDataPtr(), GetLength(), pString, _tcslen(pString));
}

KString KString::operator + (TCHAR cChar) const
{
	return KString(GetDataPtr(), GetLength(), &cChar, 1);
}

KString KString::operator + (INT64 iValue) const
{
	TCHAR Buf[128];
	_stprintf(Buf, INT64_FMT, iValue);

	return *this + Buf;
}

KString KString::operator + (QWORD qwValue) const
{
	if((INT64)qwValue >= -8 && (INT64)qwValue <= -1)
		return *this + (INT64)qwValue;

	TCHAR Buf[128];
	_stprintf(Buf, UINT64_FMT, qwValue);

	return *this + Buf;
}

KString KString::operator + (float fValue) const
{
	TCHAR Buf[4096];
	_stprintf(Buf, TEXT("%.4f"), fValue);

	return *this + ShortenFloatBuf(Buf);
};

KString KString::operator + (double dValue) const
{
	TCHAR Buf[4096];
	_stprintf(Buf, TEXT("%.8lf"), dValue);

	return *this + ShortenFloatBuf(Buf);
}

KString KString::operator + (bool bValue) const
{
	return bValue ?
				KString(GetDataPtr(), GetLength(), TEXT("true"),	4) :
				KString(GetDataPtr(), GetLength(), TEXT("false"),	5);
}

KString KString::operator + (TAlignment Value) const
{
	return *this + ToString(Value);
}

KString& KString::operator += (const KString& SString)
{
	if(&SString == this)
	{
		size_t szOldLength = GetLength();

		Extend(GetLength());

		memcpy(GetDataPtr() + szOldLength, GetDataPtr(), GetStreamCharsLength(szOldLength));
	}
	else
	{
		Extend(SString, SString.GetLength());
	}

	return *this;
}

KString& KString::operator += (LPCTSTR pString)
{
	Extend(pString, _tcslen(pString));

	return *this;
}

KString& KString::operator += (TCHAR cChar)
{
	*Extend(1) = cChar;

	return *this;
}

KString& KString::operator += (INT64 iValue)
{
	TCHAR Buf[128];
	_stprintf(Buf, INT64_FMT, iValue);

	return *this += Buf;
}

KString& KString::operator += (QWORD qwValue)
{
	if((INT64)qwValue >= -8 && (INT64)qwValue <= -1)
		return *this += (INT64)qwValue;

	TCHAR Buf[128];
	_stprintf(Buf, UINT64_FMT, qwValue);

	return *this += Buf;
}

KString& KString::operator += (float fValue)
{
	TCHAR Buf[4096];
	_stprintf(Buf, TEXT("%.4f"), fValue);

	return *this += ShortenFloatBuf(Buf);
}

KString& KString::operator += (double dValue)
{
	TCHAR Buf[4096];
	_stprintf(Buf, TEXT("%.8lf"), dValue);

	return *this += ShortenFloatBuf(Buf);
}

KString& KString::operator += (bool bValue)
{
	if(bValue)
		Extend(TEXT("true"), 4);
	else
		Extend(TEXT("false"), 5);

	return *this;
}

KString& KString::operator += (TAlignment Value)
{
	return *this += ToString(Value);
}

KString KString::GetLower() const
{
	KString String = *this;

	_tcslwr(String.GetDataPtr());

	return String;
}

KString KString::GetUpper() const
{
	KString String = *this;

	_tcsupr(String.GetDataPtr());

	return String;
}

size_t KString::Find(LPCTSTR pString, size_t szStart) const
{
	UpdateMin(szStart, GetLength());

	LPCTSTR ptr = _tcsstr(GetDataPtr() + szStart, pString);

	return ptr ? ptr - GetDataPtr() : -1;
}

size_t KString::Find(TCHAR cChar, size_t szStart) const
{
	if(szStart >= GetLength())
		return -1;

	LPCTSTR ptr = _tcschr(GetDataPtr() + szStart, cChar);

	return ptr ? ptr - GetDataPtr() : -1;
}

size_t KString::FindOneOf(LPCTSTR pChars, size_t szStart) const
{
	if(szStart >= GetLength())
		return -1;

	UpdateMin(szStart, GetLength());

	LPCTSTR ptr = _tcspbrk(GetDataPtr() + szStart, pChars);

	return ptr ? ptr - GetDataPtr() : -1;
}

size_t KString::FindRev(LPCTSTR pString, size_t szStart) const
{
	if(szStart == -1 || IsEmpty())
		return -1;

	size_t l = _tcslen(pString);

	UpdateMin(szStart, GetLength() - l); // includes (size_t)-1 - 1

	LPCTSTR p = (LPCTSTR)*this + szStart;

	for(size_t i = szStart ; i != -1 ; i--, p--)
	{
		if(!memcmp(p, pString, l * sizeof(TCHAR)))
			return i;
	}

	return -1;
}

size_t KString::FindRev(TCHAR cChar, size_t szStart) const
{
	if(szStart == -1 || IsEmpty())
		return -1;

	UpdateMin(szStart, GetLength() - 1); // includes (size_t)-1 - 1

	LPCTSTR p = (LPCTSTR)*this + szStart;

	for(size_t i = szStart ; i != -1 ; i--, p--)
	{
		if(*p == cChar)
			return i;
	}

	return -1;
}

size_t KString::FindOneOfRev(LPCTSTR pChars, size_t szStart) const
{
	if(szStart == -1 || IsEmpty())
		return -1;

	UpdateMin(szStart, GetLength() - 1); // includes (size_t)-1 - 1

	LPCTSTR p = (LPCTSTR)*this + szStart;

	for(size_t i = szStart ; i != -1 ; i--, p--)
	{
		if(_tcschr(pChars, *p))
			return i;
	}

	return -1;
}

KString KString::Left(size_t szCount) const
{
	if(szCount > GetLength())
		szCount = GetLength();

	return KString(GetDataPtr(), szCount);
}

KString KString::Right(size_t szCount) const
{
	if(szCount > GetLength())
		szCount = GetLength();

	return KString(GetDataPtr() + (GetLength() - szCount), szCount);
}

KString KString::Mid(size_t szStart, size_t szCount) const
{
	if(szStart > GetLength())
		szStart = GetLength();

	if(szCount == -1 || szStart + szCount > GetLength())
		szCount = GetLength() - szStart;

	return KString(GetDataPtr() + szStart, szCount);
}

KString& KString::SetLeft(size_t szCount)
{
	if(szCount > GetLength())
		szCount = GetLength();

	m_Chars.Del(szCount, GetLength());

	return *this;
}

KString& KString::SetRight(size_t szCount)
{
	if(szCount > GetLength())
		szCount = GetLength();

	m_Chars.Del(0, GetLength() - szCount);

	return *this;
}

KString& KString::SetMid(size_t szStart, size_t szCount)
{
	if(szStart > GetLength())
		szStart = GetLength();

	if(szCount == -1 || szStart + szCount > GetLength())
		szCount = GetLength() - szStart;

	return SetLeft(szStart + szCount).SetRight(szCount);
}

KString& KString::ToLower()
{
	_tcslwr(GetDataPtr());

	return *this;
}

KString& KString::ToUpper()
{
	_tcsupr(GetDataPtr());

	return *this;
}

KString KString::TrimmedLeft() const
{
	size_t i;
	for(i = 0 ; _istspace(m_Chars[i]) ; i++);

	return Mid(i);
}

KString KString::TrimmedRight() const
{
	size_t i;
	for(i = GetLength() - 1 ; i != -1 && _istspace(m_Chars[i]) ; i--);

	return Left(i + 1);
}

KString KString::Trimmed() const
{
	ptrdiff_t i;
	for(i = 0 ; _istspace(m_Chars[i]) ; i++);

	ptrdiff_t j;
	for(j = GetLength() - 1 ; j > i && _istspace(m_Chars[j]) ; j--);

	return Mid(i, j - i + 1);
}

KString& KString::TrimLeft()
{
	size_t i;
	for(i = 0 ; _istspace(m_Chars[i]) ; i++);

	return SetMid(i);
}

KString& KString::TrimRight()
{
	size_t i;
	for(i = GetLength() - 1 ; i != -1 && _istspace(m_Chars[i]) ; i--);

	return SetLeft(i + 1);
}

KString& KString::Trim()
{
	ptrdiff_t i;
	for(i = 0 ; _istspace(m_Chars[i]) ; i++);

	ptrdiff_t j;
	for(j = GetLength() - 1 ; j > i && _istspace(m_Chars[j]) ; j--);

	return SetMid(i, j - i + 1);
}

bool KString::IsTrimmed() const
{
	if(IsEmpty())
		return true;

	return !_istspace(m_Chars[0]) && !_istspace(m_Chars[GetLength() - 1]);
}

KString KString::TrimmedSingleSpace() const
{
	KString Result;

	LPCTSTR s = *this;

	// Trimming left
	while(_istspace(*s))
		s++;

	// Shrinking spaces
	while(*s)
	{
		if(_istspace(*s))
		{
			Result += TEXT(' ');

			for(s++ ; _istspace(*s) ; s++);
		}
		else
		{
			Result += *s++;
		}
	}

	// Trimming right
	return Result.TrimRight();
}

KString& KString::TrimSingleSpace()
{
	const LPTSTR q = GetDataPtr();

	LPCTSTR s = q;

	LPTSTR p = q;

	// Trimming left
	while(_istspace(*s))
		s++;

	// Shrinking spaces
	{
		while(*s)
		{
			if(_istspace(*s))
			{
				*p++ = TEXT(' ');

				for(s++ ; _istspace(*s) ; s++);
			}
			else
			{
				*p++ = *s++;
			}
		}

		SetLeft(p - q);
	}

	// Trimming right
	return TrimRight();
}

bool KString::IsTrimmedSingleSpace() const
{
	if(IsEmpty())
		return true;

	if(!IsTrimmed())
		return false;

	LPCTSTR s = *this;

	size_t i;

	for(i = 1 ; s[i] ; i++)
	{
		if(_istspace(s[i]) && (s[i] != TEXT(' ') || s[i-1] == TEXT(' ')))
			return false;
	}

	return true;
}

KString KString::UnAccelerated() const
{
	KString String;

	for(size_t i = 0 ; i < GetLength() ; i++)
	{
		if(GetChar(i) == TEXT('&'))
			i++;

		String += GetChar(i);
	}

	return String;
}

KString KString::RNToN() const
{
	KString String;

	for(size_t i = 0 ; i < GetLength() ; i++)
	{
		if(GetChar(i) == TEXT('\r') && GetChar(i + 1) == TEXT('\n'))
			i++;

		String += GetChar(i);
	}

	return String;
}

KString KString::NToRN() const
{
	KString String;

	for(size_t i = 0 ; i < GetLength() ; i++)
	{
		if(GetChar(i) == TEXT('\n'))
			String += TEXT('\r');

		String += GetChar(i);
	}

	return String;
}

int KString::QSortFunctionCase(const void* p1, const void* p2)
{
	return ((const KString*)p1)->Collate(*(const KString*)p2);
}

int KString::QSortFunctionNoCase(const void* p1, const void* p2)
{
	return ((const KString*)p1)->CollateNoCase(*(const KString*)p2);
}

int KString::QSortFunctionNumeric(const void* p1, const void* p2)
{
	TCHAR	Buf[1024];
	size_t	szBufLen;
	int		v1, v2;
	size_t	i;

	const KString& String1 = *(const KString*)p1;
	const KString& String2 = *(const KString*)p2;

	// Fetching first value
	szBufLen = 0;
	for(i=0 ; i<String1.GetLength() ; i++)
	{
		if(_istdigit(String1[i]) && szBufLen < ARRAY_SIZE(Buf) - 1)
			Buf[szBufLen++] = String1[i];
	}

	Buf[szBufLen] = 0, v1 = _ttoi(Buf);

	// Fetching second value
	szBufLen = 0;
	for(i=0 ; i<String2.GetLength() ; i++)
	{
		if(_istdigit(String2[i]) && szBufLen < ARRAY_SIZE(Buf) - 1)
			Buf[szBufLen++] = String2[i];
	}

	Buf[szBufLen] = 0, v2 = _ttoi(Buf);

	// Comparing
	if(v1 < v2)
		return -1;

	if(v1 > v2)
		return 1;

	return QSortFunctionNoCase(p1, p2);
}

int KString::QSortFunctionFileNoCase(const void* p1, const void* p2)
{
	return QSortFunctionNoCase(GetFileName(*(const KString*)p1), GetFileName(*(const KString*)p2));
}

int KString::QSortFunctionFileNumeric(const void* p1, const void* p2)
{
	return QSortFunctionNumeric(GetFileName(*(const KString*)p1), GetFileName(*(const KString*)p2));
}

KString KString::CurrentDateString()
{
	time_t long_time;

	time(&long_time);

	TCHAR Buf[128];

	_tcsftime(	Buf,
				sizeof(Buf) - 1,
				TEXT("%Y-%m-%d"),
				localtime(&long_time));

	return Buf;
}

KString KString::CurrentTimeString()
{
	time_t long_time;

	time(&long_time);

	TCHAR Buf[128];

	_tcsftime(	Buf,
				sizeof(Buf) - 1,
				TEXT("%H:%M:%S"),
				localtime(&long_time));

	return Buf;
}

KString KString::DateTimeString(const KString& DateString,
								const KString& TimeString)
{
	return DateString + TEXT("  ") + TimeString;
}

KString KString::CurrentDateTimeString()
{
	return DateTimeString(CurrentDateString(), CurrentTimeString());
}

#ifdef _MSC_VER

KString KString::GetOEMToChar() const
{
  TAnsiString Source(*this);
  KString Result(GetLength());
  OemToCharBuff(Source.GetDataPtr(), Result.GetDataPtr(), (DWORD)GetLength());
	return Result;
}

KString KString::GetCharToOEM() const
{
  TAnsiString Result;
  Result.SetN(GetLength() + 1);
	CharToOemBuff(GetDataPtr(), Result.GetDataPtr(), (DWORD)GetLength());
  Result.GetLastItem() = 0;
	return (LPCTSTR)TDefaultString(Result);
}

#endif // _MSC_VER

KString& KString::TrimByZero(size_t szStart)
{
	DEBUG_VERIFY(szStart <= GetLength());

  for(size_t i = szStart; i < GetLength(); i++)
    if((*this)[i] == 0)
    {
      SetLeft(i);
      break;
    }

	return *this;
}

bool KString::IsEmptyOrSpaces() const
{
	LPCTSTR s = GetDataPtr();

	for( ; *s ; s++)
	{
		if(!_istspace(*s))
			return false;
	}

	return true;
}

// -------------------------
// CP 1251 conversion table
// -------------------------
#ifndef _MSC_VER

static WCHAR gs_1251ToUnicode[0x100] =
{
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
	0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021, 0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
	0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x0098, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
	0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7, 0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
	0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7, 0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};

static char gs_UnicodeTo1251[0x10000];

struct TUnicodeTo1251Initializer
{
	TUnicodeTo1251Initializer()
	{
		memset(gs_UnicodeTo1251, '?', sizeof(gs_UnicodeTo1251));

		FOR_EACH_C_ARRAY(gs_1251ToUnicode, i)
			gs_UnicodeTo1251[gs_1251ToUnicode[i]] = (char)i;
	}
};

static TUnicodeTo1251Initializer gs_UnicodeTo1251Initializer;

#endif // _MSC_VER1

// ------------
// Ansi string
// ------------
TAnsiString::TAnsiString(size_t szLength)
{
  Add(szLength + 1);
  GetFirstItem() = 0;
}

TAnsiString::TAnsiString(LPCSTR pAnsiString, size_t szLength)
{
	if(szLength == -1)
		szLength = strlen(pAnsiString);

	SetN(szLength + 1);

	memcpy(GetDataPtr(), pAnsiString, szLength * sizeof(CHAR));

	GetDataPtr()[szLength] = 0;
}

TAnsiString::TAnsiString(LPCWSTR pWideString, size_t szLength, UINT uiCodePage)
{
	if(szLength == -1)
		szLength = wcslen(pWideString);

	#ifdef _MSC_VER
	{
		SetN(szLength * 4 + 1);

		szLength = WideCharToMultiByte(	uiCodePage,
										0,
										pWideString,
										(int)szLength,
										GetDataPtr(),
										(int)(GetN() - 1),
										NULL,
										NULL);

		UpdateMin(szLength, GetN() - 1);

		GetDataPtr()[szLength] = 0;

		SetN(szLength + 1);
	}
	#else // _MSC_VER
	{
		SetN(szLength + 1);

		LPCWSTR pSrc = pWideString;
		LPSTR   pDst = GetDataPtr();

		for(size_t i = 0 ; i < szLength ; i++)
			*pDst++ = gs_UnicodeTo1251[(WORD)*pSrc++];

		*pDst = 0;
	}
	#endif // _MSC_VER
}

// ------------
// Wide string
// ------------
TWideString::TWideString(size_t szLength)
{
  Add(szLength + 1);
  GetFirstItem() = 0;
}

TWideString::TWideString(LPCSTR pAnsiString, size_t szLength, UINT uiCodePage)
{
	if(szLength == -1)
		szLength = strlen(pAnsiString);

	#ifdef _MSC_VER
	{
		SetN(szLength * 4 + 1); // safety, normally szLength + 1 is enough

		szLength = MultiByteToWideChar(	uiCodePage,
										0,
										pAnsiString,
										(int)szLength,
										GetDataPtr(),
										(int)(GetN() - 1));

		UpdateMin(szLength, GetN() - 1);

		GetDataPtr()[szLength] = 0;

		SetN(szLength + 1);
	}
	#else // _MSC_VER
	{
		SetN(szLength + 1);

		LPCSTR pSrc = pAnsiString;
		LPWSTR pDst = GetDataPtr();

		for(size_t i = 0 ; i < szLength ; i++)
			*pDst++ = gs_1251ToUnicode[(BYTE)*pSrc++];

		*pDst = 0;
	}
	#endif // _MSC_VER
}

TWideString::TWideString(LPCWSTR pWideString, size_t szLength)
{
	if(szLength == -1)
		szLength = wcslen(pWideString);

	SetN(szLength + 1);

	memcpy(GetDataPtr(), pWideString, szLength * sizeof(WCHAR));

	GetDataPtr()[szLength] = 0;
}

// ----------------
// Global routines
// ----------------
LPCTSTR NullString(LPCTSTR pString)
{
	return pString ? pString : TEXT("(null)");
}

static inline int ncmp(LPCTSTR s1, size_t l1, LPCTSTR s2, size_t l2)
{
	while(l1 && l2)
	{
		if(int d = *s1 - *s2)
			return d;

		s1++, s2++, l1--, l2--;
	}

	return Sign(l1 - l2);
}

static inline int nicmp(LPCTSTR s1, size_t l1, LPCTSTR s2, size_t l2)
{
	while(l1 && l2)
	{
		if(int d = _totlower(*s1) - _totlower(*s2))
			return d;

		s1++, s2++, l1--, l2--;
	}

	return Sign(l1 - l2);
}

int str_alnum_cmp(LPCTSTR s1, LPCTSTR s2)
{
	DEBUG_VERIFY(s1 && s2);

	int fdl1 = 0;

	while(*s1 && *s2)
	{
		bool num1 = _istdigit(*s1);
		bool num2 = _istdigit(*s2);

		if(num1 != num2)
			return (int)num2 - (int)num1;

		if(num1)
		{
			LPCTSTR p1, p2;

			for(p1 = s1+1 ; *p1 && _istdigit(*p1) ; p1++);
			for(p2 = s2+1 ; *p2 && _istdigit(*p2) ; p2++);

			int dl1 = Compare(p1-s1, p2-s2);

			for( ; *s1 == TEXT('0') ; s1++);
			for( ; *s2 == TEXT('0') ; s2++);

			int dl2 = Compare(p1-s1, p2-s2);

			if(dl2)
				return dl2;

			if(int d = ncmp(s1, p1-s1, s2, p2-s2))
				return d;

			if(dl1 && !fdl1)
				fdl1 = dl1;

			s1 = p1, s2 = p2;
		}
		else
		{
			LPCTSTR p1, p2;

			for(p1 = s1+1 ; *p1 && !_istdigit(*p1) ; p1++);
			for(p2 = s2+1 ; *p2 && !_istdigit(*p2) ; p2++);

			if(int d = ncmp(s1, p1-s1, s2, p2-s2))
				return d;

			s1 = p1, s2 = p2;
		}
	}

	if(*s1 || *s2)
		return *s1 - *s2;

	return fdl1;
}

int str_alnum_icmp(LPCTSTR s1, LPCTSTR s2)
{
	DEBUG_VERIFY(s1 && s2);

	int fdl1 = 0;

	while(*s1 && *s2)
	{
		bool num1 = _istdigit(*s1);
		bool num2 = _istdigit(*s2);

		if(num1 != num2)
			return (int)num2 - (int)num1;

		if(num1)
		{
			LPCTSTR p1, p2;

			for(p1 = s1+1 ; *p1 && _istdigit(*p1) ; p1++);
			for(p2 = s2+1 ; *p2 && _istdigit(*p2) ; p2++);

			int dl1 = Compare(p1-s1, p2-s2);

			for( ; *s1 == TEXT('0') ; s1++);
			for( ; *s2 == TEXT('0') ; s2++);

			int dl2 = Compare(p1-s1, p2-s2);

			if(dl2)
				return dl2;

			if(int d = ncmp(s1, p1-s1, s2, p2-s2))
				return d;

			if(dl1 && !fdl1)
				fdl1 = dl1;

			s1 = p1, s2 = p2;
		}
		else
		{
			LPCTSTR p1, p2;

			for(p1 = s1+1 ; *p1 && !_istdigit(*p1) ; p1++);
			for(p2 = s2+1 ; *p2 && !_istdigit(*p2) ; p2++);

			if(int d = nicmp(s1, p1-s1, s2, p2-s2))
				return d;

			s1 = p1, s2 = p2;
		}
	}

	if(*s1 || *s2)
		return *s1 - *s2;

	return fdl1;
}

const char* stristr(const char* s1, const char* s2)
{
	size_t i, j;

	for(i = 0 ; s1[i] ; i++)
	{
		for(j = 0 ; s2[j] && tolower(s1[i+j]) == tolower(s2[j]) ; j++);

		if(!s2[j])
			return s1 + i;
	}

	return NULL;
}
