#include "kfc_interface_pch.h"
#include "image_control.h"

#include <KFC_Graphics\font_storage.h>
#include <KFC_Graphics\sprite_storage.h>
#include "interface_font_defs.h"

// ------------------------------
// Image control creation struct
// ------------------------------
TImageControlCreationStruct::TImageControlCreationStruct()
{
}

void TImageControlCreationStruct::Load(	TInfoNodeConstIterator	InfoNode,
										const TControl*			pParentControl,
										const FRECT&			Resolution)
{
	TControlCreationStruct::Load(InfoNode, pParentControl, Resolution);
}

// -------------------------------
// Image control sprites provider
// -------------------------------
TImageControlSpritesProvider::TImageControlSpritesProvider()
{
}

void TImageControlSpritesProvider::Load(TInfoNodeConstIterator InfoNode)
{
	g_SpriteStorage.LoadObject(InfoNode, TEXT("Sprite"), m_Sprite, true);
}

KString TImageControlSpritesProvider::GetText(bool* pRSuccess) const
{
	return GetSpriteText(m_Sprite.GetDataPtr(), pRSuccess);
}

bool TImageControlSpritesProvider::SetText(const KString& Text)
{
	return SetSpriteText(m_Sprite.GetDataPtr(), Text);
}

// --------------
// Image control
// --------------
TControl* TImageControl::LoadControl(	type_t					tpType,
										TInfoNodeConstIterator	InfoNode,
										const TControl*			pParentControl,
										const FRECT&			Resolution)
{
	DEBUG_VERIFY(tpType == CONTROL_TYPE_IMAGE);

	DEBUG_VERIFY(InfoNode.IsValid());

	TImageControlCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode, pParentControl, Resolution);

	TImageControlSpritesProvider SpritesProvider;
	SpritesProvider.Load(InfoNode);

	return new TImageControl(CreationStruct, SpritesProvider);
}

TImageControl::TImageControl(const TImageControlCreationStruct&	CreationStruct,
							 TImageControlSpritesProvider&		SpritesProvider) :

	TControl(CreationStruct)

{
	m_Sprite.ReOwn(SpritesProvider.m_Sprite);
}

// Update/render events
void TImageControl::OnRender() const
{
	TControl::OnRender();
	
	const TSprite* pSprite = m_Sprite.GetDataPtr();

	if(pSprite)
	{
		pSprite->DrawRect(	GetCurrentScreenState().m_Rect.	m_OwnPart,
							GetCurrentScreenState().m_Color.m_OwnPart,
							(float)GetVisibleState());
	}
}

TImageControl::TVisibleState TImageControl::GetVisibleState() const
{
	return GetCurrentScreenState().IsEnabled() ? VS_ENABLED : VS_DISABLED;
}

KString TImageControl::GetText(bool* pRSuccess) const
{
	return GetSpriteText(m_Sprite.GetDataPtr(), pRSuccess);
}

bool TImageControl::SetText(const KString& Text)
{
	return SetSpriteText(m_Sprite.GetDataPtr(), Text);
}

// -----------------------------------
// Easy label control creation struct
// -----------------------------------
TEasyLabelControlCreationStruct::TEasyLabelControlCreationStruct()
{
}

void TEasyLabelControlCreationStruct::Load(	TInfoNodeConstIterator	InfoNode,
											const TControl*			pParentControl,
											const FRECT&			Resolution)
{
	TControlCreationStruct::Load(InfoNode, pParentControl, Resolution);

	TTextSpriteCreationStruct::Load(InfoNode);
}

// ----------------------------------
// Easy label control fonts provider
// ----------------------------------
TEasyLabelControlFontsProvider::TEasyLabelControlFontsProvider()
{
	m_Font.Allocate(FONTS_REGISTRATION_MANAGER[DEFAULT_LABEL_FONT_INDEX], true);
}

void TEasyLabelControlFontsProvider::Load(TInfoNodeConstIterator InfoNode)
{
	TTextSpriteFontsProvider::Load(InfoNode);
}

// -------------------
// Easy label control
// -------------------
TControl* TEasyLabelControl::LoadControl(	type_t					tpType,
											TInfoNodeConstIterator	InfoNode,
											const TControl*			pParentControl,
											const FRECT&			Resolution)
{
	DEBUG_VERIFY(tpType == CONTROL_TYPE_EASY_LABEL);

	DEBUG_VERIFY(InfoNode.IsValid());

	TEasyLabelControlCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode, pParentControl, Resolution);

	TEasyLabelControlFontsProvider FontsProvider;
	FontsProvider.Load(InfoNode);

	return new TEasyLabelControl(CreationStruct, FontsProvider);
}

TEasyLabelControl::TEasyLabelControl(const TEasyLabelControlCreationStruct&	CreationStruct,
									 TEasyLabelControlFontsProvider&		FontsProvider) :

	TImageControl(CreationStruct, TImageControlSpritesProvider())	
{
	((TTextSprite*)m_Sprite.Allocate(new TTextSprite, false))->
		Allocate(CreationStruct, FontsProvider);
}