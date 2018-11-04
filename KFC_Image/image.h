#ifndef image_h
#define image_h

#include <KFC_KTL/vmatrix.h>

// --------------
// Bit blit mode
// --------------
enum TBitBlitMode
{
	BBM_COPY						= 1,
	BBM_ALPHABLIT					= 2,
	BBM_ONLY_FULL_SRC_ALPHA_PIXELS	= 3,
	BBM_ONLY_FULL_DST_ALPHA_PIXELS	= 4,
	BBM_FORCE_UINT					= UINT_MAX,
};

// ------
// Image
// ------
class TImage
{
private:
	bool m_bAllocated;

	TValueMatrix<DWORD, true>	m_Pixels;
	SZSIZE						m_Size;

private:
	void LoadAsBMP	(const KString& FilaName);
	void LoadAsJPEG	(const KString& FileName);
	void LoadAsTGA	(const KString& FileName);
	void LoadAsTIFF	(const KString& FileName);
	void LoadAsPNG	(const KString& FileName);

	void SaveAsBMP	(const KString& FileName) const;
	void SaveAsJPEG	(const KString& FileName, int iQuality) const;
	void SaveAsTGA	(const KString& FileName) const;
	void SaveAsTIFF	(const KString& FileName) const;
	void SaveAsPNG	(const KString& FileName) const;

public:
	TImage();

	TImage(const SZSIZE& SSize);

	TImage(const KString& FileName);

	TImage(const TImage& Image, const SZRECT& Rect); // cropped

	TImage(const TImage& Image, const SZSIZE& Size); // resized
	
	~TImage()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	TImage& Allocate(const SZSIZE& SSize);

	TImage& CreateCropped(const TImage& SrcImage, const SZRECT& Rect);

	TImage& CreateResized(const TImage& SrcImage, const SZSIZE& SSize);

	TImage& Clear()
	{
		DEBUG_VERIFY_ALLOCATION;

		memset(GetDataPtr(), 0x00, m_Size.GetArea() * sizeof(DWORD));

		return *this;
	}

	TImage& FillWhite()
	{
		DEBUG_VERIFY_ALLOCATION;

		memset(GetDataPtr(), 0xFF, m_Size.GetArea() * sizeof(DWORD));

		return *this;
	}

	TImage& Fill(DWORD dwColor)
	{
		DEBUG_VERIFY_ALLOCATION;

		DWORD* pData = GetDataPtr();

		for(size_t i = m_Size.GetArea() ; i ; i--)
			*pData++ = dwColor;

		return *this;
	}

	DWORD GetPixel(const SZPOINT& Coords) const
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_Pixels.GetDataRef(Coords);
	}

	void SetPixel(const SZPOINT& Coords, DWORD dwColor)
	{
		DEBUG_VERIFY_ALLOCATION;

		m_Pixels.GetDataRef(Coords) = dwColor;
	}

	TImage& BitBlit(const TImage&	SrcImage,
					IPOINT			DstCoords	= IPOINT(0, 0),
					const IRECT*	pSrcRect	= NULL,
					TBitBlitMode	Mode		= BBM_COPY,
					const ALSIZE&	Alignment	= ALSIZE(ALIGNMENT_MIN, ALIGNMENT_MIN),
					float			fMulAlpha	= 1.0f);

	TImage& DrawLine(	const IPOINT&	Coords1,
						const IPOINT&	Coords2,
						DWORD			dwColor,
						const IRECT*	pClipRect = NULL);

	void Load(const KString& FileName);

	void Save(const KString& FileName, int iQuality = 90) const;	

	static size_t GetFileSize(const SZSIZE& Size)
		{ return Size.GetArea() * sizeof(DWORD); }

	DWORD* GetDataPtr()
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_Pixels.GetDataPtr();
	}

	const DWORD* GetDataPtr() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_Pixels.GetDataPtr();
	}

	DWORD* GetDataPtr(const SZPOINT& Coords)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(Coords.x < m_Size.cx && Coords.y < m_Size.cy);

		return m_Pixels.GetDataPtr(Coords);
	}

	const DWORD* GetDataPtr(const SZPOINT& Coords) const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(Coords.x < m_Size.cx && Coords.y < m_Size.cy);

		return m_Pixels.GetDataPtr(Coords);
	}

	const SZSIZE& GetSize() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Size; }

	IRECT GetRect() const
		{ DEBUG_VERIFY_ALLOCATION; return IRECT(0, 0, m_Size.cx, m_Size.cy); }

	size_t GetWidth() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Size.cx; }

	size_t GetHeight() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Size.cy; }
};

DECLARE_STREAMING(TImage);

#endif // image_h
