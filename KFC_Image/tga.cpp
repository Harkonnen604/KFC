#include "kfc_image_pch.h"
#include "tga.h"

// --------------
// TGA file head
// --------------
void TGA_FILEHEAD::Load(TStream& Stream)
{
    Stream >> bIDLength;
    Stream >> bColorMapType;
    Stream >> bImageType;
    Stream >> wCMapStart    >> wCMapLength >> bCMapDepth;
    Stream >> wXOffset  >> wYOffset;
    Stream >> wWidth        >> wHeight;
    Stream >> bPixelDepth;
    Stream >> bImageDescription;
}

void TGA_FILEHEAD::Save(TStream& Stream) const
{
    Stream << bIDLength;
    Stream << bColorMapType;
    Stream << bImageType;
    Stream << wCMapStart    << wCMapLength << bCMapDepth;
    Stream << wXOffset  << wYOffset;
    Stream << wWidth        << wHeight;
    Stream << bPixelDepth;
    Stream << bImageDescription;
}
