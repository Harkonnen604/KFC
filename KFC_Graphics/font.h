#ifndef font_h
#define font_h

#include <KFC_Common\structured_info.h>
#include "font_defs.h"
#include "d3d_color.h"
#include "color_defs.h"

// ---------------------
// Font creation struct
// ---------------------
struct TFontCreationStruct
{
	TD3DColor m_Color;

	FSIZE m_CharSpacing;


	TFontCreationStruct();

	void Load(TInfoNodeConstIterator InfoNode);
};

// -----
// Font
// -----
class TFont
{
private:
	bool m_bAllocated;

	FSIZE m_CharSpacing;
	FSIZE m_CharSizes[256];
	FSIZE m_MaxCharSize;

public:
	TD3DColor m_Color;

	TFont();

	virtual ~TFont()
		{ Release(); }		

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(	const TFontCreationStruct&	CreationStruct,
					const FSIZE*				SCharSizes,
					const FSIZE&				SMaxCharSize);

	virtual void Load(TInfoNodeConstIterator InfoNode) = 0;

	virtual void DrawChar(	TCHAR				cChar,
							FPOINT				DstCoords, // can be ealtered inside
							const TD3DColor&	Color = WhiteColor()) const = 0;

	virtual void DrawText(	const KString&		Text,
							FPOINT				DstCoords, // can be altered inside
							const TD3DColor&	Color = WhiteColor()) const = 0;

	void GetTextSize(const KString& Text, FSIZE& RSize) const;

	FSIZE GetTextSize(const KString& Text) const;

	// ---------------- TRIVIALS ----------------
	const FSIZE* GetCharSizes	() const { return m_CharSizes;		}
	const FSIZE& GetMaxCharSize	() const { return m_MaxCharSize;	}
	const FSIZE& GetCharSpacing	() const { return m_CharSpacing;	}
};

#endif // font_h