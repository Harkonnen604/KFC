#include "kfc_image_pch.h"
#include "image.h"

#include <KFC_Common/file.h>
#include "rgb.h"
#include "bmp.h"
#include "tga.h"

// ------
// Image
// ------
TImage::TImage()
{
	m_bAllocated = false;
}

TImage::TImage(const SZSIZE& SSize)
{
	m_bAllocated = false;

	Allocate(SSize);
}

TImage::TImage(const KString& FileName)
{
	m_bAllocated = false;

	Load(FileName);
}

TImage::TImage(const TImage& Image, const SZRECT& Rect)
{
	m_bAllocated = false;

	CreateCropped(Image, Rect);
}

TImage::TImage(const TImage& Image, const SZSIZE& Size)
{
	m_bAllocated = false;

	CreateResized(Image, Size);
}

void TImage::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		m_Pixels.Release();
	}
}

TImage& TImage::Allocate(const SZSIZE& SSize)
{
	if(IsAllocated() && GetSize() == SSize)
		return *this;

	Release();

	try
	{
		DEBUG_VERIFY(!SSize.IsFlat());

		m_Size = SSize;

		m_Pixels.Allocate(m_Size);

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}

	return *this;
}

TImage& TImage::CreateCropped(const TImage& SrcImage, const SZRECT& Rect)
{
	Release();

	try
	{
		DEBUG_VERIFY(SrcImage.IsAllocated());

		DEBUG_VERIFY(Rect.IsValid());

		DEBUG_VERIFY(Rect.m_Right <= SrcImage.GetSize().cx && Rect.m_Bottom <= SrcImage.GetSize().cy);

		Allocate(Rect);

		const DWORD* pSrcData = SrcImage.GetDataPtr(SZPOINT(Rect.m_Left, Rect.m_Top));
		DWORD* pDstData = GetDataPtr();
		
		for(size_t y = Rect.m_Top ; y < Rect.m_Bottom ; y++)
		{
			memcpy(pDstData, pSrcData, m_Size.cx * sizeof(DWORD));
			pSrcData += SrcImage.GetSize().cx, pDstData += m_Size.cx;
		}
	}

	catch(...)
	{
		Release(true);
		throw;
	}
	
	return *this;
}

TImage& TImage::CreateResized(const TImage& SrcImage, const SZSIZE& SSize)
{
	Release();

	try
	{
		DEBUG_VERIFY(SrcImage.IsAllocated());

		Allocate(SSize);		

		if(SrcImage.GetWidth() == GetWidth() && SrcImage.GetHeight() == GetHeight()) // copy case
		{
			memcpy(GetDataPtr(), SrcImage.GetDataPtr(), GetSize().GetArea() * sizeof(DWORD));
		}
		if(!(SrcImage.GetWidth() % GetWidth()) && !(SrcImage.GetHeight() % GetHeight())) // integer shrinking
		{
			const BYTE* pPixSrcData = (const BYTE*)SrcImage.GetDataPtr();

			BYTE* pPixDstData = (BYTE*)GetDataPtr();

			size_t nx = SrcImage.GetWidth () / GetWidth ();
			size_t ny = SrcImage.GetHeight() / GetHeight();

			size_t cf = nx * ny;

			size_t szRowDelta	= SrcImage.GetWidth() * (ny - 1) * sizeof(DWORD);
			size_t szColDelta	= nx * sizeof(DWORD);
			size_t szBlockDelta	= (SrcImage.GetWidth() - nx) * sizeof(DWORD);

			for(size_t y = GetHeight() ; y ; y--, pPixSrcData += szRowDelta)
			{
				for(size_t x = GetWidth() ; x ; x--, pPixSrcData += szColDelta)
				{
					const BYTE* pCurScanSrcPixData = pPixSrcData;
					
					size_t tr = 0;
					size_t tg = 0;
					size_t tb = 0;
					size_t ta = 0;

					for(size_t cy = ny ; cy ; cy--, pCurScanSrcPixData += szBlockDelta)
					{
						for(size_t cx = nx ; cx ; cx--)
						{
							tb += *pCurScanSrcPixData++;
							tg += *pCurScanSrcPixData++;
							tr += *pCurScanSrcPixData++;
							ta += *pCurScanSrcPixData++;
						}
					}

					*pPixDstData++ = (BYTE)(tb / cf);
					*pPixDstData++ = (BYTE)(tg / cf);
					*pPixDstData++ = (BYTE)(tr / cf);
					*pPixDstData++ = (BYTE)(ta / cf);
				}
			}
		}
		else if(!(GetWidth() % SrcImage.GetWidth()) && !(GetHeight() % SrcImage.GetHeight())) // integer enlarging
		{
			const DWORD* pSrcData = SrcImage.GetDataPtr();

			DWORD* pDstData = GetDataPtr();

			size_t nx = GetWidth () / SrcImage.GetWidth ();
			size_t ny = GetHeight() / SrcImage.GetHeight();

			size_t szRowDelta	= GetWidth() * (ny - 1);
			size_t szColDelta	= nx;
			size_t szBlockDelta	= GetWidth() - nx;

			for(size_t y = SrcImage.GetHeight() ; y ; y--, pDstData += szRowDelta)
			{
				for(size_t x = SrcImage.GetWidth() ; x ; x--, pDstData += szColDelta)
				{
					DWORD v = *pSrcData++;

					DWORD* pScanDstData = pDstData;

					for(size_t cy = ny ; cy ; cy--, pScanDstData += szBlockDelta)
					{
						for(size_t cx = nx ; cx ; cx--)
							*pScanDstData++ = v;
					}
				}
			}
		}
		else // general case
		{
			const BYTE* pPixSrcData = (const BYTE*)SrcImage.GetDataPtr();

			BYTE* pPixDstData = (BYTE*)GetDataPtr();

			size_t y, x;
			double cy, cx;
			double ny, nx;

			double yc = (double)SrcImage.GetHeight() / GetHeight();
			double xc = (double)SrcImage.GetWidth () / GetWidth ();

			double cf = 1.0 / (xc * yc);

			size_t szSrcPitch = SrcImage.GetWidth() << 2;

			double y1 = 0, y2 = yc;

			for(y = m_Size.cy ; y ; y--, y1 = y2, y2 += yc)
			{
				double x1 = 0, x2 = xc;

				for(x = m_Size.cx ; x ; x--, x1 = x2, x2 += xc)
				{
					double tr = 0;
					double tg = 0;
					double tb = 0;
					double ta = 0;

					for(cy = y1 ; cy < y2 - 1e-4 ; cy = ny)
					{
						ny = Min(ceil(cy + 1e-4), y2);

						const BYTE* pRowPixSrcData = pPixSrcData + (size_t)cy * szSrcPitch;

						for(cx = x1 ; cx < x2 - 1e-4 ; cx = nx)
						{
							nx = Min(ceil(cx + 1e-4), x2);

							double cv = (nx-cx) * (ny-cy);

							const BYTE* pCurPixSrcData =
								pRowPixSrcData + ((size_t)cx << 2);

							tb += *pCurPixSrcData++ * cv;
							tg += *pCurPixSrcData++ * cv;
							tr += *pCurPixSrcData++ * cv;
							ta += *pCurPixSrcData++ * cv;
						}
					}

					*pPixDstData++ = (BYTE)(tb*cf + 1e-5);
					*pPixDstData++ = (BYTE)(tg*cf + 1e-5);
					*pPixDstData++ = (BYTE)(tr*cf + 1e-5);
					*pPixDstData++ = (BYTE)(ta*cf + 1e-5);
				}
			}
		}
	}

	catch(...)
	{
		Release(true);
		throw;
	}

	return *this;
}

TImage& TImage::BitBlit(const TImage&	SrcImage,
						IPOINT			DstCoords,
						const IRECT*	pSrcRect,
						TBitBlitMode	Mode,
						const ALSIZE&	Alignment,
						float			fMulAlpha)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(SrcImage.IsAllocated());

	IRECT SrcRect = pSrcRect ? *pSrcRect : SrcImage.GetRect();

	// Setting absolute X
	if(Alignment.cx == ALIGNMENT_MID)
		DstCoords.x = (int)(m_Size.cx - SrcImage.GetSize().cx) / 2;
	else if(Alignment.cx == ALIGNMENT_MAX)
		DstCoords.x = m_Size.cx - SrcImage.GetSize().cx - DstCoords.x;

	// Setting absolute Y
	if(Alignment.cy == ALIGNMENT_MID)
		DstCoords.y = (int)(m_Size.cy - SrcImage.GetSize().cy) / 2;
	else if(Alignment.cy == ALIGNMENT_MAX)
		DstCoords.y = m_Size.cy - SrcImage.GetSize().cy - DstCoords.y;

	// Source clipping
	if(SrcRect.m_Left < 0)
		DstCoords.x -= SrcRect.m_Left, SrcRect.m_Left = 0;

	if(SrcRect.m_Top < 0)
		DstCoords.y -= SrcRect.m_Top, SrcRect.m_Top = 0;

	UpdateMin(SrcRect.m_Right,  (int)SrcImage.GetWidth ());
	UpdateMin(SrcRect.m_Bottom, (int)SrcImage.GetHeight());

	// Destination clipping
	if(DstCoords.x < 0)
		SrcRect.m_Left -= DstCoords.x, DstCoords.x = 0;

	if(DstCoords.y < 0)
		SrcRect.m_Top -= DstCoords.y, DstCoords.y = 0;

	UpdateMin(SrcRect.m_Right,  (int)GetWidth () - DstCoords.x + SrcRect.m_Left);
	UpdateMin(SrcRect.m_Bottom, (int)GetHeight() - DstCoords.y + SrcRect.m_Top);

	if(!SrcRect.IsValid() || DstCoords.x >= (int)GetWidth() || DstCoords.y >= (int)GetHeight())
		return *this;

	// Blitting
	const DWORD* pSrcData = SrcImage.GetDataPtr(TO_SZ(SrcRect.GetTopLeft()));

	DWORD* pDstData = GetDataPtr(TO_SZ(DstCoords));

	if(Mode == BBM_COPY)
	{
		if(SrcRect == SrcImage.GetRect() && SrcImage.GetSize() == GetSize() && DstCoords == IPOINT(0, 0))
		{
			memcpy(pDstData, pSrcData, GetSize().GetArea() * sizeof(DWORD));
		}
		else
		{
			size_t szSrcDelta = SrcImage.GetWidth();
			size_t szDstDelta = GetWidth();

			size_t szAmt = SrcRect.GetWidth() * sizeof(DWORD);

			for(size_t y = SrcRect.GetHeight() ; y ; y--)
			{
				memcpy(pDstData, pSrcData, szAmt);

				pSrcData += szSrcDelta;
				pDstData += szDstDelta;
			}
		}
	}
	else if(Mode == BBM_ALPHABLIT)
	{
		DEBUG_VERIFY(HitsSegmentBounds(fMulAlpha, FSEGMENT(0, 1)));

		fMulAlpha /= 255.0;

		if(fMulAlpha < FLOAT_THRESHOLD)
			return *this;

		size_t szSrcDelta = SrcImage.GetWidth() - SrcRect.GetWidth();

		size_t szDstDelta = GetWidth() - SrcRect.GetWidth();

		for(size_t y = SrcRect.GetHeight() ; y ; y--)
		{
			for(size_t x = SrcRect.GetWidth() ; x ; x--)
			{
				DWORD	SPixel = *pSrcData++; 
				DWORD&	DPixel = *pDstData++;

				TRGB SColor;
				SColor.Set32bpp(SPixel);
				
				TRGB DColor;
				DColor.Set32bpp(DPixel);

				BYTE bSAlpha = (BYTE)(SPixel >> 24);
				BYTE bDAlpha = (BYTE)(DPixel >> 24);

				float fAlpha = bSAlpha * fMulAlpha;

				DColor *= 1.0f - fAlpha;
				SColor *= fAlpha;
				DColor += SColor;

				DPixel = DColor.Get32bpp(bDAlpha);
			}

			pSrcData += szSrcDelta;
			pDstData += szDstDelta;
		}
	}
	else if(Mode == BBM_ONLY_FULL_SRC_ALPHA_PIXELS)
	{
		size_t szSrcDelta = SrcImage.GetWidth() - SrcRect.GetWidth();
		size_t szDstDelta = GetWidth() - SrcRect.GetWidth();

		for(size_t y = SrcRect.GetHeight() ; y ; y--)
		{
			for(size_t x = SrcRect.GetWidth() ; x ; x--)
			{
				DWORD	SPixel = *pSrcData++; 
				DWORD&	DPixel = *pDstData++;
				
				if((SPixel >> 24) == 0xFF)
					DPixel = (SPixel & 0x00FFFFFF) | (DPixel & 0xFF000000);
			}

			pSrcData += szSrcDelta;
			pDstData += szDstDelta;
		}
	}
	else if(Mode == BBM_ONLY_FULL_DST_ALPHA_PIXELS)
	{
		size_t szSrcDelta = SrcImage.GetWidth() - SrcRect.GetWidth();
		size_t szDstDelta = GetWidth() - SrcRect.GetWidth();

		for(size_t y = SrcRect.GetHeight() ; y ; y--)
		{
			for(size_t x = SrcRect.GetWidth() ; x ; x--)
			{
				DWORD	SPixel = *pSrcData++; 
				DWORD&	DPixel = *pDstData++;
				
				if((DPixel >> 24) == 0xFF)
					DPixel = SPixel | 0xFF000000;
			}

			pSrcData += szSrcDelta;
			pDstData += szDstDelta;
		}
	}
	else
	{
		INITIATE_FAILURE;
	}

	return *this;
}

TImage& TImage::DrawLine(const IPOINT& Coords1, const IPOINT& Coords2, DWORD dwColor, const IRECT* pClipRect)
{
	DEBUG_VERIFY_ALLOCATION;

	IRECT ClipRect(0, 0, m_Size.cx, m_Size.cy);
	
	if(pClipRect)
		IntersectRect(ClipRect, *pClipRect);

	if(!ClipRect.IsValid())
		return *this;

	int dx = Coords2.x - Coords1.x;
	int dy = Coords2.y - Coords1.y;

	if(	Max(Coords1.x, Coords2.x) <  ClipRect.m_Left	||
		Min(Coords1.x, Coords2.x) >= ClipRect.m_Right	||
		Max(Coords1.y, Coords2.y) <  ClipRect.m_Top		||
		Min(Coords1.y, Coords2.y) >= ClipRect.m_Bottom)
	{
		return *this;
	}

	int xs, ys;

	if(dx < 0)
		dx = -dx, xs = -1;
	else
		xs = 1;

	if(dy < 0)
		dy = -dy, ys = -1;
	else
		ys = 1;

	int adx = abs(dx), ady = abs(dy);

	int iDlt = ys * m_Size.cx;

	IPOINT cp = Coords1;

	DWORD* pDst = GetDataPtr() + cp.y * m_Size.cx + cp.x;

	if(adx >= ady)
	{
		int e  = 0;
		int de = ady + 1;
		int cs = adx + 1;

		for(;;)
		{
			if(HitsRect(cp, ClipRect))
				*pDst = dwColor;

			if(cp.x == Coords2.x)
				break;

			if((e += de) >= cs)
				e -= cs, cp.y += ys, pDst += iDlt;

			cp.x += xs, pDst += xs;
		}

		DEBUG_VERIFY(cp == Coords2);
	}
	else // adx < ady
	{
		int e  = 0;
		int de = adx + 1;
		int cs = ady + 1;

		for(;;)
		{
			if(HitsRect(cp, ClipRect))
				*pDst = dwColor;

			if(cp.y == Coords2.y)
				break;

			if((e += de) >= cs)
				e -= cs, cp.x += xs, pDst += xs;

			cp.y += ys, pDst += iDlt;
		}

		DEBUG_VERIFY(cp == Coords2);
	}

	return *this;
}

void TImage::Load(const KString& FileName)
{
	Release();

	if(!FileExists(FileName))
		INITIATE_DEFINED_FAILURE((KString)TEXT("Image file \"") + FileName + TEXT("\" not found."));

	try
	{
		KString Extension = GetFileExtension(FileName);

		if(Extension.CompareNoCase(TEXT("bmp"))==0)
			LoadAsBMP(FileName);
		else if(Extension.CompareNoCase(TEXT("jpg"))==0 || Extension.CompareNoCase(TEXT("jpeg"))==0)
			LoadAsJPEG(FileName);
		else if(Extension.CompareNoCase(TEXT("tga"))==0)
			LoadAsTGA(FileName);
		else if(Extension.CompareNoCase(TEXT("tif"))==0 || Extension.CompareNoCase(TEXT("tiff"))==0)
			LoadAsTIFF(FileName);
		else if(Extension.CompareNoCase(TEXT("png"))==0)
			LoadAsPNG(FileName);
		else
			INITIATE_DEFINED_FAILURE((KString)TEXT("Unknown image file type \"") + FileName + TEXT("\"."));
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void TImage::Save(const KString& FileName, int iQuality) const
{
	DEBUG_VERIFY_ALLOCATION;

	KString Extension = GetFileExtension(FileName);

	if(Extension.CompareNoCase(TEXT("bmp"))==0)
		SaveAsBMP(FileName);
	else if(Extension.CompareNoCase(TEXT("jpg"))==0 || Extension.CompareNoCase(TEXT("jpeg"))==0)
		SaveAsJPEG(FileName, iQuality);
	else if(Extension.CompareNoCase(TEXT("tga"))==0)
		SaveAsTGA(FileName);
	else if(Extension.CompareNoCase(TEXT("tif"))==0 || Extension.CompareNoCase(TEXT("tiff"))==0)
		SaveAsTIFF(FileName);
	else if(Extension.CompareNoCase(TEXT("png"))==0)
		SaveAsPNG(FileName);
	else
		INITIATE_DEFINED_FAILURE(TEXT("Unsupported image file extension on saving."));
}

void TImage::LoadAsBMP(const KString& FileName)
{
	Release();

	try
	{
		TFile File(FileName, FOF_READ);

		// Reading file header
		BITMAPFILEHEADER FileHeader;
		File >> FileHeader;
		
		// Checking file type
		LPCSTR pType = (char*)&FileHeader.bfType;
		if(pType[0] != 'B' || pType[1] != 'M')
			INITIATE_FAILURE;

		// Reading info header
		BITMAPINFOHEADER InfoHeader;
		File >> InfoHeader;
		
		// Checking compression
		if(InfoHeader.biCompression != BI_RGB)
			INITIATE_FAILURE;

		// Allocating
		Allocate(SZSIZE(InfoHeader.biWidth, InfoHeader.biHeight));

		// Settting NColors
		size_t szNColors = 0;

		if(InfoHeader.biBitCount < 16)
		{
			szNColors =	InfoHeader.biClrUsed ?
						InfoHeader.biClrUsed :
						(1 << InfoHeader.biBitCount);
		}

		// Setting bytes width
		size_t szBytesWidth = (m_Size.cx * InfoHeader.biBitCount + 7) >> 3;
		
		// Evaluating image size and pitch
		size_t szImageSize;
		size_t szPitch;

		if(InfoHeader.biSizeImage)
		{
			szImageSize	= InfoHeader.biSizeImage;
			szPitch		= szImageSize / m_Size.cy;
		}
		else
		{
			szPitch = szBytesWidth;

			// Paragraph alignment
			if(szPitch & 3)
				szPitch += 4 - (szPitch & 3);

			szImageSize = szPitch * m_Size.cy;
		}
		
		// Seeking to the pixel data
		File.Seek(sizeof(FileHeader) + InfoHeader.biSize);

		// Reading palette
		TArray<RGBQUAD, true> Palette;

		if(szNColors)
		{
			Palette.Add(szNColors);
			Palette.ReadItems(File);
		}

		// Read buffer
		TArray<BYTE, true> ReadBuf;
		ReadBuf.Add(szPitch * m_Size.cy);
		ReadBuf.ReadItems(File);

		BYTE*	pSrc = ReadBuf.GetDataPtr() + szPitch * (m_Size.cy - 1);
		int		iReadBufDelta = -(int)(szBytesWidth + szPitch);

		DWORD* pDst = GetDataPtr();

		// Filling
		DWORD r, g, b;

		size_t x, y, i;
		size_t szIndex;

		if(InfoHeader.biBitCount == 1)
		{
			for(y = m_Size.cy ; y ; y--)
			{
				i = 0;

				for(x = m_Size.cx ; x ; x--)
				{
					szIndex = (*pSrc & 0x80) >> 7;

					if(++i==8 || x==1)
						i=0, pSrc++;
					else
						*pSrc <<= 1;

					b = Palette[szIndex].rgbBlue;
					g = Palette[szIndex].rgbGreen	<< 8;
					r = Palette[szIndex].rgbRed		<< 16;

					*pDst++ = 0xFF000000 | r | g | b;						
				}

				pSrc += iReadBufDelta;
			}
		}
		else if(InfoHeader.biBitCount == 4)
		{
			for(y = m_Size.cy ; y ; y--)
			{
				i = 0;

				for(x = m_Size.cx ; x ; x--)
				{
					szIndex = (*pSrc & 0xF0) >> 4;

					if(++i==2 || x==1)
						i=0, pSrc++;
					else
						*pSrc <<= 4;

					b = Palette[szIndex].rgbBlue;
					g = Palette[szIndex].rgbGreen	<< 8;
					r = Palette[szIndex].rgbRed		<< 16;

					*pDst++ = 0xFF000000 | r | g | b;
				}

				pSrc += iReadBufDelta;
			}
		}
		else if(InfoHeader.biBitCount == 8)
		{
			for(y = m_Size.cy ; y ; y--)
			{
				for(x = m_Size.cx ; x ; x--)
				{
					BYTE bSrc = *pSrc++;
					
					b = Palette[bSrc].rgbBlue;
					g = Palette[bSrc].rgbGreen	<< 8;
					r = Palette[bSrc].rgbRed	<< 16;

					*pDst++ = 0xFF000000 | r | g | b;
				}

				pSrc += iReadBufDelta;
			}
		}
		else if(InfoHeader.biBitCount==16)
		{
			for(y = m_Size.cy ; y ; y--)
			{
				for(x = m_Size.cx ; x ; x--)
				{
					WORD wSrc = *(WORD*)pSrc;
					pSrc += sizeof(WORD);

					b = wSrc & 0x001F;
					g = ((wSrc >> 5)	& 0x1F) << 8;
					r = ((wSrc >> 10)	& 0x1F) << 16;

					*pDst++ = 0xFF000000 | r | g | b;
				}

				pSrc += iReadBufDelta;
			}
		}
		else if(InfoHeader.biBitCount == 24)
		{
			for(y = m_Size.cy ; y ; y--)
			{
				for(x = m_Size.cx ; x ; x--)
				{
					b = *pSrc++;
					g = *pSrc++ << 8;
					r = *pSrc++ << 16;

					*pDst++ = 0xFF000000 | r | g | b;
				}

				pSrc += iReadBufDelta;
			}
		}
		else if(InfoHeader.biBitCount == 32)
		{
			for(y = m_Size.cy ; y ; y--)
			{
				for(x = m_Size.cx ; x ; x--)
					*pDst++ = 0xFF000000 | *pSrc++;

				pSrc += iReadBufDelta;
			}
		}
		else
		{
			INITIATE_FAILURE;
		}
	}

	catch(...)
	{
		Release(true);
		INITIATE_DEFINED_FAILURE((KString)TEXT("\"") + FileName + TEXT("\" is not valid BMP file."));
	}
}

void TImage::LoadAsJPEG(const KString& FileName)
{
	Release();

	try
	{
		FILE* pFile = _tfopen(FileName, TEXT("rb"));
		if(pFile == NULL)
			INITIATE_DEFINED_FAILURE((KString)TEXT("Error opening JPEG file \"") + FileName + TEXT("\"."));

		try
		{	
			jpeg_decompress_struct	cinfo;
			jpeg_error_mgr			jerr;

			// Preparing JPEG info
			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_decompress(&cinfo);

			try
			{		
				// Preparing JPEG input
				jpeg_stdio_src(&cinfo, pFile);
				
				// Loading JPEG header
				if(!jpeg_read_header(&cinfo, true))
					throw T_KFC_Exception();

				// Preparing JPEG decompression
				try
				{
					jpeg_start_decompress(&cinfo);

					// Allocating
					Allocate(SZSIZE(cinfo.image_width, cinfo.image_height));

					// Reading scanlines
					int row_stride = cinfo.output_width * cinfo.output_components;
					JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

					DWORD *pDst = GetDataPtr();

					while(cinfo.output_scanline < cinfo.output_height)
					{
						jpeg_read_scanlines(&cinfo, buffer, 1);

						BYTE *pSrc = *((BYTE**)buffer);

						if(cinfo.num_components==1)
						{
							for(size_t i=cinfo.image_width ; i ;i--)
							{
								register DWORD v = *pSrc++;
								*pDst++ = 0xFF000000 | (v<<16) | (v<<8) | v;
							}
						}
						else
						{
							for(size_t i=cinfo.image_width ; i ;i--)
							{
								DWORD r = *pSrc++ << 16;
								DWORD g = *pSrc++ << 8;
								DWORD b = *pSrc++;

								*pDst++ = 0xFF000000 | r | g | b;
							}
						}
					}
				}

				catch(...)
				{
					jpeg_finish_decompress(&cinfo);
					throw;
				}

				jpeg_finish_decompress(&cinfo);
			}

			catch(...)
			{
				jpeg_destroy_decompress(&cinfo);
				throw;
			}

			jpeg_destroy_decompress(&cinfo);
		}

		catch(...)
		{
			fclose(pFile);
			throw;
		}

		fclose(pFile);
	}

	catch(...)
	{
		Release(true);
		INITIATE_DEFINED_FAILURE((KString)TEXT("\"") + FileName + TEXT("\" is not valid JPEG file."));
	}
}

void TImage::LoadAsTGA(const KString& FileName)
{
	Release();

	try
	{
		TFile File(FileName, FOF_BINARYREAD);

		// Reading file head
		TGA_FILEHEAD fhead;
		File >> fhead;
		
		// Checking file type
		if(	fhead.bImageType != 2 &&
			fhead.bImageType != 10)
		{
			INITIATE_FAILURE;
		}

		if(fhead.bColorMapType)
			INITIATE_FAILURE;

		if(	fhead.bPixelDepth != 16 &&
			fhead.bPixelDepth != 24 &&
			fhead.bPixelDepth != 32)
		{
			INITIATE_FAILURE;
		}

		// Setting alpha presence (16bit only)
		bool bHasAlpha = (fhead.bImageDescription & 7) ? true : false;

		// Skipping image ID
		File.Seek(fhead.bIDLength, FSM_CURRENT);

		// Read buffer
		TArray<BYTE, true> ReadBuffer;
		size_t szReadBufferSize = File.GetLength() - File.Tell();
		ReadBuffer.Add(szReadBufferSize);

		// Loading image data into the buffer
		ReadBuffer.ReadItems(File);
		
		// Retrieving src data ptr
		const BYTE* pSrcData = ReadBuffer.GetDataPtr();

		// Loading image data
		Allocate(SZSIZE(fhead.wWidth, fhead.wHeight));

		DWORD *write = GetDataPtr(SZPOINT(0, m_Size.cy - 1));

		size_t szWriteDelta = fhead.wWidth << 1;

		size_t	x, y;
		DWORD	v;
		BYTE	rv;

		if(fhead.bImageType == 2) // no compression
		{
			if(fhead.bPixelDepth == 16)
			{
				for(y = m_Size.cy ; y ; y--)
				{
					for(x = m_Size.cx ; x ; x--)
					{
						v = 0;
						rv = *pSrcData++;
						v |= (rv & 0x1F) << 3, v |= (rv & 0xE0) << 6;

						rv = *pSrcData++;
						v |= (rv & 0x03) << 14, v |= (rv & 0x7C) << 17;

						if(bHasAlpha && (rv & 0x80))
							v |= 0xFF000000;

						*write++ = v;
					}

					write -= szWriteDelta;
				}
			}
			else if(fhead.bPixelDepth == 24)
			{
				for(y = m_Size.cy ; y ; y--)
				{
					for(x = m_Size.cx ; x ; x--)
					{
						v = 0xFF000000;

						v |= *pSrcData++;
						v |= *pSrcData++ << 8;
						v |= *pSrcData++ << 16;

						*write++ = v;
					}

					write -= szWriteDelta;
				}
			}
			else if(fhead.bPixelDepth == 32)
			{
				size_t szRowSizeBytes = m_Size.cx * sizeof(DWORD);
				for(y = m_Size.cy ; y ; y--)
				{
					memcpy(write, pSrcData, szRowSizeBytes), pSrcData += szRowSizeBytes;
					write += m_Size.cx, write -= szWriteDelta;
				}
			}
		}
		else if(fhead.bImageType == 10) // compression
		{
			if(fhead.bPixelDepth == 16)
			{
				for(y = m_Size.cy ; y ; y--)
				{				
					for(x = m_Size.cx ; x ; )
					{
						BYTE bPref = *pSrcData++;
						
						if(bPref & 0x80)
						{
							bPref &= 0x7F , bPref++;

							v = 0;

							rv = *pSrcData++;
							v |= (rv & 0x1F) << 3, v |= (rv & 0xE0) << 6;

							rv = *pSrcData++;
							v |= (rv & 0x03) << 14, v |= (rv & 0x7C) << 17;

							if(bHasAlpha && (rv & 0x80))
								v |= 0xFF000000;

							for(BYTE j=bPref ; j ; j--,x--)
								*write++ = v;
						}
						else
						{
							bPref++;

							for(BYTE j=bPref ; j ; j--,x--)
							{
								v = 0;

								rv = *pSrcData++;
								v |= (rv & 0x1F) << 3, v |= (rv & 0xE0) << 6;

								rv = *pSrcData++;
								v |= (rv & 0x03) << 14, v |= (rv & 0x7C) << 17;

								if(bHasAlpha && (rv & 0x80))
									v |= 0xFF000000;

								*write++ = v;
							}
						}
					}

					write -= szWriteDelta;
				}
			}
			else if(fhead.bPixelDepth == 24)
			{
				for(y = m_Size.cy ; y ; y--)
				{
					for(x = m_Size.cx ; x ; )
					{
						BYTE bPref = *pSrcData++;
						
						if(bPref & 0x80)
						{
							bPref &= 0x7F , bPref++;

							v = 0xFF000000;

							v |= *pSrcData++;
							v |= *pSrcData++ << 8;
							v |= *pSrcData++ << 16;

							for(BYTE j=bPref ; j ; j--,x--)
								*write++ = v;
						}
						else
						{
							bPref++;

							for(BYTE j=bPref ; j ; j--,x--)
							{
								v = 0xFF000000;

								v |= *pSrcData++;
								v |= *pSrcData++ << 8;
								v |= *pSrcData++ << 16;

								*write++ = v;
							}
						}
					}

					write -= szWriteDelta;
				}
			}
			else if(fhead.bPixelDepth == 32)
			{
				for(y = m_Size.cy ; y ; y--)
				{		
					for(x = m_Size.cx ; x ; )
					{
						BYTE bPref = *pSrcData++;
						
						if(bPref & 0x80)
						{
							bPref &= 0x7F , bPref++;

							v = *pSrcData++;
							
							for(BYTE j=bPref ; j ; j--,x--)
								*write++ = v;
						}
						else
						{
							bPref++;

							size_t szStripSizeBytes = bPref * sizeof(DWORD);
							memcpy(write, pSrcData, szStripSizeBytes), pSrcData += szStripSizeBytes;
							write+=bPref, x-=bPref;
						}
					}

					write -= szWriteDelta;
				}
			}
		}
	}

	catch(...)
	{
		Release(true);		
		INITIATE_DEFINED_FAILURE((KString)TEXT("\"") + FileName + TEXT("\" is not a valid TGA file."));
	}
}

void TImage::LoadAsTIFF(const KString& FileName)
{
	Release();

	try
	{
		size_t i;

		TIFF* pTiff = TIFFOpen(FileName, "r");
		if(pTiff == NULL)
			INITIATE_DEFINED_FAILURE((KString)"File \"" + FileName + "\" not found.");

		try
		{	
			SZSIZE Size;

			TIFFGetField(pTiff, TIFFTAG_IMAGEWIDTH,		&Size.cx);
			TIFFGetField(pTiff, TIFFTAG_IMAGELENGTH,	&Size.cy);

			Allocate(Size);
			
			#ifdef _MSC_VER
				if(!TIFFReadRGBAImage(pTiff, m_Size.cx, m_Size.cy, GetDataPtr(), 0))
					throw 1;
			#else // _MSC_VER
				if(!TIFFReadRGBAImage(pTiff, m_Size.cx, m_Size.cy, (uint32*)GetDataPtr(), 0))
					throw 1;			
			#endif // _MSC_VER

			m_Pixels.FlipHorizontal();

			TIFFClose(pTiff);
		}

		catch(...)
		{
			TIFFClose(pTiff);
			throw 1;
		}

		// Converting colors
		DWORD* p = GetDataPtr();		

		for(i = m_Size.GetArea() ; i ; i--, p++)
		{
			const DWORD a = (*p >> 24)	& 0xFF;
			const DWORD b = (*p >> 16)	& 0xFF;
			const DWORD g = (*p >> 8)	& 0xFF;
			const DWORD r = (*p)		& 0xFF;

			*p = (a << 24) | (r << 16) | (g << 8) | b;
		}
	}

	catch(...)
	{
		Release(true);
		INITIATE_DEFINED_FAILURE((KString)TEXT("\"") + FileName + TEXT("\" is not a valid TIFF file."));
	}
}

void TImage::LoadAsPNG(const KString& FileName)
{
	Release();

	try
	{
		size_t i, j;

		FILE* pFile = fopen(FileName, "rb");
		if(pFile == NULL)
			INITIATE_DEFINED_FAILURE((KString)"File \"" + FileName + "\" not found.");

		try
		{
			// Checking file signature
			BYTE sig[8];
			if(fread(sig, 1, 8, pFile) != 8 || png_sig_cmp(sig, 0, 8))
				throw 1;

			// Creating read struct
			png_structp pPng =
				png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

			if(pPng == NULL)
				throw 1;

			// Creating info struct
			png_infop pInfo =
				png_create_info_struct(pPng);

			if(pInfo == NULL)
			{
				png_destroy_read_struct(&pPng, NULL, NULL);
				throw 1;
			}

			// Initializing IO
			png_init_io(pPng, pFile);

			png_set_sig_bytes(pPng, 8);

			png_read_png(	pPng,
							pInfo,
							PNG_TRANSFORM_STRIP_16 |
								PNG_TRANSFORM_PACKING,
							NULL);

			Allocate(SZSIZE(pInfo->width, pInfo->height));

			BYTE** ppRows = png_get_rows(pPng, pInfo);

			DEBUG_VERIFY(ppRows);

			if(pInfo->channels == 1) // gray or paletted
			{
				if(pInfo->color_type == PNG_COLOR_TYPE_PALETTE)
				{
					DWORD* pDst = GetDataPtr();

					for(i = 0 ; i < m_Size.cy ; i++)
					{
/*						if(IsTerminated(hTerminator))
						{
							png_destroy_read_struct(&pPng, &pInfo, png_infopp_NULL);
							fclose(pFile);
							Release();
							return;
						}*/

						const BYTE* pSrc = ppRows[i];

						for(j = 0 ; j < m_Size.cx ; j++)
						{
							const png_color& col = pInfo->palette[*pSrc++];

							BYTE r = col.red;
							BYTE g = col.green;
							BYTE b = col.blue;
							BYTE a = 0xFF;

							*pDst++ = (a << 24) | (r << 16) | (g << 8) | b;
						}
					}
				}
				else
				{
					DWORD* pDst = GetDataPtr();

					for(i = 0 ; i < m_Size.cy ; i++)
					{
/*						if(IsTerminated(hTerminator))
						{
							png_destroy_read_struct(&pPng, &pInfo, png_infopp_NULL);
							fclose(pFile);
							Release();
							return;
						}*/

						const BYTE* pSrc = ppRows[i];

						for(j = 0 ; j < m_Size.cx ; j++)
						{
							BYTE v = *pSrc++;
							BYTE a = 0xFF;

							*pDst++ = (a << 24) | (v << 16) | (v << 8) | v;
						}
					}
				}
			}
			else if(pInfo->channels == 2) // gray + alpha
			{
				DWORD* pDst = GetDataPtr();

				for(i = 0 ; i < m_Size.cy ; i++)
				{
/*					if(IsTerminated(hTerminator))
					{
						png_destroy_read_struct(&pPng, &pInfo, png_infopp_NULL);
						fclose(pFile);
						Release();
						return;
					}*/

					const BYTE* pSrc = ppRows[i];

					for(j = 0 ; j < m_Size.cx ; j++)
					{
						BYTE v = *pSrc++;
						BYTE a = *pSrc++;

						*pDst++ = (a << 24) | (v << 16) | (v << 8) | v;
					}
				}
			}
			else if(pInfo->channels == 3) // rgb
			{
				DWORD* pDst = GetDataPtr();

				for(i = 0 ; i < m_Size.cy ; i++)
				{
/*					if(IsTerminated(hTerminator))
					{
						png_destroy_read_struct(&pPng, &pInfo, png_infopp_NULL);
						fclose(pFile);
						Release();
						return;
					}*/

					const BYTE* pSrc = ppRows[i];

					for(j = 0 ; j < m_Size.cx ; j++)
					{
						BYTE r = *pSrc++;
						BYTE g = *pSrc++;
						BYTE b = *pSrc++;
						BYTE a = 0xFF;

						*pDst++ = (a << 24) | (r << 16) | (g << 8) | b;
					}
				}
			}
			else if(pInfo->channels == 4) // rgb + alpha
			{
				DWORD* pDst = GetDataPtr();

				for(i = 0 ; i < m_Size.cy ; i++)
				{
/*					if(IsTerminated(hTerminator))
					{
						png_destroy_read_struct(&pPng, &pInfo, png_infopp_NULL);
						fclose(pFile);
						Release();
						return;
					}*/

					const BYTE* pSrc = ppRows[i];

					for(j = 0 ; j < m_Size.cx ; j++)
					{
						BYTE r = *pSrc++;
						BYTE g = *pSrc++;
						BYTE b = *pSrc++;
						BYTE a = *pSrc++;

						*pDst++ = (a << 24) | (r << 16) | (g << 8) | b;
					}
				}
			}

			png_destroy_read_struct(&pPng, &pInfo, png_infopp_NULL);

			fclose(pFile);
		}

		catch(...)
		{
			fclose(pFile);
			throw 1;
		}		
	}

	catch(...)
	{
		Release(true);
		INITIATE_DEFINED_FAILURE((KString)TEXT("\"") + FileName + TEXT("\" is not a valid PNG file."));
	}
}

void TImage::SaveAsBMP(const KString& FileName) const
{
	DEBUG_VERIFY_ALLOCATION;
		
	TFile File(FileName, FOF_BINARYWRITE);

	// Data size
	size_t szRowBytesSize = m_Size.cx * 3;
	if(szRowBytesSize & 3) // 4-byte alignment
		szRowBytesSize |= 3, szRowBytesSize++;

	const size_t szDataSize = szRowBytesSize * m_Size.cy;

	// File header
	BITMAPFILEHEADER FileHead;

	((LPSTR)&FileHead.bfType)[0] = 'B';
	((LPSTR)&FileHead.bfType)[1] = 'M';

	FileHead.bfReserved1	= 0;
	FileHead.bfReserved2	= 0;
	FileHead.bfOffBits		= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	FileHead.bfSize			= FileHead.bfOffBits + szDataSize;

	File << FileHead;

	// Info header
	BITMAPINFOHEADER InfoHead;
	
	InfoHead.biSize				= sizeof(InfoHead);
	InfoHead.biWidth			= m_Size.cx;
	InfoHead.biHeight			= m_Size.cy;
	InfoHead.biPlanes			= 1;
	InfoHead.biBitCount			= 24;
	InfoHead.biCompression		= BI_RGB;
	InfoHead.biSizeImage		= szDataSize;
	InfoHead.biXPelsPerMeter	= 2048;
	InfoHead.biYPelsPerMeter	= 2048;
	InfoHead.biClrUsed			= 0;
	InfoHead.biClrImportant		= 0;

	File << InfoHead;

	// Data
	TArray<BYTE, true> Data;
	Data.Add(szDataSize);

	const BYTE* pSrcData = (const BYTE*)GetDataPtr(SZPOINT(0, m_Size.cy - 1));

	BYTE* pDstData = Data.GetDataPtr();
	
	const size_t szSrcDelta = m_Size.cx << 3;
	const size_t szDstDelta = szRowBytesSize - (m_Size.cx * 3);

	for(size_t y = m_Size.cy ; y ; y--)
	{
		for(size_t x = m_Size.cx ; x ; x--)
		{
			*pDstData++ = *pSrcData++;	// red
			*pDstData++ = *pSrcData++;	// green
			*pDstData++ = *pSrcData++;	// blue
			pSrcData++;					// alpha;
		}

		pSrcData -= szSrcDelta;
		pDstData += szDstDelta;
	}

	Data.WriteItems(File);
}

void TImage::SaveAsJPEG(const KString& FileName, int iQuality) const
{
	DEBUG_VERIFY_ALLOCATION;

//	INITIATE_DEFINED_FAILURE(TEXT("Move to KFC_Image!"));

	FILE* pFile = _tfopen(FileName, TEXT("wb"));
	if(pFile == NULL)
		INITIATE_DEFINED_FAILURE((KString)TEXT("Error creating JPEG file \"") + FileName + TEXT("\"."));
	
	jpeg_compress_struct	cinfo;
	jpeg_error_mgr			jerr;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_compress(&cinfo);

	try
	{
		jpeg_stdio_dest(&cinfo, pFile);

		cinfo.image_width		= m_Size.cx;
		cinfo.image_height		= m_Size.cy;
		cinfo.input_components	= 3;
		cinfo.in_color_space	= JCS_RGB;

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, iQuality, TRUE);

		jpeg_start_compress(&cinfo, TRUE);

		try
		{
			size_t row_stride = m_Size.cx * 3;

			TArray<BYTE, true> ScanLine;
			ScanLine.Add(row_stride);

			BYTE *pSrcData = (BYTE*)GetDataPtr();

			while(cinfo.next_scanline < cinfo.image_height)
			{
				BYTE *pTrgData = ScanLine.GetDataPtr();
	
				for(size_t i=m_Size.cx ; i ; i--)
				{
					*pTrgData++ = pSrcData[2];
					*pTrgData++ = pSrcData[1];
					*pTrgData++ = pSrcData[0];
					pSrcData += 4;
				}

				BYTE *pScanLine = ScanLine.GetDataPtr();
				jpeg_write_scanlines(&cinfo, &pScanLine, 1);
			}
		}

		catch(...)
		{
			jpeg_finish_compress(&cinfo);
			throw;
		}

		jpeg_finish_compress(&cinfo);
	}

	catch(...)
	{
		jpeg_destroy_compress(&cinfo);
		fclose(pFile);		
		INITIATE_DEFINED_FAILURE((KString)TEXT("Error saving JPEG file \"") + FileName + TEXT("\"."));
	}

	jpeg_destroy_compress(&cinfo);

	fclose(pFile);
}

void TImage::SaveAsTGA(const KString& FileName) const
{
	DEBUG_VERIFY_ALLOCATION;

	TFile File(FileName, FOF_BINARYWRITE);

	TGA_FILEHEAD fhead;

	fhead.bIDLength			= 0;
	fhead.bColorMapType		= 0;
	fhead.bImageType		= 2; // no compression
	fhead.wCMapStart		= 0;
	fhead.wCMapLength		= 0;
	fhead.bCMapDepth		= 0;
	fhead.wXOffset			= 0;
	fhead.wYOffset			= 0;
	fhead.wWidth			= (WORD)m_Size.cx;
	fhead.wHeight			= (WORD)m_Size.cy;
	fhead.bPixelDepth		= 32;
	fhead.bImageDescription	= 0x08; // 32bpp doesn't require this to be 0<x<8

	fhead.Save(File);

	const DWORD* pSrcData = GetDataPtr(SZPOINT(0, m_Size.cy - 1));
	for(size_t y = m_Size.cy ; y ; y--)
		File.Write(pSrcData, m_Size.cx * sizeof(DWORD)), pSrcData -= m_Size.cx;
}

void TImage::SaveAsTIFF(const KString& FileName) const
{
	DEBUG_VERIFY_ALLOCATION;

	TIFF* pTiff = TIFFOpen(FileName, "w");
	if(pTiff == NULL)
		INITIATE_DEFINED_FAILURE((KString)TEXT("Error creating file \"") + FileName + TEXT("\"."));

	try
	{
		TIFFSetField(pTiff, TIFFTAG_IMAGEWIDTH,			m_Size.cx);
		TIFFSetField(pTiff, TIFFTAG_IMAGELENGTH,		m_Size.cy);
		TIFFSetField(pTiff, TIFFTAG_ORIENTATION,		ORIENTATION_TOPLEFT);
		TIFFSetField(pTiff, TIFFTAG_SAMPLESPERPIXEL,	3);
		TIFFSetField(pTiff, TIFFTAG_BITSPERSAMPLE,		8);
		TIFFSetField(pTiff, TIFFTAG_PLANARCONFIG,		PLANARCONFIG_CONTIG);
		TIFFSetField(pTiff, TIFFTAG_PHOTOMETRIC,		PHOTOMETRIC_RGB);
		TIFFSetField(pTiff, TIFFTAG_COMPRESSION,		COMPRESSION_NONE);

		size_t y, x;

		TArray<BYTE, true> Scan(m_Size.cx * 3);

		for(y = 0 ; y < m_Size.cy ; y++)
		{
			const DWORD*	src = GetDataPtr(SZPOINT(0, y));
			BYTE*			dst = Scan.GetDataPtr();

			for(x = 0 ; x < m_Size.cx ; x++, src++)
			{
				const BYTE r = (BYTE)((*src >> 16)	& 0xFF);
				const BYTE g = (BYTE)((*src >> 8)	& 0xFF);
				const BYTE b = (BYTE)((*src)		& 0xFF);

				*dst++ = r, *dst++ = g, *dst++ = b;
			}

			if(!TIFFWriteScanline(pTiff, Scan.GetDataPtr(), y))
				throw 1;
		}

		TIFFClose(pTiff);
	}

	catch(...)
	{
		TIFFClose(pTiff);
		INITIATE_DEFINED_FAILURE((KString)TEXT("Error saving TIFF file \"") + FileName + TEXT("\"."));
	}
}

void TImage::SaveAsPNG(const KString& FileName) const
{
	DEBUG_VERIFY_ALLOCATION;
	
	DEBUG_VERIFY_ALLOCATION;

	FILE* pFile = fopen(FileName, "wb");
	if(pFile == NULL)
		INITIATE_DEFINED_FAILURE((KString)TEXT("Error creating file \"") + FileName + TEXT("\"."));

	try
	{
		png_structp pPng =
			png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if(pPng == NULL)
			throw 1;

		png_infop pInfo = png_create_info_struct(pPng);

		if(pInfo == NULL)
		{
			png_destroy_write_struct(&pPng, NULL);
			throw 1;
		}

		png_init_io(pPng, pFile);

		png_set_IHDR(	pPng,
						pInfo,
						m_Size.cx,
						m_Size.cy,
						8,
						PNG_COLOR_TYPE_RGB_ALPHA,
						PNG_INTERLACE_NONE,
						PNG_COMPRESSION_TYPE_DEFAULT,
						PNG_FILTER_TYPE_DEFAULT);

		TValueMatrix<BYTE, true> Temp;
		Temp.Allocate(SZSIZE(m_Size.cx * 4, m_Size.cy));

		const DWORD*	pSrc = GetDataPtr();
		BYTE*			pDst = Temp.GetDataPtr();

		size_t i, j;
		for(i = 0 ; i < m_Size.cy ; i++)
		{
			for(j = 0 ; j < m_Size.cx ; j++)
			{
				BYTE a = (BYTE)(*pSrc >> 24)	& 0xFF;
				BYTE r = (BYTE)(*pSrc >> 16)	& 0xFF;
				BYTE g = (BYTE)(*pSrc >> 8)		& 0xFF;
				BYTE b = (BYTE)(*pSrc)			& 0xFF;

				*pDst++ = r;
				*pDst++ = g;
				*pDst++ = b;
				*pDst++ = a;

				pSrc++;
			}
		}

		TArray<BYTE*, true> Rows(m_Size.cy);		

		for(i = 0 ; i < m_Size.cy ; i++)
			Rows[i] = Temp.GetDataPtr(SZPOINT(0, i));

		png_set_rows(pPng, pInfo, Rows.GetDataPtr());
		png_write_png(pPng, pInfo, PNG_TRANSFORM_IDENTITY, NULL);

		png_destroy_write_struct(&pPng, &pInfo);

		fclose(pFile);
	}

	catch(...)
	{
		fclose(pFile);
		INITIATE_DEFINED_FAILURE((KString)TEXT("Error saving PNG file \"") + FileName + TEXT("\"."));
	}
}

TStream& operator >> (TStream& Stream, TImage& RImage)
{
	SZSIZE Size;
	Stream >> Size;

	RImage.Allocate(Size);

	Stream.StreamRead(RImage.GetDataPtr(), Size.GetArea() << 2);

	return Stream;
}

TStream& operator << (TStream& Stream, const TImage& Image)
{
	DEBUG_VERIFY(Image.IsAllocated());

	Stream << Image.GetSize();

	Stream.StreamWrite(Image.GetDataPtr(), Image.GetSize().GetArea() << 2);

	return Stream;
}
