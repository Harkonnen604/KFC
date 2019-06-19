#ifndef encoding_h
#define encoding_h

/*
// ---------
// Encoding
// ---------
class TEncoding
{
private:
	KString m_Name;

	TArray<BYTE, true> m_EncData;
	TArray<BYTE, true> m_DecData;

public:
	TEncoding();

	TEncoding(LPCTSTR pSName, LPCTSTR pFileNamePrefix);

	~TEncoding()
		{ Release(); }

	bool IsAllocated() const
		{ return !m_Name.IsEmpty(); }

	void Release();

	void AllocateIdentity(LPCTSTR pSName);

	void Allocate(LPCTSTR pSName, LPCTSTR pFileNamePrefix = NULL);

	void AllocateOEM(LPCTSTR pSName = TEXT("oem"));

	KString Encode(LPCSTR s, size_t l = UINT_MAX) const;
	KString Decode(LPCSTR s, size_t l = UINT_MAX) const;

	LPSTR EncodeSelf(LPSTR s, size_t l = UINT_MAX) const;
	LPSTR DecodeSelf(LPSTR s, size_t l = UINT_MAX) const;

	const KString& GetName() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Name; }
};

// ----------------
// Global routines
// ----------------

// Encoding
KString Encode(LPCTSTR pName, LPCSTR s, size_t l = UINT_MAX);
KString Decode(LPCTSTR pName, LPCSTR s, size_t l = UINT_MAX);

KString EncodeSafe(LPCTSTR pName, LPCSTR s, size_t l = UINT_MAX);
KString DecodeSafe(LPCTSTR pName, LPCSTR s, size_t l = UINT_MAX);

LPSTR EncodeSelf(LPCTSTR pName, LPSTR s, size_t l = UINT_MAX);
LPSTR DecodeSelf(LPCTSTR pName, LPSTR s, size_t l = UINT_MAX);

LPSTR EncodeSelfSafe(LPCTSTR pName, LPSTR s, size_t l = UINT_MAX);
LPSTR DecodeSelfSafe(LPCTSTR pName, LPSTR s, size_t l = UINT_MAX);
*/

// Base64
KString EncodeBase64(const BYTE* p, size_t l);
void DecodeBase64(TArray<BYTE, true>& RData, LPCSTR s, size_t l = UINT_MAX);

/*
// Quoted-printable custom
KString EncodeQuotedPrintableCustom(char		cPrefix,
									char		cSpace,
									LPCSTR		pDelimeters,
									const BYTE*	p,
									size_t		l);

KString EncodeQuotedPrintableCustom(char	cPrefix,
									char	cSpace,
									LPCSTR	pDelimeters,
									LPCSTR	s,
									size_t	l = UINT_MAX);

void DecodeQuotedPrintableCustom(	char				cPrefix,
									char				cSpace,
									LPCSTR				pDelimeters,
									TArray<BYTE, true>&	RData,
									LPCSTR				s,
									size_t				l = UINT_MAX);

KString DecodeQuotedPrintableCustom(char				cPrefix,
									char				cSpace,
									LPCSTR				pDelimeters,
									LPCSTR				s,
									size_t				l = UINT_MAX);

// Quoted-printable
inline KString EncodeQuotedPrintable(const BYTE* p, size_t l)
	{ return EncodeQuotedPrintableCustom('=', '_', NULL, p, l); };

inline KString EncodeQuotedPrintable(LPCSTR s, size_t l = UINT_MAX)
	{ return EncodeQuotedPrintableCustom('=', '_', NULL, s, l); }

inline void DecodeQuotedPrintable(TArray<BYTE, true>& RData, LPCSTR s, size_t l = UINT_MAX)
	{ DecodeQuotedPrintableCustom('=', '_', NULL, RData, s, l); }

inline KString DecodeQuotedPrintable(LPCSTR s, size_t l = UINT_MAX)
	{ return DecodeQuotedPrintableCustom('=', '_', NULL, s, l); }

// CGI
inline KString EncodeCGI_Parameter(LPCTSTR s, size_t l = UINT_MAX)
	{ return EncodeQuotedPrintableCustom('%', '+', "?=&'\"", s, l); }

inline KString DecodeCGI_Parameter(LPCTSTR s, size_t l = UINT_MAX)
{
	KString Text = DecodeQuotedPrintableCustom('%', '+', "?=&'\"", s, l);

	LPTSTR p = Text.GetDataPtr();

	for( ; *p ; p++)
	{
		if(*p == (TCHAR)0xA0)
			*p = TEXT(' ');
	}

	return Text;
}

// MIME
KString EncodeMIME(LPCTSTR pEncoding, const BYTE* p, size_t l);

KString EncodeMIME(LPCTSTR pEncoding, LPCSTR s, size_t l = UINT_MAX);

KString DecodeMIME(LPCSTR s, size_t l = UINT_MAX);

// Quoted
KString EncodeQuoted(LPCTSTR s, TCHAR qc, bool bWeak = false);

KString DecodeQuoted(LPCTSTR s, TCHAR qc);

KString EncodeQuotedComma(const TArray<KString>& Values, TCHAR qc, bool bWeak = false, TCHAR cSeparator = TEXT(','));

TArray<KString>& DecodeQuotedComma(LPCTSTR s, TCHAR qc, TArray<KString>& RValues, TCHAR cSeperator = TEXT(','));

// CSV
inline KString EncodeCSV(LPCTSTR s)
	{ return EncodeQuoted(s, TEXT('"'), true); }

inline KString DecodeCSV(LPCTSTR s)
	{ return DecodeQuoted(s, TEXT('"')); }

inline KString EncodeCSV(const TArray<KString>& Values, TCHAR cSeparator = TEXT(','))
	{ return EncodeQuotedComma(Values, TEXT('"'), true, cSeparator); }

inline TArray<KString>& DecodeCSV(LPCTSTR s, TArray<KString>& RValues, TCHAR cSeparator = TEXT(';'))
	{ return DecodeQuotedComma(s, TEXT('"'), RValues, cSeparator); }

// MySQL
inline KString EncodeMySQL(LPCTSTR s)
	{ return EncodeQuoted(s, TEXT('\'')); }

inline KString DecodeMySQL(LPCTSTR s)
	{ return DecodeQuoted(s, TEXT('\'')); }

inline KString EncodeMySQL(const TArray<KString>& Values)
	{ return EncodeQuotedComma(Values, TEXT('\'')); }

inline TArray<KString>& DecodeMySQL(LPCTSTR s, TArray<KString>& RValues)
	{ return DecodeQuotedComma(s, TEXT('\''), RValues); }

// JS
KString EncodeJS_String(LPCTSTR s);

KString DecodeJS_String(LPCTSTR s);
*/
#endif // encoding_h
