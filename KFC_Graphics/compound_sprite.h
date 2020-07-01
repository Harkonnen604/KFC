#ifndef compound_sprite_h
#define compound_sprite_h

#include <limits.h>
#include <KFC_KTL\object_pointer.h>
#include "sprite.h"

// ---------------------------
// Compound sprite omittables
// ---------------------------
enum TCompoundSpriteOmitMode
{
	CSOM_NONE		= 0,
	CSOM_TAKE_PREV	= 1,
	CSOM_FORCE_UINT	= UINT_MAX,
};

// --------------------------------
// Compound sprite item parameters
// --------------------------------
struct TCompoundSpriteItemParameters
{
	TD3DColor	m_Color;
	FSIZE		m_Offset;
	

	TCompoundSpriteItemParameters();
};

// --------------------------------
// Compound sprite creation struct
// --------------------------------
struct TCompoundSpriteCreationStruct : public TSpriteCreationStruct
{
	size_t									m_szNItems;
	TArray<TCompoundSpriteItemParameters>	m_Parameters;


	TCompoundSpriteCreationStruct();

	void Load(TInfoNodeConstIterator InfoNode);

	void SetNItems(size_t szSNItems);
};

// ---------------------------------
// Compound sprite sprites provider
// ---------------------------------
struct TCompoundSpriteSpritesProvider
{
	TArray< TObjectPointer<TSprite> > m_Sprites;


	TCompoundSpriteSpritesProvider();

	void Load(	TInfoNodeConstIterator	InfoNode,
				size_t					szNItems,
				TCompoundSpriteOmitMode	OmitMode);

	void SetNItems(size_t szNItems);
};

// ----------------
// Compound sprite
// ----------------
class TCompoundSprite : public TSprite
{
private:
	bool m_bAllocated;

	bool	m_bHasDefaultSize;
	FSIZE	m_DefaultSize;

public:	

	struct TItem
	{
		TObjectPointer<TSprite>			m_Sprite;
		TCompoundSpriteItemParameters	m_Parameters;
	};

	typedef TArray<TItem> TItems;

private:
	TItems m_Items;

public:
	TCompoundSprite();

	virtual ~TCompoundSprite()
		{ Release(); }

	bool IsAllocated() const
		{ return TSprite::IsAllocated() && m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(	const TCompoundSpriteCreationStruct&	CreationStruct,
					TCompoundSpriteSpritesProvider&			SpritesProvider,
					TCompoundSpriteOmitMode					OmitMode);


	bool HasDefaultSize() const;

	void GetDefaultSize(FSIZE& RSize) const;

	void UpdateDefaultSize();

	TSprite* GetSubObject(size_t szIndex) { return m_Items[szIndex].m_Sprite.GetDataPtr(); }

	const TSprite* GetSubObject(size_t szIndex) const { return m_Items[szIndex].m_Sprite.GetDataPtr(); }

	size_t GetNSubObjects() const { return m_Items.GetN(); }

	// ---------------- TRIVIALS ----------------	
	TItems& GetItems() { return m_Items; }

	const TItems& GetItems() const { return m_Items; }
};

#endif // compound_sprite_h
