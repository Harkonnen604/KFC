#include "kfc_image_pch.h"
#include "bitmap_image.h"

#ifdef _MSC_VER

// ------------------
// Bitmap image desc
// ------------------
TBitmapImageDesc::TBitmapImageDesc()
{
	SetDefaults();
}

TBitmapImageDesc::TBitmapImageDesc(	const SZSIZE&	SSize,
									bool			bInversedOrientation)
{
	SetDefaults();

	SetSize(SSize, bInversedOrientation);
}

TBitmapImageDesc::TBitmapImageDesc(const BITMAPINFO& SBitmapInfo)
{
	SetDefaults();

	SetBitmapInfo(SBitmapInfo);
}

void TBitmapImageDesc::SetDefaults()
{
	memset(&m_BitmapInfo, 0, sizeof(m_BitmapInfo));
	m_BitmapInfo.bmiHeader.biSize = sizeof(m_BitmapInfo.bmiHeader);

	m_BitmapInfo.bmiHeader.biPlanes			= 1;
	m_BitmapInfo.bmiHeader.biBitCount		= 32;
	m_BitmapInfo.bmiHeader.biCompression	= BI_RGB;

	m_pData = NULL;
}

void TBitmapImageDesc::SetSize(	const SZSIZE&	SSize,
								bool			bInversedOrientation)
{
	DEBUG_VERIFY(SSize.IsValid());

	m_Size = SSize;
	
	m_BitmapInfo.bmiHeader.biWidth = (int)m_Size.cx;

	if(bInversedOrientation)
		m_BitmapInfo.bmiHeader.biHeight = (int)m_Size.cy;
	else
		m_BitmapInfo.bmiHeader.biHeight = -(int)m_Size.cy;
}

void TBitmapImageDesc::SetBitmapInfo(const BITMAPINFO& SBitmapInfo)
{
	m_Size.Set(	(size_t)SBitmapInfo.bmiHeader.biWidth,
				(size_t)GetAbs((int)SBitmapInfo.bmiHeader.biHeight));

	DEBUG_VERIFY(m_Size.IsValid());

	memcpy(&m_BitmapInfo, &SBitmapInfo, sizeof(m_BitmapInfo));
}

bool TBitmapImageDesc::operator == (const TBitmapImageDesc& SDesc) const
{
	return	m_Size == SDesc.m_Size &&
			!memcmp(&m_BitmapInfo, &SDesc.m_BitmapInfo, sizeof(m_BitmapInfo));
}

// -------------
// Bitmap image
// -------------
TBitmapImage::TBitmapImage()
{
	m_bAllocated = false;

	m_hBitmap = NULL;
}

TBitmapImage::TBitmapImage(const TBitmapImageDesc& SDesc)
{
	m_bAllocated = false;

	m_hBitmap = NULL;

	Allocate(SDesc);
}

TBitmapImage::TBitmapImage(	const SZSIZE&	SSize,
							bool			bInversedOrientation)
{
	m_bAllocated = false;

	m_hBitmap = NULL;

	Allocate(SSize, bInversedOrientation);
}

TBitmapImage::TBitmapImage(const TImage& Image)
{
	m_bAllocated = false;

	m_hBitmap = NULL;

	Allocate(Image);
}

void TBitmapImage::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		if(m_hBitmap)
			DeleteObject(m_hBitmap), m_hBitmap = NULL;
	}
}

void TBitmapImage::Allocate(const TBitmapImageDesc& SDesc)
{
	if(IsAllocated() && m_Desc == SDesc)
		return;

	Release();

	try
	{
		m_Desc = SDesc;

		m_hBitmap = CreateDIBSection(	NULL,
										&m_Desc.m_BitmapInfo,
										DIB_RGB_COLORS,
										&m_Desc.m_pData,
										NULL,
										0);

		if(m_hBitmap == NULL)
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating DIB section"), GetLastError());
		
		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

void TBitmapImage::Allocate(const TImage& Image)
{
	Release();

	DEBUG_VERIFY(Image.IsAllocated());
	
	try
	{
		Allocate(TBitmapImageDesc(Image.GetSize()));

		const DWORD* pSrcData = Image.GetDataPtr();

		DWORD* pDstData = (DWORD*)m_Desc.m_pData + (m_Desc.m_Size.cy - 1) * m_Desc.m_Size.cx;
		
		for(size_t y = m_Desc.m_Size.cy ; y ; y--)
		{
			memcpy(pDstData, pSrcData, m_Desc.m_Size.cx << 2);
			pSrcData += m_Desc.m_Size.cx, pDstData -= m_Desc.m_Size.cx;
		}
	}
	
	catch(...)
	{
		Release(true);
		throw;
	}
}

void TBitmapImage::CreateImage(TImage& RImage) const
{
	DEBUG_VERIFY_ALLOCATION;

	CreateImageFromBitmapBits(m_Desc.m_BitmapInfo, m_Desc.m_pData, RImage);
}

// ----------------
// Global routines
// ----------------
void CreateImageFromBitmapBits(	const BITMAPINFO&	BitmapInfo,
								const void*			pData,
						   		TImage&				RImage)
{
	DEBUG_VERIFY(	BitmapInfo.bmiHeader.biWidth		>	0		&&
					BitmapInfo.bmiHeader.biHeight		>	0		&&
					BitmapInfo.bmiHeader.biCompression	==	BI_RGB	&&
					(	BitmapInfo.bmiHeader.biBitCount	== 16 ||
						BitmapInfo.bmiHeader.biBitCount	== 24 ||
						BitmapInfo.bmiHeader.biBitCount	== 32));

	RImage.Allocate(SZSIZE(	(size_t)BitmapInfo.bmiHeader.biWidth,
							(size_t)GetAbs((int)BitmapInfo.bmiHeader.biHeight)));

	DWORD* pDstData = RImage.GetDataPtr(SZPOINT(0, RImage.GetSize().cy - 1));
	const size_t szDstDelta = RImage.GetSize().cx << 1;

	if(BitmapInfo.bmiHeader.biBitCount == 16)
	{
		size_t szPitch = RImage.GetSize().cx;
		if(RImage.GetSize().cx & 1)
			szPitch++;

		const WORD* pSrcData = (const WORD*)pData;

		const size_t szSrcDelta = szPitch - RImage.GetSize().cx;

		for(size_t y = RImage.GetSize().cy ; y ; y--)
		{
			for(size_t x = RImage.GetSize().cx ; x ; x--)
			{
				register DWORD dwValue = 0xFF000000;

				dwValue |= (((*pSrcData)		& 0x1F) * 255 / 31);		// blue
				dwValue |= (((*pSrcData >> 5)	& 0x1F) * 255 / 31) << 8;	// green
				dwValue |= (((*pSrcData >> 10)	& 0x1F) * 255 / 31) << 16;	// red
				pSrcData++;

				*pDstData++ = dwValue;
			}

			pSrcData += szSrcDelta;
			pDstData -= szDstDelta;
		}
	}
	else if(BitmapInfo.bmiHeader.biBitCount == 24)
	{
		size_t szPitch = RImage.GetSize().cx * 3;
		if(szPitch & 3)
			szPitch += 4 - (szPitch & 3);
		
		const BYTE* pSrcData = (const BYTE*)pData;
		
		const size_t szSrcDelta = szPitch - RImage.GetSize().cx * 3;

		for(size_t y = RImage.GetSize().cy ; y ; y--)
		{
			for(size_t x = RImage.GetSize().cx ; x ; x--)
			{
				register DWORD dwValue = 0xFF000000;

				dwValue |= *pSrcData++;			// blue
				dwValue |= *pSrcData++ << 8;	// green
				dwValue |= *pSrcData++ << 16;	// red

				*pDstData++ = dwValue;
			}

			pSrcData += szSrcDelta;
			pDstData -= szDstDelta;
		}
	}
	else if(BitmapInfo.bmiHeader.biBitCount == 32)
	{
		const DWORD* pSrcData = (const DWORD*)pData;
		
		for(size_t y = RImage.GetSize().cy ; y ; y--)
		{
			for(size_t x = RImage.GetSize().cx ; x ; x--)
				*pDstData++ = 0xFF000000 | *pSrcData++;

			pDstData -= szDstDelta;
		}
	}
}

#endif // _MSC_VER
