#ifndef image_list_h
#define image_list_h

// -----------
// Image list
// -----------
class TImageList
{
private:
	HIMAGELIST m_hImageList;

public:
	TImageList();

	~TImageList()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hImageList; }

	void Release();

	void Allocate(	const SZSIZE&	Size,
					kflags_t		flFlags		= ILC_COLOR | ILC_MASK,
					size_t			szInitial	= 4,
					size_t			szGrow		= 4);

	size_t AddImage(HBITMAP hImage, HBITMAP hMask = NULL);

	void ReplaceImage(size_t szIndex, HBITMAP hImage, HBITMAP hMask = NULL);

	size_t AddMaskedImage(HBITMAP hImage, COLORREF crMaskColor);

	size_t AddIcon(HICON hIcon);

	size_t AddIcon(int iID, HINSTANCE hInstance = GetModuleHandle(NULL))
		{ return AddIcon(LoadIcon(hInstance, MAKEINTRESOURCE(iID))); }

	void ReplaceIcon(size_t szIndex, HICON hIcon);

	void ReplaceIcon(size_t szIndex, int iID)
		{  ReplaceIcon(szIndex, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(iID))); }

	HIMAGELIST GetImageList() const
		{ DEBUG_VERIFY_ALLOCATION; return m_hImageList; }

	operator HIMAGELIST () const
		{ return GetImageList(); }
};

#endif // image_list_h
