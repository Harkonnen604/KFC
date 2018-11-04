#ifndef module_h
#define module_h

// ----------------
// Global routines
// ----------------

#ifdef _MSC_VER

HMODULE GetKModuleHandle(LPCTSTR pName = NULL);

#endif // _MSC_VER

KString GetKModuleFileName(HMODULE hModule = NULL);

#endif // module_h
