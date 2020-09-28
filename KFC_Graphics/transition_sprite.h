#ifndef transition_sprite_h
#define transition_sprite_h

#include <limits.h>
#include <KFC_KTL\object_pointer.h>
#include "compound_sprite.h"

// ---------------------------------
// Transition sub-sprite blend mode
// ---------------------------------
enum TTransitionSubSpriteBlendMode
{
    TSSBM_BLEND         = 0,
    TSSBM_TRANSPARENT   = 1,
    TSSBM_OPAQUE        = 2,
    TSSBM_FORCE_UINT    = UINT_MAX,
};

// ----------------------------------
// Transition sprite creation struct
// ----------------------------------
struct TTransitionSpriteCreationStruct : public TCompoundSpriteCreationStruct
{
    TTransitionSubSpriteBlendMode m_FloorSubSpriteBlendMode;
    TTransitionSubSpriteBlendMode m_CeilSubSpriteBlendMode;


    TTransitionSpriteCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);
};

// -----------------------------------
// Transition sprite sprites provider
// -----------------------------------
struct TTransitionSpriteSpritesProvider : public TCompoundSpriteSpritesProvider
{
    TTransitionSpriteSpritesProvider();

    void Load(  TInfoNodeConstIterator  InfoNode,
                size_t                  szNItems);
};

// ------------------
// Transition sprite
// ------------------
class TTransitionSprite : public TCompoundSprite
{
private:
    bool m_bAllocated;

public:
    TTransitionSubSpriteBlendMode m_FloorSubSpriteBlendMode;
    TTransitionSubSpriteBlendMode m_CeilSubSpriteBlendMode;


    static TSprite* Create(type_t tpType);

    TTransitionSprite();

    ~TTransitionSprite()
        { Release(); }

    bool IsAllocated() const
        { return TCompoundSprite::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  const TTransitionSpriteCreationStruct&  CreationStruct,
                    TTransitionSpriteSpritesProvider&       SpritesProvider);

    void Load(TInfoNodeConstIterator InfoNode);

    void DrawRect(  const FRECT&            DstRect,
                    const TD3DColor&        Color   = WhiteColor(),
                    const TSpriteStates&    States  = TSpriteStates()) const;

    void DrawNonScaled( const FPOINT&           DstCoords,
                        const TD3DColor&        Color   = WhiteColor(),
                        const TSpriteStates&    States  = TSpriteStates()) const;
};

// ----------------
// Global routines
// ----------------
bool FromString(const KString&                  String,
                TTransitionSubSpriteBlendMode&  RBlendMode);

void ReadTransitionSubSpriteBlendMode(  const KString&                  String,
                                        TTransitionSubSpriteBlendMode&  RBlendMode,
                                        LPCTSTR                         pValueName);

#endif // transition_sprite_h
