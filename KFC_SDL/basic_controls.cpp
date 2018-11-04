#include "kfc_sdl_pch.h"
#include "basic_controls.h"

#include "sdl_tls_item.h"
#include "sdl_consts.h"
#include "sdl_interface.h"
#include "sdl_resource_storage.h"

// -------------------
// Button SDL control
// -------------------

// Button SDL control own definition
TButtonSDL_ControlOwnDefinition::TButtonSDL_ControlOwnDefinition()
{
	m_uiBG_Color = UINT_MAX;

	m_pNormalImage		= NULL;	
	m_pPushedImage		= NULL;	
	m_pDisabledImage	= NULL;
	m_pFocusedImage		= NULL;

	m_pNormalFont	= NULL;
	m_pPushedFont	= NULL;
	m_pDisabledFont	= NULL;
	m_pFocusedFont	= NULL;

	m_uiNormalTextColor		= RGB(255, 255, 255);
	m_uiPushedTextColor		= UINT_MAX;
	m_uiDisabledTextColor	= UINT_MAX;
	m_uiFocusedTextColor	= UINT_MAX;	

	m_TextOffset.Set(0, 0);
	m_TextAlignment.Set(ALIGNMENT_MID, ALIGNMENT_MID);

	m_PushedTextOffset.Set(2, 2);

	m_pNormalTextImage		= NULL;
	m_pPushedTextImage		= NULL;
	m_pDisabledTextImage	= NULL;
	m_pFocusedTextImage		= NULL;	

	m_TextImageOffset.Set(8, 0);
	m_TextImageAlignment.Set(ALIGNMENT_MIN, ALIGNMENT_MID);

	m_pNormalExtraImage		= NULL;
	m_pPushedExtraImage		= NULL;
	m_pDisabledExtraImage	= NULL;
	m_pFocusedExtraImage	= NULL;	

	m_ExtraImageOffset.Set(8, 0);
	m_ExtraImageAlignment.Set(ALIGNMENT_MAX, ALIGNMENT_MID);
}

// Button SDL control
TButtonSDL_Control::TButtonSDL_Control(const TDefinition& Definition) :
	T_SDL_Control(Definition), TButtonSDL_ControlOwnDefinition(Definition)
{
	// If 'm_pNormalImage' is 'NULL', all other images must also be 'NULL'
	DEBUG_VERIFY(!((m_pPushedImage || m_pDisabledImage || m_pFocusedImage) && !m_pNormalImage));

	// If 'm_pNormalTextFont' is NULL, all other fonts must also be 'NULL'
	DEBUG_VERIFY(!((m_pPushedFont || m_pDisabledFont || m_pFocusedFont) && !m_pNormalFont));

	// If 'm_pNormalTextImage' is 'NULL', all other text images must also be 'NULL'
	DEBUG_VERIFY(!((m_pPushedTextImage || m_pDisabledTextImage || m_pFocusedTextImage) && !m_pNormalTextImage));

	// If 'm_pNormalImage' is 'NULL', 'm_pNormalTextImage' must also be 'NULL'
	DEBUG_VERIFY(!(!m_pNormalImage && m_pNormalTextImage));

	// If 'm_pNormalTextImage' is 'NULL', all other text images must also be 'NULL'
	DEBUG_VERIFY(!((m_pPushedTextImage || m_pDisabledTextImage || m_pFocusedTextImage) && !m_pNormalTextImage));

	// If 'm_pNormalExtraImage' is 'NULL', all other exra images must also be 'NULL'
	DEBUG_VERIFY(!((m_pPushedExtraImage || m_pDisabledExtraImage || m_pFocusedExtraImage) && !m_pNormalExtraImage));

	// If 'm_pNormalImage' is 'NULL', 'm_pNormalTextImage' must also be 'NULL'
	DEBUG_VERIFY(!(!m_pNormalImage && m_pNormalTextImage));

	// If 'm_pNormalImage' is 'NULL', 'm_pNormalExtraImage' must also be 'NULL'
	DEBUG_VERIFY(!(!m_pNormalImage && m_pNormalExtraImage));

	m_bPushed = false;
}

void TButtonSDL_Control::OnDraw(bool bFocusChange) const
{
	DEBUG_VERIFY(IsAttached());

	if(bFocusChange)
	{
		bool bDiffers = false;

		if(IsImageButton())
		{
			if(m_pFocusedImage && m_pFocusedImage != m_pNormalImage)
				bDiffers = true;

			if(m_pFocusedTextImage && m_pFocusedTextImage != m_pNormalTextImage)
				bDiffers = true;

			if(m_pFocusedExtraImage && m_pFocusedExtraImage != m_pNormalExtraImage)
				bDiffers = true;
		}

		if(IsTextButton())
		{
			if(m_pFocusedFont && m_pFocusedFont != m_pNormalFont)
				bDiffers = true;

			if(m_uiFocusedTextColor != UINT_MAX && m_uiFocusedTextColor != m_uiNormalTextColor)
				bDiffers = true;
		}

		if(!bDiffers)
			return;
	}

	IRECT TextRect = GetRect();

	if(IsImageButton())
	{
		const T_SDL_MultiImage* pImage =
			IsPushed() ? m_pPushedImage : !IsEnabled() ? m_pDisabledImage : IsFocused() ? m_pFocusedImage : m_pNormalImage;

		if(!pImage)
			pImage = m_pNormalImage;

		DEBUG_VERIFY(pImage);

		pImage->Draw(g_pSDL_FB, GetRect());

		if(IsTextImageButton())
		{
			const T_SDL_Image* pTextImage =
				IsPushed() ? m_pPushedTextImage : !IsEnabled() ? m_pDisabledTextImage : IsFocused() ? m_pFocusedTextImage : m_pNormalTextImage;

			if(!pTextImage)
				pTextImage = m_pNormalTextImage;

			DEBUG_VERIFY(pTextImage);

			IPOINT Coords;

			if(m_TextImageAlignment.cx == ALIGNMENT_MIN)
			{
				Coords.x = GetRect().m_Left + m_TextImageOffset.cx;

				TextRect.m_Left = Coords.x + pTextImage->GetWidth();
			}
			else if(m_TextImageAlignment.cx == ALIGNMENT_MID)
			{
				Coords.x = (GetRect().m_Left + GetRect().m_Right - pTextImage->GetWidth()) >> 1;
			}
			else if(m_TextImageAlignment.cx == ALIGNMENT_MAX)
			{
				Coords.x = GetRect().m_Right - pTextImage->GetWidth() - m_TextImageOffset.cx * 2;

				TextRect.m_Right = Coords.x;
			}
			else
			{
				DEBUG_INITIATE_FAILURE;
			}

			if(m_TextImageAlignment.cy == ALIGNMENT_MIN)
				Coords.y = GetRect().m_Top + m_TextImageOffset.cy;
			else if(m_TextImageAlignment.cy == ALIGNMENT_MID)
				Coords.y = (GetRect().m_Top + GetRect().m_Bottom - pTextImage->GetHeight()) >> 1;
			else if(m_TextImageAlignment.cy == ALIGNMENT_MAX)
				Coords.y = GetRect().m_Bottom - pTextImage->GetHeight() - m_TextImageOffset.cy;
			else
				DEBUG_INITIATE_FAILURE;

			if(IsPushed())
				ShiftPoint(Coords, m_PushedTextOffset);

			BlitSDL_Image(*pTextImage, IRECT(0, 0, GetRect().GetWidth(), GetRect().GetHeight()), g_pSDL_FB, Coords);
		}

		DrawExtraImage();		
	}
	else
	{
		if(m_uiBG_Color != UINT_MAX)
			FillSDL_Image(g_pSDL_FB, GetRect(), m_uiBG_Color);
		else
			GetInterface().DrawBG(GetRect());
	}	

	if(IsTextButton())
	{
		const T_SGE_Font* pTextFont =
			IsPushed() ? m_pPushedFont : !IsEnabled() ? m_pDisabledFont : IsFocused() ? m_pFocusedFont : m_pNormalFont;

		if(!pTextFont)
			pTextFont = m_pNormalFont;

		DEBUG_VERIFY(pTextFont);

		UINT uiColor = !IsEnabled() ? m_uiDisabledTextColor : IsPushed() ? m_uiPushedTextColor : IsFocused() ? m_uiFocusedTextColor : m_uiNormalTextColor;
		
		if(uiColor == UINT_MAX)
			uiColor = m_uiNormalTextColor;

		ISIZE PostOffset = IsPushed() ? m_PushedTextOffset : ISIZE(0, 0);
	
		DrawClippedAlignedSDL_Text(	g_pSDL_FB,
									*pTextFont,
									m_Text,
									TextRect,
									m_TextOffset,
									m_TextAlignment,
									uiColor,
									PostOffset);
	}
}

IPOINT TButtonSDL_Control::GetExtraImageCoords() const
{
	DEBUG_VERIFY(IsExtraImageButton());

	const T_SDL_Image* pExtraImage =
		IsPushed() ? m_pPushedExtraImage : !IsEnabled() ? m_pDisabledExtraImage : IsFocused() ? m_pFocusedExtraImage : m_pNormalExtraImage;

	if(!pExtraImage)
		pExtraImage = m_pNormalExtraImage;

	IPOINT Coords;

	if(m_ExtraImageAlignment.cx == ALIGNMENT_MIN)
		Coords.x = GetRect().m_Left + m_ExtraImageOffset.cx;
	else if(m_ExtraImageAlignment.cx == ALIGNMENT_MID)
		Coords.x = (GetRect().m_Left + GetRect().m_Right - pExtraImage->GetWidth()) >> 1;
	else if(m_ExtraImageAlignment.cx == ALIGNMENT_MAX)
		Coords.x = GetRect().m_Right - pExtraImage->GetWidth() - m_ExtraImageOffset.cx;
	else
		DEBUG_INITIATE_FAILURE;

	if(m_ExtraImageAlignment.cy == ALIGNMENT_MIN)
		Coords.y = GetRect().m_Top + m_ExtraImageOffset.cy;
	else if(m_ExtraImageAlignment.cy == ALIGNMENT_MID)
		Coords.y = (GetRect().m_Top + GetRect().m_Bottom - pExtraImage->GetHeight()) >> 1;
	else if(m_ExtraImageAlignment.cy == ALIGNMENT_MAX)
		Coords.y = GetRect().m_Bottom - pExtraImage->GetHeight() - m_ExtraImageOffset.cy;
	else
		DEBUG_INITIATE_FAILURE;

	if(IsPushed())
		ShiftPoint(Coords, m_PushedTextOffset);

	return Coords;
}

bool TButtonSDL_Control::DrawExtraImage() const
{
	DEBUG_VERIFY(IsAttached());

	if(!CanDraw())
		return false;

	if(!IsExtraImageButton())
		return false;

	const T_SDL_Image* pExtraImage =
		IsPushed() ? m_pPushedExtraImage : !IsEnabled() ? m_pDisabledExtraImage : IsFocused() ? m_pFocusedExtraImage : m_pNormalExtraImage;

	if(!pExtraImage)
		pExtraImage = m_pNormalExtraImage;

	DEBUG_VERIFY(pExtraImage);

	BlitSDL_Image(*pExtraImage, IRECT(0, 0, GetRect().GetWidth(), GetRect().GetHeight()), g_pSDL_FB, GetExtraImageCoords());

	return true;
}

size_t TButtonSDL_Control::HandleEvent(T_KSDL_Event Event, size_t szParam)
{
	if(Event == KSDLE_KNOB && szParam == KNOB_RELEASE)
	{
		GetInterface().GetEventSink().OnClick(this);

		Push(false);
	}

	return 0;
}

void TButtonSDL_Control::OnKNOB(size_t szKey)
{
	if(szKey == KNOB_PUSH)
	{
		Push();

		if(GetInterface().GetEventSink().OnPush(this))
			RunSDL_MessageLoop(*this, &TButtonSDL_Control::HandleEvent, GetInterface().GetTimers(), GetInterface().GetEventSink(), m_bPushed);
	}
}

void TButtonSDL_Control::Push(bool bPush)
{
	DEBUG_VERIFY(IsAttached());

	if(m_bPushed == bPush)
		return;

	m_bPushed = bPush;

	Redraw();
}

// Button SDL control loaders
T_SDL_Control::TDefinition*	ButtonSDL_ControlLoader(TInfoNodeConstIterator			Node,
													const T_SDL_ResourceStorage&	Storage,
													const T_SDL_ResourceID_Map&		ID_Map,
													const TTokens&					ValueTokens,
													const TTokens&					ColorTokens,
													const TTokens&					StringTokens)
{
	TPtrHolder<TButtonSDL_Control::TDefinition> pDefinition
		(new TButtonSDL_Control::TDefinition);
	
	pDefinition->m_szID =
		ID_Map[Node->GetParameterValue("ID", "")];

	pDefinition->m_bVisible =
		ReadFromString<bool>(Node->GetParameterValue("Visible", "true"));

	pDefinition->m_bEnabled =
		ReadFromString<bool>(Node->GetParameterValue("Enabled", "true"));

	pDefinition->m_uiBG_Color = ReadRGB(ColorTokens(Node->GetParameterValue("BG_Color", WriteRGB(pDefinition->m_uiBG_Color))));

	if(pDefinition->m_pNormalImage = Storage.GetMultiImage(ID_Map[Node->GetParameterValue("Image", Node->GetParameterValue("NormalImage", ""))]))
	{
		pDefinition->m_pPushedImage		= Storage.GetMultiImage(ID_Map[Node->GetParameterValue("PushedImage",	"")]);		
		pDefinition->m_pDisabledImage	= Storage.GetMultiImage(ID_Map[Node->GetParameterValue("DisabledImage",	"")]);
		pDefinition->m_pFocusedImage	= Storage.GetMultiImage(ID_Map[Node->GetParameterValue("FocusedImage",	"")]);

		if(pDefinition->m_pNormalTextImage = Storage.GetImage(ID_Map[Node->GetParameterValue("TextImage", Node->GetParameterValue("NormalTextImage", ""))]))
		{
			pDefinition->m_pPushedTextImage		= Storage.GetImage(ID_Map[Node->GetParameterValue("PushedTextImage",	"")]);			
			pDefinition->m_pDisabledTextImage	= Storage.GetImage(ID_Map[Node->GetParameterValue("DisabledTextImage",	"")]);
			pDefinition->m_pFocusedTextImage	= Storage.GetImage(ID_Map[Node->GetParameterValue("FocusedTextImage",	"")]);
		}

		if(pDefinition->m_pNormalExtraImage = Storage.GetImage(ID_Map[Node->GetParameterValue("ExtraImage", Node->GetParameterValue("NormalExtraImage", ""))]))
		{
			pDefinition->m_pPushedExtraImage	= Storage.GetImage(ID_Map[Node->GetParameterValue("PushedExtraImage",	"")]);			
			pDefinition->m_pDisabledExtraImage	= Storage.GetImage(ID_Map[Node->GetParameterValue("DisabledExtraImage",	"")]);
			pDefinition->m_pFocusedExtraImage	= Storage.GetImage(ID_Map[Node->GetParameterValue("FocusedExtraImage",	"")]);
		}
	}

	if(pDefinition->m_pNormalFont = Storage.GetFont(ID_Map[Node->GetParameterValue("TextFont", Node->GetParameterValue("NormalTextFont", ""))]))
	{
		pDefinition->m_pPushedFont		= Storage.GetFont(ID_Map[Node->GetParameterValue("PushedTextFont",		"")]);		
		pDefinition->m_pDisabledFont	= Storage.GetFont(ID_Map[Node->GetParameterValue("DisabledTextFont",	"")]);
		pDefinition->m_pFocusedFont		= Storage.GetFont(ID_Map[Node->GetParameterValue("FocusedTextFont",		"")]);
	}

	pDefinition->m_TextImageAlignment =
		ReadFromString<ALSIZE>(Node->GetParameterValue("TextImageAlignment", pDefinition->m_TextImageAlignment));

	pDefinition->m_TextImageOffset =
		ReadFromString<ISIZE>(ValueTokens(Node->GetParameterValue("TextImageOffset", pDefinition->m_TextImageOffset)));

	pDefinition->m_ExtraImageAlignment =
		ReadFromString<ALSIZE>(Node->GetParameterValue("ExtraImageAlignment", pDefinition->m_ExtraImageAlignment));

	pDefinition->m_ExtraImageOffset =
		ReadFromString<ISIZE>(ValueTokens(Node->GetParameterValue("ExtraImageOffset", pDefinition->m_ExtraImageOffset)));

	pDefinition->m_Text =
		StringTokens(Node->GetParameterValue("Text", pDefinition->m_Text));

	pDefinition->m_uiNormalTextColor =
		ReadRGB(ColorTokens(Node->GetParameterValue("TextColor",
													Node->GetParameterValue("NormalTextColor",
																			WriteRGB(pDefinition->m_uiNormalTextColor)))));

	pDefinition->m_uiPushedTextColor =
		ReadRGB(ColorTokens(Node->GetParameterValue("PushedTextColor", WriteRGB(pDefinition->m_uiPushedTextColor))));	

	pDefinition->m_uiDisabledTextColor =
		ReadRGB(ColorTokens(Node->GetParameterValue("DisabledTextColor", WriteRGB(pDefinition->m_uiDisabledTextColor))));

	pDefinition->m_uiFocusedTextColor =
		ReadRGB(ColorTokens(Node->GetParameterValue("FocusedTextColor", WriteRGB(pDefinition->m_uiFocusedTextColor))));

	pDefinition->m_TextAlignment =
		ReadFromString<ALSIZE>(Node->GetParameterValue("Alignment", pDefinition->m_TextAlignment));

	pDefinition->m_TextOffset =
		ReadFromString<ISIZE>(Node->GetParameterValue("TextOffset", pDefinition->m_TextOffset));

	pDefinition->m_PushedTextOffset =
		ReadFromString<ISIZE>(ValueTokens(Node->GetParameterValue("PushedTextOffset", pDefinition->m_PushedTextOffset)));

	pDefinition->m_Rect = ReadFromString<IRECT>(ValueTokens(Node->GetParameterValue("Rect", pDefinition->m_Rect)));

	return pDefinition.Extract();
}

T_SDL_Control::TDefinition*	LabelSDL_ControlLoader(	TInfoNodeConstIterator			Node,
													const T_SDL_ResourceStorage&	Storage,
													const T_SDL_ResourceID_Map&		ID_Map,
													const TTokens&					ValueTokens,
													const TTokens&					ColorTokens,
													const TTokens&					StringTokens)
{
	TPtrHolder<TButtonSDL_Control::TDefinition> pDefinition
		(new TButtonSDL_Control::TDefinition);

	pDefinition->m_szID =
		ID_Map[Node->GetParameterValue("ID", "")];

	pDefinition->m_bVisible =
		ReadFromString<bool>(Node->GetParameterValue("Visible", "true"));

	pDefinition->m_bEnabled = false;

	pDefinition->m_uiBG_Color = ReadRGB(ColorTokens(Node->GetParameterValue("BG_Color", WriteRGB(pDefinition->m_uiBG_Color))));

	KFC_VERIFY(pDefinition->m_pNormalFont = Storage.GetFont(ID_Map[Node->GetParameterValue("Font")]));

	pDefinition->m_Text =
		StringTokens(Node->GetParameterValue("Text", pDefinition->m_Text));

	pDefinition->m_uiNormalTextColor =
		ReadRGB(ColorTokens(Node->GetParameterValue("Color", WriteRGB(pDefinition->m_uiNormalTextColor))));

	pDefinition->m_TextAlignment =
		ReadFromString<ALSIZE>(Node->GetParameterValue("Alignment", pDefinition->m_TextAlignment));

	pDefinition->m_TextOffset =
		ReadFromString<ISIZE>(Node->GetParameterValue("Offset", pDefinition->m_TextOffset));

	if(Node->HasParameter("Rect"))
	{
		pDefinition->m_Rect = ReadFromString<IRECT>(ValueTokens(Node->GetParameterValue("Rect")));
	}
	else if(Node->HasParameter("Coords"))
	{
		pDefinition->m_Rect = RectFromCS(	ReadFromString<IPOINT>(ValueTokens(Node->GetParameterValue("Coords"))),
											GetSDL_TextSize(*pDefinition->m_pNormalFont, pDefinition->m_Text));
	}

	return pDefinition.Extract();
}

T_SDL_Control::TDefinition*	ImageSDL_ControlLoader(	TInfoNodeConstIterator			Node,
													const T_SDL_ResourceStorage&	Storage,
													const T_SDL_ResourceID_Map&		ID_Map,
													const TTokens&					ValueTokens,
													const TTokens&					ColorTokens,
													const TTokens&					StringTokens)
{
	TPtrHolder<TButtonSDL_Control::TDefinition> pDefinition
		(new TButtonSDL_Control::TDefinition);

	pDefinition->m_szID =
		ID_Map[Node->GetParameterValue("ID", "")];

	pDefinition->m_bVisible =
		ReadFromString<bool>(Node->GetParameterValue("Visible", "true"));

	pDefinition->m_bEnabled = false;

	pDefinition->m_uiBG_Color = ReadRGB(ColorTokens(Node->GetParameterValue("BG_Color", WriteRGB(pDefinition->m_uiBG_Color))));

	pDefinition->m_pNormalImage = Storage.GetMultiImage(ID_Map[Node->GetParameterValue("Image", "")]);

	if(Node->HasParameter("Rect"))
	{
		pDefinition->m_Rect = ReadFromString<IRECT>(ValueTokens(Node->GetParameterValue("Rect")));
	}
	else if(Node->HasParameter("Coords"))
	{
		KFC_VERIFY("Coords require single-type multi-image" && (pDefinition->m_pNormalImage && pDefinition->m_pNormalImage->IsSingle()));

		pDefinition->m_Rect = RectFromCS(	ReadFromString<IPOINT>(ValueTokens(Node->GetParameterValue("Coords"))),
											pDefinition->m_pNormalImage->GetSingleSize());
	}

	return pDefinition.Extract();
}

// ------------------------
// Progress barSDL control
// ------------------------

// Progress bar SDL control own definition
TProgressBarSDL_ControlOwnDefinition::TProgressBarSDL_ControlOwnDefinition()
{
	m_uiBG_Color = RGB(  0,   0,   0);
	m_uiFG_Color = RGB(255, 255, 255);
}

// Progress SDL control
TProgressBarSDL_Control::TProgressBarSDL_Control(const TDefinition& Definition) :
	T_SDL_Control(Definition), TProgressBarSDL_ControlOwnDefinition(Definition)
{
	m_Range.Set(0, 1);
	m_dPos = 0.5;
}

void TProgressBarSDL_Control::OnDraw(bool bFocusChange) const
{
	if(bFocusChange)
		return;

	FillSDL_Image(g_pSDL_FB, GetBG_Rect(), m_uiBG_Color, false);
	FillSDL_Image(g_pSDL_FB, GetFG_Rect(), m_uiFG_Color, false);

	UpdateSDL_Image(g_pSDL_FB, GetRect());
}

void TProgressBarSDL_Control::DynamicRedraw(int iOldCoord, int iNewCoord)
{
	if(iNewCoord < iOldCoord)
	{
		if(CanDraw())
		{
			FillSDL_Image(	g_pSDL_FB,
							IRECT(	GetRect().m_Left + iNewCoord,
									GetRect().m_Top,
									GetRect().m_Left + iOldCoord,
									GetRect().m_Bottom),
							m_uiBG_Color,
							true);
		}
	}
	else if(iNewCoord > iOldCoord)
	{
		if(CanDraw())
		{
			FillSDL_Image(	g_pSDL_FB,
							IRECT(	GetRect().m_Left + iOldCoord,
									GetRect().m_Top,
									GetRect().m_Left + iNewCoord,
									GetRect().m_Bottom),
							m_uiFG_Color,
							true);
		}		
	}
}

// Progress bar SDL control loader
T_SDL_Control::TDefinition*	ProgressBarSDL_ControlLoader(	TInfoNodeConstIterator			Node,
															const T_SDL_ResourceStorage&	Storage,
															const T_SDL_ResourceID_Map&		ID_Map,
															const TTokens&					ValueTokens,
															const TTokens&					ColorTokens,
															const TTokens&					StringTokens)
{
	TPtrHolder<TProgressBarSDL_Control::TDefinition> pDefinition
		(new TProgressBarSDL_Control::TDefinition);

	pDefinition->m_szID =
		ID_Map[Node->GetParameterValue("ID", "")];

	pDefinition->m_bVisible =
		ReadFromString<bool>(Node->GetParameterValue("Visible", "true"));

	pDefinition->m_bEnabled = false;

	pDefinition->m_uiBG_Color = ReadRGB(ColorTokens(Node->GetParameterValue("BG_Color", WriteRGB(pDefinition->m_uiBG_Color))));
	pDefinition->m_uiFG_Color = ReadRGB(ColorTokens(Node->GetParameterValue("FG_Color", WriteRGB(pDefinition->m_uiFG_Color))));

	pDefinition->m_Rect = ReadFromString<IRECT>(ValueTokens(Node->GetParameterValue("Rect", pDefinition->m_Rect)));

	return pDefinition.Extract();
}

// -----------------
// List SDL control
// -----------------

// List SDL control own definition
TListSDL_ControlOwnDefinition::TListSDL_ControlOwnDefinition()
{
	m_pHeaderFont	= NULL;
	m_pItemFont		= NULL;

	m_pArrowUpImage = NULL;
	m_pArrowDnImage = NULL;

	m_szTextsEdgeOffset	= 6;
	m_szHeaderSpacing	= 2;

	m_uiHeaderColor			= RGB(0, 0, 0);
	m_uiNormalItemColor		= RGB(0, 0, 0);
	m_uiSelectedItemColor	= RGB(0, 0, 0);
	m_uiNormalBG_Color		= g_SDL_Consts.m_uiHeaderBG_CN_Color;
	m_uiSelectedBG_Color	= g_SDL_Consts.m_uiListSelectionBG_Color;

	m_bHasHeader = true;

	m_bNeedExtFocus = true;
}

// List SDL control
TListSDL_Control::TListSDL_Control(const TDefinition& Definition) :
	T_SDL_Control(Definition), TListSDL_ControlOwnDefinition(Definition)
{
	DEBUG_VERIFY(m_pArrowUpImage);
	DEBUG_VERIFY(m_pArrowDnImage);	

	DEBUG_VERIFY(m_pHeaderFont);
	DEBUG_VERIFY(m_pItemFont);

	#ifdef _DEBUG
		FOR_EACH_ARRAY(m_Columns, i)
			DEBUG_VERIFY(m_Columns[i].m_szWidth >= MIN_SDL_EMBOSS_SIZE + (i == m_Columns.GetLast() ? 0 : m_szHeaderSpacing));
	#endif // _DEBUG

	DEBUG_VERIFY(m_pArrowUpImage->GetWidth() == m_pArrowDnImage->GetWidth());

	DEBUG_VERIFY(m_pArrowUpImage->GetWidth() >= MIN_SDL_EMBOSS_SIZE);

	DEBUG_VERIFY(	m_szItemHeight * m_szNVisibleItems >=
					m_pArrowUpImage->GetHeight() + m_szMinThumbHeight + m_pArrowDnImage->GetHeight() + 4);

	KFC_VERIFY(m_szMinThumbHeight >= MIN_SDL_EMBOSS_SIZE);

	DEBUG_VERIFY(!m_Columns.IsEmpty());

	DEBUG_VERIFY(m_szNVisibleItems > 0);	

	m_szFirstVisibleItem = 0;

	m_szSelectedItem = UINT_MAX;

	size_t szTotalColumnsWidth = 0;

	FOR_EACH_ARRAY(m_Columns, i)
		szTotalColumnsWidth += m_Columns[i].m_szWidth;

	SetRect(RectFromCS(	GetRect().GetTopLeft(),
						ISIZE(	szTotalColumnsWidth + (HasScrolling() ? m_pArrowUpImage->GetWidth() + 2 : 0),
								GetTotalHeaderHeight() + m_szItemHeight * m_szNVisibleItems)));
}

void TListSDL_Control::OnDraw(bool bFocusChange) const
{
	if(bFocusChange)
		return;

	DrawHeader();

	DrawItems();

	DrawScrollElements();
}

void TListSDL_Control::OnKNOB(size_t szKey)
{
	if(szKey == KNOB_PUSH)
	{
		if(!GetInterface().GetEventSink().OnPush(this))
			return;

		EnterSelectMode();

		GetInterface().GetEventSink().OnClick(this);
	}
}

void TListSDL_Control::DrawHeader() const
{
	if(!m_bHasHeader)
		return;

	IRECT Rect = GetRect();

	Rect.m_Bottom = Rect.m_Top + m_szHeaderHeight;

	for(size_t i = 0 ; i < m_Columns.GetN() ; i++)
	{
		Rect.m_Right = Rect.m_Left + m_Columns[i].m_szWidth;

		if(i < m_Columns.GetLast())
		{
			{
				IRECT Rect2 = Rect;
				Rect2.m_Right -= m_szHeaderSpacing;
				DrawRaisedSDL_Rect(g_pSDL_FB, Rect2);
			}

			{
				IRECT Rect2 = Rect;
				Rect2.m_Left = Rect2.m_Right - m_szHeaderSpacing;
				FillSDL_Image(g_pSDL_FB, Rect2, g_SDL_Consts.m_uiHeaderBG_CN_Color);
			}
		}
		else
		{
			DrawRaisedSDL_Rect(g_pSDL_FB, Rect);
		}

		DrawClippedAlignedSDL_Text(	g_pSDL_FB,
									*m_pHeaderFont,
									m_Columns[i].m_Title,
									Rect,
									ISIZE(m_szTextsEdgeOffset, 0),
									ALSIZE(m_Columns[i].m_Alignment, ALIGNMENT_MID),
									m_uiHeaderColor);

		Rect.m_Left = Rect.m_Right;
	}

	FillSDL_Image(	g_pSDL_FB,
					IRECT(	GetRect().m_Left,
							GetRect().m_Top + m_szHeaderHeight,
							GetRightEdge(),
							GetRect().m_Top + m_szHeaderHeight + 1),
					g_SDL_Consts.m_uiHeaderBG_CN_Color);
}

void TListSDL_Control::DrawItem(size_t szIndex) const
{
	DEBUG_VERIFY(szIndex != UINT_MAX);

	if(szIndex < GetNItems() && m_ItemRedrawBlocks[szIndex])
		return;

	if(szIndex < m_szFirstVisibleItem)
		return;

	size_t szRow = szIndex - m_szFirstVisibleItem;

	if(szRow >= m_szNVisibleItems)
		return;

	IRECT Rect = GetRect();

	Rect.m_Right = GetRightEdge();

	if(m_bHasHeader)
		Rect.m_Top += GetTotalHeaderHeight();

	Rect.m_Top += szRow * m_szItemHeight;

	Rect.m_Bottom = Rect.m_Top + m_szItemHeight;

	DrawSunkenSDL_Rect(	g_pSDL_FB,
						IRECT(	GetRect().m_Left,
								GetRect().m_Top + GetTotalHeaderHeight(),
								GetRightEdge(),
								GetRect().m_Bottom),
						&Rect,
						szIndex < GetNItems() && IsItemSelected(szIndex) ?
							m_uiSelectedBG_Color : m_uiNormalBG_Color);

	if(szIndex >= GetNItems())
		return;

	for(size_t i = 0 ; i < m_Columns.GetN() ; i++)
	{
		Rect.m_Right = Rect.m_Left + m_Columns[i].m_szWidth;

		DrawClippedAlignedSDL_Text(	g_pSDL_FB,
									m_ItemFonts[szIndex][i] ? *m_ItemFonts[szIndex][i] : *m_pItemFont,
									m_Items[szIndex][i],
									Rect,
									ISIZE (m_Columns[i].m_szTextOffset, 0),
									ALSIZE(m_Columns[i].m_Alignment, ALIGNMENT_MID),
									IsItemSelected(szIndex) ?
										(m_SelectedItemColors[szIndex][i] == UINT_MAX ?
											m_uiSelectedItemColor : m_SelectedItemColors[szIndex][i]) :
										(m_NormalItemColors[szIndex][i] == UINT_MAX ?
											m_uiNormalItemColor : m_NormalItemColors[szIndex][i]));

		Rect.m_Left = Rect.m_Right;
	}
}

void TListSDL_Control::DrawScrollElements() const
{
	if(!HasScrolling())
		return;

	// Header contour
	if(m_bHasHeader)
	{
		FillSDL_Image(	g_pSDL_FB,
						IRECT(	GetRightEdge(),
								GetRect().m_Top,
								GetRect().m_Right,
								GetRect().m_Top + m_szHeaderHeight + 1),
						g_SDL_Consts.m_uiHeaderBG_CN_Color);
	}

	// Above arrow up
	FillSDL_Image(	g_pSDL_FB,
					IRECT(	GetRightEdge() + 1,
							GetRect().m_Top + GetTotalHeaderHeight(),
							GetRect().m_Right - 1,
							GetRect().m_Top + GetTotalHeaderHeight() + 1),
					g_SDL_Consts.m_uiHeaderBG_SC_Color);

	// Below arrow up
	FillSDL_Image(	g_pSDL_FB,
					IRECT(	GetRightEdge() + 1,
							GetRect().m_Top + GetTotalHeaderHeight() + m_pArrowUpImage->GetHeight() + 1,
							GetRect().m_Right - 1,
							GetRect().m_Top + GetTotalHeaderHeight() + m_pArrowUpImage->GetHeight() + 2),
					g_SDL_Consts.m_uiHeaderBG_SC_Color);

	// Above arrow down
	FillSDL_Image(	g_pSDL_FB,
					IRECT(	GetRightEdge() + 1,
							GetRect().m_Bottom - m_pArrowDnImage->GetHeight() - 2,
							GetRect().m_Right - 1,
							GetRect().m_Bottom - m_pArrowDnImage->GetHeight() - 1),
					g_SDL_Consts.m_uiHeaderBG_SC_Color);

	// Below arrow down
	FillSDL_Image(	g_pSDL_FB,
					IRECT(	GetRightEdge() + 1,
							GetRect().m_Bottom - 1,
							GetRect().m_Right  - 1,
							GetRect().m_Bottom),
					g_SDL_Consts.m_uiHeaderBG_SC_Color);

	// Left vertical stride
	FillSDL_Image(	g_pSDL_FB,
					IRECT(	GetRightEdge(),
							GetRect().m_Top + GetTotalHeaderHeight(),
							GetRightEdge() + 1,
							GetRect().m_Bottom),
					g_SDL_Consts.m_uiHeaderBG_SC_Color);

	// Right vertical stride
	FillSDL_Image(	g_pSDL_FB,
					IRECT(	GetRect().m_Right - 1,
							GetRect().m_Top + GetTotalHeaderHeight(),
							GetRect().m_Right,
							GetRect().m_Bottom),
					g_SDL_Consts.m_uiHeaderBG_SC_Color);

	// Arrow up
	BlitSDL_Image(	*m_pArrowUpImage,
					g_pSDL_FB,
					IPOINT(	GetRightEdge() + 1,
							GetRect().m_Top + GetTotalHeaderHeight() + 1));

	// Arrow down
	BlitSDL_Image(	*m_pArrowDnImage,
					g_pSDL_FB,
					IPOINT(	GetRightEdge() + 1,
							GetRect().m_Bottom - m_pArrowDnImage->GetHeight() - 1));

	IRECT ThumbRect = GetThumbRect();

	// Thumb BG
	DrawThumbBG(ISEGMENT(GetRect().m_Top, ThumbRect.m_Top));
	DrawThumbBG(ISEGMENT(ThumbRect.m_Bottom, GetRect().m_Bottom));

	// Thumb
	DrawThumb(ThumbRect);
}

void TListSDL_Control::DrawThumbBG(const ISEGMENT& YRange) const
{
	IRECT IntRect(	GetRect().m_Left,
					YRange.m_First,
					GetRect().m_Right,
					YRange.m_Last);

	DrawSunkenSDL_Rect(	g_pSDL_FB,
						IRECT(	GetRightEdge() + 1,
								GetRect().m_Top + GetTotalHeaderHeight() + m_pArrowUpImage->GetHeight() + 2,
								GetRect().m_Right - 1,
								GetRect().m_Bottom - m_pArrowDnImage->GetHeight() - 2),
						&IntRect,
						g_SDL_Consts.m_uiHeaderBG_SC_Color);
}

void TListSDL_Control::DrawThumb(const IRECT& ThumbRect) const
{
	DrawRaisedSDL_Rect(g_pSDL_FB, ThumbRect, NULL, g_SDL_Consts.m_uiHeaderBG_SC_Color);
}

void TListSDL_Control::RedrawThumb(const IRECT& OldThumbRect, const IRECT& NewThumbRect) const
{
	DEBUG_VERIFY(HasScrolling());

	DrawThumb(NewThumbRect);

	ISEGMENT OldYRange(OldThumbRect.m_Top, OldThumbRect.m_Bottom);
	ISEGMENT NewYRange(NewThumbRect.m_Top, NewThumbRect.m_Bottom);

	ISEGMENT YRanges[2];
	size_t szN = SubtractSegment(OldYRange, NewYRange, YRanges);

	for(size_t i = 0 ; i <szN ; i++)
		DrawThumbBG(YRanges[i]);
}

size_t TListSDL_Control::HandleEvent(T_KSDL_Event Event, size_t szParam)
{
	if(Event == KSDLE_KNOB)
	{
		if(szParam == KNOB_LEFT || szParam == KNOB_PUSHED_LEFT)
		{
			if(m_bSelecting)
			{
				if(GetSelectedItem() == UINT_MAX)
				{
					if(!IsEmpty())
						SelectItem(0);
				}
				else if(GetSelectedItem() - 1 != UINT_MAX)
				{
					SelectItem(GetSelectedItem() - 1);
				}
			}
		}
		else if(szParam == KNOB_RIGHT || szParam == KNOB_PUSHED_RIGHT)
		{
			if(m_bSelecting)
			{
				if(GetSelectedItem() == UINT_MAX)
				{
					if(!IsEmpty())
						SelectItem(0);
				}
				else if(GetSelectedItem() + 1 < GetNItems())
				{
					SelectItem(GetSelectedItem() + 1);
				}
			}
		}
		if(szParam == KNOB_PUSH)
		{
//			if(m_bSelecting)
				m_bSelecting = false;
		}
		else if(szParam == KNOB_RELEASE)
		{
			if(!m_bSelecting)
				m_bRunning = false;
		}

		return 0;
	}

	return 0;
}

IRECT TListSDL_Control::GetThumbRect() const
{
	DEBUG_VERIFY(HasScrolling());

	size_t szAreaHeight = GetRect().GetHeight() - GetTotalHeaderHeight() -
		m_pArrowUpImage->GetHeight() - m_pArrowDnImage->GetHeight() - 4;

	size_t szThumbHeight = GetNVisibleItems() * szAreaHeight / GetNItems();

	LimitValue(szThumbHeight, SZSEGMENT(m_szMinThumbHeight, szAreaHeight));

	// [0;n-nvis] -> [0;area-thumb]
	int iPos = m_szFirstVisibleItem * (szAreaHeight - szThumbHeight) / (GetNItems() - GetNVisibleItems());

	int iTop = GetRect().m_Top + GetTotalHeaderHeight() + m_pArrowUpImage->GetHeight() + 2 + iPos;

	int iBottom = iTop + szThumbHeight;

	return IRECT(GetRightEdge() + 1, iTop, GetRect().m_Right - 1, iBottom);
}

void TListSDL_Control::Clear()
{
	if(IsEmpty())
		return;

	bool bHadScrolling = HasScrolling();

	m_Items.				Clear();
	m_NormalItemColors.		Clear();
	m_SelectedItemColors.	Clear();
	m_ItemFonts.			Clear();
	m_ItemRedrawBlocks.		Clear();

	m_szFirstVisibleItem = 0;

	m_szSelectedItem = UINT_MAX;	

	{
		T_SDL_ControlRedrawBlocker Blocker0(this);

		if(bHadScrolling)
		{
			IRECT Rect = GetRect();
			Rect.m_Right -= m_pArrowUpImage->GetWidth() + 2;
			SetRect(Rect);
		}

		Redraw();
	}
}

void TListSDL_Control::SelectItem(size_t szIndex, bool bEnsureVisible)
{
	DEBUG_VERIFY(szIndex == UINT_MAX || szIndex < GetNItems());

	size_t szOldSelectedItem = m_szSelectedItem;

	m_szSelectedItem = szIndex;

	{
		if(szOldSelectedItem != UINT_MAX)
			m_ItemRedrawBlocks[szOldSelectedItem]++;

		if(m_szSelectedItem != UINT_MAX)
			m_ItemRedrawBlocks[m_szSelectedItem]++;

		if(szOldSelectedItem != m_szSelectedItem && GetThreadSelectSDL_ListCallback())
			GetThreadSelectSDL_ListCallback()(szOldSelectedItem, m_szSelectedItem, GetThreadSelectSDL_ListItemCallbackParam());

		if(m_szSelectedItem != UINT_MAX)
			m_ItemRedrawBlocks[m_szSelectedItem]--;

		if(szOldSelectedItem != UINT_MAX)
			m_ItemRedrawBlocks[szOldSelectedItem]--;
	}

	bool bScrolled = bEnsureVisible && EnsureVisible(szIndex);

	if(szOldSelectedItem != UINT_MAX && CanDraw() && !bScrolled)
		DrawItem(szOldSelectedItem);

	if(m_szSelectedItem != UINT_MAX && CanDraw() && !bScrolled)
		DrawItem(m_szSelectedItem);
}

bool TListSDL_Control::EnsureVisible(size_t szIndex)
{
	DEBUG_VERIFY(szIndex < GetNItems());

	if(szIndex < GetFirstVisibleItem())
	{
		SetFirstVisibleItem(szIndex);
		return true;
	}

	if(szIndex >= GetFirstVisibleItem() + m_szNVisibleItems)
	{
		SetFirstVisibleItem(szIndex - m_szNVisibleItems + 1);
		return true;
	}

	return false;
}

void TListSDL_Control::SetItem(size_t szIndex, size_t szColumn, LPCTSTR pText)
{
	DEBUG_VERIFY(szIndex < m_Items.GetN());
	DEBUG_VERIFY(szColumn < m_Columns.GetN());

	m_Items[szIndex][szColumn] = pText;

	if(CanDraw())
		DrawItem(szIndex);
}

void TListSDL_Control::SetItemColor(size_t szIndex, size_t szColumn, UINT32 uiNormalColor, UINT uiSelectedColor)
{
	DEBUG_VERIFY(szIndex < m_Items.GetN());
	DEBUG_VERIFY(szColumn < m_Columns.GetN() || szColumn == UINT_MAX);

	if(szColumn == UINT_MAX)
	{
		for(size_t i = 0 ; i < GetNColumns() ; i++)
		{
			m_NormalItemColors	[szIndex][i] = uiNormalColor;
			m_SelectedItemColors[szIndex][i] = uiSelectedColor;
		}
	}
	else
	{
		m_NormalItemColors	[szIndex][szColumn] = uiNormalColor;
		m_SelectedItemColors[szIndex][szColumn] = uiSelectedColor;
	}

	if(CanDraw())
		DrawItem(szIndex);
}

void TListSDL_Control::SetItemFont(size_t szIndex, size_t szColumn, const T_SGE_Font* pFont)
{
	DEBUG_VERIFY(szIndex < m_Items.GetN());
	DEBUG_VERIFY(szColumn < m_Columns.GetN());

	m_ItemFonts[szIndex][szColumn] = pFont;

	if(CanDraw())
		DrawItem(szIndex);
}

size_t TListSDL_Control::AddItem(bool bRedraw)
{
	bool bOldHasScrolling = HasScrolling();

	IRECT OldThumbRect;

	if(bOldHasScrolling)
		OldThumbRect = GetThumbRect();

	m_Items.Add().SetN(m_Columns.GetN());

	memset(&m_NormalItemColors.  Add().SetN(m_Columns.GetN()), 0xFF, m_Columns.GetN() * sizeof(UINT32));
	memset(&m_SelectedItemColors.Add().SetN(m_Columns.GetN()), 0xFF, m_Columns.GetN() * sizeof(UINT32));
	memset(&m_ItemFonts.         Add().SetN(m_Columns.GetN()), 0,    m_Columns.GetN() * sizeof(const T_SGE_Font*));

	m_ItemRedrawBlocks.Add() = 0;

	size_t szIndex = m_Items.GetLast();

	if(!bOldHasScrolling && HasScrolling())		
	{
		IRECT Rect = GetRect();
		Rect.m_Right += m_pArrowUpImage->GetWidth() + 2;
		SetRect(Rect);
	}
	else if(bRedraw && CanDraw())
	{
		DrawItem(szIndex);
		
		if(HasScrolling())
		{
			DEBUG_VERIFY(bOldHasScrolling);
			RedrawThumb(OldThumbRect, GetThumbRect());
		}
	}

	return szIndex;
}

size_t TListSDL_Control::SetFirstVisibleItem(size_t szIndex)
{
	if(!HasScrolling())
		return 0;

	UpdateMin(szIndex, GetNItems() - GetNVisibleItems());

	if(m_szFirstVisibleItem == szIndex)
		return m_szFirstVisibleItem;

	IRECT OldThumbRect = GetThumbRect();

	m_szFirstVisibleItem = szIndex;

	if(CanDraw())
	{
		DrawItems();

		RedrawThumb(OldThumbRect, GetThumbRect());
	}

	return m_szFirstVisibleItem;
}

void TListSDL_Control::EnterSelectMode()
{
	DEBUG_VERIFY(IsAttached());

	DEBUG_VERIFY(IsVisible());

	if(GetSelectedItem() == UINT_MAX && !IsEmpty())
		SelectItem(0);

	m_bSelecting = true;

	m_bRunning = true;

	RunSDL_MessageLoop(*this, &TListSDL_Control::HandleEvent, GetInterface().GetTimers(), GetInterface().GetEventSink(), m_bRunning);
}

// List SDL control loaders
T_SDL_Control::TDefinition*	ListSDL_ControlLoader(	TInfoNodeConstIterator			Node,
													const T_SDL_ResourceStorage&	Storage,
													const T_SDL_ResourceID_Map&		ID_Map,
													const TTokens&					ValueTokens,
													const TTokens&					ColorTokens,
													const TTokens&					StringTokens)
{
	TPtrHolder<TListSDL_Control::TDefinition> pDefinition(new TListSDL_Control::TDefinition);

	pDefinition->m_szID =
		ID_Map[Node->GetParameterValue("ID", "")];

	pDefinition->m_bVisible =
		ReadFromString<bool>(Node->GetParameterValue("Visible", "true"));

	pDefinition->m_bEnabled =
		ReadFromString<bool>(Node->GetParameterValue("Enabled", "true"));

	KFC_VERIFY(pDefinition->m_pArrowUpImage =
		Storage.GetImage(ID_Map[Node->GetParameterValue("ArrowUpImage")]));

	KFC_VERIFY(pDefinition->m_pArrowDnImage =
		Storage.GetImage(ID_Map[Node->GetParameterValue("ArrowDnImage")]));

	KFC_VERIFY(pDefinition->m_pArrowUpImage->GetWidth() == pDefinition->m_pArrowDnImage->GetWidth());
	KFC_VERIFY(pDefinition->m_pArrowUpImage->GetWidth() >= MIN_SDL_EMBOSS_SIZE);

	KFC_VERIFY(pDefinition->m_pHeaderFont =
		Storage.GetFont(ID_Map[Node->GetParameterValue("HeaderFont")]));

	KFC_VERIFY(pDefinition->m_pItemFont =
		Storage.GetFont(ID_Map[Node->GetParameterValue("ItemFont")]));

	pDefinition->m_uiHeaderColor =
		ReadRGB(ColorTokens(Node->GetParameterValue("HeaderColor", WriteRGB(pDefinition->m_uiHeaderColor))));
	
	pDefinition->m_uiNormalItemColor =
		ReadRGB(ColorTokens(Node->GetParameterValue("NormalItemColor", WriteRGB(pDefinition->m_uiNormalItemColor))));

	pDefinition->m_uiSelectedItemColor =
		ReadRGB(ColorTokens(Node->GetParameterValue("SelectedItemColor", WriteRGB(pDefinition->m_uiSelectedItemColor))));

	pDefinition->m_uiNormalBG_Color =
		ReadRGB(ColorTokens(Node->GetParameterValue("NormalBG_Color", WriteRGB(pDefinition->m_uiNormalBG_Color))));

	pDefinition->m_uiSelectedBG_Color =
		ReadRGB(ColorTokens(Node->GetParameterValue("SelectedBG_Color", WriteRGB(pDefinition->m_uiSelectedBG_Color))));

	pDefinition->m_szItemHeight = ReadFromString<size_t>(ValueTokens(Node->GetParameterValue("ItemHeight")));
	KFC_VERIFY(pDefinition->m_szItemHeight > 0);

	pDefinition->m_szTextsEdgeOffset =
		ReadFromString<size_t>(ValueTokens(Node->GetParameterValue("TextsEdgeOffset", pDefinition->m_szTextsEdgeOffset)));

	pDefinition->m_szHeaderSpacing =
		ReadFromString<size_t>(ValueTokens(Node->GetParameterValue("HeaderSpacing", pDefinition->m_szHeaderSpacing)));

	pDefinition->m_szNVisibleItems = ReadFromString<size_t>(ValueTokens(Node->GetParameterValue("NVisibleItems")));
	KFC_VERIFY(pDefinition->m_szNVisibleItems > 0);

	pDefinition->m_Columns.Clear();

	FOR_EACH_LIST(TStructuredInfo::GetNode(Node, "Columns")->m_Parameters, TInfoParameterConstIterator, PIter)
	{
		KStrings Elements(PIter->m_Value, "|", false);

		if(Elements.GetN() < 1 || Elements.GetN() > 3)
			INITIATE_DEFINED_FAILURE("Invalid SDL list control column format.");

		TListSDL_Control::TDefinition::TColumn& Column = pDefinition->m_Columns.Add();

		Column.m_Title = StringTokens(PIter->m_Name);

		Column.m_szWidth = ReadFromString<size_t>(ValueTokens(Elements[0]));

		if(1 < Elements.GetN())
			Column.m_Alignment = ReadFromString<TAlignment>(Elements[1]);
		else
			Column.m_Alignment = ALIGNMENT_MIN;

		if(2 < Elements.GetN())
			Column.m_szTextOffset = ReadFromString<size_t>(ValueTokens(Elements[2]));
		else
			Column.m_szTextOffset = pDefinition->m_szTextsEdgeOffset;
	}

	if(pDefinition->m_Columns.IsEmpty())
		INITIATE_DEFINED_FAILURE("SDL list control must have at least one column.");

	FOR_EACH_ARRAY(pDefinition->m_Columns, i)
	{
		KFC_VERIFY(	pDefinition->m_Columns[i].m_szWidth >=
					MIN_SDL_EMBOSS_SIZE + (i == pDefinition->m_Columns.GetLast() ? 0 : pDefinition->m_szHeaderSpacing));
	}

	pDefinition->m_bHasHeader = !Node->HasParameter("NoHeader");

	pDefinition->m_bNeedExtFocus = !Node->HasParameter("NoExtFocus");

	if(pDefinition->m_bHasHeader)
		pDefinition->m_szHeaderHeight = ReadFromString<size_t>(ValueTokens(Node->GetParameterValue("HeaderHeight")));
	else
		pDefinition->m_szHeaderHeight = 0; // safety

	pDefinition->m_szMinThumbHeight = ReadFromString<size_t>(ValueTokens(Node->GetParameterValue("MinThumbHeight")));
	KFC_VERIFY(pDefinition->m_szMinThumbHeight >= MIN_SDL_EMBOSS_SIZE);	

	KFC_VERIFY(	pDefinition->m_szItemHeight * pDefinition->m_szNVisibleItems >=
				pDefinition->m_pArrowUpImage->GetHeight() + pDefinition->m_szMinThumbHeight + pDefinition->m_pArrowDnImage->GetHeight() + 4);

	pDefinition->m_Rect = RectFromCS(ReadFromString<IPOINT>(ValueTokens(Node->GetParameterValue("Coords"))), ISIZE(0, 0));

	return pDefinition.Extract();
}
