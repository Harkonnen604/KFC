#include "kfc_graphics_pch.h"
#include "2d_sprites.h"

#include <KFC_KTL\file_names.h>
#include <KFC_Common\image.h>
#include "pixel_formats.h"

// -------------------------------------
// Texture image sprite creation struct
// -------------------------------------
TTextureImageSpriteCreationStruct::TTextureImageSpriteCreationStruct()
{
}

void TTextureImageSpriteCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
	TSpriteCreationStruct::Load(InfoNode);

	if(!SetFileName(InfoNode->GetParameterValue(TEXT("FileName"))))
	{
		INITIATE_DEFINED_FAILURE((KString)TEXT("Invalid sprite filename: \"") + m_FileName + TEXT("\"."));
	}
}

bool TTextureImageSpriteCreationStruct::SetFileName(const KString& SFileName)
{
	m_FileName = FILENAME_TOKENS.Process(SFileName);

	return FileExists(m_FileName);
}

// ---------------------
// Texture image sprite
// ---------------------
TSprite* TTextureImageSprite::Create(type_t tpType)
{
	DEBUG_VERIFY(tpType == SPRITE_TYPE_FLAT);

	return new TTextureImageSprite;
}

TTextureImageSprite::TTextureImageSprite()
{
	m_bAllocated = false;
}

void TTextureImageSprite::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		m_TextureImage.Release();

		TSprite::Release();
	}
}

void TTextureImageSprite::Allocate(const TTextureImageSpriteCreationStruct& CreationStruct)
{
	Release();

	try
	{
		TSprite::Allocate(CreationStruct);

		TImage Image;
		Image.Load(CreationStruct.m_FileName);

		m_TextureImage.CreateFromImage(	Image,
										GetDefaultImageFilePixelFormat(
											GetFileExtension(CreationStruct.m_FileName)));

		m_bAllocated = true;
	}
	
	catch(...)
	{
		Release(true);
		throw;
	}
}

void TTextureImageSprite::Load(TInfoNodeConstIterator InfoNode)
{
	Release();

	DEBUG_VERIFY(InfoNode.IsValid());

	TTextureImageSpriteCreationStruct CreationStruct;	
	CreationStruct.Load(InfoNode);

	Allocate(CreationStruct);
}

void TTextureImageSprite::DrawNonScaled(const FPOINT&			DstCoords,
										const TD3DColor&		Color,
										const TSpriteStates&	States) const
{
	DEBUG_VERIFY_ALLOCATION;

	m_TextureImage.DrawNonScaled(DstCoords, Color * m_Color);
}

void TTextureImageSprite::DrawRect(	const FRECT&			DstRect,
									const TD3DColor&		Color,
									const TSpriteStates&	States) const
{
	DEBUG_VERIFY_ALLOCATION;

	m_TextureImage.DrawRect(DstRect, Color * m_Color);
}

bool TTextureImageSprite::HasDefaultSize() const
{
	DEBUG_VERIFY_ALLOCATION;

	return true;
}

void TTextureImageSprite::GetDefaultSize(FSIZE& RSize) const
{
	DEBUG_VERIFY_ALLOCATION;

	RSize.Set(	(float)m_TextureImage.GetSize().cx,
				(float)m_TextureImage.GetSize().cy);
}
