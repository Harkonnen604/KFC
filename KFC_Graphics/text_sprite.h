#ifndef text_sprite_h
#define text_sprite_h

#include <KFC_KTL\text_container.h>
#include "sprite.h"
#include "font.h"
#include "text_params.h"

// ----------------------------
// Text sprite creation struct
// ----------------------------
struct TTextSpriteCreationStruct :  public TSpriteCreationStruct,
                                    public TTextContainer
{
    TTextParams m_TextParams;


    TTextSpriteCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);

    KString GetText(bool* pRSuccess = NULL) const;

    bool SetText(const KString& Text);
};

// ---------------------------
// Text sprite fonts provider
// ---------------------------
struct TTextSpriteFontsProvider
{
    TObjectPointer<TFont> m_Font;


    TTextSpriteFontsProvider();

    void Load(TInfoNodeConstIterator InfoNode);
};

// ------------
// Text sprite
// ------------
class TTextSprite : public TSprite,
                    public TTextContainer
{
private:
    bool m_bAllocated;

    TTextParams             m_TextParams;
    TObjectPointer<TFont>   m_Font;

    mutable FSIZE m_TextSize;


    void InternalUpdateTextSize();

public:
    static TSprite* Create(type_t tpType);

    TTextSprite();

    ~TTextSprite()
        { Release(); }

    bool IsAllocated() const
        { return TSprite::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  const TTextSpriteCreationStruct&    CreationStruct,
                    TTextSpriteFontsProvider&           FontsProvider);

    void Load(TInfoNodeConstIterator InfoNode);

    void DrawNonScaled( const FPOINT&           DstCoords,
                        const TD3DColor&        Color   = WhiteColor(),
                        const TSpriteStates&    States  = TSpriteStates()) const;

    void DrawRect(  const FRECT&            DstRect,
                    const TD3DColor&        Color   = WhiteColor(),
                    const TSpriteStates&    States  = TSpriteStates()) const;

    bool HasDefaultSize() const;

    void GetDefaultSize(FSIZE& RSize) const;

    void SetTextParams(const TTextParams& STextParams);

    KString GetText(bool* pRSuccess = NULL) const;

    bool SetText(const KString& Text);

    void SetTextAlignment   (const ALSIZE&          Alignment);
    void SetTextFlags       (flags_t                flFlags);
    void SetTextFont        (TObjectPointer<TFont>& Font);

    TSprite* GetSubObject(size_t szIndex) { INITIATE_FAILURE; }

    const TSprite* GetSubObject(size_t szIndex) const { INITIATE_FAILURE; }

    size_t GetNSubObjects() const { return 0; }

    // ---------------- TRIVIALS ----------------
    const TTextParams& GetTextParams() const { return m_TextParams; }

    const TFont* GetFont() const { return m_Font.GetDataPtr(); }
};

#endif // text_sprite_h
