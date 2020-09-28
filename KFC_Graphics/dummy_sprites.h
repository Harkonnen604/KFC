#ifndef dummy_sprites_h
#define dummy_sprites_h

#include "sprite.h"

// -----------------------------
// Dummy sprite creation struct
// -----------------------------
struct TDummySpriteCreationStruct : public TSpriteCreationStruct
{
    TDummySpriteCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);
};

// -------------
// Dummy sprite
// -------------
class TDummySprite : public TSprite
{
private:
    bool m_bAllocated;

public:
    static TSprite* Create(type_t tpType);

    TDummySprite();

    ~TDummySprite()
        { Release(); }

    bool IsAllocated() const
        { return TSprite::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(TDummySpriteCreationStruct& CreationStruct);

    void Load(TInfoNodeConstIterator InfoNode);

    void DrawNonScaled( const FPOINT&           DstCoords,
                        const TD3DColor&        Color   = WhiteColor(),
                        const TSpriteStates&    States  = TSpriteStates()) const;

    void DrawRect(  const FRECT&            DstRect,
                    const TD3DColor&        Color   = WhiteColor(),
                    const TSpriteStates&    States  = TSpriteStates()) const;

    bool HasDefaultSize() const;

    void GetDefaultSize(FSIZE& RSize) const;

    TSprite* GetSubObject(size_t szIndex);

    const TSprite* GetSubObject(size_t szIndex) const;

    size_t GetNSubObjects() const;
};

// -------------------------------------
// State skipper sprite creation struct
// -------------------------------------
struct TStateSkipperSpriteCreationStruct :
    public TSpriteCreationStruct

{
    size_t m_szNStates;


    TStateSkipperSpriteCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);
};

// --------------------------------------
// State skipper sprite sprites provider
// --------------------------------------
struct TStateSkipperSpriteSpritesProvider
{
    TObjectPointer<TSprite> m_Sprite;


    TStateSkipperSpriteSpritesProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// ---------------------
// State skipper sprite
// ---------------------
class TStateSkipperSprite : public TSprite
{
private:
    bool m_bAllocated;

public:
    size_t m_szNStates;

    TObjectPointer<TSprite> m_Sprite;


    static TSprite* Create(type_t tpType);

    TStateSkipperSprite();

    ~TStateSkipperSprite()
        { Release(); }

    bool IsAllocated() const
        { return TSprite::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  const TStateSkipperSpriteCreationStruct&    CreationStruct,
                    TStateSkipperSpriteSpritesProvider&         SpritesProvider);

    void Load(TInfoNodeConstIterator InfoNode);

    void DrawNonScaled( const FPOINT&           DstCoords,
                        const TD3DColor&        Color   = WhiteColor(),
                        const TSpriteStates&    States  = TSpriteStates()) const;

    void DrawRect(  const FRECT&            DstRect,
                    const TD3DColor&        Color   = WhiteColor(),
                    const TSpriteStates&    States  = TSpriteStates()) const;

    bool HasDefaultSize() const;

    void GetDefaultSize(FSIZE& RSize) const;

    TSprite* GetSubObject(size_t szIndex);

    const TSprite* GetSubObject(size_t szIndex) const;

    size_t GetNSubObjects() const;
};

#endif // dummy_sprites_h
