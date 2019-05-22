#ifndef hex_h
#define hex_h

// ----------------
// Global routines
// ----------------
inline bool IsHexChar(TCHAR cChar)
{
	if(_istdigit(cChar))
		return true;

	cChar = _totupper(cChar);

	if(cChar >= TEXT('A') && cChar <= TEXT('F'))
		return true;

	return false;
}

inline TCHAR HexToChar(BYTE szValue)
{
	return	szValue < 10 ? (TCHAR)(TEXT('0') + szValue) :
			szValue < 16 ? (TCHAR)(TEXT('A') + szValue - 10) :
			TEXT('0');
}

inline BYTE CharToHex(TCHAR cChar)
{
	if(_istdigit(cChar))
		return cChar - TEXT('0');

	cChar = _totupper(cChar);

	if(cChar >= TEXT('A') && cChar <= TEXT('F'))
		return cChar - TEXT('A') + 10;

	return 0;
}

KString HexEncode(const void* pData, size_t szLength);

inline KString HexEncode(const KString& s)
{
	return HexEncode((LPCTSTR)s, s.GetLength());
}

void HexDecode(LPCTSTR p, void* pRData);

inline void HexDecode(LPCTSTR s, TArray<BYTE, true>& RData)
{
	HexDecode(s, &RData.SetN(_tcslen(s) / 2));
}

inline KString HexDecode(LPCTSTR s)
{
	KString Data;
	HexDecode(s, Data.Allocate(_tcslen(s) / 2));

	return Data;
}

#endif // hex_h
