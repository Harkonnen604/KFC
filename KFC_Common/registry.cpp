#include "kfc_common_pch.h"
#include "registry.h"

#ifdef _MSC_VER

#include <KFC_KTL\file_names.h>
#include "common_consts.h"

// -------------
// Registry key
// -------------
KRegistryKey::KRegistryKey()
{
	m_hParentKey = NULL;

	m_hKey = NULL;
}

KRegistryKey::KRegistryKey(	HKEY			hSParentKey,
							const KString&	SName)
{
	m_hParentKey = NULL;

	m_hKey = NULL;

	Allocate(hSParentKey, SName);
}

KRegistryKey::KRegistryKey(	HKEY			hSParentKey,
							const KString&	SName,
							REGSAM			rsAccess)
{
	m_hParentKey = NULL;

	m_hKey = NULL;

	Allocate(hSParentKey, SName, rsAccess);
}

void KRegistryKey::Release()
{
	if(IsAllocated())
		Close();
	
	m_hParentKey = NULL;

	m_Name.Empty();

	m_hKey = NULL;
}

void KRegistryKey::Allocate(HKEY			hSParentKey,
							const KString&	SName)
{
	Release();

	try
	{
		DEBUG_VERIFY(hSParentKey != NULL);
	
		m_hParentKey = hSParentKey;

		m_Name = SlashedFolderName(SName);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void KRegistryKey::Allocate(HKEY			hSParentKey,
							const KString&	SName,
							REGSAM			rsAccess)
{
	Release();

	try
	{
		Allocate(hSParentKey, SName);

		Open(rsAccess);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void KRegistryKey::Close()
{
	DEBUG_VERIFY_ALLOCATION;
	
	if(!IsOpen())
		return;
	
	RegCloseKey(m_hKey), m_hKey = NULL;
}

KRegistryKey& KRegistryKey::Open(REGSAM rsAccess)
{
	DEBUG_VERIFY_ALLOCATION;

	if(IsOpen())
		Close();

	size_t szStartTime = GetTickCount();
	
	LONG r;
	while(r = RegOpenKeyEx(m_hParentKey, UnslashedFolderName(m_Name), 0, rsAccess, &m_hKey))
	{
		if(r == ERROR_FILE_NOT_FOUND)
		{
			INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error opening registry key \"") +
												m_Name +
												TEXT("\""),
											r);
		}

		if(GetTickCount() - szStartTime >= g_CommonConsts.m_szRegistryKeyTimeout)
		{
			m_hKey = NULL;

			INITIATE_DEFINED_FAILURE(	(KString)TEXT("Registry key \"") +
										m_Name +
										TEXT("\" opening time out."));
		}
	}

	return *this;
}

bool KRegistryKey::Create(	REGSAM		rsAccess,
							kflags_t	flOptions,
							bool		bAutoOpen)
{
	DEBUG_VERIFY_ALLOCATION;

	if(IsOpen())
		return false;

	DWORD dwDisposition;

	size_t szStartTime = GetTickCount();
	while(RegCreateKeyEx(	m_hParentKey,
							UnslashedFolderName(m_Name),
							0,
							NULL,
							flOptions,
							rsAccess,
							NULL,
							&m_hKey,
							&dwDisposition))
	{
		const size_t r = GetLastError();

		if(GetTickCount() - szStartTime >= g_CommonConsts.m_szRegistryKeyTimeout)
		{
			m_hKey = NULL;
			INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error creating registry key: \"") +
												m_Name +
												TEXT("\""),
											r);
		}
	}

	if(!bAutoOpen)
		Close();

	return dwDisposition != REG_OPENED_EXISTING_KEY;
}

HKEY KRegistryKey::GetKey() const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	return m_hKey;
}

void KRegistryKey::ReadInt64(const KString& ValueName, INT64& iRValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	DWORD dwType;
	DWORD dwSize = sizeof(iRValue);
	if(	(r = RegQueryValueEx(m_hKey, ValueName, NULL, &dwType, (BYTE*)&iRValue, &dwSize)) ||
		dwType != REG_BINARY || dwSize != sizeof(iRValue))
	{
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error reading INT64 registry value \"") + ValueName + TEXT("\""), r);
	}
}

void KRegistryKey::ReadQWORD(const KString& ValueName, QWORD& uiRValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	DWORD dwType;
	DWORD dwSize = sizeof(uiRValue);
	if(	(r = RegQueryValueEx(m_hKey, ValueName, NULL, &dwType, (BYTE*)&uiRValue, &dwSize)) ||
		dwType != REG_BINARY || dwSize != sizeof(uiRValue))
	{
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error reading QWORD registry value \"") + ValueName + TEXT("\""), r);
	}
}

void KRegistryKey::ReadInt(const KString& ValueName, int& iRValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	DWORD dwType;
	DWORD dwSize = sizeof(iRValue);
	if(	(r = RegQueryValueEx(m_hKey, ValueName, NULL, &dwType, (BYTE*)&iRValue, &dwSize)) ||
		dwType != REG_DWORD || dwSize != sizeof(iRValue))
	{
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error reading INT registry value \"") + ValueName + TEXT("\""), r);
	}
}

void KRegistryKey::ReadUINT(const KString& ValueName, UINT& uiRValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	DWORD dwType;
	DWORD dwSize = sizeof(uiRValue);
	if(	(r = RegQueryValueEx(m_hKey, ValueName, NULL, &dwType, (BYTE*)&uiRValue, &dwSize)) ||
		dwType != REG_DWORD || dwSize != sizeof(uiRValue))
	{
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error reading UINT registry value \"") + ValueName + TEXT("\""), r);
	}
}

void KRegistryKey::ReadString(const KString& ValueName, KString& RValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	// Reading string
	DWORD dwType;
	DWORD dwSize;
	if(	(r = RegQueryValueEx(m_hKey, ValueName, NULL, &dwType, NULL, &dwSize)) ||
		dwType != REG_SZ ||
		(r = RegQueryValueEx(	m_hKey, ValueName, NULL, NULL,
								(BYTE*)RValue.Allocate((dwSize + sizeof(TCHAR) - 1) / sizeof(TCHAR)), &dwSize)))
	{
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error reading STRING registry value \"") + ValueName + TEXT("\""), r);
	}

	// Truncating trailing zeroes
	{
		size_t i;

		for(i = RValue.GetLength() - 1 ; i != UINT_MAX && !RValue[i] ; i--);

		RValue.SetLeft(i + 1);
	}
}

void KRegistryKey::ReadBool(const KString& ValueName, bool& bRValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	UINT uiValue;

	ReadUINT(ValueName, uiValue);

	bRValue = uiValue;
}

void KRegistryKey::ReadGUID(const KString& ValueName, GUID& RValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	DWORD dwType;
	DWORD dwSize = sizeof(RValue);
	if(	(r = RegQueryValueEx(m_hKey, ValueName, NULL, &dwType, (BYTE*)&RValue, &dwSize)) ||
		dwType != REG_BINARY || dwSize != sizeof(RValue))
	{
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error reading GUID registry value \"") + ValueName + TEXT("\""), r);
	}
}

void KRegistryKey::ReadFloat(const KString& ValueName, float& fRValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	fRValue = (float)atof(ReadString(ValueName));
}

void KRegistryKey::ReadDouble(const KString& ValueName, double& dRValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	dRValue = atof(ReadString(ValueName));
}

void KRegistryKey::ReadInt64(const KString& ValueName, INT64& iRValue, INT64 iDefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadInt64(ValueName, iRValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		iRValue = iDefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::ReadQWORD(const KString& ValueName, QWORD& qwRValue, QWORD qwDefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadQWORD(ValueName, qwRValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		qwRValue = qwDefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::ReadInt(const KString& ValueName, int& iRValue, const int iDefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadInt(ValueName, iRValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		iRValue = iDefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::ReadUINT(const KString& ValueName, UINT& uiRValue, const UINT uiDefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadUINT(ValueName, uiRValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		uiRValue = uiDefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::ReadString(const KString& ValueName, KString& RValue, const KString& DefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadString(ValueName, RValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		RValue = DefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::ReadBool(const KString& ValueName, bool& bRValue, const bool bDefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadBool(ValueName, bRValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		bRValue = bDefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::ReadGUID(const KString& ValueName, GUID& RValue, const GUID& DefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadGUID(ValueName, RValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		RValue = DefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::ReadFloat(const KString& ValueName, float& fRValue, const float fDefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadFloat(ValueName, fRValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		fRValue = fDefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::ReadDouble(const KString& ValueName, double& dRValue, const double dDefaultValue) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		ReadDouble(ValueName, dRValue);
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		dRValue = dDefaultValue;
	}
	TEST_BLOCK_END
}

void KRegistryKey::WriteInt64(const KString& ValueName, const INT64 iValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	if(r = RegSetValueEx(m_hKey, ValueName, NULL, REG_BINARY, (BYTE*)&iValue, sizeof(iValue)))
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error writing INT64 registry value ") + ValueName + TEXT("\""), r);
}

void KRegistryKey::WriteQWORD(const KString& ValueName, const QWORD qwValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	if(r = RegSetValueEx(m_hKey, ValueName, NULL, REG_BINARY, (BYTE*)&qwValue, sizeof(qwValue)))
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error writing QWORD registry value ") + ValueName + TEXT("\""), r);
}

void KRegistryKey::WriteInt(const KString& ValueName, const int iValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	if(r = RegSetValueEx(m_hKey, ValueName, NULL, REG_DWORD, (BYTE*)&iValue, sizeof(iValue)))
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error writing INT registry value ") + ValueName + TEXT("\""), r);
}

void KRegistryKey::WriteUINT(const KString& ValueName, const UINT uiValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	if(r = RegSetValueEx(m_hKey, ValueName, NULL, REG_DWORD, (BYTE*)&uiValue, sizeof(uiValue)))
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error writing UINT registry value ") + ValueName + TEXT("\""), r);
}

void KRegistryKey::WriteString(const KString& ValueName, const KString& Value)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	if(r = RegSetValueEx(m_hKey, ValueName, NULL, REG_SZ, (BYTE*)Value.GetDataPtr(), (Value.GetLength() + 1) * sizeof(TCHAR)))
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error writing STRING registry value ") + ValueName + TEXT("\""), r);
}

void KRegistryKey::WriteBool(const KString& ValueName, const bool bValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	WriteUINT(ValueName, bValue ? 1 : 0);
}

void KRegistryKey::WriteGUID(const KString& ValueName, const GUID& Value)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	int r;

	if(r = RegSetValueEx(m_hKey, ValueName, NULL, REG_BINARY, (BYTE*)&Value, sizeof(Value)))
		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error writing GUID registry value ") + ValueName + TEXT("\""), r);
}

void KRegistryKey::WriteFloat(const KString& ValueName, const float fValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	WriteString(ValueName, fValue);
}

void KRegistryKey::WriteDouble(const KString& ValueName, const double dValue)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	WriteString(ValueName, dValue);
}

DWORD KRegistryKey::GetValueType(const KString& ValueName) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	DWORD dwType;
	LONG r;
	if(r = RegQueryValueEx(m_hKey, ValueName, NULL, &dwType, NULL, NULL))
	{
		if(r == ERROR_FILE_NOT_FOUND)
			return REG_NONE;

		INITIATE_DEFINED_CODE_FAILURE((KString)TEXT("Error getting regisry value \"") + ValueName + TEXT("\" type"), r);
	}
		
	return dwType;
}

bool KRegistryKey::HasKey(const KString& KeyName) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	TEST_BLOCK_BEGIN
	{
		KRegistryKey(*this, KeyName).Open(KEY_READ);

		return true;
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		return false;
	}
	TEST_BLOCK_END
}

bool KRegistryKey::HasValue(const KString& ValueName) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	return GetValueType(ValueName) != REG_NONE;
}

void KRegistryKey::EnlistKeys(KStrings& RKeys) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	RKeys.Clear();

	for(size_t i = 0 ; ; i++)
	{
		TCHAR Name[16384];
		DWORD dwNameLength = sizeof(Name) - 1;

		LONG res = RegEnumKeyEx(*this, i, Name, &dwNameLength, NULL, NULL, NULL, NULL);

		if(res && res != ERROR_MORE_DATA)
			break;

		Name[dwNameLength] = 0;

		if(Name[0])
			(*RKeys.AddLast() = Name) += TEXT("\\");
	}
}

void KRegistryKey::EnlistValues(KStrings& RValues) const
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	RValues.Clear();

	for(size_t i = 0 ; ; i++)
	{
		TCHAR Name[16384];
		DWORD dwNameLength = sizeof(Name) - 1;

		LONG res = RegEnumValue(*this, i, Name, &dwNameLength, NULL, NULL, NULL, NULL);

		if(res && res != ERROR_MORE_DATA)
			break;

		Name[dwNameLength] = 0;

		if(Name[0])
			*RValues.AddLast() = Name;
	}
}

bool KRegistryKey::DeleteValue(const KString& ValueName, bool bSafe)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(IsOpen());

	if(RegDeleteValue(*this, ValueName))
	{
		if(bSafe)
			return false;

		INITIATE_DEFINED_CODE_FAILURE(	TEXT("Error deleteting registry key value"),
										GetLastError());
	}

	return true;
}

bool KRegistryKey::Delete(bool bRecursive, bool bSafe)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(!IsOpen());

	bool bSuccess = false;

	// Deleting sub-keys
	TEST_BLOCK_BEGIN
	{
		KRegistryKeyOpener Opener0(*this, KEY_ALL_ACCESS);

		KStrings Keys;
		EnlistKeys(Keys);

		if(!Keys.IsEmpty())
		{
			if(!bRecursive)
				return false;

			// Deleting sub-keys
			for(KStrings::TConstIterator Iter = Keys.GetFirst() ; Iter.IsValid() ; ++Iter)
			{
				if(!KRegistryKey(*this, *Iter).Delete(bRecursive, bSafe))
					bSuccess = false;
			}
		}
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		if(!bSafe)
			throw;
	}
	TEST_BLOCK_END

	// Deleting the key
	if(RegDeleteKey(m_hParentKey, m_Name))
	{
		if(!bSafe)
		{		
			INITIATE_DEFINED_CODE_FAILURE(	(KString)TEXT("Error deleting registry key \"") +
												m_Name +
												TEXT("\""),
											GetLastError());
		}
		
		bSuccess = false;
	}

	return bSuccess;
}

// --------------------
// Registry key opener
// --------------------
KRegistryKeyOpener::KRegistryKeyOpener(	KRegistryKey&	SKey,
										REGSAM			rsAccess) : m_Key(SKey)
{
	m_Key.Open(rsAccess);
}

KRegistryKeyOpener::~KRegistryKeyOpener()
{
	m_Key.Close();
}

static HKEY GetBaseKey(LPCTSTR pBaseKeyName)
{
	if(	!CompareNoCase(pBaseKeyName, TEXT("HKEY_CLASSES_ROOT")) ||
		!CompareNoCase(pBaseKeyName, TEXT("HKCR")))
	{
		return HKEY_CLASSES_ROOT;
	}

	if(	!CompareNoCase(pBaseKeyName, TEXT("HKEY_LOCAL_MACHINE")) ||
		!CompareNoCase(pBaseKeyName, TEXT("HKLM")))
	{
		return HKEY_LOCAL_MACHINE;
	}

	if(	!CompareNoCase(pBaseKeyName, TEXT("HKEY_CURRENT_USER")) ||
		!CompareNoCase(pBaseKeyName, TEXT("HKCU")))
	{
		return HKEY_CURRENT_USER;
	}

	INITIATE_DEFINED_FAILURE((KString)TEXT("Invalid base key name: \"") + pBaseKeyName + TEXT("\"."));
}

// ----------------
// Global routines
// ----------------
void ProcessMultiReg(LPCTSTR pText, bool bSafe, bool bUnroll)
{
	TEST_BLOCK_BEGIN
	{
		KStrings Rows;
		Rows.SplitToTokens(pText, TEXT("\r\n"));

		KRegistryKey Key;

		HKEY	hLastBaseKey = NULL;
		KString	LastKeyName;

		FOR_EACH_LIST(Rows, KStrings::TIterator, Iter)
		{
			size_t i;

			Iter->Trim();

			i = Iter->FindOneOf(TEXT("/\\"));

			if(i == UINT_MAX)
				INITIATE_DEFINED_FAILURE((KString)TEXT("No base key found in multireg row \"" + *Iter + TEXT("\".")));

			KString BaseKeyName		= Iter->Left(i);
			KString NameAndValue	= Iter->Mid (i+1);			

			HKEY hBaseKey = GetBaseKey(BaseKeyName);			

			KString	Name, Value;
			bool	bValue;
			TCHAR	cType;

			i = NameAndValue.Find(TEXT('='));

			if(i == UINT_MAX)
			{
				Name	= NameAndValue;
				bValue	= false;
			}
			else
			{
				Name = NameAndValue.Left(i).Trimmed();

				if(NameAndValue[i + 1] == 'S' || NameAndValue[i + 1] == 'D')
				{
					cType = _totlower(NameAndValue[i + 1]);

					Value = NameAndValue.Mid(i + 2).Trimmed();
				}
				else
				{
					cType = 's';

					Value = NameAndValue.Mid(i + 1).Trimmed();
				}
				
				bValue = true;
			}

			FixSlashes(Name, TEXT('\\'));

			KString KeyName = bValue ? GetFilePath(Name) : SlashedFolderName(Name);

			TEST_BLOCK_BEGIN
			{
				if(hBaseKey != hLastBaseKey || CompareNoCase(LastKeyName, KeyName))
				{
					Key.Allocate(hBaseKey, KeyName);

					Key.Create(KEY_WRITE, 0, true);

					hLastBaseKey	= hBaseKey;
					LastKeyName		= KeyName;
				}
			}
			TEST_BLOCK_KFC_EXCEPTION_HANDLER
			{
				if(!bSafe)
					throw;

				Key.Release();

				hLastBaseKey = NULL, LastKeyName.Empty();
			}
			TEST_BLOCK_END

			if(Key.IsAllocated() && Key.IsOpen() && bValue)
			{
				TEST_BLOCK_BEGIN
				{
					if(cType == 's')
						Key.WriteString(GetFileName(Name), Value);
					else if(cType == 'd')
						Key.WriteUINT(GetFileName(Name), ReadFromString<DWORD>(Value));
				}
				TEST_BLOCK_KFC_EXCEPTION_HANDLER
				{
					if(!bSafe)
						throw;
				}
				TEST_BLOCK_END
			}
		}
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		if(bUnroll)
			UnprocessMultiReg(pText, false);

		throw;
	}
	TEST_BLOCK_END
}

void UnprocessMultiReg(LPCTSTR pText, bool bForceKeyDeletion)
{
	KStrings Rows;
	Rows.SplitToTokens(pText, TEXT("\r\n"));

	KRegistryKey Key;

	HKEY	hLastBaseKey = NULL;
	KString	LastKeyName;

	FOR_EACH_LIST_REV(Rows, KStrings::TIterator, Iter)
	{
		size_t i;

		Iter->Trim();

		i = Iter->FindOneOf(TEXT("/\\"));

		if(i == UINT_MAX)
			INITIATE_DEFINED_FAILURE((KString)TEXT("No base key found in multireg row \"" + *Iter + TEXT("\".")));

		KString BaseKeyName  = Iter->Left(i);
		KString NameAndValue = Iter->Mid (i+1);			

		HKEY hBaseKey = GetBaseKey(BaseKeyName);			

		KString	Name;
		bool	bValue;

		i = NameAndValue.Find(TEXT('='));

		if(i == UINT_MAX)
		{
			Name	= NameAndValue;
			bValue	= false;
		}
		else
		{
			Name	= NameAndValue.Left(i).Trimmed();
			bValue	= true;
		}

		FixSlashes(Name, TEXT('\\'));

		KString KeyName = bValue ? GetFilePath(Name) : SlashedFolderName(Name);

		if(hBaseKey != hLastBaseKey || CompareNoCase(LastKeyName, KeyName))
		{
			TEST_BLOCK_BEGIN
			{
				Key.Allocate(hBaseKey, KeyName);

				Key.Open(KEY_WRITE);

				hLastBaseKey = hBaseKey;
				LastKeyName  = KeyName;
			}
			TEST_BLOCK_KFC_EXCEPTION_HANDLER
			{
				Key.Release();

				hLastBaseKey = NULL, LastKeyName.Empty();
			}
			TEST_BLOCK_END
		}

		if(Key.IsAllocated() && Key.IsOpen())
		{
			if(bValue)
			{
				KString ValueName = GetFileName(Name);

				if(!ValueName.IsEmpty())
					Key.DeleteValue(ValueName, true);
			}
			else
			{
				Key.Close();

				Key.Delete(bForceKeyDeletion, true);

				Key.Release();

				hLastBaseKey = NULL, LastKeyName.Empty();
			}
		}
	}
}

#endif // _MSC_VER
