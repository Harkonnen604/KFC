#include "kfc_common_pch.h"
#include "file.h"

#include <KFC_KTL/masking.h>
#include "common_consts.h"

// Stream mode indices
#define _SMI_R      (0)
#define _SMI_W      (1)
#define _SMI_RP     (2)
#define _SMI_WP     (3)

// -----
// File
// -----
TFile::TFile()
{
    m_bAllocated = false;
}

TFile::TFile(LPCTSTR pFileName)
{
    m_bAllocated = false;

    Allocate(pFileName);
}

TFile::TFile(LPCTSTR pFileName, kflags_t flSOpenFlags, kflags_t flBaseSharingFlags)
{
    m_bAllocated = false;

    Allocate(pFileName), Open(flSOpenFlags, flBaseSharingFlags);
}

void TFile::Release()
{
    try { InternalClose(); } catch(...) {}

    if(IsAllocated() && m_bRemoveOnRelease)
        try { InternalRemove(); } catch(...) {}

    m_bAllocated = false;
}

void TFile::Allocate(LPCTSTR pFileName)
{
    Release();

    try
    {
        DEBUG_VERIFY(pFileName && *pFileName);

        m_FileName = pFileName;

        m_szOffset  = 0;
        m_szLength  = 0;

        m_pFile = NULL;

        m_bRemoveOnClose    = false;
        m_bRemoveOnRelease  = false;

        m_bAllocated = true;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void TFile::Allocate(LPCTSTR pFileName, kflags_t flSOpenFlags, kflags_t flBaseSharingFlags)
{
    Release();

    try
    {
        DEBUG_VERIFY(pFileName && *pFileName);

        m_FileName = pFileName;
        m_szLength = 0;

        m_pFile = NULL;

        m_bRemoveOnClose    = false;
        m_bRemoveOnRelease  = false;

        m_bAllocated = true;

        Open(flSOpenFlags, flBaseSharingFlags);
    }

    catch(...)
    {
        Release();
        throw;
    }
}

#ifdef _MSC_VER

void TFile::InternalOpenHandle(kflags_t flBaseSharingFlags)
{
    kflags_t flAccess   = 0;
    kflags_t flSharing  = 0;

    if(flBaseSharingFlags == FSF_DEFAULT)
        flBaseSharingFlags = (m_flOpenFlags & FSF_WRITE) ? 0 : FSF_READ;

    if(flBaseSharingFlags & FSF_READ)
        flSharing |= FILE_SHARE_READ;

    if(flBaseSharingFlags & FSF_WRITE)
        flSharing |= FILE_SHARE_WRITE;

    if(m_flOpenFlags & FOF_READ)
        flAccess |= GENERIC_READ;

    if(m_flOpenFlags & FOF_WRITE)
        flAccess |= GENERIC_WRITE;

    // Opening/creating a file
    m_hFile = CreateFile(   m_FileName,
                            flAccess,
                            flSharing,
                            NULL,
                            (m_flOpenFlags & FOF_NEWFILE)   ?   CREATE_ALWAYS   :
                            (m_flOpenFlags & FOF_CREATE)    ?   OPEN_ALWAYS     :
                                                                OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

    if(!m_hFile || m_hFile == INVALID_HANDLE_VALUE)
        INITIATE_DEFINED_FAILURE((KString)TEXT("Error opening file \"") + m_FileName + TEXT("\"."));
}

#endif // _MSC_VER

void TFile::InternalOpenStream(const LPCTSTR ppModes[4])
{
    if((m_flOpenFlags & FOF_READ) && (m_flOpenFlags & FOF_WRITE))
    {
        if(m_flOpenFlags & FOF_NEWFILE)
        {
            m_pFile = _tfopen(m_FileName, ppModes[_SMI_WP]);
        }
        else
        {
            if(!(m_pFile = _tfopen(m_FileName, ppModes[_SMI_RP])))
            {
                if(m_flOpenFlags & FOF_CREATE)
                    m_pFile = _tfopen(m_FileName, ppModes[_SMI_WP]);
            }
        }
    }
    else if(m_flOpenFlags & FOF_READ)
    {
        m_pFile = _tfopen(m_FileName, ppModes[_SMI_R]);
    }
    else if(m_flOpenFlags & FOF_WRITE)
    {
        if(m_flOpenFlags & FOF_NEWFILE)
        {
            m_pFile = _tfopen(m_FileName, ppModes[_SMI_W]);
        }
        else if(!(m_pFile = _tfopen(m_FileName, ppModes[_SMI_RP])))
        {
            if(m_flOpenFlags & FOF_CREATE)
                m_pFile = _tfopen(m_FileName, ppModes[_SMI_W]);
        }
    }
    else
    {
        DEBUG_INITIATE_FAILURE;
    }

    if(!m_pFile)
        INITIATE_DEFINED_FAILURE((KString)TEXT("Error opening file \"") + m_FileName + TEXT("\"."));
}

void TFile::Open(kflags_t flSOpenFlags, kflags_t flBaseSharingFlags)
{
    DEBUG_VERIFY_ALLOCATION;

    Close();

    // Testing for exactly one mode specified
    DEBUG_VERIFY((bool)(flSOpenFlags & FOF_BINARY) != (bool)(flSOpenFlags & FOF_TEXT));

    // Testing for at least reading or writing specified
    DEBUG_VERIFY((flSOpenFlags & FOF_READ) || (flSOpenFlags & FOF_WRITE));

    // Testing for creation without writing
    DEBUG_VERIFY(!(flSOpenFlags & FOF_CREATE) || (flSOpenFlags & FOF_WRITE));

    // Testing for new file without creation
    DEBUG_VERIFY(!(flSOpenFlags & FOF_NEWFILE) || (flSOpenFlags & FOF_CREATE));

    m_flOpenFlags = flSOpenFlags;

    // Opening the file
    if(m_flOpenFlags & FOF_TEXT)
    {
        static LPCTSTR Modes[4] = {TEXT("rt"), TEXT("wt"), TEXT("r+t"), TEXT("w+t")};
        InternalOpenStream(Modes);
    }
    else if(m_flOpenFlags & FOF_BINARY)
    {
        #ifdef _MSC_VER
            InternalOpenHandle(flBaseSharingFlags);
        #else // _MSC_VER
            static LPCTSTR Modes[4] = {"rb", "wb", "r+b", "w+b"};
            InternalOpenStream(Modes);
        #endif // MSC_VER

        m_szOffset = 0;
        m_szLength = GetOwnLength();
    }
    else
    {
        DEBUG_INITIATE_FAILURE;
    }

    m_cLastOperation = 0;
}

void TFile::Reopen(bool bRetainOldOffset, kflags_t flBaseSharingFlags)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    size_t szOldOffset;

    if(bRetainOldOffset)
        szOldOffset = Tell();

    Open(m_flOpenFlags, flBaseSharingFlags);

    if(bRetainOldOffset)
        Seek(szOldOffset, FSM_BEGIN);
}

void TFile::InternalClose()
{
    if(!IsAllocated() || !IsOpen())
        return;

    try
    {
        #ifdef _MSC_VER
        {
            if(IsTextMode())
                fclose(m_pFile), m_pFile = NULL;
            else
                CloseHandle(m_hFile), m_hFile = NULL;
        }
        #else // _MSC_VER
        {
            fclose(m_pFile), m_pFile = NULL;
        }
        #endif // _MSC_VER

        if(m_bRemoveOnClose)
            InternalRemove();
    }

    catch(...)
    {
    }
}

void TFile::Close()
{
    DEBUG_VERIFY_ALLOCATION;

    InternalClose();
}

void TFile::SetLength(size_t szNewLength, bool bRetainOldOffset)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsBinaryMode() && AllowsWriting());

    size_t szCurOffset = 0;

    if(bRetainOldOffset)
        szCurOffset = Tell();

    #ifdef _MSC_VER
    {
        SetFilePointer(m_hFile, (LONG)szNewLength, NULL, FILE_BEGIN); // ... (no error check-up) // {{{ SetFilePointerEx

        if(!SetEndOfFile(m_hFile))
            INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error setting end of file \"") + m_FileName + TEXT("\""), GetLastError());
    }
    #else // _MSC_VER
    {
        fflush(m_pFile);

        if(ftruncate(fileno(m_pFile), szNewLength))
            INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error setting end of file \"") + m_FileName + TEXT("\""), errno);
    }
    #endif // _MSC_VER

    m_szLength = szNewLength;

    if(bRetainOldOffset)
        Seek(Min(szCurOffset, m_szLength), FSM_BEGIN);
    else
        m_szOffset = InternalTell();
}

TFile& TFile::Read(void* pRData, size_t szSize)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsBinaryMode() && AllowsReading());

    if(!szSize)
        return *this;

    if(m_szOffset + szSize > m_szLength)
        INITIATE_DEFINED_FAILURE((KString)TEXT("Attempt to read past the end of file \"") + m_FileName + TEXT("\"."));

    SetLastOperation('r');

    #ifdef _MSC_VER
    {
        DWORD dwRealSize;

        if(!ReadFile(m_hFile, pRData, (DWORD)szSize, &dwRealSize, NULL) || dwRealSize != szSize)
            INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error reading from file \"") + m_FileName + TEXT("\""), GetLastError());
    }
    #else // _MSC_VER
    {
        if(fread(pRData, 1, szSize, m_pFile) != szSize)
            INITIATE_DEFINED_FAILURE((KString)TEXT("Error reading from file \"") + m_FileName + TEXT("\"."));
    }
    #endif // _MSC_VER

    m_szOffset += szSize;

    return *this;
}

size_t TFile::ReadAvailable(void* pRData, size_t szSize)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsBinaryMode() && AllowsReading());

    UpdateMin(szSize, m_szLength - m_szOffset);

    Read(pRData, szSize);

    return szSize;
}

TFile& TFile::Write(const void* pData, size_t szSize)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsBinaryMode() && AllowsWriting());

    if(!szSize)
        return *this;

    SetLastOperation('w');

    #ifdef _MSC_VER
    {
        DWORD dwRealSize;

        if(!WriteFile(m_hFile, pData, (DWORD)szSize, &dwRealSize, NULL) || dwRealSize != szSize)
            INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error writing file \"") + m_FileName + TEXT("\""), GetLastError());
    }
    #else // _MSC_VER
    {
        if(fwrite(pData, 1, szSize, m_pFile) != szSize)
            INITIATE_DEFINED_FAILURE((KString)TEXT("Error writing file \"") + m_FileName + TEXT("\"."));
    }
    #endif // _MSC_VER

    m_szOffset += szSize;

    UpdateMax(m_szLength, m_szOffset);

    return *this;
}

TFile& TFile::ReadString(KString& RString, TFileReadStringMode Mode)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsTextMode() && AllowsReading());

    SetLastOperation('r');

    RString.Empty();

    TArray<TCHAR, true> ts(4096);

    while(!IsEndOfFile())
    {
        ts[0] = 0, _fgetts(ts.GetDataPtr(), (int)ts.GetN() - 1, m_pFile);

        RString += ts.GetDataPtr();

        if(RString.GetLastChar() == TEXT('\n'))
            break;
    }

    if(Mode == FRSM_DELCRLF)
    {
        if(RString.GetLastChar() == TEXT('\n'))
            RString.DelLastChar();

        if(RString.GetLastChar() == TEXT('\r'))
            RString.DelLastChar();
    }

    return *this;
}

TFile& TFile::WriteString(LPCTSTR pString, TFileWriteStringMode Mode)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsTextMode() && AllowsWriting());

    SetLastOperation('w');

    if(Mode == FWSM_ORIGINAL)
        _ftprintf(m_pFile, TEXT("%s"), pString);
    else if(Mode == FWSM_ADDCRLF)
        _ftprintf(m_pFile, TEXT("%s\n"), pString);

    return *this;
}

TFile& TFile::ReadStrings(KStrings& RStrings, bool bClearFirst, KStrings::TIterator* pRIter)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsTextMode() && AllowsReading());

    if(bClearFirst)
        RStrings.Clear();

    const KStrings::TIterator Ret = RStrings.GetLast();

    while(!IsEndOfFile())
        ReadString(*RStrings.AddLast());

    if(pRIter)
        *pRIter = Ret.IsValid() ? Ret.GetNext() : RStrings.GetFirst();

    return *this;
}

TFile& TFile::WriteStrings(const KStrings& Strings)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsTextMode() && AllowsWriting());

    for(KStrings::TConstIterator Iter = Strings.GetFirst() ; Iter.IsValid() ; ++Iter)
        WriteString(*Iter);

    return *this;
}

KString TFile::ReadString(TFileReadStringMode Mode)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsTextMode() && AllowsReading());

    KString String;

    ReadString(String, Mode);

    return String;
}

void TFile::IndentString(size_t szAmt, TCHAR cChar)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsTextMode() && AllowsWriting());

    SetLastOperation('w');

    for( ; szAmt ; szAmt--)
        _fputtc(cChar, m_pFile);
}

void TFile::WriteEOL()
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsTextMode() && AllowsWriting());

    SetLastOperation('w');

    _ftprintf(m_pFile, TEXT("\n"));
}

#ifdef _MSC_VER

void TFile::InternalSeekHandle(ptrdiff_t offset, TFileSeekMode Mode) const
{
    DWORD dwMode;

    switch(Mode)
    {
    case FSM_BEGIN:
        dwMode = FILE_BEGIN;
        break;

    case FSM_CURRENT:
        dwMode = FILE_CURRENT;
        break;

    case FSM_END:
        dwMode = FILE_END;
        break;

    default:
        DEBUG_INITIATE_FAILURE;
    }

    if(SetFilePointer(m_hFile, (LONG)offset, NULL, dwMode) == INVALID_SET_FILE_POINTER) // {{{ SetFilePointerEx
        INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error changing offset in file \"") + m_FileName + TEXT("\""), GetLastError());
}

#endif // _MSC_VER

void TFile::InternalSeekStream(ptrdiff_t offset, TFileSeekMode Mode) const
{
    int iMode = SEEK_SET;

    switch(Mode)
    {
    case FSM_BEGIN:
        iMode = SEEK_SET;
        break;

    case FSM_CURRENT:
        iMode = SEEK_CUR;
        break;

    case FSM_END:
        iMode = SEEK_END;
        break;

    default:
        DEBUG_INITIATE_FAILURE;
    }

    if(fseek(m_pFile, (long)offset, iMode)) // {{{ fsetpos
        INITIATE_DEFINED_CODE_FAILURE(TEXT("Error changing text file offset"), GetLastError());
}

void TFile::InternalSeek(ptrdiff_t offset, TFileSeekMode Mode) const
{
    if(IsTextMode())
    {
        InternalSeekStream(offset, Mode);
    }
    else
    {
        #ifdef _MSC_VER
            InternalSeekHandle(offset, Mode);
        #else // _MSC_VER
            InternalSeekStream(offset, Mode);
        #endif // _MSC_VER
    }
}

LPTSTR TFile::ReadToString(KString& RString, bool bEmptyFirst) // requires binary mode
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsBinaryMode() && AllowsReading());

    if(bEmptyFirst)
        RString.Empty();

    const size_t szLength = (GetLength() - Tell()) / sizeof(TCHAR);

    const LPTSTR pRet = RString.Extend(szLength);

    Read(pRet, szLength * sizeof(TCHAR));

    return pRet;
}

TFile& TFile::WriteFromString(LPCTSTR s, size_t l) // requires binary mode
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(IsBinaryMode() && AllowsWriting());

    if(l == -1)
        l = _tcslen(s);

    Write(s, l * sizeof(TCHAR));

    return *this;
}

void TFile::Seek(ptrdiff_t offset, TFileSeekMode Mode)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    SetLastOperation(0);

    if(IsTextMode())
    {
        InternalSeek(offset, Mode);
    }
    else
    {
        switch(Mode)
        {
        case FSM_BEGIN:
            m_szOffset = 0;
            break;

        case FSM_CURRENT:
            break;

        case FSM_END:
            m_szOffset = m_szLength;
            break;

        default:
            DEBUG_INITIATE_FAILURE;
        }

        m_szOffset += offset;

        if((int)m_szOffset < 0 || m_szOffset > m_szLength)
            INITIATE_DEFINED_FAILURE((KString)"Attempt to seek outside of file \"" + m_FileName + TEXT("\"."));

        InternalSeek(m_szOffset, FSM_BEGIN);
    }
}

size_t TFile::InternalTell() const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    if(IsTextMode())
    {
        return ftell(m_pFile);
    }
    else
    {
        #ifdef _MSC_VER
            return SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
        #else // _MSC_VER
            return ftell(m_pFile);
        #endif // _MSC_VER
    }
}

size_t TFile::GetOwnLength() const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    size_t szCurOffset = InternalTell();

    InternalSeek(0, FSM_END);
    size_t szLength = InternalTell();

    InternalSeek(szCurOffset);

    return szLength;
}

void TFile::SetLastOperation(char cOperation)
{
    DEBUG_VERIFY(!cOperation || cOperation == 'r' || cOperation == 'w');

    if(
#ifdef _MSC_VER
        IsTextMode()
#else // _MSC_VER
        true
#endif // _MSC_VER
        )
    {
        if(m_cLastOperation && cOperation && cOperation != m_cLastOperation)
            fseek(m_pFile, 0, SEEK_CUR);
    }

    m_cLastOperation = cOperation;
}

bool TFile::InternalRemove()
{
    if(IsOpen())
        return false;

    #ifdef _MSC_VER
    {
        if(DeleteFile(m_FileName))
            return true;

        if(GetLastError() == ERROR_FILE_NOT_FOUND)
            return true;

        DWORD dwAttrib = GetFileAttributes(m_FileName);

        if(dwAttrib == INVALID_FILE_ATTRIBUTES)
            return false;

        if(!SetFileAttributes(m_FileName, dwAttrib & ~FILE_ATTRIBUTE_READONLY))
            return false;

        return DeleteFile(m_FileName);
    }
    #else // _MSC_VER
    {
        return !remove(m_FileName) || !FileExists(m_FileName);
    }
    #endif // _MSC_VER
}

void TFile::Rename(LPCTSTR pFileName)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsOpen());

    if(_trename(m_FileName, pFileName))
        INITIATE_DEFINED_FAILURE((KString)TEXT("Unable to rename file \"") + m_FileName + TEXT("\" to \"") + pFileName + TEXT("\"."));

    m_FileName = pFileName;
}

void TFile::Remove(bool bSafe)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(!IsOpen());

    if(!InternalRemove() && !bSafe)
    {
        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error deleting file \"") +
                                            m_FileName              +
                                            TEXT("\""),
                                        GetLastError());
    }
}

bool TFile::DoesExist() const
{
    DEBUG_VERIFY_ALLOCATION;

    if(IsOpen())
        return true;

    return FileExists(m_FileName);
}

void TFile::Copy(LPCTSTR pSrcFileName, LPCTSTR pDstFileName)
{
    TFile SrcFile(pSrcFileName, FOF_BINARYREAD);
    TFile DstFile(pDstFileName, FOF_BINARYWRITE);

    TArray<BYTE, true> Buf(g_CommonConsts.m_szFileCopyBufferSize);

    size_t szNRemaining = SrcFile.GetLength();

    while(szNRemaining)
    {
        size_t szNTransfer = Min(szNRemaining, Buf.GetN());

        SrcFile.Read    (Buf.GetDataPtr(), szNTransfer);
        DstFile.Write   (Buf.GetDataPtr(), szNTransfer);

        szNRemaining -= szNTransfer;
    }
}

#ifdef _MSC_VER

void TFile::GetTimes(   FILETIME* pCreationTime,
                        FILETIME* pLastAccessTime,
                        FILETIME* pLastWriteTime) const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(AllowsReading());

    if(!GetFileTime(m_hFile, pCreationTime, pLastAccessTime, pLastWriteTime))
        INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Unable to get file times of \"") + m_FileName + TEXT("\"."), GetLastError());
}

void TFile::SetTimes(   const FILETIME* pCreationTime,
                        const FILETIME* pLastAccessTime,
                        const FILETIME* pLastWriteTime)
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(IsOpen());

    DEBUG_VERIFY(AllowsWriting());

    if(!SetFileTime(m_hFile, pCreationTime, pLastAccessTime, pLastWriteTime))
        INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Unable to set file times for \"") + m_FileName + TEXT("\"."), GetLastError());
}

#endif // _MSC_VER

// ------------
// File opener
// ------------
TFileOpener::TFileOpener(TFile& SFile, kflags_t flOpenFlags) : m_File(SFile)
{
    m_File.Open(flOpenFlags);
}

TFileOpener::~TFileOpener()
{
    m_File.Close();
}

// ----------------
// Global routines
// ----------------
KStrings::TIterator EnlistFiles(const KString&  Mask,
                                KStrings&       RFiles,
                                bool            bFullPaths,
                                bool            bClearFirst)
{
    if(bClearFirst)
        RFiles.Clear();

    const KStrings::TIterator Ret = RFiles.GetLast();

    KString FolderName = GetFilePath(Mask);

    #ifdef _MSC_VER
    {
        _tfinddata_t FileInfo;

        intptr_t iHandle = _tfindfirst(Mask, &FileInfo);

        if(iHandle < 0)
            return NULL;

        for(;;)
        {
            if(!(FileInfo.attrib & _A_SUBDIR))
                RFiles << (bFullPaths ? FolderName + FileInfo.name : FileInfo.name);

            // To the next file
            if(_tfindnext(iHandle, &FileInfo)==-1)
                break;
        }

        _findclose(iHandle);
    }
    #else // _MSC_VER
    {
        KString FileNameMask = GetFileName(Mask);

        DIR* pDir = opendir(FolderName.IsEmpty() ? TEXT(".") : (LPCTSTR)UnslashedFolderName(FolderName));

        if(!pDir)
            return NULL;

        for(;;)
        {
            dirent* pDirent = readdir(pDir);

            if(!pDirent)
                break;

            struct stat st;
            stat(FolderName + pDirent->d_name, &st);

            if(!S_ISDIR(st.st_mode) && DoesFileMaskMatch(pDirent->d_name, FileNameMask))
                RFiles << (bFullPaths ? FolderName + pDirent->d_name : (KString)pDirent->d_name);
        };

        closedir(pDir);
    }
    #endif // _MSC_VER

    return Ret.IsValid() ? Ret.GetNext() : RFiles.GetFirst();
}

KStrings::TIterator EnlistFolders(  const KString&  Mask,
                                    KStrings&       RFolders,
                                    bool            bFullPaths,
                                    bool            bClearFirst)
{
    if(bClearFirst)
        RFolders.Clear();

    const KStrings::TIterator Ret = RFolders.GetLast();

    KString FolderName = GetFilePath(Mask);

    #ifdef _MSC_VER
    {
        _tfinddata_t FileInfo;

        intptr_t iHandle = _tfindfirst(Mask, &FileInfo);

        if(iHandle < 0)
            return NULL;

        if(iHandle>=0)
        {
            for(;;)
            {
                if( (FileInfo.attrib & _A_SUBDIR)       &&
                    _tcscmp(FileInfo.name, TEXT("."))   &&
                    _tcscmp(FileInfo.name, TEXT("..")))
                {
                    RFolders << (bFullPaths ? FolderName + FileInfo.name : FileInfo.name) + DEFAULT_SLASH_CHAR;
                }

                // To the next file
                if(_tfindnext(iHandle, &FileInfo)==-1)
                    break;
            }

            _findclose(iHandle);
        }
    }
    #else // _MSC_VER
    {
        KString FolderNameMask = GetFileName(Mask);

        DIR* pDir = opendir(FolderName.IsEmpty() ? TEXT(".") : (LPCTSTR)UnslashedFolderName(FolderName));

        if(!pDir)
            return NULL;

        for(;;)
        {
            dirent* pDirent = readdir(pDir);

            if(!pDirent)
                break;

            if(_tcscmp(pDirent->d_name, ".") && _tcscmp(pDirent->d_name, ".."))
            {
                struct stat st;
                stat(FolderName + pDirent->d_name, &st);

                if(S_ISDIR(st.st_mode) && DoesFileMaskMatch(pDirent->d_name, FolderNameMask))
                    RFolders << (bFullPaths ? FolderName + pDirent->d_name : (KString)pDirent->d_name) + DEFAULT_SLASH_CHAR;
            }
        };

        closedir(pDir);
    }
    #endif // _MSC_VER

    return Ret.IsValid() ? Ret.GetNext() : RFolders.GetFirst();
}

KStrings::TIterator EnlistFilesRec( const KString&  FolderAndMask,
                                    KStrings&       RFiles,
                                    bool            bClearFirst)
{
    if(bClearFirst)
        RFiles.Clear();

    const KStrings::TIterator Ret = RFiles.GetLast();

    const KString Folder    = GetFilePath(FolderAndMask);
    const KString Mask      = GetFileName(FolderAndMask);

    KStrings Folders;
    EnlistFoldersRec(Folder, Folders);

    for(KStrings::TConstIterator Iter = Folders.GetFirst() ; Iter.IsValid() ; ++Iter)
        EnlistFiles(*Iter + Mask, RFiles, true, false);

    return Ret.IsValid() ? Ret.GetNext() : RFiles.GetFirst();
}

static void InternalEnlistFoldersRec(const KString& Folder, KStrings& RFolders)
{
    *RFolders.AddLast() = Folder;

    KStrings Folders;
    EnlistFolders(Folder + ALL_FILES_MASK, Folders);

    for(KStrings::TConstIterator Iter = Folders.GetFirst() ; Iter.IsValid() ; ++Iter)
        InternalEnlistFoldersRec(*Iter, RFolders);
}

KStrings::TIterator EnlistFoldersRec(   KString         Folder,
                                        KStrings&       RFolders,
                                        bool            bClearFirst)
{
    if(bClearFirst)
        RFolders.Clear();

    const KStrings::TIterator Ret = RFolders.GetLast();

    InternalEnlistFoldersRec(SlashedFolderName(Folder), RFolders);

    return Ret.IsValid() ? Ret.GetNext() : RFolders.GetFirst();
}

#ifdef _MSC_VER

void EnlistDrives(  KStrings&   Drives,
                    bool        bEnlistFloppy,
                    bool        bFullPaths)
{
    Drives.Clear();

    DWORD dwDrives = GetLogicalDrives();

    for(size_t i = 0 ; i < 32 ; i++, dwDrives >>= 1)
    {
        if((dwDrives & 1) && (i >= 2 || bEnlistFloppy))
        {
            Drives.AddLast()->Format(   bFullPaths ?
                                            TEXT("%c:\\") :
                                            TEXT("%c:"),
                                        (TCHAR)(TEXT('A') + i));
        }
    }
}

#endif // _MSC_VER

bool FileExists(LPCTSTR pFileName)
{
    FILE *pFile = _tfopen(pFileName, TEXT("rb"));

    if(pFile)
    {
        fclose(pFile);
        return true;
    }

    return false;
}

bool FolderExists(const KString& FolderName)
{
    TCHAR CurDir[1024] = TEXT("");

    _tgetcwd(CurDir, sizeof(CurDir) - 1);

    const bool bResult = !_tchdir(UnslashedFolderName(FolderName));

    _tchdir(CurDir);

    return bResult;
}

size_t GetFileLength(LPCTSTR pFileName)
{
    if(!*pFileName)
        return 0;

    size_t ret;

    FILE* file = _tfopen(pFileName, TEXT("rb"));

    if(file)
    {
        fseek(file, 0, SEEK_END);
        ret = ftell(file);
        fclose(file);
    }
    else
    {
        ret = 0;
    }

    return ret;
}

bool CreateEmptyFile(LPCTSTR pFileName)
{
    FILE* file = _tfopen(pFileName, TEXT("wb"));

    if(!file)
        return false;

    fclose(file);

    return true;
}

void CreateFolder(const KString& FolderName, bool bSafe)
{
    #ifdef _MSC_VER
    {
        if( !CreateDirectory(UnslashedFolderName(FolderName), NULL) &&
                GetLastError() != ERROR_ALREADY_EXISTS)
        {
            if(!bSafe)
            {
                INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error creating folder \"") + FolderName + TEXT("\""),
                                                GetLastError());
            }
        }
    }
    #else // _MSC_VER
    {
        if(_tmkdir(UnslashedFolderName(FolderName)) && errno != EEXIST)
        {
            if(!bSafe)
                INITIATE_DEFINED_FAILURE((KString)TEXT("Error creating folder \"") + FolderName + TEXT("\"."));
        }
    }
    #endif // _MSC_VER
}

bool CleanFolder(KString FolderName, bool bRecursive, bool bSafe)
{
    SlashFolderName(FolderName);

    bool bSuccess = true;

    // Files
    TEST_BLOCK_BEGIN
    {
        KStrings Files;
        EnlistFiles(FolderName + ALL_FILES_MASK, Files, false);

        for(KStrings::TConstIterator Iter = Files.GetFirst() ; Iter.IsValid() ; ++Iter)
        {
            TEST_BLOCK_BEGIN
            {
                TFile(FolderName + *Iter).Remove(false);
            }
            TEST_BLOCK_KFC_EXCEPTION_HANDLER
            {
                if(!bSafe)
                    throw;

                bSuccess = false;
            }
            TEST_BLOCK_END
        }
    }
    TEST_BLOCK_KFC_EXCEPTION_HANDLER
    {
        if(!bSafe)
            throw;

        bSuccess = false;
    }
    TEST_BLOCK_END

    // Folders
    TEST_BLOCK_BEGIN
    {
        KStrings Folders;

        EnlistFolders(FolderName + ALL_FILES_MASK, Folders, false);

        if(!Folders.IsEmpty())
        {
            if(!bRecursive)
                return false;

            for(KStrings::TConstIterator Iter = Folders.GetFirst() ; Iter.IsValid() ; ++Iter)
            {
                if(!DeleteFolder(FolderName + *Iter, bRecursive, bSafe))
                    bSuccess = false;
            }
        }
    }
    TEST_BLOCK_KFC_EXCEPTION_HANDLER
    {
        if(!bSafe)
            throw;

        bSuccess = false;
    }
    TEST_BLOCK_END

    return bSuccess;
}

bool DeleteFolder(const KString& FolderName, bool bRecursive, bool bSafe)
{
    if(!CleanFolder(FolderName, bRecursive, bSafe))
        return false;

    if(!GetWorkingDirectory().CollateNoCase(SlashedFolderName(FolderName)))
        _tchdir(TEXT(".."));

    if(_trmdir(UnslashedFolderName(FolderName)))
    {
        if(!bSafe)
        {
            INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error deleting folder \"") +
                                                FolderName +
                                                TEXT("\""),
                                            errno);
        }

        return false;
    }

    return true;
}

void CopyFolder(const KString&  SrcFolderName,
                const KString&  DstFolderName,
                TFileFilter*    pFilter,
                void*           pFilterParam,
                bool            bExceptionOnFail)
{
    // Getting complete folder names
    const KString SrcName = SlashedFolderName(SrcFolderName);
    const KString DstName = SlashedFolderName(DstFolderName);

    // Creating destination folder
    TEST_BLOCK_BEGIN
    {
        CreateFolder(DstName);
    }
    TEST_BLOCK_KFC_EXCEPTION_HANDLER
    {
        if(bExceptionOnFail)
            throw;

        return;
    }
    TEST_BLOCK_END

    // Copying
    KStrings::TConstIterator Iter;

    // Copying folders
    KStrings Folders;
    EnlistFolders(SrcName + ALL_FILES_MASK, Folders, false);

    for(Iter = Folders.GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if(pFilter == NULL || pFilter(SrcName + *Iter, false, pFilterParam))
        {
            TEST_BLOCK_BEGIN
            {
                CopyFolder( SrcName + *Iter,
                            DstName + *Iter,
                            pFilter,
                            pFilterParam,
                            bExceptionOnFail);
            }
            TEST_BLOCK_KFC_EXCEPTION_HANDLER
            {
                if(bExceptionOnFail)
                    throw;
            }
            TEST_BLOCK_END
        }
    }

    Folders.Clear();

    // Copying files
    KStrings Files;
    EnlistFiles(SrcName + ALL_FILES_MASK, Files, false);

    for(Iter = Files.GetFirst() ; Iter.IsValid() ; ++Iter)
    {
        if(pFilter == NULL || pFilter(SrcName + *Iter, true, pFilterParam))
        {
            TEST_BLOCK_BEGIN
            {
                TFile::Copy(SrcName + *Iter, DstName + *Iter);
            }
            TEST_BLOCK_KFC_EXCEPTION_HANDLER
            {
                if(bExceptionOnFail)
                    throw;
            }
            TEST_BLOCK_END
        }
    }

    Files.Clear();
}

void CreateFoldersTree(KString CompletePath, bool bSafe)
{
    if(CompletePath.IsEmpty())
        return;

    TEST_BLOCK_BEGIN
    {
        KStrings Folders;
        ParseFoldersSequence(CompletePath, Folders, true);

        if(IsAbsolutePath(CompletePath))
            Folders.DelFirst();

        FOR_EACH_LIST(Folders, KStrings::TConstIterator, Iter)
            CreateFolder(*Iter);
    }
    TEST_BLOCK_KFC_EXCEPTION_HANDLER
    {
        if(!bSafe)
            throw;
    }
    TEST_BLOCK_END
}

KString GetWorkingDirectory()
{
    TCHAR buf[1024] = TEXT("");

    _tgetcwd(buf, sizeof(buf) - 1);

    return SlashedFolderName(buf);
}

void SetWorkingDirectory(LPCTSTR pFolder, bool bSafe)
{
    if(_tchdir(pFolder) && !bSafe)
    {
        INITIATE_DEFINED_CODE_FAILURE(  (KString)TEXT("Error setting working directory \"") + pFolder + TEXT("\""),
                                        GetLastError());
    }
}

void RemoveFile(LPCTSTR pFileName, bool bSafe)
{
    TFile File(pFileName);

    File.Remove(bSafe);
}
