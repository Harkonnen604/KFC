#ifndef window_dragger_h
#define window_dragger_h

// ---------------
// Window dragger
// ---------------
class TWindowDragger
{
private:
	HWND m_hWnd;

	bool m_bDragged;

	ISIZE m_MouseCoordsOffset;

	IRECT m_DragRect;

	IRECT m_LimitRect;

private:
	void ResetWindowCoords();

	void SetWindowCoords(IPOINT Coords);

	void UpdateWindowPos();
	
public:
	TWindowDragger();

	~TWindowDragger()
		{ Release(); }

	bool IsAllocated() const
		{ return m_hWnd; }
	
	void Release();

	void Allocate(	HWND			hWnd,
					const IRECT&	DragRect	= IRECT(),
					const IRECT&	LimitRect	= IRECT());

	void SetDragRect(const IRECT& DragRect);

	void SetLimitRect(const IRECT& LimitRect);

	bool ProcessMessage(UINT uiMsg);

	bool IsDragged() const
		{ DEBUG_VERIFY_ALLOCATION; return m_bDragged; }
};

#endif // window_dragger_h