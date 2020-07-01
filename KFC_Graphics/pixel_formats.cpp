#include "kfc_graphics_pch.h"
#include "pixel_formats.h"

#include "graphics_device_globals.h"

const TPixelFormatsInfo g_PixelFormatsInfo;

// -------------------
// Pixel formats info
// -------------------
size_t	TPixelFormatsInfo::s_PixelBytesLength	[MAX_D3D_PIXEL_FORMATS];
bool	TPixelFormatsInfo::s_IsRGBPixelFormat	[MAX_D3D_PIXEL_FORMATS];
bool	TPixelFormatsInfo::s_IsAlphaPixelFormat	[MAX_D3D_PIXEL_FORMATS];

TPixelFormatsInfo::TPixelFormatsInfo()
{
	memset(s_IsRGBPixelFormat,   0, sizeof(s_IsRGBPixelFormat));
	memset(s_IsAlphaPixelFormat, 0, sizeof(s_IsAlphaPixelFormat));

	// Pixel byte length
	s_PixelBytesLength[D3DFMT_R8G8B8]		= 3;
	s_PixelBytesLength[D3DFMT_A8R8G8B8]		= 4;
	s_PixelBytesLength[D3DFMT_X8R8G8B8]		= 4;
	s_PixelBytesLength[D3DFMT_R5G6B5]		= 2;
	s_PixelBytesLength[D3DFMT_X1R5G5B5]		= 2;
	s_PixelBytesLength[D3DFMT_A1R5G5B5]		= 2;
	s_PixelBytesLength[D3DFMT_A4R4G4B4]		= 2;
	s_PixelBytesLength[D3DFMT_R3G3B2]		= 1;
	s_PixelBytesLength[D3DFMT_A8]			= 1;
	s_PixelBytesLength[D3DFMT_A8R3G3B2]		= 2;
	s_PixelBytesLength[D3DFMT_X4R4G4B4]		= 2;
	s_PixelBytesLength[D3DFMT_A2B10G10R10]	= 4;
	s_PixelBytesLength[D3DFMT_A8B8G8R8]		= 4;
	s_PixelBytesLength[D3DFMT_X8B8G8R8]		= 4;
	s_PixelBytesLength[D3DFMT_G16R16]		= 4;
	s_PixelBytesLength[D3DFMT_A2R10G10B10]	= 4;
	s_PixelBytesLength[D3DFMT_A16B16G16R16]	= 8;
	s_PixelBytesLength[D3DFMT_A8P8]			= 1;
	s_PixelBytesLength[D3DFMT_P8]			= 1;
	s_PixelBytesLength[D3DFMT_L8]			= 1;
	s_PixelBytesLength[D3DFMT_L16]			= 2;
	s_PixelBytesLength[D3DFMT_A8L8]			= 2;
	s_PixelBytesLength[D3DFMT_A4L4]			= 1;

	// RGB
	s_IsRGBPixelFormat[D3DFMT_R8G8B8]		= true;
	s_IsRGBPixelFormat[D3DFMT_A8R8G8B8]		= true;
	s_IsRGBPixelFormat[D3DFMT_X8R8G8B8]		= true;
	s_IsRGBPixelFormat[D3DFMT_R5G6B5]		= true;
	s_IsRGBPixelFormat[D3DFMT_X1R5G5B5]		= true;
	s_IsRGBPixelFormat[D3DFMT_A1R5G5B5]		= true;
	s_IsRGBPixelFormat[D3DFMT_A4R4G4B4]		= true;
	s_IsRGBPixelFormat[D3DFMT_R3G3B2]		= true;
	s_IsRGBPixelFormat[D3DFMT_A8R3G3B2]		= true;
	s_IsRGBPixelFormat[D3DFMT_X4R4G4B4]		= true;
	s_IsRGBPixelFormat[D3DFMT_A2B10G10R10]	= true;
	s_IsRGBPixelFormat[D3DFMT_A8B8G8R8]		= true;
	s_IsRGBPixelFormat[D3DFMT_X8B8G8R8]		= true;
	s_IsRGBPixelFormat[D3DFMT_A2R10G10B10]	= true;
	s_IsRGBPixelFormat[D3DFMT_A16B16G16R16]	= true;
	s_IsRGBPixelFormat[D3DFMT_A8P8]			= true;
	s_IsRGBPixelFormat[D3DFMT_P8]			= true;

	// Alpha
	s_IsAlphaPixelFormat[D3DFMT_A8R8G8B8]		= true;
	s_IsAlphaPixelFormat[D3DFMT_A1R5G5B5]		= true;
	s_IsAlphaPixelFormat[D3DFMT_A4R4G4B4]		= true;
	s_IsAlphaPixelFormat[D3DFMT_A8]				= true;
	s_IsAlphaPixelFormat[D3DFMT_A8R3G3B2]		= true;
	s_IsAlphaPixelFormat[D3DFMT_A2B10G10R10]	= true;
	s_IsAlphaPixelFormat[D3DFMT_A8B8G8R8]		= true;
	s_IsAlphaPixelFormat[D3DFMT_A2R10G10B10]	= true;
	s_IsAlphaPixelFormat[D3DFMT_A16B16G16R16]	= true;
	s_IsAlphaPixelFormat[D3DFMT_A8P8]			= true;
	s_IsAlphaPixelFormat[D3DFMT_A8L8]			= true;
	s_IsAlphaPixelFormat[D3DFMT_A4L4]			= true;
}

// ----------------
// Global routines
// ----------------
D3DFORMAT GetDefaultImageFilePixelFormat(const KString& FileExtension, bool bTexture)
{
	if(bTexture)
	{
		if(FileExtension.CompareNoCase(TEXT("bmp")) == 0)
		{
			return g_GraphicsDeviceGlobals.m_DefaultTextureFormat;
		}
		else if(FileExtension.CompareNoCase(TEXT("jpg"))	== 0 ||
				FileExtension.CompareNoCase(TEXT("jpeg"))	== 0)
		{
			return g_GraphicsDeviceGlobals.m_DefaultTextureFormat;
		}
		else if(FileExtension.CompareNoCase(TEXT("tga")) == 0)
		{
			return g_GraphicsDeviceGlobals.m_DefaultAlphaTextureFormat;
		}
	}
	else
	{
		return g_GraphicsDeviceGlobals.m_ImageFormat;
	}

	return D3DFMT_UNKNOWN;
}

void FillD3DPixelBuffer(const TImage&	SrcImage,
						void*			pData,
						size_t			szPitch,
						D3DFORMAT		Format,
						const SZRECT*	pSrcRect,
						const SZSIZE*	pClampSize)
{
	DEBUG_VERIFY(SrcImage.IsAllocated());

	const SZSIZE& SrcImageSize = SrcImage.GetSize();

	// Temporary source rect
	SZRECT TempSrcRect;
	if(pSrcRect == NULL)
	{
		TempSrcRect.Set(0, 0, SrcImageSize.cx, SrcImageSize.cy);
		pSrcRect = &TempSrcRect;
	}

	DEBUG_VERIFY(pSrcRect->IsValid());

	DEBUG_VERIFY(pSrcRect->m_Right <= SrcImageSize.cx && pSrcRect->m_Bottom <= SrcImageSize.cy);

	// Source size
	const SZSIZE SrcSize = *pSrcRect;

	// Temporary clamp size
	SZSIZE TempClampSize;
	if(pClampSize == NULL)
	{
		TempClampSize = SrcSize;
		pClampSize = &TempClampSize;
	}
	else
	{
		DEBUG_VERIFY(!(*pClampSize < SrcSize));
	}

	// Copying pixel data
	const BYTE* pSrcData = (const BYTE*)SrcImage.GetDataPtr(SZPOINT(pSrcRect->m_Left,
																	pSrcRect->m_Top));

	size_t x, y;
	
	const size_t szSrcDelta		= (SrcImageSize.cx - SrcSize.cx) * sizeof(DWORD);
	const size_t szSrcBackDelta	= (SrcSize.cx * sizeof(DWORD));
	const size_t szDstDelta		= (szPitch - pClampSize->cx);

	const size_t szPixelSizeBytes = TPixelFormatsInfo::GetPixelBytesLength(Format);

	if(szPixelSizeBytes == 1)
	{
		BYTE* pDstData = (BYTE*)pData;
		
		if(Format == D3DFMT_A8)
		{
			for(y = 0 ; y < pClampSize->cy ; y++)
			{
				BYTE bValue;

				for(x = 0 ; x < pClampSize->cx ; x++)
				{
					if(x < SrcSize.cx)
					{
						pSrcData += 3;			// rgb
						bValue = *pSrcData++;	// alpha
					}

					*pDstData++ = bValue;
				}

				if(y < SrcSize.cy - 1)
					pSrcData += szSrcDelta;
				else
					pSrcData -= szSrcBackDelta;

				pDstData += szDstDelta;
			}
		}
		else if(Format == D3DFMT_R3G3B2)
		{
			for(y = 0 ; y < pClampSize->cy ; y++)
			{
				BYTE bValue;

				for(x = 0 ; x < pClampSize->cx ; x++)
				{
					if(x < SrcSize.cx)
					{
						bValue = 0;

						bValue |= (*pSrcData++ >> 6);		// blue
						bValue |= (*pSrcData++ >> 5) << 2;	// green
						bValue |= (*pSrcData++ >> 5) << 5;	// red
						pSrcData++;							// alpha
					}

					*pDstData++ = bValue;
				}

				if(y < SrcSize.cy)
					pSrcData += szSrcDelta;
				else
					pSrcData -= szSrcBackDelta;

				pDstData += szDstDelta;
			}
		}
		else
		{
			INITIATE_FAILURE;
		}
	}
	else if(szPixelSizeBytes == 2)
	{
		WORD* pDstData = (WORD*)pData;		

		if(Format == D3DFMT_A8R3G3B2)
		{
			for(y = 0 ; y < pClampSize->cy ; y++)
			{
				WORD wValue;

				for(x = 0 ; x < pClampSize->cx ; x++)
				{
					if(x < SrcSize.cx)
					{
						wValue = 0;

						wValue |= (*pSrcData++ >> 6);			// blue
						wValue |= (*pSrcData++ >> 5)	<< 2;	// green
						wValue |= (*pSrcData++ >> 5)	<< 5;	// red
						wValue |= (*pSrcData++)			<< 8;	// alpha
					}

					*pDstData++ = wValue;
				}

				if(y < SrcSize.cy - 1)
					pSrcData += szSrcDelta;
				else
					pSrcData -= szSrcBackDelta;

				pDstData += szDstDelta;
			}
		}
		else if(Format == D3DFMT_X4R4G4B4 || Format == D3DFMT_A4R4G4B4)
		{
			for(y = 0 ; y < pClampSize->cy ; y++)
			{
				WORD wValue;

				for(x = 0 ; x < pClampSize->cx ; x++)
				{
					if(x < SrcSize.cx)
					{
						wValue = 0;

						wValue |= (*pSrcData++ >> 4);		// blue
						wValue |= (*pSrcData++ >> 4) << 4;	// green
						wValue |= (*pSrcData++ >> 4) << 8;	// red
						wValue |= (*pSrcData++ >> 4) << 12;	// alpha
					}

					*pDstData++ = wValue;
				}

				if(y < SrcSize.cy - 1)
					pSrcData += szSrcDelta;
				else
					pSrcData -= szSrcBackDelta;

				pDstData += szDstDelta;
			}
		}
		else if(Format == D3DFMT_X1R5G5B5 || Format == D3DFMT_A1R5G5B5)
		{
			for(y = 0 ; y < pClampSize->cy ; y++)
			{
				WORD wValue;

				for(x = 0 ; x < pClampSize->cx ; x++)
				{
					if(x < SrcSize.cx)
					{
						wValue = 0;

						wValue |= (*pSrcData++ >> 3);		// blue
						wValue |= (*pSrcData++ >> 3) << 5;	// green
						wValue |= (*pSrcData++ >> 3) << 10;	// red
						wValue |= (*pSrcData++ >> 7) << 15;	// alpha
					}
						
					*pDstData++ = wValue;
				}

				if(y < SrcSize.cy - 1)
					pSrcData += szSrcDelta;
				else
					pSrcData -= szSrcBackDelta;
				
				pDstData += szDstDelta;
			}
		}
		else if(Format == D3DFMT_R5G6B5)
		{
			for(y = 0 ; y < pClampSize->cy ; y++)
			{
				WORD wValue;

				for(x = 0 ; x < pClampSize->cx ; x++)
				{
					if(x < SrcSize.cx)
					{
						wValue = 0;

						wValue |= (*pSrcData++ >> 3);		// blue
						wValue |= (*pSrcData++ >> 2) << 5;	// green
						wValue |= (*pSrcData++ >> 3) << 11;	// red
						pSrcData++;							// alpha
					}
					
					*pDstData++ = wValue;
				}

				if(y < SrcSize.cy - 1)
					pSrcData += szSrcDelta;
				else
					pSrcData -= szSrcBackDelta;

				pDstData += szDstDelta;
			}
		}
		else
		{
			INITIATE_FAILURE;
		}
	}
	else if(szPixelSizeBytes == 4)
	{
		DWORD* pDstData = (DWORD*)pData;

		if(Format == D3DFMT_X8R8G8B8 || Format == D3DFMT_A8R8G8B8)
		{
			for(y = 0 ; y < pClampSize->cy ; y++)
			{
				DWORD dwValue;

				for(x = 0 ; x < pClampSize->cx ; x++)
				{
					if(x < SrcSize.cx)
					{
						dwValue = *(DWORD*)pSrcData, pSrcData += sizeof(DWORD);
					}

					*pDstData++ = dwValue;
				}

				if(y < SrcSize.cy - 1)
					pSrcData += szSrcDelta;
				else
					pSrcData -= szSrcBackDelta;

				pDstData += szDstDelta;
			}
		}
		else
		{
			INITIATE_FAILURE;
		}
	}
	else
	{
		INITIATE_FAILURE;
	}
}

void RetrieveD3DPixelBuffer(TImage&			RImage,
							const void*		pData,
							size_t			szPitch,
							D3DFORMAT		Format)
{
	DEBUG_VERIFY(RImage.IsAllocated());
	
	const size_t szPixelSizeBytes = TPixelFormatsInfo::GetPixelBytesLength(Format);

	size_t x, y;

	BYTE* pDstData = (BYTE*)RImage.GetDataPtr();

	const size_t szSrcDelta = szPitch - RImage.GetSize().cx;

	if(szPixelSizeBytes == 1)
	{
		const BYTE* pSrcData = (const BYTE*)pData;

		if(Format == D3DFMT_A8)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ; x--)
				{
					*pDstData += 3; // rgb
					*pDstData++ = *pSrcData++; // alpha
				}

				pSrcData += szSrcDelta;
			}
		}
		else if(Format == D3DFMT_R3G3B2)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ; x--)
				{
					*pDstData++ = ((*pSrcData)		& 0x03)	* 255 / 3;	// blue
					*pDstData++ = ((*pSrcData >> 2)	& 0x07)	* 255 / 7;	// green
					*pDstData++ = ((*pSrcData >> 5))		* 255 / 7;	// red
					*pDstData++ = 0xFF;									// alpha

					pSrcData++;
				}

				pSrcData += szSrcDelta;
			}
		}
		else
		{
			INITIATE_FAILURE;
		}
	}
	else if(szPixelSizeBytes == 2)
	{
		const WORD* pSrcData = (const WORD*)pData;

		if(Format == D3DFMT_A8R3G3B2)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ; x--)
				{
					*pDstData++ = ((*pSrcData)		& 0x03) * 255 / 3;	// blue
					*pDstData++ = ((*pSrcData >> 2)	& 0x07) * 255 / 7;	// green
					*pDstData++ = ((*pSrcData >> 5)	& 0x07) * 255 / 7;	// red
					*pDstData++ = ((*pSrcData >> 8));					// alpha

					pSrcData++;
				}

				pSrcData += szSrcDelta;
			}
		}
		else if(Format == D3DFMT_X4R4G4B4)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ;  x--)
				{
					*pDstData++ = ((*pSrcData)			& 0x0F) * 255 / 15;	// blue
					*pDstData++ = ((*pSrcData >> 4)		& 0x0F) * 255 / 15;	// green
					*pDstData++ = ((*pSrcData >> 8)		& 0x0F) * 255 / 15;	// red
					*pDstData++ = 0xFF;										// alpha

					pSrcData++;
				}

				pSrcData += szSrcDelta;
			}
		}
		else if(Format == D3DFMT_A4R4G4B4)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ;  x--)
				{
					*pDstData++ = ((*pSrcData)			& 0x0F)	* 255 / 15; // blue
					*pDstData++ = ((*pSrcData >> 4)		& 0x0F)	* 255 / 15; // green
					*pDstData++ = ((*pSrcData >> 8)		& 0x0F)	* 255 / 15; // red
					*pDstData++ = ((*pSrcData >> 12))			* 255 / 15; // alpha

					pSrcData++;
				}

				pSrcData += szSrcDelta;
			}
		}
		else if(Format == D3DFMT_X1R5G5B5)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ; x--)
				{
					*pDstData++ = ((*pSrcData)			& 0x1F) * 255 / 31;	// blue
					*pDstData++ = ((*pSrcData >> 5)		& 0x1F) * 255 / 31;	// green
					*pDstData++ = ((*pSrcData >> 10)	& 0x1F) * 255 / 31;	// red
					*pDstData++ = 0xFF;										// alpha

					pSrcData++;
				}

				pSrcData += szSrcDelta;
			}
		}
		else if(Format == D3DFMT_A1R5G5B5)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ; x--)
				{
					*pDstData++ = ((*pSrcData)			& 0x1F)	* 255 / 31;	// blue
					*pDstData++ = ((*pSrcData >> 5)		& 0x1F)	* 255 / 31;	// green
					*pDstData++ = ((*pSrcData >> 10)	& 0x1F)	* 255 / 31;	// red
					*pDstData++ = ((*pSrcData >> 15))			* 255;		// alpha

					pSrcData++;
				}

				pSrcData += szSrcDelta;
			}
		}
		else if(Format == D3DFMT_R5G6B5)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ; x--)
				{
					*pDstData++ = ((*pSrcData)			& 0x1F)	* 255 / 31;	// blue
					*pDstData++ = ((*pSrcData >> 5)		& 0x3F)	* 255 / 63;	// green
					*pDstData++ = ((*pSrcData >> 11))			* 255 / 31;	// red
					*pDstData++ = 0xFF;										// alpha

					pSrcData++;
				}

				pSrcData += szSrcDelta;
			}
		}
		else
		{
			INITIATE_FAILURE;
		}
	}
	else if(szPixelSizeBytes == 4)
	{
		const DWORD* pSrcData = (const DWORD*)pData;
		
		if(Format == D3DFMT_X8R8G8B8)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				for(x = RImage.GetSize().cx ; x ; x--)
				{
					*(DWORD*)pDstData = *pSrcData++ | 0xFF000000; // rgba
					pDstData += sizeof(DWORD);
				}

				pSrcData += szSrcDelta;
			}
		}
		else if(Format == D3DFMT_A8R8G8B8)
		{
			for(y = RImage.GetSize().cy ; y ; y--)
			{
				memcpy(pDstData, pSrcData, RImage.GetSize().cx * sizeof(DWORD));
				pSrcData += szPitch, pDstData += RImage.GetSize().cx * sizeof(DWORD);
			}
		}
		else
		{
			INITIATE_FAILURE;
		}
	}
	else
	{
		INITIATE_FAILURE;
	}
}
