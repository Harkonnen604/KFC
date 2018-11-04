#ifndef gui_elements_h
#define gui_elements_h

// ----------------
// Global routines
// ----------------
size_t MeasureCheckText(HDC hDC);

size_t MeasureNoCheckText(HDC hDC);

void DrawCheckText(	HDC				hDC,
					LPCTSTR			pText,
					const IRECT&	Rect,
					TAlignment		Alignment,
					bool			bChecked,
					bool			bSelected,								
					bool			bActiveControl, // i.e. has focus
					const ISIZE&	BorderSpacing	= ISIZE(0,0), // flat - near text, positive - from cell
					COLORREF		crTextBGColor	= CLR_INVALID,
					COLORREF		crTextFGColor	= CLR_INVALID,
					kflags_t		flFlags			= DT_NOPREFIX | DT_WORD_ELLIPSIS); // 'DT_SINGLELINE | DT_VCENTER' are added anyway

void DrawNoCheckText(	HDC				hDC,
						LPCTSTR			pText,
						const IRECT&	Rect,
						TAlignment		Alignment,
						bool			bSelected,									
						bool			bActiveControl, // i.e. has focus
						const ISIZE&	BorderSpacing	= ISIZE(0,0), // flat - near text, positive - from cell
						COLORREF		crTextBGColor	= CLR_INVALID,
						COLORREF		crTextFGColor	= CLR_INVALID,
						kflags_t		flFlags			= DT_NOPREFIX | DT_WORD_ELLIPSIS); // 'DT_SINGLELINE | DT_VCENTER' are added anyway

bool TogglesCheckText(	const IRECT&	Rect,
						const IPOINT&	Coords);

#endif // gui_elements_h