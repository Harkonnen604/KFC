#include "kfc_ktl_pch.h"
#include "hex.h"

// ----------------
// Global routines
// ----------------
KString HexEncode(const void* pData, size_t szLength)
{
	DEBUG_VERIFY(!szLength || pData);

	const BYTE* p = (const BYTE*)pData;

	KString s;
	s.Allocate(szLength * 2);

	size_t i = 0;

	for( ; szLength ; szLength--, p++)
	{
		s.SetChar(i++, HexToChar(*p >> 4));
		s.SetChar(i++, HexToChar(*p & 0xF));
	}

	return s;
}

void HexDecode(LPCTSTR s, void* pRData)
{
	DEBUG_VERIFY(s);

	BYTE* p = (BYTE*)pRData;

	for( ; s[0] && s[1] ; s += 2)
		*p++ = (CharToHex(s[0]) << 4) | CharToHex(s[1]);
}
