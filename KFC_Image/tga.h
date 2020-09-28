#ifndef tga_h
#define tga_h

// --------------
// TGA file head
// --------------
struct TGA_FILEHEAD
{
public:
    BYTE bIDLength;
    BYTE bColorMapType;
    BYTE bImageType;
    WORD wCMapStart;
    WORD wCMapLength;
    BYTE bCMapDepth;
    WORD wXOffset;
    WORD wYOffset;
    WORD wWidth;
    WORD wHeight;
    BYTE bPixelDepth;
    BYTE bImageDescription;

public:
    void Load(TStream& Stream);
    void Save(TStream& Stream) const;
};

inline TStream& operator >> (TStream& Stream, TGA_FILEHEAD& RFileHead)
{
    return RFileHead.Load(Stream), Stream;
}

inline TStream& operator << (TStream& Stream, const TGA_FILEHEAD& FileHead)
{
    return FileHead.Save(Stream), Stream;
}

#endif // tga_h
