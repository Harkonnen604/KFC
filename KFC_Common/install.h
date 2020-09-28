#ifndef install_h
#define install_h

#ifdef _MSC_VER

// ----------------
// Global routines
// ----------------

// Installation
bool IsInstallationRegistered(LPCTSTR pName);

void UnregisterInstallation(LPCTSTR pName);

void RegisterInstallation(  LPCTSTR         pName,
                            LPCTSTR         pUninstallerFileName,
                            const KStrings* pCustomNames    = NULL,
                            const KStrings* pCustomValues   = NULL);

KString GetInstallationCustomValue(LPCTSTR pName, LPCTSTR pCustomName);

// Start-up
bool IsRegisteredAllUsersStartUp(LPCTSTR pName);

void UnregisterAllUsersStartUp(LPCTSTR pName);

void RegisterAllUsersStartUp(LPCTSTR pName, LPCTSTR pFileName);

void UnregisterCurrentUserStartUp(LPCTSTR pName);

void RegisterCurrentUserStartUp(LPCTSTR pName, LPCTSTR pFileName);

#endif // _MSC_VER

#endif // install_h
