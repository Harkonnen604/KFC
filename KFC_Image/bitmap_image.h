#ifndef bitmap_image_h
#define bitmap_image_h

#ifdef _MSC_VER

#include "image.h"

// Bitmap image load struct flags
#define BILSF_ALLOW_SHRINK          (0x0001)
#define BILSF_ALLOW_ENLARGE         (0x0002)
#define BILSF_SHRINK_FILTERING      (0x0004)
#define BILSF_ENLARGE_FILTERING     (0x0008)
#define BILSF_ERRORTEXT_IN_IMAGE    (0x0010)

// Bitmap image load struct default flags
#define BILSF_DEFAULT   (BILSF_ALLOW_SHRINK | BILSF_ERRORTEXT_IN_IMAGE)

// ------------------
// Bitmap image desc
// ------------------
struct TBitmapImageDesc
{
    SZSIZE      m_Size;
    BITMAPINFO  m_BitmapInfo;
    void*       m_pData;


    TBitmapImageDesc();

    TBitmapImageDesc(   const SZSIZE&   SSize,
                        bool            bInversedOrientation = true);

    TBitmapImageDesc(const BITMAPINFO& SBitmapInfo);

    void SetDefaults();

    void SetSize(   const SZSIZE&   SSize,
                    bool            bInversedOrientation = true);

    void SetBitmapInfo(const BITMAPINFO& SBitmapInfo);

    bool operator == (const TBitmapImageDesc& SDesc) const;

    bool operator != (const TBitmapImageDesc& SDesc) const
        { return !(*this == SDesc); }
};

// -------------
// Bitmap Image
// -------------
class TBitmapImage
{
private:
    bool m_bAllocated;

    HBITMAP m_hBitmap;

    TBitmapImageDesc m_Desc;

public:
    TBitmapImage();

    TBitmapImage(const TBitmapImageDesc& SDesc);

    TBitmapImage(   const SZSIZE&   SSize,
                    bool            bInversedOrientation = true);

    TBitmapImage(const TImage& Image);

    ~TBitmapImage() { Release(); }

    bool IsAllocated() const
        { return m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(const TBitmapImageDesc& SDesc);

    void Allocate(  const SZSIZE&   SSize,
                    bool            bInversedOrientation = true)
    {
        Allocate(TBitmapImageDesc(SSize, bInversedOrientation));
    }

    void Allocate(const TImage& Image);

    void CreateImage(TImage& RImage) const;

    HBITMAP GetBitmap() const
    {
        DEBUG_VERIFY_ALLOCATION;

        return m_hBitmap;
    }

    operator HBITMAP () const { return GetBitmap(); }

    HGDIOBJ GetGDIObject() const
    {
        DEBUG_VERIFY_ALLOCATION;

        return m_hBitmap;
    }

    operator HGDIOBJ () const { return GetGDIObject(); }

    void* GetDataPtr()
    {
        DEBUG_VERIFY_ALLOCATION;

        return m_Desc.m_pData;
    }

    const void* GetDataPtr() const
    {
        DEBUG_VERIFY_ALLOCATION;

        return m_Desc.m_pData;
    }

    const SZSIZE& GetSize() const
    {
        DEBUG_VERIFY_ALLOCATION;

        return m_Desc.m_Size;
    }

    // ------------------ TRIVIALS --------------------
    const TBitmapImageDesc& GetDesc() const { return m_Desc; }

    bool DoesHaveInversedOrientation() const { return m_Desc.m_BitmapInfo.bmiHeader.biHeight < 0; }
};

// ----------------
// Global routines
// ----------------
void CreateImageFromBitmapBits( const BITMAPINFO&   BitmapInfo,
                                const void*         pData,
                                TImage&             RImage);

#endif // _MSC_VER

#endif // bitmap_image_h
