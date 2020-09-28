#ifndef _2d_sprites_h
#define _2d_sprites_h

#include <limits.h>
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include "sprite.h"
#include "texture_image.h"

// -------------------------------------
// Texture image sprite creation struct
// -------------------------------------
struct TTextureImageSpriteCreationStruct : public TSpriteCreationStruct
{
    KString m_FileName;


    TTextureImageSpriteCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);

    bool SetFileName(const KString& SFileName);
};

// ---------------------
// Texture image sprite
// ---------------------
class TTextureImageSprite : public TSprite
{
private:
    bool m_bAllocated;

    TTextureImage m_TextureImage;

public:
    static TSprite* Create(type_t tpType);

    TTextureImageSprite();

    ~TTextureImageSprite()
        { Release(); }

    bool IsAllocated() const
        { return TSprite::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(const TTextureImageSpriteCreationStruct& CreationSturct);

    void Load(TInfoNodeConstIterator InfoNode);

    void DrawNonScaled( const FPOINT&           DstCoords,
                        const TD3DColor&        Color   = WhiteColor(),
                        const TSpriteStates&    States  = TSpriteStates()) const;

    void DrawRect(  const FRECT&            DstRect,
                    const TD3DColor&        Color   = WhiteColor(),
                    const TSpriteStates&    States  = TSpriteStates()) const;

    bool HasDefaultSize() const;

    void GetDefaultSize(FSIZE& RSize) const;

    TSprite* GetSubObject(size_t szIndex) { INITIATE_FAILURE; }

    const TSprite* GetSubObject(size_t szIndex) const { INITIATE_FAILURE; }

    size_t GetNSubObjects() const { return 0; }
};

#endif // _2d_sprites_h
