#ifndef sge_font_h
#define sge_font_h

// ---------
// SGE font
// ---------
class T_SGE_Font
{
private:
    sge_TTFont* m_pFont;

private:
    T_SGE_Font(const T_SGE_Font&);

    T_SGE_Font& operator = (const T_SGE_Font& Font);

public:
    T_SGE_Font();

    T_SGE_Font(LPCTSTR pFileName, size_t szSize);

    ~T_SGE_Font()
        { Release(); }

    bool IsAllocated() const
        { return m_pFont; }

    void Release();

    void Load(LPCTSTR pFileName, size_t szSize);

    sge_TTFont* GetFont() const
        { DEBUG_VERIFY_ALLOCATION; return m_pFont; }

    operator sge_TTFont* () const
        { return GetFont(); }
};

#endif // sge_font_h
