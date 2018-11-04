#ifndef basic_controls_h
#define basic_controls_h

#include <KFC_KTL/text_container.h>
#include "sdl_control.h"
#include "sdl_image.h"
#include "sge_font.h"
#include "sdl_common.h"

// -------------------
// Button SDL control
// -------------------

// SDL control own definition
struct TButtonSDL_ControlOwnDefinition
{
public:
	UINT32 m_uiBG_Color;

	const T_SDL_MultiImage* m_pNormalImage;
	const T_SDL_MultiImage* m_pPushedImage;
	const T_SDL_MultiImage* m_pDisabledImage;
	const T_SDL_MultiImage* m_pFocusedImage;

	const T_SGE_Font* m_pNormalFont;
	const T_SGE_Font* m_pPushedFont;
	const T_SGE_Font* m_pDisabledFont;
	const T_SGE_Font* m_pFocusedFont;

	KString m_Text;

	UINT32 m_uiNormalTextColor;
	UINT32 m_uiPushedTextColor;
	UINT32 m_uiDisabledTextColor;
	UINT32 m_uiFocusedTextColor;

	ALSIZE	m_TextAlignment;
	ISIZE	m_TextOffset;
	ISIZE	m_PushedTextOffset;

	const T_SDL_Image* m_pNormalTextImage;
	const T_SDL_Image* m_pPushedTextImage;
	const T_SDL_Image* m_pDisabledTextImage;
	const T_SDL_Image* m_pFocusedTextImage;

	ALSIZE	m_TextImageAlignment;
	ISIZE	m_TextImageOffset;

	const T_SDL_Image* m_pNormalExtraImage;
	const T_SDL_Image* m_pPushedExtraImage;
	const T_SDL_Image* m_pDisabledExtraImage;
	const T_SDL_Image* m_pFocusedExtraImage;

	ALSIZE	m_ExtraImageAlignment;
	ISIZE	m_ExtraImageOffset;

public:
	TButtonSDL_ControlOwnDefinition();
};

// Button SDL control
class TButtonSDL_Control : public T_SDL_Control, private TButtonSDL_ControlOwnDefinition, public TTextContainer
{
public:
	// Definition
	struct TDefinition : public T_SDL_Control::TDefinition, public TButtonSDL_ControlOwnDefinition
	{
	public:
		T_SDL_Control::TDefinition* CreateCopy() const
			{ return new TDefinition(*this); }

		T_SDL_Control* CreateControl() const
			{ return new TButtonSDL_Control(*this); }
	};

private:
	bool m_bPushed;

private:
	bool OnNeedExtFocus() const
		{ return true; }

	void OnDraw(bool bFocusChange) const;

	size_t HandleEvent(T_KSDL_Event Event, size_t szParam);
	
	void OnKNOB(size_t szKey);

public:
	TButtonSDL_Control(const TDefinition& Definition);

	IPOINT GetExtraImageCoords() const;

	bool DrawExtraImage() const;

	UINT32 GetBG_Color() const
		{ return m_uiBG_Color; }

	void SetBG_Color(UINT32 uiBG_Color)
	{
		if(m_uiBG_Color == uiBG_Color)
			return;

		m_uiBG_Color = uiBG_Color;

		Redraw();
	}

	const T_SDL_MultiImage* GetImage(	const T_SDL_MultiImage*& pRPushedImage		= temp<const T_SDL_MultiImage*>(),
										const T_SDL_MultiImage*& pRDisabledImage	= temp<const T_SDL_MultiImage*>(),
										const T_SDL_MultiImage*& pRFocusedImage		= temp<const T_SDL_MultiImage*>())
	{
		pRPushedImage	= m_pPushedImage;
		pRDisabledImage	= m_pDisabledImage;
		pRFocusedImage	= m_pFocusedImage;

		return m_pNormalImage;
	}

	void SetImage(	const T_SDL_MultiImage* pNormalImage,
					const T_SDL_MultiImage* pPushedImage	= NULL,
					const T_SDL_MultiImage* pDisabledImage	= NULL,
					const T_SDL_MultiImage* pFocusedImage	= NULL)
	{
		// If 'pNormalImage' is 'NULL', all other images must also be 'NULL'
		DEBUG_VERIFY(!((pPushedImage || pDisabledImage || pFocusedImage) && !pNormalImage));

		// If 'pNormalImage' is 'NULL', 'm_pNormalTextImage' must also be 'NULL'
		DEBUG_VERIFY(!(!pNormalImage && m_pNormalTextImage));

		// If 'pNormalImage' is 'NULL', 'm_pNormalExtraImage' must also be 'NULL'
		DEBUG_VERIFY(!(!pNormalImage && m_pNormalExtraImage));

		m_pNormalImage		= pNormalImage;
		m_pPushedImage		= pPushedImage;		
		m_pDisabledImage	= pDisabledImage;
		m_pFocusedImage		= pFocusedImage;

		Redraw();
	}
	
	const T_SGE_Font* GetFont(	const T_SGE_Font*& pRPushedFont		= temp<const T_SGE_Font*>(),
								const T_SGE_Font*& pRDisabledFont	= temp<const T_SGE_Font*>(),
								const T_SGE_Font*& pRFocusedFont	= temp<const T_SGE_Font*>())
	{
		pRPushedFont	= m_pPushedFont;		
		pRDisabledFont	= m_pDisabledFont;
		pRFocusedFont	= m_pFocusedFont;

		return m_pNormalFont;
	}

	void SetFont(	const T_SGE_Font* pNormalFont,
					const T_SGE_Font* pPushedFont	= NULL,					
					const T_SGE_Font* pDisabledFont	= NULL,
					const T_SGE_Font* pFocusedFont	= NULL)
	{
		// If 'pNormalTextFont' is NULL, all other fonts must also be 'NULL'
		DEBUG_VERIFY(!((pPushedFont || pDisabledFont || pFocusedFont) && !pNormalFont));
		
		m_pNormalFont	= pNormalFont;
		m_pPushedFont	= pPushedFont;		
		m_pDisabledFont	= pDisabledFont;
		m_pFocusedFont	= pFocusedFont;

		Redraw();
	}

	KString GetText() const
		{ return m_Text; }

	void SetText(const KString& Text)
	{
		if(m_Text == Text)
			return;
			
		m_Text = Text;
	
		if(CanDraw() && IsTextButton())
			Redraw();
	}

	UINT GetTextColor(	UINT32& uiRPushedTextColor		= temp<UINT32>(),
						UINT32& uiRDisabledTextColor	= temp<UINT32>(),
						UINT32& uiRFocusedTextColor		= temp<UINT32>())
	{
		uiRPushedTextColor		= m_uiPushedTextColor;		
		uiRDisabledTextColor	= m_uiDisabledTextColor;
		uiRFocusedTextColor		= m_uiFocusedTextColor;
		
		return m_uiNormalTextColor;
	}		

	void SetTextColor(	UINT32 uiNormalTextColor,
						UINT32 uiPushedTextColor 	= UINT_MAX,
						UINT32 uiDisabledTextColor	= UINT_MAX,
						UINT32 uiFocusedTextColor 	= UINT_MAX)
	{
		m_uiNormalTextColor		= uiNormalTextColor;
		m_uiPushedTextColor		= uiPushedTextColor;		
		m_uiDisabledTextColor	= uiDisabledTextColor;
		m_uiFocusedTextColor	= uiFocusedTextColor;

		if(CanDraw() && IsTextButton())
			Redraw();
	}

	const ISIZE& GetTextOffset() const
		{ return m_TextOffset; }

	void SetTextOffset(const ISIZE& TextOffset)
	{
		if(m_TextOffset == TextOffset)
			return;

		m_TextOffset = TextOffset;

		if(CanDraw() && IsTextButton())
			Redraw();
	}

	const ALSIZE& GetTextAlignment() const
		{ return m_TextAlignment; }
		
	void SetTextAlignment(const ALSIZE& TextAlignment)
	{
		if(m_TextAlignment == TextAlignment)
			return;
	
		m_TextAlignment = TextAlignment;

		if(CanDraw() && IsTextButton())
			Redraw();
	}
	
	const ISIZE& GetPushedTextOffset() const
		{ return m_PushedTextOffset; }
		
	void SetPushedTextOffset(const ISIZE& PushedTextOffset)
	{
		if(m_PushedTextOffset == PushedTextOffset)
			return;
			
		m_PushedTextOffset = PushedTextOffset;
		
		if(CanDraw() && (IsTextButton() || IsTextImageButton()) && IsPushed())
			Redraw();
	}

	const T_SDL_Image* GetTextImage(const T_SDL_Image*& pRPushedTextImage	= temp<const T_SDL_Image*>(),
									const T_SDL_Image*& pRDisabledTextImage	= temp<const T_SDL_Image*>(),
									const T_SDL_Image*& pRFocusedTextImage	= temp<const T_SDL_Image*>())
	{
		pRPushedTextImage	= m_pPushedTextImage;		
		pRDisabledTextImage	= m_pDisabledTextImage;
		pRFocusedTextImage	= m_pFocusedTextImage;

		return m_pNormalTextImage;
	}
	
	void SetTextImage(	const T_SDL_Image* pNormalTextImage,
						const T_SDL_Image* pPushedTextImage		= NULL,						
						const T_SDL_Image* pDisabledTextImage	= NULL,
						const T_SDL_Image* pFocusedTextImage	= NULL)
	{
		// If 'pNormalTextImage' is 'NULL', all other text images must also be 'NULL'
		DEBUG_VERIFY(!((pPushedTextImage || pDisabledTextImage || pFocusedTextImage) && !pNormalTextImage));

		// If 'm_pNormalImage' is 'NULL', 'pNormalTextImage' must also be 'NULL'
		DEBUG_VERIFY(!(!m_pNormalImage && pNormalTextImage));

		m_pNormalTextImage		= pNormalTextImage;
		m_pPushedTextImage		= pPushedTextImage;
		m_pDisabledTextImage	= pDisabledTextImage;
		m_pFocusedTextImage		= pFocusedTextImage;		

		Redraw();
	}

	const ALSIZE& GetTextImageAlignment() const
		{ return m_TextImageAlignment; }

	void SetTextImageAlignment(const ALSIZE& TextImageAlignment)
	{
		if(m_TextImageAlignment == TextImageAlignment)
			return;

		m_TextImageAlignment = TextImageAlignment;

		if(CanDraw() && IsTextImageButton())
			Redraw();
	}

	const ISIZE& GetTextImageOffset() const
		{ return m_TextImageOffset; }

	void SetTextImageOffset(const ISIZE& TextImageOffset)
	{
		if(m_TextImageOffset == TextImageOffset)
			return;

		m_TextImageOffset = TextImageOffset;

		if(CanDraw() && IsTextImageButton())
			Redraw();
	}

	const T_SDL_Image* GetExtraImage(	const T_SDL_Image*& pRPushedExtraImage		= temp<const T_SDL_Image*>(),
										const T_SDL_Image*& pRDisabledExtraImage	= temp<const T_SDL_Image*>(),
										const T_SDL_Image*& pRFocusedExtraImage		= temp<const T_SDL_Image*>())
	{
		pRPushedExtraImage		= m_pPushedExtraImage;		
		pRDisabledExtraImage	= m_pDisabledExtraImage;
		pRFocusedExtraImage		= m_pFocusedExtraImage;

		return m_pNormalExtraImage;
	}
	
	void SetExtraImage(	const T_SDL_Image* pNormalExtraImage,
						const T_SDL_Image* pPushedExtraImage	= NULL,
						const T_SDL_Image* pDisabledExtraImage	= NULL,
						const T_SDL_Image* pFocusedExtraImage	= NULL)
	{
		// If 'pNormalExtraImage' is 'NULL', all other extra images must also be 'NULL'
		DEBUG_VERIFY(!((pPushedExtraImage || pDisabledExtraImage || pFocusedExtraImage) && !pNormalExtraImage));

		// If 'm_pNormalImage' is 'NULL', 'pNormalExtraImage' must also be 'NULL'
		DEBUG_VERIFY(!(!m_pNormalImage && pNormalExtraImage));

		m_pNormalExtraImage		= pNormalExtraImage;
		m_pPushedExtraImage		= pPushedExtraImage;
		m_pDisabledExtraImage	= pDisabledExtraImage;
		m_pFocusedExtraImage	= pFocusedExtraImage;		

		Redraw();
	}	

	const ALSIZE& GetExtraImageAlignment() const
		{ return m_ExtraImageAlignment; }

	void SetExtraImageAlignment(const ALSIZE& ExtraImageAlignment)
	{
		if(m_ExtraImageAlignment == ExtraImageAlignment)
			return;

		m_ExtraImageAlignment = ExtraImageAlignment;

		if(CanDraw() && IsExtraImageButton())
			Redraw();
	}

	const ISIZE& GetExtraImageOffset() const
		{ return m_ExtraImageOffset; }

	void SetExtraImageOffset(const ISIZE& ExtraImageOffset)
	{
		if(m_ExtraImageOffset == ExtraImageOffset)
			return;

		m_ExtraImageOffset = ExtraImageOffset;

		if(CanDraw() && IsExtraImageButton())
			Redraw();
	}

	bool IsImageButton() const
		{ return m_pNormalImage; }

	bool IsTextButton() const
		{ return m_pNormalFont; }

	bool IsTextImageButton() const
		{ return m_pNormalTextImage; }

	bool IsExtraImageButton() const
		{ return m_pNormalExtraImage; }

	bool IsPushed() const
		{ return m_bPushed; }
		
	void Push(bool bPush = true);
};

// Button SDL control loaders
T_SDL_ControlLoader ButtonSDL_ControlLoader;
T_SDL_ControlLoader LabelSDL_ControlLoader;
T_SDL_ControlLoader ImageSDL_ControlLoader;

// -------------------------
// Progress bar SDL control
// -------------------------

// Progress bar SDL control own definition
struct TProgressBarSDL_ControlOwnDefinition
{
public:
	UINT32 m_uiBG_Color;
	UINT32 m_uiFG_Color;	

public:
	TProgressBarSDL_ControlOwnDefinition();
};

// Progress barSDL control
class TProgressBarSDL_Control : public T_SDL_Control, public TProgressBarSDL_ControlOwnDefinition
{
public:
	// Definition
	struct TDefinition : public T_SDL_Control::TDefinition, public TProgressBarSDL_ControlOwnDefinition
	{
		T_SDL_Control::TDefinition* CreateCopy() const
			{ return new TDefinition(*this); }

		T_SDL_Control* CreateControl() const
			{ return new TProgressBarSDL_Control(*this); }
	};

private:
	DSEGMENT m_Range;
	double   m_dPos;

private:
	void OnDraw(bool bFocusChange) const;

private:
	void DynamicRedraw(int iOldCoord, int iNewCoord);

	int GetSplitCoord() const
	{
		DEBUG_VERIFY(GetRect().IsValid());
		return (int)((m_dPos - m_Range.m_First) * GetRect().GetWidth() / m_Range.GetLength());
	}

	IRECT GetBG_Rect() const
	{
		IRECT Rect = GetRect();
		Rect.m_Left += GetSplitCoord();
		return Rect;
	}

	IRECT GetFG_Rect() const
	{
		IRECT Rect = GetRect();
		Rect.m_Right = Rect.m_Left + GetSplitCoord();
		return Rect;
	}

public:
	TProgressBarSDL_Control(const TDefinition& Definition);

	const DSEGMENT& GetRange() const
		{ return m_Range; }

	void SetRange(const DSEGMENT& Range)
	{
		DEBUG_VERIFY(Range.IsValid());

		int iOldCoord = GetSplitCoord();

		m_Range = Range;
		LimitValue(m_dPos, m_Range);

		int iNewCoord = GetSplitCoord();

		DynamicRedraw(iOldCoord, iNewCoord);
	}

	double GetPos() const
		{ return m_dPos; }

	void SetPos(double dPos)
	{
		int iOldCoord = GetSplitCoord();

		m_dPos = dPos;
		LimitValue(m_dPos, m_Range);

		int iNewCoord = GetSplitCoord();

		DynamicRedraw(iOldCoord, iNewCoord);
	}
};

// Progress bar SDL control loader
T_SDL_ControlLoader ProgressBarSDL_ControlLoader;

// -----------------
// List SDL control
// -----------------

// List SDL control own definition
struct TListSDL_ControlOwnDefinition
{
public:
	// Column
	struct TColumn
	{
		KString		m_Title;
		size_t		m_szWidth;
		TAlignment	m_Alignment;
		size_t		m_szTextOffset;
	};

public:
	const T_SDL_Image* m_pArrowUpImage;
	const T_SDL_Image* m_pArrowDnImage;

	const T_SGE_Font* m_pHeaderFont;
	const T_SGE_Font* m_pItemFont;	

	size_t m_szHeaderHeight;
	size_t m_szItemHeight;
	size_t m_szNVisibleItems;
	size_t m_szMinThumbHeight;

	size_t m_szHeaderSpacing;
	size_t m_szTextsEdgeOffset;

	UINT32 m_uiHeaderColor;
	UINT32 m_uiNormalItemColor;
	UINT32 m_uiSelectedItemColor;
	UINT32 m_uiNormalBG_Color;
	UINT32 m_uiSelectedBG_Color;

	TArray<TColumn> m_Columns;

	bool m_bHasHeader;

	bool m_bNeedExtFocus;

public:
	TListSDL_ControlOwnDefinition();

	size_t GetTotalHeaderHeight() const
		{ return m_bHasHeader ? m_szHeaderHeight + 1 : 0; }
};

// List SDL control
class TListSDL_Control : public T_SDL_Control, private TListSDL_ControlOwnDefinition
{
public:
	// Definition
	struct TDefinition : public T_SDL_Control::TDefinition, public TListSDL_ControlOwnDefinition
	{
	public:
		T_SDL_Control::TDefinition* CreateCopy() const
			{ return new TDefinition(*this); }

		T_SDL_Control* CreateControl() const
			{ return new TListSDL_Control(*this); }
	};

private:
	bool OnNeedExtFocus() const
		{ return m_bNeedExtFocus; }

	void OnDraw(bool bFocusChange) const;

	void OnKNOB(size_t szKey);

private:
	void DrawHeader() const;

	void DrawItems() const
	{
		for(size_t i = 0 ; i < m_szNVisibleItems ; i++)
			DrawItem(m_szFirstVisibleItem + i);
	}

	void DrawItem(size_t szIndex) const;

	void DrawScrollElements() const;

	void DrawThumbBG(const ISEGMENT& YRange) const;

	void DrawThumb(const IRECT& ThumbRect) const;

	void RedrawThumb(const IRECT& OldThumbRect, const IRECT& NewThumbRect) const;

private:
	size_t HandleEvent(T_KSDL_Event Event, size_t szParam);

	IRECT GetThumbRect() const;

	bool HasScrolling() const
		{ return GetNVisibleItems() < GetNItems(); }

	int GetRightEdge() const
		{ return HasScrolling() ? GetRect().m_Right - m_pArrowUpImage->GetWidth() - 2 : GetRect().m_Right; }

private:
	TArray<TArray<KString> >					m_Items;
	TArray<TArray<UINT32, true> >				m_NormalItemColors;
	TArray<TArray<UINT32, true> >				m_SelectedItemColors;
	TArray<TArray<const T_SGE_Font*, true> >	m_ItemFonts;
	TArray<size_t, true>						m_ItemRedrawBlocks;

	size_t m_szFirstVisibleItem;

	size_t m_szSelectedItem;

	bool m_bSelecting;

	bool m_bRunning;

public:
	TListSDL_Control(const TDefinition& Definition);	

	size_t GetNColumns() const
		{ return m_Columns.GetN(); }

	size_t GetNVisibleItems() const
		{ return m_szNVisibleItems; }

	size_t GetNItems() const
		{ return m_Items.GetN(); }

	bool IsEmpty() const
		{ return !GetNItems(); }

	void Clear();

	size_t GetSelectedItem() const
		{ return m_szSelectedItem; }

	void SelectItem(size_t szIndex, bool bEnsureVisible = true);

	bool EnsureVisible(size_t szIndex);

	bool IsItemSelected(size_t szIndex) const
	{
		DEBUG_VERIFY(szIndex < GetNItems());

		return szIndex == GetSelectedItem();
	}

	bool HasSelection() const
		{ return GetSelectedItem() != UINT_MAX; }

	const KString& GetItem(size_t szIndex, size_t szColumn) const
		{ return m_Items[szIndex][szColumn]; }

	void SetItem(size_t szIndex, size_t szColumn, LPCTSTR pText);

	void SetItem(size_t szIndex, LPCTSTR pText)
	{
		DEBUG_VERIFY(GetNColumns() == 1);

		DEBUG_VERIFY(szIndex < GetNItems());

		m_Items[szIndex][0] = pText;

		if(CanDraw())
			DrawItem(szIndex);
	}

	void SetItem(size_t szIndex, LPCTSTR pText1, LPCTSTR pText2)
	{
		DEBUG_VERIFY(GetNColumns() == 2);

		DEBUG_VERIFY(szIndex < GetNItems());

		m_Items[szIndex][0] = pText1;
		m_Items[szIndex][1] = pText2;

		if(CanDraw())
			DrawItem(szIndex);
	}

	void SetItem(size_t szIndex, LPCTSTR pText1, LPCTSTR pText2, LPCTSTR pText3)
	{
		DEBUG_VERIFY(GetNColumns() == 3);

		DEBUG_VERIFY(szIndex < GetNItems());

		m_Items[szIndex][0] = pText1;
		m_Items[szIndex][1] = pText2;
		m_Items[szIndex][2] = pText3;

		if(CanDraw())
			DrawItem(szIndex);
	}

	void SetItem(size_t szIndex, LPCTSTR pText1, LPCTSTR pText2, LPCTSTR pText3, LPCTSTR pText4)
	{
		DEBUG_VERIFY(GetNColumns() == 4);

		DEBUG_VERIFY(szIndex < GetNItems());

		m_Items[szIndex][0] = pText1;
		m_Items[szIndex][1] = pText2;
		m_Items[szIndex][2] = pText3;
		m_Items[szIndex][3] = pText4;

		if(CanDraw())
			DrawItem(szIndex);
	}

	UINT32 GetItemColor(size_t szIndex, size_t szColumn, UINT32& uiRSelectedColor = temp<UINT32>()) const
	{
		DEBUG_VERIFY(szIndex < m_Items.GetN());
		DEBUG_VERIFY(szColumn < m_Columns.GetN());

		uiRSelectedColor = m_SelectedItemColors[szIndex][szColumn];

		return m_NormalItemColors[szIndex][szColumn];
	}

	void SetItemColor(size_t szIndex, size_t szColumn, UINT32 uiNormalColor, UINT32 uiSelectedColor = UINT_MAX);

	const T_SGE_Font* GetItemFont(size_t szIndex, size_t szColumn) const
	{
		DEBUG_VERIFY(szIndex < m_Items.GetN());
		DEBUG_VERIFY(szColumn < m_Columns.GetN());

		return m_ItemFonts[szIndex][szColumn];
	}

	void SetItemFont(size_t szIndex, size_t szColumn, const T_SGE_Font* pFont);

	size_t AddItem(bool bRedraw = true);

	size_t AddItem(LPCTSTR pText)
		{ size_t szIndex = AddItem(false); SetItem(szIndex, pText); return szIndex; }

	size_t AddItem(LPCTSTR pText1, LPCTSTR pText2)
		{ size_t szIndex = AddItem(false); SetItem(szIndex, pText1, pText2); return szIndex; }

	size_t AddItem(LPCTSTR pText1, LPCTSTR pText2, LPCTSTR pText3)
		{ size_t szIndex = AddItem(false); SetItem(szIndex, pText1, pText2, pText3); return szIndex; }

	size_t AddItem(LPCTSTR pText1, LPCTSTR pText2, LPCTSTR pText3, LPCTSTR pText4)
		{ size_t szIndex = AddItem(false); SetItem(szIndex, pText1, pText2, pText3, pText4); return szIndex; }

	size_t GetFirstVisibleItem() const
		{ return m_szFirstVisibleItem; }

	size_t SetFirstVisibleItem(size_t szIndex);

	void EnterSelectMode();
};

// List SDL control loaders
T_SDL_ControlLoader ListSDL_ControlLoader;

#endif // basic_controls_h
