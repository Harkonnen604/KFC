#ifndef version_h
#define version_h

// ----------------
// Global routines
// ----------------
int CompareVersions(const KString& Version1,
					const KString& Version2);

#ifdef _MSC_VER

bool IsWindowsNT();

#endif // _MSC_VER

#endif // version_h
