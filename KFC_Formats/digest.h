#ifndef digest_h
#define digest_h

// ----------------
// Global routines
// ----------------
KString MD5(const BYTE* pData, size_t szN);

inline KString MD5(const KString& Text)
{
	return MD5((const BYTE*)(LPCTSTR)Text, Text.GetStreamCharsLength());
}

inline KString MD5(LPCTSTR pText)
{
	DEBUG_VERIFY(pText);

	return MD5((const BYTE*)pText, _tcslen(pText) * sizeof(TCHAR));
}

#endif // digest_h
