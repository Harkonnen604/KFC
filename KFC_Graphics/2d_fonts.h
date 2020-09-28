#ifndef _2d_fonts
#define _2d_fonts

#include <KFC_KTL\object_pointer.h>
#include <KFC_Common\image.h>
#include <KFC_GUI\gui_font.h>
#include "font.h"
#include "texture.h"

// ---------------
// 2D font source
// ---------------
struct T2DFontSource
{
    KString                     m_FileName; // w/o extension
    TObjectPointer<TGUIFont>    m_GUIFont;


    T2DFontSource();

    void Load(  TInfoNodeConstIterator  InfoNode,
                const KString&          NamePrefix = TEXT(""),  // trailing space should be added
                LPCTSTR                 pValueName = TEXT("")); // trailing space should be added

    bool SetFileName(const KString& SFileName);
};

// -----------
// Font image
// -----------
class TFontImage : public TImage
{
private:
    bool m_bAllocated;

    FSIZE m_CharSizes[256];
    FSIZE m_MaxCharSize;

public:
    TFontImage();

    ~TFontImage()
        { Release(); }

    bool IsAllocated() const
        { return TImage::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(const T2DFontSource& Source);

    // ---------------- TRIVIALS ----------------
    const FSIZE* GetCharSizes   () const { return m_CharSizes;      }
    const FSIZE& GetMaxCharSize () const { return m_MaxCharSize;    }
};

// -----------------------------
// Texture font creation struct
// -----------------------------
struct TTextureFontCreationStruct : public TFontCreationStruct
{
    T2DFontSource m_Source;


    TTextureFontCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);
};

// -------------
// Texture font
// -------------
class TTextureFont : public TFont
{
private:
    bool m_bAllocated;

    TTexture m_Texture;

public:
    static TFont* Create(type_t tpType);

    TTextureFont();

    ~TTextureFont()
        { Release(); }

    bool IsAllocated() const
        { return TFont::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(const TTextureFontCreationStruct& CreationStruct);

    void Load(TInfoNodeConstIterator InfoNode);

    void DrawChar(  TCHAR               cChar,
                    FPOINT              DstCoords,
                    const TD3DColor&    Color = WhiteColor()) const;

    void DrawText(  const KString&      Text,
                    FPOINT              DstCoords,
                    const TD3DColor&    Color = WhiteColor()) const;

    // ---------------- TRIVIALS ----------------
    TTexture&       GetTexture()        { return m_Texture; }
    const TTexture& GetTexture() const  { return m_Texture; }
};

// ------------------------------------
// Texture shadow font creation struct
// ------------------------------------
struct TTextureShadowFontCreationStruct : public TFontCreationStruct
{
    T2DFontSource m_BaseSource;
    T2DFontSource m_ShadowSource;

    TD3DColor   m_ShadowColor;
    FSIZE       m_ShadowOffset;


    TTextureShadowFontCreationStruct();

    void Load(TInfoNodeConstIterator InfoNode);
};

// --------------------
// Texture shadow font
// --------------------
class TTextureShadowFont : public TFont
{
    bool m_bAllocated;

    TTexture m_BaseTexture;
    TTexture m_ShadowTexture;

    FSIZE m_ShadowOffset;

    FSIZE m_ShadowCharSizes[256];
    FSIZE m_MaxShadowCharSize;

public:
    TD3DColor m_ShadowColor;


    static TFont* Create(type_t tpType);

    TTextureShadowFont();

    ~TTextureShadowFont()
        { Release(); }

    bool IsAllocated() const
        { return TFont::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(const TTextureShadowFontCreationStruct& CreationStruct);

    void Load(TInfoNodeConstIterator InfoNode);

    void DrawChar(  TCHAR               cChar,
                    FPOINT              DstCoords,
                    const TD3DColor&    Color = WhiteColor()) const;

    void DrawText(  const KString&      Text,
                    FPOINT              DstCoords,
                    const TD3DColor&    Color = WhiteColor()) const;

    // ---------------- TRIVIALS ----------------
    TTexture&       GetBaseTexture()        { return m_BaseTexture; }
    const TTexture& GetBaseTexture() const  { return m_BaseTexture; }

    TTexture&       GetShadowTexture()          { return m_ShadowTexture; }
    const TTexture& GetShadowTexture() const    { return m_ShadowTexture; }

    const FSIZE& GetShadowOffset() const { return m_ShadowOffset; }

    const FSIZE* GetShadowCharSizes     () const { return m_ShadowCharSizes;    }
    const FSIZE& GetMaxShadowCharSize   () const { return m_MaxShadowCharSize;  }
};

#endif // _2d_fonts
