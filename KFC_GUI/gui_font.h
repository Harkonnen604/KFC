#ifndef gui_font_h
#define gui_font_h

#include <KFC_Common\structured_info.h>
#include "gui_font_defs.h"

// -------------------------
// GUI font creation struct
// -------------------------
struct T_GUI_FontCreationStruct
{
public:
	LOGFONT m_LogFont;

public:
	T_GUI_FontCreationStruct();
	
	void Load(TInfoNodeConstIterator InfoNode);

	bool SetFaceName(const KString& FaceName);
};

// ---------
// GUI font
// ---------
class T_GUI_Font
{
private:
	bool m_bAllocated;
	
	HFONT m_hFont;

public:
	static T_GUI_Font* Create(ktype_t tpType);

	T_GUI_Font();

	T_GUI_Font(const T_GUI_FontCreationStruct& CreationStruct);

	~T_GUI_Font()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(const T_GUI_FontCreationStruct& CreationStruct);

	virtual void Load(TInfoNodeConstIterator InfoNode);

	HFONT GetFont() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hFont; }

	operator HFONT () const
		{ return GetFont(); }

	operator HGDIOBJ () const
		{ return (HGDIOBJ)GetFont(); }

	// Sizes
	void GetCharSize(TCHAR			cChar,	SZSIZE& RSize) const;
	void GetTextSize(const KString&	Text,	SZSIZE& RSize) const;

	SZSIZE GetCharSize(TCHAR cChar) const
	{
		SZSIZE Size;
		GetCharSize(cChar, Size);

		return Size;
	}

	SZSIZE GetTextSize(const KString& Text) const
	{
		SZSIZE Size;
		GetTextSize(Text, Size);

		return Size;
	}
};

#endif // gui_font_h
