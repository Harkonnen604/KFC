#ifndef kstring_h
#define kstring_h

#include "basic_types.h"
#include "basic_wintypes.h"
#include "array.h"

// Alphabet limitations
#define MAX_ALPHABET_RADIX	(10 + 26 + 26) // '0'-'9', 'A'-'Z', 'a'-'z'

// Declarations
class KString;
enum  TAlignment;
class TAnsiString;
class TWideString;

KString ToString(TAlignment Value);

// Casting
#define TO_STRING(v)	((v).operator KString ())

// -------
// String
// -------
class KString
{
private:
	TArray<TCHAR, true> m_Chars;

public:
	KString()
		{ Empty(); }

	KString(const KString& SString) : m_Chars(SString.m_Chars) {}

	KString(LPCTSTR pStr)
		{ *this = pStr; }

	KString(TCHAR c, size_t szNRepeats = 1);

	KString(LPCTSTR pStr, size_t szLength);

	KString(LPCTSTR pStr1, size_t szLength1, LPCTSTR pStr2, size_t szLength2);

	KString(INT64 iValue, bool bSeparate = false)
		{ WriteInt64(iValue,  bSeparate); }

	KString(QWORD qwValue, bool bSeparate = false)
		{ (INT64)qwValue >= -8 && (INT64)qwValue <= -1 ? WriteInt64((INT64)qwValue) : WriteQWORD(qwValue, bSeparate); }

	KString(long lValue, bool bSeparate = false)
		{ WriteInt64(lValue,  bSeparate); }

	KString(DWORD dwValue, bool bSeparate = false)
		{ (long)dwValue >= -8 && (long)dwValue <= -1 ? WriteInt64((long)dwValue) : WriteQWORD(dwValue, bSeparate); }

	KString(int iValue, bool bSeparate = false)
		{ WriteInt64(iValue,  bSeparate); }

	KString(UINT uiValue, bool bSeparate = false)
		{ (int)uiValue >= -8 && (int)uiValue <= -1 ? WriteInt64((int)uiValue) : WriteQWORD(uiValue, bSeparate); }

	KString(short sValue, bool bSeparate = false)
		{ WriteInt64(sValue, bSeparate); }

	KString(WORD wValue, bool bSeparate = false)
		{ (short)wValue >= -8 && (short)wValue <= -1 ? WriteInt64((short)wValue) : WriteQWORD(wValue, bSeparate); }

	KString(BYTE bValue)
		{ WriteQWORD(bValue); }

	KString(float fValue)
		{ *this = fValue; }

	KString(double dValue)
		{ *this = dValue; }

	KString(bool bValue)
		{ *this = bValue; }

	KString(TAlignment Value);

	LPTSTR Allocate(size_t szLength);

	LPTSTR Allocate(LPCTSTR s)
		{ return Allocate(s, _tcslen(s)); }

	LPTSTR Allocate(LPCTSTR s, size_t l)
		{ return (LPTSTR)memcpy(Allocate(l), s, l * sizeof(TCHAR)); }

	void Empty()
		{ Allocate(0u); }

	size_t GetLength() const
		{ return m_Chars.GetN() - 1; }

	static bool IsSpaceChar(TCHAR cChar);

	static TCHAR ValueToAlphabet(size_t szValue);

	static size_t AlphabetToValue(TCHAR cChar);

	size_t GetNoSpaceCharactersLength() const;

	bool IsEmpty() const
		{ return GetLength() == 0; }

	void Terminate()
		{ m_Chars[GetLength()] = 0; }

	LPTSTR Extend(size_t l);

	LPTSTR Extend(LPCTSTR s)
		{ return Extend(s, _tcslen(s)); };

	LPTSTR Extend(LPCTSTR s, size_t l);

	KString& WriteInt64(INT64 iValue,  bool bSeparate = false);
	KString& WriteQWORD(QWORD qwValue, bool bSeparate = false);

	KString& Format	(LPCTSTR pFormat, ...);
	KString& FormatV(LPCTSTR pFormat, va_list ArgList);

	static KString Formatted	(LPCTSTR pFormat, ...);
	static KString FormattedV	(LPCTSTR pFormat, va_list ArgList);

	static size_t GetStreamCharsLength		(size_t szLength) { return szLength * sizeof(TCHAR);						}
	static size_t GetStreamTermCharsLength	(size_t szLength) { return GetStreamCharsLength(szLength + 1);				}
	static size_t GetStreamTotalLength		(size_t szLength) { return sizeof(size_t) + GetStreamCharsLength(szLength);	}

	size_t GetStreamCharsLength		() const { return GetStreamCharsLength		(GetLength()); }
	size_t GetStreamTermCharsLength	() const { return GetStreamTermCharsLength	(GetLength()); }
	size_t GetStreamTotalLength		() const { return GetStreamTotalLength		(GetLength()); }

	KString& operator = (const KString& SString);

	KString& operator = (LPCTSTR pString);

	KString& operator = (TCHAR cChar);

	KString& operator = (INT64 iValue)
		{ return WriteInt64(iValue); }

	KString& operator = (QWORD qwValue)
	{
		return	(INT64)qwValue >= -8 && (INT64)qwValue <= -1 ?
					WriteInt64((INT64)qwValue) :
					WriteQWORD(qwValue);
	}

	KString& operator = (long lValue)
		{ return WriteInt64(lValue); }

	KString& operator = (DWORD dwValue)
	{
		return	(long)dwValue >= -8 && (long)dwValue <= -1 ?
					WriteInt64((long)dwValue) :
					WriteQWORD(dwValue);
	}

	KString& operator = (int iValue)
		{ return WriteInt64(iValue); }

	KString& operator = (UINT uiValue)
	{
		return	(int)uiValue >= -8 && (int)uiValue <= -1 ?
					WriteInt64((int)uiValue) :
					WriteQWORD(uiValue);
	}

	KString& operator = (short sValue)
		{ return WriteInt64(sValue); }

	KString& operator = (WORD wValue)
	{
		return	(short)wValue >= -8 && (short)wValue <= -1 ?
					WriteInt64((short)wValue) :
					WriteQWORD(wValue);
	}

	KString& operator = (BYTE bValue)
		{ return WriteQWORD(bValue); }

	KString& operator = (float fValue);

	KString& operator = (double dValue);

	KString& operator = (bool bValue);

	KString& operator = (TAlignment Value);

	KString operator + (const KString& SString) const;

	KString operator + (LPCTSTR pString) const;

	KString operator + (TCHAR cChar) const;

	KString operator + (INT64 iValue) const;

	KString operator + (QWORD qwValue) const;

	KString operator + (long lValue) const
		{ return *this + (INT64)lValue; }

	KString operator + (DWORD dwValue) const
	{
		return	(long)dwValue >= -8 && (long)dwValue <= -1 ?
			(*this + (INT64)(long)dwValue) :
			(*this + (QWORD)dwValue);
	}

	KString operator + (int iValue) const
		{ return *this + (INT64)iValue; }

	KString operator + (UINT uiValue) const
	{
		return	(int)uiValue >= -8 && (int)uiValue <= -1 ?
					(*this + (INT64)(int)uiValue) :
					(*this + (QWORD)uiValue);
	}

	KString operator + (short sValue) const
		{ return *this + (INT64)sValue; }

	KString operator + (WORD wValue) const
	{
		return	(short)wValue >= -8 && (short)wValue <= -1 ?
					(*this + (INT64)(short)wValue) :
					(*this + (QWORD)wValue);
	}

	KString operator + (BYTE bValue) const
		{ return *this + (QWORD)bValue; }

	KString operator + (float fValue) const;

	KString operator + (double dValue) const;

	KString operator + (bool bValue) const;

	KString operator + (TAlignment Value) const;

	KString& operator += (const KString& SString);

	KString& operator += (LPCTSTR pString);

	KString& operator += (TCHAR cChar);

	KString& operator += (INT64 iValue);

	KString& operator += (QWORD qwValue);

	KString& operator += (long lValue)
		{ return *this += (INT64)lValue; }

	KString& operator += (DWORD dwValue)
	{
		return	(long)dwValue >= -8 && (long)dwValue <= -1 ?
			(*this += (INT64)(long)dwValue) :
			(*this += (QWORD)dwValue);
	}

	KString& operator += (int iValue)
		{ return *this += (INT64)iValue; }

	KString& operator += (UINT uiValue)
	{
		return	(int)uiValue >= -8 && (int)uiValue <= -1 ?
					(*this += (INT64)(int)uiValue) :
					(*this += (QWORD)uiValue);
	}

	KString& operator += (short sValue)
		{ return *this += (INT64)sValue; }

	KString& operator += (WORD wValue)
	{
		return	(short)wValue >= -8 && (short)wValue <= -1 ?
					(*this += (INT64)(short)wValue) :
					(*this += (QWORD)wValue);
	}

	KString& operator += (BYTE bValue)
		{ return *this += (QWORD)bValue; }

	KString& operator += (float fValue);

	KString& operator += (double dValue);

	KString& operator += (bool bValue);

	KString& operator += (TAlignment Value);

	LPTSTR GetDataPtr()
		{ return m_Chars.GetDataPtr(); }

	LPCTSTR	GetDataPtr() const
		{ return m_Chars.GetDataPtr(); }

	TCHAR GetChar(size_t szIndex) const { return m_Chars[szIndex]; }

	void SetChar(size_t szIndex, TCHAR c) { m_Chars[szIndex] = c; }

	KString GetLower() const;
	KString GetUpper() const;

	size_t Find(LPCTSTR pString, size_t szStart = 0) const;

	size_t FindNoCase(LPCTSTR pString, size_t szStart = 0) const
		{ return GetLower().Find(KString(pString).ToLower(), szStart); }

	size_t Find(TCHAR cChar, size_t szStart = 0) const;

	size_t FindNoCase(TCHAR cChar, size_t szStart = 0) const
		{ return GetLower().Find(_totlower(cChar), szStart); }

	size_t FindOneOf(LPCTSTR pChars, size_t szStart = 0) const;

	size_t FindOneOfNoCase(LPCTSTR pChars, size_t szStart = 0) const
		{ return GetLower().FindOneOf(KString(pChars).ToLower(), szStart); }

	// UINT_MAX rev finder result is ambiguous since UINT_MAX can also mean pre-start

	size_t FindRev(LPCTSTR pString, size_t szStart = UINT_MAX - 1)  const;

	size_t FindRevNoCase(LPCTSTR pString, size_t szStart = UINT_MAX - 1) const
		{ return GetLower().FindRev(KString(pString).ToLower(), szStart); }

	size_t FindRev(TCHAR cChar, size_t szStart = UINT_MAX - 1) const;

	size_t FindRevNoCase(TCHAR cChar, size_t szStart = UINT_MAX - 1) const
		{ return GetLower().FindRev(_totlower(cChar), szStart); }

	size_t FindOneOfRev(LPCTSTR pChars, size_t szStart = UINT_MAX - 1) const;

	size_t FindOneOfRevNoCase(LPCTSTR pChars, size_t szStart = UINT_MAX - 1) const
		{ return GetLower().FindOneOfRev(KString(pChars).ToLower(), szStart); }

	KString Left	(size_t szCount) const;
	KString Right	(size_t szCount) const;
	KString Mid		(size_t szStart, size_t szCount = UINT_MAX) const;

	KString& SetLeft	(size_t szCount);
	KString& SetRight	(size_t szCount);
	KString& SetMid		(size_t szStart, size_t szCount = UINT_MAX);

	int CompareStart(const KString& SString) const
		{ return _tcsncoll(*this, SString, SString.GetLength()); }

	int CompareStartNoCase(const KString& SString) const
		{ return _tcsnicoll(*this, SString, SString.GetLength()); }

	int CompareEnd(const KString& SString) const
		{ return GetLength() >= SString.GetLength() ? _tcscoll((LPCTSTR)*this + (GetLength() - SString.GetLength()), SString) : -1; }

	int CompareEndNoCase(const KString& SString) const
		{ return GetLength() >= SString.GetLength() ? _tcsicoll((LPCTSTR)*this + (GetLength() - SString.GetLength()), SString) : -1; }

	bool DoesStart(const KString& String) const
	{
		return	GetLength() >= String.GetLength() &&
				!_tcsncoll(*this, String, String.GetLength());
	}

	bool DoesStart(LPCTSTR s) const
	{
		size_t l = _tcslen(s);

		return GetLength() >= l && !_tcsncoll(*this, s, l);
	}

	bool DoesStartNoCase(const KString& String) const
	{
		return	GetLength() >= String.GetLength() &&
				!_tcsnicoll(*this, String, String.GetLength());
	}

	bool DoesStartNoCase(LPCTSTR s) const
	{
		size_t l = _tcslen(s);

		return GetLength() >= l && !_tcsnicoll(*this, s, l);
	}

	bool DoesEnd(const KString& String) const
	{
		return	GetLength() >= String.GetLength() &&
				!_tcscoll((LPCTSTR)*this + (GetLength() - String.GetLength()), String);
	}

	bool DoesEnd(LPCTSTR s) const
	{
		size_t l = _tcslen(s);

		return	GetLength() >= l &&
				!_tcscoll((LPCTSTR)*this + (GetLength() - l), s);
	}

	bool DoesEndNoCase(const KString& String) const
	{
		return	GetLength() >= String.GetLength() &&
				!_tcsicoll((LPCTSTR)*this + (GetLength() - String.GetLength()), String);
	}

	bool DoesEndNoCase(LPCTSTR s) const
	{
		size_t l = _tcslen(s);

		return	GetLength() >= l &&
				!_tcsicoll((LPCTSTR)*this + (GetLength() - l), s);
	}

	KString TrimmedLeft	() const;
	KString TrimmedRight() const;
	KString Trimmed		() const;

	KString& TrimLeft ();
	KString& TrimRight();
	KString& Trim     ();

	bool IsTrimmed() const;

	KString TrimmedSingleSpace() const; // \r\n\t... -> ' '

	KString& TrimSingleSpace();

	bool IsTrimmedSingleSpace() const;

	KString& ToLower();
	KString& ToUpper();

	KString UnAccelerated() const;

	KString RNToN() const;
	KString NToRN() const;

	int Compare			(LPCTSTR pString) const { return Sign(_tcscmp (GetDataPtr(), pString)); }
	int CompareNoCase	(LPCTSTR pString) const { return Sign(_tcsicmp(GetDataPtr(), pString)); }

	int Collate			(LPCTSTR pString) const { return Sign(_tcscoll (GetDataPtr(), pString)); }
	int CollateNoCase	(LPCTSTR pString) const { return Sign(_tcsicoll(GetDataPtr(), pString)); }

	bool operator <  (const KString& SString) const { return Collate(SString) <  0; }
	bool operator >  (const KString& SString) const { return Collate(SString) >  0; }
	bool operator <= (const KString& SString) const { return Collate(SString) <= 0; }
	bool operator >= (const KString& SString) const { return Collate(SString) >= 0; }

	bool operator == (TCHAR cChar) const
		{ return GetLength() == 1 && m_Chars[0] == cChar; }

	bool operator == (const KString& SString) const
	{
		return	GetLength() == SString.GetLength() &&
					Collate(SString) == 0;
	}

	bool operator != (const KString& SString) const
	{
		return	GetLength() != SString.GetLength() ||
					Collate(SString) != 0;
	}

	bool operator <  (LPCTSTR pString) const { return Collate(pString) <  0; }
	bool operator >  (LPCTSTR pString) const { return Collate(pString) >  0; }
	bool operator <= (LPCTSTR pString) const { return Collate(pString) <= 0; }
	bool operator >= (LPCTSTR pString) const { return Collate(pString) >= 0; }
	bool operator == (LPCTSTR pString) const { return Collate(pString) == 0; }
	bool operator != (LPCTSTR pString) const { return Collate(pString) != 0; }

	static int QSortFunctionCase		(const void* p1, const void* p2);
	static int QSortFunctionNoCase		(const void* p1, const void* p2);
	static int QSortFunctionNumeric		(const void* p1, const void* p2);
	static int QSortFunctionFileNoCase	(const void* p1, const void* p2);
	static int QSortFunctionFileNumeric	(const void* p1, const void* p2);

	static KString CurrentDateString();
	static KString CurrentTimeString();

	static KString DateTimeString(	const KString& DateString,
									const KString& TimeString);

	static KString CurrentDateTimeString();

	operator LPCTSTR () const { return GetDataPtr(); }

	#ifdef _MSC_VER

		KString GetOEMToChar() const;
		KString GetCharToOEM() const;

	#else // _MSC_VER

		KString GetOEMToChar() const
			{ return *this; }

		KString GetCharToOEM() const
			{ return *this; }

	#endif // _MSC_VER

	KString& TrimByZero(size_t szStart = 0);

	KString& DelFirstChar()
		{ return IsEmpty() ? *this : SetMid(1); }

	KString& DelLastChar()
		{ return IsEmpty() ? *this : SetLeft(GetLength() - 1); }

	TCHAR GetFirstChar() const
		{ return GetLength() < 1 ? 0 : m_Chars[0]; }

	TCHAR GetPostFirstChar() const
		{ return GetLength() < 2 ? 0 : m_Chars[1]; }

	TCHAR GetLastChar() const
		{ return GetLength() < 1 ? 0 : m_Chars[GetLength() - 1]; }

	TCHAR GetPreLastChar() const
		{ return GetLength() < 2 ? 0 : m_Chars[GetLength() - 2]; }

	bool IsSpaces() const
		{ return !IsEmpty() && IsEmptyOrSpaces(); }

	bool IsEmptyOrSpaces() const;
};

extern const KString g_EmptyString;

// Streaming
template <class t>
inline KString& operator << (KString& String, const t& v)
	{ return String += v; }

// Compare
inline int Compare(const KString& String1, const KString& String2)
	{ return _tcscmp(String1, String2); }

inline int Compare(const KString& String1, LPCTSTR pString2)
	{ return _tcscmp(String1, pString2); }

inline int Compare(LPCTSTR pString1, const KString& String2)
	{ return _tcscmp(pString1, String2); }

inline int Compare(LPCTSTR pString1, LPCTSTR pString2)
	{ return _tcscmp(pString1, pString2); }

// Compare no case
inline int CompareNoCase(const KString& String1, const KString& String2)
	{ return _tcsicmp(String1, String2); }

inline int CompareNoCase(const KString& String1, LPCTSTR pString2)
	{ return _tcsicmp(String1, pString2); }

inline int CompareNoCase(LPCTSTR pString1, const KString& String2)
	{ return _tcsicmp(pString1, String2); }

inline int CompareNoCase(LPCTSTR pString1, LPCTSTR pString2)
	{ return _tcsicmp(pString1, pString2); }

// Compare alpha-numeric
int str_alnum_cmp(LPCTSTR s1, LPCTSTR s2);

inline int CompareAlphaNumeric(const KString& String1, const KString& String2)
	{ return str_alnum_cmp(String1, String2); }

inline int CompareAlphaNumeric(const KString& String1, LPCTSTR pString2)
	{ return str_alnum_cmp(String1, pString2); }

inline int CompareAlphaNumeric(LPCTSTR pString1, const KString& String2)
	{ return str_alnum_cmp(pString1, String2); }

inline int CompareAlphaNumeric(LPCTSTR pString1, LPCTSTR pString2)
	{ return str_alnum_cmp(pString1, pString2); }

// Compare alpha-numeric no case
int str_alnum_icmp(LPCTSTR s1, LPCTSTR s2);

inline int CompareAlphaNumericNoCase(const KString& String1, const KString& String2)
	{ return str_alnum_icmp(String1, String2); }

inline int CompareAlphaNumericNoCase(const KString& String1, LPCTSTR pString2)
	{ return str_alnum_icmp(String1, pString2); }

inline int CompareAlphaNumericNoCase(LPCTSTR pString1, const KString& String2)
	{ return str_alnum_icmp(pString1, String2); }

inline int CompareAlphaNumericNoCase(LPCTSTR pString1, LPCTSTR pString2)
	{ return str_alnum_icmp(pString1, pString2); }

// Collate
inline int Collate(const KString& String1, const KString& String2)
	{ return _tcscoll(String1, String2); }

inline int Collate(const KString& String1, LPCTSTR pString2)
	{ return _tcscoll(String1, pString2); }

inline int Collate(LPCTSTR pString1, const KString& String2)
	{ return _tcscoll(pString1, String2); }

inline int Collate(LPCTSTR pString1, LPCTSTR pString2)
	{ return _tcscoll(pString1, pString2); }

// Collate no case
inline int CollateNoCase(const KString& String1, const KString& String2)
	{ return _tcsicoll(String1, String2); }

inline int CollateNoCase(const KString& String1, LPCTSTR pString2)
	{ return _tcsicoll(String1, pString2); }

inline int CollateNoCase(LPCTSTR pString1, const KString& String2)
	{ return _tcsicoll(pString1, String2); }

inline int CollateNoCase(LPCTSTR pString1, LPCTSTR pString2)
	{ return _tcsicoll(pString1, pString2); }

// Operators
inline bool operator < (LPCTSTR pString1, const KString& String2)
	{ return String2.Collate(pString1) > 0; }

inline bool operator > (LPCTSTR pString1, const KString& String2)
	{ return String2.Collate(pString1) < 0; }

inline bool operator <= (LPCTSTR pString1, const KString& String2)
	{ return String2.Collate(pString1) >= 0; }

inline bool operator >= (LPCTSTR pString1, const KString& String2)
	{ return String2.Collate(pString1) <= 0; }

inline bool operator == (LPCTSTR pString1, const KString& String2)
	{ return String2.Collate(pString1) == 0; }

inline bool operator != (LPCTSTR pString1, const KString& String2)
	{ return String2.Collate(pString1) != 0; }

inline KString operator + (LPCTSTR pString1, const KString& String2)
	{ return (KString)pString1 + String2; }

inline KString operator + (TCHAR c, const KString& String)
	{ return (KString)c + String; }

// ------------
// Ansi string
// ------------
class TAnsiString : public TArray<char, true>
{
public:
	TAnsiString(size_t szLength = 0);

	TAnsiString(LPCSTR pAnsiString, size_t szLength = UINT_MAX);

	TAnsiString(LPCWSTR pWideString, size_t szLength = UINT_MAX, UINT uiCodePage = CP_ACP);

	size_t GetLength() const { return GetN() - 1; }

  bool IsEmpty() const { return GetLength() == 0; }

	TAnsiString operator + (LPCSTR s) const
	{
		size_t l = strlen(s);

		TAnsiString ret;
		ret.SetN(GetLength() + l + 1);

		memcpy(ret.GetDataPtr(), GetDataPtr(), GetLength());
		memcpy(ret.GetDataPtr() + GetLength(), s, l + 1);

		return ret;
	}

	operator LPSTR	()			{ return GetDataPtr(); }
	operator LPCSTR	() const	{ return GetDataPtr(); }
};

inline TStream& operator >> (TStream& Stream, TAnsiString& RString)
{
	size_t szLength;

	Stream >> szLength;

	RString.SetN(szLength + 1);

	STREAM_READ_ARR(Stream, RString.GetDataPtr(), szLength, TCHAR);

	RString.GetLastItem() = 0;

	return Stream;
}

inline TStream& operator << (TStream& Stream, const TAnsiString& String)
{
	Stream << String.GetLength();

	STREAM_WRITE_ARR(Stream, String.GetDataPtr(), String.GetLength(), TCHAR);

	return Stream;
}

inline int Compare(const TAnsiString& String1, const TAnsiString& String2)
{
	return strcoll(String1, String2);
}

// ansi-ansi
inline bool operator == (const TAnsiString& String1, const TAnsiString& String2)
{
	return	String1.GetLength() == String2.GetLength() &&
				strcoll(String1, String2) == 0;
}

inline bool operator != (const TAnsiString& String1, const TAnsiString& String2)
{
	return	String1.GetLength() != String2.GetLength() ||
				strcoll(String1, String2) != 0;
}

inline bool operator < (const TAnsiString& String1, const TAnsiString& String2)
{
	return Compare(String1, String2) < 0;
}

inline bool operator > (const TAnsiString& String1, const TAnsiString& String2)
{
	return Compare(String1, String2) > 0;
}

inline bool operator <= (const TAnsiString& String1, const TAnsiString& String2)
{
	return Compare(String1, String2) <= 0;
}

inline bool operator >= (const TAnsiString& String1, const TAnsiString& String2)
{
	return Compare(String1, String2) >= 0;
}

// ansi-s
inline bool operator == (const TAnsiString& String1, LPCSTR pString2)
{
	return strcoll(String1, pString2) == 0;
}

inline bool operator != (const TAnsiString& String1, LPCSTR pString2)
{
	return strcoll(String1, pString2) != 0;
}

inline bool operator < (const TAnsiString& String1, LPCSTR pString2)
{
	return strcoll(String1, pString2) < 0;
}

inline bool operator > (const TAnsiString& String1, LPCSTR pString2)
{
	return strcoll(String1, pString2) > 0;
}

inline bool operator <= (const TAnsiString& String1, LPCSTR pString2)
{
	return strcoll(String1, pString2) <= 0;
}

inline bool operator >= (const TAnsiString& String1, LPCSTR pString2)
{
	return strcoll(String1, pString2) >= 0;
}

// s-ansi
inline bool operator == (LPCSTR pString1, const TAnsiString& String2)
{
	return strcoll(pString1, String2) == 0;
}

inline bool operator != (LPCSTR pString1, const TAnsiString& String2)
{
	return strcoll(pString1, String2) != 0;
}

inline bool operator < (LPCSTR pString1, const TAnsiString& String2)
{
	return strcoll(pString1, String2) < 0;
}

inline bool operator > (LPCSTR pString1, const TAnsiString& String2)
{
	return strcoll(pString1, String2) > 0;
}

inline bool operator <= (LPCSTR pString1, const TAnsiString& String2)
{
	return strcoll(pString1, String2) <= 0;
}

inline bool operator >= (LPCSTR pString1, const TAnsiString& String2)
{
	return strcoll(pString1, String2) >= 0;
}

// ------------
// Wide string
// ------------
class TWideString : public TArray<wchar_t, true>
{
public:
	TWideString(size_t szLength = 0);

	TWideString(LPCSTR pAnsiString, size_t szLength = UINT_MAX, UINT uiCodePage = CP_ACP);

	TWideString(LPCWSTR pWideString, size_t szLength = UINT_MAX);

	size_t GetLength() const { return GetN() - 1; }

  bool IsEmpty() const { return GetLength() == 0; }

	TWideString operator + (LPCWSTR s) const
	{
		size_t l = wcslen(s);

		TWideString ret;
		ret.SetN(GetLength() + l + 1);

		memcpy(ret.GetDataPtr(), GetDataPtr(), GetLength() * sizeof(WCHAR));
		memcpy(ret.GetDataPtr() + GetLength(), s, (l + 1) * sizeof(WCHAR));

		return ret;
	}

	operator LPWSTR	()			{ return GetDataPtr(); }
	operator LPCWSTR() const	{ return GetDataPtr(); }
};

inline TStream& operator >> (TStream& Stream, TWideString& RString)
{
	size_t szLength;

	Stream >> szLength;

	RString.SetN(szLength + 1);

	STREAM_READ_ARR(Stream, RString.GetDataPtr(), szLength, wchar_t);

	RString.GetLastItem() = 0;

	return Stream;
}

inline TStream& operator << (TStream& Stream, const TWideString& String)
{
	Stream << String.GetLength();

	STREAM_WRITE_ARR(Stream, String.GetDataPtr(), String.GetLength(), wchar_t);

	return Stream;
}

inline int Compare(const TWideString& String1, const TWideString& String2)
{

	return wcscmp(String1, String2);
}

// wide-wide
inline bool operator == (const TWideString& String1, const TWideString& String2)
{
	return	String1.GetLength() == String2.GetLength() &&
				wcscmp(String1, String2) == 0;
}

inline bool operator != (const TWideString& String1, const TWideString& String2)
{
	return	String1.GetLength() != String2.GetLength() ||
				wcscmp(String1, String2) != 0;
}

inline bool operator < (const TWideString& String1, const TWideString& String2)
{
	return Compare(String1, String2) < 0;
}

inline bool operator > (const TWideString& String1, const TWideString& String2)
{
	return Compare(String1, String2) > 0;
}

inline bool operator <= (const TWideString& String1, const TWideString& String2)
{
	return Compare(String1, String2) <= 0;
}

inline bool operator >= (const TWideString& String1, const TWideString& String2)
{
	return Compare(String1, String2) >= 0;
}

// wide-s
inline bool operator == (const TWideString& String1, LPCWSTR pString2)
{
	return wcscmp(String1, pString2) == 0;
}

inline bool operator != (const TWideString& String1, LPCWSTR pString2)
{
	return wcscmp(String1, pString2) != 0;
}

inline bool operator < (const TWideString& String1, LPCWSTR pString2)
{
	return wcscmp(String1, pString2) < 0;
}

inline bool operator > (const TWideString& String1, LPCWSTR pString2)
{
	return wcscmp(String1, pString2) > 0;
}

inline bool operator <= (const TWideString& String1, LPCWSTR pString2)
{
	return wcscmp(String1, pString2) <= 0;
}

inline bool operator >= (const TWideString& String1, LPCWSTR pString2)
{
	return wcscmp(String1, pString2) >= 0;
}

// s-wide
inline bool operator == (LPCWSTR pString1, const TWideString& String2)
{
	return wcscmp(pString1, String2) == 0;
}

inline bool operator != (LPCWSTR pString1, const TWideString& String2)
{
	return wcscmp(pString1, String2) != 0;
}

inline bool operator < (LPCWSTR pString1, const TWideString& String2)
{
	return wcscmp(pString1, String2) < 0;
}

inline bool operator > (LPCWSTR pString1, const TWideString& String2)
{
	return wcscmp(pString1, String2) > 0;
}

inline bool operator <= (LPCWSTR pString1, const TWideString& String2)
{
	return wcscmp(pString1, String2) <= 0;
}

inline bool operator >= (LPCWSTR pString1, const TWideString& String2)
{
	return wcscmp(pString1, String2) >= 0;
}

// ---------------
// Default string
// ---------------
#ifdef _UNICODE
	typedef TWideString TDefaultString;
#else // _UNICODE
	typedef TAnsiString TDefaultString;
#endif // _UNICODE

// def-k
inline bool operator == (const TDefaultString& String1, const KString& String2)
{
	return	String1.GetLength() == String2.GetLength() &&
				(LPCTSTR)String1 == String2;
}

inline bool operator != (const TDefaultString& String1, const KString& String2)
{
	return	String1.GetLength() != String2.GetLength() ||
				(LPCTSTR)String1 != String2;
}

inline bool operator < (const TDefaultString& String1, const KString& String2)
{
	return (LPCTSTR)String1 < String2;
}

inline bool operator > (const TDefaultString& String1, const KString& String2)
{
	return (LPCTSTR)String1 > String2;
}

inline bool operator <= (const TDefaultString& String1, const KString& String2)
{
	return (LPCTSTR)String1 <= String2;
}

inline bool operator >= (const TDefaultString& String1, const KString& String2)
{
	return (LPCTSTR)String1 >= String2;
}

// k-def
inline bool operator == (const KString& String1, const TDefaultString& String2)
{
	return String1.GetLength() == String2.GetLength() && String1 == (LPCTSTR)String2;
}

inline bool operator != (const KString& String1, const TDefaultString& String2)
{
	return String1.GetLength() != String2.GetLength() || String1 != (LPCTSTR)String2;
}

inline bool operator < (const KString& String1, const TDefaultString& String2)
{
	return String1 < (LPCTSTR)String2;
}

inline bool operator > (const KString& String1, const TDefaultString& String2)
{
	return String1 > (LPCTSTR)String2;
}

inline bool operator <= (const KString& String1, const TDefaultString& String2)
{
	return String1 <= (LPCTSTR)String2;
}

inline bool operator >= (const KString& String1, const TDefaultString& String2)
{
	return String1 >= (LPCTSTR)String2;
}

// -----------------
// String streaming
// -----------------
inline TStream& operator >> (TStream& Stream, KString& RString)
{
	size_t szLength;
	Stream >> szLength;

	RString.Allocate(szLength);

	STREAM_READ_ARR(Stream, RString.GetDataPtr(), szLength, TCHAR);

	return Stream;
}

inline TStream& operator << (TStream& Stream, const KString& String)
{
	Stream << String.GetLength();

	STREAM_WRITE_ARR(Stream, String.GetDataPtr(), String.GetLength(), TCHAR);

	return Stream;
}

inline TStream& operator << (TStream& Stream, const char* pString)
{
	const size_t szLength = strlen(pString);

	Stream << szLength;

	STREAM_WRITE_ARR(Stream, pString, szLength, char);

	return Stream;
}

inline TStream& operator << (TStream& Stream, const wchar_t* pString)
{
	const size_t szLength = wcslen(pString);

	Stream << szLength;

	STREAM_WRITE_ARR(Stream, pString, szLength, wchar_t);

	return Stream;
}

// ----------------
// Global routines
// ----------------
LPCTSTR NullString(LPCTSTR pString);

const char* stristr(const char* s1, const char* s2);

#endif // kstring_h
