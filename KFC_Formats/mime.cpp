#include "kfc_formats_pch.h"
#include "mime.h"

#include "encoding.h"

/*
// ---------------
// E-mail address
// ---------------
void TEMailAddress::Parse(LPCTSTR s)
{
	const KString Text = DecodeMIME(s);

	size_t i, j;

	if(	((i = Text.Find('<'))      != UINT_MAX) &&
		((j = Text.Find('>', i+1)) != UINT_MAX))
	{
		m_Address = Text.Mid(i+1, j-i-1).Trim();

		(m_Name = Text.Left(i) + ' ' + Text.Mid(j+1)).TrimSingleSpace();
	}
	else
	{
		(m_Address = Text).Trim();

		m_Name.Empty();
	}
}

// -----------------
// E-mail addresses
// -----------------
TEMailAddresses::TIterator TEMailAddresses::Parse(LPCTSTR s, bool bClearFirst)
{
	if(bClearFirst)
		Clear();

	const TIterator Ret = GetLast();

	KStrings Tokens(s, TEXT(",;"));

	for(KStrings::TConstIterator Iter = Tokens.GetFirst() ; Iter.IsValid() ; ++Iter)
		AddLast()->Parse(*Iter);

	return Ret.IsValid() ? Ret.GetNext() : GetFirst();
}

// -------------
// MIME headers
// -------------
void TMIMEHeaders::Parse(bool bDotted, LPCTSTR pBody, size_t szLength)
{
	if(szLength == UINT_MAX)
		szLength = strlen(pBody);

	Clear();

	LPCTSTR s = pBody;

	KString Line;

	while(s - pBody < (int)szLength)
	{
		if(bDotted && *s == '.')
		{
			s++;

			if(s - pBody == (int)szLength)
				break;
		}

		if(*s != ' ' && *s != '\t') // new line
		{
			if(!Line.IsEmpty())
			{
				const size_t i = Line.Find(':');

				if(i != UINT_MAX)
				{
					AddLast()->Set(	Line.Left(i).Trimmed(),
									DecodeMIME(Line.Mid(i+1).Trimmed()));
				}

				Line.Empty();
			}
		}
		else // spaced line
		{
			Line += ' ';

			for(s++ ; s - pBody < (int)szLength && isspace(*s) ; s++);
		}

		for( ; s - pBody < (int)szLength ; s++)
		{
			if((s+1) - pBody < (int)szLength && s[0] == '\r' && s[1] == '\n')
			{
				s += 2;
				break;
			}

			Line += *s;
		}
	}
}

KString TMIMEHeaders::GetValue(LPCTSTR pName, LPCTSTR pDefaultValue) const
{
	for(TConstIterator Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
	{
		if(!Iter->m_Name.CompareNoCase(pName))
			return Iter->m_Value;
	}

	return pDefaultValue;
}

TEMailAddresses::TIterator
	TMIMEHeaders::ExtractFromAddresses(	TEMailAddresses&	RAddresses,
										bool				bClearFirst) const
{
	return	RAddresses.Parse(GetValue("From"),		bClearFirst),
			RAddresses.Parse(GetValue("Reply-to"),	false);
}

TEMailAddresses::TIterator
	TMIMEHeaders::ExtractToAddresses(	TEMailAddresses&	RAddresses,
										bool				bClearFirst) const
{
	return	RAddresses.Parse(GetValue("To"),	bClearFirst),
			RAddresses.Parse(GetValue("CC"),	false),
			RAddresses.Parse(GetValue("BCC"),	false);
}

// ----------------
// Global routines
// ----------------
KString PrepareMailBody(KString RawBody)
{
	KStrings Lines;
	Lines.SplitString(RawBody, TEXT("\n"), SSEM_INNER);

	KStrings::TIterator Iter;

	RawBody.Empty();

	for(Iter = Lines.GetFirst() ; Iter.IsValid() ; ++Iter)
	{
		if(!Iter->IsEmpty() && (*Iter)[Iter->GetLength() - 1] == TEXT('\r'))
			Iter->SetLeft(Iter->GetLength() - 1);

		if((*Iter)[0] == TEXT('.'))
			RawBody += TEXT('.');

		RawBody += *Iter;

		RawBody += TEXT("\r\n");
	}

	RawBody += TEXT('.');

	return RawBody;
}
*/
