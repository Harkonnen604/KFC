#ifndef file_names_h
#define file_names_h

#ifdef _MSC_VER
    #define DEFAULT_SLASH_CHAR      TEXT('\\')
    #define DEFAULT_SLASH_STRING    TEXT("\\")
    #define ALL_FILES_MASK          TEXT("*.*")
#else // _MSC_VER
    #define DEFAULT_SLASH_CHAR      TEXT('/')
    #define DEFAULT_SLASH_STRING    TEXT("/")
    #define ALL_FILES_MASK          TEXT("*")
#endif // _MSC_VER

#include "kstring.h"

// ---------------
// File path type
// ---------------
enum TFilePathType
{
    FPT_RELATIVE = 0,
    FPT_ROOT,
    FPT_DRIVE,
    FPT_NETWORK
};

// ----------------
// Global routines
// ----------------

#ifdef _MSC_VER

inline int CompareFileCase(LPCTSTR s1, LPCTSTR s2)
{
    return CompareNoCase(s1, s2);
}

#else // _MSC_VER

inline int CompareFileCase(LPCTSTR s1, LPCTSTR s2)
{
    return Compare(s1, s2);
}

#endif // _MSC_VER

inline bool IsSlash(TCHAR a)
{
    return a == TEXT('\\') || a == TEXT('/');
}

inline bool IsRootPath(LPCTSTR s)
{
    return IsSlash(s[0]) && !IsSlash(s[1]);
}

TFilePathType GetFilePathType(LPCTSTR s);

inline bool IsAbsolutePath(LPCTSTR s)
{
    return GetFilePathType(s);
}

KString SlashedFolderName   (const KString& FolderName);
KString UnslashedFolderName (const KString& FolderName);

KString& SlashFolderName    (KString& FolderName);
KString& UnslashFolderName  (KString& FolderName);

KStrings& SlashFolderNames  (KStrings& FolderNames);
KStrings& UnslashFolderNames(KStrings& FolderNames);

KString GetFilePath         (const KString& FileName);
KString GetFileName         (const KString& FileName);
KString GetFileNameName     (const KString& FileName);
KString GetFileExtension    (const KString& FileName);
KString Get83FileName       (const KString& FileName);
KString GetLastFolderName   (const KString& Path);

bool IsValidFileName(const KString& s);

KString& FixSlashes(KString& s, TCHAR cSlash);

inline KStrings& FixSlashes(KStrings& s, TCHAR cSlash)
{
    for(KStrings::TIterator Iter = s.GetFirst() ; Iter.IsValid() ; ++Iter)
        FixSlashes(*Iter, cSlash);

    return s;
}

KStrings::TIterator ParseFoldersSequence(   KString     Path,
                                            KStrings&   RFolders,
                                            bool        bFullPaths,
                                            bool        bClearFirst = true);

KString NormalizePath(LPCTSTR pPath);

KString FollowPath(LPCTSTR pPath, LPCTSTR pRoute);

KString GetRelativePath(LPCTSTR pSrc, LPCTSTR pDst);

#endif // file_names_h
