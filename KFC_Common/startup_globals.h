#ifndef startup_globals_h
#define startup_globals_h

#include <KFC_KTL/globals.h>

// ----------------
// Startup globals
// ----------------
class TStartupGlobals : public TGlobals
{
private:
    void OnInitialize   ();
    void OnUninitialize ();

public:
    // Files
    KString m_StartFile;

    // Folders
    KString m_StartFolder;
    KString m_TempFolder;

    #ifdef _MSC_VER
        KString m_WindowsFolder;
        KString m_SystemFolder;
        KString m_ProgramFilesFolder;
        KString m_StartMenuProgramsFolder;
        KString m_DesktopFolder;
    #endif // _MSC_VER

    // Parameters
    KStrings m_Parameters;

public:
    TStartupGlobals();
};

extern TStartupGlobals g_StartupGlobals;

#endif // startup_globals_h
