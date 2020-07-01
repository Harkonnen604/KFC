#ifndef pixel_formats_h
#define pixel_formats_h

#include "d3d_inc.h"
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\kstring.h>
#include <KFC_Common\image.h>
#include "graphics_limits.h"

// -------------------
// Pixel formats info
// -------------------
class TPixelFormatsInfo
{
private:
	static size_t	s_PixelBytesLength	[MAX_D3D_PIXEL_FORMATS];
	static bool		s_IsRGBPixelFormat	[MAX_D3D_PIXEL_FORMATS];
	static bool		s_IsAlphaPixelFormat[MAX_D3D_PIXEL_FORMATS];

public:
	TPixelFormatsInfo();

	static bool IsDXTFormat(D3DFORMAT Format)
	{
		return	Format == D3DFMT_DXT1 ||
				Format == D3DFMT_DXT2 ||
				Format == D3DFMT_DXT3 ||
				Format == D3DFMT_DXT4 ||
				Format == D3DFMT_DXT5;
	}

	static size_t GetPixelBytesLength(D3DFORMAT Format)
	{
		DEBUG_VERIFY(Format < MAX_D3D_PIXEL_FORMATS);

		return s_PixelBytesLength[(size_t)Format];
	}

	static bool IsRGBPixelFormat(D3DFORMAT Format)
	{
		return	Format < MAX_D3D_PIXEL_FORMATS ?
					s_IsRGBPixelFormat[(size_t)Format] :
					IsDXTFormat(Format);
	}

	static bool IsAlphaPixelFormat(D3DFORMAT Format)
	{
		return	Format < MAX_D3D_PIXEL_FORMATS ?
					s_IsAlphaPixelFormat[(size_t)Format] :
					IsDXTFormat(Format);
	}
};

extern const TPixelFormatsInfo g_PixelFormatsInfo;

// ----------------
// Global routines
// ----------------
D3DFORMAT GetDefaultImageFilePixelFormat(	const KString&	FileExtension,
											bool			bTexture = true);

void FillD3DPixelBuffer(const TImage&	SrcImage,
						void*			pData,
						size_t			szPitch,
						D3DFORMAT		Format,
						const SZRECT*	pSrcRect	= NULL,
						const SZSIZE*	pClampSize	= NULL);

void RetrieveD3DPixelBuffer(TImage&			RImage,
							const void*		pData,
							size_t			szPitch,
							D3DFORMAT		Format);


#endif // pixel_formats_h
