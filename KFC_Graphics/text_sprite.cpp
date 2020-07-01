#include "kfc_graphics_pch.h"
#include "text_sprite.h"

#include "font_storage.h"

// ----------------------------
// Text sprite creation struct
// ----------------------------
TTextSpriteCreationStruct::TTextSpriteCreationStruct()
{
}

void TTextSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
	m_TextParams.Load(InfoNode);
}

// ---------------------------
// Text sprite fonts provider
// ---------------------------
TTextSpriteFontsProvider::TTextSpriteFontsProvider()
{
	m_Font.Allocate(FONTS_REGISTRATION_MANAGER[DEFAULT_FONT_INDEX], true);
}

void TTextSpriteFontsProvider::Load(TInfoNodeConstIterator InfoNode)
{
	g_FontStorage.LoadObject(InfoNode, TEXT("Font"), m_Font, true);
}

KString TTextSpriteCreationStruct::GetText(bool* pRSuccess) const
{
	return m_TextParams.GetText(pRSuccess);
}

bool TTextSpriteCreationStruct::SetText(const KString& Text)
{
	return m_TextParams.SetText(Text);
}

// ------------
// Text sprite
// ------------
TSprite* TTextSprite::Create(type_t tpType)
{
	DEBUG_VERIFY(tpType == SPRITE_TYPE_TEXT);

	return new TTextSprite;
}

TTextSprite::TTextSprite()
{
	m_bAllocated = false;
}

void TTextSprite::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;
		
		m_TextParams.Reset();

		m_Font.Release();

		TSprite::Release();
	}
}

void TTextSprite::Allocate(	const TTextSpriteCreationStruct&	CreationStruct,
							TTextSpriteFontsProvider&			FontsProvider)
{
	Release();

	try
	{
		TSprite::Allocate(CreationStruct);

		m_TextParams = CreationStruct.m_TextParams;

		m_Font.ReOwn(FontsProvider.m_Font);		

		InternalUpdateTextSize();

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void TTextSprite::Load(TInfoNodeConstIterator InfoNode)
{
	Release();

	DEBUG_VERIFY(InfoNode.IsValid());

	TTextSpriteCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode);

	TTextSpriteFontsProvider FontsProvider;
	FontsProvider.Load(InfoNode);

	Allocate(CreationStruct, FontsProvider);
}

void TTextSprite::DrawNonScaled(const FPOINT&			DstCoords,
								const TD3DColor&		Color,
								const TSpriteStates&	States) const
{
	DEBUG_VERIFY_ALLOCATION;

	m_Font->DrawText(m_TextParams.m_Text, DstCoords, Color * m_Color);
}

void TTextSprite::DrawRect(	const FRECT&			DstRect,
							const TD3DColor&		Color,
							const TSpriteStates&	States) const
{
	DEBUG_VERIFY_ALLOCATION;	

	m_Font->DrawText(	m_TextParams.m_Text,
						m_TextParams.GetOrigin(DstRect, m_TextSize),
						Color * m_Color);
}

bool TTextSprite::HasDefaultSize() const
{
	DEBUG_VERIFY_ALLOCATION;

	return true;
}

void TTextSprite::GetDefaultSize(FSIZE& RSize) const
{
	DEBUG_VERIFY_ALLOCATION;

	RSize = m_TextSize;
}

void TTextSprite::InternalUpdateTextSize()
{
	m_Font->GetTextSize(m_TextParams.m_Text, m_TextSize);
}

void TTextSprite::SetTextParams(const TTextParams& STextParams)
{
	DEBUG_VERIFY_ALLOCATION;

	m_TextParams = STextParams;

	InternalUpdateTextSize();
}

KString TTextSprite::GetText(bool* pRSuccess) const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_TextParams.GetText(pRSuccess);
}

bool TTextSprite::SetText(const KString& Text)
{
	DEBUG_VERIFY_ALLOCATION;

	if(m_TextParams.SetText(Text))
	{
		InternalUpdateTextSize();

		return true;
	}

	return false;
}

void TTextSprite::SetTextAlignment(const ALSIZE& Alignment)
{
	DEBUG_VERIFY_ALLOCATION;

	m_TextParams.m_Alignment = Alignment;
}

void TTextSprite::SetTextFlags(flags_t flFlags)
{
	DEBUG_VERIFY_ALLOCATION;

	m_TextParams.m_flFlags = flFlags;

	InternalUpdateTextSize(); // ... (still no flags)
}

void TTextSprite::SetTextFont(TObjectPointer<TFont>& Font)
{
	DEBUG_VERIFY_ALLOCATION;

	m_Font.ReOwn(Font);

	InternalUpdateTextSize();
}