#include "kfc_common_pch.h"
#include "install.h"

#include "registry.h"

#ifdef _MSC_VER

// ----------------
// Global routines
// ----------------

// Installation
bool IsInstallationRegistered(LPCTSTR pName)
{
	DEBUG_VERIFY(pName);

	KRegistryKey Key(	HKEY_LOCAL_MACHINE,
						(KString)TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"));

	Key.Create(KEY_READ, 0, true);

	return Key.HasKey(pName);
}

void UnregisterInstallation(LPCTSTR pName)
{
	DEBUG_VERIFY(pName);

	KRegistryKey Key(	HKEY_LOCAL_MACHINE,
						(KString)TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\") +
							pName);

	Key.Delete(true, true);
}

void RegisterInstallation(	LPCTSTR			pName,
							LPCTSTR			pUninstallerFileName,
							const KStrings*	pCustomNames,
							const KStrings* pCustomValues)
{
	DEBUG_VERIFY(pName);

	DEBUG_VERIFY(pUninstallerFileName);

	DEBUG_VERIFY(	(pCustomNames	? pCustomNames->	GetN() : 0) ==
					(pCustomValues	? pCustomValues->	GetN() : 0));

	KRegistryKey Key(	HKEY_LOCAL_MACHINE,
						(KString)TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\") +
							pName);

	Key.Create(KEY_WRITE, 0, true);

	Key.WriteString(TEXT("DisplayName"), pName);

	Key.WriteString(TEXT("UninstallString"), pUninstallerFileName);

	if(pCustomNames && pCustomValues)
	{
		KStrings::TConstIterator NIter, VIter;

		for(NIter = pCustomNames->GetFirst(), VIter = pCustomValues->GetFirst() ;
			NIter.IsValid() && VIter.IsValid() ;
			++NIter, ++VIter)
		{
			Key.WriteString(*NIter, *VIter);
		}
	}
}

KString GetInstallationCustomValue(LPCTSTR pName, LPCTSTR pCustomName)
{
	DEBUG_VERIFY(pName);

	DEBUG_VERIFY(pCustomName);

	KRegistryKey Key(	HKEY_LOCAL_MACHINE,
						(KString)TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\") +
							pName);

	Key.Open(KEY_READ);

	return Key.ReadString(pCustomName);
}

// Start-up
bool IsRegisteredAllUsersStartUp(LPCTSTR pName)
{
	DEBUG_VERIFY(pName);

	KRegistryKey Key(	HKEY_LOCAL_MACHINE,
						TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"));

	Key.Create(KEY_READ, 0, true);

	return Key.HasValue(pName);
}

void UnregisterAllUsersStartUp(LPCTSTR pName)
{
	DEBUG_VERIFY(pName);

	KRegistryKey Key(	HKEY_LOCAL_MACHINE,
						TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"));

	Key.Create(KEY_WRITE, 0, true);

	Key.DeleteValue(pName, true);
}

void RegisterAllUsersStartUp(LPCTSTR pName, LPCTSTR pFileName)
{
	DEBUG_VERIFY(pName);

	DEBUG_VERIFY(pFileName);

	KRegistryKey Key(	HKEY_LOCAL_MACHINE,
						TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"));

	Key.Create(KEY_WRITE, 0, true);

	Key.WriteString(pName, pFileName);
}

void UnregisterCurrentUserStartUp(LPCTSTR pName)
{
	DEBUG_VERIFY(pName);

	KRegistryKey Key(	HKEY_CURRENT_USER,
						TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"));

	Key.Create(KEY_WRITE, 0, true);

	Key.DeleteValue(pName, true);
}

void RegisterCurrentUserStartUp(LPCTSTR pName, LPCTSTR pFileName)
{
	DEBUG_VERIFY(pName);

	DEBUG_VERIFY(pFileName);

	KRegistryKey Key(	HKEY_CURRENT_USER,
						TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"));

	Key.Create(KEY_WRITE, 0, true);

	Key.WriteString(pName, pFileName);
}

#endif // _MSC_VER
