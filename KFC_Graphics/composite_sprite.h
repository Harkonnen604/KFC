#ifndef composite_sprite_h
#define composite_sprite_h

#include "compound_sprite.h"

// ---------------------------------
// Composite sprite creation struct
// ---------------------------------
struct TCompositeSpriteCreationStruct : public TCompoundSpriteCreationStruct
{
	TCompositeSpriteCreationStruct();

	void Load(TInfoNodeConstIterator InfoNode);
};

// ----------------------------------
// Composite sprite sprites provider
// ----------------------------------
struct TCompositeSpriteSpritesProvider : public TCompoundSpriteSpritesProvider
{
	TCompositeSpriteSpritesProvider();

	void Load(	TInfoNodeConstIterator	InfoNode,
				size_t					szNItems);
};

// -----------------
// Composite sprite
// -----------------
class TCompositeSprite : public TCompoundSprite
{
private:
	bool m_bAllocated;

public:
	static TSprite* Create(type_t tpType);

	TCompositeSprite();

	~TCompositeSprite()
		{ Release(); }

	bool IsAllocated() const
		{ return TCompoundSprite::IsAllocated() && m_bAllocated; }

	void Release(bool bFromAllocatorException = false);
	
	void Allocate(	const TCompositeSpriteCreationStruct&	CreationStruct,
					TCompositeSpriteSpritesProvider&		SpritesProvider);

	void Load(TInfoNodeConstIterator InfoNode);

	void DrawNonScaled(	const FPOINT&			DstCoords,
						const TD3DColor&		Color	= WhiteColor(),
						const TSpriteStates&	States	= TSpriteStates()) const;
	
	void DrawRect(	const FRECT&			DstRect,
					const TD3DColor&		Color	= WhiteColor(),
					const TSpriteStates&	States	= TSpriteStates()) const;
};

#endif // composite_sprite_h
