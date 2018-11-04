#ifndef common_dialogs_h
#define common_dialogs_h

// ----------------
// Global routines
// ----------------
bool GetDialogOpenFileName(	HWND		hParentWnd,
							LPCTSTR		pTitle,
							LPCTSTR		pFilter,
							LPCTSTR		pInitialFileName,	
							KString&	RFileName,
							LPCTSTR		pDefExt			= NULL,
							size_t		szFilterIndex	= 1);

bool GetDialogOpenFileNames(HWND		hParentWnd,
							LPCTSTR		pTitle,
							LPCTSTR		pFilter,
							KStrings&	RFileNames,
							LPCTSTR		pDefExt			= NULL,
							size_t		szFilterIndex	= 1);

bool GetDialogSaveFileName(	HWND		hParentWnd,
							LPCTSTR		pTitle,
							LPCTSTR		pFilter,
							LPCTSTR		pInitialFileName,
							KString&	RFileName,
							LPCTSTR		pDefExt			= NULL,
							size_t		szFilterIndex	= 1);

bool ChooseFont(HWND		hParentWnd,
				bool		bUsePassed,
				LOGFONT&	lfRFont,
				bool		bAllowEffects = true);

bool ChooseColor(	HWND		hParentWnd,
					COLORREF	CustomColors[16],
					bool		bUsePassed,
					COLORREF&	crRColor);

#endif // common_dialogs_h