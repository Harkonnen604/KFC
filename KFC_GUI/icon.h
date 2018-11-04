#ifndef icon_h
#define icon_h

#include <KFC_Common\module.h>

// -----
// Icon
// -----
class TIcon
{
private:
	HICON m_hIcon;

public:
	TIcon();

	TIcon(HICON hSIcon);

	TIcon(HINSTANCE hInstance, UINT uiID);

	TIcon(UINT uiID);

	~TIcon()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hIcon; }

	void Release();

	void Allocate(HICON hSIcon);	

	void Allocate(HINSTANCE hInstance, UINT uiID)
		{ Allocate(LoadIcon(hInstance, MAKEINTRESOURCE(uiID))); }

	void Allocate(UINT uiID)
		{ Allocate(GetKModuleHandle(), uiID); }

	HICON GetIcon() const;

	operator HICON() const { return m_hIcon; }
};

#endif // icon_h