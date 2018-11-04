#ifndef gui_back_buffer_h
#define gui_back_buffer_h

#include "gui.h"

// ----------------
// GUI back buffer
// ----------------
class TGUIBackBuffer
{
private:
	bool m_bAllocated;

	TDC m_DC;

	TBitmap m_Bitmap;

	HBITMAP m_hOldBitmap;

	SZSIZE m_LastSize;

public:
	TGUIBackBuffer();

	~TGUIBackBuffer()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate();

	TDC& GetDC();
};

#endif // gui_back_buffer_h