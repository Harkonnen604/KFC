#ifndef shortcut_h
#define shortcut_h

#ifdef _MSC_VER

// ----------------
// Global routines
// ----------------
KString GetShortcutTarget(	const KString&	FileName,
							bool			bResolve	= false,
							kflags_t		flFlags		= SLGP_UNCPRIORITY);

void CreateShortcut(const KString& FileName, const KString& Target);

#endif // _MSC_VER

#endif // shortcut_h
