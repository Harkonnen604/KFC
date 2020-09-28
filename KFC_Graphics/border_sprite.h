#ifndef border_sprite_h
#define border_sprite_h

#include <KFC_KTL\object_pointer.h>
#include "sprite.h"

// ------------------------
// Border sprite placement
// ------------------------
enum TBorderSpritePlacement
{
    BSP_INSIDE      = 0,
    BSP_OUTSIDE     = 1,
    BSP_FORCE_UINT  = UINT_MAX,
};

// ------------------------------
// Border sprite creation struct
// ------------------------------
struct TBorderSpriteCreationStruct : public TSpriteCreationStruct
{
    TBorderSpritePlacement m_Placement;


    TBorderSpriteCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);
};

// -------------------------------
// Border sprite sprites provider
// -------------------------------
struct TBorderSpriteSpritesProvider
{
    TObjectPointer<TSprite> m_Sprites[3][3];


    TBorderSpriteSpritesProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// --------------
// Border sprite
// --------------
class TBorderSprite : public TSprite
{
private:
    bool m_bAllocated;

    TObjectPointer<TSprite> m_Sprites[3][3];

    TBorderSpritePlacement m_Placement;

public:
    static TSprite* Create(type_t tpType);

    TBorderSprite();

    ~TBorderSprite() { Release(); }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  const TBorderSpriteCreationStruct&  CreationStruct,
                    TBorderSpriteSpritesProvider&       SpritesProvider);

    void Load(TInfoNodeConstIterator InfoNode);

    void DrawNonScaled( const FPOINT&           DstCoords,
                        const TD3DColor&        Color   = WhiteColor(),
                        const TSpriteStates&    States  = TSpriteStates()) const;

    void DrawRect(  const FRECT&            DstRect,
                    const TD3DColor&        Color   = WhiteColor(),
                    const TSpriteStates&    States  = TSpriteStates()) const;

    bool HasDefaultSize() const;

    void GetDefaultSize(FSIZE& RSize) const;

    TSprite* GetSubObject(size_t szIndex)
        { return m_Sprites[szIndex / 3][szIndex % 3].GetDataPtr(); }

    const TSprite* GetSubObject(size_t szIndex) const
        { return m_Sprites[szIndex / 3][szIndex % 3].GetDataPtr(); }

    size_t GetNSubObjects() const { return 3 * 3; }

    // ---------------- TRIVIALS ----------------
    TBorderSpritePlacement GetPlacement() const { return m_Placement; }
};

// ----------------
// Global routines
// ----------------
bool FromString(const KString&          String,
                TBorderSpritePlacement& RPlacement);

void ReadBorderSpritePlacement( const KString&          String,
                                TBorderSpritePlacement& RPlacement,
                                LPCTSTR                 pValueName);

#endif // border_sprite_h
