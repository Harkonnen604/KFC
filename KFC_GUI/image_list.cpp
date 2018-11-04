#include "kfc_gui_pch.h"
#include "image_list.h"

// -----------
// Image list
// -----------
TImageList::TImageList()
{
	m_hImageList = NULL;
}

void TImageList::Release()
{
	if(m_hImageList)
		ImageList_Destroy(m_hImageList), m_hImageList = NULL;
}

void TImageList::Allocate(	const SZSIZE&	Size,
							kflags_t		flFlags,
							size_t			szInitial,
							size_t			szGrow)
{
	Release();

	DEBUG_VERIFY(Size.IsPositive());

	m_hImageList = ImageList_Create(Size.cx, Size.cy, flFlags, szInitial, szGrow);

	if(!m_hImageList)
		INITIATE_DEFINED_FAILURE(TEXT("Error creating image list."));
}

size_t TImageList::AddImage(HBITMAP hImage, HBITMAP hMask)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(hImage);

	size_t szIndex = ImageList_Add(*this, hImage, hMask);

	if(szIndex == UINT_MAX)
		INITIATE_DEFINED_FAILURE(TEXT("Error adding image to image list."));

	return szIndex;
}

void TImageList::ReplaceImage(size_t szIndex, HBITMAP hImage, HBITMAP hMask)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(hImage);

	if(ImageList_Replace(*this, szIndex, hImage, hMask) == -1)
		INITIATE_DEFINED_FAILURE(TEXT("Error replacing image of image list."));
}

size_t TImageList::AddMaskedImage(HBITMAP hImage, COLORREF crMaskColor)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(hImage);

	size_t szIndex = ImageList_AddMasked(*this, hImage, crMaskColor);

	if(szIndex == UINT_MAX)
		INITIATE_DEFINED_FAILURE(TEXT("Error adding masked image to image list."));

	return szIndex;
}

size_t TImageList::AddIcon(HICON hIcon)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(hIcon);

	size_t szIndex = ImageList_AddIcon(*this, hIcon);

	if(szIndex == UINT_MAX)
		INITIATE_DEFINED_FAILURE(TEXT("Error adding icon to image list."));

	return szIndex;
}

void TImageList::ReplaceIcon(size_t szIndex, HICON hIcon)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(hIcon);

	if(ImageList_ReplaceIcon(*this, szIndex, hIcon) == -1)
		INITIATE_DEFINED_FAILURE(TEXT("Error replacing icon of image list."));
}
