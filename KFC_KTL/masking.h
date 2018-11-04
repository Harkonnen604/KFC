#ifndef search_h
#define search_h

// ----------------
// Global routines
// ----------------
bool DoesMaskMatch(LPCTSTR s, LPCSTR m, bool bCaseSensitive = true);

inline bool DoesMaskMatch(LPCTSTR s, const KStrings& m, bool bCaseSensitive = true)
{
	for(KStrings::TConstIterator Iter = m.GetFirst();  Iter.IsValid() ; ++Iter)
	{
		if(DoesMaskMatch(s, *Iter, bCaseSensitive))
			return true;
	}

	return false;
}

#ifdef _MSC_VER
	#define DEFAULT_FMASK_CS	(false)
#else // _MSC_VER
	#define DEFAULT_FMASK_CS	(true)
#endif // _MSC_VER

bool DoesFileMaskMatch(LPCTSTR pFileName, LPCTSTR pMask, bool bCaseSensitive = DEFAULT_FMASK_CS);

inline bool DoesFileMaskMatch(LPCTSTR s, const KStrings& m, bool bCaseSensitive = DEFAULT_FMASK_CS)
{
	for(KStrings::TConstIterator Iter = m.GetFirst() ; Iter.IsValid() ; ++Iter)
	{
		if(DoesFileMaskMatch(s, *Iter, bCaseSensitive))
			return true;
	}

	return false;
}

bool DoesPathMaskMatch(LPCTSTR pFileNAme, LPCTSTR pMask, bool bCaseSensitive = DEFAULT_FMASK_CS);

inline bool DoesPathMaskMatch(LPCTSTR s, const KStrings& m, bool bCaseSensitive = DEFAULT_FMASK_CS)
{
	for(KStrings::TConstIterator Iter = m.GetFirst() ; Iter.IsValid() ; ++Iter)
	{
		if(DoesPathMaskMatch(s, *Iter, bCaseSensitive))
			return true;
	}

	return false;
}

#endif // search_h
