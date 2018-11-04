#ifndef http_h
#define http_h

#include <KFC_KTL/variable_list.h>
#include <KFC_Formats/url.h>

// ------------
// HTTP header
// ------------
typedef TVariable T_HTTP_Header;

// -------------
// HTTP headers
// -------------
class T_HTTP_Headers : public TVariableList
{
public:
	KString m_Prefix;

public:
	T_HTTP_Headers() : TVariableList(false) {}

	T_HTTP_Headers(LPCTSTR s) : TVariableList(false)
		{ Parse(s); }

	void Clear();

	void Parse(LPCTSTR s);

	KString Write() const;

	void ParseRequestPrefix(KString* pRCommand,
							KString* pRResource,
							KString* pRVersion = NULL) const;

	void ParseResponsePrefix(	int*		pRCode,
								KString*	pRCodeText	= NULL,
								KString*	pRVersion	= NULL) const;

	void SetRequestPrefix(	LPCTSTR pCommand,
							LPCTSTR pResource,
							LPCTSTR pVersion = TEXT("HTTP/1.1"));

	void SetResponsePrefix(	int		iCode		= 200,
							LPCTSTR	pCodeText	= TEXT("OK"),
							LPCTSTR	pVersion	= TEXT("HTTP/1.1"));
};

// ------------
// HTTP buffer
// ------------
class T_HTTP_Buffer : public TArray<BYTE, true>
{
public:
	T_HTTP_Buffer() {}

	T_HTTP_Buffer(LPCSTR s)
		{ *this = s; }

	T_HTTP_Buffer(const KString &s)
		{ *this = s; }

	T_HTTP_Buffer& operator = (LPCSTR s)
		{ const size_t l = _tcslen(s); memcpy(&SetN(l), s, l); return *this; }

	T_HTTP_Buffer& operator = (const KString& s)
		{ memcpy(&SetN(s.GetLength()), s.GetDataPtr(), s.GetLength()); return *this; }

	operator KString () const
	{
		KString Text;
		
		Text.Allocate(GetN());
		
		const BYTE* pSrc = GetDataPtr();

		TCHAR* pDst = Text.GetDataPtr();

		for(size_t i = GetN() ; i ; i--, pSrc++, pDst++)
			*pDst = *pSrc ? *pSrc : TEXT(' ');

		return Text;
	}
};

// ----------------
// Global routines
// ----------------
void OpenURL(	const T_URL&			URL,
				const T_HTTP_Buffer&	Request,
				T_HTTP_Buffer&			RResponse,
				T_HTTP_Headers*			pRHeaders		= NULL,
				bool					bHeadersOnly	= false,
				HANDLE					hTerminator		= NULL,
				size_t					szTimeout		= UINT_MAX);

inline KString OpenURL(	const T_URL&	URL,
						T_HTTP_Headers*	pRHeaders	= NULL,
						HANDLE			hTerminator	= NULL,
						size_t			szTimeout	= UINT_MAX)
{
	T_HTTP_Buffer Data;

	return OpenURL(URL, T_HTTP_Buffer(), Data, pRHeaders, false, hTerminator, szTimeout), Data;
}

inline KString OpenURL(	const T_URL&			URL,
						const T_HTTP_Buffer&	Request,
						T_HTTP_Headers*			pRHeaders	= NULL,
						HANDLE					hTerminator	= NULL,
						size_t					szTimeout	= UINT_MAX)
{
	T_HTTP_Buffer Data;

	return OpenURL(URL, Request, Data, pRHeaders, false, hTerminator, szTimeout), Data;
}

void ParseContentType(LPCTSTR s, KString& RType, KString* pRCharset = NULL);

#endif // http_h
