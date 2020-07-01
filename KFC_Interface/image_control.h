#ifndef image_control_h
#define image_control_h

#include <KFC_KTL\text_container.h>
#include <KFC_Graphics\sprite.h>
#include <KFC_Graphics\text_sprite.h>
#include <KFC_Graphics\sprite_helpers.h>
#include "control.h"

// ------------------------------
// Image control creation struct
// ------------------------------
struct TImageControlCreationStruct : public TControlCreationStruct
{
	TImageControlCreationStruct();

	void Load(	TInfoNodeConstIterator	InfoNode,
				const TControl*			pParentControl,
				const FRECT&			Resolution);
};

// -------------------------------
// Image control sprites provider
// -------------------------------
struct TImageControlSpritesProvider : public TTextContainer
{
	TObjectPointer<TSprite> m_Sprite;

	
	TImageControlSpritesProvider();

	void Load(TInfoNodeConstIterator InfoNode);

	KString GetText(bool* pRSuccess = NULL) const;

	bool SetText(const KString& Text);
};

// --------------
// Image control
// --------------
class TImageControl :	public TControl,
						public TTextContainer
{
protected:
	// Update/render events
	virtual void OnRender() const;

public:
	enum TVisibleState
	{
		VS_DISABLED		= 0,
		VS_ENABLED		= 1,
		VS_FORCE_UINT	= UINT_MAX,
	};

	TObjectPointer<TSprite> m_Sprite;


	static TControl* LoadControl(	type_t					tpType,
									TInfoNodeConstIterator	InfoNode,
									const TControl*			pParentControl,
									const FRECT&			Resolution);

	TImageControl(	const TImageControlCreationStruct&	CreationStruct,
					TImageControlSpritesProvider&		SpritesProvider);	

	TVisibleState GetVisibleState() const;

	KString GetText(bool* pRSuccess = NULL) const;

	bool SetText(const KString& Text);
};

// -----------------------------------
// Easy label control creation struct
// -----------------------------------
struct TEasyLabelControlCreationStruct :	TImageControlCreationStruct,
											TTextSpriteCreationStruct
{
	TEasyLabelControlCreationStruct();

	void Load(	TInfoNodeConstIterator	InfoNode,
				const TControl*			pParentControl,
				const FRECT&			Resolution);
};

// ----------------------------------
// Easy label control fonts provider
// ----------------------------------
struct TEasyLabelControlFontsProvider : TTextSpriteFontsProvider
{
	TEasyLabelControlFontsProvider();

	void Load(TInfoNodeConstIterator InfoNode);
};

// -------------------
// Easy label control
// -------------------
class TEasyLabelControl : public TImageControl
{
public:
	static TControl* LoadControl(	type_t					tpType,
									TInfoNodeConstIterator	InfoNode,
									const TControl*			pParentControl,
									const FRECT&			Resolution);

	TEasyLabelControl(	const TEasyLabelControlCreationStruct&	CreationStruct,
						TEasyLabelControlFontsProvider&			FontsProvider);
};

#endif // image_control_h