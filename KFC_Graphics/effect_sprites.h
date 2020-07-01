#ifndef effect_sprites_h
#define effect_sprites_h

#include "sprite.h"

// -----------------------------------
// Rect effect sprite creation struct
// -----------------------------------
struct TRectEffectSpriteCreationStruct : public TSpriteCreationStruct
{
	TD3DColor m_SubColors[4];


	TRectEffectSpriteCreationStruct();

	void Load(TInfoNodeConstIterator InfoNode);
};

// -------------------
// Rect effect sprite
// -------------------
class TRectEffectSprite : public TSprite
{
private:
	bool m_bAllocated;

public:
	TD3DColor m_SubColors[4];


	static TSprite* Create(type_t tpType);

	TRectEffectSprite();

	~TRectEffectSprite()
		{ Release(); }

	bool IsAllocated() const
		{ return TSprite::IsAllocated() && m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(const TRectEffectSpriteCreationStruct& CreationStruct);

	void Load(TInfoNodeConstIterator InfoNode);

	void DrawNonScaled(	const FPOINT&			DstCoords,
						const TD3DColor&		Color	= WhiteColor(),
						const TSpriteStates&	States	= TSpriteStates()) const;

	void DrawRect(	const FRECT&			DstRect,
					const TD3DColor&		Color	= WhiteColor(),
					const TSpriteStates&	States	= TSpriteStates()) const;

	bool HasDefaultSize() const { return false; }

	void GetDefaultSize(FSIZE& RSize) const;

	TSprite *GetSubObject(size_t szIndex) { INITIATE_FAILURE; }

	const TSprite* GetSubObject(size_t szIndex) const { INITIATE_FAILURE; }

	size_t GetNSubObjects() const { return 0; }
};

#endif // effect_sprites_h
