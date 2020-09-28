#ifndef sdl_image_h
#define sdl_image_h

// ----------
// SDL image
// ----------
class T_SDL_Image
{
private:
    SDL_Surface* m_pSurface;

    UINT32 m_uiColorKey;

public:
    T_SDL_Image();

    T_SDL_Image(const T_SDL_Image& Image);

    T_SDL_Image(LPCTSTR pFileName);

    T_SDL_Image(const SZSIZE& Size);

    ~T_SDL_Image()
        { Release(); }

    bool IsAllocated() const
        { return m_pSurface; }

    void Release();

    void Create(const SZSIZE& Size);

    void Load(LPCTSTR pFileName);

    const SDL_PixelFormat* GetFormat() const
        { DEBUG_VERIFY_ALLOCATION; return m_pSurface->format; }

    UINT GetColorKey() const
        { DEBUG_VERIFY_ALLOCATION; return m_uiColorKey; }

    bool HasColorKey() const
        { DEBUG_VERIFY_ALLOCATION; return m_uiColorKey != UINT_MAX; }

    void SetColorKey(UINT32 uiColorKey);

    T_SDL_Image& operator = (const T_SDL_Image& Image);

    size_t GetWidth() const
        { DEBUG_VERIFY_ALLOCATION; return m_pSurface->w; }

    size_t GetHeight() const
        { DEBUG_VERIFY_ALLOCATION; return m_pSurface->h; }

    SZSIZE GetSize() const
        { DEBUG_VERIFY_ALLOCATION; return SZSIZE(m_pSurface->w, m_pSurface->h); }

    SDL_Surface* GetSurface() const
        { DEBUG_VERIFY_ALLOCATION; return m_pSurface; }

    operator SDL_Surface* () const
        { return GetSurface(); }
};

// ----------------
// SDL multi-image
// ----------------
class T_SDL_MultiImage
{
private:
    T_SDL_Image m_Images[3][3];

public:
    T_SDL_MultiImage();

    T_SDL_MultiImage(LPCTSTR pFileName, bool bSingle = false);

    ~T_SDL_MultiImage()
        { Release(); }

    bool IsAllocated() const
        { return m_Images[1][1].IsAllocated(); }

    void Release();

    bool IsSingle() const
        { return !m_Images[0][0].IsAllocated(); }

    SZSIZE GetSingleSize() const
    {
        DEBUG_VERIFY_ALLOCATION;
        DEBUG_VERIFY(IsSingle());

        return m_Images[1][1].GetSize();
    }

    void Load(LPCTSTR pFileName, bool bSingle = false);

    void Draw(SDL_Surface* pSurface, const IRECT& Rect, bool bUpdate = true) const;
};

#endif // sdl_image_h
