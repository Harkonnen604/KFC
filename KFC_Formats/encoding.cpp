#include "kfc_formats_pch.h"
#include "encoding.h"

#include <KFC_KTL/hex.h>
#include "encoding_globals.h"

// --------
// Statics
// --------
static const TCHAR s_Base64Alphabet[64+1] =
	TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

static BYTE s_Base64Map[0x100];

// ------------
// Initializer
// ------------
struct TInitializer
{
	TInitializer();
};

static TInitializer s_Initializer;

TInitializer::TInitializer()
{
	BYTE i;

	// Base64 map
	{
		memset(s_Base64Map, 0xFF, sizeof(s_Base64Map));

		for(i = 0 ; s_Base64Alphabet[i] ; i++)
			s_Base64Map[(BYTE)s_Base64Alphabet[i]] = i;
	}
}

/*
// ---------
// Encoding
// ---------
TEncoding::TEncoding()
{
}

TEncoding::TEncoding(LPCTSTR pSName, LPCTSTR pFileNamePrefix)
{
	Allocate(pSName, pFileNamePrefix);
}

void TEncoding::Release()
{
	m_DecData.Clear();
	m_EncData.Clear();

	m_Name.Empty();
}

void TEncoding::AllocateIdentity(LPCTSTR pSName)
{
	Release();

	DEBUG_VERIFY(pSName && pSName[0]);

	m_Name = pSName;

	m_EncData.SetN(0x100);
	m_DecData.SetN(0x100);

	BYTE* p1 = m_EncData.GetDataPtr();
	BYTE* p2 = m_DecData.GetDataPtr();

	size_t i;

	for(i = 0 ; i < 0x100 ; i++, p1++, p2++)
		*p1 = i, *p2 = i;
}

void TEncoding::Allocate(LPCTSTR pSName, LPCTSTR pFileNamePrefix)
{
	Release();

	try
	{
		DEBUG_VERIFY(pSName && pSName[0]);

		if(!pFileNamePrefix)
			pFileNamePrefix = pSName;

		m_Name = pSName;

		KString FileName;

		FILE* pFile;

		// Enc
		{
			if(!(pFile = _tfopen(FileName = (KString)pFileNamePrefix + TEXT(".enc"), TEXT("rb"))))
				INITIATE_DEFINED_FAILURE((KString)TEXT("Error opening encoding file \"") + FileName + TEXT("\"."));

			if(fread(&m_EncData.SetN(0x100), 1, 0x100, pFile) != 0x100)
			{
				fclose(pFile);
				INITIATE_DEFINED_FAILURE((KString)TEXT("Erorr reading encoding file \"") + FileName + TEXT("\"."));
			}

			fclose(pFile);
		}

		// Dec
		{
			if(!(pFile = _tfopen(FileName = (KString)pFileNamePrefix + TEXT(".dec"), TEXT("rb"))))
				INITIATE_DEFINED_FAILURE((KString)TEXT("Error opening encoding file \"") + FileName + TEXT("\"."));

			if(fread(&m_DecData.SetN(0x100), 1, 0x100, pFile) != 0x100)
			{
				fclose(pFile);
				INITIATE_DEFINED_FAILURE((KString)TEXT("Erorr reading encoding file \"") + FileName + TEXT("\"."));
			}

			fclose(pFile);
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TEncoding::AllocateOEM(LPCTSTR pSName)
{
	Release();

	try
	{
		DEBUG_VERIFY(pSName && pSName[0]);

		m_Name = pSName;

		m_EncData.SetN(0x100);
		m_DecData.SetN(0x100);

		size_t i;

		BYTE* p1 = m_EncData.GetDataPtr();
		BYTE* p2 = m_DecData.GetDataPtr();

		for(i = 0 ; i < 0x100 ; i++)
			*p1++ = *p2++ = (BYTE)i;

		#ifdef _MSC_VER
		{
			CharToOemBuff((LPCSTR)m_EncData.GetDataPtr(), (LPSTR)m_EncData.GetDataPtr(), 0x100);
			OemToCharBuff((LPCSTR)m_DecData.GetDataPtr(), (LPSTR)m_DecData.GetDataPtr(), 0x100);
		}
		#endif // _MSC_VER
	}

	catch(...)
	{
		Release();
		throw;
	}
}

KString TEncoding::Encode(LPCSTR s, size_t l) const
{
	if(l == UINT_MAX)
		l = strlen(s);

	if(!this)
		return KString(TDefaultString(s), l);

	DEBUG_VERIFY_ALLOCATION;

	if(l == UINT_MAX)
		l = strlen(s);

	KString Text;

	LPSTR p = Text.Allocate(l);

	for( ; l ; l--)
		*p++ = (char)m_EncData[(BYTE)*s++];

	return Text;
}

KString TEncoding::Decode(LPCSTR s, size_t l) const
{
	if(l == UINT_MAX)
		l = strlen(s);

	if(!this)
		return KString(s, l);

	DEBUG_VERIFY_ALLOCATION;

	KString Text;

	LPTSTR p = Text.Allocate(l);

	for( ; l ; l--)
		*p++ = (char)m_DecData[(BYTE)*s++];

	return Text;
}

LPSTR TEncoding::EncodeSelf(LPSTR s, size_t l) const
{
	if(!this)
		return s;

	DEBUG_VERIFY_ALLOCATION;

	if(l == UINT_MAX)
		l = strlen(s);

	LPSTR p;

	for(p = s ; l ; l--, p++)
		*p = (char)m_EncData[(BYTE)*p];

	return s;
}

LPSTR TEncoding::DecodeSelf(LPSTR s, size_t l) const
{
	if(!this)
		return s;

	DEBUG_VERIFY_ALLOCATION;

	if(l == UINT_MAX)
		l = strlen(s);

	LPTSTR p;

	for(p = s ; l ; l--, p++)
		*p = (char)m_DecData[(BYTE)*p];

	return s;
}
*/

/*
// ----------------
// Global routines
// ----------------

// Encoding
KString Encode(LPCTSTR pName, LPCSTR s, size_t l)
{
	return g_EncodingGlobals.GetEncoding(pName).Encode(s, l);
}

KString Decode(LPCTSTR pName, LPCSTR s, size_t l)
{
	return g_EncodingGlobals.GetEncoding(pName).Decode(s, l);
}

KString EncodeSafe(LPCTSTR pName, LPCSTR s, size_t l)
{
	return g_EncodingGlobals.FindEncoding(pName)->Encode(s, l);
}

KString DecodeSafe(LPCTSTR pName, LPCSTR s, size_t l)
{
	return g_EncodingGlobals.FindEncoding(pName)->Decode(s, l);
}

LPTSTR EncodeSelf(LPCTSTR pName, LPSTR s, size_t l)
{
	return g_EncodingGlobals.GetEncoding(pName).EncodeSelf(s, l);
}

LPTSTR DecodeSelf(LPCTSTR pName, LPSTR s, size_t l)
{
	return g_EncodingGlobals.GetEncoding(pName).DecodeSelf(s, l);
}

LPTSTR EncodeSelfSafe(LPCTSTR pName, LPSTR s, size_t l)
{
	return g_EncodingGlobals.FindEncoding(pName)->EncodeSelf(s, l);
}

LPTSTR DecodeSelfSafe(LPCTSTR pName, LPSTR s, size_t l)
{
	return g_EncodingGlobals.FindEncoding(pName)->DecodeSelf(s, l);
}
*/

// Base64
KString EncodeBase64(const BYTE* p, size_t l)
{
	if(!l)
		return "";

	KString Text;

	size_t i;

	DWORD v = 0;

	for(i = 0 ; ; l--, p++)
	{
		v <<= 8, v |= (int)l > 0 ? *p : 0;

		if(++i == 3)
		{
			Text += s_Base64Alphabet[(v >> 18)];
			Text += s_Base64Alphabet[(v >> 12) & 0x3F];
			Text += s_Base64Alphabet[(v >> 6)  & 0x3F];
			Text += s_Base64Alphabet[(v)       & 0x3F];

			if(l == 1)
				break;

			if(l == 0)
			{
				Text.SetChar(Text.GetLength() - 1, TEXT('='));
				break;
			}

			if((int)l == -1)
			{
				Text.SetChar(Text.GetLength() - 2, TEXT('='));
				Text.SetChar(Text.GetLength() - 1, TEXT('='));
				break;
			}

			v = 0, i = 0;
		}
	}

	return Text;
}

void DecodeBase64(TArray<BYTE, true>& RData, LPCSTR s, size_t l)
{
	if(l == UINT_MAX)
		l = strlen(s);

	RData.Clear();

	if(l == 0)
		return;

	size_t i;

	DWORD v = 0;

	size_t ne = 0;

	for(i = 0 ; ; s++, l--)
	{
		char c = (int)l > 0 ? *s : '=';

		if(c == '=')
		{
			ne++, c = 0;
		}
		else
		{
			c = s_Base64Map[(BYTE)c];

			if((BYTE)c == 0xFF)
				continue;
		}

		v <<= 6, v |= c;

		if(++i == 4)
		{
			if(ne <= 2)
				RData.Add() = (BYTE)(v >> 16);

			if(ne <= 1)
				RData.Add() = (BYTE)((v >> 8) & 0xFF);

			if(ne <= 0)
				RData.Add() = (BYTE)(v & 0xFF);

			if(ne || l == 1)
				break;

			v = 0, i = 0;
		}
	}
}

/*
// Quoted-printable custom
KString EncodeQuotedPrintableCustom(char		cPrefix,
									char		cSpace,
									LPCSTR		pDelimeters,
									const BYTE*	p,
									size_t		l)
{
	KString Text;

	for( ; l ; l--, p++)
	{
		if(*p == ' ')
		{
			Text += cSpace;
		}
		else if(*p >= 33 &&*p <= 126 &&
				*p != cPrefix && *p != cSpace &&
				(!pDelimeters || !strchr(pDelimeters, *p)))
		{
			Text += (char)*p;
		}
		else
		{
			Text += cPrefix, Text += HexToChar(*p >> 4), Text += HexToChar(*p & 0xF);
		}
	}

	return Text;
}

KString EncodeQuotedPrintableCustom(char	cPrefix,
									char	cSpace,
									LPCSTR	pDelimeters,
									LPCSTR	s,
									size_t	l)
{
	if(l == UINT_MAX)
		l = strlen(s);

	return EncodeQuotedPrintableCustom(cPrefix, cSpace, pDelimeters, (const BYTE*)s, l);
}

void DecodeQuotedPrintableCustom(	char				cPrefix,
									char				cSpace,
									LPCSTR				pDelimeters,
									TArray<BYTE, true>&	RData,
									LPCSTR				s,
									size_t				l)
{
	if(l == UINT_MAX)
		l = strlen(s);

	RData.Clear();

	while(l)
	{
		if(*s == cSpace)
			RData.Add() = ' ', s++, l--;
		else if(*s == cPrefix && l >= 3)
			RData.Add() = (CharToHex(s[1]) << 4) | CharToHex(s[2]), s += 3, l -= 3;
		else
			RData.Add() = *s, s++, l--;
	}
}

KString DecodeQuotedPrintableCustom(char				cPrefix,
									char				cSpace,
									LPCSTR				pDelimeters,
									LPCSTR				s,
									size_t				l)
{
	TArray<BYTE, true> Data;

	DecodeQuotedPrintableCustom(cPrefix, cSpace, pDelimeters, Data, s, l);

	return KString((LPCSTR)Data.GetDataPtr(), Data.GetN());
}

// MIME
KString EncodeMIME(LPCTSTR pEncoding, const BYTE* p, size_t l)
{
	if(!l)
		return TEXT("");

	size_t i;

	if(!isspace(p[0]) && !isspace(p[l-1]))
	{
		for(i = 0 ; i < l ; i++)
		{
			if(p[i] < 32 || p[i] > 126)
				break;
		}

		if(i == l)
			return KString((LPCTSTR)p, l);
	}

	size_t qpl = 0, bpl = 0;

	for(i = 0 ; i < l ; i++)
	{
		if(p[i] < 32 || p[i] > 126)
			qpl += 3;
		else
			qpl++;
	}

	bpl = (l + 2) / 3 * 4;

	KString s;

	s = pEncoding ? Encode(pEncoding, (LPCSTR)p, l) : KString((LPCTSTR)p, l);

	if(qpl <= bpl)
		return (KString)"=?" + pEncoding + "?Q?" + EncodeQuotedPrintable(s.GetDataPtr(), s.GetLength()) + "?=";
	else
		return (KString)"=?" + pEncoding + "?B?" + EncodeBase64(s.GetDataPtr(), s.GetLength()) + "?=";
}

KString EncodeMIME(LPCTSTR pEncoding, LPCSTR s, size_t l)
{
	if(l == UINT_MAX)
		l = strlen(s);

	return EncodeMIME(pEncoding, (const BYTE*)s, l);
}

KString DecodeMIME(LPCSTR s, size_t l)
{
	if(l == UINT_MAX)
		l = strlen(s);

	KString Text;

	LPCSTR p = s;

	LPCSTR q1, q2, q3, q4;

	for(;;)
	{
		if((q1 = strstr(p, "=?")) && (q1 - s) < (int)l)
		{
			Text.Extend(p, q1 - p);

			if(	(q2 = strstr(q1+2, "?" )) && q2 - s < (int)l &&
				(q3 = strstr(q2+1, "?" )) && q3 - s < (int)l &&
				(q4 = strstr(q3+1, "?=")) && q4 - s < (int)l)
			{
				KString TextPart;

				if(q3 - q2 == 2)
				{
					if(tolower(q2[1]) == 'q')
						TextPart = DecodeQuotedPrintable(q3+1, (q4-q3)-1);
					else if(tolower(q2[1]) == 'b')
						TextPart = DecodeBase64(q3+1, (q4-q3)-1);
				}

				Text += DecodeSafe(KString(q1+2, q2-q1-2), TextPart);

				p = q4 + 2;
			}
			else // false '=?' detected
			{
				Text += "=?";

				p = q1 + 2;
			}
		}
		else // remainder is plaintext
		{
			Text.Extend(p, l - (p - s));

			break;
		}
	}

	return Text;
}

// Quoted
KString EncodeQuoted(LPCTSTR s, TCHAR qc, bool bWeak)
{
	DEBUG_VERIFY(IsQuote(qc));

	for(size_t i = 0 ; s[i] ; i++)
	{
		if(s[i] == qc || s[i] == ',' || s[i] == ';')
			bWeak = false;
	}

	KString Text;

	if(!bWeak)
		Text += qc;

	for( ; *s ; s++)
	{
		if(*s == qc)
			Text += qc, Text += qc;
		else
			Text += *s;
	}

	if(!bWeak)
		Text += qc;

	return Text;
}

KString DecodeQuoted(LPCTSTR s, TCHAR qc)
{
	DEBUG_VERIFY(IsQuote(qc));

	if(s[0] != qc || s[_tcslen(s) - 1] != qc)
		return s;

	KString Text;

	for(s++ ; s[1] ; )
	{
		if(s[0] == qc && s[1] == qc)
			Text += qc, s += 2;
		else
			Text += *s++;
	}

	return Text;
}

KString EncodeQuotedComma(const TArray<KString>& Values, TCHAR qc, bool bWeak, TCHAR cSeparator)
{
	KString Text;

	for(size_t i = 0 ; i < Values.GetN() ; i++)
	{
		if(i > 0)
			Text += cSeparator;

		Text += EncodeQuoted(Values[i], qc, bWeak);
	}

	return Text;
}

TArray<KString>& DecodeQuotedComma(LPCTSTR s, TCHAR qc, TArray<KString>& RValues, TCHAR cSeparator)
{
	RValues.Clear();

	if(!*s)
		return RValues;

	RValues.Add();

	bool q = false;

	for( ; *s ; s++)
	{
		if(*s == qc)
		{
			if(q)
			{
				if(s[1] == qc)
					RValues.GetLastItem() += qc, s++;
				else
					q = false;
			}
			else
			{
				q = true;
			}
		}
		else if(*s == cSeparator && !q)
		{
			RValues.Add();
		}
		else
		{
			RValues.GetLastItem() += *s;
		}
	}

	return RValues;
}

// JS
KString EncodeJS_String(LPCTSTR s)
{
	KString Text;

	for( ; *s ; s++)
	{
		switch(*s)
		{
		case TEXT('\t'):
			Text += TEXT("\\t");
			break;

		case TEXT('\r'):
			Text += TEXT("\\r");
			break;

		case TEXT('\n'):
			Text += TEXT("\\n");
			break;

		case TEXT('\\'):
			Text += TEXT("\\\\");
			break;

		case TEXT('"'):
			Text += TEXT("\\\"");
			break;

		case TEXT('\''):
			Text += TEXT("\\'");
			break;

		default:
			Text += *s;
		}
	}

	return Text;
}

KString DecodeJS_String(LPCTSTR s)
{
	KString Text;

	for( ; *s ; s++)
	{
		if(*s == TEXT('\\'))
		{
			s++;

			if(!*s)
				break;

			switch(*s)
			{
			case TEXT('t'):
				Text += TEXT('\t');
				break;

			case TEXT('r'):
				Text += TEXT('\r');
				break;

			case TEXT('n'):
				Text += TEXT('\n');
				break;

			default:
				Text += *s;
			}
		}
		else
		{
			Text += *s;
		}
	}

	return Text;
}
*/
