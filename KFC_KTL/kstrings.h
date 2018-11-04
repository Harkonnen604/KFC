#ifndef kstrings_h
#define kstrings_h

#include "list.h"
#include "kstring.h"

// ------------------------
// Split string empty mode
// ------------------------
#define SSEM_LEADING	(0x0001)
#define SSEM_INNER		(0x0002)
#define SSEM_TRAILING	(0x0004)

#define SSEM_NONE		(0)
#define SSEM_ALL		(UINT_MAX)

// --------
// Strings
// --------
class KStrings : public TList<KString>
{
public:
	KStrings() {}

	KStrings(const KStrings& SStrings) : TList<KString>(SStrings) {}

	KStrings(const KString& s, LPCTSTR p, bool bSplitToTokens = true)
		{ if(bSplitToTokens) SplitToTokens(s, p); else SplitString(s, p); }

	KStrings(const TArray<KString>& Strings);

	TIterator SplitString(	const KString&	String,
							LPCTSTR			pSplitter,
							kflags_t		flEmptyMode	= SSEM_ALL,
							bool			bClearFirst	= true);

	TIterator SplitToTokens(LPCTSTR	s,
							LPCTSTR	pDelimiters,
							bool	bClearFirst = true);

	void KillDupes();

	void KillDupesNoCase();

	bool HasString(const KString& String) const;

	bool HasStringNoCase(const KString& String) const;

	bool HasValueString(const KString& String, KString& RValue) const;

	bool HasValueStringNoCase(const KString& String, KString& RValue) const;

	KString GenerateString(	LPCTSTR	pDelimiter,
							bool	bLastDelimiter = false) const;

	TIterator SplitQuotedString(LPCTSTR pString, bool bClearFirst);
	
	void TrimAll();

	KStrings& operator = (const TArray<KString>& Strings);

	KStrings& operator << (const KString& Text)
		{ static_cast<TList<KString>&>(*this) << Text; return *this; }
};

#endif // kstrings_h
