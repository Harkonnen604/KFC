#include "kfc_ktl_pch.h"
#include "search.h"

#include "pow2.h"
#include "file_names.h"

// ----------------
// Goobal routines
// ----------------
bool DoesMaskMatch(LPCTSTR s, LPCTSTR m, bool bCaseSensitive)
{
	const size_t ml = _tcslen(m);

	bool* f[2];

	f[0] = new bool[ml+1];
	f[1] = new bool[ml+1];

	memset(f[0], 0, (ml+1) * sizeof(bool));
	memset(f[1], 0, (ml+1) * sizeof(bool));

	const uintptr_t dif = (uintptr_t)f[0] ^ (uintptr_t)f[1];

	bool* cf = f[0];
	bool* nf = f[1];

	*cf = true;

	if(bCaseSensitive)
	{
		for(LPCTSTR cs = s ; *cs ; cs++)
		{
			for(LPCTSTR cm = m ; *cm ; cm++, cf++, nf++)
			{
				if(!*cf)
					continue;

				if(*cm == TEXT('*'))
					cf[1] = true, nf[0] = true;
				else if(*cs && (*cm == TEXT('?') || *cs == *cm))
					nf[1] = true;

				*cf = false;
			}

			*cf = false;

			cf -= ml, cf = (bool*)((uintptr_t)cf ^ dif);
			nf -= ml, nf = (bool*)((uintptr_t)nf ^ dif);
		}
	}
	else
	{
		for(LPCTSTR cs = s ; *cs ; cs++)
		{
			for(LPCTSTR cm = m ; *cm ; cm++, cf++, nf++)
			{
				if(!*cf)
					continue;

				if(*cm == TEXT('*'))
					cf[1] = true, nf[0] = true;
				else if(*cs && (*cm == TEXT('?') || _totlower(*cs) == _totlower(*cm)))
					nf[1] = true;

				*cf = false;
			}

			*cf = false;

			cf -= ml, cf = (bool*)((uintptr_t)cf ^ dif);
			nf -= ml, nf = (bool*)((uintptr_t)nf ^ dif);
		}
	}

	for(LPCTSTR cm = m ; *cm ; cf++, cm++)
	{
		if(*cf && *cm == TEXT('*'))
			cf[1] = true;
	}

	const bool ret = *cf;

	delete[] f[1];
	delete[] f[0];

	return ret;
}

bool DoesFileMaskMatch(LPCTSTR pFileName, LPCTSTR pMask, bool bCaseSensitive)
{
	#ifdef _MSC_VER
	{
		return	DoesMaskMatch(GetFileNameName (pFileName), GetFileNameName (pMask), bCaseSensitive) &&
				DoesMaskMatch(GetFileExtension(pFileName), GetFileExtension(pMask), bCaseSensitive);
	}
	#else // _MSC_VER
	{
		return DoesMaskMatch(pFileName, pMask, bCaseSensitive);
	}
	#endif // _MSC_VER
}

bool DoesPathMaskMatch(LPCTSTR pFileName, LPCTSTR pMask, bool bCaseSensitive)
{
	if(!DoesFileMaskMatch(GetFileName(pFileName), GetFileName(pMask), bCaseSensitive))
		return false;

	KStrings FN_Folders;
	ParseFoldersSequence(GetFilePath(pFileName), FN_Folders, false);

	KStrings MaskFolders;
	ParseFoldersSequence(GetFilePath(pMask), MaskFolders, false);

	while(!MaskFolders.IsEmpty() && UnslashedFolderName(MaskFolders[0]) == TEXT("**"))
	{
		MaskFolders.DelFirst();

		while(FN_Folders.GetN() > MaskFolders.GetN())
			FN_Folders.DelFirst();
	}

	if(FN_Folders.GetN() != MaskFolders.GetN())
		return false;

	KStrings::TIterator FN_Iter, MaskIter;

	for(FN_Iter = FN_Folders.GetFirst(), MaskIter = MaskFolders.GetFirst() ;
		FN_Iter.IsValid() && MaskIter.IsValid() ;
		++FN_Iter, ++MaskIter)
	{
		if(!DoesFileMaskMatch(	UnslashedFolderName(*FN_Iter),
								UnslashedFolderName(*MaskIter),
								bCaseSensitive))
		{
			return false;
		}
	}

	return true;
}
