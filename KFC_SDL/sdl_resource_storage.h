#ifndef sdl_resource_storage_h
#define sdl_resource_storage_h

#include "sge_font.h"
#include "sdl_image.h"
#include "sdl_interface.h"

// -------------------
// SDL resouce ID map
// -------------------
class T_SDL_ResourceID_Map
{
private:
    // Storage
    typedef TTire<size_t> TStorage;

private:
    TStorage m_Storage;

public:
    T_SDL_ResourceID_Map() {}

    T_SDL_ResourceID_Map(LPCTSTR pFileName)
        { Parse(pFileName); }

    void Parse(LPCTSTR pFileName);

    void Clear()
        { m_Storage.Clear(); }

    void Add(LPCTSTR pName, size_t szValue)
    {
        DEBUG_VERIFY(*pName != '[');

        bool bNew;

        TStorage::TIterator Iter = m_Storage.Add(pName, bNew);

        if(!bNew)
            INITIATE_DEFINED_FAILURE((KString)"Duplicate SDL resource ID definition for \"" + pName + "\".");

        *Iter = szValue;
    }

    size_t operator [] (LPCTSTR pName) const
    {
        if(!*pName)
            return UINT_MAX;

        TStorage::TConstIterator Iter;

        if(*pName == '[')
        {
            size_t szLength = strlen(pName);

            if(szLength < 2 || pName[szLength - 1] != ']')
                INITIATE_DEFINED_FAILURE((KString)"Malformed SDL resource ID definition: \"" + pName + "\" not found.");

            Iter = m_Storage.Find(pName + 1, szLength - 2);
        }
        else
        {
            Iter = m_Storage.Find(pName);
        }

        if(!Iter.IsValid())
            INITIATE_DEFINED_FAILURE((KString)"SDL resource ID definition for \"" + pName + "\" not found.");

        return *Iter;
    }

    void TestDupes() const;
};

// ---------------------
// SDL resource storage
// ---------------------
class T_SDL_ResourceStorage
{
private:
    // Cached font
    struct TCachedFont
    {
    public:
        // Key
        struct TKey
        {
        public:
            KString m_FileName;
            size_t  m_szSize;

        public:
            TKey(LPCTSTR pFileName, size_t szSize) :
              m_FileName(pFileName), m_szSize(szSize) {}
        };

    public:
        KString m_FileName;
        size_t  m_szSize;

        T_SGE_Font m_Font;

    public:
        TCachedFont(const TKey& Key) :
          m_FileName(Key.m_FileName), m_szSize(Key.m_szSize)
          {
              m_Font.Load(m_FileName, m_szSize);
          }
    };

    friend inline int Compare(const TCachedFont& Image, const TCachedFont::TKey& Key);

    // Cached image
    struct TCachedImage
    {
    public:
        // Key
        struct TKey
        {
        public:
            KString m_FileName;
            UINT32  m_uiColorKey;

        public:
            TKey(LPCTSTR pFileName, UINT32 uiColorKey) :
                m_FileName(pFileName), m_uiColorKey(uiColorKey) {}
        };

    public:
        KString m_FileName;
        UINT32  m_uiColorKey;

        T_SDL_Image m_Image;

    public:
        TCachedImage(const TKey& Key) :
            m_FileName(Key.m_FileName), m_uiColorKey(Key.m_uiColorKey)
        {
            m_Image.Load(m_FileName);

            if(m_uiColorKey != UINT_MAX)
                m_Image.SetColorKey(m_uiColorKey);
        }
    };

    friend inline int Compare(const TCachedImage& Image, const TCachedImage::TKey& Key);

    // Cached multi-image
    struct TCachedMultiImage
    {
    public:
        // Key
        struct TKey
        {
        public:
            KString m_FileName;
            bool    m_bSingle;

        public:
            TKey(LPCTSTR pFileName, bool bSingle) :
                m_FileName(pFileName), m_bSingle(bSingle) {}
        };

    public:
        KString m_FileName;
        bool    m_bSingle;

        T_SDL_MultiImage m_Image;

    public:
        TCachedMultiImage(const TKey& Key) :
            m_FileName(Key.m_FileName), m_bSingle(Key.m_bSingle)
        {
            m_Image.Load(m_FileName, m_bSingle);
        }
    };

    friend inline int Compare(const TCachedMultiImage& Image, const TCachedMultiImage::TKey& Key);

private:
    static T_SDL_InterfaceDefinition ms_EmptyInterfaceDef;

private:
    T_AVL_Storage<TCachedFont>          m_FontsCache;
    T_AVL_Storage<TCachedImage>         m_ImagesCache;
    T_AVL_Storage<TCachedMultiImage>    m_MultiImagesCache;

    mutable TArray<double, true>            m_StateValues;
    TArray<double, true>                    m_Values;
    TArray<UINT32, true>                    m_Colors;
    TArray<KString>                         m_Strings;
    TArray<const T_SGE_Font*,       true>   m_Fonts;
    TArray<const T_SDL_Image*,      true>   m_Images;
    TArray<const T_SDL_MultiImage*, true>   m_MultiImages;
    TArray<T_SDL_InterfaceDefinition>       m_InterfaceDefs;

private:
    const T_SGE_Font* LoadFont( LPCTSTR         pString,
                                const TTokens&  ValueTokens,
                                const TTokens&  ColorTokens,
                                const TTokens&  StringTokens);

    const T_SDL_Image* LoadImage(   LPCTSTR         pString,
                                    const TTokens&  ValueTokens,
                                    const TTokens&  ColorTokens,
                                    const TTokens&  StringTokens);

    const T_SDL_MultiImage* LoadMultiImage( LPCTSTR         pString,
                                            const TTokens&  ValueTokens,
                                            const TTokens&  ColorTokens,
                                            const TTokens&  StringTokens);

public:
    T_SDL_ResourceStorage() {}

    T_SDL_ResourceStorage(LPCTSTR pFileName)
        { Load(pFileName); }

    void Clear();

    void Load(LPCTSTR pFileName);

    double& GetStateValue(size_t szID) const
    {
        KFC_VERIFY(szID < m_StateValues.GetN());

        return m_StateValues[szID];
    }

    double GetValue(size_t szID) const
    {
        if(szID == UINT_MAX)
            return 0;

        KFC_VERIFY(szID < m_Values.GetN());

        return m_Values[szID];
    }

    UINT32 GetColor(size_t szID) const
    {
        if(szID == UINT_MAX)
            return UINT_MAX;

        KFC_VERIFY(szID < m_Colors.GetN());

        return m_Colors[szID];
    }

    const KString& GetString(size_t szID) const
    {
        if(szID == UINT_MAX)
            return g_EmptyString;

        KFC_VERIFY(szID < m_Strings.GetN());

        return m_Strings[szID];
    }

    const T_SGE_Font* GetFont(size_t szID) const
    {
        if(szID == UINT_MAX)
            return NULL;

        KFC_VERIFY(szID < m_Fonts.GetN() && m_Fonts[szID]);

        return m_Fonts[szID];
    }

    const T_SDL_Image* GetImage(size_t szID) const
    {
        if(szID == UINT_MAX)
            return NULL;

        KFC_VERIFY(szID < m_Images.GetN() && m_Images[szID]);

        return m_Images[szID];
    }

    const T_SDL_MultiImage* GetMultiImage(size_t szID) const
    {
        if(szID == UINT_MAX)
            return NULL;

        KFC_VERIFY(szID < m_MultiImages.GetN() && m_MultiImages[szID]);

        return m_MultiImages[szID];
    }

    const T_SDL_InterfaceDefinition& GetInterfaceDef(size_t szID) const
    {
        if(szID == UINT_MAX)
            return ms_EmptyInterfaceDef;

        KFC_VERIFY(szID < m_InterfaceDefs.GetN());

        return m_InterfaceDefs[szID];
    }
};

inline int Compare(const T_SDL_ResourceStorage::TCachedFont& Font, const T_SDL_ResourceStorage::TCachedFont::TKey& Key)
{
    int d;

    if(d = Compare(Font.m_FileName, Key.m_FileName))
        return d;

    if(d = Compare(Font.m_szSize, Key.m_szSize))
        return d;

    return 0;
}

inline int Compare(const T_SDL_ResourceStorage::TCachedImage& Image, const T_SDL_ResourceStorage::TCachedImage::TKey& Key)
{
    int d;

    if(d = Compare(Image.m_FileName, Key.m_FileName))
        return d;

    if(d = Compare(Image.m_uiColorKey, Key.m_uiColorKey))
        return d;

    return 0;
}

inline int Compare(const T_SDL_ResourceStorage::TCachedMultiImage& Image, const T_SDL_ResourceStorage::TCachedMultiImage::TKey& Key)
{
    int d;

    if(d = Compare(Image.m_FileName, Key.m_FileName))
        return d;

    if(d = Compare(Image.m_bSingle, Key.m_bSingle))
        return d;

    return 0;
}

#endif // sdl_resource_storage_h
