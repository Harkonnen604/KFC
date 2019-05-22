#include "kfc_ktl_pch.h"
#include "file_names.h"

// ----------------
// Global routines
// ----------------
TFilePathType GetFilePathType(LPCTSTR s)
{
	return	IsSlash(s[0]) ?
				(IsSlash(s[1]) ? FPT_NETWORK : FPT_ROOT) :
				(_istalpha(s[0]) && s[1] == TEXT(':') && IsSlash(s[2]) ?
					FPT_DRIVE : FPT_RELATIVE);
}

KString SlashedFolderName(const KString& FolderName)
{
	size_t szLength = FolderName.GetLength();
	return szLength==0 || IsSlash(FolderName[szLength-1]) ? FolderName : (FolderName + DEFAULT_SLASH_CHAR);
}

KString UnslashedFolderName(const KString& FolderName)
{
	size_t szLength = FolderName.GetLength();
	return szLength && IsSlash(FolderName[szLength-1]) ? FolderName.Left(szLength-1) : FolderName;
}

KString& SlashFolderName(KString& FolderName)
{
	if(!FolderName.IsEmpty() && !IsSlash(FolderName[FolderName.GetLength() - 1]))
		FolderName += DEFAULT_SLASH_CHAR;

	return FolderName;
}

KString& UnslashFolderName(KString& FolderName)
{
	if(!FolderName.IsEmpty() && IsSlash(FolderName[FolderName.GetLength() - 1]))
		FolderName.SetLeft(FolderName.GetLength() - 1);

	return FolderName;
}

KStrings& SlashFolderNames(KStrings& FolderNames)
{
	for(KStrings::TIterator Iter = FolderNames.GetFirst() ; Iter.IsValid() ; ++Iter)
		SlashFolderName(*Iter);

	return FolderNames;
}

KStrings& UnslashFolderNames(KStrings& FolderNames)
{
	for(KStrings::TIterator Iter = FolderNames.GetFirst() ; Iter.IsValid() ; ++Iter)
		UnslashFolderName(*Iter);

	return FolderNames;
}

KString GetFilePath(const KString& FileName)
{
	if(FileName == TEXT(".") || FileName == TEXT(".."))
		return FileName;

	size_t i;
	for(i=FileName.GetLength()-1 ; i!=UINT_MAX && !IsSlash(FileName[i]) && (i!=1 || FileName[i]!=TEXT(':')) ; i--);

	return i==UINT_MAX ? KString(TEXT("")) : FileName.Left(i+1);
}

KString GetFileName(const KString& FileName)
{
	if(FileName == TEXT(".") || FileName == TEXT(".."))
		return TEXT("");

	size_t szLength = FileName.GetLength();

	size_t i;
	for(i=szLength-1 ; i!=UINT_MAX && !IsSlash(FileName[i]) ; i--);

	return i==UINT_MAX ? FileName : FileName.Right(szLength-i-1);
}

KString GetFileNameName(const KString& FileName)
{
	if(FileName == TEXT(".") || FileName == TEXT(".."))
		return TEXT("");

	KString str = GetFileName(FileName);

	size_t i;
	for(i=str.GetLength()-1 ; i!=UINT_MAX && str[i]!=TEXT('.') ; i--);

	return i==UINT_MAX ? str : str.Left(i);
}

KString GetFileExtension(const KString& FileName)
{
	if(FileName == TEXT(".") || FileName == TEXT(".."))
		return TEXT("");

	KString str = GetFileName(FileName);

	size_t szLength = str.GetLength();

	size_t i;
	for(i=szLength-1 ; i!=UINT_MAX && str[i]!=TEXT('.') ; i--);

	return i==UINT_MAX ? KString(TEXT("")) : str.Right(szLength - i - 1);
}

KString GetLastFolderName(const KString& Path)
{
	return SlashedFolderName(GetFileName(UnslashedFolderName(Path)));
}

KString Get83FileName(const KString& FileName)
{
	return GetFileNameName(FileName).Left(8) + TEXT('.') + GetFileExtension(FileName).Left(3);
}

bool IsValidFileName(const KString& s)
{
	size_t szLength = s.GetLength();
	if(szLength == 0)
		return false;

	for(size_t i=0 ; i<szLength ; i++)
	{
		switch(s[i])
		{
		case TEXT('\\'):
		case TEXT('/'):
		case TEXT(':'):
		case TEXT('*'):
		case TEXT('?'):
		case TEXT('\"'):
		case TEXT('<'):
		case TEXT('>'):
		case TEXT('|'):
			return false;
		}
	}

	return true;
}

KString& FixSlashes(KString& s, TCHAR cSlash)
{
	DEBUG_VERIFY(IsSlash(cSlash));

	LPTSTR p = s.GetDataPtr();

	for( ; *p ; p++)
	{
		if(IsSlash(*p))
			*p = cSlash;
	}

	return s;
}

KStrings::TIterator ParseFoldersSequence(	KString		Path,
											KStrings&	RFolders,
											bool		bFullPaths,
											bool		bClearFirst)
{
	if(bClearFirst)
		RFolders.Clear();

	const KStrings::TIterator Ret = RFolders.GetLast();

	if(Path.IsEmpty())
		return Ret;

	FixSlashes(Path = SlashedFolderName(Path), DEFAULT_SLASH_CHAR);

	size_t szPos = 0;

	for(size_t i = 0 ; i < Path.GetLength() ; i++)
	{
		if(IsSlash(Path[i]))
		{
			RFolders << Path.Mid(szPos, i + 1 - szPos);

			if(!bFullPaths)
				szPos = i + 1;
		}
	}

	if(GetFilePathType(Path) == FPT_NETWORK)
	{
		KStrings::TIterator Iter =
			Ret.IsValid() ? Ret.GetNext() : RFolders.GetFirst();

		DEBUG_VERIFY(Iter.IsValid());

		for(size_t i = 3 ; i && Iter.GetNext().IsValid() ; i--)
		{
			if(bFullPaths)
				*Iter = *Iter.GetNext();
			else
				*Iter += *Iter.GetNext();

			RFolders.Del(Iter.GetNext());
		}
	}

	return Ret.IsValid() ? Ret.GetNext() : RFolders.GetFirst();
}

KString NormalizePath(LPCTSTR pPath)
{
	if(!*pPath)
		return "";

	KStrings Folders;
	ParseFoldersSequence(pPath, Folders, false);

	KString RootFolder = "";

	if(IsAbsolutePath(pPath))
		RootFolder = *Folders.GetFirst(), Folders.DelFirst();

	FOR_EACH_LIST(Folders, KStrings::TIterator, Iter)
	{
		if(*Iter == "." DEFAULT_SLASH_STRING)
		{
			Folders.Del(Iter);
		}
		else if(*Iter == ".." DEFAULT_SLASH_STRING &&
				Iter.GetPrev().IsValid() &&
				*Iter.GetPrev() != ".." DEFAULT_SLASH_STRING)
		{
			Folders.Del(Iter.GetPrev()), Folders.Del(Iter);
		}
	}

	return RootFolder + Folders.GenerateString(TEXT(""));
}

KString FollowPath(LPCTSTR pPath, LPCTSTR pRoute)
{
	return NormalizePath(	IsAbsolutePath(pRoute) ?
								KString(pRoute) :
								SlashedFolderName(pPath) + pRoute);
}

KString GetRelativePath(LPCTSTR pSrc, LPCTSTR pDst)
{
	bool bSrcAbs = IsAbsolutePath(pSrc);
	bool bDstAbs = IsAbsolutePath(pDst);

	if(bSrcAbs != bDstAbs)
		return NormalizePath(pDst);

	KStrings SrcFolders;
	ParseFoldersSequence(NormalizePath(pSrc), SrcFolders, false);

	KStrings DstFolders;
	ParseFoldersSequence(NormalizePath(pDst), DstFolders, false);

	if(bSrcAbs && CompareFileCase(*SrcFolders.GetFirst(), *DstFolders.GetFirst()))
		return NormalizePath(pDst);

	KStrings::TConstIterator SrcIter;
	KStrings::TConstIterator DstIter;

	for(SrcIter = SrcFolders.GetFirst(), DstIter = DstFolders.GetFirst() ;
		SrcIter.IsValid() && DstIter.IsValid() &&
			!CompareFileCase(*SrcIter, *DstIter) ;
		++SrcIter, ++DstIter);

	KString Route;

	for( ; SrcIter.IsValid() ; ++SrcIter)
		Route += ".." DEFAULT_SLASH_STRING;

	for( ; DstIter.IsValid() ; ++DstIter)
		Route += *DstIter;

	return Route;
}
