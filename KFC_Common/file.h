#ifndef file_h
#define file_h

#include <KFC_KTL/file_names.h>
#include "common_tokens.h"

// ----------------
// File open flags
// ----------------
#define FOF_BINARY		(0x0001) // binary mode
#define FOF_TEXT		(0x0002) // text mode
#define FOF_READ		(0x0004) // allow reading
#define FOF_WRITE		(0x0008) // allow writing
#define FOF_CREATE		(0x0010) // create if not exists
#define FOF_NEWFILE		(0x0020) // always create new file

#define FOF_BINARYREAD	(FOF_BINARY | FOF_READ)
#define FOF_BINARYWRITE	(FOF_BINARY | FOF_WRITE | FOF_CREATE | FOF_NEWFILE)

#define FOF_TEXTREAD	(FOF_TEXT | FOF_READ)
#define FOF_TEXTWRITE	(FOF_TEXT | FOF_WRITE | FOF_CREATE | FOF_NEWFILE)

// -------------------
// File sharing flags
// -------------------
#define FSF_READ		(0x0001)
#define FSF_WRITE		(0x0002)
#define FSF_DEFAULT		(0xFFFF) // FSF_READ when not writing, 0 otherwise

// ---------------
// File seek mode
// ---------------
enum TFileSeekMode
{
	FSM_BEGIN		= 1,
	FSM_CURRENT		= 2,
	FSM_END			= 3,
	FSM_FORCE_UINT	= UINT_MAX,
};

// ----------------------
// File read string mode
// ----------------------
enum TFileReadStringMode
{
	FRSM_ORIGINAL	= 0,
	FRSM_DELCRLF	= 1,
	FRSM_FORCE_UINT	= UINT_MAX,
};

// -----------------------
// File write string mode
// -----------------------
enum TFileWriteStringMode
{
	FWSM_ORIGINAL	= 0,
	FWSM_ADDCRLF	= 1,
	FWSM_FORCE_UINT	= UINT_MAX,
};

// ----------------
// Global routines
// ----------------
size_t GetFileLength(LPCTSTR pFileName);

// -----
// File
// -----
class TFile : public TStream
{
private:
	bool m_bAllocated;

	union
	{
		#ifdef _MSC_VER
		HANDLE m_hFile;
		#endif // _MSC_VER

		FILE* m_pFile;
	};

	KString m_FileName;

	kflags_t m_flOpenFlags;

	size_t m_szOffset; // binary mode only
	size_t m_szLength; // binary mode only

	char m_cLastOperation; // "\0rw"

private:
	size_t GetOwnLength() const;

	void SetLastOperation(char cOperation);

#ifdef _MSC_VER
	void InternalOpenHandle(kflags_t flBaseSharingFlags);
#endif // _MSC_VER
	void InternalOpenStream(const LPCTSTR ppModes[4]);

	void	InternalClose	();
	bool	InternalRemove	();

#ifdef _MSC_VER
	void InternalSeekHandle(int iOffset, TFileSeekMode Mode = FSM_BEGIN) const;
#endif // _MSC_VER
	void InternalSeekStream(int iOffset, TFileSeekMode Mode = FSM_BEGIN) const;

	void InternalSeek(int iOffset, TFileSeekMode Mode = FSM_BEGIN) const;

	size_t InternalTell() const;

public:
	bool m_bRemoveOnClose;
	bool m_bRemoveOnRelease;

public:
	TFile();

	TFile(LPCTSTR pFileName);

	TFile(LPCTSTR pFileName, kflags_t flSOpenFlags, kflags_t flBaseSharingFlags = FSF_DEFAULT);

	~TFile()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(LPCTSTR pFileName);

	void Allocate(LPCTSTR pFileName, kflags_t flSOpenFlags, kflags_t flBaseSharingFlags = FSF_DEFAULT);

	bool IsOpen() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_pFile ? true : false;
	}

	bool IsBinaryMode() const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsOpen());

		return m_flOpenFlags & FOF_BINARY;
	}

	bool IsTextMode() const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsOpen());

		return m_flOpenFlags & FOF_TEXT;
	}

	bool AllowsReading() const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsOpen());

		return m_flOpenFlags & FOF_READ;
	}

	bool AllowsWriting() const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsOpen());

		return m_flOpenFlags & FOF_WRITE;
	}

	const KString& GetFileName() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_FileName;
	}

	size_t GetLength() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return IsOpen() ? (IsBinaryMode() ? m_szLength : GetOwnLength()) : GetFileLength(m_FileName);
	}

	void Open(kflags_t flSOpenFlags, kflags_t flBaseSharingFlags = FSF_DEFAULT); // reopens if opened

	void Reopen(bool bRetainOldOffset, kflags_t flBaseSharingFlags = FSF_DEFAULT);

	void Close(); // safe for closed

	void SetLength(size_t szNewLength, bool bRetainOldOffset);

	TFile& Read	(void*			pRData,	size_t szSize);
	TFile& Write(const void*	pData,	size_t szSize);

	size_t ReadAvailable(void* pRData, size_t szSize);

	TFile& ReadString	(KString&	RString, TFileReadStringMode	Mode = FRSM_DELCRLF);
	TFile& WriteString	(LPCTSTR	pString, TFileWriteStringMode	Mode = FWSM_ADDCRLF);

	TFile& ReadStrings	(KStrings&			RStrings, bool bClearFirst = true, KStrings::TIterator* pRIter = NULL);
	TFile& WriteStrings	(const KStrings&	Strings);

	KString ReadString(TFileReadStringMode Mode = FRSM_DELCRLF);

	void IndentString(size_t szAmt, TCHAR cChar = TEXT('\t'));

	void WriteEOL();

	LPTSTR ReadToString(KString& RString, bool bEmptyFirst = true); // requires binary mode

	KString ReadToString()
	{
		KString String;
		ReadToString(String);

		return String;
	}

	TFile& WriteFromString(LPCTSTR s, size_t l = UINT_MAX);

	TFile& WriteFromString(const KString& String)
		{ return WriteFromString((LPCTSTR)String, String.GetLength()); }

	void Seek(int iOffset, TFileSeekMode Mode = FSM_BEGIN);

	size_t Tell() const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsOpen());

		return IsBinaryMode() ? m_szOffset : InternalTell();
	}

	size_t GetOffset() const
		{ return Tell(); }

	bool IsEndOfFile() const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsOpen());

		return IsBinaryMode() ? m_szOffset == m_szLength : feof(m_pFile);
	}

	void Rename(LPCTSTR pFileName);

	void Remove(bool bSafe);

	bool DoesExist() const;

	static void Copy(LPCTSTR pSrcFileName, LPCTSTR pDstFileName);

#ifdef _MSC_VER

	void GetTimes(	FILETIME* pCreationTime,
					FILETIME* pLastAccessTime,
					FILETIME* pLastWriteTime) const;

	void SetTimes(	const FILETIME* pCreationTime,
					const FILETIME* pLastAccessTime,
					const FILETIME* pLastWriteTime);
#endif // _MSC_VER

	#ifdef _MSC_VER

	HANDLE GetHandle()
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsOpen());

		DEBUG_VERIFY(IsBinaryMode());

		return m_hFile;
	}

	operator HANDLE ()
		{ return GetHandle(); }

	#endif // _MSC_VER

	FILE* GetStream()
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(IsOpen());

		#ifdef _MSC_VER
			DEBUG_VERIFY(IsTextMode());
		#endif // _MSC_VER

		return m_pFile;
	}

	operator FILE* ()
		{ return GetStream(); }

	void StreamRead(void* pRData, size_t szLength)
		{ Read(pRData, szLength); }

	void StreamWrite(const void* pData, size_t szLength)
		{ Write(pData, szLength); }

	kflags_t GetOpenFlags() const
		{ DEBUG_VERIFY_ALLOCATION; return m_flOpenFlags; }
};

// ------------
// File opener
// ------------
class TFileOpener
{
private:
	TFile& m_File;

public:
	TFileOpener(TFile& SFile, kflags_t flOpenFlags);

	~TFileOpener();

	// ---------------- TRIVIALS ----------------
	TFile&			GetFile()		{ return m_File; }
	const TFile&	GetFile() const	{ return m_File; }
};

// ------------
// File filter
// ------------
typedef bool TFileFilter(const KString& Name, bool bFile, void* pParam);

// -----------------------
// File system entry type
// -----------------------
enum TFileSystemEntryType
{
	FSET_NONE		= 0,
	FSET_FILE		= 1,
	FSET_FOLDER		= 2,
	FSET_FORCE_UINT	= UINT_MAX,
};

DECLARE_BASIC_STREAMING(TFileSystemEntryType);

// ----------------
// Global routines
// ----------------
KStrings::TIterator EnlistFiles(const KString&	Mask,
								KStrings&		RFiles,
								bool			bFullPaths	= true,
								bool			bClearFirst	= true);

KStrings::TIterator EnlistFolders	(	const KString&	Mask,
										KStrings&		RFolders,
										bool			bFullPaths	= true,
										bool			bClearFirst	= true);

KStrings::TIterator EnlistFilesRec(	const KString&	FolderAndMask,
									KStrings&		RFiles,
									bool			bClearFirst	= true);

KStrings::TIterator EnlistFoldersRec(	KString		Folder,
										KStrings&	RFolders,
										bool		bClearFirst	= true);

#ifdef _MSC_VER

void EnlistDrives(	KStrings&	Drives,
					bool		bEnlistFloppy	= true,
					bool		bFullPaths		= true);

#endif // _MSC_VER

bool FileExists(LPCTSTR pFileName);

bool FolderExists(const KString& FolderName);

void CreateFolder(const KString& FolderName, bool bSafe = false);

bool CleanFolder(KString FolderName, bool bRecursive, bool bSafe);

bool DeleteFolder(const KString& FolderName, bool bRecursive, bool bSafe);

inline void RecreateFolder(const KString& FolderName)
	{ DeleteFolder(FolderName, true, true), CreateFolder(FolderName); }

void CopyFolder(const KString&	SrcFolderName,
				const KString&	DstFolderName,
				TFileFilter*	pFilter				= NULL,
				void*			pFilterParam		= NULL,
				bool			bExceptionOnFail	= true);

void CreateFoldersTree(KString CompletePath, bool bSafe = false);

bool CreateEmptyFile(LPCTSTR pFileName);

KString GetWorkingDirectory();

void SetWorkingDirectory(LPCTSTR pFolder, bool bSafe = false);

void RemoveFile(LPCTSTR pFileName, bool bSafe);

// ----------------------------
// Current directory preserver
// ----------------------------
class TCurrentDirectoryPreserver
{
private:
	KString m_Folder;

public:
	TCurrentDirectoryPreserver()
		{ m_Folder = GetWorkingDirectory(); }

	~TCurrentDirectoryPreserver()
		{ SetWorkingDirectory(m_Folder, true); }
};

// -------------
// File deleter
// -------------
class TFileDeleter
{
private:
	KString m_FileName;

public:
	TFileDeleter() {}

	TFileDeleter(LPCTSTR pFileName)
	{
		Allocate(pFileName);
	}

	~TFileDeleter()
		{ Release(); }

	bool IsAllocated() const
		{ return !m_FileName.IsEmpty(); }

	void Release()
	{
		if(!m_FileName.IsEmpty())
			_tremove(m_FileName), m_FileName.Empty();
	}

	void Allocate(LPCTSTR pFileName)
	{
		DEBUG_VERIFY(*pFileName);

		m_FileName = pFileName;
	}
};

// ---------------
// Folder deleter
// ---------------
class TFolderDeleter
{
private:
	KString m_FolderName;

public:
	TFolderDeleter() {}

	TFolderDeleter(LPCTSTR pFolderName)
	{
		Allocate(pFolderName);
	}

	~TFolderDeleter()
		{ Release(); }

	bool IsAllocated() const
		{ return !m_FolderName.IsEmpty(); }

	void Release()
	{
		if(!m_FolderName.IsEmpty())
			DeleteFolder(m_FolderName, true, true), m_FolderName.Empty();
	}

	void Allocate(LPCTSTR pFolderName)
	{
		DEBUG_VERIFY(*pFolderName);

		m_FolderName = pFolderName;
	}
};

#endif // file_h
