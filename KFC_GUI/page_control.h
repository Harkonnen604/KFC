#ifndef page_control_h
#define page_control_h

#include "dialog.h"

// -------------
// Page control
// -------------
class TPageControl
{
private:
	// DLGTEMPLATEEX
	typedef struct tagDLGTEMPLATEEX
	{
		WORD	dlgVer;
		WORD	signature;
		DWORD	helpID;
		DWORD	exStyle;
		DWORD	style;
		WORD	cDlgItems;
		short	x;
		short	y;
		short	cx;
		short	cy;

	}DLGTEMPLATEEX, *LPDLGTEMPLATEEX;


	// Page control
	bool m_bAllocated;	

	HWND m_hWnd;

	TArray<TDialog*, true> m_Pages;


	static int CALLBACK StaticPropertySheetCallback(HWND hWnd, UINT uiMsg, LPARAM lParam);

	static BOOL CALLBACK StaticEnumProc(HWND hWnd, LPARAM lParam);

public:
	TPageControl();

	~TPageControl()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }
	
	void Release(bool bFromAllocatorException = false);

	void Allocate(	TDialog&		ParentDialog,
					const IPOINT&	Coords,
					TDialog* const*	ppSPages,
					size_t			szN,
					size_t			szStartPage = 0);

	void SetActivePage(size_t szIndex);	

	// Getters
	size_t GetN() const;

	HWND GetWnd() const;

	operator HWND () const { return GetWnd(); }
};

#endif // page_control_h