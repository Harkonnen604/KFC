#ifndef debug_console_h
#define debug_console_h

// ----------------
// Global routines
// ----------------
void CreateDebugConsole();

#ifdef _MSC_VER

void DebugConsolePause( LPCTSTR pMessage    = TEXT("Press any key to continue..."),
                        bool    bFlushFirst = false);

#else // _MSC_VER

void DebugConsolePause(LPCTSTR pMessage = "Press Enter to continue...");

#endif // _MSC_VER

void DebugConsoleHexDump(const void* p, size_t n);

#endif // debug_console_h
